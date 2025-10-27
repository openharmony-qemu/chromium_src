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

#include "ohos_adapter/bridge/ark_format_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkFormatAdapterImpl::ArkFormatAdapterImpl(std::shared_ptr<OHOS::NWeb::FormatAdapter> ref) : real_(ref) {}

uint32_t ArkFormatAdapterImpl::GetWidth()
{
    return real_->GetWidth();
}

uint32_t ArkFormatAdapterImpl::GetHeight()
{
    return real_->GetHeight();
}

float ArkFormatAdapterImpl::GetFrameRate()
{
    return real_->GetFrameRate();
}

int32_t ArkFormatAdapterImpl::GetPixelFormat()
{
    return (int32_t)real_->GetPixelFormat();
}

} // namespace OHOS::ArkWeb
