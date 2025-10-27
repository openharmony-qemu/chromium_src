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

#include "ohos_adapter/bridge/ark_location_request_config_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkLocationRequestConfigImpl::ArkLocationRequestConfigImpl(std::shared_ptr<OHOS::NWeb::LocationRequestConfig> ref)
    : real_(ref)
{}

void ArkLocationRequestConfigImpl::SetScenario(int32_t scenario)
{
    real_->SetScenario(scenario);
}

void ArkLocationRequestConfigImpl::SetFixNumber(int32_t number)
{
    real_->SetFixNumber(number);
}

void ArkLocationRequestConfigImpl::SetMaxAccuracy(int32_t maxAccuary)
{
    real_->SetMaxAccuracy(maxAccuary);
}

void ArkLocationRequestConfigImpl::SetDistanceInterval(int32_t disInterval)
{
    real_->SetDistanceInterval(disInterval);
}

void ArkLocationRequestConfigImpl::SetTimeInterval(int32_t timeInterval)
{
    real_->SetTimeInterval(timeInterval);
}

void ArkLocationRequestConfigImpl::SetPriority(int32_t priority)
{
    real_->SetPriority(priority);
}

} // namespace OHOS::ArkWeb
