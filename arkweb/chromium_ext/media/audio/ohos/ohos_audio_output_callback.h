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

#ifndef MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_CALLBACK_H_
#define MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_CALLBACK_H_

#include "media/audio/ohos/ohos_audio_output_stream.h"
#include "ohaudio/native_audiorenderer.h"
#include "ohaudio/native_audiostreambuilder.h"

namespace media {

class OHOSAudioOutputStream;

class OHOSAudioOutputCallback {
public:
    OHOSAudioOutputCallback(
        base::WeakPtr<OHOSAudioOutputStream> audio_output_stream);

    void AudioRendererOnWriteData(void* buffer, int32_t length);

    void AudioRendererOnError(OH_AudioStream_Result error);

    void AudioRendererOnInterruptEvent(OH_AudioInterrupt_Hint hint);

    void AudioRendererOutputDeviceChangeCallback(OH_AudioStream_DeviceChangeReason reason);

    static std::shared_mutex& GetAudioCallbackMutex();

private:
    static std::shared_mutex audio_callback_mutex_;
    base::WeakPtr<OHOSAudioOutputStream> audio_output_stream_;
};

} // namespace media

#endif // MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_CALLBACK_H_