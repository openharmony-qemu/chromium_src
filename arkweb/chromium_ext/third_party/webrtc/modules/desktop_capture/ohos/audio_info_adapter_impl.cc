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

#include "audio_info_adapter_impl.h"

namespace OHOS::NWeb {

std::shared_ptr<AudioCaptureInfoAdapter> AudioInfoAdapterImpl::GetMicCapInfo() {
  return mic_cap_info_;
}

std::shared_ptr<AudioCaptureInfoAdapter>
AudioInfoAdapterImpl::GetInnerCapInfo() {
  return inner_cap_info_;
}

std::shared_ptr<AudioEncInfoAdapter> AudioInfoAdapterImpl::GetAudioEncInfo() {
  return enc_info_;
}

void AudioInfoAdapterImpl::SetMicCapInfo(
    std::shared_ptr<AudioCaptureInfoAdapter> info) {
  mic_cap_info_ = info;
}

void AudioInfoAdapterImpl::SetInnerCapInfo(
    std::shared_ptr<AudioCaptureInfoAdapter> info) {
  inner_cap_info_ = info;
}

void AudioInfoAdapterImpl::SetAudioEncInfo(
    std::shared_ptr<AudioEncInfoAdapter> info) {
  enc_info_ = info;
}

}  // namespace OHOS::NWeb
