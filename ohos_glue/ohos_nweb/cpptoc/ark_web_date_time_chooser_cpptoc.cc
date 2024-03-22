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

#include "ohos_nweb/cpptoc/ark_web_date_time_chooser_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int ARK_WEB_CALLBACK
ark_web_date_time_chooser_get_type(struct _ark_web_date_time_chooser_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebDateTimeChooserCppToC::Get(self)->GetType();
}

double ARK_WEB_CALLBACK
ark_web_date_time_chooser_get_step(struct _ark_web_date_time_chooser_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebDateTimeChooserCppToC::Get(self)->GetStep();
}

ArkWebDateTime ARK_WEB_CALLBACK ark_web_date_time_chooser_get_minimum(
    struct _ark_web_date_time_chooser_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_date_time_default);

  // Execute
  return ArkWebDateTimeChooserCppToC::Get(self)->GetMinimum();
}

ArkWebDateTime ARK_WEB_CALLBACK ark_web_date_time_chooser_get_maximum(
    struct _ark_web_date_time_chooser_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_date_time_default);

  // Execute
  return ArkWebDateTimeChooserCppToC::Get(self)->GetMaximum();
}

ArkWebDateTime ARK_WEB_CALLBACK ark_web_date_time_chooser_get_dialog_value(
    struct _ark_web_date_time_chooser_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_date_time_default);

  // Execute
  return ArkWebDateTimeChooserCppToC::Get(self)->GetDialogValue();
}

bool ARK_WEB_CALLBACK ark_web_date_time_chooser_get_has_selected(
    struct _ark_web_date_time_chooser_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebDateTimeChooserCppToC::Get(self)->GetHasSelected();
}

size_t ARK_WEB_CALLBACK ark_web_date_time_chooser_get_suggestion_index(
    struct _ark_web_date_time_chooser_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebDateTimeChooserCppToC::Get(self)->GetSuggestionIndex();
}

} // namespace

ArkWebDateTimeChooserCppToC::ArkWebDateTimeChooserCppToC() {
  GetStruct()->get_type = ark_web_date_time_chooser_get_type;
  GetStruct()->get_step = ark_web_date_time_chooser_get_step;
  GetStruct()->get_minimum = ark_web_date_time_chooser_get_minimum;
  GetStruct()->get_maximum = ark_web_date_time_chooser_get_maximum;
  GetStruct()->get_dialog_value = ark_web_date_time_chooser_get_dialog_value;
  GetStruct()->get_has_selected = ark_web_date_time_chooser_get_has_selected;
  GetStruct()->get_suggestion_index =
      ark_web_date_time_chooser_get_suggestion_index;
}

ArkWebDateTimeChooserCppToC::~ArkWebDateTimeChooserCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkWebDateTimeChooserCppToC, ArkWebDateTimeChooser,
                           ark_web_date_time_chooser_t>::kBridgeType =
        ARK_WEB_DATE_TIME_CHOOSER;

} // namespace OHOS::ArkWeb
