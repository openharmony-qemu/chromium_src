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

#include "ohos_adapter/ctocpp/ark_screen_capture_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_screen_capture_callback_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_screen_capture_config_adapter_cpptoc.h"
#include "ohos_adapter/ctocpp/ark_surface_buffer_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int32_t ArkScreenCaptureAdapterCToCpp::Init(
    const ArkWebRefPtr<ArkScreenCaptureConfigAdapter> config) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_screen_capture_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, init, 0);

  // Execute
  return _struct->init(_struct,
                       ArkScreenCaptureConfigAdapterCppToC::Invert(config));
}

ARK_WEB_NO_SANITIZE
int32_t ArkScreenCaptureAdapterCToCpp::SetMicrophoneEnable(bool enable) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_screen_capture_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_microphone_enable, 0);

  // Execute
  return _struct->set_microphone_enable(_struct, enable);
}

ARK_WEB_NO_SANITIZE
int32_t ArkScreenCaptureAdapterCToCpp::StartCapture() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_screen_capture_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start_capture, 0);

  // Execute
  return _struct->start_capture(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkScreenCaptureAdapterCToCpp::StopCapture() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_screen_capture_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, stop_capture, 0);

  // Execute
  return _struct->stop_capture(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkScreenCaptureAdapterCToCpp::SetCaptureCallback(
    const ArkWebRefPtr<ArkScreenCaptureCallbackAdapter> callback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_screen_capture_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_capture_callback, 0);

  // Execute
  return _struct->set_capture_callback(
      _struct, ArkScreenCaptureCallbackAdapterCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkSurfaceBufferAdapter>
ArkScreenCaptureAdapterCToCpp::AcquireVideoBuffer() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_screen_capture_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, acquire_video_buffer, nullptr);

  // Execute
  ark_surface_buffer_adapter_t *_retval =
      _struct->acquire_video_buffer(_struct);

  // Return type: refptr_same
  return ArkSurfaceBufferAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
int32_t ArkScreenCaptureAdapterCToCpp::ReleaseVideoBuffer() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_screen_capture_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, release_video_buffer, 0);

  // Execute
  return _struct->release_video_buffer(_struct);
}

ArkScreenCaptureAdapterCToCpp::ArkScreenCaptureAdapterCToCpp() {
}

ArkScreenCaptureAdapterCToCpp::~ArkScreenCaptureAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkScreenCaptureAdapterCToCpp, ArkScreenCaptureAdapter,
    ark_screen_capture_adapter_t>::kBridgeType = ARK_SCREEN_CAPTURE_ADAPTER;

} // namespace OHOS::ArkWeb
