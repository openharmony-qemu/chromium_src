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

#include "ohos_nweb/cpptoc/ark_web_first_meaningful_paint_details_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int64_t ARK_WEB_CALLBACK
ark_web_first_meaningful_paint_details_get_navigation_start_time(
    struct _ark_web_first_meaningful_paint_details_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebFirstMeaningfulPaintDetailsCppToC::Get(self)
      ->GetNavigationStartTime();
}

int64_t ARK_WEB_CALLBACK
ark_web_first_meaningful_paint_details_get_first_meaningful_paint_time(
    struct _ark_web_first_meaningful_paint_details_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebFirstMeaningfulPaintDetailsCppToC::Get(self)
      ->GetFirstMeaningfulPaintTime();
}

} // namespace

ArkWebFirstMeaningfulPaintDetailsCppToC::
    ArkWebFirstMeaningfulPaintDetailsCppToC() {
  GetStruct()->get_navigation_start_time =
      ark_web_first_meaningful_paint_details_get_navigation_start_time;
  GetStruct()->get_first_meaningful_paint_time =
      ark_web_first_meaningful_paint_details_get_first_meaningful_paint_time;
}

ArkWebFirstMeaningfulPaintDetailsCppToC::
    ~ArkWebFirstMeaningfulPaintDetailsCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkWebFirstMeaningfulPaintDetailsCppToC, ArkWebFirstMeaningfulPaintDetails,
    ark_web_first_meaningful_paint_details_t>::kBridgeType =
    ARK_WEB_FIRST_MEANINGFUL_PAINT_DETAILS;

} // namespace OHOS::ArkWeb
