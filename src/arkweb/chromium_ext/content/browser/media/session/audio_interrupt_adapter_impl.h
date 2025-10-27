/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef AUDIO_INTERRUPT_ADAPTER_IMPL_H
#define AUDIO_INTERRUPT_ADAPTER_IMPL_H

#include "audio_system_manager_adapter.h"

namespace OHOS::NWeb {

class AudioInterruptAdapterImpl : public AudioInterruptAdapter {
 public:
  AudioInterruptAdapterImpl() = default;

  AudioAdapterStreamUsage GetStreamUsage() override;

  AudioAdapterContentType GetContentType() override;

  AudioAdapterStreamType GetStreamType() override;

  uint32_t GetSessionID() override;

  bool GetPauseWhenDucked() override;

  void SetStreamUsage(AudioAdapterStreamUsage usage);

  void SetContentType(AudioAdapterContentType type);

  void SetStreamType(AudioAdapterStreamType type);

  void SetSessionID(uint32_t id);

  void SetPauseWhenDucked(bool paused);

 private:
  AudioAdapterStreamUsage stream_usage_ =
      AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
  AudioAdapterContentType content_type_ =
      AudioAdapterContentType::CONTENT_TYPE_MUSIC;
  AudioAdapterStreamType stream_type_ = AudioAdapterStreamType::STREAM_MUSIC;
  uint32_t session_id_ = 0;
  bool pause_when_ducked_ = false;
};

}  // namespace OHOS::NWeb

#endif  // AUDIO_INTERRUPT_ADAPTER_IMPL_H
