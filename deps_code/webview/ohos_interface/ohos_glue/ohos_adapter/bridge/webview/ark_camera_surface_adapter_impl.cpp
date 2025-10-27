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

#include "ohos_adapter/bridge/ark_camera_surface_adapter_impl.h"

#include "ohos_adapter/bridge/ark_camera_surface_buffer_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkCameraSurfaceAdapterImpl::ArkCameraSurfaceAdapterImpl(std::shared_ptr<OHOS::NWeb::CameraSurfaceAdapter> ref)
    : real_(ref)
{}

int32_t ArkCameraSurfaceAdapterImpl::ReleaseBuffer(ArkWebRefPtr<ArkCameraSurfaceBufferAdapter> buffer, int32_t fence)
{
    ArkCameraSurfaceBufferAdapterImpl* imp = static_cast<ArkCameraSurfaceBufferAdapterImpl*>(buffer.get());
    return real_->ReleaseBuffer(std::move(imp->real_), fence);
}

} // namespace OHOS::ArkWeb
