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

#ifndef MEDIA_OHOS_BASE_SCREEN_CAPTURE_READ_CALLBACK_H_
#define MEDIA_OHOS_BASE_SCREEN_CAPTURE_READ_CALLBACK_H_

#include "base/files/file_util.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread_checker.h"
#include "content/common/content_export.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/screen_capture_adapter.h"

namespace webrtc {

using namespace OHOS::NWeb;
using namespace media;

class BaseScreenCaptureReadCallback {
public:

    BaseScreenCaptureReadCallback() = default;

    virtual ~BaseScreenCaptureReadCallback() = default;

    virtual void OnReadData() = 0;

    virtual void OnReadData(OHOS::NWeb::AudioCaptureSourceTypeAdapter type) = 0;

    virtual void OnDisplaySelected(uint64_t displayId) = 0;
};

}
#endif // MEDIA_OHOS_BASE_SCREEN_CAPTURE_READ_CALLBACK_H_