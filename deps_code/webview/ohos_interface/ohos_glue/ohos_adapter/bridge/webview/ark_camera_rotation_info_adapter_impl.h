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

#ifndef ARK_CAMERA_ROTATION_INFO_ADAPTER_IMPL_H
#define ARK_CAMERA_ROTATION_INFO_ADAPTER_IMPL_H
#pragma once

#include "camera_manager_adapter.h"
#include "ohos_adapter/include/ark_camera_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkCameraRotationInfoAdapterImpl : public ArkCameraRotationInfoAdapter {
public:
    explicit ArkCameraRotationInfoAdapterImpl(std::shared_ptr<OHOS::NWeb::CameraRotationInfoAdapter>);

    int32_t GetRotation() override;

    bool GetIsFlipX() override;

    bool GetIsFlipY() override;

private:
    std::shared_ptr<OHOS::NWeb::CameraRotationInfoAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkCameraRotationInfoAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_CAMERA_ROTATION_INFO_ADAPTER_IMPL_H
