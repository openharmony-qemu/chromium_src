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

#include "ohos_adapter/bridge/ark_location_request_config_wrapper.h"

namespace OHOS::ArkWeb {

ArkLocationRequestConfigWrapper::ArkLocationRequestConfigWrapper(ArkWebRefPtr<ArkLocationRequestConfig> ref)
    : ctocpp_(ref)
{}

void ArkLocationRequestConfigWrapper::SetScenario(int32_t scenario)
{
    ctocpp_->SetScenario(scenario);
}
void ArkLocationRequestConfigWrapper::SetFixNumber(int32_t number)
{
    ctocpp_->SetFixNumber(number);
}
void ArkLocationRequestConfigWrapper::SetMaxAccuracy(int32_t maxAccuary)
{
    ctocpp_->SetMaxAccuracy(maxAccuary);
}
void ArkLocationRequestConfigWrapper::SetDistanceInterval(int32_t disInterval)
{
    ctocpp_->SetDistanceInterval(disInterval);
}
void ArkLocationRequestConfigWrapper::SetTimeInterval(int32_t timeInterval)
{
    ctocpp_->SetTimeInterval(timeInterval);
}
void ArkLocationRequestConfigWrapper::SetPriority(int32_t priority)
{
    ctocpp_->SetPriority(priority);
}

} // namespace OHOS::ArkWeb
