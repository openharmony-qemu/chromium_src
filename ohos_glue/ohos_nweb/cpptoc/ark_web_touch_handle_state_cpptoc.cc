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

#include "ohos_nweb/cpptoc/ark_web_touch_handle_state_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int32_t ARK_WEB_CALLBACK
ark_web_touch_handle_state_get_x(struct _ark_web_touch_handle_state_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebTouchHandleStateCppToC::Get(self)->GetX();
}

int32_t ARK_WEB_CALLBACK
ark_web_touch_handle_state_get_y(struct _ark_web_touch_handle_state_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebTouchHandleStateCppToC::Get(self)->GetY();
}

bool ARK_WEB_CALLBACK ark_web_touch_handle_state_is_enable(
    struct _ark_web_touch_handle_state_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebTouchHandleStateCppToC::Get(self)->IsEnable();
}

float ARK_WEB_CALLBACK ark_web_touch_handle_state_get_alpha(
    struct _ark_web_touch_handle_state_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebTouchHandleStateCppToC::Get(self)->GetAlpha();
}

float ARK_WEB_CALLBACK ark_web_touch_handle_state_get_edge_height(
    struct _ark_web_touch_handle_state_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebTouchHandleStateCppToC::Get(self)->GetEdgeHeight();
}

int32_t ARK_WEB_CALLBACK ark_web_touch_handle_state_get_view_port_x(
    struct _ark_web_touch_handle_state_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebTouchHandleStateCppToC::Get(self)->GetViewPortX();
}

int32_t ARK_WEB_CALLBACK ark_web_touch_handle_state_get_view_port_y(
    struct _ark_web_touch_handle_state_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebTouchHandleStateCppToC::Get(self)->GetViewPortY();
}

int32_t ARK_WEB_CALLBACK ark_web_touch_handle_state_get_touch_handle_id(
    struct _ark_web_touch_handle_state_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebTouchHandleStateCppToC::Get(self)->GetTouchHandleId();
}

int ARK_WEB_CALLBACK ark_web_touch_handle_state_get_touch_handle_type(
    struct _ark_web_touch_handle_state_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebTouchHandleStateCppToC::Get(self)->GetTouchHandleType();
}

} // namespace

ArkWebTouchHandleStateCppToC::ArkWebTouchHandleStateCppToC() {
  GetStruct()->get_x = ark_web_touch_handle_state_get_x;
  GetStruct()->get_y = ark_web_touch_handle_state_get_y;
  GetStruct()->is_enable = ark_web_touch_handle_state_is_enable;
  GetStruct()->get_alpha = ark_web_touch_handle_state_get_alpha;
  GetStruct()->get_edge_height = ark_web_touch_handle_state_get_edge_height;
  GetStruct()->get_view_port_x = ark_web_touch_handle_state_get_view_port_x;
  GetStruct()->get_view_port_y = ark_web_touch_handle_state_get_view_port_y;
  GetStruct()->get_touch_handle_id =
      ark_web_touch_handle_state_get_touch_handle_id;
  GetStruct()->get_touch_handle_type =
      ark_web_touch_handle_state_get_touch_handle_type;
}

ArkWebTouchHandleStateCppToC::~ArkWebTouchHandleStateCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkWebTouchHandleStateCppToC, ArkWebTouchHandleState,
                           ark_web_touch_handle_state_t>::kBridgeType =
        ARK_WEB_TOUCH_HANDLE_STATE;

} // namespace OHOS::ArkWeb
