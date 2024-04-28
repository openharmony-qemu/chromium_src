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

#include "ohos_adapter/cpptoc/ark_screen_capture_callback_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_screen_capture_callback_adapter_on_error(
    struct _ark_screen_capture_callback_adapter_t *self, int32_t errorCode) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkScreenCaptureCallbackAdapterCppToC::Get(self)->OnError(errorCode);
}

void ARK_WEB_CALLBACK
ark_screen_capture_callback_adapter_on_audio_buffer_available(
    struct _ark_screen_capture_callback_adapter_t *self, bool isReady,
    int32_t type) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkScreenCaptureCallbackAdapterCppToC::Get(self)->OnAudioBufferAvailable(
      isReady, type);
}

void ARK_WEB_CALLBACK
ark_screen_capture_callback_adapter_on_video_buffer_available(
    struct _ark_screen_capture_callback_adapter_t *self, bool isReady) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkScreenCaptureCallbackAdapterCppToC::Get(self)->OnVideoBufferAvailable(
      isReady);
}

} // namespace

ArkScreenCaptureCallbackAdapterCppToC::ArkScreenCaptureCallbackAdapterCppToC() {
  GetStruct()->on_error = ark_screen_capture_callback_adapter_on_error;
  GetStruct()->on_audio_buffer_available =
      ark_screen_capture_callback_adapter_on_audio_buffer_available;
  GetStruct()->on_video_buffer_available =
      ark_screen_capture_callback_adapter_on_video_buffer_available;
}

ArkScreenCaptureCallbackAdapterCppToC::
    ~ArkScreenCaptureCallbackAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkScreenCaptureCallbackAdapterCppToC, ArkScreenCaptureCallbackAdapter,
    ark_screen_capture_callback_adapter_t>::kBridgeType =
    ARK_SCREEN_CAPTURE_CALLBACK_ADAPTER;

} // namespace OHOS::ArkWeb
