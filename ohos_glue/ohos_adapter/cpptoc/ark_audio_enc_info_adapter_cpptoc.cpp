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

#include "ohos_adapter/cpptoc/ark_audio_enc_info_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int32_t ARK_WEB_CALLBACK ark_audio_enc_info_adapter_get_audio_bitrate(
    struct _ark_audio_enc_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkAudioEncInfoAdapterCppToC::Get(self)->GetAudioBitrate();
}

int32_t ARK_WEB_CALLBACK ark_audio_enc_info_adapter_get_audio_codecformat(
    struct _ark_audio_enc_info_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkAudioEncInfoAdapterCppToC::Get(self)->GetAudioCodecformat();
}

} // namespace

ArkAudioEncInfoAdapterCppToC::ArkAudioEncInfoAdapterCppToC() {
  GetStruct()->get_audio_bitrate = ark_audio_enc_info_adapter_get_audio_bitrate;
  GetStruct()->get_audio_codecformat =
      ark_audio_enc_info_adapter_get_audio_codecformat;
}

ArkAudioEncInfoAdapterCppToC::~ArkAudioEncInfoAdapterCppToC() {
}

template <>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkAudioEncInfoAdapterCppToC, ArkAudioEncInfoAdapter,
                           ark_audio_enc_info_adapter_t>::kBridgeType =
        ARK_AUDIO_ENC_INFO_ADAPTER;

} // namespace OHOS::ArkWeb
