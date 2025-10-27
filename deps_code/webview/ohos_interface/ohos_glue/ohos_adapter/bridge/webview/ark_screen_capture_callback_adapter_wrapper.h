/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ARK_SCREEN_CAPTURE_CALLBACK_ADAPTER_WRAPPER_H
#define ARK_SCREEN_CAPTURE_CALLBACK_ADAPTER_WRAPPER_H
#pragma once

#include "ohos_adapter/include/ark_screen_capture_adapter.h"
#include "screen_capture_adapter.h"

namespace OHOS::ArkWeb {

class ArkScreenCaptureCallbackAdapterWrapper : public OHOS::NWeb::ScreenCaptureCallbackAdapter {
public:
    explicit ArkScreenCaptureCallbackAdapterWrapper(ArkWebRefPtr<ArkScreenCaptureCallbackAdapter>);

    void OnError(int32_t errorCode) override;

    void OnAudioBufferAvailable(bool isReady, OHOS::NWeb::AudioCaptureSourceTypeAdapter type) override;

    void OnVideoBufferAvailable(bool isReady) override;

    void OnStateChange(OHOS::NWeb::ScreenCaptureStateCodeAdapter stateCode) override;

private:
    ArkWebRefPtr<ArkScreenCaptureCallbackAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_SCREEN_CAPTURE_CALLBACK_ADAPTER_WRAPPER_H
