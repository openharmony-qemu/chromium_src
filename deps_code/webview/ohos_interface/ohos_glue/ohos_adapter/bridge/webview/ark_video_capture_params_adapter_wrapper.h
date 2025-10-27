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

#ifndef ARK_VIDEO_CAPTURE_PARAMS_ADAPTER_WRAPPER_H
#define ARK_VIDEO_CAPTURE_PARAMS_ADAPTER_WRAPPER_H
#pragma once

#include "camera_manager_adapter.h"
#include "ohos_adapter/include/ark_camera_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkVideoCaptureParamsAdapterWrapper : public NWeb::VideoCaptureParamsAdapter {
public:
    explicit ArkVideoCaptureParamsAdapterWrapper(ArkWebRefPtr<ArkVideoCaptureParamsAdapter>);

    uint32_t GetWidth() override;

    uint32_t GetHeight() override;

    float GetFrameRate() override;

    NWeb::VideoPixelFormatAdapter GetPixelFormat() override;

    bool GetEnableFaceDetection() override;

private:
    ArkWebRefPtr<ArkVideoCaptureParamsAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_VIDEO_CAPTURE_PARAMS_ADAPTER_WRAPPER_H
