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

#include "ohos_adapter/ctocpp/ark_battery_mgr_client_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_battery_event_callback_cpptoc.h"
#include "ohos_adapter/ctocpp/ark_battery_info_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void ArkBatteryMgrClientAdapterCToCpp::RegBatteryEvent(
    ArkWebRefPtr<ArkBatteryEventCallback> eventCallback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_battery_mgr_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, reg_battery_event, );

  // Execute
  _struct->reg_battery_event(
      _struct, ArkBatteryEventCallbackCppToC::Invert(eventCallback));
}

ARK_WEB_NO_SANITIZE
bool ArkBatteryMgrClientAdapterCToCpp::StartListen() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_battery_mgr_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start_listen, false);

  // Execute
  return _struct->start_listen(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkBatteryMgrClientAdapterCToCpp::StopListen() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_battery_mgr_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, stop_listen, );

  // Execute
  _struct->stop_listen(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkBatteryInfo>
ArkBatteryMgrClientAdapterCToCpp::RequestBatteryInfo() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_battery_mgr_client_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, request_battery_info, nullptr);

  // Execute
  ark_battery_info_t *_retval = _struct->request_battery_info(_struct);

  // Return type: refptr_same
  return ArkBatteryInfoCToCpp::Invert(_retval);
}

ArkBatteryMgrClientAdapterCToCpp::ArkBatteryMgrClientAdapterCToCpp() {
}

ArkBatteryMgrClientAdapterCToCpp::~ArkBatteryMgrClientAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkBatteryMgrClientAdapterCToCpp, ArkBatteryMgrClientAdapter,
    ark_battery_mgr_client_adapter_t>::kBridgeType =
    ARK_BATTERY_MGR_CLIENT_ADAPTER;

} // namespace OHOS::ArkWeb
