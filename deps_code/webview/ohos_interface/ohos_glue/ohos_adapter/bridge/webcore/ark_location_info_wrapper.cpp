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

#include "ohos_adapter/bridge/ark_location_info_wrapper.h"

namespace OHOS::ArkWeb {

ArkLocationInfoWrapper::ArkLocationInfoWrapper(ArkWebRefPtr<ArkLocationInfo> ref) : ctocpp_(ref) {}

double ArkLocationInfoWrapper::GetLatitude()
{
    return ctocpp_->GetLatitude();
}

double ArkLocationInfoWrapper::GetLongitude()
{
    return ctocpp_->GetLongitude();
}

double ArkLocationInfoWrapper::GetAltitude()
{
    return ctocpp_->GetAltitude();
}

float ArkLocationInfoWrapper::GetAccuracy()
{
    return ctocpp_->GetAccuracy();
}

float ArkLocationInfoWrapper::GetSpeed()
{
    return ctocpp_->GetSpeed();
}

double ArkLocationInfoWrapper::GetDirection()
{
    return ctocpp_->GetDirection();
}

int64_t ArkLocationInfoWrapper::GetTimeStamp()
{
    return ctocpp_->GetTimeStamp();
}

int64_t ArkLocationInfoWrapper::GetTimeSinceBoot()
{
    return ctocpp_->GetTimeSinceBoot();
}

std::vector<std::string> ArkLocationInfoWrapper::GetAdditions()
{
    ArkWebStringVector vec = ctocpp_->GetAdditions();
    std::vector<std::string> result = ArkWebStringVectorStructToClass(vec);
    ArkWebStringVectorStructRelease(vec);
    return result;
}

} // namespace OHOS::ArkWeb
