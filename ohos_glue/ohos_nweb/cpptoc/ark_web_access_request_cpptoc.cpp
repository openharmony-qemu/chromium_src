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

#include "ohos_nweb/cpptoc/ark_web_access_request_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_web_access_request_agree(
    struct _ark_web_access_request_t *self, int resource_id) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebAccessRequestCppToC::Get(self)->Agree(resource_id);
}

ArkWebString ARK_WEB_CALLBACK
ark_web_access_request_origin(struct _ark_web_access_request_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebAccessRequestCppToC::Get(self)->Origin();
}

void ARK_WEB_CALLBACK
ark_web_access_request_refuse(struct _ark_web_access_request_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebAccessRequestCppToC::Get(self)->Refuse();
}

int ARK_WEB_CALLBACK ark_web_access_request_resource_access_id(
    struct _ark_web_access_request_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebAccessRequestCppToC::Get(self)->ResourceAccessId();
}

} // namespace

ArkWebAccessRequestCppToC::ArkWebAccessRequestCppToC() {
  GetStruct()->agree = ark_web_access_request_agree;
  GetStruct()->origin = ark_web_access_request_origin;
  GetStruct()->refuse = ark_web_access_request_refuse;
  GetStruct()->resource_access_id = ark_web_access_request_resource_access_id;
}

ArkWebAccessRequestCppToC::~ArkWebAccessRequestCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkWebAccessRequestCppToC, ArkWebAccessRequest,
                           ark_web_access_request_t>::kBridgeType =
        ARK_WEB_ACCESS_REQUEST;

} // namespace OHOS::ArkWeb
