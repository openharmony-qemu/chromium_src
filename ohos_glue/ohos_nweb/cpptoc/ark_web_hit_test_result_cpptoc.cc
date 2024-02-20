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

#include "ohos_nweb/cpptoc/ark_web_hit_test_result_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_web_hit_test_result_set_type(
    struct _ark_web_hit_test_result_t *self, int type) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHitTestResultCppToC::Get(self)->SetType(type);
}

int ARK_WEB_CALLBACK
ark_web_hit_test_result_get_type(struct _ark_web_hit_test_result_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebHitTestResultCppToC::Get(self)->GetType();
}

ArkWebString ARK_WEB_CALLBACK
ark_web_hit_test_result_get_extra(struct _ark_web_hit_test_result_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebHitTestResultCppToC::Get(self)->GetExtra();
}

} // namespace

ArkWebHitTestResultCppToC::ArkWebHitTestResultCppToC() {
  GetStruct()->set_type = ark_web_hit_test_result_set_type;
  GetStruct()->get_type = ark_web_hit_test_result_get_type;
  GetStruct()->get_extra = ark_web_hit_test_result_get_extra;
}

ArkWebHitTestResultCppToC::~ArkWebHitTestResultCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkWebHitTestResultCppToC, ArkWebHitTestResult,
                           ark_web_hit_test_result_t>::kBridgeType =
        ARK_WEB_HIT_TEST_RESULT;

} // namespace OHOS::ArkWeb
