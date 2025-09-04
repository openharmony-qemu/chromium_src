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

int32_t OHOSAudioOutputCallback::

}