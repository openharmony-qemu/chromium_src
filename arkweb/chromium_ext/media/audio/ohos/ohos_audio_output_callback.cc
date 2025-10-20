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

#include "base/logging.h"
#include "ohos_audio_output_callback.h"

namespace media {

OHOSAudioOutputCallback::OHOSAudioOutputCallback(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    base::WeakPtr<OHOSAudioOutputStream> audio_output_stream)
    : task_runner_(task_runner), audio_output_stream_(audio_output_stream) {
    DCHECK(audio_output_stream_);
}

void OHOSAudioOutputCallback::AudioRendererOnWriteData(void* buffer, int32_t length) {
    if (buffer && task_runner_) {
        task_runner_->PostTask(
            FROM_HERE, base::BindOnce(&OHOSAudioOutputStream::OnWriteData, audio_output_stream_, buffer, length));
        task_runner_->PostTask(
            FROM_HERE, base::BindOnce(&OHOSAudioOutputStream::SetUpAudioSilentState, audio_output_stream_));
    }
}

void OHOSAudioOutputCallback::AudioRendererOnError(OH_AudioStream_Result error) {
    if (task_runner_) {
        task_runner_->PostTask(
            FROM_HERE, base::BindOnce(&OHOSAudioOutputStream::ReportError, audio_output_stream_));
    }
}

void OHOSAudioOutputCallback::AudioRendererOnInterruptEvent(OH_AudioInterrupt_Hint hint) {
    if (task_runner_) {
        switch (hint) {
            case OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_PAUSE:
                task_runner_->PostTask(
                    FROM_HERE, base::BindOnce(&OHOSAudioOutputStream::OnSuspend, audio_output_stream_));
                break;
            case OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_STOP:
                task_runner_->PostTask(
                    FROM_HERE, base::BindOnce(&OHOSAudioOutputStream::OnSuspend, audio_output_stream_));
                break;
            case OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_RESUME:
                task_runner_->PostTask(
                    FROM_HERE, base::BindOnce(&OHOSAudioOutputStream::OnResume, audio_output_stream_));
                break;
            default:
                LOG(ERROR) << "audio renderer interrupt hint not foud, code:" << hint;
                break;
        }
    }
}

void OHOSAudioOutputCallback::AudioRendererOutputDeviceChangeCallback(OH_AudioStream_DeviceChangeReason reason) {
    if (task_runner_) {
        switch (reason) {
        case OH_AudioStream_DeviceChangeReason::REASON_OLD_DEVICE_UNAVAILABLE:
            task_runner_->PostTask(
                    FROM_HERE, base::BindOnce(&OHOSAudioOutputStream::OldDeviceUnavailable, audio_output_stream_));
            break;
        default:
            LOG(ERROR) << "AudioRendererOutputDeviceChangeCallback reason not foud, reason:" << reason;
            break;
        }
    }
}

} // namespace media