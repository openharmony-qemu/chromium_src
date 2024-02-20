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

#include "ohos_nweb/cpptoc/ark_web_context_menu_params_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int32_t ARK_WEB_CALLBACK ark_web_context_menu_params_get_xcoord(
    struct _ark_web_context_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebContextMenuParamsCppToC::Get(self)->GetXCoord();
}

int32_t ARK_WEB_CALLBACK ark_web_context_menu_params_get_ycoord(
    struct _ark_web_context_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebContextMenuParamsCppToC::Get(self)->GetYCoord();
}

bool ARK_WEB_CALLBACK ark_web_context_menu_params_is_editable(
    struct _ark_web_context_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebContextMenuParamsCppToC::Get(self)->IsEditable();
}

ArkWebString ARK_WEB_CALLBACK ark_web_context_menu_params_get_link_url(
    struct _ark_web_context_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebContextMenuParamsCppToC::Get(self)->GetLinkUrl();
}

ArkWebString ARK_WEB_CALLBACK ark_web_context_menu_params_get_page_url(
    struct _ark_web_context_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebContextMenuParamsCppToC::Get(self)->GetPageUrl();
}

ArkWebString ARK_WEB_CALLBACK ark_web_context_menu_params_get_source_url(
    struct _ark_web_context_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebContextMenuParamsCppToC::Get(self)->GetSourceUrl();
}

ArkWebString ARK_WEB_CALLBACK ark_web_context_menu_params_get_title_text(
    struct _ark_web_context_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebContextMenuParamsCppToC::Get(self)->GetTitleText();
}

ArkWebString ARK_WEB_CALLBACK ark_web_context_menu_params_get_selection_text(
    struct _ark_web_context_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebContextMenuParamsCppToC::Get(self)->GetSelectionText();
}

int ARK_WEB_CALLBACK ark_web_context_menu_params_get_media_type(
    struct _ark_web_context_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebContextMenuParamsCppToC::Get(self)->GetMediaType();
}

int ARK_WEB_CALLBACK ark_web_context_menu_params_get_source_type(
    struct _ark_web_context_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebContextMenuParamsCppToC::Get(self)->GetSourceType();
}

int ARK_WEB_CALLBACK ark_web_context_menu_params_get_input_field_type(
    struct _ark_web_context_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebContextMenuParamsCppToC::Get(self)->GetInputFieldType();
}

bool ARK_WEB_CALLBACK ark_web_context_menu_params_has_image_contents(
    struct _ark_web_context_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebContextMenuParamsCppToC::Get(self)->HasImageContents();
}

int32_t ARK_WEB_CALLBACK ark_web_context_menu_params_get_edit_state_flags(
    struct _ark_web_context_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebContextMenuParamsCppToC::Get(self)->GetEditStateFlags();
}

ArkWebString ARK_WEB_CALLBACK
ark_web_context_menu_params_get_unfiltered_link_url(
    struct _ark_web_context_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebContextMenuParamsCppToC::Get(self)->GetUnfilteredLinkUrl();
}

int32_t ARK_WEB_CALLBACK
ark_web_context_menu_params_get_context_menu_type_flags(
    struct _ark_web_context_menu_params_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebContextMenuParamsCppToC::Get(self)->GetContextMenuTypeFlags();
}

} // namespace

ArkWebContextMenuParamsCppToC::ArkWebContextMenuParamsCppToC() {
  GetStruct()->get_xcoord = ark_web_context_menu_params_get_xcoord;
  GetStruct()->get_ycoord = ark_web_context_menu_params_get_ycoord;
  GetStruct()->is_editable = ark_web_context_menu_params_is_editable;
  GetStruct()->get_link_url = ark_web_context_menu_params_get_link_url;
  GetStruct()->get_page_url = ark_web_context_menu_params_get_page_url;
  GetStruct()->get_source_url = ark_web_context_menu_params_get_source_url;
  GetStruct()->get_title_text = ark_web_context_menu_params_get_title_text;
  GetStruct()->get_selection_text =
      ark_web_context_menu_params_get_selection_text;
  GetStruct()->get_media_type = ark_web_context_menu_params_get_media_type;
  GetStruct()->get_source_type = ark_web_context_menu_params_get_source_type;
  GetStruct()->get_input_field_type =
      ark_web_context_menu_params_get_input_field_type;
  GetStruct()->has_image_contents =
      ark_web_context_menu_params_has_image_contents;
  GetStruct()->get_edit_state_flags =
      ark_web_context_menu_params_get_edit_state_flags;
  GetStruct()->get_unfiltered_link_url =
      ark_web_context_menu_params_get_unfiltered_link_url;
  GetStruct()->get_context_menu_type_flags =
      ark_web_context_menu_params_get_context_menu_type_flags;
}

ArkWebContextMenuParamsCppToC::~ArkWebContextMenuParamsCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkWebContextMenuParamsCppToC, ArkWebContextMenuParams,
    ark_web_context_menu_params_t>::kBridgeType = ARK_WEB_CONTEXT_MENU_PARAMS;

} // namespace OHOS::ArkWeb
