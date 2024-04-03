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

#ifndef AUDIO_CAPTURER_OPTIONS_ADAPTER_IMPL_H
#define AUDIO_CAPTURER_OPTIONS_ADAPTER_IMPL_H

#include "audio_capturer_adapter.h"

namespace OHOS::NWeb {

class AudioCapturerOptionsAdapterImpl : public AudioCapturerOptionsAdapter {
 public:
  AudioCapturerOptionsAdapterImpl() = default;

  AudioAdapterSamplingRate GetSamplingRate() override;

  AudioAdapterEncodingType GetEncoding() override;

  AudioAdapterSampleFormat GetSampleFormat() override;

  AudioAdapterChannel GetChannels() override;

  AudioAdapterSourceType GetSourceType() override;

  int32_t GetCapturerFlags() override;

  void SetSamplingRate(AudioAdapterSamplingRate rate);

  void SetEncoding(AudioAdapterEncodingType type);

  void SetSampleFormat(AudioAdapterSampleFormat format);

  void SetChannels(AudioAdapterChannel channel);

  void SetSourceType(AudioAdapterSourceType type);

  void SetCapturerFlags(int32_t flags);

 private:
  AudioAdapterSamplingRate sampling_rate_;
  AudioAdapterEncodingType encoding_;
  AudioAdapterSampleFormat format_;
  AudioAdapterChannel channels_;
  AudioAdapterSourceType source_type_;
  int32_t capturer_flags_;
};

}  // namespace OHOS::NWeb

#endif  // AUDIO_CAPTURER_OPTIONS_ADAPTER_IMPL_H
