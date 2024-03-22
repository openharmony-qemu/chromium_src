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

#include "ohos_nweb/cpptoc/ark_web_touch_handle_hot_zone_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_web_touch_handle_hot_zone_set_width(
    struct _ark_web_touch_handle_hot_zone_t *self, double width) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebTouchHandleHotZoneCppToC::Get(self)->SetWidth(width);
}

void ARK_WEB_CALLBACK ark_web_touch_handle_hot_zone_set_height(
    struct _ark_web_touch_handle_hot_zone_t *self, double height) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebTouchHandleHotZoneCppToC::Get(self)->SetHeight(height);
}

} // namespace

ArkWebTouchHandleHotZoneCppToC::ArkWebTouchHandleHotZoneCppToC() {
  GetStruct()->set_width = ark_web_touch_handle_hot_zone_set_width;
  GetStruct()->set_height = ark_web_touch_handle_hot_zone_set_height;
}

ArkWebTouchHandleHotZoneCppToC::~ArkWebTouchHandleHotZoneCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkWebTouchHandleHotZoneCppToC, ArkWebTouchHandleHotZone,
    ark_web_touch_handle_hot_zone_t>::kBridgeType =
    ARK_WEB_TOUCH_HANDLE_HOT_ZONE;

} // namespace OHOS::ArkWeb
