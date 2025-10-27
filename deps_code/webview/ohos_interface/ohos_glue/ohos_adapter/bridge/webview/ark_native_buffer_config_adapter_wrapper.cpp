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

#include "ohos_adapter/bridge/ark_native_buffer_config_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkNativeBufferConfigAdapterWrapper::ArkNativeBufferConfigAdapterWrapper(
    ArkWebRefPtr<ArkNativeBufferConfigAdapter> ref) : ctocpp_(ref)
{}

int ArkNativeBufferConfigAdapterWrapper::GetBufferWidth()
{
    return ctocpp_->GetBufferWidth();
}

int ArkNativeBufferConfigAdapterWrapper::GetBufferHeight()
{
    return ctocpp_->GetBufferHeight();
}

int ArkNativeBufferConfigAdapterWrapper::GetBufferFormat()
{
    return ctocpp_->GetBufferFormat();
}

int ArkNativeBufferConfigAdapterWrapper::GetBufferUsage()
{
    return ctocpp_->GetBufferUsage();
}

int ArkNativeBufferConfigAdapterWrapper::GetBufferStride()
{
    return ctocpp_->GetBufferStride();
}

void ArkNativeBufferConfigAdapterWrapper::SetBufferWidth(int width)
{
    ctocpp_->SetBufferWidth(width);
}

void ArkNativeBufferConfigAdapterWrapper::SetBufferHeight(int height)
{
    ctocpp_->SetBufferHeight(height);
}

void ArkNativeBufferConfigAdapterWrapper::SetBufferFormat(int format)
{
    ctocpp_->SetBufferFormat(format);
}

void ArkNativeBufferConfigAdapterWrapper::SetBufferUsage(int usage)
{
    ctocpp_->SetBufferUsage(usage);
}

void ArkNativeBufferConfigAdapterWrapper::SetBufferStride(int stride)
{
    ctocpp_->SetBufferStride(stride);
}
} // namespace OHOS::ArkWeb
