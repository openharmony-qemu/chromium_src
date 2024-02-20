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

#include "ohos_nweb/cpptoc/ark_web_quick_menu_params_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_nweb/cpptoc/ark_web_touch_handle_state_cpptoc.h"

namespace OHOS::ArkWeb {

namespace {

int32_t ARK_WEB_CALLBACK ark_web_quick_menu_params_get_xcoord(
    struct _ark_web_quick_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebQuickMenuParamsCppToC::Get(self)->GetXCoord();
}

int32_t ARK_WEB_CALLBACK ark_web_quick_menu_params_get_ycoord(
    struct _ark_web_quick_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebQuickMenuParamsCppToC::Get(self)->GetYCoord();
}

int32_t ARK_WEB_CALLBACK
ark_web_quick_menu_params_get_width(struct _ark_web_quick_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebQuickMenuParamsCppToC::Get(self)->GetWidth();
}

int32_t ARK_WEB_CALLBACK ark_web_quick_menu_params_get_height(
    struct _ark_web_quick_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebQuickMenuParamsCppToC::Get(self)->GetHeight();
}

int32_t ARK_WEB_CALLBACK ark_web_quick_menu_params_get_edit_state_flags(
    struct _ark_web_quick_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebQuickMenuParamsCppToC::Get(self)->GetEditStateFlags();
}

int32_t ARK_WEB_CALLBACK ark_web_quick_menu_params_get_select_x(
    struct _ark_web_quick_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebQuickMenuParamsCppToC::Get(self)->GetSelectX();
}

int32_t ARK_WEB_CALLBACK ark_web_quick_menu_params_get_select_y(
    struct _ark_web_quick_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebQuickMenuParamsCppToC::Get(self)->GetSelectY();
}

int32_t ARK_WEB_CALLBACK ark_web_quick_menu_params_get_select_width(
    struct _ark_web_quick_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebQuickMenuParamsCppToC::Get(self)->GetSelectWidth();
}

int32_t ARK_WEB_CALLBACK ark_web_quick_menu_params_get_select_xheight(
    struct _ark_web_quick_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebQuickMenuParamsCppToC::Get(self)->GetSelectXHeight();
}

ark_web_touch_handle_state_t *ARK_WEB_CALLBACK
ark_web_quick_menu_params_get_touch_handle_state(
    struct _ark_web_quick_menu_params_t *self, int type) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebTouchHandleState> _retval =
      ArkWebQuickMenuParamsCppToC::Get(self)->GetTouchHandleState(type);

  // Return type: refptr_same
  return ArkWebTouchHandleStateCppToC::Invert(_retval);
}

} // namespace

ArkWebQuickMenuParamsCppToC::ArkWebQuickMenuParamsCppToC() {
  GetStruct()->get_xcoord = ark_web_quick_menu_params_get_xcoord;
  GetStruct()->get_ycoord = ark_web_quick_menu_params_get_ycoord;
  GetStruct()->get_width = ark_web_quick_menu_params_get_width;
  GetStruct()->get_height = ark_web_quick_menu_params_get_height;
  GetStruct()->get_edit_state_flags =
      ark_web_quick_menu_params_get_edit_state_flags;
  GetStruct()->get_select_x = ark_web_quick_menu_params_get_select_x;
  GetStruct()->get_select_y = ark_web_quick_menu_params_get_select_y;
  GetStruct()->get_select_width = ark_web_quick_menu_params_get_select_width;
  GetStruct()->get_select_xheight =
      ark_web_quick_menu_params_get_select_xheight;
  GetStruct()->get_touch_handle_state =
      ark_web_quick_menu_params_get_touch_handle_state;
}

ArkWebQuickMenuParamsCppToC::~ArkWebQuickMenuParamsCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkWebQuickMenuParamsCppToC, ArkWebQuickMenuParams,
                           ark_web_quick_menu_params_t>::kBridgeType =
        ARK_WEB_QUICK_MENU_PARAMS;

} // namespace OHOS::ArkWeb
