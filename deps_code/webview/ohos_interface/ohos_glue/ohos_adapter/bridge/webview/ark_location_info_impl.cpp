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

#include "ohos_adapter/bridge/ark_location_info_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkLocationInfoImpl::ArkLocationInfoImpl(std::shared_ptr<OHOS::NWeb::LocationInfo> ref) : real_(ref) {}

double ArkLocationInfoImpl::GetLatitude()
{
    return real_->GetLatitude();
}

double ArkLocationInfoImpl::GetLongitude()
{
    return real_->GetLongitude();
}

double ArkLocationInfoImpl::GetAltitude()
{
    return real_->GetAltitude();
}

float ArkLocationInfoImpl::GetAccuracy()
{
    return real_->GetAccuracy();
}

float ArkLocationInfoImpl::GetSpeed()
{
    return real_->GetSpeed();
}

double ArkLocationInfoImpl::GetDirection()
{
    return real_->GetDirection();
}

int64_t ArkLocationInfoImpl::GetTimeStamp()
{
    return real_->GetTimeStamp();
}

int64_t ArkLocationInfoImpl::GetTimeSinceBoot()
{
    return real_->GetTimeSinceBoot();
}

ArkWebStringVector ArkLocationInfoImpl::GetAdditions()
{
    std::vector<std::string> vec = real_->GetAdditions();
    return ArkWebStringVectorClassToStruct(vec);
}

} // namespace OHOS::ArkWeb
