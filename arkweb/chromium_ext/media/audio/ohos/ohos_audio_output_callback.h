// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_CALLBACK_H_
#define MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_CALLBACK_H_

#include "media/audio/ohos/ohos_audio_output_stream.h"

namespace media {

class OHOSAudioOutputStream;

class OHOSAudioOutputCallback {
public:
    OHOSAudioOutputCallback(
        const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
        base::WeakPtr<OHOSAudioOutputStream> audio_output_stream);

    int32_t AudioRendererOnWriteData(void* buffer, int32_t length);

    int32_t AudioRendererOnError(OH_AudioStream_Result error);

    int32_t AudioRendererOnInterruptEvent(OH_AudioInterrupt_Hint hint);

    void AudioRendererOutputDeviceChangeCallback(OH_AudioStream_DeviceChangeReason reason);

private:
    scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
    base::WeakPtr<OHOSAudioOutputStream> audio_output_stream_;
}

}