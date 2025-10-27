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

#include "audio_interrupt_adapter_impl.h"

namespace OHOS::NWeb {

AudioAdapterStreamUsage AudioInterruptAdapterImpl::GetStreamUsage() {
  return stream_usage_;
}

AudioAdapterContentType AudioInterruptAdapterImpl::GetContentType() {
  return content_type_;
}

AudioAdapterStreamType AudioInterruptAdapterImpl::GetStreamType() {
  return stream_type_;
}

uint32_t AudioInterruptAdapterImpl::GetSessionID() {
  return session_id_;
}

bool AudioInterruptAdapterImpl::GetPauseWhenDucked() {
  return pause_when_ducked_;
}

void AudioInterruptAdapterImpl::SetStreamUsage(AudioAdapterStreamUsage usage) {
  stream_usage_ = usage;
}

void AudioInterruptAdapterImpl::SetContentType(AudioAdapterContentType type) {
  content_type_ = type;
}

void AudioInterruptAdapterImpl::SetStreamType(AudioAdapterStreamType type) {
  stream_type_ = type;
}

void AudioInterruptAdapterImpl::SetSessionID(uint32_t id) {
  session_id_ = id;
}

void AudioInterruptAdapterImpl::SetPauseWhenDucked(bool paused) {
  pause_when_ducked_ = paused;
}

}  // namespace OHOS::NWeb
