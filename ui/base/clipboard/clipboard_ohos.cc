/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ui/base/clipboard/clipboard_ohos.h"

#include <accesstoken/ability_access_control.h>
#include <database/pasteboard/oh_pasteboard.h>
#include <database/pasteboard/oh_pasteboard_err_code.h>
#include <database/udmf/udmf_err_code.h>
#include <database/udmf/udmf_meta.h>

#include <map>
#include <set>

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
#include "base/task/thread_pool.h"
#include "base/threading/hang_watcher.h"
#include "base/types/optional_util.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/common/content_switches.h"
#include "ohos/adapter/common/shared_library.h"
#include "ohos/adapter/file_manager/file_manager_adapter.h"
#include "ohos/adapter/permission_manager/permission_manager_adapter.h"
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

namespace ohos_permission = ohos::adapter::permission;
namespace ui {
namespace {

constexpr base::TimeDelta kClipboardHangWatchTime = base::Seconds(30);
constexpr int kMaxUriDecodeLen = 2048;
const char* kImageBmp = "image/bmp";
const std::string kPasteboardLogTag = "[OhosPasteboard] ";

const std::map<ClipboardInternalFormat, std::vector<std::string>> kTypeMapping = {
    { ClipboardInternalFormat::kHtml, { "text/html" } },
    { ClipboardInternalFormat::kText, { "text/plain" } },
    { ClipboardInternalFormat::kBookmark, { "chromium/x-bookmark-entries" } },
    { ClipboardInternalFormat::kPng, { "pixelMap", "image/bmp" } },
    { ClipboardInternalFormat::kFilenames, { "text/uri" } },
    { ClipboardInternalFormat::kRtf, { "text/rtf" } }
};

using PasteboardGetChangeCountFunc = uint32_t(OH_Pasteboard*);

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

}  // namespace

Clipboard* Clipboard::Create() {
    ohos_permission::PermissionManagerAdapter::CheckAndRequestPermission(
        ohos_permission::OHOSPermissionType::PASTEBOARD);
  return new ClipboardOHOS;
}

class ClipboardOHOSInternal {
 public:
  ClipboardOHOSInternal() : pasteboard_lib_("pasteboard") {
    pasteboard_ = OH_Pasteboard_Create();
    if (!pasteboard_lib_.IsLoaded()) {
      LOG(ERROR) << kPasteboardLogTag << "pasteboard lib load failed";
      return;
    }
    bool load_success = pasteboard_lib_.LoadFunction(&pasteboard_get_change_count_func_,
                                                     "OH_Pasteboard_GetChangeCount");
    if (!load_success) {
      LOG(ERROR) << kPasteboardLogTag << "load OH_Pasteboard_GetChangeCount failed";
    }
  }

  ~ClipboardOHOSInternal() {
    if (pasteboard_ == nullptr) {
      LOG(ERROR) << "[Pasteboard]~ClipboardOHOSInternal, pasteboard_ is null";
      return;
    }
    OH_Pasteboard_Destroy(pasteboard_);
    pasteboard_get_change_count_func_ = nullptr;
  }
  void WritePasteboard(OH_UdmfRecord*& udmf_record) {
    OH_UdmfData* udmf_data = OH_UdmfData_Create();
    if (udmf_data == nullptr) {
      LOG(ERROR) << "[Pasteboard]WritePasteboard OH_UdmfData_Create fail, "
                        "udmf_data is null";
      return;
    }
    auto result = OH_UdmfData_AddRecord(udmf_data, udmf_record);
    if (result != ERR_OK) {
      LOG(ERROR) << "[Pasteboard]WritePasteboard OH_UdmfData_AddRecord failed,error " 
                 "code is :" << result;
      DestroyUdmfData(&udmf_data);
      return;
    }

    //The value must be the same as the timeout interval of the OH pasteboard interface.
    base::WatchHangsInScope scope(kClipboardHangWatchTime);
    
    result = OH_Pasteboard_SetData(pasteboard_, udmf_data);
    if (result != ERR_OK) {
      LOG(ERROR) << "[Pasteboard]WritePasteboard OH_Pasteboard_SetData failed,error code is :" 
          << result;
    }
    DestroyUdmfData(&udmf_data);
  }

  void Clear() {
    sequence_number_ = ClipboardSequenceNumberToken();
    data_.reset();
  }

  __attribute__((no_sanitize("cfi", "cfi-icall")))
  const ClipboardSequenceNumberToken& sequence_number() const {
    if (pasteboard_get_change_count_func_ == nullptr) {
      LOG(WARNING) << kPasteboardLogTag << "can not get OH_Pasteboard_GetChangeCount.";
      return sequence_number_;
    }
    uint32_t sequence_number = pasteboard_get_change_count_func_(pasteboard_);
    if (sequence_number != clipboard_sequence_.sequence_number) {
      // Generate a unique token associated with the current sequence number.
      clipboard_sequence_ = {sequence_number, ClipboardSequenceNumberToken()};
    }
    return clipboard_sequence_.token;
  }
  // Returns the current clipboard data, which may be nullptr if nothing has
  // been written since the last Clear().
  const ClipboardData* GetData() const { return data_.get(); }

  // Returns true if the data on top of the clipboard stack has format |format|
  // or another format that can be converted to |format|.
  bool IsFormatAvailable(ClipboardInternalFormat format) {
    LOG(INFO) << kPasteboardLogTag << __FUNCTION__ << " format:" << static_cast<int>(format);
    auto iter = kTypeMapping.find(format);
    if (iter == kTypeMapping.end()) {
      return false;
    }
    for (std::string mime_type: iter->second) {
      if (OH_Pasteboard_HasType(pasteboard_, mime_type.c_str())) {
        return true;
      }
    }
    return false;
  }

  SkAlphaType AlphaTypeToSkAlphaType(
      const ui::ClipBoardImageAlphaType alpha_type) const {
    switch (alpha_type) {
      case ui::ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN:
        return SkAlphaType::kUnknown_SkAlphaType;
      case ui::ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE:
        return SkAlphaType::kOpaque_SkAlphaType;
      case ui::ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED:
        return SkAlphaType::kPremul_SkAlphaType;
      case ui::ClipBoardImageAlphaType::ALPHA_TYPE_UNPREMUL:
        return SkAlphaType::kUnpremul_SkAlphaType;
      default:
        return SkAlphaType::kUnknown_SkAlphaType;
    }
  }

  SkColorType PixelFormatToSkColorType(
      const ui::ClipBoardImageColorType color_type) const {
    switch (color_type) {
      case ui::ClipBoardImageColorType::COLOR_TYPE_RGBA_8888:
        return SkColorType::kRGBA_8888_SkColorType;
      case ui::ClipBoardImageColorType::COLOR_TYPE_BGRA_8888:
        return SkColorType::kBGRA_8888_SkColorType;
      default:
        return SkColorType::kUnknown_SkColorType;
    }
  }

  ui::ClipBoardImageAlphaType ImageToClipboardAlphaType(
      SkAlphaType alpha_type) {
    switch (alpha_type) {
      case kUnknown_SkAlphaType:
        return ui::ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
      case kOpaque_SkAlphaType:
        return ui::ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE;
      case kPremul_SkAlphaType:
        return ui::ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED;
      default:
        return ui::ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
    }
  }
  ui::ClipBoardImageColorType ImageToClipboardColorType(
      SkColorType color_type) {
    switch (color_type) {
      case kRGBA_8888_SkColorType:
        return ui::ClipBoardImageColorType::COLOR_TYPE_RGBA_8888;
      case kBGRA_8888_SkColorType:
        return ui::ClipBoardImageColorType::COLOR_TYPE_BGRA_8888;
      default:
        return ui::ClipBoardImageColorType::COLOR_TYPE_UNKNOWN;
    }
  }
  void DestroyUdsPlainText(OH_UdsPlainText** plain_text) {
    if (*plain_text != nullptr) {
      OH_UdsPlainText_Destroy(*plain_text);
      *plain_text = nullptr;
    }
  }
  void DestroyUdmfRecord(OH_UdmfRecord** udmf_record) const {
    if (*udmf_record) {
      OH_UdmfRecord_Destroy(*udmf_record);
      *udmf_record = nullptr;
    }
  }
  void DestroyUdsFileUri(OH_UdsFileUri** uds_file_uri) const {
    OH_UdsFileUri_Destroy(*uds_file_uri);
    *uds_file_uri = nullptr;
  }
  void DestroyUdsHtml(OH_UdsHtml** uds_html) const {
    OH_UdsHtml_Destroy(*uds_html);
    *uds_html = nullptr;
  }
  void DestroyUdsPixelMap(OH_UdsPixelMap** uds_pixel_map) const {
    OH_UdsPixelMap_Destroy(*uds_pixel_map);
    *uds_pixel_map = nullptr;
  }
  void DestroyUdmfData(OH_UdmfData** udmf_data) const {
    OH_UdmfData_Destroy(*udmf_data);
    *udmf_data = nullptr;
  }

  // Reads text from the ClipboardData.
  void ReadTextFromPasteBoard(std::u16string* result) {
    if (!result) {
      LOG(ERROR) << "ReadTextFromPasteBoard result is null";
      return;
    }
    int status = -1;
    if (!OH_Pasteboard_HasData(pasteboard_)) {
        LOG(ERROR) << kPasteboardLogTag
        << " ReadTextFromPasteBoard OH_Pasteboard_HasData fail.";
        return;
    }

    //The value must be the same as the timeout interval of the OH pasteboard interface.
    base::WatchHangsInScope scope(kClipboardHangWatchTime);
    
    OH_UdmfData* udmf_data = OH_Pasteboard_GetData(pasteboard_, &status);
    if (status != ERR_OK) {
        LOG(ERROR) << kPasteboardLogTag
        << " ReadTextFromPasteBoard OH_Pasteboard_GetData fail,error code:"
        << status;
        return;
    }
    unsigned int count = 0;
    OH_UdmfRecord** records = OH_UdmfData_GetRecords(udmf_data, &count);
    if (records == nullptr) {
      LOG(ERROR)
          << "[Pasteboard]ReadTextFromPasteBoard OH_UdmfData_GetRecords fail";
      return;
    }
    for (unsigned int i = 0; i < count; i++) {
      OH_UdmfRecord* udmf_record = records[i];
      if (udmf_record == nullptr) {
        LOG(ERROR)
            << "[Pasteboard]ReadTextFromPasteBoard udmf_record is null";
        continue;
      }
      OH_UdsPlainText* plain_text = OH_UdsPlainText_Create();
      if (plain_text == nullptr) {
        LOG(ERROR) << "[Pasteboard]ReadTextFromPasteBoard "
                      "OH_UdsPlainText_Create fail, "
                      "plain_text is null";
        DestroyUdmfRecord(&udmf_record);
        records[i] = nullptr;
        continue;
      }
      int res = OH_UdmfRecord_GetPlainText(udmf_record, plain_text);
      if (res != UDMF_E_OK) {
        LOG(ERROR) << "[Pasteboard]ReadTextFromPasteBoard "
                      "OH_UdmfRecord_GetPlainText fail,error code:"
                    << res;
        DestroyUdsPlainText(&plain_text);
        DestroyUdmfRecord(&udmf_record);
        records[i] = nullptr;
        continue;
      }
      const char* text = OH_UdsPlainText_GetContent(plain_text);
      if (text == nullptr) {
        LOG(ERROR) << "[Pasteboard]ReadTextFromPasteBoard "
                      "OH_UdsPlainText_GetContent fail";
        continue;
      }
      result->append(base::UTF8ToUTF16(text));
      DestroyUdsPlainText(&plain_text);
      DestroyUdmfRecord(&udmf_record);
      records[i] = nullptr;
    }
  }
  void ReadHtmlFromPasteBoard(std::u16string* result) {
    if (!result) {
      LOG(ERROR) << "ReadHtmlFromPasteBoard result is null";
      return;
    }
    int status = -1;
    if (!OH_Pasteboard_HasData(pasteboard_)) {
        LOG(ERROR) << kPasteboardLogTag
        << " ReadHtmlFromPasteBoard OH_Pasteboard_HasData fail.";
        return;
    }
    OH_UdmfData* udmf_data = OH_Pasteboard_GetData(pasteboard_, &status);
    if (status != ERR_OK) {
        LOG(ERROR) << kPasteboardLogTag
        << " ReadHtmlFromPasteBoard OH_Pasteboard_GetData fail,error code:"
        << status;
        return;
    }
    unsigned int count = 0;
    OH_UdmfRecord** records = OH_UdmfData_GetRecords(udmf_data, &count);
    if (records == nullptr) {
      LOG(ERROR) << "[Pasteboard]ReadHtmlFromPasteBoard "
                    "OH_UdmfData_GetRecords fail ";
      return;
    }
    for (unsigned int i = 0; i < count; i++) {
      OH_UdmfRecord* udmf_record = records[i];
      if (udmf_record == nullptr) {
        LOG(ERROR)
            << "[Pasteboard]ReadHtmlFromPasteBoard udmf_record is null";
        continue;
      }
      OH_UdsHtml* uds_html = OH_UdsHtml_Create();
      if (uds_html == nullptr) {
        LOG(ERROR)
            << "[Pasteboard]ReadHtmlFromPasteBoard OH_UdsHtml_Create fail, "
                "uds_html is null";
        DestroyUdmfRecord(&udmf_record);
        records[i] = nullptr;
        continue;
      }
      int res = OH_UdmfRecord_GetHtml(udmf_record, uds_html);
      if (res != UDMF_E_OK) {
        LOG(ERROR) << "[Pasteboard]ReadHtmlFromPasteBoard "
                      "OH_UdmfRecord_GetHtml fail,error code:"
                    << res;
        DestroyUdsHtml(&uds_html);
        DestroyUdmfRecord(&udmf_record);
        records[i] = nullptr;
        continue;
      }
      const char* html = OH_UdsHtml_GetContent(uds_html);
      if (html == nullptr) {
        LOG(ERROR) << "[Pasteboard]ReadHtmlFromPasteBoard "
                      "OH_UdsHtml_GetContent fail ";
        continue;
      }
      result->append(base::UTF8ToUTF16(html));
      DestroyUdsHtml(&uds_html);
      DestroyUdmfRecord(&udmf_record);
      records[i] = nullptr;
    }
  }

  // Reads HTML from the ClipboardData.
  void ReadHTML(std::u16string* markup,
                std::string* src_url,
                uint32_t* fragment_start,
                uint32_t* fragment_end) {
    if (!markup || !src_url || !fragment_start || !fragment_end) {
      LOG(ERROR) << "ReadHTML result is null";
      return;
    }

    markup->clear();
    src_url->clear();
    *fragment_start = 0;
    *fragment_end = 0;

    std::u16string html;
    std::u16string text;
    ReadHtmlFromPasteBoard(&html);
    ReadTextFromPasteBoard(&text);
    if (!html.empty()) {
      *markup = html;
      *fragment_start = 0;
      *fragment_end = static_cast<uint32_t>(markup->length());
    }else if (!text.empty()) {
      *markup = base::UTF8ToUTF16("<span>" + base::UTF16ToASCII(text) + "</span>");
      *fragment_start = 0;
      *fragment_end = static_cast<uint32_t>(markup->length());
    }
  }

  // Reads filenames image from the ClipboardData.
  void ReadFilenames(std::vector<ui::FileInfo>* result) {
    if (!result) {
      LOG(ERROR) << "ReadFilenames result is null";
      return;
    }
    int status = -1;
    if (!OH_Pasteboard_HasData(pasteboard_)) {
        LOG(ERROR) << kPasteboardLogTag
        << " ReadFilenames OH_Pasteboard_HasData fail.";
      return;
    }
    OH_UdmfData* udmf_data = OH_Pasteboard_GetData(pasteboard_, &status);
    if (status != ERR_OK) {
        LOG(ERROR) << kPasteboardLogTag
         << " ReadFilenames OH_Pasteboard_GetData fail,error code:"
         << status;
      return;
    }
    unsigned int count = 0;
    OH_UdmfRecord** records = OH_UdmfData_GetRecords(udmf_data, &count);
    if (records == nullptr) {
      LOG(ERROR) << "[Pasteboard]ReadFilenames OH_UdmfData_GetRecords fail ";
      return;
    }
    std::vector<std::string> file_paths;
    for (unsigned int i = 0; i < count; i++) {
      OH_UdmfRecord* udmf_record = records[i];
      if (udmf_record == nullptr) {
        LOG(ERROR) << "[Pasteboard]ReadFilenames udmf_record is null";
        continue;
      }
      OH_UdsFileUri* uds_file_uri = OH_UdsFileUri_Create();
      if (uds_file_uri == nullptr) {
        LOG(ERROR) << "[Pasteboard]ReadFilenames OH_UdsFileUri_Create fail, "
                      "uds_file_uri is null";
        DestroyUdmfRecord(&udmf_record);
        records[i] = nullptr;
        continue;
      }
      int res = OH_UdmfRecord_GetFileUri(udmf_record, uds_file_uri);
      if (res != UDMF_E_OK) {
        LOG(ERROR) << "[Pasteboard]ReadFilenames OH_UdmfRecord_GetFileUri "
                      "fail,error code:"
                   << res;
        DestroyUdsFileUri(&uds_file_uri);
        DestroyUdmfRecord(&udmf_record);
        records[i] = nullptr;
        continue;
      }
      const char* file_uri = OH_UdsFileUri_GetFileUri(uds_file_uri);
      if (file_uri == nullptr) {
        LOG(ERROR) << "[Pasteboard]ReadFilenames OH_UdsFileUri_GetFileUri "
                      "fail, file_uri is null";
        DestroyUdsFileUri(&uds_file_uri);
        DestroyUdmfRecord(&udmf_record);
        records[i] = nullptr;
        continue;
      }
      std::string file_path;
      ohos::adapter::FileManagerAdapter::GetInstance().GetPathForUri(file_uri,
                                                                     file_path);
      if (file_path.empty()) {
        LOG(ERROR) << "[Pasteboard]ReadFilenames GetPathForUri fail, "
                      "file_path is null";
        DestroyUdsFileUri(&uds_file_uri);
        DestroyUdmfRecord(&udmf_record);
        records[i] = nullptr;
        continue;
      }
      file_paths.push_back(file_path);
      DestroyUdsFileUri(&uds_file_uri);
      DestroyUdmfRecord(&udmf_record);
      records[i] = nullptr;
    }
    for (const std::string& file_path : file_paths) {
      url::RawCanonOutputW<kMaxUriDecodeLen> unescaped_path;
      url::DecodeURLEscapeSequences(file_path.c_str(),
                                    url::DecodeURLMode::kUTF8OrIsomorphic,
                                    &unescaped_path);
      std::string decode_file_path = base::UTF16ToUTF8(
          std::u16string_view(unescaped_path.data(), unescaped_path.length()));
      result->emplace_back(base::FilePath(FILE_PATH_LITERAL(decode_file_path)),
                           base::FilePath());
    }
  }
void PrepareImageInfoForRead(OH_UdsPixelMap*& uds_pixel_map,
                             OH_PixelmapNative*& pixelmap_native,
                             OH_Pixelmap_ImageInfo*& image_info) {
  int option_width = 1;
  int option_height = 1;
  OH_Pixelmap_InitializationOptions* pixelmap_option;
  OH_PixelmapInitializationOptions_Create(&pixelmap_option);
  OH_PixelmapInitializationOptions_SetWidth(pixelmap_option, option_width);
  OH_PixelmapInitializationOptions_SetHeight(pixelmap_option, option_height);
  OH_PixelmapInitializationOptions_SetPixelFormat(pixelmap_option,
                                                  +PIXEL_FORMAT_BGRA_8888);
  OH_PixelmapInitializationOptions_SetAlphaType(pixelmap_option,
                                                +PIXELMAP_ALPHA_TYPE_OPAQUE);
  Image_ErrorCode err_code =
      OH_PixelmapNative_CreateEmptyPixelmap(pixelmap_option, &pixelmap_native);
  if (err_code != IMAGE_SUCCESS) {
    LOG(ERROR) << "[Pasteboard]ReadPng OH_PixelmapNative_CreateEmptyPixelmap "
                  "fail,error code:"
               << err_code;
    OH_PixelmapInitializationOptions_Release(pixelmap_option);
    return;
  }
  OH_PixelmapInitializationOptions_Release(pixelmap_option);
  OH_UdsPixelMap_GetPixelMap(uds_pixel_map, pixelmap_native);
  if (pixelmap_native == nullptr) {
    LOG(ERROR) << "[Pasteboard]ReadPng OH_UdsPixelMap_GetPixelMap fail, "
                  "pixelmap_native is null";
    return;
  }
  int res = OH_PixelmapImageInfo_Create(&image_info);
  if (res != IMAGE_SUCCESS) {
    LOG(ERROR) << "[Pasteboard]ReadPng OH_PixelmapImageInfo_Create "
                  "fail,error code:"
               << res;
    return;
  }
  res = OH_PixelmapNative_GetImageInfo(pixelmap_native, image_info);
  if (res != IMAGE_SUCCESS) {
    LOG(ERROR) << "[Pasteboard]ReadPng OH_PixelmapNative_GetImageInfo "
                  "fail,error code:"
               << res;
    res = OH_PixelmapImageInfo_Release(image_info);
    if (res != UDMF_E_OK) {
      LOG(ERROR) << "[Pasteboard]ReadPng OH_PixelmapImageInfo_Release "
                    "fail,error code:"
                 << res;
    }
  }
}

void PrepareImgBufferForRead(OH_Pixelmap_ImageInfo*& image_info,
                             OH_PixelmapNative*& pixelmap_native,
                             std::vector<uint8_t>& buff_data) {
  uint32_t width = 0;
  int res = OH_PixelmapImageInfo_GetWidth(image_info, &width);
  if (res != IMAGE_SUCCESS) {
    LOG(ERROR) << "[Pasteboard]ReadPng OH_PixelmapImageInfo_GetWidth "
                  "fail,error code:"
               << res;
  }
  uint32_t height = 0;
  res = OH_PixelmapImageInfo_GetHeight(image_info, &height);
  if (res != IMAGE_SUCCESS) {
    LOG(ERROR) << "[Pasteboard]ReadPng OH_PixelmapImageInfo_GetHeight "
                  "fail,error code:"
               << res;
  }
  size_t data_size = height * width * 4;
  std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(data_size);
  uint32_t* pixel_data = nullptr;
  res = OH_PixelmapNative_ReadPixels(pixelmap_native, data.get(), &data_size);
  if (res != IMAGE_SUCCESS) {
    LOG(ERROR) << "[Pasteboard]ReadPng OH_PixelmapNative_ReadPixels "
                  "fail,error code:"
               << res;
  } else {
    pixel_data = reinterpret_cast<uint32_t*>(data.get());
  }

  uint32_t row_stride = 0;
  res = OH_PixelmapImageInfo_GetRowStride(image_info, &row_stride);
  if (res != IMAGE_SUCCESS) {
    LOG(ERROR) << "[Pasteboard]ReadPng OH_PixelmapImageInfo_GetRowStride"
                  " fail,error code:"
               << res;
  }
  int32_t pixel_format = 0;
  res = OH_PixelmapImageInfo_GetPixelFormat(image_info, &pixel_format);
  if (res != IMAGE_SUCCESS) {
    LOG(ERROR) << "[Pasteboard]ReadPng OH_PixelmapImageInfo_GetPixelFormat"
                  " fail,error code:"
               << res;
  }
  res = OH_PixelmapImageInfo_Release(image_info);
  if (res != UDMF_E_OK) {
    LOG(ERROR) << "[Pasteboard]ReadPng OH_PixelmapImageInfo_Release "
                  "fail,error code:"
               << res;
  }


  SkColorType sk_color_type = PixelFormatToSkColorType(
      static_cast<ui::ClipBoardImageColorType>(pixel_format));
  // The image pixel data is pre-multiplied according to the alpha type,
  // and the RGB data is no longer processed according to the alpha.
  SkAlphaType sk_alpha_type = SkAlphaType::kPremul_SkAlphaType;
  sk_sp<SkColorSpace> color_space = SkColorSpace::MakeSRGB();
  SkImageInfo sk_image_info = SkImageInfo::Make(width, height, sk_color_type,
                                                sk_alpha_type, color_space);
  SkBitmap img;
  SkPixmap pixmap(sk_image_info, pixel_data, row_stride);
  if (!img.installPixels(pixmap)) {
    LOG(ERROR) << "[Pasteboard]installPixels failed";
  } else {
    buff_data = clipboard_util::EncodeBitmapToPng(img);
    LOG(INFO) << kPasteboardLogTag << __FUNCTION__
              << " PrepareImgBufferForRead buff_data size:"
              << buff_data.size();
  }
}

  // Reads image from the ClipboardData.
  void ReadPng(Clipboard::ReadPngCallback callback) {
    if (!IsFormatAvailable(ClipboardInternalFormat::kPng)) {
      LOG(ERROR) << "[Pasteboard]ReadPng no bitMap format in pasteboard";
      std::move(callback).Run(std::vector<uint8_t>());
      return;
    }
    int status = -1;
    if (!OH_Pasteboard_HasData(pasteboard_)) {
        LOG(ERROR) << kPasteboardLogTag
        << " ReadPng OH_Pasteboard_HasData fail.";
        return;
    }
    OH_UdmfData* udmf_data = OH_Pasteboard_GetData(pasteboard_, &status);
    if (status != ERR_OK) {
        LOG(ERROR) << kPasteboardLogTag
        << " ReadPng OH_Pasteboard_GetData fail,error code:"
        << status;
        return;
    }
    unsigned int count = 0;
    OH_UdmfRecord** records = OH_UdmfData_GetRecords(udmf_data, &count);
    if (records == nullptr) {
      LOG(ERROR) << "[Pasteboard]ReadPng OH_UdmfData_GetRecords fail";
      return;
    }
    for (unsigned int i = 0; i < count; i++) {
      OH_UdmfRecord* udmf_record = records[i];
      if (udmf_record == nullptr) {
        LOG(ERROR) << "[Pasteboard]ReadPng udmf_record is null";
        continue;
      }
      std::vector<uint8_t> buff_data;
      int res = ERR_INNER_ERROR;
      if (DataTypeExist(udmf_record, UDMF_META_OPENHARMONY_PIXEL_MAP)) {
        res = GetPixelMapBuffData(udmf_record, buff_data);
      } else if (DataTypeExist(udmf_record, kImageBmp)) {
        res = GetBmpBuffData(udmf_record, buff_data);
      }
      DestroyUdmfRecord(&udmf_record);
      records[i] = nullptr;
      if (res == 0) {
        std::move(callback).Run(std::move(buff_data));
        break;
      } else {
        continue;
      }
    }
  }
 
  int GetPixelMapBuffData(OH_UdmfRecord* udmf_record, std::vector<uint8_t>& buff_data) {
    OH_UdsPixelMap* uds_pixel_map = OH_UdsPixelMap_Create();
    if (uds_pixel_map == nullptr) {
      LOG(ERROR) << kPasteboardLogTag << __FUNCTION__
                 <<" OH_UdsPixelMap_Create fail, "
                    "uds_pixel_map is null";
      return ERR_INNER_ERROR;
    }
    int res = OH_UdmfRecord_GetPixelMap(udmf_record, uds_pixel_map);
    if (res != UDMF_E_OK) {
      LOG(ERROR) << kPasteboardLogTag << __FUNCTION__
                 << " OH_UdmfRecord_GetPixelMap "
                    "fail,error code:"
                  << res;
      DestroyUdsPixelMap(&uds_pixel_map);
      return res;
    }
    OH_PixelmapNative* pixelmap_native = nullptr;
    OH_Pixelmap_ImageInfo* image_info = nullptr;
    PrepareImageInfoForRead(uds_pixel_map, pixelmap_native, image_info);
    if (!image_info) {
      LOG(ERROR) << "[Pasteboard]ReadPng PrepareImageInfoForRead "
                    "fail, image_info is null";
      DestroyUdsPixelMap(&uds_pixel_map);
      return ERR_INNER_ERROR;
    }
    DestroyUdsPixelMap(&uds_pixel_map);
    PrepareImgBufferForRead(image_info, pixelmap_native, buff_data);
    return ERR_OK;
  }

  int GetBmpBuffData(OH_UdmfRecord* udmf_record, std::vector<uint8_t>& buff_data) {
    unsigned char* entrys;
    unsigned int entry_count;
    int res = OH_UdmfRecord_GetGeneralEntry(udmf_record, kImageBmp, &entrys,
                                            &entry_count);
    if (res != UDMF_E_OK) {
      LOG(ERROR) << "[Pasteboard]OH_UdmfRecord_GetGeneralEntry get bmp failed, error code is:"
                << res;
      return res;
    }
    buff_data.reserve(entry_count);
    for (size_t j = 0; j < entry_count; j++) {
      buff_data.push_back(static_cast<uint8_t>(entrys[j]));
    }
    return ERR_OK;
  }

  bool DataTypeExist(OH_UdmfRecord* udmf_record, const char* data_type) {
    unsigned int type_count;
    char** types = OH_UdmfRecord_GetTypes(udmf_record, &type_count);
    if (types == nullptr || type_count == 0) {
      return false;
    }
    for (unsigned int j = 0; j < type_count; j++) {
      if (strcmp(types[j], data_type) == 0) {
        return true;
      }
    }
    return false;
  }

  void ReadData(const std::string& type, std::string* result) const {
    if (!result) {
      LOG(ERROR) << "ReadData result is null";
      return;
    }
    result->clear();
    int status = -1;
    if (!OH_Pasteboard_HasData(pasteboard_)) {
        LOG(ERROR) << kPasteboardLogTag
        << " ReadData OH_Pasteboard_HasData fail.";
        return;
    }
    OH_UdmfData* udmf_data = OH_Pasteboard_GetData(pasteboard_, &status);
    if (status != ERR_OK) {
        LOG(ERROR) << kPasteboardLogTag
        << " ReadData OH_Pasteboard_GetData fail,error code:"
        << status;
        return;
    }
    unsigned int count = 0;
    OH_UdmfRecord** records = OH_UdmfData_GetRecords(udmf_data, &count);
    if (records == nullptr) {
      LOG(ERROR) << "[Pasteboard]ReadData OH_UdmfData_GetRecords fail ";
      return;
    }
    for (unsigned int i = 0; i < count; i++) {
      OH_UdmfRecord* udmf_record = records[i];
      if (udmf_record == nullptr) {
        LOG(ERROR) << "[Pasteboard]ReadData udmf_record is null";
        continue;
      }
      const char* type_id = type.c_str();
      unsigned char* entrys;
      unsigned int entry_count;
      int res = OH_UdmfRecord_GetGeneralEntry(udmf_record, type_id, &entrys,
                                              &entry_count);
      if (res != UDMF_E_OK) {
        LOG(ERROR) << "[Pasteboard]ReadData OH_UdmfRecord_GetGeneralEntry "
                      "fail,error code:"
                    << res;
        DestroyUdmfRecord(&udmf_record);
        records[i] = nullptr;
        continue;
      }
      *result =
          std::string(reinterpret_cast<const char*>(entrys), entry_count);
      DestroyUdmfRecord(&udmf_record);
      records[i] = nullptr;
      break;
    }
  }

  void WriteHtmlToRecord(const char* html_text, OH_UdmfRecord* record) {
    OH_UdsHtml* uds_html = OH_UdsHtml_Create();
    if (uds_html == nullptr) {
      LOG(ERROR) << "[Pasteboard]WriteHtmlToRecord OH_UdsHtml_Create fail, "
                    "uds_html is null";
      return;
    }
    int res = OH_UdsHtml_SetContent(uds_html, html_text);
    if (res != UDMF_E_OK) {
      LOG(ERROR) << "[Pasteboard]WriteHtmlToRecord OH_UdsHtml_SetContent "
                    "failed,error code is :"
                 << res;
      DestroyUdsHtml(&uds_html);
      return;
    }
    res = OH_UdmfRecord_AddHtml(record, uds_html);
    if (res != UDMF_E_OK) {
      LOG(ERROR) << "[Pasteboard]WriteHtmlToRecord OH_UdmfRecord_AddHtml "
                    "failed,error code is :"
                 << res;
      DestroyUdsHtml(&uds_html);
      return;
    }
    DestroyUdsHtml(&uds_html);
  }

  void WriteTextToRecord(const char* text, OH_UdmfRecord* record) {
    OH_UdsPlainText* uds_plain_text = OH_UdsPlainText_Create();
    if (uds_plain_text == nullptr) {
      LOG(ERROR) << "[Pasteboard]WriteTextToRecord OH_UdsPlainText_Create "
                    "fail, uds_plain_text is null";
      return;
    }
    int res = OH_UdsPlainText_SetContent(uds_plain_text, text);
    if (res != UDMF_E_OK) {
      LOG(ERROR) << "[Pasteboard]WriteTextToRecord OH_UdsPlainText_SetContent "
                    "failed,error code is :"
                 << res;
      DestroyUdsPlainText(&uds_plain_text);
      return;
    }
    res = OH_UdmfRecord_AddPlainText(record, uds_plain_text);
    if (res != UDMF_E_OK) {
      LOG(ERROR) << "[Pasteboard]WriteTextToRecord OH_UdmfRecord_AddPlainText "
                    "failed,error code is :"
                 << res;
      DestroyUdsPlainText(&uds_plain_text);
      return;
    }
    DestroyUdsPlainText(&uds_plain_text);
  }
  void WriteCustomDataToRecord(const CustomDataMap& custom_datas,
                               OH_UdmfRecord* record) {
    for (auto it = custom_datas.begin(); it != custom_datas.end(); ++it) {
      const ClipboardFormatType format_type = it->first;
      std::string custom_data = it->second;
      unsigned int count = custom_data.length();
      unsigned char* entry =
          reinterpret_cast<unsigned char*>(custom_data.data());
      int entry_res = OH_UdmfRecord_AddGeneralEntry(
          record, format_type.GetName().c_str(), entry, count);
      if (entry_res != UDMF_E_OK) {
        LOG(ERROR) << "[Pasteboard]WriteData OH_UdmfRecord_AddGeneralEntry "
                      "failed,code is :"
                  << entry_res;
      }
    }
  }

  // Writes |data| to the ClipboardData and returns the previous data.
  std::unique_ptr<ClipboardData> WriteData(
      std::unique_ptr<ClipboardData> data) {
    DCHECK(data);
    OH_UdmfRecord* udmf_record = OH_UdmfRecord_Create();
    if (udmf_record == nullptr) {
      LOG(ERROR) << "[Pasteboard]WriteData OH_UdmfRecord_Create fail, "
                    "udmf_record is null";
      return nullptr;
    }
    std::unique_ptr<ClipboardData> previous_data = std::move(data_);
    data_ = std::move(data);
    const ClipboardData* current_data = GetData();
    if (!current_data) {
      return nullptr;
    }

    if (HasFormat(ClipboardInternalFormat::kPng)) {
      const std::optional<SkBitmap>& optional_bitmap =
          current_data->GetBitmapIfPngNotEncoded();
      if (optional_bitmap.has_value()) {
        WriteBitmapToRecord(*optional_bitmap, udmf_record);
      }
    }

    if (HasFormat(ClipboardInternalFormat::kText)) {
      const char* text = current_data->text().c_str();
      WriteTextToRecord(text, udmf_record);
    }

    if (HasFormat(ClipboardInternalFormat::kHtml)) {
      const char* html_text = current_data->markup_data().c_str();
      WriteHtmlToRecord(html_text, udmf_record);
    }

    if (HasFormat(ClipboardInternalFormat::kCustom)) {
      const CustomDataMap& custom_datas = current_data->GetAllCustomData();
      WriteCustomDataToRecord(custom_datas, udmf_record);
    }

    WritePasteboard(udmf_record);
    DestroyUdmfRecord(&udmf_record);
    sequence_number_ = ClipboardSequenceNumberToken();
    return previous_data;
  }

  bool IsReadAllowed(const DataTransferEndpoint* data_dst,
                     std::optional<ClipboardInternalFormat> format,
                     const std::optional<ClipboardFormatType>&
                         custom_data_format = std::nullopt) {
    const bool show = data_dst ? data_dst->truely_transfer(): true;
    // check system level permission first
    if (!ohos_permission::PermissionManagerAdapter::CheckAndRequestPermission(
        ohos_permission::OHOSPermissionType::PASTEBOARD)) {
        LOG(INFO)
            << kPasteboardLogTag
            << "ClipboardOHOSInternal::RequestPasteBoardPermissionCallback: "
            << "request "
            << "PASTEBOARD permission result -> The system rejects the request.";
        if (!promptDialogOpened_ && show) {
          promptDialogOpened_ = true;
          base::ThreadPool::PostTaskAndReplyWithResult(
              FROM_HERE, base::MayBlock(),
              base::BindOnce(
                  &ohos_permission::PermissionManagerAdapter::
                      OpenPermissionConfirm,
                  ohos::adapter::permission::OHOSPermissionType::PASTEBOARD),
              base::BindOnce(
                  &ClipboardOHOSInternal::OpenConfirmDialogCallback,
                  weak_factory_.GetWeakPtr()));
        }
        return false;
    }
    LOG(INFO)
        << kPasteboardLogTag << __FUNCTION__
        << " system level PASTEBOARD permission"
        << " request success.";
    return true;
  }

  void OpenConfirmDialogCallback(bool replySuccess) {
    promptDialogOpened_ = false;
  }

  void ReleasePixelmapOption(OH_Pixelmap_InitializationOptions*& options) {
    if (!options) {
      return;
    }
    int res = OH_PixelmapInitializationOptions_Release(options);
    if (res != IMAGE_SUCCESS) {
      LOG(ERROR) << "WriteBitmap OH_PixelmapInitializationOptions_Release "
                    "failed,error code is :"
                 << res;
    }
  }

  void ReleasePixelmapNative(OH_PixelmapNative*& pixelmap_native) {
    if (!pixelmap_native) {
      return;
    }
    int res = OH_PixelmapNative_Release(pixelmap_native);
    if (res != IMAGE_SUCCESS) {
      LOG(ERROR) << "WriteBitmap OH_PixelmapInitializationOptions_Release "
                    "failed,error code is :"
                 << res;
    }
  }

  bool PreparePixelmapOptionForWrite(
      const SkBitmap& bitmap,
      OH_Pixelmap_InitializationOptions* options) {
    if (options == nullptr) {
      LOG(ERROR) << "[Pasteboard] " << __FUNCTION__ << " options is null";
      return false;
    }
    uint32_t width = static_cast<uint32_t>(bitmap.width());
    int res = OH_PixelmapInitializationOptions_SetWidth(options, width);
    if (res != IMAGE_SUCCESS) {
      LOG(ERROR) << "WriteBitmap OH_PixelmapInitializationOptions_SetWidth "
                    "failed,error code is :"
                 << res;
      return false;
    }
    uint32_t height = static_cast<uint32_t>(bitmap.height());
    res = OH_PixelmapInitializationOptions_SetHeight(options, height);
    if (res != IMAGE_SUCCESS) {
      LOG(ERROR) << "WriteBitmap OH_PixelmapInitializationOptions_SetHeight "
                    "failed,error code is :"
                 << res;
      return false;
    }
    ui::ClipBoardImageAlphaType bitmap_alpha_type =
        ImageToClipboardAlphaType(bitmap.alphaType());
    int32_t alpha_type = static_cast<int32_t>(bitmap_alpha_type);
    res = OH_PixelmapInitializationOptions_SetAlphaType(options, alpha_type);
    if (res != IMAGE_SUCCESS) {
      LOG(ERROR) << "WriteBitmap OH_PixelmapInitializationOptions_SetAlphaType "
                    "failed,error code is :"
                 << res;
      return false;
    }
    ui::ClipBoardImageColorType color_type =
        ImageToClipboardColorType(bitmap.colorType());
    int32_t pixel_format = static_cast<int32_t>(color_type);
    res = OH_PixelmapInitializationOptions_SetSrcPixelFormat(options,
                                                             pixel_format);
    if (res != IMAGE_SUCCESS) {
      LOG(ERROR)
          << "WriteBitmap OH_PixelmapInitializationOptions_SetSrcPixelFormat "
             "failed,error code is :"
          << res;
      return false;
    }
    return true;
  }

  bool PreparePixelmapForWrite(const SkBitmap& bitmap,
                               OH_Pixelmap_InitializationOptions* options,
                               OH_UdsPixelMap* uds_pixel_map) {
    if (uds_pixel_map == nullptr || options == nullptr) {
      LOG(ERROR) << "[Pasteboard] " << __FUNCTION__
                 << " uds_pixel_map or options is null";
      return false;
    }
    uint8_t* pixel_data = reinterpret_cast<uint8_t*>(bitmap.getPixels());
    uint64_t stride = static_cast<uint64_t>(bitmap.width()) << 2;
    size_t buffer_size =
        static_cast<size_t>(stride * static_cast<uint64_t>(bitmap.height()));
    OH_PixelmapNative* pixelmap_native = nullptr;
    int res = OH_PixelmapNative_CreatePixelmap(pixel_data, buffer_size, options,
                                               &pixelmap_native);
    if (res != IMAGE_SUCCESS) {
      LOG(ERROR) << "WriteBitmap OH_PixelmapNative_CreatePixelmap failed,error "
                    "code is :"
                 << res;
      return false;
    }
    res = OH_UdsPixelMap_SetPixelMap(uds_pixel_map, pixelmap_native);
    if (res != IMAGE_SUCCESS) {
      LOG(ERROR)
          << "WriteBitmap OH_UdsPixelMap_SetPixelMap failed,error code is :"
          << res;
      ReleasePixelmapNative(pixelmap_native);
      return false;
    }
    ReleasePixelmapNative(pixelmap_native);
    return true;
  }

  void WriteBitmapToRecord(const SkBitmap& bitmap, OH_UdmfRecord* record) {
    OH_Pixelmap_InitializationOptions* options = nullptr;
    int res = OH_PixelmapInitializationOptions_Create(&options);
    if (res != IMAGE_SUCCESS) {
      LOG(ERROR) << "WriteBitmap OH_PixelmapInitializationOptions_Create "
                    "failed,error code is:"
                 << res;
      return;
    }
    bool ret = PreparePixelmapOptionForWrite(bitmap, options);
    if (!ret) {
      LOG(ERROR) << "WriteBitmap PreparePixelmapOptionForWrite "
                    "failed, options is null";
      ReleasePixelmapOption(options);
      return;
    }
    OH_PixelmapNative* pixelmap_native = nullptr;
    OH_UdsPixelMap* uds_pixel_map = OH_UdsPixelMap_Create();
    if (uds_pixel_map == nullptr) {
      LOG(ERROR) << "[Pasteboard]WriteBitmap OH_UdsPixelMap_Create fail, "
                    "uds_pixel_map is null";
      return;
    }
    ret = PreparePixelmapForWrite(bitmap, options, uds_pixel_map);
    if (!ret) {
      LOG(ERROR) << "WriteBitmap PreparePixelmapForWrite "
                    "failed, uds_pixel_map is null";
      ReleasePixelmapOption(options);
      DestroyUdsPixelMap(&uds_pixel_map);
      return;
    }

    res = OH_UdmfRecord_AddPixelMap(record, uds_pixel_map);
    if (res != UDMF_E_OK) {
      LOG(ERROR)
          << "WriteBitmap OH_UdmfRecord_AddPixelMap failed,error code is :"
          << res;
      ReleasePixelmapOption(options);
      DestroyUdsPixelMap(&uds_pixel_map);
      return;
    }
    ReleasePixelmapOption(options);
    DestroyUdsPixelMap(&uds_pixel_map);
  }

 private:
  // True if the ClipboardData has format |format|.
  bool HasFormat(ClipboardInternalFormat format) const {
    const ClipboardData* data = GetData();
    return data ? data->format() & static_cast<int>(format) : false;
  }

  // Current ClipboardData.
  std::unique_ptr<ClipboardData> data_;

  // Sequence number uniquely identifying clipboard state.
  ClipboardSequenceNumberToken sequence_number_;
  RAW_PTR_EXCLUSION OH_Pasteboard* pasteboard_;
  bool promptDialogOpened_ = false;

  // Mapping of OS-provided sequence number to a unique token.
  mutable struct {
    uint32_t sequence_number;
    ClipboardSequenceNumberToken token;
  } clipboard_sequence_;
  ohos::adapter::common::SharedLibrary pasteboard_lib_;
  PasteboardGetChangeCountFunc* pasteboard_get_change_count_func_ = nullptr;

  base::WeakPtrFactory<ClipboardOHOSInternal> weak_factory_{this};
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

  static void WriteData(const ClipboardFormatType& format,
                        base::span<const uint8_t> data) {
    ClipboardData* clipboard_data = GetCurrentData();
    clipboard_data->SetCustomData(
        format,
        std::string(reinterpret_cast<const char*>(data.data()), data.size()));
  }

  static void WriteBitmap(const SkBitmap& bitmap) {
    ClipboardData* data = GetCurrentData();
    if (data) {
      data->SetBitmapData(bitmap);
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
  LOG(INFO) << kPasteboardLogTag << __FUNCTION__;
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
  LOG(INFO) << kPasteboardLogTag << __FUNCTION__ << " start, format is " << format.GetName();
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

  types->clear();
  *types = GetStandardFormats(buffer, data_dst);

  if (clipboard_internal_->GetData()) {
    const auto& custom_data =
        clipboard_internal_->GetData()->GetDataTransferCustomData();
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
  clipboard_internal_->ReadTextFromPasteBoard(result);
  LOG(INFO) << kPasteboardLogTag << __FUNCTION__ << " end, text-length:" << result->length();
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
  LOG(INFO) << kPasteboardLogTag << __FUNCTION__ << " end, markup-length:" << markup->length();
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
  LOG(INFO) << kPasteboardLogTag << __FUNCTION__;
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
  LOG(INFO) << kPasteboardLogTag << __FUNCTION__ << " end, result_size is " << result->size();
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
  return false;
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
  LOG(INFO) << kPasteboardLogTag << __FUNCTION__ << ", text-length:" << text.length();
  ClipboardDataBuilderOhos::WriteText(text);
}

void ClipboardOHOS::WriteHTML(std::string_view markup,
                              std::optional<std::string_view> source_url) {
  LOG(INFO) << kPasteboardLogTag
            << __FUNCTION__
            << ", markup_len:" << markup.length()
            << ", url_len:" << source_url->length();
  ClipboardDataBuilderOhos::WriteHTML(markup, source_url);
}

void ClipboardOHOS::WriteSvg(std::string_view markup) {}

void ClipboardOHOS::WriteRTF(std::string_view rtf) {}

void ClipboardOHOS::WriteFilenames(std::vector<ui::FileInfo> filenames) {}

void ClipboardOHOS::WriteBookmark(std::string_view title,
                                  std::string_view url) {}

void ClipboardOHOS::WriteWebSmartPaste() {}

void ClipboardOHOS::WriteBitmap(const SkBitmap& bitmap) {
  LOG(INFO) << kPasteboardLogTag << __FUNCTION__;
  ClipboardDataBuilderOhos::WriteBitmap(bitmap);
}

void ClipboardOHOS::WriteData(const ClipboardFormatType& format,
                              base::span<const uint8_t> data) {
  LOG(INFO) << kPasteboardLogTag << __FUNCTION__ << " data_len is " << data.size();
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
