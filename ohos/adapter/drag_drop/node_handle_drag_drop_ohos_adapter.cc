/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "ohos/adapter/drag_drop/node_handle_drag_drop_ohos_adapter.h"

#include <arkui/native_node.h>
#include <database/udmf/udmf_err_code.h>
#include <database/udmf/udmf_meta.h>
#include <multimedia/image_framework/image/image_common.h>
#include <multimedia/image_framework/image/image_native.h>
#include <multimedia/image_framework/image/pixelmap_native.h>
#include <multimedia/image_framework/image_pixel_map_mdk.h>

#include "ohos/adapter/device_info/device_info.h"
#include "ohos/adapter/file_manager/file_manager_adapter.h"
#include "ohos/adapter/node_handle/node_handle_impl.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"

namespace {
const int kImagePixelmap = 4;
const std::string kBookmarkDefinedType = "ApplicationDefinedType-bookmark";
const std::string kWebCustomDefinedType = "ApplicationDefinedType-webCustom";
const std::string kFileDragDefaultName = "/storage/User/currentUser/file.png";
const std::string kHyperlinkDragDefaultUrl = "https://www.exampleholder.com";
const std::string khyperlinkDragDefaultTitle = "ExampleHolder";
const std::string kPlainTextDragDefault = "ExamplePlainText";
const int kMaxDataTypeLength = 128;
const char* kNodeHandleDragTag = "[OhosDragNodeHandle]";
}  // namespace

namespace ohos {
namespace adapter {
using ohos::adapter::xcomponent::XComponentManager;
using ohos::adapter::FileManagerAdapter;
std::string NodeHandleDragDropOhosAdapter::window_id_ = "";

NodeHandleDragDropOhosAdapter::~NodeHandleDragDropOhosAdapter() {
  ClearDragRecords();
}

NodeHandleDragDropOhosAdapter& NodeHandleDragDropOhosAdapter::GetInstance() {
  static NodeHandleDragDropOhosAdapter adapter;
  return adapter;
}

bool NodeHandleDragDropOhosAdapter::PrepareDragActionOptions(
    std::shared_ptr<OhosStartDragParam> drag_param) {
  preview_options_ = OH_ArkUI_CreateDragPreviewOption();
  if (preview_options_ == nullptr) {
    LOGE(
        "%{public}s %{public}s, preview option create fail, "
        "window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
    ClearDragActionResource();
    return false;
  }
  int err_code = OH_ArkUI_DragPreviewOption_SetScaleMode(
      preview_options_, ARKUI_DRAG_PREVIEW_SCALE_DISABLED);
  if (err_code != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE(
        "%{public}s %{public}s, set scale mode fail, "
        "code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    ClearDragActionResource();
    return false;
  }
  if ((err_code = OH_ArkUI_DragAction_SetDragPreviewOption(
           drag_action_, preview_options_)) != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE(
        "%{public}s %{public}s, set preview option fail, code:%{public}d,"
        "window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    ClearDragActionResource();
    return false;
  }
  if ((err_code = OH_ArkUI_DragAction_SetPointerId(drag_action_, 0)) !=
      ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE(
        "%{public}s %{public}s, set pointer id fail, code:%{public}d,"
        "window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    ClearDragActionResource();
    return false;
  }
  if ((err_code = OH_ArkUI_DragAction_SetTouchPointX(
           drag_action_, drag_param->pixelmap_touch_x)) !=
      ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE(
        "%{public}s %{public}s, set touch point x fail, "
        "code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    ClearDragActionResource();
    return false;
  }
  if ((err_code = OH_ArkUI_DragAction_SetTouchPointY(
           drag_action_, drag_param->pixelmap_touch_y)) !=
      ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE(
        "%{public}s %{public}s, set touch point y fail, code:%{public}d,"
        "window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    ClearDragActionResource();
    return false;
  }
  return true;
}

bool NodeHandleDragDropOhosAdapter::PreparePixelmapOptions(
    std::shared_ptr<OhosStartDragParam> drag_param) {
  int err_code =
      OH_PixelmapInitializationOptions_Create(&pixelmap_initial_options_);
  if (err_code != IMAGE_SUCCESS || pixelmap_initial_options_ == nullptr) {
    LOGE(
        "%{public}s %{public}s, options create fail, "
        "code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    ClearDragActionResource();
    return false;
  }
  err_code = OH_PixelmapInitializationOptions_SetWidth(
      pixelmap_initial_options_, drag_param->pixelmap_width);
  if (err_code != IMAGE_SUCCESS) {
    LOGE(
        "%{public}s %{public}s , set width fail, "
        "code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    ClearDragActionResource();
    return false;
  }
  err_code = OH_PixelmapInitializationOptions_SetHeight(
      pixelmap_initial_options_, drag_param->pixelmap_height);
  if (err_code != IMAGE_SUCCESS) {
    LOGE(
        "%{public}s %{public}s, set height "
        "fail, code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    ClearDragActionResource();
    return false;
  }
  err_code = OH_PixelmapInitializationOptions_SetPixelFormat(
      pixelmap_initial_options_, PIXEL_FORMAT_BGRA_8888);
  if (err_code != IMAGE_SUCCESS) {
    LOGE(
        "%{public}s %{public}s, set format fail,"
        "code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    ClearDragActionResource();
    return false;
  }
  err_code = OH_PixelmapInitializationOptions_SetAlphaType(
      pixelmap_initial_options_, PIXELMAP_ALPHA_TYPE_UNKNOWN);
  if (err_code != IMAGE_SUCCESS) {
    LOGE(
        "%{public}s %{public}s, set alpha type fail,"
        "code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    ClearDragActionResource();
    return false;
  }
  return true;
}

bool NodeHandleDragDropOhosAdapter::PrepareDragActionPixelmap(
    std::shared_ptr<OhosStartDragParam> drag_param,
    std::vector<OH_PixelmapNative*>& pixel_vector) {
  if (!PreparePixelmapOptions(drag_param)) {
    return false;
  }
  int err_code = OH_PixelmapNative_CreatePixelmap(
      (uint8_t*)drag_param->pixelmap_buffer.get(),
      drag_param->pixelmap_width * drag_param->pixelmap_height * kImagePixelmap,
      pixelmap_initial_options_, &pixelmap_native_);
  if (err_code != IMAGE_SUCCESS) {
    LOGE(
        "%{public}s %{public}s, pixelmap create fail, "
        "code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    ClearDragActionResource();
    return false;
  }
  pixel_vector.push_back(pixelmap_native_);
  err_code = OH_ArkUI_DragAction_SetPixelMaps(drag_action_, pixel_vector.data(),
                                              pixel_vector.size());
  if (err_code != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE(
        "%{public}s %{public}s, set pixelmaps fail, "
        "code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    ClearDragActionResource();
    return false;
  }
  return true;
}

void NodeHandleDragDropOhosAdapter::ClearDragActionResource() {
  if (preview_options_ != nullptr) {
    OH_ArkUI_DragPreviewOption_Dispose(preview_options_);
    preview_options_ = nullptr;
  }
  if (pixelmap_initial_options_ != nullptr) {
    OH_PixelmapInitializationOptions_Release(pixelmap_initial_options_);
    pixelmap_initial_options_ = nullptr;
  }
  if (pixelmap_native_ != nullptr) {
    OH_PixelmapNative_Release(pixelmap_native_);
    pixelmap_native_ = nullptr;
  }
  if (udmf_data_ != nullptr) {
    OH_UdmfData_Destroy(udmf_data_);
    udmf_data_ = nullptr;
  }
  if (drag_action_ != nullptr) {
    OH_ArkUI_DragAction_Dispose(drag_action_);
    drag_action_ = nullptr;
  }
}

void NodeHandleDragDropOhosAdapter::SetSuggestedDropOperation(
    ArkUI_NodeEvent* node_event,
    ArkUI_DragEvent** drag_event) {
  *drag_event = OH_ArkUI_NodeEvent_GetDragEvent(node_event);
  if (drag_event == nullptr || *drag_event == nullptr) {
    LOGE("%{public}s %{public}s get drag_event fail, drag_event is null",
         kNodeHandleDragTag, __FUNCTION__);
    return;
  }
  // When the drag data length is 1 and the operation is MOVE,
  // the drag shadow upper right corner label is not displayed.
  OH_ArkUI_DragEvent_SetSuggestedDropOperation(*drag_event,
                                               ARKUI_DROP_OPERATION_MOVE);
}

void NodeHandleDragDropOhosAdapter::HandlePlainTextRecord(
    std::shared_ptr<OhosStartDragParam> drag_param) {
  if (!drag_param->basic_data.text.empty()) {
    UdsPlainTextPtr plain_text_ptr(OH_UdsPlainText_Create());
    if (plain_text_ptr == nullptr) {
      LOGE(
          "%{public}s %{public}s, plain text create fail, "
          "plain_text is null window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
      return;
    }
    int err_code = OH_UdsPlainText_SetContent(
        plain_text_ptr.get(), drag_param->basic_data.text.c_str());
    if (err_code != UDMF_E_OK) {
      LOGE(
          "%{public}s %{public}s, set content fail,"
          "code:%{public}d, window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
      return;
    }
    OH_UdmfRecord* record = GetUdmfRecordForDragData();
    if (record == nullptr) {
      LOGE(
          "%{public}s %{public}s, get record fail, record "
          "is null, window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
      return;
    }
    err_code = OH_UdmfRecord_AddPlainText(record, plain_text_ptr.get());
    if (err_code != UDMF_E_OK) {
      LOGE(
          "%{public}s %{public}s, add plain text fail,"
          "code:%{public}d, window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
      return;
    }
    if (device_info::DeviceInfo::SdkApi() < device_info::SDK_VERSION_15) {
      drag_records_.push_back(record);
    }
  }
}

void NodeHandleDragDropOhosAdapter::HandleUdsHyperlinkRecord(
    std::shared_ptr<OhosStartDragParam> drag_param) {
  UdsHyperlinkPtr hyperlink_ptr(OH_UdsHyperlink_Create());
  if (hyperlink_ptr == nullptr) {
    LOGE(
        "%{public}s %{public}s, hyperlink create fail, "
        "window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
    return;
  }
  int url_err_code = UDMF_E_INVALID_PARAM;
  if (!drag_param->basic_data.url.empty()) {
    url_err_code = OH_UdsHyperlink_SetUrl(hyperlink_ptr.get(),
                                          drag_param->basic_data.url.c_str());
    if (url_err_code != UDMF_E_OK) {
      LOGE(
          "%{public}s %{public}s, set url fail, "
          "code:%{public}d, window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, url_err_code, window_id_.c_str());
    }
  }
  int url_title_err_code = UDMF_E_INVALID_PARAM;
  if (!drag_param->basic_data.url_title.empty()) {
    url_title_err_code = OH_UdsHyperlink_SetDescription(
        hyperlink_ptr.get(), drag_param->basic_data.url_title.c_str());
    if (url_title_err_code != UDMF_E_OK) {
      LOGE(
          "%{public}s %{public}s, set url title fail,"
          "code:%{public}d, window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, url_title_err_code,
          window_id_.c_str());
    }
  }
  if (url_err_code != UDMF_E_OK && url_title_err_code != UDMF_E_OK) {
    return;
  }
  OH_UdmfRecord* record = GetUdmfRecordForDragData();
  if (record == nullptr) {
    LOGE(
        "%{public}s %{public}s, get udmf record fail, record "
        "is null window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
    return;
  }
  int err_code = OH_UdmfRecord_AddHyperlink(record, hyperlink_ptr.get());
  if (err_code != UDMF_E_OK) {
    LOGE(
        "%{public}s %{public}s, add hyperlink record fail, "
        "code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    return;
  }
  if (device_info::DeviceInfo::SdkApi() < device_info::SDK_VERSION_15) {
    drag_records_.push_back(record);
  }
}

void NodeHandleDragDropOhosAdapter::HandleUdsHtmlRecord(
    std::shared_ptr<OhosStartDragParam> drag_param) {
  if (!drag_param->basic_data.html.empty()) {
    UdsHtmlPtr uds_html_ptr(OH_UdsHtml_Create());
    if (uds_html_ptr == nullptr) {
      LOGE(
          "%{public}s %{public}s, html record create fail, "
          "window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
      return;
    }
    int err_code = OH_UdsHtml_SetContent(uds_html_ptr.get(),
                                         drag_param->basic_data.html.c_str());
    if (err_code != UDMF_E_OK) {
      LOGE(
          "%{public}s %{public}s, set content fail, "
          "code:%{public}d, window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
      return;
    }
    if (!drag_param->basic_data.text.empty()) {
      err_code = OH_UdsHtml_SetPlainContent(
          uds_html_ptr.get(), drag_param->basic_data.text.c_str());
      if (err_code != UDMF_E_OK) {
        LOGE(
            "%{public}s %{public}s, set plain content fail, "
            "code:%{public}d, window_id:%{public}s",
            kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
      }
    }
    OH_UdmfRecord* record = GetUdmfRecordForDragData();
    if (record == nullptr) {
      LOGE(
          "%{public}s %{public}s, get udmf record fail, "
          "window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
      return;
    }
    err_code = OH_UdmfRecord_AddHtml(record, uds_html_ptr.get());
    if (err_code != UDMF_E_OK) {
      LOGE(
          "%{public}s %{public}s, add html record fail, "
          "code:%{public}d, window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
      return;
    }
    if (device_info::DeviceInfo::SdkApi() < device_info::SDK_VERSION_15) {
      drag_records_.push_back(record);
    }
  }
}

void NodeHandleDragDropOhosAdapter::HandleWebImageRecord(
    std::shared_ptr<OhosStartDragParam> drag_param) {
  if (!drag_param->web_image_file_path.empty()) {
    std::string file_uri;
    FileManagerAdapter::GetInstance().GetUriForPath(
        drag_param->web_image_file_path.c_str(), file_uri);
    if (file_uri.empty()) {
      LOGE(
          "%{public}s %{public}s get path for uri fail, "
          "window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
      return;
    }
    UdsFileUriPtr uds_file_uri_ptr(OH_UdsFileUri_Create());
    if (uds_file_uri_ptr == nullptr) {
      LOGE(
          "%{public}s %{public}s, file uri record create fail, "
          "window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
      return;
    }
    int err_code =
        OH_UdsFileUri_SetFileUri(uds_file_uri_ptr.get(), file_uri.c_str());
    if (err_code != UDMF_E_OK) {
      LOGE(
          "%{public}s %{public}s, set file uri fail, "
          "code:%{public}d, window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
      return;
    }
    err_code =
        OH_UdsFileUri_SetFileType(uds_file_uri_ptr.get(), UDMF_META_IMAGE);
    if (err_code != UDMF_E_OK) {
      LOGE(
          "%{public}s %{public}s set file type fail, "
          "code:%{public}d, window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
      return;
    }
    OH_UdmfRecord* record = GetUdmfRecordForDragData();
    if (record == nullptr) {
      LOGE(
          "%{public}s %{public}s, get udmf record fail, "
          "window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
      return;
    }
    err_code = OH_UdmfRecord_AddFileUri(record, uds_file_uri_ptr.get());
    if (err_code != UDMF_E_OK) {
      LOGE(
          "%{public}s %{public}s, add file uri record fail, "
          "code:%{public}d, window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
      return;
    }
    if (device_info::DeviceInfo::SdkApi() < device_info::SDK_VERSION_15) {
      drag_records_.push_back(record);
    }
  }
}

void NodeHandleDragDropOhosAdapter::HandleWebCustomDataRecord(
    std::shared_ptr<OhosStartDragParam> drag_param) {
  if (!drag_param->basic_data.web_custom_data.empty()) {
    OH_UdmfRecord* record = GetUdmfRecordForDragData();
    if (record == nullptr) {
      LOGE(
          "%{public}s %{public}s, get udmf record fail, "
          "record is null window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
      return;
    }
    unsigned char* entry = reinterpret_cast<unsigned char*>(
        drag_param->basic_data.web_custom_data.data());
    unsigned int count = static_cast<unsigned int>(
        drag_param->basic_data.web_custom_data.size());
    int err_code = OH_UdmfRecord_AddGeneralEntry(
        record, kWebCustomDefinedType.c_str(), entry, count);
    if (err_code != UDMF_E_OK) {
      LOGE(
          "%{public}s %{public}s, add generate entry fail, "
          "code:%{public}d, window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
      return;
    }
    if (device_info::DeviceInfo::SdkApi() < device_info::SDK_VERSION_15) {
      drag_records_.push_back(record);
    }
  }
}

void NodeHandleDragDropOhosAdapter::HandleBookmarkDataRecord(
    std::shared_ptr<OhosStartDragParam> drag_param) {
  if (!drag_param->basic_data.bookmark_data.empty()) {
    OH_UdmfRecord* record = GetUdmfRecordForDragData();
    if (record == nullptr) {
      LOGE(
          "%{public}s %{public}s, get udmf record fail, "
          "record is null window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
      return;
    }
    unsigned char* entry = reinterpret_cast<unsigned char*>(
        drag_param->basic_data.bookmark_data.data());
    unsigned int count =
        static_cast<unsigned int>(drag_param->basic_data.bookmark_data.size());
    int err_code = OH_UdmfRecord_AddGeneralEntry(
        record, kBookmarkDefinedType.c_str(), entry, count);
    if (err_code != UDMF_E_OK) {
      LOGE(
          "%{public}s %{public}s, add generate entry fail, "
          "code:%{public}d, window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
      return;
    }
    if (device_info::DeviceInfo::SdkApi() < device_info::SDK_VERSION_15) {
      drag_records_.push_back(record);
    }
  }
}
void NodeHandleDragDropOhosAdapter::ClearDragRecords() {
  if (!drag_records_.empty()) {
    for (unsigned int i = 0; i < drag_records_.size(); i++) {
      OH_UdmfRecord_Destroy(drag_records_[i]);
      drag_records_[i] = nullptr;
    }
    drag_records_.clear();
    std::vector<OH_UdmfRecord*>().swap(drag_records_);
  }
}

OH_UdmfRecord* NodeHandleDragDropOhosAdapter::GetUdmfRecordForDragData() {
  if (device_info::DeviceInfo::SdkApi() >= device_info::SDK_VERSION_15) {
    if (drag_records_.size() == 0) {
      LOGI("%{public}s %{public}s create one record",
           kNodeHandleDragTag, __FUNCTION__);
      OH_UdmfRecord* record = OH_UdmfRecord_Create();
      if (record == nullptr) {
        LOGE(
            "%{public}s %{public}s, udmf record create fail, "
            "window_id:%{public}s",
            kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
        return nullptr;
      }
      drag_records_.push_back(record);
      return record;
    } else {
      return drag_records_[0];
    }
  } else {
    LOGI("%{public}s %{public}s create record for data", kNodeHandleDragTag, __FUNCTION__);
    OH_UdmfRecord* record = OH_UdmfRecord_Create();
    if (record == nullptr) {
      LOGE(
          "%{public}s %{public}s, udmf record create fail, "
          "window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
      return nullptr;
    }
    return record;
  }
}

bool NodeHandleDragDropOhosAdapter::HasWebImageRecord() {
  if (drag_records_.empty()) {
    return false;
  }
  for (unsigned int i = 0; i < drag_records_.size(); i++) {
    unsigned int type_count;
    char** types = OH_UdmfRecord_GetTypes(drag_records_[i], &type_count);
    if (types == nullptr || type_count == 0) {
      LOGE("%{public}s %{public}s get udmf type fail, types is null",
           kNodeHandleDragTag, __FUNCTION__);
      continue;
    }
    for (unsigned int j = 0; j < type_count; j++) {
      if (strcmp(types[j], UDMF_META_GENERAL_FILE_URI) == 0) {
        return true;
      }
    }
  }
  return false;
}

bool NodeHandleDragDropOhosAdapter::PrepareDragData(
    std::shared_ptr<OhosStartDragParam> drag_param) {
  if (!drag_records_.empty()) {
    LOGW(
        "%{public}s %{public}s drag_records_ is not properly cleared!! "
        ", records-size:%{public}zu, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, drag_records_.size(),
        window_id_.c_str());
    ClearDragRecords();
  }
  udmf_data_ = OH_UdmfData_Create();
  if (udmf_data_ == nullptr) {
    LOGE(
        "%{public}s %{public}s, udmf data create fail, "
        "window_id::%{public}s",
        kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
    ClearDragActionResource();
    return false;
  }
  HandleWebImageRecord(drag_param);
  HandlePlainTextRecord(drag_param);
  HandleUdsHyperlinkRecord(drag_param);
  HandleUdsHtmlRecord(drag_param);
  HandleWebCustomDataRecord(drag_param);
  HandleBookmarkDataRecord(drag_param);
  LOGI(
      "%{public}s %{public}s after prepare data"
      ", records-size:%{public}zu",
      kNodeHandleDragTag, __FUNCTION__, drag_records_.size());
  for (unsigned int i = 0; i < drag_records_.size(); i++) {
    int err_code = OH_UdmfData_AddRecord(udmf_data_, drag_records_[i]);
    if (err_code != UDMF_E_OK) {
      LOGE(
          "%{public}s %{public}s, add udmf record fail, "
          "code:%{public}d, window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    }
  }
  int err_code = OH_ArkUI_DragAction_SetData(drag_action_, udmf_data_);
  if (err_code != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE(
        "%{public}s %{public}s, set udmf data fail, "
        "code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    ClearDragActionResource();
    return false;
  }
  return true;
}

void NodeHandleDragDropOhosAdapter::PreparePlainTextData(
    OH_UdmfRecord* umdf_record,
    OhosDropData& drop_data) {
  UdsPlainTextPtr plain_text_ptr(OH_UdsPlainText_Create());
  if (plain_text_ptr == nullptr) {
    LOGE(
        "%{public}s %{public}s, plain text create fail, "
        "window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
    return;
  }
  int res = OH_UdmfRecord_GetPlainText(umdf_record, plain_text_ptr.get());
  if (res != UDMF_E_OK) {
    LOGE(
        "%{public}s %{public}s, get plain text fail, "
        "code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, res, window_id_.c_str());
    return;
  }
  const char* text = OH_UdsPlainText_GetContent(plain_text_ptr.get());
  if (text != nullptr) {
    drop_data.basic_data.text = text;
  }
}

void NodeHandleDragDropOhosAdapter::PrepareHyperlinkData(
    OH_UdmfRecord* umdf_record,
    OhosDropData& drop_data) {
  UdsHyperlinkPtr hyperlink_ptr(OH_UdsHyperlink_Create());
  if (hyperlink_ptr == nullptr) {
    LOGE(
        "%{public}s %{public}s, hyperlink create fail, "
        "window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
    return;
  }
  int res = OH_UdmfRecord_GetHyperlink(umdf_record, hyperlink_ptr.get());
  if (res != UDMF_E_OK) {
    LOGE(
        "%{public}s %{public}s, get hyperlink fail, "
        "code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, res, window_id_.c_str());
    return;
  }
  const char* hyperlink_url = OH_UdsHyperlink_GetUrl(hyperlink_ptr.get());
  if (hyperlink_url != nullptr) {
    drop_data.basic_data.url = hyperlink_url;
  }
  const char* hyperlink_description =
      OH_UdsHyperlink_GetDescription(hyperlink_ptr.get());
  if (hyperlink_description != nullptr) {
    drop_data.basic_data.url_title = hyperlink_description;
  }
}

void NodeHandleDragDropOhosAdapter::PrepareHtmlData(OH_UdmfRecord* umdf_record,
                                                    OhosDropData& drop_data) {
  UdsHtmlPtr html_ptr(OH_UdsHtml_Create());
  if (html_ptr == nullptr) {
    LOGE(
        "%{public}s %{public}s, uds html create fail, "
        "window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
    return;
  }
  int res = OH_UdmfRecord_GetHtml(umdf_record, html_ptr.get());
  if (res != UDMF_E_OK) {
    LOGE(
        "%{public}s %{public}s, get uds html fail, "
        "code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, res, window_id_.c_str());
    return;
  }
  const char* html_content = OH_UdsHtml_GetContent(html_ptr.get());
  if (html_content != nullptr) {
    drop_data.basic_data.html = html_content;
  }
}

void NodeHandleDragDropOhosAdapter::PrepareFileUriData(
    OH_UdmfRecord* umdf_record,
    std::vector<std::string>& file_paths) {
  if (HasWebImageRecord()) {
    LOGW(
        "%{public}s %{public}s has web image, do not prepare file uri data,"
        "window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
    return;
  }
  UdsFileUriPtr uds_file_uri_ptr(OH_UdsFileUri_Create());
  if (uds_file_uri_ptr == nullptr) {
    LOGE(
        "%{public}s %{public}s, uds file uri create fail, "
        "uds_file_uri is null, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
    return;
  }
  int res = OH_UdmfRecord_GetFileUri(umdf_record, uds_file_uri_ptr.get());
  if (res != UDMF_E_OK) {
    LOGE(
        "%{public}s %{public}s, get uds file uri fail, "
        "code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, res, window_id_.c_str());
    return;
  }
  const char* file_url = OH_UdsFileUri_GetFileUri(uds_file_uri_ptr.get());
  if (file_url == nullptr) {
    LOGE(
        "%{public}s %{public}s, get file uri fail, "
        "window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
    return;
  }
  std::string file_path;
  FileManagerAdapter::GetInstance().GetPathForUri(file_url, file_path);
  if (file_path.empty()) {
    LOGE(
        "%{public}s %{public}s get path for uri fail, "
        "window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
    return;
  }
  file_paths.push_back(file_path);
}

void NodeHandleDragDropOhosAdapter::PrepareBookmarkData(
    OH_UdmfRecord* umdf_record,
    OhosDropData& drop_data) {
  unsigned char* entry;
  unsigned int entry_count;
  int res = OH_UdmfRecord_GetGeneralEntry(
      umdf_record, kBookmarkDefinedType.c_str(), &entry, &entry_count);
  if (res != UDMF_E_OK) {
    LOGE(
        "%{public}s %{public}s, get generate entry fail, "
        "res=%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, res, window_id_.c_str());
    return;
  }
  std::vector<uint8_t> bookmark_data;
  for (unsigned int k = 0; k < entry_count; ++k) {
    bookmark_data.push_back(*entry);
    ++entry;
  }
  drop_data.basic_data.bookmark_data = std::move(bookmark_data);
}

void NodeHandleDragDropOhosAdapter::PrepareWebCustomeData(
    OH_UdmfRecord* umdf_record,
    OhosDropData& drop_data) {
  unsigned char* entry;
  unsigned int entry_count;
  int res = OH_UdmfRecord_GetGeneralEntry(
      umdf_record, kWebCustomDefinedType.c_str(), &entry, &entry_count);
  if (res != UDMF_E_OK) {
    LOGE(
        "%{public}s %{public}s, get generate entry fail, "
        "res=%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, res, window_id_.c_str());
    return;
  }
  std::vector<uint8_t> web_custome_data;
  for (unsigned int k = 0; k < entry_count; ++k) {
    web_custome_data.push_back(*entry);
    ++entry;
  }
  drop_data.basic_data.web_custom_data = std::move(web_custome_data);
}

void NodeHandleDragDropOhosAdapter::HandleRecordData(OH_UdmfRecord** records,
                                                     unsigned int count,
                                                     OhosDropData& drop_data) {
  std::vector<std::string> file_paths;
  for (unsigned int i = 0; i < count; i++) {
    OH_UdmfRecord* umdf_record = records[i];
    unsigned int type_count;
    char** types = OH_UdmfRecord_GetTypes(umdf_record, &type_count);
    if (types == nullptr || type_count == 0) {
      LOGE(
          "%{public}s %{public}s, get types fail, "
          "indow_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
      continue;
    }
    for (unsigned int j = 0; j < type_count; j++) {
      LOGI("%{public}s %{public}s type:%{public}s", kNodeHandleDragTag, __FUNCTION__, types[j]);
      if (strcmp(types[j], UDMF_META_PLAIN_TEXT) == 0) {
        PreparePlainTextData(umdf_record, drop_data);
      } else if (strcmp(types[j], UDMF_META_HYPERLINK) == 0) {
        PrepareHyperlinkData(umdf_record, drop_data);
      } else if (strcmp(types[j], UDMF_META_HTML) == 0) {
        PrepareHtmlData(umdf_record, drop_data);
      } else if (strcmp(types[j], UDMF_META_GENERAL_FILE_URI) == 0) {
        PrepareFileUriData(umdf_record, file_paths);
      } else if (strcmp(types[j], kBookmarkDefinedType.c_str()) == 0) {
        PrepareBookmarkData(umdf_record, drop_data);
      } else if (strcmp(types[j], kWebCustomDefinedType.c_str()) == 0) {
        PrepareWebCustomeData(umdf_record, drop_data);
      } else {
        LOGW("%{public}s %{public}s type not suppoert:%{public}s",
            kNodeHandleDragTag, __FUNCTION__, types[j]);
      }
    }
  }
  LOGI("%{public}s %{public}s file_paths-size:%{public}zu", kNodeHandleDragTag,
       __FUNCTION__, file_paths.size());
  drop_data.file_paths = file_paths;
}

void NodeHandleDragDropOhosAdapter::HandleDefaultValueForDragEnter(
    ArkUI_DragEvent* drag_event,
    OhosDropData& drop_data) {
  if (drag_event == nullptr) {
    LOGE("%{public}s %{public}s drag_event is null", kNodeHandleDragTag, __FUNCTION__);
    return;
  }
  int32_t data_type_count;
  int32_t err_code =
      OH_ArkUI_DragEvent_GetDataTypeCount(drag_event, &data_type_count);
  if (err_code != ARKUI_ERROR_CODE_NO_ERROR || data_type_count <= 0) {
    LOGE(
        "%{public}s %{public}s, get data type count fail,"
        "code:%{public}d",
        kNodeHandleDragTag, __FUNCTION__, err_code);
    return;
  }
  char array_temp[data_type_count][kMaxDataTypeLength];
  char* data_types[data_type_count];
  for (int i = 0; i < data_type_count; i++) {
    data_types[i] = array_temp[i];
  }
  err_code = OH_ArkUI_DragEvent_GetDataTypes(
      drag_event, data_types, data_type_count, kMaxDataTypeLength + 1);
  if (err_code != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE(
        "%{public}s %{public}s, get data types fail, "
        "code:%{public}d",
        kNodeHandleDragTag, __FUNCTION__, err_code);
  } else {
    std::vector<std::string> file_paths;
    // drag from other apps, there is no actual data, use temporary data
    for (int32_t i = 0; i < data_type_count; i++) {
      LOGI("%{public}s %{public}s, type:%{public}s", kNodeHandleDragTag, __FUNCTION__,
           data_types[i]);
      if (strcmp(data_types[i], UDMF_META_HYPERLINK) == 0) {
        drop_data.basic_data.url = kHyperlinkDragDefaultUrl;
        drop_data.basic_data.url_title = khyperlinkDragDefaultTitle;
      } else if (strcmp(data_types[i], UDMF_META_PLAIN_TEXT) == 0) {
        drop_data.basic_data.text = kPlainTextDragDefault;
      } else if (strcmp(data_types[i], UDMF_META_GENERAL_FILE) == 0 ||
                 strcmp(data_types[i], UDMF_META_FOLDER) == 0 ||
                 strcmp(data_types[i], UDMF_META_IMAGE) == 0 ||
                 strcmp(data_types[i], UDMF_META_VIDEO) == 0 ||
                 strcmp(data_types[i], UDMF_META_AUDIO) == 0) {
        file_paths.push_back(kFileDragDefaultName);
      } else {
        LOGW(
            "%{public}s %{public}s, type:%{public}s do not need default "
            "value.",
            kNodeHandleDragTag, __FUNCTION__, data_types[i]);
      }
    }
    drop_data.file_paths = file_paths;
  }
}

void NodeHandleDragDropOhosAdapter::HandleDropDataForDrop(
    ArkUI_NodeEvent* node_event,
    OhosDropData& drop_data,
    const std::string xcomponent_id) {
  ArkUI_DragEvent* drag_event = OH_ArkUI_NodeEvent_GetDragEvent(node_event);
  if (drag_event == nullptr) {
    LOGE(
        "%{public}s %{public}s, get drag event fail, "
        "xcomponent_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, xcomponent_id.c_str());
    return;
  }
  UdmfDataPtr drag_data_ptr(OH_UdmfData_Create());
  if (drag_data_ptr == nullptr) {
    LOGE(
        "%{public}s %{public}s, udmf data create fail, "
        "xcomponent_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, xcomponent_id.c_str());
    return;
  }
  int err_code =
      OH_ArkUI_DragEvent_GetUdmfData(drag_event, drag_data_ptr.get());
  if (err_code != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE(
        "%{public}s %{public}s, get udmf data fail, "
        "code:%{public}d, xcomponent_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, xcomponent_id.c_str());
    return;
  }
  unsigned int count = 0;
  OH_UdmfRecord** records = OH_UdmfData_GetRecords(drag_data_ptr.get(), &count);
  if (records == nullptr || count == 0) {
    LOGE(
        "%{public}s %{public}s, get udmf records fail, "
        "xcomponent_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, xcomponent_id.c_str());
    return;
  }
  HandleRecordData(records, count, drop_data);
}

bool NodeHandleDragDropOhosAdapter::ExecuteDrag(
    std::shared_ptr<OhosStartDragParam> drag_param,
    const std::string& window_id) {
  window_id_ = window_id;
  std::shared_ptr<xcomponent::NodeHandleXComponentImpl>
      node_handle_xcomponent_impl =
          XComponentManager::GetInstance()->GetNodeHandleXComponent(window_id);
  if (node_handle_xcomponent_impl == nullptr) {
    LOGE(
        "%{public}s %{public}s fail, xcomponent_impl is null, "
        "window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, window_id.c_str());
    return false;
  }
  ArkUI_NodeHandle node_handle = node_handle_xcomponent_impl->GetNodeHandle();
  drag_action_ = OH_ArkUI_CreateDragActionWithNode(node_handle);
  if (drag_action_ == nullptr) {
    LOGE(
        "%{public}s %{public}s fail, drag_action is null, "
        "window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, window_id_.c_str());
    return false;
  }
  LOGI(
      "%{public}s %{public}s, window_id:%{public}s, start drag "
      "params:%{public}s",
      kNodeHandleDragTag, __FUNCTION__, window_id_.c_str(),
      drag_param->ToString().c_str());
  // Prepare configuration parameters, drag shadow data
  // and drag data for dragAction
  std::vector<OH_PixelmapNative*> pixel_vector;
  if (!PrepareDragActionOptions(drag_param) ||
      !PrepareDragActionPixelmap(drag_param, pixel_vector) ||
      !PrepareDragData(drag_param)) {
    return false;
  }
  int err_code = OH_ArkUI_DragAction_RegisterStatusListener(
      drag_action_, nullptr,
      [](ArkUI_DragAndDropInfo* drag_drop_info, void* user_data) -> void {
        ArkUI_DragStatus status =
            OH_ArkUI_DragAndDropInfo_GetDragStatus(drag_drop_info);
        if (status == ARKUI_DRAG_STATUS_ENDED) {
          NodeHandleDragDropOhosAdapter::GetInstance().OnDragEndCB(window_id_);
        }
      });
  if (err_code != ARKUI_ERROR_CODE_NO_ERROR) {
    LOGE(
        "%{public}s %{public}s, register status listener fail"
        "code:%{public}d, window_id:%{public}s",
        kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
  } else {
    err_code = OH_ArkUI_StartDrag(drag_action_);
    if (err_code != ARKUI_ERROR_CODE_NO_ERROR) {
      LOGE(
          "%{public}s %{public}s, start drag fail, code:%{public}d, "
          "window_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, err_code, window_id_.c_str());
    } else {
      SetDraggingStarted(true);
    }
  }
  ClearDragActionResource();
  return err_code == ARKUI_ERROR_CODE_NO_ERROR;
}
 
void NodeHandleDragDropOhosAdapter::OnDragEnterCB(
    const std::string xcomponent_id,
    ArkUI_NodeEvent* node_event) {
  LOGI("%{public}s %{public}s, xcomponent_id:%{public}s",
       kNodeHandleDragTag, __FUNCTION__, xcomponent_id.c_str());
  auto render =
      XComponentManager::GetInstance()->GetNodeHandleXComponent(xcomponent_id);
  if (render == nullptr) {
    LOGE("%{public}s %{public}s can not get render: %{public}s",
         kNodeHandleDragTag, __FUNCTION__, xcomponent_id.c_str());
    return;
  }
  ArkUI_DragEvent* drag_event = nullptr;
  SetSuggestedDropOperation(node_event, &drag_event);

  OhosDropData drop_data;
  if (!drag_records_.empty()) {
    HandleRecordData(drag_records_.data(), drag_records_.size(), drop_data);
  } else {
    HandleDefaultValueForDragEnter(drag_event, drop_data);
  }
  LOGI(
      "%{public}s %{public}s, xcomponent_id:%{public}s, drag "
      "data:%{public}s",
      kNodeHandleDragTag, __FUNCTION__, xcomponent_id.c_str(),
      drop_data.ToString().c_str());
  render->OnDragEnterEvent(drop_data);
}

void NodeHandleDragDropOhosAdapter::OnDragMoveCB(
    const std::string xcomponent_id,
    ArkUI_NodeEvent* node_event) {
  auto render =
      XComponentManager::GetInstance()->GetNodeHandleXComponent(xcomponent_id);
  if (render == nullptr) {
    LOGE("%{public}s %{public}s can not get render: %{public}s",
         kNodeHandleDragTag, __FUNCTION__, xcomponent_id.c_str());
    return;
  }
  ArkUI_DragEvent* drag_event = nullptr;
  SetSuggestedDropOperation(node_event, &drag_event);
  if (drag_event != nullptr) {
    ArkUI_IntOffset offset;
    int32_t ret = OH_ArkUI_NodeUtils_GetPositionWithTranslateInScreen(
        render->GetNodeHandle(), &offset);
    if (ret != ARKUI_ERROR_CODE_NO_ERROR) {
      LOGE(
          "%{public}s %{public}s "
          "OH_ArkUI_NodeUtils_GetPositionWithTranslateInScreen fail, "
          "code:%{public}d, xcomponent_id:%{public}s",
          kNodeHandleDragTag, __FUNCTION__, ret, xcomponent_id.c_str());
      return;
    }
    float display_x = OH_ArkUI_DragEvent_GetTouchPointXToDisplay(drag_event);
    float display_y = OH_ArkUI_DragEvent_GetTouchPointYToDisplay(drag_event);
    float component_x = display_x - offset.x;
    float component_y = display_y - offset.y;
    render->OnDragMoveEvent(component_x, component_y);
  }
}

void NodeHandleDragDropOhosAdapter::OnDragLeaveCB(
    const std::string xcomponent_id,
    ArkUI_NodeEvent* node_event) {
  LOGI("%{public}s %{public}s, xcomponent_id:%{public}s",
       kNodeHandleDragTag, __FUNCTION__, xcomponent_id.c_str());
  auto render =
      XComponentManager::GetInstance()->GetNodeHandleXComponent(xcomponent_id);
  if (render == nullptr) {
    LOGE("%{public}s %{public}s can not get render: %{public}s",
         kNodeHandleDragTag, __FUNCTION__, xcomponent_id.c_str());
    return;
  }
  ArkUI_DragEvent* drag_event = nullptr;
  SetSuggestedDropOperation(node_event, &drag_event);
  render->OnDragLeaveEvent();
}

void NodeHandleDragDropOhosAdapter::OnDropCB(const std::string xcomponent_id,
                                             ArkUI_NodeEvent* node_event) {
  LOGI("%{public}s %{public}s, xcomponent_id:%{public}s", kNodeHandleDragTag,
       __FUNCTION__, xcomponent_id.c_str());
  auto render =
      XComponentManager::GetInstance()->GetNodeHandleXComponent(xcomponent_id);
  if (render == nullptr) {
    LOGE("%{public}s %{public}s can not get render: %{public}s",
         kNodeHandleDragTag, __FUNCTION__, xcomponent_id.c_str());
    return;
  }

  OhosDropData drop_data;
  HandleDropDataForDrop(node_event, drop_data, xcomponent_id);
  LOGI(
      "%{public}s %{public}s, xcomponent_id:%{public}s, drop "
      "data:%{public}s",
      kNodeHandleDragTag, __FUNCTION__, xcomponent_id.c_str(),
      drop_data.ToString().c_str());
  render->OnDropEvent(drop_data);
}

void NodeHandleDragDropOhosAdapter::OnDragEndCB(
    const std::string xcomponent_id) {
  LOGI("%{public}s %{public}s, xcomponent_id:%{public}s",
       kNodeHandleDragTag, __FUNCTION__, xcomponent_id.c_str());
  ClearDragRecords();
  SetDraggingStarted(false);
  auto render =
      XComponentManager::GetInstance()->GetNodeHandleXComponent(xcomponent_id);
  if (render == nullptr) {
    LOGE("%{public}s %{public}s can not get render: %{public}s",
         kNodeHandleDragTag, __FUNCTION__, xcomponent_id.c_str());
    return;
  }
  render->OnDragEndEvent();
}

}  // namespace adapter
}  // namespace ohos
