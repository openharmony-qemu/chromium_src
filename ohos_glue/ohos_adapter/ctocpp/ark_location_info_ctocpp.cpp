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

#include "ohos_adapter/ctocpp/ark_location_info_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
double ArkLocationInfoCToCpp::GetLatitude() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_latitude, 0);

  // Execute
  return _struct->get_latitude(_struct);
}

ARK_WEB_NO_SANITIZE
double ArkLocationInfoCToCpp::GetLongitude() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_longitude, 0);

  // Execute
  return _struct->get_longitude(_struct);
}

ARK_WEB_NO_SANITIZE
double ArkLocationInfoCToCpp::GetAltitude() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_altitude, 0);

  // Execute
  return _struct->get_altitude(_struct);
}

ARK_WEB_NO_SANITIZE
float ArkLocationInfoCToCpp::GetAccuracy() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_accuracy, 0);

  // Execute
  return _struct->get_accuracy(_struct);
}

ARK_WEB_NO_SANITIZE
float ArkLocationInfoCToCpp::GetSpeed() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_speed, 0);

  // Execute
  return _struct->get_speed(_struct);
}

ARK_WEB_NO_SANITIZE
double ArkLocationInfoCToCpp::GetDirection() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_direction, 0);

  // Execute
  return _struct->get_direction(_struct);
}

ARK_WEB_NO_SANITIZE
int64_t ArkLocationInfoCToCpp::GetTimeStamp() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_time_stamp, 0);

  // Execute
  return _struct->get_time_stamp(_struct);
}

ARK_WEB_NO_SANITIZE
int64_t ArkLocationInfoCToCpp::GetTimeSinceBoot() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_time_since_boot, 0);

  // Execute
  return _struct->get_time_since_boot(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebStringVector ArkLocationInfoCToCpp::GetAdditions() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_location_info_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_vector_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_additions,
                                   ark_web_string_vector_default);

  // Execute
  return _struct->get_additions(_struct);
}

ArkLocationInfoCToCpp::ArkLocationInfoCToCpp() {
}

ArkLocationInfoCToCpp::~ArkLocationInfoCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<ArkLocationInfoCToCpp, ArkLocationInfo,
                                        ark_location_info_t>::kBridgeType =
    ARK_LOCATION_INFO;

} // namespace OHOS::ArkWeb
