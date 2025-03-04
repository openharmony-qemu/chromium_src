/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef AUDIO_CAPTURE_INFO_ADAPTER_IMPL_H
#define AUDIO_CAPTURE_INFO_ADAPTER_IMPL_H

#include "screen_capture_adapter.h"

namespace OHOS::NWeb {

class AudioCaptureInfoAdapterImpl : public AudioCaptureInfoAdapter {
 public:
  AudioCaptureInfoAdapterImpl() = default;

  int32_t GetAudioSampleRate() override;

  int32_t GetAudioChannels() override;

  AudioCaptureSourceTypeAdapter GetAudioSource() override;

  void SetAudioSampleRate(int32_t rate);

  void SetAudioChannels(int32_t channel);

  void SetAudioSource(AudioCaptureSourceTypeAdapter type);

 private:
  /* Audio capture sample rate info */
  int32_t sample_rate_ = 0;
  /* Audio capture channel info */
  int32_t channels_ = 0;
  /* Audio capture source type */
  AudioCaptureSourceTypeAdapter source_ =
      AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT;
};

}  // namespace OHOS::NWeb

#endif  // AUDIO_CAPTURE_INFO_ADAPTER_IMPL_H
