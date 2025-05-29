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

#ifndef ARKWEB_EXT_THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_DELAY_H_
#define ARKWEB_EXT_THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_DELAY_H_

#include <memory>
#include <string>

#include "arkweb/build/features/features.h"
#include "base/logging.h"

namespace blink {
class Delay;

bool MemcpyMediaAudioUtils(float* write_pointer, int remainder, const float* source,
                           int write_index, uint32_t frames_to_process, float* buffer);

class DelayUtils {
public:
    Delay* delay;
    DelayUtils(Delay* impl);

    bool MemcpyMediaAudio(float* sample, uint32_t remainder, float* read_pointer,
                          uint32_t frames_to_process, int read_index, float* buffer);

    bool MemcpyMediaAudioTwo(float* sample_one, float* sample_two, uint32_t remainder,
                             float* read_pointer, uint32_t frames_to_process, int read_index, float* buffer);
};

}  // namespace blink

#endif  // ARKWEB_EXT_THIRD_PARTY_BLINK_RENDERER_PLATFORM_AUDIO_DELAY_H_