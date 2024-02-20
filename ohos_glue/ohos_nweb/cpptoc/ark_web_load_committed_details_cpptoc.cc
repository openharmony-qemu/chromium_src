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

#include "ohos_nweb/cpptoc/ark_web_load_committed_details_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

bool ARK_WEB_CALLBACK ark_web_load_committed_details_is_main_frame(
    struct _ark_web_load_committed_details_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebLoadCommittedDetailsCppToC::Get(self)->IsMainFrame();
}

bool ARK_WEB_CALLBACK ark_web_load_committed_details_is_same_document(
    struct _ark_web_load_committed_details_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebLoadCommittedDetailsCppToC::Get(self)->IsSameDocument();
}

bool ARK_WEB_CALLBACK ark_web_load_committed_details_did_replace_entry(
    struct _ark_web_load_committed_details_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebLoadCommittedDetailsCppToC::Get(self)->DidReplaceEntry();
}

int ARK_WEB_CALLBACK ark_web_load_committed_details_get_navigation_type(
    struct _ark_web_load_committed_details_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebLoadCommittedDetailsCppToC::Get(self)->GetNavigationType();
}

ArkWebString ARK_WEB_CALLBACK ark_web_load_committed_details_get_url(
    struct _ark_web_load_committed_details_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebLoadCommittedDetailsCppToC::Get(self)->GetURL();
}

} // namespace

ArkWebLoadCommittedDetailsCppToC::ArkWebLoadCommittedDetailsCppToC() {
  GetStruct()->is_main_frame = ark_web_load_committed_details_is_main_frame;
  GetStruct()->is_same_document =
      ark_web_load_committed_details_is_same_document;
  GetStruct()->did_replace_entry =
      ark_web_load_committed_details_did_replace_entry;
  GetStruct()->get_navigation_type =
      ark_web_load_committed_details_get_navigation_type;
  GetStruct()->get_url = ark_web_load_committed_details_get_url;
}

ArkWebLoadCommittedDetailsCppToC::~ArkWebLoadCommittedDetailsCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkWebLoadCommittedDetailsCppToC, ArkWebLoadCommittedDetails,
    ark_web_load_committed_details_t>::kBridgeType =
    ARK_WEB_LOAD_COMMITTED_DETAILS;

} // namespace OHOS::ArkWeb
