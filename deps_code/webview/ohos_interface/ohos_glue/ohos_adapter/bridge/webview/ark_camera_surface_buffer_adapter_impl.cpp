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

#include "ohos_adapter/bridge/ark_camera_surface_buffer_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkCameraSurfaceBufferAdapterImpl::ArkCameraSurfaceBufferAdapterImpl(
    std::shared_ptr<OHOS::NWeb::CameraSurfaceBufferAdapter> ref)
    : real_(ref)
{}

int32_t ArkCameraSurfaceBufferAdapterImpl::GetFileDescriptor()
{
    return real_->GetFileDescriptor();
}

int32_t ArkCameraSurfaceBufferAdapterImpl::GetWidth()
{
    return real_->GetWidth();
}

int32_t ArkCameraSurfaceBufferAdapterImpl::GetHeight()
{
    return real_->GetHeight();
}

int32_t ArkCameraSurfaceBufferAdapterImpl::GetStride()
{
    return real_->GetStride();
}

int32_t ArkCameraSurfaceBufferAdapterImpl::GetFormat()
{
    return real_->GetFormat();
}

uint32_t ArkCameraSurfaceBufferAdapterImpl::GetSize()
{
    return real_->GetSize();
}

uint8_t* ArkCameraSurfaceBufferAdapterImpl::GetBufferAddr()
{
    return real_->GetBufferAddr();
}

} // namespace OHOS::ArkWeb
