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

#include "ohos_adapter/ctocpp/ark_enterprise_device_management_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_edm_policy_changed_event_callback_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int32_t ArkEnterpriseDeviceManagementAdapterCToCpp::GetPolicies(
    ArkWebString &policies) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_enterprise_device_management_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_policies, 0);

  // Execute
  return _struct->get_policies(_struct, &policies);
}

ARK_WEB_NO_SANITIZE
void ArkEnterpriseDeviceManagementAdapterCToCpp::
    RegistPolicyChangeEventCallback(
        ArkWebRefPtr<ArkEdmPolicyChangedEventCallbackAdapter> eventCallback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_enterprise_device_management_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct,
                                   regist_policy_change_event_callback, );

  // Execute
  _struct->regist_policy_change_event_callback(
      _struct,
      ArkEdmPolicyChangedEventCallbackAdapterCppToC::Invert(eventCallback));
}

ARK_WEB_NO_SANITIZE
bool ArkEnterpriseDeviceManagementAdapterCToCpp::StartObservePolicyChange() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_enterprise_device_management_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start_observe_policy_change, false);

  // Execute
  return _struct->start_observe_policy_change(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkEnterpriseDeviceManagementAdapterCToCpp::StopObservePolicyChange() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_enterprise_device_management_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, stop_observe_policy_change, false);

  // Execute
  return _struct->stop_observe_policy_change(_struct);
}

ArkEnterpriseDeviceManagementAdapterCToCpp::
    ArkEnterpriseDeviceManagementAdapterCToCpp() {
}

ArkEnterpriseDeviceManagementAdapterCToCpp::
    ~ArkEnterpriseDeviceManagementAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkEnterpriseDeviceManagementAdapterCToCpp,
    ArkEnterpriseDeviceManagementAdapter,
    ark_enterprise_device_management_adapter_t>::kBridgeType =
    ARK_ENTERPRISE_DEVICE_MANAGEMENT_ADAPTER;

} // namespace OHOS::ArkWeb
