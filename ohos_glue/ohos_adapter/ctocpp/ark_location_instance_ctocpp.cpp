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

#include "ohos_adapter/ctocpp/ark_location_instance_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/bridge/ark_web_adapter_bridge_helper.h"
#include "ohos_adapter/ctocpp/ark_location_proxy_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_location_request_config_ctocpp.h"

namespace OHOS::ArkWeb {

using ArkLocationInstanceGetInstanceFunc = ark_location_instance_t *(*)(void);
static ArkLocationInstanceGetInstanceFunc ark_location_instance_get_instance =
    nullptr;

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkLocationInstance> ArkLocationInstance::GetInstance() {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_location_instance_get_instance) {
    ark_location_instance_get_instance =
        reinterpret_cast<ArkLocationInstanceGetInstanceFunc>(
            ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
                "ark_location_instance_get_instance_static"));
    if (!ark_location_instance_get_instance) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return nullptr;
    }
  }

  // Execute
  ark_location_instance_t *_retval = ark_location_instance_get_instance();

  // Return type: refptr_same
  return ArkLocationInstanceCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkLocationProxyAdapter>
ArkLocationInstanceCToCpp::CreateLocationProxyAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_instance_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_location_proxy_adapter,
                                   nullptr);

  // Execute
  ark_location_proxy_adapter_t *_retval =
      _struct->create_location_proxy_adapter(_struct);

  // Return type: refptr_same
  return ArkLocationProxyAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkLocationRequestConfig>
ArkLocationInstanceCToCpp::CreateLocationRequestConfig() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_instance_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_location_request_config,
                                   nullptr);

  // Execute
  ark_location_request_config_t *_retval =
      _struct->create_location_request_config(_struct);

  // Return type: refptr_same
  return ArkLocationRequestConfigCToCpp::Invert(_retval);
}

ArkLocationInstanceCToCpp::ArkLocationInstanceCToCpp() {
}

ArkLocationInstanceCToCpp::~ArkLocationInstanceCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkLocationInstanceCToCpp, ArkLocationInstance,
                           ark_location_instance_t>::kBridgeType =
        ARK_LOCATION_INSTANCE;

} // namespace OHOS::ArkWeb
