// Copyright 2022 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/clipboard/ohos/clipboard_ohos.h"
#include "ui/base/clipboard/ohos/clipboard_ohos_read_data.h"
#include "ui/base/clipboard/ohos/clip_board_image_data_adapter_impl.h"

#include "base/check_op.h"
#include "base/command_line.h"
#include "base/containers/contains.h"
#include "base/datashare_uri_utils.h"
#include "base/feature_list.h"
#include "base/files/file_util.h"
#include "base/memory/ptr_util.h"
#include "base/no_destructor.h"
#include "base/notreached.h"
#include "base/pickle.h"
#include "base/strings/utf_string_conversions.h"
#include "base/synchronization/lock.h"
#include "base/task/thread_pool.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/content_switches.h"
#include "ohos_glue/base/include/ark_web_errno.h"
#include "skia/ext/skia_utils_base.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkData.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkStream.h"
#include "ui/base/clipboard/clipboard_constants.h"
#include "ui/base/clipboard/clipboard_data.h"
#include "ui/base/clipboard/clipboard_format_type.h"
#include "ui/base/clipboard/clipboard_metrics.h"
#include "ui/base/clipboard/clipboard_monitor.h"
#include "ui/base/clipboard/custom_data_helper.h"
#include "ui/base/data_transfer_policy/data_transfer_endpoint.h"
#include "ui/base/data_transfer_policy/data_transfer_policy_controller.h"
#include "ui/gfx/color_space.h"

#include "base/logging.h"
#include "ohos_adapter_helper.h"
#include "ohos_resource_adapter.h"

#include <map>
#include <set>
#include <unordered_map>

#include "third_party/icu/source/i18n/unicode/regex.h"
#include "url/gurl.h"

using namespace OHOS::NWeb;

namespace ui {
namespace {

using InstanceRegistry = std::set<const ClipboardOHOS*, std::less<>>;
InstanceRegistry* GetInstanceRegistry() {
  static base::NoDestructor<InstanceRegistry> registry;
  return registry.get();
}

base::Lock& GetInstanceRegistryLock() {
  static base::NoDestructor<base::Lock> registry_lock;
  return *registry_lock;
}

void RegisterInstance(const ClipboardOHOS* clipboard) {
  base::AutoLock lock(GetInstanceRegistryLock());
  GetInstanceRegistry()->insert(clipboard);
}

void UnregisterInstance(const ClipboardOHOS* clipboard) {
  base::AutoLock lock(GetInstanceRegistryLock());
  GetInstanceRegistry()->erase(clipboard);
}

bool IsRegisteredInstance(const Clipboard* clipboard) {
  base::AutoLock lock(GetInstanceRegistryLock());
  return base::Contains(*GetInstanceRegistry(), clipboard);
}

ClipBoardImageAlphaType ImageToClipboardAlphaType(SkAlphaType alpha_type) {
  switch (alpha_type) {
    case kUnknown_SkAlphaType:
      return ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
    case kOpaque_SkAlphaType:
      return ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE;
    case kPremul_SkAlphaType:
      return ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED;
    default:
      return ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
  }
}

ClipBoardImageColorType ImageToClipboardColorType(SkColorType color_type) {
  switch (color_type) {
    case kRGBA_8888_SkColorType:
      return ClipBoardImageColorType::COLOR_TYPE_RGBA_8888;
    case kBGRA_8888_SkColorType:
      return ClipBoardImageColorType::COLOR_TYPE_BGRA_8888;
    default:
      return ClipBoardImageColorType::COLOR_TYPE_UNKNOWN;
  }
}

void ReadCustomDataIntoMap(
    const void* data,
    size_t data_length,
    std::unordered_map<std::string, std::string>* result) {
  base::Pickle pickle(reinterpret_cast<const char*>(data), data_length);
  base::PickleIterator iter(pickle);

  uint32_t size = 0;
  if (!iter.ReadUInt32(&size))
    return;

  for (uint32_t i = 0; i < size; ++i) {
    std::string type;
    if (!iter.ReadString(&type)) {
      // Data is corrupt, return an empty map.
      result->clear();
      return;
    }
    auto insert_result = result->insert({type, std::string()});
    if (!iter.ReadString(&insert_result.first->second)) {
      // Data is corrupt, return an empty map.
      result->clear();
      return;
    }
  }
}

void WriteCustomDataToPickle(
    const std::unordered_map<std::string, std::string>& data,
    base::Pickle* pickle) {
  pickle->WriteUInt32(data.size());
  for (const auto& it : data) {
    pickle->WriteString(it.first);
    pickle->WriteString(it.second);
  }
}
}  // namespace

Clipboard* Clipboard::Create() {
  return new ClipboardOHOS;
}

class ClipboardOHOSInternal {
 public:
  enum class ClipboardState {
    kOutOfDate,
    kUpToDate,
  };

  class PasteboardObserverOhos : public PasteboardObserverAdapter {
   public:
    PasteboardObserverOhos() = default;
    void OnPasteboardChanged() override {
      if (clipboard_internal_) {
        LOG(INFO) << "Inform Clipboard Changed";
        clipboard_internal_->SetClipboardState(ClipboardState::kOutOfDate);
      }
    }

    void SetClipboardInternal(ClipboardOHOSInternal* clipboard_internal) {
      clipboard_internal_ = clipboard_internal;
    }

   private:
    ClipboardOHOSInternal* clipboard_internal_ = nullptr;
  };

  ClipboardOHOSInternal() {
    observer_ = std::make_shared<PasteboardObserverOhos>();
    observer_id_ = OhosAdapterHelper::GetInstance()
        .GetPasteBoard()
        .AddPasteboardChangedObserver(observer_);
    observer_->SetClipboardInternal(this);
    std::string hapPath =
        base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
            switches::kOhosHapPath);
    resource_adapter_ =
        OHOS::NWeb::OhosAdapterHelper::GetInstance().GetResourceAdapter(
            hapPath);
  }

  ~ClipboardOHOSInternal() {
    OhosAdapterHelper::GetInstance()
        .GetPasteBoard()
        .RemovePasteboardChangedObserver(observer_id_);
  }

  void Clear() {
    sequence_number_ = ClipboardSequenceNumberToken();
    data_.reset();
    OhosAdapterHelper::GetInstance().GetPasteBoard().Clear();
  }

  const ClipboardSequenceNumberToken& sequence_number() const {
    return sequence_number_;
  }
  // Returns the current clipboard data, which may be nullptr if nothing has
  // been written since the last Clear().
  const ClipboardData* GetData() const { return data_.get(); }

  // Returns true if the data on top of the clipboard stack has format |format|
  // or another format that can be converted to |format|.
  bool IsFormatAvailable(ClipboardInternalFormat format) {
    if (format == ClipboardInternalFormat::kText) {
      return HasFormatInMisc(ClipboardInternalFormat::kText) ||
             HasFormatInMisc(ClipboardInternalFormat::kBookmark);
    }
    return HasFormatInMisc(format);
  }

  void ReadAvailableCustomDataTypes(std::vector<std::u16string>* types) const {
    std::string data;
    ReadCustomDataFromReadData(kMimeTypeWebCustomData, &data);
    if (!data.empty()) {
      ReadCustomDataTypes(data.data(), data.size(), types);
    }
  }

  void SetClipboardState(ClipboardState state) {
    if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
      content::GetUIThreadTaskRunner({})->PostTask(
          FROM_HERE, base::BindOnce(&ClipboardOHOSInternal::SetClipboardState,
                                    base::Unretained(this), state));
    } else {
      state_ = state;
    }
  }

  void SetOutOfDateAfterRead() {
    if (is_data_guard_enabled_) {
      return;
    }
    LOG(INFO) << "set out of data after read";
    state_ = ClipboardState::kOutOfDate;
  }

  void OnClipboardDataGuard(bool status) {
    LOG(INFO) << "set clipboard data guard=" << status;
    is_data_guard_enabled_ = status;
    if (!is_data_guard_enabled_) {
      SetOutOfDateAfterRead();
    }
  }

  void UpdateClipboardData() {
    if (state_ == ClipboardState::kUpToDate) {
      LOG(DEBUG) << "No need to update Clipboard";
      return;
    }
    LOG(INFO) << "Update clipboard data, state=" << static_cast<int>(state_);

    read_data_ = nullptr;
    PasteRecordVector record_vector;
    if (OhosAdapterHelper::GetInstance().GetPasteBoard().GetPasteData(
            record_vector)) {
      // Notice: Because pasteboard observer dont notify cross device.
      // So now we always get data from system clipboard instead of cache data.
      state_ = ClipboardState::kUpToDate;
      ClipboardOhosReadData::SetConvertHtmlCallback(convert_html_callback_);
      read_data_ = std::make_shared<ClipboardOhosReadData>(record_vector);
      return;
    }
    LOG(ERROR) << "UpdateClipboardData Failed";
  }

  // Reads text from the ClipboardData.
  void ReadText(std::u16string* result) {
    UpdateClipboardData();
    SetOutOfDateAfterRead();
    if (!read_data_) {
      return;
    }

    std::shared_ptr<std::string> text = read_data_->ReadText();
    if (text) {
      std::string data = *text;
      *result = base::UTF8ToUTF16(data);
    }
  }

  // Reads HTML from the ClipboardData.
  void ReadHTML(std::u16string* markup,
                std::string* src_url,
                uint32_t* fragment_start,
                uint32_t* fragment_end) {
    markup->clear();
    if (src_url) {
      src_url->clear();
    }
    *fragment_start = 0;
    *fragment_end = 0;

    UpdateClipboardData();
    SetOutOfDateAfterRead();
    if (!read_data_) {
      return;
    }
    std::string data;
    std::shared_ptr<std::string> html = read_data_->ReadHtml();
    std::shared_ptr<std::string> text = read_data_->ReadText();
    if (html) {
      data = *html;
      *markup = base::UTF8ToUTF16(data);
      *fragment_start = 0;
      *fragment_end = static_cast<uint32_t>(markup->length());
      return;
    } else if (text) {
      data = "<span>" + *text + "</span>";
      *markup = base::UTF8ToUTF16(data);
      *fragment_start = 0;
      *fragment_end = static_cast<uint32_t>(markup->length());
    }
  }

  SkAlphaType AlphaTypeToSkAlphaType(
      const std::shared_ptr<ClipBoardImageDataAdapter> imgData) const {
    if (!imgData) {
      return SkAlphaType::kUnknown_SkAlphaType;
    }

    switch (imgData->GetAlphaType()) {
      case ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN:
        return SkAlphaType::kUnknown_SkAlphaType;
      case ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE:
        return SkAlphaType::kOpaque_SkAlphaType;
      case ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED:
        return SkAlphaType::kPremul_SkAlphaType;
      case ClipBoardImageAlphaType::ALPHA_TYPE_POSTMULTIPLIED:
        return SkAlphaType::kUnpremul_SkAlphaType;
      default:
        return SkAlphaType::kUnknown_SkAlphaType;
    }
  }

  SkColorType PixelFormatToSkColorType(
      const std::shared_ptr<ClipBoardImageDataAdapter> imgData) const {
    if (!imgData) {
      return SkColorType::kUnknown_SkColorType;
    }

    switch (imgData->GetColorType()) {
      case ClipBoardImageColorType::COLOR_TYPE_RGBA_8888:
        return SkColorType::kRGBA_8888_SkColorType;
      case ClipBoardImageColorType::COLOR_TYPE_BGRA_8888:
        return SkColorType::kBGRA_8888_SkColorType;
      default:
        return SkColorType::kUnknown_SkColorType;
    }
  }

  SkImageInfo MakeSkImageInfoFromPixelMap(
      const std::shared_ptr<ClipBoardImageDataAdapter> imgData) const {
    SkColorType colorType = PixelFormatToSkColorType(imgData);
    SkAlphaType alphaType = AlphaTypeToSkAlphaType(imgData);
    sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();
    return SkImageInfo::Make(imgData->GetWidth(), imgData->GetHeight(),
                             colorType, alphaType, colorSpace);
  }

  void DidGetPng(Clipboard::ReadPngCallback callback,
      std::vector<uint8_t> result) {
    // GetPngData attempts to read from the Java Clipboard, which sometimes is
    // not available (ex. the app is not in focus, such as in unit tests).
    if (!result.empty()) {
      std::move(callback).Run(std::move(result));
      return;
    }
    std::move(callback).Run(std::vector<uint8_t>());
  }

  // Reads image from the ClipboardData.
  void ReadPng(Clipboard::ReadPngCallback callback) {
    if (!HasFormatInMisc(ClipboardInternalFormat::kPng)) {
      LOG(ERROR) << "no bitMap format in pasteboard";
      std::move(callback).Run(std::vector<uint8_t>());
      SetOutOfDateAfterRead();
      return;
    }
    SetOutOfDateAfterRead();

    if (!read_data_) {
      LOG(ERROR) << "read_data is null";
      std::move(callback).Run(std::vector<uint8_t>());
      return;
    }
    SkBitmap img;
    PasteRecordVector record_vector = read_data_->GetPasteRecordVector();
    for (const auto& r : record_vector) {
      if (ReadPngRecordInner(r, img)) {
        base::ThreadPool::PostTaskAndReplyWithResult(
            FROM_HERE, {base::MayBlock(), base::TaskPriority::USER_BLOCKING},
            base::BindOnce(&ClipboardData::EncodeBitmapData, std::move(img)),
            base::BindOnce(&ClipboardOHOSInternal::DidGetPng, base::Unretained(this),
                          std::move(callback)));
        return;
      }
    }
    LOG(ERROR) << "get image from pasteboard failed";
    std::move(callback).Run(std::vector<uint8_t>());
  }

  // Reads data of type |type| from the ClipboardOhosReadData.
  void ReadCustomData(const std::u16string& type, std::u16string* result) {
    if (!result) {
      return;
    }
    result->clear();

    UpdateClipboardData();
    SetOutOfDateAfterRead();
    std::string data;
    ReadCustomDataFromReadData(kMimeTypeWebCustomData, &data);
    if (!data.empty()) {
      ReadCustomDataForType(data.data(), data.size(), type, result);
      if (!result->empty()) {
        return;
      }
    }
    LOG(INFO) << "no specified custom data in clipbaord";
  }

  void ReadData(const std::string& type, std::string* result) {
    if (!result) {
      return;
    }
    result->clear();

    UpdateClipboardData();
    SetOutOfDateAfterRead();
    ReadCustomDataFromReadData(type, result);
  }

  // Writes |data| to the ClipboardData and returns the previous data.
  std::unique_ptr<ClipboardData> WriteData(
      std::unique_ptr<ClipboardData> data) {
    DCHECK(data);
    std::unique_ptr<ClipboardData> previous_data = std::move(data_);
    data_ = std::move(data);
    const ClipboardData* currentData = GetData();
    if (!currentData) {
      return nullptr;
    }
    PasteRecordVector result_vector;
#if defined(OHOS_CLIPBOARD)
    CopyOptionMode copy_option = currentData->copy_option();
#endif // defined(OHOS_CLIPBOARD)
    std::shared_ptr<PasteDataRecordAdapter> record =
        PasteDataRecordAdapter::NewRecord("text/html");
    bool is_has_html = HasFormat(ClipboardInternalFormat::kHtml);
    bool is_has_text = HasFormat(ClipboardInternalFormat::kText);
    bool is_has_png = HasFormat(ClipboardInternalFormat::kPng);
    if (is_has_html) {
      std::shared_ptr<std::string> html =
          std::make_shared<std::string>(currentData->markup_data());
      bool htmlTextResult = record->SetHtmlTextV2(html);
      if (ArkWebGetErrno() != ArkWebInterfaceResult::RESULT_OK) {
        htmlTextResult = record->SetHtmlText(html);
      }
      if (htmlTextResult) {
        LOG(INFO) << "set html to record success";
      } else {
        LOG(ERROR) << "set html to record failed";
      }
    } else if (is_has_text && !is_has_png) {
      LOG(INFO) << "set text when no html and no png";
      record = PasteDataRecordAdapter::NewRecord("text/plain");
    }

    if (is_has_text) {
      std::shared_ptr<std::string> text =
          std::make_shared<std::string>(currentData->text());
      bool plainTextResult = record->SetPlainTextV2(text);
      if (ArkWebGetErrno() != ArkWebInterfaceResult::RESULT_OK) {
        plainTextResult = record->SetPlainText(text);
      }
      if (plainTextResult) {
        LOG(INFO) << "set text to record success";
      } else {
        LOG(ERROR) << "set text to record failed";
      }
    }

    if (is_has_png) {
      auto bitmap = currentData->GetBitmapIfPngNotEncoded();
      if (bitmap.has_value()) {
        auto bitmap_record = WriteBitmapToClipboard(bitmap.value());
        if (record->SetImgData(bitmap_record)) {
          LOG(INFO) << "set image to record success";
        } else {
          LOG(ERROR) << "set image to record failed";
        }
      }
    }

    if (HasFormat(ClipboardInternalFormat::kCustom)) {
      base::Pickle pickle;
      WriteCustomDataToPickle(currentData->GetCustomDataMap(), &pickle);
      auto custom_data_vector =
      std::vector<uint8_t>(pickle.data(), pickle.data() + pickle.size());
      OHOS::NWeb::PasteCustomData custom_data_map = {
          {kMimeTypeOHOSCustomData, custom_data_vector}};
      bool customDataResult = record->SetCustomDataV2(custom_data_map);
      if (ArkWebGetErrno() != ArkWebInterfaceResult::RESULT_OK) {
        customDataResult = record->SetCustomData(custom_data_map);
      }
      if (customDataResult) {
        LOG(INFO) << "set custom data to record success";
      } else {
        LOG(ERROR) << "set custom data to record failed";
      }
    }

    result_vector.push_back(record);
    OhosAdapterHelper::GetInstance().GetPasteBoard().SetPasteData(result_vector
#if defined(OHOS_CLIPBOARD)
,
                                                                  ChangeCopyOptionMode(copy_option)
#endif // defined(OHOS_CLIPBOARD)
    );
    sequence_number_ = ClipboardSequenceNumberToken();
    return previous_data;
  }

#if defined(OHOS_CLIPBOARD)
  OHOS::NWeb::CopyOptionMode ChangeCopyOptionMode(ui::CopyOptionMode copy_option) {
    if (copy_option == ui::CopyOptionMode::NONE) {
      return OHOS::NWeb::CopyOptionMode::NONE;
    } else if (copy_option == ui::CopyOptionMode::IN_APP) {
      return OHOS::NWeb::CopyOptionMode::IN_APP;
    } else if (copy_option == ui::CopyOptionMode::LOCAL_DEVICE) {
      return OHOS::NWeb::CopyOptionMode::LOCAL_DEVICE;
    } else {
      return OHOS::NWeb::CopyOptionMode::CROSS_DEVICE;
    }
  }
#endif // defined(OHOS_CLIPBOARD)

  bool IsReadAllowed(const DataTransferEndpoint* data_dst,
                     absl::optional<ClipboardInternalFormat> format) const {
    DataTransferPolicyController* policy_controller =
        DataTransferPolicyController::Get();
    auto* data = GetData();
    if (!policy_controller || !data) {
      return true;
    }
    return policy_controller->IsClipboardReadAllowed(data->source(), data_dst,
                                                     data->size(format));
  }

  static void SetSpanstringConvertHtml(std::shared_ptr<OHOS::NWeb::NWebSpanstringConvertHtmlCallback> callback) {
    convert_html_callback_ = callback;
  }

 private:
  // True if the ClipboardData has format |format|.
  bool HasFormat(ClipboardInternalFormat format) const {
    const ClipboardData* data = GetData();
    return data ? data->format() & static_cast<int>(format) : false;
  }

  bool HasFormatInMisc(ClipboardInternalFormat format) {
    UpdateClipboardData();
    int allFormat = 0;
    if (!read_data_) {
      return false;
    }
    PasteRecordVector record_vector = read_data_->GetPasteRecordVector();
    const std::string SPAN_STRING_TAG = "openharmony.styled-string";
    for (auto& record : record_vector) {
      std::shared_ptr<std::string> html = record->GetHtmlTextV2();
      if (ArkWebGetErrno() != ArkWebInterfaceResult::RESULT_OK) {
        html = record->GetHtmlText();
      }
      std::shared_ptr<std::string> text = record->GetPlainTextV2();
      if (ArkWebGetErrno() != ArkWebInterfaceResult::RESULT_OK) {
        text = record->GetPlainText();
      }
      std::shared_ptr<ClipBoardImageDataAdapterImpl> imgData
        = std::make_shared<ClipBoardImageDataAdapterImpl>();

      bool imgFlag = false;
      imgFlag = record->GetImgData(imgData);
      std::shared_ptr<std::string> uri = record->GetUriV2();
      if (ArkWebGetErrno() != ArkWebInterfaceResult::RESULT_OK) {
        uri = record->GetUri();
      }
      std::shared_ptr<PasteCustomData> pasteCustomData = record->GetCustomDataV2();
      if (ArkWebGetErrno() != ArkWebInterfaceResult::RESULT_OK) {
        pasteCustomData = record->GetCustomData();
      }
      if (pasteCustomData && (pasteCustomData->find(SPAN_STRING_TAG) != pasteCustomData->end())) {
        allFormat |= static_cast<int>(ClipboardInternalFormat::kHtml);
      }
      if (html) {
        allFormat |= static_cast<int>(ClipboardInternalFormat::kHtml);
      }
      if (text) {
        allFormat |= static_cast<int>(ClipboardInternalFormat::kText);
      }
      if (imgFlag || uri) {
        allFormat |= static_cast<int>(ClipboardInternalFormat::kPng);
      }
      if (!(read_data_->ReadCustomDatas().empty())) {
        allFormat |= static_cast<int>(ClipboardInternalFormat::kCustom);
      }
    }
    return allFormat & static_cast<int>(format);
  }

  bool ReadPngRecordInner(const std::shared_ptr<PasteDataRecordAdapter>& record,
                          SkBitmap& img) {
    auto imgData = std::make_shared<ClipBoardImageDataAdapterImpl>();
    if (!imgData) {
      LOG(ERROR) << "ClipBoardImageDataAdapterImpl create failed";
      return false;
    }

    if (record->GetImgData(imgData)) {
      SkImageInfo skImageInfo = MakeSkImageInfoFromPixelMap(imgData);
      SkPixmap pixmap(skImageInfo, imgData->GetData(), imgData->GetRowBytes());
      if (!img.installPixels(pixmap)) {
        LOG(ERROR) << "installPixels failed";
        return false;
      }
      return true;
    }
    std::shared_ptr<std::string> uri = record->GetUriV2();
    if (ArkWebGetErrno() != ArkWebInterfaceResult::RESULT_OK) {
      uri = record->GetUri();
    }
    return ReadPngByUri(uri, img);
  }

  // Reads image from URI
  bool ReadPngByUri(const std::shared_ptr<std::string>& uri, SkBitmap& img) {
    if (!uri || uri->empty()) {
      return false;
    }

    std::string uriRealPath = base::GetRealPath(base::FilePath(*uri));
    if (uriRealPath.empty()) {
      LOG(ERROR) << "uri real path is empty";
      return false;
    }
    std::unique_ptr<SkStream> stream =
        SkStream::MakeFromFile(uriRealPath.c_str());
    if (!stream) {
      LOG(ERROR) << "Couldn't read current uriRealPath";
      return false;
    }
    sk_sp<SkData> data =
        SkData::MakeFromStream(stream.get(), stream->getLength());
    if (!data) {
      LOG(ERROR) << "Couldn't parse stream file";
      return false;
    }
    sk_sp<SkImage> image = SkImages::DeferredFromEncodedData(data);
    if (!image) {
      LOG(ERROR) << "invalid image, could not decode";
      return false;
    }

    if (!image->asLegacyBitmap(&img)) {
      LOG(ERROR) << "uri image store bitmap failed";
      return false;
    }
    return true;
  }

  void ReadCustomDataFromReadData(std::string type, std::string* result) const {
    if (!result) {
      return;
    }
    if (!read_data_) {
      LOG(ERROR) << "read_data is null";
      return;
    }
    auto custom_datas = read_data_->ReadCustomDatas();
    for (const auto& data : custom_datas) {
      if (!data.empty()) {
        std::unordered_map<std::string, std::string> custom_data_map;
        ReadCustomDataIntoMap(data.data(), data.size(), &custom_data_map);
        auto it = custom_data_map.find(type);
        if (it != custom_data_map.end()) {
          *result = it->second;
          return;
        }
      }
    }
  }

  std::shared_ptr<ClipBoardImageDataAdapter> WriteBitmapToClipboard(
      const SkBitmap& bitmap) {
    std::shared_ptr<ClipBoardImageDataAdapterImpl> imageInfo
      = std::make_shared<ClipBoardImageDataAdapterImpl>();
    if (!imageInfo) {
      LOG(ERROR) << "WriteBitmapToClipboard ClipBoardImageDataAdapterImpl create failed";
      return nullptr;
    }
    imageInfo->SetColorType(ImageToClipboardColorType(bitmap.colorType()));
    imageInfo->SetAlphaType(ImageToClipboardAlphaType(bitmap.alphaType()));
    imageInfo->SetData((uint32_t*)bitmap.getPixels());
    imageInfo->SetDataSize(bitmap.computeByteSize());
    imageInfo->SetWidth(bitmap.width());
    imageInfo->SetHeight(bitmap.height());
    return imageInfo;
  }
  // Current ClipboardData.
  std::unique_ptr<ClipboardData> data_;

  // Sequence number uniquely identifying clipboard state.
  ClipboardSequenceNumberToken sequence_number_;
  std::shared_ptr<PasteboardObserverOhos> observer_;
  int32_t observer_id_ = -1;
  ClipboardState state_ = ClipboardState::kOutOfDate;
  std::shared_ptr<ClipboardOhosReadData> read_data_ = nullptr;
  std::unique_ptr<OHOS::NWeb::OhosResourceAdapter> resource_adapter_ = nullptr;
  static std::shared_ptr<OHOS::NWeb::NWebSpanstringConvertHtmlCallback> convert_html_callback_;

  bool is_data_guard_enabled_ = false;
};

std::shared_ptr<OHOS::NWeb::NWebSpanstringConvertHtmlCallback>
    ClipboardOHOSInternal::convert_html_callback_ = nullptr;

class ClipboardDataBuilder {
 public:
  // If |data_src| is nullptr, this means that the data source isn't
  // confidential and the data can be pasted in any document.
  static void CommitToClipboard(
      ClipboardOHOSInternal* clipboard,
      std::unique_ptr<DataTransferEndpoint> data_src) {
    ClipboardData* data = GetCurrentData();
    if (data) {
      data->set_source(std::move(data_src));
      clipboard->WriteData(TakeCurrentData());
    }
  }

  static void WriteText(const char* text_data, size_t text_len
#if defined(OHOS_CLIPBOARD)
,
                        const CopyOptionMode copy_option
#endif // defined(OHOS_CLIPBOARD)
  ) {
    ClipboardData* data = GetCurrentData();
    if (data) {
      data->set_text(std::string(text_data, text_len));
#if defined(OHOS_CLIPBOARD)
      data->set_copy_option(copy_option);
#endif // defined(OHOS_CLIPBOARD)
    }
  }

  static void WriteHTML(const char* markup_data,
                        size_t markup_len,
                        const char* url_data,
                        size_t url_len
#if defined(OHOS_CLIPBOARD)
,
                        const CopyOptionMode copy_option
#endif // defined(OHOS_CLIPBOARD)
                        ) {
    ClipboardData* data = GetCurrentData();
    if (data) {
      data->set_markup_data(std::string(markup_data, markup_len));
      data->set_url(std::string(url_data, url_len));
#if defined(OHOS_CLIPBOARD)
      data->set_copy_option(copy_option);
#endif // defined(OHOS_CLIPBOARD)
    }
  }

  static void WriteBitmap(const SkBitmap& bitmap) {
    ClipboardData* data = GetCurrentData();
    if (data) {
      data->SetBitmapData(bitmap);
    }
  }

  static void WriteData(const std::string& format,
                        const char* data_data,
                        size_t data_len) {
    ClipboardData* data = GetCurrentData();
    if (data) {
      data->SetCustomData(format, std::string(data_data, data_len));
    }
  }

  static void SetCopyOptionMode(CopyOptionMode copy_option) {
    ClipboardData* data = GetCurrentData();
    if (data) {
      data->set_copy_option(copy_option);
    }
  }

 private:
  static ClipboardData* GetCurrentData() {
    if (!current_data_) {
      current_data_ = new ClipboardData;
    }
    return current_data_;
  }

  static std::unique_ptr<ClipboardData> TakeCurrentData() {
    std::unique_ptr<ClipboardData> data = base::WrapUnique(GetCurrentData());
    current_data_ = nullptr;
    return data;
  }

  // This is a raw pointer instead of a std::unique_ptr to avoid adding a
  // static initializer.
  static ClipboardData* current_data_;
};

ClipboardData* ClipboardDataBuilder::current_data_ = nullptr;

// static
ClipboardOHOS* ClipboardOHOS::GetForCurrentThread() {
  auto* clipboard = Clipboard::GetForCurrentThread();

  // Ensure type safety. In tests the instance may not be registered.
  if (!IsRegisteredInstance(clipboard)) {
    return nullptr;
  }

  return static_cast<ClipboardOHOS*>(clipboard);
}

// static
void ClipboardOHOS::SetConvertHtmlCallback(std::shared_ptr<OHOS::NWeb::NWebSpanstringConvertHtmlCallback> callback) {
  ClipboardOHOSInternal::SetSpanstringConvertHtml(callback);
}

// ClipboardOHOS implementation.
ClipboardOHOS::ClipboardOHOS()
    : clipboard_internal_(std::make_unique<ClipboardOHOSInternal>()) {
  DCHECK(CalledOnValidThread());
  RegisterInstance(this);
}

ClipboardOHOS::~ClipboardOHOS() {
  DCHECK(CalledOnValidThread());
  UnregisterInstance(this);
}

std::unique_ptr<ClipboardData> ClipboardOHOS::WriteClipboardData(
    std::unique_ptr<ClipboardData> data) {
  DCHECK(CalledOnValidThread());
  return clipboard_internal_->WriteData(std::move(data));
}

void ClipboardOHOS::OnPreShutdown() {}

std::vector<std::u16string> ClipboardOHOS::GetStandardFormats(
    ClipboardBuffer buffer,
    const DataTransferEndpoint* data_dst) const {
  std::vector<std::u16string> types;
  if (IsFormatAvailable(ClipboardFormatType::PlainTextType(), buffer,
                        data_dst)) {
    types.push_back(
        base::UTF8ToUTF16(ClipboardFormatType::PlainTextType().GetName()));
  }
  if (IsFormatAvailable(ClipboardFormatType::HtmlType(), buffer, data_dst)) {
    types.push_back(
        base::UTF8ToUTF16(ClipboardFormatType::HtmlType().GetName()));
  }
  if (IsFormatAvailable(ClipboardFormatType::SvgType(), buffer, data_dst)) {
    types.push_back(
        base::UTF8ToUTF16(ClipboardFormatType::SvgType().GetName()));
  }
  if (IsFormatAvailable(ClipboardFormatType::RtfType(), buffer, data_dst)) {
    types.push_back(
        base::UTF8ToUTF16(ClipboardFormatType::RtfType().GetName()));
  }
  if (IsFormatAvailable(ClipboardFormatType::BitmapType(), buffer, data_dst)) {
    types.push_back(base::UTF8ToUTF16(kMimeTypePNG));
  }
  if (IsFormatAvailable(ClipboardFormatType::FilenamesType(), buffer,
                        data_dst)) {
    types.push_back(base::UTF8ToUTF16(kMimeTypeURIList));
  }
  return types;
}

DataTransferEndpoint* ClipboardOHOS::GetSource(ClipboardBuffer buffer) const {
  const ClipboardData* data = clipboard_internal_->GetData();
  return data ? data->source() : nullptr;
}

const ClipboardSequenceNumberToken& ClipboardOHOS::GetSequenceNumber(
    ClipboardBuffer buffer) const {
  DCHECK(CalledOnValidThread());
  return clipboard_internal_->sequence_number();
}

bool ClipboardOHOS::IsFormatAvailable(
    const ClipboardFormatType& format,
    ClipboardBuffer buffer,
    const DataTransferEndpoint* data_dst) const {
  DCHECK(CalledOnValidThread());
  DCHECK(IsSupportedClipboardBuffer(buffer));

  if (format == ClipboardFormatType::PlainTextType() ||
      format == ClipboardFormatType::UrlType()) {
    return clipboard_internal_->IsFormatAvailable(
        ClipboardInternalFormat::kText);
  }
  if (format == ClipboardFormatType::HtmlType()) {
    return clipboard_internal_->IsFormatAvailable(
        ClipboardInternalFormat::kHtml);
  }
  if (format == ClipboardFormatType::BitmapType()) {
    return clipboard_internal_->IsFormatAvailable(
        ClipboardInternalFormat::kPng);
  }
  if (format == ClipboardFormatType::WebKitSmartPasteType()) {
    return clipboard_internal_->IsFormatAvailable(
        ClipboardInternalFormat::kWeb);
  }
  const ClipboardData* data = clipboard_internal_->GetData();
  return data && data->custom_data_format() == format.GetName();
}

void ClipboardOHOS::Clear(ClipboardBuffer buffer) {
  DCHECK(CalledOnValidThread());
  DCHECK(IsSupportedClipboardBuffer(buffer));
  clipboard_internal_->Clear();
}

void ClipboardOHOS::OnClipboardDataGuard(bool status) {
  if (clipboard_internal_) {
    clipboard_internal_->OnClipboardDataGuard(status);
  }
}

void ClipboardOHOS::ReadAvailableTypes(
    ClipboardBuffer buffer,
    const DataTransferEndpoint* data_dst,
    std::vector<std::u16string>* types) const {
  DCHECK(CalledOnValidThread());
  DCHECK(types);

  if (!clipboard_internal_->IsReadAllowed(data_dst, absl::nullopt)) {
    return;
  }
  types->clear();
  if (IsFormatAvailable(ClipboardFormatType::PlainTextType(), buffer,
                        data_dst)) {
    types->push_back(
        base::UTF8ToUTF16(ClipboardFormatType::PlainTextType().GetName()));
  }
  if (IsFormatAvailable(ClipboardFormatType::HtmlType(), buffer, data_dst)) {
    types->push_back(
        base::UTF8ToUTF16(ClipboardFormatType::HtmlType().GetName()));
  }
  if (IsFormatAvailable(ClipboardFormatType::BitmapType(), buffer, data_dst)) {
    types->push_back(base::UTF8ToUTF16(kMimeTypePNG));
  }
  if (clipboard_internal_->IsFormatAvailable(
          ClipboardInternalFormat::kCustom)) {
    clipboard_internal_->ReadAvailableCustomDataTypes(types);
  }

  std::ostringstream typesSumStream;
  for (const auto& str : *types) {
    typesSumStream << " " << base::UTF16ToUTF8(str);
  }
  LOG(INFO) << "clipboard available types =" << typesSumStream.str();
}

void ClipboardOHOS::ReadText(ClipboardBuffer buffer,
                             const DataTransferEndpoint* data_dst,
                             std::u16string* result) const {
  LOG(INFO) << "start read text";
  DCHECK(CalledOnValidThread());
  if (!clipboard_internal_->IsReadAllowed(data_dst,
                                          ClipboardInternalFormat::kText)) {
    LOG(ERROR) << "not allow to read when read text";
    return;
  }
  RecordRead(ClipboardFormatMetric::kText);
  clipboard_internal_->ReadText(result);
}

void ClipboardOHOS::ReadAsciiText(ClipboardBuffer buffer,
                                  const DataTransferEndpoint* data_dst,
                                  std::string* result) const {
  DCHECK(CalledOnValidThread());
}

void ClipboardOHOS::ReadHTML(ClipboardBuffer buffer,
                             const DataTransferEndpoint* data_dst,
                             std::u16string* markup,
                             std::string* src_url,
                             uint32_t* fragment_start,
                             uint32_t* fragment_end) const {
  LOG(INFO) << "start read html";
  DCHECK(CalledOnValidThread());

  if (!clipboard_internal_->IsReadAllowed(data_dst,
                                          ClipboardInternalFormat::kHtml)) {
    LOG(ERROR) << "not allow to read when read html";
    return;
  }

  RecordRead(ClipboardFormatMetric::kHtml);
  clipboard_internal_->ReadHTML(markup, src_url, fragment_start, fragment_end);
}

void ClipboardOHOS::ReadSvg(ClipboardBuffer buffer,
                            const DataTransferEndpoint* data_dst,
                            std::u16string* result) const {
  DCHECK(CalledOnValidThread());
}

void ClipboardOHOS::ReadRTF(ClipboardBuffer buffer,
                            const DataTransferEndpoint* data_dst,
                            std::string* result) const {
  DCHECK(CalledOnValidThread());
}

void ClipboardOHOS::ReadPng(ClipboardBuffer buffer,
                            const DataTransferEndpoint* data_dst,
                            ReadPngCallback callback) const {
  LOG(INFO) << "start read image";
  DCHECK(CalledOnValidThread());
  if (!clipboard_internal_->IsReadAllowed(data_dst,
                                          ClipboardInternalFormat::kPng)) {
    std::move(callback).Run(std::vector<uint8_t>());
    return;
  }
  RecordRead(ClipboardFormatMetric::kImage);
  clipboard_internal_->ReadPng(std::move(callback));
}

void ClipboardOHOS::ReadCustomData(ClipboardBuffer buffer,
                                   const std::u16string& type,
                                   const DataTransferEndpoint* data_dst,
                                   std::u16string* result) const {
  LOG(INFO) << "start read custom data, type = " << type;
  DCHECK(CalledOnValidThread());
  if (!clipboard_internal_->IsReadAllowed(data_dst,
                                          ClipboardInternalFormat::kCustom)) {
    return;
  }

  RecordRead(ClipboardFormatMetric::kCustomData);
  clipboard_internal_->ReadCustomData(type, result);
}

void ClipboardOHOS::ReadFilenames(ClipboardBuffer buffer,
                                  const DataTransferEndpoint* data_dst,
                                  std::vector<ui::FileInfo>* result) const {
  DCHECK(CalledOnValidThread());
}

void ClipboardOHOS::ReadBookmark(const DataTransferEndpoint* data_dst,
                                 std::u16string* title,
                                 std::string* url) const {
  DCHECK(CalledOnValidThread());
}

void ClipboardOHOS::ReadData(const ClipboardFormatType& format,
                             const DataTransferEndpoint* data_dst,
                             std::string* result) const {
  LOG(INFO) << "start read data, type = " << format.GetName();
  DCHECK(CalledOnValidThread());
  if (!clipboard_internal_->IsReadAllowed(data_dst, absl::nullopt)) {
    return;
  }

  RecordRead(ClipboardFormatMetric::kData);
  clipboard_internal_->ReadData(format.GetName(), result);
}

bool ClipboardOHOS::IsSelectionBufferAvailable() const {
  return true;
}

void ClipboardOHOS::WritePortableAndPlatformRepresentations(
    ClipboardBuffer buffer,
    const ObjectMap& objects,
    std::vector<Clipboard::PlatformRepresentation> platform_representations,
    std::unique_ptr<DataTransferEndpoint> data_src) {
  DCHECK(CalledOnValidThread());
  DCHECK(IsSupportedClipboardBuffer(buffer));

  DispatchPlatformRepresentations(std::move(platform_representations));
  for (const auto& object : objects) {
    DispatchPortableRepresentation(object.first, object.second);
  }

  ClipboardDataBuilder::CommitToClipboard(clipboard_internal_.get(),
                                          std::move(data_src));
}

void ClipboardOHOS::WriteText(const char* text_data, size_t text_len
#if defined(OHOS_CLIPBOARD)
,
                              const CopyOptionMode copy_option
#endif // defined(OHOS_CLIPBOARD)
) {
  ClipboardDataBuilder::WriteText(text_data, text_len
#if defined(OHOS_CLIPBOARD)
,
                                  copy_option
#endif // defined(OHOS_CLIPBOARD)
  );
}

void ClipboardOHOS::WriteHTML(const char* markup_data,
                              size_t markup_len,
                              const char* url_data,
                              size_t url_len
#if defined(OHOS_CLIPBOARD)
,
                              const CopyOptionMode copy_option
#endif // defined(OHOS_CLIPBOARD)
                              ) {
  ClipboardDataBuilder::WriteHTML(markup_data, markup_len, url_data, url_len
#if defined(OHOS_CLIPBOARD)
,
                                  copy_option
#endif // defined(OHOS_CLIPBOARD)
  );
}

void ClipboardOHOS::WriteUnsanitizedHTML(const char* markup_data,
                                         size_t markup_len,
                                         const char* url_data,
                                         size_t url_len
#if defined(OHOS_CLIPBOARD)
,
                                         const CopyOptionMode copy_option
#endif // defined(OHOS_CLIPBOARD)
                                         ) {
  ClipboardDataBuilder::WriteHTML(markup_data, markup_len, url_data, url_len
#if defined(OHOS_CLIPBOARD)
,
                                  copy_option
#endif // defined(OHOS_CLIPBOARD)
  );
}

void ClipboardOHOS::WriteSvg(const char* markup_data, size_t markup_len) {}

void ClipboardOHOS::WriteRTF(const char* rtf_data, size_t data_len) {}

void ClipboardOHOS::WriteFilenames(std::vector<ui::FileInfo> filenames) {}

void ClipboardOHOS::WriteBookmark(const char* title_data,
                                  size_t title_len,
                                  const char* url_data,
                                  size_t url_len
#if defined(OHOS_CLIPBOARD)
,
                                  const CopyOptionMode copy_option
#endif // defined(OHOS_CLIPBOARD)
                                  ) {}

void ClipboardOHOS::WriteWebSmartPaste(
#if defined(OHOS_CLIPBOARD)
  const CopyOptionMode copy_option
#endif // defined(OHOS_CLIPBOARD)
) {}

void ClipboardOHOS::WriteBitmap(const SkBitmap& bitmap) {
  ClipboardDataBuilder::WriteBitmap(bitmap);
}

void ClipboardOHOS::WriteData(const ClipboardFormatType& format,
                              const char* data_data,
                              size_t data_len) {
  ClipboardDataBuilder::WriteData(format.GetName(), data_data, data_len);
}

bool ClipboardOHOS::HasPasteData() const {
  return OhosAdapterHelper::GetInstance().GetPasteBoard().HasPasteData();
}

void ClipboardOHOS::SetCopyOptionMode(CopyOptionMode copy_option) {
  ClipboardDataBuilder::SetCopyOptionMode(copy_option);
}

}  // namespace ui
