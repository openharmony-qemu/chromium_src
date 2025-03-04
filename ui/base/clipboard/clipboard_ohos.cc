// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/clipboard/clipboard_ohos.h"

#include <map>
#include <set>
#include <unordered_map>

#include "base/check_op.h"
#include "base/command_line.h"
#include "base/containers/contains.h"
#include "base/feature_list.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/no_destructor.h"
#include "base/notreached.h"
#include "base/strings/utf_string_conversions.h"
#include "base/synchronization/lock.h"
#include "base/types/optional_util.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/content_switches.h"
#include "ohos/adapter/pasteboard/pasteboard_adapter.h"
#include "skia/ext/skia_utils_base.h"
#include "third_party/icu/source/i18n/unicode/regex.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkColorSpace.h"
#include "ui/base/clipboard/clipboard_constants.h"
#include "ui/base/clipboard/clipboard_data.h"
#include "ui/base/clipboard/clipboard_format_type.h"
#include "ui/base/clipboard/clipboard_metrics.h"
#include "ui/base/clipboard/clipboard_monitor.h"
#include "ui/base/clipboard/clipboard_util.h"
#include "ui/base/clipboard/custom_data_helper.h"
#include "ui/base/data_transfer_policy/data_transfer_endpoint.h"
#include "ui/base/data_transfer_policy/data_transfer_policy_controller.h"
#include "url/gurl.h"
#include "url/url_util.h"

namespace ui {
namespace {

constexpr int kMaxUriDecodeLen = 2048;

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

ohos::adapter::ClipBoardImageAlphaType ImageToClipboardAlphaType(
    SkAlphaType alpha_type) {
  switch (alpha_type) {
    case kUnknown_SkAlphaType:
      return ohos::adapter::ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
    case kOpaque_SkAlphaType:
      return ohos::adapter::ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE;
    case kPremul_SkAlphaType:
      return ohos::adapter::ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED;
    default:
      return ohos::adapter::ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
  }
}

ohos::adapter::ClipBoardImageColorType ImageToClipboardColorType(
    SkColorType color_type) {
  switch (color_type) {
    case kRGBA_8888_SkColorType:
      return ohos::adapter::ClipBoardImageColorType::COLOR_TYPE_RGBA_8888;
    case kBGRA_8888_SkColorType:
      return ohos::adapter::ClipBoardImageColorType::COLOR_TYPE_BGRA_8888;
    default:
      return ohos::adapter::ClipBoardImageColorType::COLOR_TYPE_UNKNOWN;
  }
}

}  // namespace

Clipboard* Clipboard::Create() {
  return new ClipboardOHOS;
}

class ClipboardOHOSInternal {
 public:
  ClipboardOHOSInternal() {}

  ~ClipboardOHOSInternal() {}

  void Clear() {
    sequence_number_ = ClipboardSequenceNumberToken();
    data_.reset();
    auto& paste_board_adapter_instance =
        ohos::adapter::PasteBoardAdapter::GetInstance();
    paste_board_adapter_instance.Clear();
  }

  const ClipboardSequenceNumberToken& sequence_number() const {
    return sequence_number_;
  }
  // Returns the current clipboard data, which may be nullptr if nothing has
  // been written since the last Clear().
  const ClipboardData* GetData() const { return data_.get(); }

  // Returns true if the data on top of the clipboard stack has format |format|
  // or another format that can be converted to |format|.
  bool IsFormatAvailable(ClipboardInternalFormat format) const {
    return HasFormatInMisc(format);
  }

  SkAlphaType AlphaTypeToSkAlphaType(
      const ohos::adapter::ClipBoardImageData& img_data) const {
    switch (img_data.alpha_type) {
      case ohos::adapter::ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN:
        return SkAlphaType::kUnknown_SkAlphaType;
      case ohos::adapter::ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE:
        return SkAlphaType::kOpaque_SkAlphaType;
      case ohos::adapter::ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED:
        return SkAlphaType::kPremul_SkAlphaType;
      case ohos::adapter::ClipBoardImageAlphaType::ALPHA_TYPE_UNPREMUL:
        return SkAlphaType::kUnpremul_SkAlphaType;
      default:
        return SkAlphaType::kUnknown_SkAlphaType;
    }
  }

  SkColorType PixelFormatToSkColorType(
      const ohos::adapter::ClipBoardImageData& img_data) const {
    switch (img_data.color_type) {
      case ohos::adapter::ClipBoardImageColorType::COLOR_TYPE_RGBA_8888:
        return SkColorType::kRGBA_8888_SkColorType;
      case ohos::adapter::ClipBoardImageColorType::COLOR_TYPE_BGRA_8888:
        return SkColorType::kBGRA_8888_SkColorType;
      default:
        return SkColorType::kUnknown_SkColorType;
    }
  }

  SkImageInfo MakeSkImageInfoFromPixelMap(
      const ohos::adapter::ClipBoardImageData& img_data) const {
    SkColorType color_type = PixelFormatToSkColorType(img_data);
    SkAlphaType alpha_type = AlphaTypeToSkAlphaType(img_data);
    sk_sp<SkColorSpace> color_space = SkColorSpace::MakeSRGB();
    return SkImageInfo::Make(img_data.width, img_data.height, color_type,
                             alpha_type, color_space);
  }

  // Reads text from the ClipboardData.
  void ReadText(std::u16string* result) {
    if (!result) {
      LOG(ERROR) << "readtext result is null";
      return;
    }
    auto& pasteboard_adapter_instance =
        ohos::adapter::PasteBoardAdapter::GetInstance();
    std::string message = pasteboard_adapter_instance.ReadText();
    *result = base::UTF8ToUTF16(message);
  }

  // Reads HTML from the ClipboardData.
  void ReadHTML(std::u16string* markup,
                std::string* src_url,
                uint32_t* fragment_start,
                uint32_t* fragment_end) {
    if (!markup || !src_url || !fragment_start || !fragment_end) {
      LOG(ERROR) << "readhtml result is null";
      return;
    }

    markup->clear();
    src_url->clear();
    *fragment_start = 0;
    *fragment_end = 0;

    auto& pasteboard_adapter_instance =
        ohos::adapter::PasteBoardAdapter::GetInstance();
    std::string message_html = pasteboard_adapter_instance.ReadHTML();
    std::string message_text = pasteboard_adapter_instance.ReadText();
    if (!message_html.empty()) {
      *markup = base::UTF8ToUTF16(message_html);
      *fragment_start = 0;
      *fragment_end = static_cast<uint32_t>(markup->length());
    } else if (!message_text.empty()) {
      *markup = base::UTF8ToUTF16("<span>" + message_text + "</span>");
      *fragment_start = 0;
      *fragment_end = static_cast<uint32_t>(markup->length());
    }
  }

  // Reads filenames image from the ClipboardData.
  void ReadFilenames(std::vector<ui::FileInfo>* result) {
    if (!result) {
      LOG(ERROR) << "readFilenames result is null";
      return;
    }
    auto& pasteboard_adapter_instance =
        ohos::adapter::PasteBoardAdapter::GetInstance();
    std::vector<std::string> file_paths =
        pasteboard_adapter_instance.ReadFilenames();

    std::vector<ui::FileInfo> filenames;
    for (const std::string& file_path : file_paths) {
      url::RawCanonOutputW<kMaxUriDecodeLen> unescaped_path;
      url::DecodeURLEscapeSequences(file_path.c_str(),
                                    url::DecodeURLMode::kUTF8OrIsomorphic,
                                    &unescaped_path);
      std::string decode_file_path = base::UTF16ToUTF8(
          std::u16string_view(unescaped_path.data(), unescaped_path.length()));
      filenames.emplace_back(
          base::FilePath(FILE_PATH_LITERAL(decode_file_path)),
          base::FilePath());
    }
    if (filenames.empty()) {
      LOG(ERROR) << "readFilenames filenames is empty";
      return;
    }

    *result = filenames;
  }

  // Reads image from the ClipboardData.
  void ReadPng(Clipboard::ReadPngCallback callback) {
    if (!HasFormatInMisc(ClipboardInternalFormat::kPng)) {
      LOG(ERROR) << "no bitMap format in pasteboard";
      std::move(callback).Run(std::vector<uint8_t>());
      return;
    }
    SkBitmap img;
    auto& paste_board_adapter_instance =
        ohos::adapter::PasteBoardAdapter::GetInstance();
    ohos::adapter::ClipBoardImageData img_data =
        paste_board_adapter_instance.ReadImage();
    SkImageInfo sk_image_info = MakeSkImageInfoFromPixelMap(img_data);
    SkPixmap pixmap(sk_image_info, img_data.data.get(), img_data.row_bytes);
    if (!img.installPixels(pixmap)) {
      LOG(ERROR) << "installPixels failed";
    } else {
      std::vector<uint8_t> encode_png = clipboard_util::EncodeBitmapToPng(img);
      std::move(callback).Run(std::move(encode_png));
      return;
    }
    LOG(ERROR) << "get image from pasteboard failed";
    std::move(callback).Run(std::vector<uint8_t>());
  }

  void ReadData(const std::string& type, std::string* result) const {
    if (result) {
      result->clear();
      auto& pasteboard_adapter_instance =
          ohos::adapter::PasteBoardAdapter::GetInstance();
      std::string message = pasteboard_adapter_instance.ReadData(type);
      *result = message;
    } else {
      LOG(ERROR) << "result is null";
    }
  }

  // Writes |data| to the ClipboardData and returns the previous data.
  std::unique_ptr<ClipboardData> WriteData(
      std::unique_ptr<ClipboardData> data) {
    DCHECK(data);
    std::unique_ptr<ClipboardData> previous_data = std::move(data_);
    data_ = std::move(data);
    const ClipboardData* current_data = GetData();
    if (!current_data) {
      return nullptr;
    }

    if (HasFormat(ClipboardInternalFormat::kPng)) {
      sequence_number_ = ClipboardSequenceNumberToken();
      return previous_data;
    }

    ohos::adapter::PasteDataRecord record;
    auto& pasteboard_adapter_instance =
        ohos::adapter::PasteBoardAdapter::GetInstance();
    if (HasFormat(ClipboardInternalFormat::kHtml)) {
      record.html_text = current_data->markup_data();
    }

    if (HasFormat(ClipboardInternalFormat::kText)) {
      record.plain_text = current_data->text();
    }

    if (HasFormat(ClipboardInternalFormat::kCustom)) {
      if (current_data->HasCustomDataFormat(
              ClipboardFormatType::BookMarkType())) {
        record.bookmark_data =
            current_data->GetCustomData(ClipboardFormatType::BookMarkType());
      }
    }

    pasteboard_adapter_instance.SetPasteData(record);
    sequence_number_ = ClipboardSequenceNumberToken();
    return previous_data;
  }

  bool IsReadAllowed(const DataTransferEndpoint* data_dst,
                     std::optional<ClipboardInternalFormat> format,
                     const std::optional<ClipboardFormatType>&
                         custom_data_format = std::nullopt) const {
    DataTransferPolicyController* policy_controller =
        DataTransferPolicyController::Get();
    auto* data = GetData();
    if (!policy_controller || !data || !data_dst) {
      return true;
    }
    return policy_controller->IsClipboardReadAllowed(
        data->source(), data_dst,
        data->CalculateSize(format, custom_data_format));
  }

 private:
  // True if the ClipboardData has format |format|.
  bool HasFormat(ClipboardInternalFormat format) const {
    const ClipboardData* data = GetData();
    return data ? data->format() & static_cast<int>(format) : false;
  }

  bool HasFormatInMisc(ClipboardInternalFormat format) const {
    auto& paste_board_adapter_instance =
        ohos::adapter::PasteBoardAdapter::GetInstance();
    int all_format = 0;
    if ((format == ClipboardInternalFormat::kHtml) &&
        (paste_board_adapter_instance.IsExistMimeType("htmlText"))) {
      all_format |= static_cast<int>(ClipboardInternalFormat::kHtml);
    }
    if ((format == ClipboardInternalFormat::kText) &&
        (paste_board_adapter_instance.IsExistMimeType("plainText"))) {
      all_format |= static_cast<int>(ClipboardInternalFormat::kText);
    }
    if ((format == ClipboardInternalFormat::kBookmark) &&
        (paste_board_adapter_instance.IsExistMimeType(
            "chromium/x-bookmark-entries"))) {
      all_format |= static_cast<int>(ClipboardInternalFormat::kBookmark);
    }
    if ((format == ClipboardInternalFormat::kPng) &&
        (paste_board_adapter_instance.IsExistMimeType("pixelMap"))) {
      all_format |= static_cast<int>(ClipboardInternalFormat::kPng);
    }
    if ((format == ClipboardInternalFormat::kFilenames) &&
        (paste_board_adapter_instance.IsExistMimeType("uri"))) {
      all_format |= static_cast<int>(ClipboardInternalFormat::kFilenames);
    }
    return all_format & static_cast<int>(format);
  }

  // Current ClipboardData.
  std::unique_ptr<ClipboardData> data_;

  // Sequence number uniquely identifying clipboard state.
  ClipboardSequenceNumberToken sequence_number_;
};

class ClipboardDataBuilderOhos {
 public:
  // If |data_src| is nullptr, this means that the data source isn't
  // confidential and the data can be pasted in any document.
  static void CommitToClipboard(ClipboardOHOSInternal* clipboard,
                                std::optional<DataTransferEndpoint> data_src) {
    ClipboardData* data = GetCurrentData();
    if (data) {
      data->set_source(std::move(data_src));
      clipboard->WriteData(TakeCurrentData());
    }
  }

  static void WriteText(std::string_view text) {
    ClipboardData* data = GetCurrentData();
    if (data) {
      data->set_text(text.data());
    }
  }

  static void WriteHTML(std::string_view markup,
                        std::optional<std::string_view> source_url) {
    ClipboardData* data = GetCurrentData();
    if (data) {
      data->set_markup_data(std::string(markup));
      data->set_url(source_url ? std::string(*source_url) : std::string());
    }
  }

  static void WriteBitmap(const SkBitmap& bitmap) {
    ClipboardData* data = GetCurrentData();
    if (data) {
      data->SetBitmapData(bitmap);
    }
    auto& pasteboard_adapter_instance =
        ohos::adapter::PasteBoardAdapter::GetInstance();
    pasteboard_adapter_instance.WriteImage(
        (uint8_t*)bitmap.getPixels(), bitmap.height(), bitmap.width(),
        ImageToClipboardColorType(bitmap.colorType()),
        ImageToClipboardAlphaType(bitmap.alphaType()));
  }

  static void WriteData(const ClipboardFormatType& format,
                        base::span<const uint8_t> data) {
    ClipboardData* clipboard_data = GetCurrentData();
    clipboard_data->SetCustomData(
        format,
        std::string(reinterpret_cast<const char*>(data.data()), data.size()));
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

ClipboardData* ClipboardDataBuilderOhos::current_data_ = nullptr;

// static
ClipboardOHOS* ClipboardOHOS::GetForCurrentThread() {
  auto* clipboard = Clipboard::GetForCurrentThread();

  // Ensure type safety. In tests the instance may not be registered.
  if (!IsRegisteredInstance(clipboard)) {
    return nullptr;
  }

  return static_cast<ClipboardOHOS*>(clipboard);
}

// ClipboardOHOS implementation.
ClipboardOHOS::ClipboardOHOS()
    : clipboard_internal_(std::make_unique<ClipboardOHOSInternal>()) {
  DCHECK(CalledOnValidThread());
  RegisterInstance(this);

  // Unfortunately we cannot call Clipboard::IsSupportedClipboardBuffer()
  // from here because some components (like Ozone) are not yet initialized,
  // so create internal clipboards for platform supported clipboard buffers.
  constexpr ClipboardBuffer kClipboardBuffers[] = {
      ClipboardBuffer::kCopyPaste,
      ClipboardBuffer::kSelection,
  };
  for (ClipboardBuffer buffer : kClipboardBuffers) {
    internal_clipboards_[buffer] = std::make_unique<ClipboardOHOSInternal>();
  }
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
    types.push_back(
        base::UTF8ToUTF16(ClipboardFormatType::PngType().GetName()));
  }
  if (IsFormatAvailable(ClipboardFormatType::FilenamesType(), buffer,
                        data_dst)) {
    types.push_back(
        base::UTF8ToUTF16(ClipboardFormatType::FilenamesType().GetName()));
  }
  return types;
}

std::optional<DataTransferEndpoint> ClipboardOHOS::GetSource(
    ClipboardBuffer buffer) const {
  const ClipboardData* data = clipboard_internal_->GetData();
  return data ? data->source() : std::nullopt;
}

const ClipboardSequenceNumberToken& ClipboardOHOS::GetSequenceNumber(
    ClipboardBuffer buffer) const {
  DCHECK(CalledOnValidThread());
  return clipboard_internal_->sequence_number();
}

// |data_dst| is not used. It's only passed to be consistent with other
// platforms.
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
  if (format == ClipboardFormatType::BookMarkType()) {
    return clipboard_internal_->IsFormatAvailable(
        ClipboardInternalFormat::kBookmark);
  }
  if (format == ClipboardFormatType::FilenamesType()) {
    return clipboard_internal_->IsFormatAvailable(
        ClipboardInternalFormat::kFilenames);
  }

  const ClipboardData* data = clipboard_internal_->GetData();
  if (!data) {
    LOG(ERROR) << "[ClipboardData] Get ClipboardData is null";
    return false;
  }
  return data->HasCustomDataFormat(format);
}

void ClipboardOHOS::Clear(ClipboardBuffer buffer) {
  DCHECK(CalledOnValidThread());
  DCHECK(IsSupportedClipboardBuffer(buffer));
  clipboard_internal_->Clear();
}

void ClipboardOHOS::ReadAvailableTypes(
    ClipboardBuffer buffer,
    const DataTransferEndpoint* data_dst,
    std::vector<std::u16string>* types) const {
  DCHECK(CalledOnValidThread());
  DCHECK(types);

  const ClipboardOHOSInternal& clipboard_internal =
      GetInternalClipboard(buffer);

  if (!clipboard_internal.IsReadAllowed(data_dst, std::nullopt)) {
    return;
  }

  types->clear();
  *types = GetStandardFormats(buffer, data_dst);

  if (clipboard_internal.IsFormatAvailable(ClipboardInternalFormat::kCustom) &&
      clipboard_internal.GetData()) {
    const auto& custom_data =
        clipboard_internal.GetData()->GetDataTransferCustomData();
    ReadCustomDataTypes(
        base::as_bytes(base::span(custom_data.data(), custom_data.length())),
        types);
  }
}

void ClipboardOHOS::ReadText(ClipboardBuffer buffer,
                             const DataTransferEndpoint* data_dst,
                             std::u16string* result) const {
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

void ClipboardOHOS::ReadDataTransferCustomData(
    ClipboardBuffer buffer,
    const std::u16string& type,
    const DataTransferEndpoint* data_dst,
    std::u16string* result) const {
  DCHECK(CalledOnValidThread());
}

void ClipboardOHOS::ReadFilenames(ClipboardBuffer buffer,
                                  const DataTransferEndpoint* data_dst,
                                  std::vector<ui::FileInfo>* result) const {
  DCHECK(CalledOnValidThread());
  if (!clipboard_internal_->IsReadAllowed(
          data_dst, ClipboardInternalFormat::kFilenames)) {
    LOG(INFO) << "not allow to read when read filenames";
    return;
  }
  RecordRead(ClipboardFormatMetric::kFilenames);
  clipboard_internal_->ReadFilenames(std::move(result));
}

void ClipboardOHOS::ReadBookmark(const DataTransferEndpoint* data_dst,
                                 std::u16string* title,
                                 std::string* url) const {
  DCHECK(CalledOnValidThread());
}

void ClipboardOHOS::ReadData(const ClipboardFormatType& format,
                             const DataTransferEndpoint* data_dst,
                             std::string* result) const {
  DCHECK(CalledOnValidThread());

  if (!clipboard_internal_->IsReadAllowed(data_dst, std::nullopt)) {
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
    std::unique_ptr<DataTransferEndpoint> data_src,
    uint32_t privacy_types) {
  DCHECK(CalledOnValidThread());
  DCHECK(IsSupportedClipboardBuffer(buffer));

  if (buffer == ClipboardBuffer::kSelection) {
    return;
  }

  clipboard_internal_->Clear();
  DispatchPlatformRepresentations(std::move(platform_representations));
  for (const auto& object : objects) {
    DispatchPortableRepresentation(object.second);
  }

  ClipboardDataBuilderOhos::CommitToClipboard(
      clipboard_internal_.get(), base::OptionalFromPtr(data_src.get()));
}

void ClipboardOHOS::WriteText(std::string_view text) {
  ClipboardDataBuilderOhos::WriteText(text);
}

void ClipboardOHOS::WriteHTML(std::string_view markup,
                              std::optional<std::string_view> source_url) {
  ClipboardDataBuilderOhos::WriteHTML(markup, source_url);
}

void ClipboardOHOS::WriteSvg(std::string_view markup) {}

void ClipboardOHOS::WriteRTF(std::string_view rtf) {}

void ClipboardOHOS::WriteFilenames(std::vector<ui::FileInfo> filenames) {}

void ClipboardOHOS::WriteBookmark(std::string_view title,
                                  std::string_view url) {}

void ClipboardOHOS::WriteWebSmartPaste() {}

void ClipboardOHOS::WriteBitmap(const SkBitmap& bitmap) {
  ClipboardDataBuilderOhos::WriteBitmap(bitmap);
}

void ClipboardOHOS::WriteData(const ClipboardFormatType& format,
                              base::span<const uint8_t> data) {
  RecordWrite(ClipboardFormatMetric::kData);
  ClipboardDataBuilderOhos::WriteData(format, data);
}

void ClipboardOHOS::WriteClipboardHistory() {}

void ClipboardOHOS::WriteUploadCloudClipboard() {}

void ClipboardOHOS::WriteConfidentialDataForPassword() {}

const ClipboardOHOSInternal& ClipboardOHOS::GetInternalClipboard(
    ClipboardBuffer buffer) const {
  return *internal_clipboards_.at(buffer);
}

}  // namespace ui
