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

#include "ohos_nweb/cpptoc/ark_web_select_popup_menu_param_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_nweb/cpptoc/ark_web_select_menu_bound_cpptoc.h"

namespace OHOS::ArkWeb {

namespace {

ArkWebSelectPopupMenuItemVector ARK_WEB_CALLBACK
ark_web_select_popup_menu_param_get_menu_items(
    struct _ark_web_select_popup_menu_param_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self,
                             ark_web_select_popup_menu_item_vector_default);

  // Execute
  return ArkWebSelectPopupMenuParamCppToC::Get(self)->GetMenuItems();
}

int ARK_WEB_CALLBACK ark_web_select_popup_menu_param_get_item_height(
    struct _ark_web_select_popup_menu_param_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebSelectPopupMenuParamCppToC::Get(self)->GetItemHeight();
}

int ARK_WEB_CALLBACK ark_web_select_popup_menu_param_get_selected_item(
    struct _ark_web_select_popup_menu_param_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebSelectPopupMenuParamCppToC::Get(self)->GetSelectedItem();
}

double ARK_WEB_CALLBACK ark_web_select_popup_menu_param_get_item_font_size(
    struct _ark_web_select_popup_menu_param_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebSelectPopupMenuParamCppToC::Get(self)->GetItemFontSize();
}

bool ARK_WEB_CALLBACK ark_web_select_popup_menu_param_get_is_right_aligned(
    struct _ark_web_select_popup_menu_param_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebSelectPopupMenuParamCppToC::Get(self)->GetIsRightAligned();
}

ark_web_select_menu_bound_t *ARK_WEB_CALLBACK
ark_web_select_popup_menu_param_get_select_menu_bound(
    struct _ark_web_select_popup_menu_param_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebSelectMenuBound> _retval =
      ArkWebSelectPopupMenuParamCppToC::Get(self)->GetSelectMenuBound();

  // Return type: refptr_same
  return ArkWebSelectMenuBoundCppToC::Invert(_retval);
}

bool ARK_WEB_CALLBACK
ark_web_select_popup_menu_param_get_is_allow_multiple_selection(
    struct _ark_web_select_popup_menu_param_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebSelectPopupMenuParamCppToC::Get(self)
      ->GetIsAllowMultipleSelection();
}

} // namespace

ArkWebSelectPopupMenuParamCppToC::ArkWebSelectPopupMenuParamCppToC() {
  GetStruct()->get_menu_items = ark_web_select_popup_menu_param_get_menu_items;
  GetStruct()->get_item_height =
      ark_web_select_popup_menu_param_get_item_height;
  GetStruct()->get_selected_item =
      ark_web_select_popup_menu_param_get_selected_item;
  GetStruct()->get_item_font_size =
      ark_web_select_popup_menu_param_get_item_font_size;
  GetStruct()->get_is_right_aligned =
      ark_web_select_popup_menu_param_get_is_right_aligned;
  GetStruct()->get_select_menu_bound =
      ark_web_select_popup_menu_param_get_select_menu_bound;
  GetStruct()->get_is_allow_multiple_selection =
      ark_web_select_popup_menu_param_get_is_allow_multiple_selection;
}

ArkWebSelectPopupMenuParamCppToC::~ArkWebSelectPopupMenuParamCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkWebSelectPopupMenuParamCppToC, ArkWebSelectPopupMenuParam,
    ark_web_select_popup_menu_param_t>::kBridgeType =
    ARK_WEB_SELECT_POPUP_MENU_PARAM;

} // namespace OHOS::ArkWeb
