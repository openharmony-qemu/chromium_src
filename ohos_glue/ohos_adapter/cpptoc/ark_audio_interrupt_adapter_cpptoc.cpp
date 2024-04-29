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

#include "ohos_adapter/cpptoc/ark_audio_interrupt_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int32_t ARK_WEB_CALLBACK ark_audio_interrupt_adapter_get_stream_usage(
    struct _ark_audio_interrupt_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkAudioInterruptAdapterCppToC::Get(self)->GetStreamUsage();
}

int32_t ARK_WEB_CALLBACK ark_audio_interrupt_adapter_get_content_type(
    struct _ark_audio_interrupt_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkAudioInterruptAdapterCppToC::Get(self)->GetContentType();
}

int32_t ARK_WEB_CALLBACK ark_audio_interrupt_adapter_get_stream_type(
    struct _ark_audio_interrupt_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkAudioInterruptAdapterCppToC::Get(self)->GetStreamType();
}

uint32_t ARK_WEB_CALLBACK ark_audio_interrupt_adapter_get_session_id(
    struct _ark_audio_interrupt_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkAudioInterruptAdapterCppToC::Get(self)->GetSessionID();
}

bool ARK_WEB_CALLBACK ark_audio_interrupt_adapter_get_pause_when_ducked(
    struct _ark_audio_interrupt_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkAudioInterruptAdapterCppToC::Get(self)->GetPauseWhenDucked();
}

} // namespace

ArkAudioInterruptAdapterCppToC::ArkAudioInterruptAdapterCppToC() {
  GetStruct()->get_stream_usage = ark_audio_interrupt_adapter_get_stream_usage;
  GetStruct()->get_content_type = ark_audio_interrupt_adapter_get_content_type;
  GetStruct()->get_stream_type = ark_audio_interrupt_adapter_get_stream_type;
  GetStruct()->get_session_id = ark_audio_interrupt_adapter_get_session_id;
  GetStruct()->get_pause_when_ducked =
      ark_audio_interrupt_adapter_get_pause_when_ducked;
}

ArkAudioInterruptAdapterCppToC::~ArkAudioInterruptAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkAudioInterruptAdapterCppToC, ArkAudioInterruptAdapter,
    ark_audio_interrupt_adapter_t>::kBridgeType = ARK_AUDIO_INTERRUPT_ADAPTER;

} // namespace OHOS::ArkWeb
