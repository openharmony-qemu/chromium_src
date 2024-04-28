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

#include "ohos_nweb/cpptoc/ark_web_history_item_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

bool ARK_WEB_CALLBACK ark_web_history_item_get_favicon(
    struct _ark_web_history_item_t *self, void **data, int *width, int *height,
    int *color_type, int *alpha_type) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(data, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(width, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(height, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(color_type, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(alpha_type, false);

  // Execute
  return ArkWebHistoryItemCppToC::Get(self)->GetFavicon(
      data, *width, *height, *color_type, *alpha_type);
}

ArkWebString ARK_WEB_CALLBACK
ark_web_history_item_get_history_url(struct _ark_web_history_item_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebHistoryItemCppToC::Get(self)->GetHistoryUrl();
}

ArkWebString ARK_WEB_CALLBACK
ark_web_history_item_get_history_title(struct _ark_web_history_item_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebHistoryItemCppToC::Get(self)->GetHistoryTitle();
}

ArkWebString ARK_WEB_CALLBACK
ark_web_history_item_get_history_raw_url(struct _ark_web_history_item_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebHistoryItemCppToC::Get(self)->GetHistoryRawUrl();
}

} // namespace

ArkWebHistoryItemCppToC::ArkWebHistoryItemCppToC() {
  GetStruct()->get_favicon = ark_web_history_item_get_favicon;
  GetStruct()->get_history_url = ark_web_history_item_get_history_url;
  GetStruct()->get_history_title = ark_web_history_item_get_history_title;
  GetStruct()->get_history_raw_url = ark_web_history_item_get_history_raw_url;
}

ArkWebHistoryItemCppToC::~ArkWebHistoryItemCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkWebHistoryItemCppToC, ArkWebHistoryItem,
                           ark_web_history_item_t>::kBridgeType =
        ARK_WEB_HISTORY_ITEM;

} // namespace OHOS::ArkWeb
