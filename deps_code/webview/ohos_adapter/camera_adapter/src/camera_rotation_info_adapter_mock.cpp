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

#include "camera_rotation_info_adapter_impl.h"

namespace OHOS::NWeb {

int32_t CameraRotationInfoAdapterImpl::GetRotation()
{
    return 0;
}

bool CameraRotationInfoAdapterImpl::GetIsFlipX()
{
    return false;
}

bool CameraRotationInfoAdapterImpl::GetIsFlipY()
{
    return false;
}

void CameraRotationInfoAdapterImpl::SetRotation(int32_t rotation)
{
}

void CameraRotationInfoAdapterImpl::SetIsFlipX(bool value)
{
}

void CameraRotationInfoAdapterImpl::SetIsFlipY(bool value)
{
}

} // namespace OHOS::NWeb
