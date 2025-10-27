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

#include "ohos_adapter/bridge/ark_camera_surface_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_camera_surface_buffer_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkCameraSurfaceAdapterWrapper::ArkCameraSurfaceAdapterWrapper(ArkWebRefPtr<ArkCameraSurfaceAdapter> ref) : ctocpp_(ref)
{}

int32_t ArkCameraSurfaceAdapterWrapper::ReleaseBuffer(
    std::shared_ptr<OHOS::NWeb::CameraSurfaceBufferAdapter> buffer, int32_t fence)
{
    std::shared_ptr<ArkCameraSurfaceBufferAdapterWrapper> bufferWrapper =
        std::static_pointer_cast<ArkCameraSurfaceBufferAdapterWrapper>(buffer);
    return ctocpp_->ReleaseBuffer(bufferWrapper->ctocpp_, fence);
}

} // namespace OHOS::ArkWeb
