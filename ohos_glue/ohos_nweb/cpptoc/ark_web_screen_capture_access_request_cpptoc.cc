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

#include "ohos_nweb/cpptoc/ark_web_screen_capture_access_request_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_nweb/ctocpp/ark_web_screen_capture_config_ctocpp.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_web_screen_capture_access_request_agree(
    struct _ark_web_screen_capture_access_request_t *self,
    ark_web_screen_capture_config_t *config) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebScreenCaptureAccessRequestCppToC::Get(self)->Agree(
      ArkWebScreenCaptureConfigCToCpp::Invert(config));
}

void ARK_WEB_CALLBACK ark_web_screen_capture_access_request_refuse(
    struct _ark_web_screen_capture_access_request_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebScreenCaptureAccessRequestCppToC::Get(self)->Refuse();
}

ArkWebString ARK_WEB_CALLBACK ark_web_screen_capture_access_request_origin(
    struct _ark_web_screen_capture_access_request_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebScreenCaptureAccessRequestCppToC::Get(self)->Origin();
}

} // namespace

ArkWebScreenCaptureAccessRequestCppToC::
    ArkWebScreenCaptureAccessRequestCppToC() {
  GetStruct()->agree = ark_web_screen_capture_access_request_agree;
  GetStruct()->refuse = ark_web_screen_capture_access_request_refuse;
  GetStruct()->origin = ark_web_screen_capture_access_request_origin;
}

ArkWebScreenCaptureAccessRequestCppToC::
    ~ArkWebScreenCaptureAccessRequestCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkWebScreenCaptureAccessRequestCppToC, ArkWebScreenCaptureAccessRequest,
    ark_web_screen_capture_access_request_t>::kBridgeType =
    ARK_WEB_SCREEN_CAPTURE_ACCESS_REQUEST;

} // namespace OHOS::ArkWeb
