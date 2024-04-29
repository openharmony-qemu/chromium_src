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

#include "ohos_adapter/cpptoc/ark_audio_info_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_adapter/cpptoc/ark_audio_capture_info_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_audio_enc_info_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

namespace {

ark_audio_capture_info_adapter_t *ARK_WEB_CALLBACK
ark_audio_info_adapter_get_mic_cap_info(
    struct _ark_audio_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkAudioCaptureInfoAdapter> _retval =
      ArkAudioInfoAdapterCppToC::Get(self)->GetMicCapInfo();

  // Return type: refptr_same
  return ArkAudioCaptureInfoAdapterCppToC::Invert(_retval);
}

ark_audio_capture_info_adapter_t *ARK_WEB_CALLBACK
ark_audio_info_adapter_get_inner_cap_info(
    struct _ark_audio_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkAudioCaptureInfoAdapter> _retval =
      ArkAudioInfoAdapterCppToC::Get(self)->GetInnerCapInfo();

  // Return type: refptr_same
  return ArkAudioCaptureInfoAdapterCppToC::Invert(_retval);
}

ark_audio_enc_info_adapter_t *ARK_WEB_CALLBACK
ark_audio_info_adapter_get_audio_enc_info(
    struct _ark_audio_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkAudioEncInfoAdapter> _retval =
      ArkAudioInfoAdapterCppToC::Get(self)->GetAudioEncInfo();

  // Return type: refptr_same
  return ArkAudioEncInfoAdapterCppToC::Invert(_retval);
}

} // namespace

ArkAudioInfoAdapterCppToC::ArkAudioInfoAdapterCppToC() {
  GetStruct()->get_mic_cap_info = ark_audio_info_adapter_get_mic_cap_info;
  GetStruct()->get_inner_cap_info = ark_audio_info_adapter_get_inner_cap_info;
  GetStruct()->get_audio_enc_info = ark_audio_info_adapter_get_audio_enc_info;
}

ArkAudioInfoAdapterCppToC::~ArkAudioInfoAdapterCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkAudioInfoAdapterCppToC, ArkAudioInfoAdapter,
                           ark_audio_info_adapter_t>::kBridgeType =
        ARK_AUDIO_INFO_ADAPTER;

} // namespace OHOS::ArkWeb
