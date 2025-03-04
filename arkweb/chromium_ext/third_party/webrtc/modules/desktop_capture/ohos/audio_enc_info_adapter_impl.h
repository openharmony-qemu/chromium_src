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

#ifndef AUDIO_ENC_INFO_ADAPTER_IMPL_H
#define AUDIO_ENC_INFO_ADAPTER_IMPL_H

#include "screen_capture_adapter.h"

namespace OHOS::NWeb {

class AudioEncInfoAdapterImpl : public AudioEncInfoAdapter {
 public:
  AudioEncInfoAdapterImpl() = default;

  int32_t GetAudioBitrate() override;

  AudioCodecFormatAdapter GetAudioCodecformat() override;

  void SetAudioBitrate(int32_t bitrate);

  void SetAudioCodecformat(AudioCodecFormatAdapter format);

 private:
  /* Audio encoder bitrate */
  int32_t bitrate_ = 0;
  /* Audio codec format */
  AudioCodecFormatAdapter codec_format_ =
      AudioCodecFormatAdapter::AUDIO_DEFAULT;
};

}  // namespace OHOS::NWeb

#endif  // AUDIO_ENC_INFO_ADAPTER_IMPL_H
