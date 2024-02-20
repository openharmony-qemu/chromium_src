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

#include "ohos_nweb/cpptoc/ark_web_file_selector_params_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int ARK_WEB_CALLBACK ark_web_file_selector_params_mode(
    struct _ark_web_file_selector_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebFileSelectorParamsCppToC::Get(self)->Mode();
}

const ArkWebString ARK_WEB_CALLBACK ark_web_file_selector_params_title(
    struct _ark_web_file_selector_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebFileSelectorParamsCppToC::Get(self)->Title();
}

bool ARK_WEB_CALLBACK ark_web_file_selector_params_is_capture(
    struct _ark_web_file_selector_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebFileSelectorParamsCppToC::Get(self)->IsCapture();
}

const ArkWebStringVector ARK_WEB_CALLBACK
ark_web_file_selector_params_accept_type(
    struct _ark_web_file_selector_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_vector_default);

  // Execute
  return ArkWebFileSelectorParamsCppToC::Get(self)->AcceptType();
}

const ArkWebString ARK_WEB_CALLBACK
ark_web_file_selector_params_default_filename(
    struct _ark_web_file_selector_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebFileSelectorParamsCppToC::Get(self)->DefaultFilename();
}

} // namespace

ArkWebFileSelectorParamsCppToC::ArkWebFileSelectorParamsCppToC() {
  GetStruct()->mode = ark_web_file_selector_params_mode;
  GetStruct()->title = ark_web_file_selector_params_title;
  GetStruct()->is_capture = ark_web_file_selector_params_is_capture;
  GetStruct()->accept_type = ark_web_file_selector_params_accept_type;
  GetStruct()->default_filename = ark_web_file_selector_params_default_filename;
}

ArkWebFileSelectorParamsCppToC::~ArkWebFileSelectorParamsCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkWebFileSelectorParamsCppToC, ArkWebFileSelectorParams,
    ark_web_file_selector_params_t>::kBridgeType = ARK_WEB_FILE_SELECTOR_PARAMS;

} // namespace OHOS::ArkWeb
