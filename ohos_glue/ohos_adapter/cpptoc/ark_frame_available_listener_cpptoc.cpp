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

#include "ohos_adapter/cpptoc/ark_frame_available_listener_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void *ARK_WEB_CALLBACK ark_frame_available_listener_get_context(
    struct _ark_frame_available_listener_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  return ArkFrameAvailableListenerCppToC::Get(self)->GetContext();
}

void *ARK_WEB_CALLBACK ark_frame_available_listener_get_on_frame_available_cb(
    struct _ark_frame_available_listener_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  return ArkFrameAvailableListenerCppToC::Get(self)->GetOnFrameAvailableCb();
}

} // namespace

ArkFrameAvailableListenerCppToC::ArkFrameAvailableListenerCppToC() {
  GetStruct()->get_context = ark_frame_available_listener_get_context;
  GetStruct()->get_on_frame_available_cb =
      ark_frame_available_listener_get_on_frame_available_cb;
}

ArkFrameAvailableListenerCppToC::~ArkFrameAvailableListenerCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkFrameAvailableListenerCppToC, ArkFrameAvailableListener,
    ark_frame_available_listener_t>::kBridgeType = ARK_FRAME_AVAILABLE_LISTENER;

} // namespace OHOS::ArkWeb
