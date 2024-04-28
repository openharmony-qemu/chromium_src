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

#include "ohos_adapter/ctocpp/ark_location_proxy_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_location_callback_adapter_cpptoc.h"
#include "ohos_adapter/ctocpp/ark_location_request_config_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int32_t ArkLocationProxyAdapterCToCpp::StartLocating(
    ArkWebRefPtr<ArkLocationRequestConfig> requestConfig,
    ArkWebRefPtr<ArkLocationCallbackAdapter> callback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_proxy_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start_locating, 0);

  // Execute
  return _struct->start_locating(
      _struct, ArkLocationRequestConfigCToCpp::Revert(requestConfig),
      ArkLocationCallbackAdapterCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
bool ArkLocationProxyAdapterCToCpp::StopLocating(int32_t callbackId) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_proxy_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, stop_locating, false);

  // Execute
  return _struct->stop_locating(_struct, callbackId);
}

ARK_WEB_NO_SANITIZE
bool ArkLocationProxyAdapterCToCpp::EnableAbility(bool isEnabled) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_proxy_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, enable_ability, false);

  // Execute
  return _struct->enable_ability(_struct, isEnabled);
}

ARK_WEB_NO_SANITIZE
bool ArkLocationProxyAdapterCToCpp::IsLocationEnabled() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_proxy_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_location_enabled, false);

  // Execute
  return _struct->is_location_enabled(_struct);
}

ArkLocationProxyAdapterCToCpp::ArkLocationProxyAdapterCToCpp() {
}

ArkLocationProxyAdapterCToCpp::~ArkLocationProxyAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkLocationProxyAdapterCToCpp, ArkLocationProxyAdapter,
    ark_location_proxy_adapter_t>::kBridgeType = ARK_LOCATION_PROXY_ADAPTER;

} // namespace OHOS::ArkWeb
