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

#include "ohos_adapter/ctocpp/ark_audio_renderer_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_audio_renderer_callback_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_audio_renderer_options_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int32_t ArkAudioRendererAdapterCToCpp::Create(
    const ArkWebRefPtr<ArkAudioRendererOptionsAdapter> options,
    ArkWebString &str) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_renderer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create, 0);

  // Execute
  return _struct->create(
      _struct, ArkAudioRendererOptionsAdapterCppToC::Invert(options), &str);
}

ARK_WEB_NO_SANITIZE
bool ArkAudioRendererAdapterCToCpp::Start() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_renderer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start, false);

  // Execute
  return _struct->start(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkAudioRendererAdapterCToCpp::Pause() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_renderer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, pause, false);

  // Execute
  return _struct->pause(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkAudioRendererAdapterCToCpp::Stop() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_renderer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, stop, false);

  // Execute
  return _struct->stop(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkAudioRendererAdapterCToCpp::Release2() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_renderer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, release2, false);

  // Execute
  return _struct->release2(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkAudioRendererAdapterCToCpp::Write(uint8_t *buffer,
                                             size_t bufferSize) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_renderer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, write, 0);

  // Execute
  return _struct->write(_struct, buffer, bufferSize);
}

ARK_WEB_NO_SANITIZE
int32_t ArkAudioRendererAdapterCToCpp::GetLatency(uint64_t &latency) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_renderer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_latency, 0);

  // Execute
  return _struct->get_latency(_struct, &latency);
}

ARK_WEB_NO_SANITIZE
int32_t ArkAudioRendererAdapterCToCpp::SetVolume(float volume) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_renderer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_volume, 0);

  // Execute
  return _struct->set_volume(_struct, volume);
}

ARK_WEB_NO_SANITIZE
float ArkAudioRendererAdapterCToCpp::GetVolume() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_renderer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_volume, 0);

  // Execute
  return _struct->get_volume(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkAudioRendererAdapterCToCpp::SetAudioRendererCallback(
    const ArkWebRefPtr<ArkAudioRendererCallbackAdapter> callback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_renderer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_audio_renderer_callback, 0);

  // Execute
  return _struct->set_audio_renderer_callback(
      _struct, ArkAudioRendererCallbackAdapterCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
void ArkAudioRendererAdapterCToCpp::SetInterruptMode(bool audioExclusive) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_renderer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_interrupt_mode, );

  // Execute
  _struct->set_interrupt_mode(_struct, audioExclusive);
}

ARK_WEB_NO_SANITIZE
bool ArkAudioRendererAdapterCToCpp::IsRendererStateRunning() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_renderer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, is_renderer_state_running, false);

  // Execute
  return _struct->is_renderer_state_running(_struct);
}

ArkAudioRendererAdapterCToCpp::ArkAudioRendererAdapterCToCpp() {
}

ArkAudioRendererAdapterCToCpp::~ArkAudioRendererAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkAudioRendererAdapterCToCpp, ArkAudioRendererAdapter,
    ark_audio_renderer_adapter_t>::kBridgeType = ARK_AUDIO_RENDERER_ADAPTER;

} // namespace OHOS::ArkWeb
