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

#include "ohos_adapter/bridge/ark_buffer_request_config_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkBufferRequestConfigAdapterWrapper::ArkBufferRequestConfigAdapterWrapper(
    ArkWebRefPtr<ArkBufferRequestConfigAdapter> ref)
    : ctocpp_(ref)
{}

int32_t ArkBufferRequestConfigAdapterWrapper::GetWidth()
{
    return ctocpp_->GetWidth();
}

int32_t ArkBufferRequestConfigAdapterWrapper::GetHeight()
{
    return ctocpp_->GetHeight();
}

int32_t ArkBufferRequestConfigAdapterWrapper::GetStrideAlignment()
{
    return ctocpp_->GetStrideAlignment();
}

int32_t ArkBufferRequestConfigAdapterWrapper::GetFormat()
{
    return ctocpp_->GetFormat();
}

uint64_t ArkBufferRequestConfigAdapterWrapper::GetUsage()
{
    return ctocpp_->GetUsage();
}

int32_t ArkBufferRequestConfigAdapterWrapper::GetTimeout()
{
    return ctocpp_->GetTimeout();
}

NWeb::ColorGamutAdapter ArkBufferRequestConfigAdapterWrapper::GetColorGamut()
{
    return (NWeb::ColorGamutAdapter)ctocpp_->GetColorGamut();
}

NWeb::TransformTypeAdapter ArkBufferRequestConfigAdapterWrapper::GetTransformType()
{
    return (NWeb::TransformTypeAdapter)ctocpp_->GetTransformType();
}

} // namespace OHOS::ArkWeb
