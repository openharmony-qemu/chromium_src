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

#include "ohos_adapter/ctocpp/ark_datashare_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int ArkDatashareAdapterCToCpp::OpenDataShareUriForRead(
    const ArkWebString &uriStr) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_datashare_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, open_data_share_uri_for_read, 0);

  // Execute
  return _struct->open_data_share_uri_for_read(_struct, &uriStr);
}

ARK_WEB_NO_SANITIZE
ArkWebString
ArkDatashareAdapterCToCpp::GetFileDisplayName(const ArkWebString &uriStr) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_datashare_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_file_display_name,
                                   ark_web_string_default);

  // Execute
  return _struct->get_file_display_name(_struct, &uriStr);
}

ARK_WEB_NO_SANITIZE
ArkWebString
ArkDatashareAdapterCToCpp::GetRealPath(const ArkWebString &uriStr) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_datashare_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_real_path,
                                   ark_web_string_default);

  // Execute
  return _struct->get_real_path(_struct, &uriStr);
}

ArkDatashareAdapterCToCpp::ArkDatashareAdapterCToCpp() {
}

ArkDatashareAdapterCToCpp::~ArkDatashareAdapterCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkDatashareAdapterCToCpp, ArkDatashareAdapter,
                           ark_datashare_adapter_t>::kBridgeType =
        ARK_DATASHARE_ADAPTER;

} // namespace OHOS::ArkWeb
