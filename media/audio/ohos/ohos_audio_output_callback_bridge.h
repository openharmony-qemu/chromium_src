/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_CALLBACK_BRIDGE_H_
#define MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_CALLBACK_BRIDGE_H_

#include "media/audio/ohos/ohos_audio_output_stream.h"
#include "ohaudio/native_audiorenderer.h"
#include "ohaudio/native_audiostreambuilder.h"

namespace media {

class OhosAudioOutputStream;

class OhosAudioOutputCallbackBridge
    : public base::RefCountedThreadSafe<OhosAudioOutputCallbackBridge> {
 public:
  explicit OhosAudioOutputCallbackBridge(
      base::WeakPtr<OhosAudioOutputStream> audio_output_stream,
      scoped_refptr<base::SingleThreadTaskRunner> current_task_runner);

  void AudioRendererOnWriteData(void* buffer, int32_t length);

  void AudioRendererOnError(OH_AudioStream_Result error);

  void AudioRendererOnInterruptEvent(OH_AudioInterrupt_Hint hint);

  void AudioRendererOutputDeviceChangeCallback(
      OH_AudioStream_DeviceChangeReason reason);

  void Detach();

 private:
  base::WeakPtr<OhosAudioOutputStream> audio_output_stream_;

  scoped_refptr<base::SingleThreadTaskRunner> current_task_runner_;
};

}  // namespace media

#endif  // MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_CALLBACK_BRIDGE_H_
