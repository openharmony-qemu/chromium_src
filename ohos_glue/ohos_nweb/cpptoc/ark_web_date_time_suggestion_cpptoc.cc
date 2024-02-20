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

#include "ohos_nweb/cpptoc/ark_web_date_time_suggestion_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

ArkWebString ARK_WEB_CALLBACK ark_web_date_time_suggestion_get_label(
    struct _ark_web_date_time_suggestion_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebDateTimeSuggestionCppToC::Get(self)->GetLabel();
}

ArkWebDateTime ARK_WEB_CALLBACK ark_web_date_time_suggestion_get_value(
    struct _ark_web_date_time_suggestion_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_date_time_default);

  // Execute
  return ArkWebDateTimeSuggestionCppToC::Get(self)->GetValue();
}

ArkWebString ARK_WEB_CALLBACK ark_web_date_time_suggestion_get_localized_value(
    struct _ark_web_date_time_suggestion_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebDateTimeSuggestionCppToC::Get(self)->GetLocalizedValue();
}

} // namespace

ArkWebDateTimeSuggestionCppToC::ArkWebDateTimeSuggestionCppToC() {
  GetStruct()->get_label = ark_web_date_time_suggestion_get_label;
  GetStruct()->get_value = ark_web_date_time_suggestion_get_value;
  GetStruct()->get_localized_value =
      ark_web_date_time_suggestion_get_localized_value;
}

ArkWebDateTimeSuggestionCppToC::~ArkWebDateTimeSuggestionCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkWebDateTimeSuggestionCppToC, ArkWebDateTimeSuggestion,
    ark_web_date_time_suggestion_t>::kBridgeType = ARK_WEB_DATE_TIME_SUGGESTION;

} // namespace OHOS::ArkWeb
