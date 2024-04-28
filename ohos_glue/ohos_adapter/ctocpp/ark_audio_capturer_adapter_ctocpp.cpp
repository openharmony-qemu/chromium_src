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

#include "ohos_adapter/ctocpp/ark_audio_capturer_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_audio_capturer_options_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_audio_capturer_read_callback_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_buffer_desc_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
int32_t ArkAudioCapturerAdapterCToCpp::Create(
    const ArkWebRefPtr<ArkAudioCapturerOptionsAdapter> capturerOptions,
    ArkWebString &cachePath) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_capturer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create, 0);

  // Execute
  return _struct->create(
      _struct, ArkAudioCapturerOptionsAdapterCppToC::Invert(capturerOptions),
      &cachePath);
}

ARK_WEB_NO_SANITIZE
bool ArkAudioCapturerAdapterCToCpp::Start() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_capturer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, start, false);

  // Execute
  return _struct->start(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkAudioCapturerAdapterCToCpp::Stop() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_capturer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, stop, false);

  // Execute
  return _struct->stop(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkAudioCapturerAdapterCToCpp::Release2() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_capturer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, release2, false);

  // Execute
  return _struct->release2(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkAudioCapturerAdapterCToCpp::SetCapturerReadCallback(
    ArkWebRefPtr<ArkAudioCapturerReadCallbackAdapter> callbck) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_capturer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_capturer_read_callback, 0);

  // Execute
  return _struct->set_capturer_read_callback(
      _struct, ArkAudioCapturerReadCallbackAdapterCppToC::Invert(callbck));
}

ARK_WEB_NO_SANITIZE
int32_t ArkAudioCapturerAdapterCToCpp::GetBufferDesc(
    ArkWebRefPtr<ArkBufferDescAdapter> buffferDesc) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_capturer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_buffer_desc, 0);

  // Execute
  return _struct->get_buffer_desc(
      _struct, ArkBufferDescAdapterCppToC::Invert(buffferDesc));
}

ARK_WEB_NO_SANITIZE
int32_t ArkAudioCapturerAdapterCToCpp::Enqueue(
    const ArkWebRefPtr<ArkBufferDescAdapter> bufferDesc) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_capturer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, enqueue, 0);

  // Execute
  return _struct->enqueue(_struct,
                          ArkBufferDescAdapterCppToC::Invert(bufferDesc));
}

ARK_WEB_NO_SANITIZE
int32_t ArkAudioCapturerAdapterCToCpp::GetFrameCount(uint32_t &frameCount) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_capturer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_frame_count, 0);

  // Execute
  return _struct->get_frame_count(_struct, &frameCount);
}

ARK_WEB_NO_SANITIZE
int64_t ArkAudioCapturerAdapterCToCpp::GetAudioTime() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_capturer_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_audio_time, 0);

  // Execute
  return _struct->get_audio_time(_struct);
}

ArkAudioCapturerAdapterCToCpp::ArkAudioCapturerAdapterCToCpp() {
}

ArkAudioCapturerAdapterCToCpp::~ArkAudioCapturerAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkAudioCapturerAdapterCToCpp, ArkAudioCapturerAdapter,
    ark_audio_capturer_adapter_t>::kBridgeType = ARK_AUDIO_CAPTURER_ADAPTER;

} // namespace OHOS::ArkWeb
