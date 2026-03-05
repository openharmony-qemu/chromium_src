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

#include "ohos_audio_output_callback_bridge.h"

#include "base/logging.h"

namespace media {

OhosAudioOutputCallbackBridge::OhosAudioOutputCallbackBridge(
    base::WeakPtr<OhosAudioOutputStream> audio_output_stream,
    scoped_refptr<base::SingleThreadTaskRunner> current_task_runner)
    : audio_output_stream_(audio_output_stream),
      current_task_runner_(current_task_runner) {
  DCHECK(audio_output_stream_);
}

void OhosAudioOutputCallbackBridge::AudioRendererOnWriteData(void* buffer,
                                                             int32_t length) {
  if (buffer) {
    current_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&OhosAudioOutputStream::PumpSamples,
                                  audio_output_stream_, buffer, length));
  }
}

void OhosAudioOutputCallbackBridge::AudioRendererOnError(
    OH_AudioStream_Result error) {
  current_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OhosAudioOutputStream::ReportError,
                                audio_output_stream_));
}

void OhosAudioOutputCallbackBridge::AudioRendererOnInterruptEvent(
    OH_AudioInterrupt_Hint hint) {
  switch (hint) {
    case OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_PAUSE:
      current_task_runner_->PostTask(
          FROM_HERE, base::BindOnce(&OhosAudioOutputStream::OnSuspend,
                                    audio_output_stream_));
      break;
    case OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_STOP:
      current_task_runner_->PostTask(
          FROM_HERE, base::BindOnce(&OhosAudioOutputStream::OnSuspend,
                                    audio_output_stream_));
      break;
    case OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_RESUME:
      current_task_runner_->PostTask(
          FROM_HERE, base::BindOnce(&OhosAudioOutputStream::OnResume,
                                    audio_output_stream_));
      break;
    default:
      LOG(ERROR) << "audio renderer interrupt hint not found, code:" << hint;
      break;
  }
}

void OhosAudioOutputCallbackBridge::AudioRendererOutputDeviceChangeCallback(
    OH_AudioStream_DeviceChangeReason reason) {
  switch (reason) {
    case OH_AudioStream_DeviceChangeReason::REASON_OLD_DEVICE_UNAVAILABLE:
      current_task_runner_->PostTask(
          FROM_HERE, base::BindOnce(&OhosAudioOutputStream::SuspendPlayer,
                                    audio_output_stream_));
      break;
    default:
      LOG(ERROR) << "AudioRendererOutputDeviceChangeCallback reason not "
                    "found, reason:"
                 << reason;
      break;
  }
}
}  // namespace media
