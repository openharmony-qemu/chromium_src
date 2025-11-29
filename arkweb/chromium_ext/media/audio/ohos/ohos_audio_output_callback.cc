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

std::shared_mutex OHOSAudioOutputCallback::audio_callback_mutex_;

OHOSAudioOutputCallback::OHOSAudioOutputCallback(
    base::WeakPtr<OHOSAudioOutputStream> audio_output_stream)
    : audio_output_stream_(audio_output_stream) {
    DCHECK(audio_output_stream_);
}

void OHOSAudioOutputCallback::AudioRendererOnWriteData(void* buffer, int32_t length) {
    std::shared_lock<std::shared_mutex> lock(audio_callback_mutex_);
    if (buffer && audio_output_stream_) {
        audio_output_stream_->OnWriteData(buffer, length);
        audio_output_stream_->SetUpAudioSilentState();
    }
}

void OHOSAudioOutputCallback::AudioRendererOnError(OH_AudioStream_Result error) {
    std::shared_lock<std::shared_mutex> lock(audio_callback_mutex_);
    if (audio_output_stream_) {
        audio_output_stream_->ReportError();
    }
}

void OHOSAudioOutputCallback::AudioRendererOnInterruptEvent(OH_AudioInterrupt_Hint hint) {
    std::shared_lock<std::shared_mutex> lock(audio_callback_mutex_);
    if (audio_output_stream_) {
        switch (hint) {
            case OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_PAUSE:
                audio_output_stream_->OnSuspend();
                break;
            case OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_STOP:
                audio_output_stream_->OnSuspend();
                break;
            case OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_RESUME:
                audio_output_stream_->OnResume();
                break;
            default:
                LOG(ERROR) << "audio renderer interrupt hint not foud, code:" << hint;
                break;
        }        
    }
}

void OHOSAudioOutputCallback::AudioRendererOutputDeviceChangeCallback(OH_AudioStream_DeviceChangeReason reason) {
    std::shared_lock<std::shared_mutex> lock(audio_callback_mutex_);
    if (audio_output_stream_) {
        switch (reason) {
        case OH_AudioStream_DeviceChangeReason::REASON_OLD_DEVICE_UNAVAILABLE:
            audio_output_stream_->OldDeviceUnavailable();
            break;
        default:
            LOG(ERROR) << "AudioRendererOutputDeviceChangeCallback reason not foud, reason:" << reason;
            break;
        }
    }
}

std::shared_mutex& OHOSAudioOutputCallback::GetAudioCallbackMutex() {
    return audio_callback_mutex_;
}

} // namespace media