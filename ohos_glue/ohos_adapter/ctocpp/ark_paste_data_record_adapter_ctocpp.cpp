/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_adapter/ctocpp/ark_paste_data_record_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/bridge/ark_web_adapter_bridge_helper.h"
#include "ohos_adapter/cpptoc/ark_clip_board_image_data_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

using ArkPasteDataRecordAdapterNewRecordFunc1 =
    ark_paste_data_record_adapter_t *(*)(const ArkWebString *mimeType);
static ArkPasteDataRecordAdapterNewRecordFunc1
    ark_paste_data_record_adapter_new_record1 = nullptr;

using ArkPasteDataRecordAdapterNewRecordFunc2 =
    ark_paste_data_record_adapter_t *(*)(const ArkWebString *mimeType,
                                         void *htmlText, void *plainText);
static ArkPasteDataRecordAdapterNewRecordFunc2
    ark_paste_data_record_adapter_new_record2 = nullptr;

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkPasteDataRecordAdapter>
ArkPasteDataRecordAdapter::NewRecord(const ArkWebString &mimeType) {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_paste_data_record_adapter_new_record1) {
    ark_paste_data_record_adapter_new_record1 =
        reinterpret_cast<ArkPasteDataRecordAdapterNewRecordFunc1>(
            ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
                "ark_paste_data_record_adapter_new_record1_static"));
    if (!ark_paste_data_record_adapter_new_record1) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return nullptr;
    }
  }

  // Execute
  ark_paste_data_record_adapter_t *_retval =
      ark_paste_data_record_adapter_new_record1(&mimeType);

  // Return type: refptr_same
  return ArkPasteDataRecordAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkPasteDataRecordAdapter>
ArkPasteDataRecordAdapter::NewRecord(const ArkWebString &mimeType,
                                     void *htmlText, void *plainText) {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_paste_data_record_adapter_new_record2) {
    ark_paste_data_record_adapter_new_record2 =
        reinterpret_cast<ArkPasteDataRecordAdapterNewRecordFunc2>(
            ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
                "ark_paste_data_record_adapter_new_record2_static"));
    if (!ark_paste_data_record_adapter_new_record2) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return nullptr;
    }
  }

  // Execute
  ark_paste_data_record_adapter_t *_retval =
      ark_paste_data_record_adapter_new_record2(&mimeType, htmlText, plainText);

  // Return type: refptr_same
  return ArkPasteDataRecordAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
bool ArkPasteDataRecordAdapterCToCpp::SetHtmlText(void *htmlText) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_record_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_html_text, false);

  // Execute
  return _struct->set_html_text(_struct, htmlText);
}

ARK_WEB_NO_SANITIZE
bool ArkPasteDataRecordAdapterCToCpp::SetPlainText(void *plainText) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_record_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_plain_text, false);

  // Execute
  return _struct->set_plain_text(_struct, plainText);
}

ARK_WEB_NO_SANITIZE
bool ArkPasteDataRecordAdapterCToCpp::SetImgData(
    ArkWebRefPtr<ArkClipBoardImageDataAdapter> imageData) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_record_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_img_data, false);

  // Execute
  return _struct->set_img_data(
      _struct, ArkClipBoardImageDataAdapterCppToC::Invert(imageData));
}

ARK_WEB_NO_SANITIZE
ArkWebString ArkPasteDataRecordAdapterCToCpp::GetMimeType() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_record_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_mime_type,
                                   ark_web_string_default);

  // Execute
  return _struct->get_mime_type(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkPasteDataRecordAdapterCToCpp::GetHtmlText(void *data) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_record_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_html_text, );

  // Execute
  _struct->get_html_text(_struct, data);
}

ARK_WEB_NO_SANITIZE
void ArkPasteDataRecordAdapterCToCpp::GetPlainText(void *data) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_record_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_plain_text, );

  // Execute
  _struct->get_plain_text(_struct, data);
}

ARK_WEB_NO_SANITIZE
bool ArkPasteDataRecordAdapterCToCpp::GetImgData(
    ArkWebRefPtr<ArkClipBoardImageDataAdapter> imageData) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_record_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_img_data, false);

  // Execute
  return _struct->get_img_data(
      _struct, ArkClipBoardImageDataAdapterCppToC::Invert(imageData));
}

ARK_WEB_NO_SANITIZE
bool ArkPasteDataRecordAdapterCToCpp::SetUri(const ArkWebString &uriString) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_record_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_uri, false);

  // Execute
  return _struct->set_uri(_struct, &uriString);
}

ARK_WEB_NO_SANITIZE
bool ArkPasteDataRecordAdapterCToCpp::SetCustomData(void *data) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_record_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_custom_data, false);

  // Execute
  return _struct->set_custom_data(_struct, data);
}

ARK_WEB_NO_SANITIZE
void ArkPasteDataRecordAdapterCToCpp::GetUri(void *data) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_record_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_uri, );

  // Execute
  _struct->get_uri(_struct, data);
}

ARK_WEB_NO_SANITIZE
void ArkPasteDataRecordAdapterCToCpp::GetCustomData(void *data) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_record_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_custom_data, );

  // Execute
  _struct->get_custom_data(_struct, data);
}

ArkPasteDataRecordAdapterCToCpp::ArkPasteDataRecordAdapterCToCpp() {
}

ArkPasteDataRecordAdapterCToCpp::~ArkPasteDataRecordAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkPasteDataRecordAdapterCToCpp, ArkPasteDataRecordAdapter,
    ark_paste_data_record_adapter_t>::kBridgeType =
    ARK_PASTE_DATA_RECORD_ADAPTER;

} // namespace OHOS::ArkWeb
