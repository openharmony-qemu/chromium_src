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

#ifndef AUDIO_INFO_ADAPTER_IMPL_H
#define AUDIO_INFO_ADAPTER_IMPL_H

#include "screen_capture_adapter.h"

namespace OHOS::NWeb {

class AudioInfoAdapterImpl : public AudioInfoAdapter {
 public:
  AudioInfoAdapterImpl() = default;

  std::shared_ptr<AudioCaptureInfoAdapter> GetMicCapInfo() override;

  std::shared_ptr<AudioCaptureInfoAdapter> GetInnerCapInfo() override;

  std::shared_ptr<AudioEncInfoAdapter> GetAudioEncInfo() override;

  void SetMicCapInfo(std::shared_ptr<AudioCaptureInfoAdapter> info);

  void SetInnerCapInfo(std::shared_ptr<AudioCaptureInfoAdapter> info);

  void SetAudioEncInfo(std::shared_ptr<AudioEncInfoAdapter> info);

 private:
  /* Audio capture info of microphone */
  std::shared_ptr<AudioCaptureInfoAdapter> mic_cap_info_ = nullptr;
  /* Audio capture info of inner */
  std::shared_ptr<AudioCaptureInfoAdapter> inner_cap_info_ = nullptr;
  /* Audio encoder info, no need to set, while dataType =
   * ORIGINAL_STREAM_DATA_TYPE */
  std::shared_ptr<AudioEncInfoAdapter> enc_info_ = nullptr;
};

}  // namespace OHOS::NWeb

#endif  // AUDIO_INFO_ADAPTER_IMPL_H
