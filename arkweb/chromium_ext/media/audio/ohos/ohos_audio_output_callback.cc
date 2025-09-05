// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/logging.h"
#include "ohos_audio_output_callback.h"

namespace media {

OHOSAudioOutputCallback::OHOSAudioOutputCallback(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    base::WeakPtr<OHOSAudioOutputStream> audio_output_stream)
    : task_runner_(task_runner), audio_output_stream_(audio_output_stream) {
    DCHECK(task_runner.get());
    DCHECK(audio_output_stream_);
}

int32_t OHOSAudioOutputCallback::AudioRendererOnWriteData(void* buffer, int32_t length) {
    if (buffer && task_runner_ && audio_output_stream_) {
        task_runner_->PostTask(
            FROM_HERE,
            base::BindOnce(&OHOSAudioOutputStream::OnWriteData, audio_output_stream_, buffer, length));
        task_runner_->PostTask(
            FROM_HERE,
            base::BindOnce(&OHOSAudioOutputStream::SetUpAudioSilentState, audio_output_stream_));
    }
    return 0;
}

int32_t OHOSAudioOutputCallback::AudioRendererOnError(OH_AudioStream_Result error) {
    if (task_runner_ && audio_output_stream_) {
        task_runner_->PostTask(
            FROM_HERE,
            base::BindOnce(&OHOSAudioOutputStream::ReportError, audio_output_stream_));
    }
    return 0;
}

int32_t OHOSAudioOutputCallback::AudioRendererOnInterruptEvent(OH_AudioInterrupt_Hint hint) {
    if (task_runner_ && audio_output_stream_) {
        switch (hint) {
            case OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_PAUSE:
                task_runner_->PostTask(
                    FROM_HERE,
                    base::BindOnce(&OHOSAudioOutputStream::OnSuspend, audio_output_stream_));
                break;
            case OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_STOP:
                task_runner_->PostTask(
                    FROM_HERE,
                    base::BindOnce(&OHOSAudioOutputStream::OnSuspend, audio_output_stream_));
                break;
            case OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_RESUME:
                task_runner_->PostTask(
                    FROM_HERE,
                    base::BindOnce(&OHOSAudioOutputStream::OnResume, audio_output_stream_));
                break;
            default:
                LOG(ERROR) << "audio renderer interrupt hint not foud, code:" << hint;
                break;
        }        
    }
    return 0;
}

void OHOSAudioOutputCallback::AudioRendererOutputDeviceChangeCallback(OH_AudioStream_DeviceChangeReason reason) {
    if (task_runner_ && audio_output_stream_) {
        switch (reason) {
        case OH_AudioStream_DeviceChangeReason::REASON_OLD_DEVICE_UNAVAILABLE:
            task_runner_->PostTask(
                FROM_HERE,
                base::BindOnce(&OHOSAudioOutputStream::OldDeviceUnavailable, audio_output_stream_));
            break;
        default:
            LOG(ERROR) << "AudioRendererOutputDeviceChangeCallback reason not foud, reason:" << reason;
            break;
        }
    }
}

} // namespace media