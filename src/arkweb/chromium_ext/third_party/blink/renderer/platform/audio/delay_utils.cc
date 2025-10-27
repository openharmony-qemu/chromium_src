/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/audio/delay_utils.h"

#include "third_party/bounds_checking_function/include/securec.h"

namespace blink {

DelayUtils::DelayUtils(Delay* delay) {
  this->delay = delay;
}

bool DelayUtils::MemcpyMediaAudio(float* sample, uint32_t remainder, float* read_pointer,
                                  uint32_t frames_to_process, int read_index, float* buffer) {
  if (memcpy_s(sample, sizeof(*sample) * remainder, read_pointer,
               sizeof(*sample) * std::min(frames_to_process, remainder))) {
    LOG(ERROR) << "sample1 copy failed";
    return true;
  }
  if (frames_to_process > remainder) {
    if (memcpy_s(sample + remainder, sizeof(*sample) * read_index, buffer,
                 sizeof(*sample) * (frames_to_process - remainder))) {
      LOG(ERROR) << "sample1_remainder copy failed";
      return true;
    }
  }
  return false;
}

bool DelayUtils::MemcpyMediaAudioTwo(float* sample_one, float* sample_two, uint32_t remainder,
                                     float* read_pointer, uint32_t frames_to_process, int read_index, float* buffer) {
  if (memcpy_s(sample_two, sizeof(*sample_one) * remainder, read_pointer,
               sizeof(*sample_one) * std::min(frames_to_process, remainder))) {
    LOG(ERROR) << "sample2 copy failed";
    return true;
  }
  if (frames_to_process > remainder) {
    if (memcpy_s(sample_two + remainder, sizeof(*sample_one) * read_index, buffer,
                 sizeof(*sample_one) * (frames_to_process - remainder))) {
      LOG(ERROR) << "sample2_remainder copy failed";
      return true;
    }
  }
  return false;
}

bool MemcpyMediaAudioUtils(float* write_pointer, int remainder, const float* source,
                           int write_index, uint32_t frames_to_process, float* buffer) {
  if (memcpy_s(write_pointer, sizeof(*write_pointer) * remainder, source,
               sizeof(*write_pointer) *
                   std::min(static_cast<int>(frames_to_process), remainder)) !=
      EOK) {
    LOG(ERROR) << "write_pointer copy failed";
    return true;
  }
  if (memcpy_s(buffer, sizeof(*write_pointer) * write_index, source + remainder,
               sizeof(*write_pointer) *
                   std::max(0, static_cast<int>(frames_to_process) - remainder))) {
    LOG(ERROR) << "buffer copy failed";
    return true;
  }
  return false;
}

}  // namespace blink