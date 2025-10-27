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

#include "ohos_adapter/bridge/ark_camera_buffer_listener_adapter_impl.h"

#include "ohos_adapter/bridge/ark_camera_rotation_info_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_camera_surface_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_camera_surface_buffer_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkCameraBufferListenerAdapterImpl::ArkCameraBufferListenerAdapterImpl(
    std::shared_ptr<OHOS::NWeb::CameraBufferListenerAdapter> ref)
    : real_(ref)
{}

void ArkCameraBufferListenerAdapterImpl::OnBufferAvailable(ArkWebRefPtr<ArkCameraSurfaceAdapter> surface,
    ArkWebRefPtr<ArkCameraSurfaceBufferAdapter> buffer, ArkWebRefPtr<ArkCameraRotationInfoAdapter> rotationInfo)
{
    if (!surface || !buffer || !rotationInfo) {
        return real_->OnBufferAvailable(nullptr, nullptr, nullptr);
    }

    return real_->OnBufferAvailable(std::make_shared<ArkCameraSurfaceAdapterWrapper>(surface),
        std::make_shared<ArkCameraSurfaceBufferAdapterWrapper>(buffer),
        std::make_shared<ArkCameraRotationInfoAdapterWrapper>(rotationInfo));
}

} // namespace OHOS::ArkWeb
