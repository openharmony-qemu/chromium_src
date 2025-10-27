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

#include "ohos_adapter/bridge/ark_native_buffer_config_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkNativeBufferConfigAdapterImpl::ArkNativeBufferConfigAdapterImpl(
    std::shared_ptr<OHOS::NWeb::NativeBufferConfigAdapter> ref)
    : real_(ref)
{}

int ArkNativeBufferConfigAdapterImpl::GetBufferWidth()
{
    return real_->GetBufferWidth();
}

int ArkNativeBufferConfigAdapterImpl::GetBufferHeight()
{
    return real_->GetBufferHeight();
}

int ArkNativeBufferConfigAdapterImpl::GetBufferFormat()
{
    return real_->GetBufferFormat();
}

int ArkNativeBufferConfigAdapterImpl::GetBufferUsage()
{
    return real_->GetBufferUsage();
}

int ArkNativeBufferConfigAdapterImpl::GetBufferStride()
{
    return real_->GetBufferStride();
}

void ArkNativeBufferConfigAdapterImpl::SetBufferWidth(int width)
{
    return real_->SetBufferWidth(width);
}

void ArkNativeBufferConfigAdapterImpl::SetBufferHeight(int height)
{
    return real_->SetBufferHeight(height);
}

void ArkNativeBufferConfigAdapterImpl::SetBufferFormat(int format)
{
    return real_->SetBufferFormat(format);
}

void ArkNativeBufferConfigAdapterImpl::SetBufferUsage(int usage)
{
    return real_->SetBufferUsage(usage);
}

void ArkNativeBufferConfigAdapterImpl::SetBufferStride(int stride)
{
    return real_->SetBufferStride(stride);
}
} // namespace OHOS::ArkWeb
