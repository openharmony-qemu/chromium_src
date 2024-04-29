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

#include "ohos_adapter/ctocpp/ark_location_request_config_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void ArkLocationRequestConfigCToCpp::SetScenario(int32_t scenario) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_request_config_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_scenario, );

  // Execute
  _struct->set_scenario(_struct, scenario);
}

ARK_WEB_NO_SANITIZE
void ArkLocationRequestConfigCToCpp::SetFixNumber(int32_t number) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_request_config_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_fix_number, );

  // Execute
  _struct->set_fix_number(_struct, number);
}

ARK_WEB_NO_SANITIZE
void ArkLocationRequestConfigCToCpp::SetMaxAccuracy(int32_t maxAccuary) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_request_config_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_max_accuracy, );

  // Execute
  _struct->set_max_accuracy(_struct, maxAccuary);
}

ARK_WEB_NO_SANITIZE
void ArkLocationRequestConfigCToCpp::SetDistanceInterval(int32_t disInterval) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_request_config_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_distance_interval, );

  // Execute
  _struct->set_distance_interval(_struct, disInterval);
}

ARK_WEB_NO_SANITIZE
void ArkLocationRequestConfigCToCpp::SetTimeInterval(int32_t timeInterval) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_request_config_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_time_interval, );

  // Execute
  _struct->set_time_interval(_struct, timeInterval);
}

ARK_WEB_NO_SANITIZE
void ArkLocationRequestConfigCToCpp::SetPriority(int32_t priority) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_request_config_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_priority, );

  // Execute
  _struct->set_priority(_struct, priority);
}

ArkLocationRequestConfigCToCpp::ArkLocationRequestConfigCToCpp() {
}

ArkLocationRequestConfigCToCpp::~ArkLocationRequestConfigCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkLocationRequestConfigCToCpp, ArkLocationRequestConfig,
    ark_location_request_config_t>::kBridgeType = ARK_LOCATION_REQUEST_CONFIG;

} // namespace OHOS::ArkWeb
