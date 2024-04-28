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

#include "ohos_adapter/ctocpp/ark_paste_data_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/ctocpp/ark_paste_data_record_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void ArkPasteDataAdapterCToCpp::AddHtmlRecord(const ArkWebString &html) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, add_html_record, );

  // Execute
  _struct->add_html_record(_struct, &html);
}

ARK_WEB_NO_SANITIZE
void ArkPasteDataAdapterCToCpp::AddTextRecord(const ArkWebString &text) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, add_text_record, );

  // Execute
  _struct->add_text_record(_struct, &text);
}

ARK_WEB_NO_SANITIZE
ArkWebStringVector ArkPasteDataAdapterCToCpp::GetMimeTypes() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_vector_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_mime_types,
                                   ark_web_string_vector_default);

  // Execute
  return _struct->get_mime_types(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkPasteDataAdapterCToCpp::GetPrimaryHtml(void *data) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_primary_html, );

  // Execute
  _struct->get_primary_html(_struct, data);
}

ARK_WEB_NO_SANITIZE
void ArkPasteDataAdapterCToCpp::GetPrimaryText(void *data) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_primary_text, );

  // Execute
  _struct->get_primary_text(_struct, data);
}

ARK_WEB_NO_SANITIZE
void ArkPasteDataAdapterCToCpp::GetPrimaryMimeType(void *data) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_primary_mime_type, );

  // Execute
  _struct->get_primary_mime_type(_struct, data);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkPasteDataRecordAdapter>
ArkPasteDataAdapterCToCpp::GetRecordAt(size_t index) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_record_at, nullptr);

  // Execute
  ark_paste_data_record_adapter_t *_retval =
      _struct->get_record_at(_struct, index);

  // Return type: refptr_same
  return ArkPasteDataRecordAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
size_t ArkPasteDataAdapterCToCpp::GetRecordCount() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_record_count, 0);

  // Execute
  return _struct->get_record_count(_struct);
}

ARK_WEB_NO_SANITIZE
ArkPasteRecordVector ArkPasteDataAdapterCToCpp::AllRecords() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_paste_data_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, {0});

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, all_records, {0});

  // Execute
  return _struct->all_records(_struct);
}

ArkPasteDataAdapterCToCpp::ArkPasteDataAdapterCToCpp() {
}

ArkPasteDataAdapterCToCpp::~ArkPasteDataAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkPasteDataAdapterCToCpp, ArkPasteDataAdapter,
                           ark_paste_data_adapter_t>::kBridgeType =
        ARK_PASTE_DATA_ADAPTER;

} // namespace OHOS::ArkWeb
