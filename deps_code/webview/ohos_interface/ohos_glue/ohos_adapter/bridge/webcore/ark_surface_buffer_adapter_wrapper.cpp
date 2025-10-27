/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_surface_buffer_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkSurfaceBufferAdapterWrapper::ArkSurfaceBufferAdapterWrapper(ArkWebRefPtr<ArkSurfaceBufferAdapter> ref) : ctocpp_(ref)
{}

int32_t ArkSurfaceBufferAdapterWrapper::GetFileDescriptor()
{
    return ctocpp_->GetFileDescriptor();
}

int32_t ArkSurfaceBufferAdapterWrapper::GetWidth()
{
    return ctocpp_->GetWidth();
}

int32_t ArkSurfaceBufferAdapterWrapper::GetHeight()
{
    return ctocpp_->GetHeight();
}

int32_t ArkSurfaceBufferAdapterWrapper::GetStride()
{
    return ctocpp_->GetStride();
}

int32_t ArkSurfaceBufferAdapterWrapper::GetFormat()
{
    return ctocpp_->GetFormat();
}

uint32_t ArkSurfaceBufferAdapterWrapper::GetSize()
{
    return ctocpp_->GetSize();
}

void* ArkSurfaceBufferAdapterWrapper::GetVirAddr()
{
    return ctocpp_->GetVirAddr();
}

} // namespace OHOS::ArkWeb
