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

#include "ohos_adapter/ctocpp/ark_system_properties_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
bool ArkSystemPropertiesAdapterCToCpp::GetResourceUseHapPathEnable() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_resource_use_hap_path_enable,
                                   false);

  // Execute
  return _struct->get_resource_use_hap_path_enable(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebString ArkSystemPropertiesAdapterCToCpp::GetDeviceInfoProductModel() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_device_info_product_model,
                                   ark_web_string_default);

  // Execute
  return _struct->get_device_info_product_model(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebString ArkSystemPropertiesAdapterCToCpp::GetDeviceInfoBrand() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_device_info_brand,
                                   ark_web_string_default);

  // Execute
  return _struct->get_device_info_brand(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkSystemPropertiesAdapterCToCpp::GetDeviceInfoMajorVersion() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_device_info_major_version, 0);

  // Execute
  return _struct->get_device_info_major_version(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkSystemPropertiesAdapterCToCpp::GetProductDeviceType() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_product_device_type, 0);

  // Execute
  return _struct->get_product_device_type(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkSystemPropertiesAdapterCToCpp::GetWebOptimizationValue() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_web_optimization_value, false);

  // Execute
  return _struct->get_web_optimization_value(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkSystemPropertiesAdapterCToCpp::IsAdvancedSecurityMode() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_advanced_security_mode, false);

  // Execute
  return _struct->is_advanced_security_mode(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebString ArkSystemPropertiesAdapterCToCpp::GetUserAgentOSName() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_user_agent_osname,
                                   ark_web_string_default);

  // Execute
  return _struct->get_user_agent_osname(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkSystemPropertiesAdapterCToCpp::GetSoftwareMajorVersion() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_software_major_version, 0);

  // Execute
  return _struct->get_software_major_version(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkSystemPropertiesAdapterCToCpp::GetSoftwareSeniorVersion() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_software_senior_version, 0);

  // Execute
  return _struct->get_software_senior_version(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebString ArkSystemPropertiesAdapterCToCpp::GetNetlogMode() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_netlog_mode,
                                   ark_web_string_default);

  // Execute
  return _struct->get_netlog_mode(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkSystemPropertiesAdapterCToCpp::GetTraceDebugEnable() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_trace_debug_enable, false);

  // Execute
  return _struct->get_trace_debug_enable(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebString ArkSystemPropertiesAdapterCToCpp::GetSiteIsolationMode() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_site_isolation_mode,
                                   ark_web_string_default);

  // Execute
  return _struct->get_site_isolation_mode(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkSystemPropertiesAdapterCToCpp::GetFlowBufMaxFd() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_flow_buf_max_fd, 0);

  // Execute
  return _struct->get_flow_buf_max_fd(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkSystemPropertiesAdapterCToCpp::GetOOPGPUEnable() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_oopgpuenable, false);

  // Execute
  return _struct->get_oopgpuenable(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkSystemPropertiesAdapterCToCpp::SetOOPGPUDisable() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_oopgpudisable, );

  // Execute
  _struct->set_oopgpudisable(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkSystemPropertiesAdapterCToCpp::AttachSysPropObserver(int32_t key,
                                                             void *observer) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, attach_sys_prop_observer, );

  // Execute
  _struct->attach_sys_prop_observer(_struct, key, observer);
}

ARK_WEB_NO_SANITIZE
void ArkSystemPropertiesAdapterCToCpp::DetachSysPropObserver(int32_t key,
                                                             void *observer) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, detach_sys_prop_observer, );

  // Execute
  _struct->detach_sys_prop_observer(_struct, key, observer);
}

ARK_WEB_NO_SANITIZE
bool ArkSystemPropertiesAdapterCToCpp::GetBoolParameter(ArkWebString key,
                                                        bool defaultValue) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_bool_parameter, false);

  // Execute
  return _struct->get_bool_parameter(_struct, key, defaultValue);
}

ARK_WEB_NO_SANITIZE
ArkFrameRateSettingAdapterVector
ArkSystemPropertiesAdapterCToCpp::GetLTPOConfig(
    const ArkWebString &settingName) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_system_properties_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct,
                             ark_frame_rate_setting_adapter_vector_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(
      _struct, get_ltpoconfig, ark_frame_rate_setting_adapter_vector_default);

  // Execute
  return _struct->get_ltpoconfig(_struct, &settingName);
}

ArkSystemPropertiesAdapterCToCpp::ArkSystemPropertiesAdapterCToCpp() {
}

ArkSystemPropertiesAdapterCToCpp::~ArkSystemPropertiesAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkSystemPropertiesAdapterCToCpp, ArkSystemPropertiesAdapter,
    ark_system_properties_adapter_t>::kBridgeType =
    ARK_SYSTEM_PROPERTIES_ADAPTER;

} // namespace OHOS::ArkWeb
