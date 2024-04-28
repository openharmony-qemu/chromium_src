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

#include "ohos_nweb/cpptoc/ark_web_image_options_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

size_t ARK_WEB_CALLBACK
ark_web_image_options_get_width(struct _ark_web_image_options_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebImageOptionsCppToC::Get(self)->GetWidth();
}

size_t ARK_WEB_CALLBACK
ark_web_image_options_get_height(struct _ark_web_image_options_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebImageOptionsCppToC::Get(self)->GetHeight();
}

int ARK_WEB_CALLBACK
ark_web_image_options_get_alpha_type(struct _ark_web_image_options_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebImageOptionsCppToC::Get(self)->GetAlphaType();
}

int ARK_WEB_CALLBACK
ark_web_image_options_get_color_type(struct _ark_web_image_options_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebImageOptionsCppToC::Get(self)->GetColorType();
}

} // namespace

ArkWebImageOptionsCppToC::ArkWebImageOptionsCppToC() {
  GetStruct()->get_width = ark_web_image_options_get_width;
  GetStruct()->get_height = ark_web_image_options_get_height;
  GetStruct()->get_alpha_type = ark_web_image_options_get_alpha_type;
  GetStruct()->get_color_type = ark_web_image_options_get_color_type;
}

ArkWebImageOptionsCppToC::~ArkWebImageOptionsCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkWebImageOptionsCppToC, ArkWebImageOptions,
                           ark_web_image_options_t>::kBridgeType =
        ARK_WEB_IMAGE_OPTIONS;

} // namespace OHOS::ArkWeb
