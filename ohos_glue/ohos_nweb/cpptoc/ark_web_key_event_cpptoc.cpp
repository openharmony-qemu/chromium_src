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

#include "ohos_nweb/cpptoc/ark_web_key_event_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int32_t ARK_WEB_CALLBACK
ark_web_key_event_get_action(struct _ark_web_key_event_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebKeyEventCppToC::Get(self)->GetAction();
}

int32_t ARK_WEB_CALLBACK
ark_web_key_event_get_key_code(struct _ark_web_key_event_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebKeyEventCppToC::Get(self)->GetKeyCode();
}

} // namespace

ArkWebKeyEventCppToC::ArkWebKeyEventCppToC() {
  GetStruct()->get_action = ark_web_key_event_get_action;
  GetStruct()->get_key_code = ark_web_key_event_get_key_code;
}

ArkWebKeyEventCppToC::~ArkWebKeyEventCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<ArkWebKeyEventCppToC, ArkWebKeyEvent,
                                        ark_web_key_event_t>::kBridgeType =
    ARK_WEB_KEY_EVENT;

} // namespace OHOS::ArkWeb
