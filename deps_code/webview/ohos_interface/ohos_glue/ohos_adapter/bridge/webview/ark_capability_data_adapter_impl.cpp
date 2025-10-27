/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_capability_data_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkCapabilityDataAdapterImpl::ArkCapabilityDataAdapterImpl(std::shared_ptr<OHOS::NWeb::CapabilityDataAdapter> ref)
    : real_(ref)
{}

int32_t ArkCapabilityDataAdapterImpl::GetMaxWidth()
{
    return real_->GetMaxWidth();
}

int32_t ArkCapabilityDataAdapterImpl::GetMaxHeight()
{
    return real_->GetMaxHeight();
}

int32_t ArkCapabilityDataAdapterImpl::GetMaxframeRate()
{
    return real_->GetMaxframeRate();
}

} // namespace OHOS::ArkWeb
