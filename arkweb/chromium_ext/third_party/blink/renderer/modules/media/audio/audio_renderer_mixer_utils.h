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

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_AUDIO_AUDIO_RENDERER_MIXER_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_AUDIO_AUDIO_RENDERER_MIXER_UTILS_H_

#include "arkweb/build/features/features.h"
#include "base/synchronization/lock.h"
#include "third_party/blink/renderer/modules/media/audio/audio_renderer_mixer.h"

namespace blink {

class AudioRendererMixerUtils {
public:
  AudioRendererMixer* audioRendererMixer;
  AudioRendererMixerUtils(AudioRendererMixer* impl);

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  void AudioRendererMixerShareInit(int media_tid);
  void AudioRendererMixerShareDestroy(int audio_output_tid, int media_tid);
  void AddMixerInputShareInit(int audio_output_tid, int media_tid);
  void AddMixerInputShareRender(int audio_output_tid, int media_tid);
#endif
};
    
}

#endif
