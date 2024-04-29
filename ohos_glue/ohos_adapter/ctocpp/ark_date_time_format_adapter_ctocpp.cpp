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

#include "ohos_adapter/ctocpp/ark_date_time_format_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_timezone_event_callback_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void ArkDateTimeFormatAdapterCToCpp::RegTimezoneEvent(
    ArkWebRefPtr<ArkTimezoneEventCallbackAdapter> eventCallback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_date_time_format_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, reg_timezone_event, );

  // Execute
  _struct->reg_timezone_event(
      _struct, ArkTimezoneEventCallbackAdapterCppToC::Invert(eventCallback));
}

ARK_WEB_NO_SANITIZE
bool ArkDateTimeFormatAdapterCToCpp::StartListen() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_date_time_format_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start_listen, false);

  // Execute
  return _struct->start_listen(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkDateTimeFormatAdapterCToCpp::StopListen() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_date_time_format_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, stop_listen, );

  // Execute
  _struct->stop_listen(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebString ArkDateTimeFormatAdapterCToCpp::GetTimezone() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_date_time_format_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_timezone,
                                   ark_web_string_default);

  // Execute
  return _struct->get_timezone(_struct);
}

ArkDateTimeFormatAdapterCToCpp::ArkDateTimeFormatAdapterCToCpp() {
}

ArkDateTimeFormatAdapterCToCpp::~ArkDateTimeFormatAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkDateTimeFormatAdapterCToCpp, ArkDateTimeFormatAdapter,
    ark_date_time_format_adapter_t>::kBridgeType = ARK_DATE_TIME_FORMAT_ADAPTER;

} // namespace OHOS::ArkWeb
