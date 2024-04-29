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

#include "ohos_adapter/cpptoc/ark_audio_renderer_callback_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_audio_renderer_callback_adapter_on_suspend(
    struct _ark_audio_renderer_callback_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkAudioRendererCallbackAdapterCppToC::Get(self)->OnSuspend();
}

void ARK_WEB_CALLBACK ark_audio_renderer_callback_adapter_on_resume(
    struct _ark_audio_renderer_callback_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkAudioRendererCallbackAdapterCppToC::Get(self)->OnResume();
}

} // namespace

ArkAudioRendererCallbackAdapterCppToC::ArkAudioRendererCallbackAdapterCppToC() {
  GetStruct()->on_suspend = ark_audio_renderer_callback_adapter_on_suspend;
  GetStruct()->on_resume = ark_audio_renderer_callback_adapter_on_resume;
}

ArkAudioRendererCallbackAdapterCppToC::
    ~ArkAudioRendererCallbackAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkAudioRendererCallbackAdapterCppToC, ArkAudioRendererCallbackAdapter,
    ark_audio_renderer_callback_adapter_t>::kBridgeType =
    ARK_AUDIO_RENDERER_CALLBACK_ADAPTER;

} // namespace OHOS::ArkWeb
