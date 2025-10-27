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

#ifndef ARK_CAMERA_BUFFER_LISTENER_ADAPTER_IMPL_H
#define ARK_CAMERA_BUFFER_LISTENER_ADAPTER_IMPL_H
#pragma once

#include "camera_manager_adapter.h"
#include "ohos_adapter/include/ark_camera_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkCameraBufferListenerAdapterImpl : public ArkCameraBufferListenerAdapter {
public:
    explicit ArkCameraBufferListenerAdapterImpl(std::shared_ptr<OHOS::NWeb::CameraBufferListenerAdapter>);

    void OnBufferAvailable(ArkWebRefPtr<ArkCameraSurfaceAdapter> surface,
        ArkWebRefPtr<ArkCameraSurfaceBufferAdapter> buffer,
        ArkWebRefPtr<ArkCameraRotationInfoAdapter> rotationInfo) override;

private:
    std::shared_ptr<OHOS::NWeb::CameraBufferListenerAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkCameraBufferListenerAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_CAMERA_BUFFER_LISTENER_ADAPTER_IMPL_H
