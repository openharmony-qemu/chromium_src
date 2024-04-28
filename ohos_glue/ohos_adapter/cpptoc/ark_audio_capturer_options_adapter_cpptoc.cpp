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

#include "ohos_adapter/cpptoc/ark_audio_capturer_options_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

int32_t ARK_WEB_CALLBACK ark_audio_capturer_options_adapter_get_sampling_rate(
    struct _ark_audio_capturer_options_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkAudioCapturerOptionsAdapterCppToC::Get(self)->GetSamplingRate();
}

int32_t ARK_WEB_CALLBACK ark_audio_capturer_options_adapter_get_encoding(
    struct _ark_audio_capturer_options_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkAudioCapturerOptionsAdapterCppToC::Get(self)->GetEncoding();
}

int32_t ARK_WEB_CALLBACK ark_audio_capturer_options_adapter_get_sample_format(
    struct _ark_audio_capturer_options_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkAudioCapturerOptionsAdapterCppToC::Get(self)->GetSampleFormat();
}

int32_t ARK_WEB_CALLBACK ark_audio_capturer_options_adapter_get_channels(
    struct _ark_audio_capturer_options_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkAudioCapturerOptionsAdapterCppToC::Get(self)->GetChannels();
}

int32_t ARK_WEB_CALLBACK ark_audio_capturer_options_adapter_get_source_type(
    struct _ark_audio_capturer_options_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkAudioCapturerOptionsAdapterCppToC::Get(self)->GetSourceType();
}

int32_t ARK_WEB_CALLBACK ark_audio_capturer_options_adapter_get_capturer_flags(
    struct _ark_audio_capturer_options_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkAudioCapturerOptionsAdapterCppToC::Get(self)->GetCapturerFlags();
}

} // namespace

ArkAudioCapturerOptionsAdapterCppToC::ArkAudioCapturerOptionsAdapterCppToC() {
  GetStruct()->get_sampling_rate =
      ark_audio_capturer_options_adapter_get_sampling_rate;
  GetStruct()->get_encoding = ark_audio_capturer_options_adapter_get_encoding;
  GetStruct()->get_sample_format =
      ark_audio_capturer_options_adapter_get_sample_format;
  GetStruct()->get_channels = ark_audio_capturer_options_adapter_get_channels;
  GetStruct()->get_source_type =
      ark_audio_capturer_options_adapter_get_source_type;
  GetStruct()->get_capturer_flags =
      ark_audio_capturer_options_adapter_get_capturer_flags;
}

ArkAudioCapturerOptionsAdapterCppToC::~ArkAudioCapturerOptionsAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkAudioCapturerOptionsAdapterCppToC, ArkAudioCapturerOptionsAdapter,
    ark_audio_capturer_options_adapter_t>::kBridgeType =
    ARK_AUDIO_CAPTURER_OPTIONS_ADAPTER;

} // namespace OHOS::ArkWeb
