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

#include "ohos_adapter/cpptoc/ark_imftext_config_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_adapter/cpptoc/ark_imfcursor_info_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_imfinput_attribute_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_imfselection_range_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

namespace {

ark_imfinput_attribute_adapter_t *ARK_WEB_CALLBACK
ark_imftext_config_adapter_get_input_attribute(
    struct _ark_imftext_config_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkIMFInputAttributeAdapter> _retval =
      ArkIMFTextConfigAdapterCppToC::Get(self)->GetInputAttribute();

  // Return type: refptr_same
  return ArkIMFInputAttributeAdapterCppToC::Invert(_retval);
}

ark_imfcursor_info_adapter_t *ARK_WEB_CALLBACK
ark_imftext_config_adapter_get_cursor_info(
    struct _ark_imftext_config_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkIMFCursorInfoAdapter> _retval =
      ArkIMFTextConfigAdapterCppToC::Get(self)->GetCursorInfo();

  // Return type: refptr_same
  return ArkIMFCursorInfoAdapterCppToC::Invert(_retval);
}

ark_imfselection_range_adapter_t *ARK_WEB_CALLBACK
ark_imftext_config_adapter_get_selection_range(
    struct _ark_imftext_config_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkIMFSelectionRangeAdapter> _retval =
      ArkIMFTextConfigAdapterCppToC::Get(self)->GetSelectionRange();

  // Return type: refptr_same
  return ArkIMFSelectionRangeAdapterCppToC::Invert(_retval);
}

uint32_t ARK_WEB_CALLBACK ark_imftext_config_adapter_get_window_id(
    struct _ark_imftext_config_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkIMFTextConfigAdapterCppToC::Get(self)->GetWindowId();
}

} // namespace

ArkIMFTextConfigAdapterCppToC::ArkIMFTextConfigAdapterCppToC() {
  GetStruct()->get_input_attribute =
      ark_imftext_config_adapter_get_input_attribute;
  GetStruct()->get_cursor_info = ark_imftext_config_adapter_get_cursor_info;
  GetStruct()->get_selection_range =
      ark_imftext_config_adapter_get_selection_range;
  GetStruct()->get_window_id = ark_imftext_config_adapter_get_window_id;
}

ArkIMFTextConfigAdapterCppToC::~ArkIMFTextConfigAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkIMFTextConfigAdapterCppToC, ArkIMFTextConfigAdapter,
    ark_imftext_config_adapter_t>::kBridgeType = ARK_IMFTEXT_CONFIG_ADAPTER;

} // namespace OHOS::ArkWeb
