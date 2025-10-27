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

#include "ohos_adapter/bridge/ark_buffer_request_config_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkBufferRequestConfigAdapterImpl::ArkBufferRequestConfigAdapterImpl(
    std::shared_ptr<OHOS::NWeb::BufferRequestConfigAdapter> ref)
    : real_(ref)
{}

int32_t ArkBufferRequestConfigAdapterImpl::GetWidth()
{
    return real_->GetWidth();
}

int32_t ArkBufferRequestConfigAdapterImpl::GetHeight()
{
    return real_->GetHeight();
}

int32_t ArkBufferRequestConfigAdapterImpl::GetStrideAlignment()
{
    return real_->GetStrideAlignment();
}

int32_t ArkBufferRequestConfigAdapterImpl::GetFormat()
{
    return real_->GetFormat();
}

uint64_t ArkBufferRequestConfigAdapterImpl::GetUsage()
{
    return real_->GetUsage();
}

int32_t ArkBufferRequestConfigAdapterImpl::GetTimeout()
{
    return real_->GetTimeout();
}

int32_t ArkBufferRequestConfigAdapterImpl::GetColorGamut()
{
    return (int32_t)real_->GetColorGamut();
}

int32_t ArkBufferRequestConfigAdapterImpl::GetTransformType()
{
    return (int32_t)real_->GetTransformType();
}

} // namespace OHOS::ArkWeb
