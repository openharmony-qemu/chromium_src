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

#include "location_proxy_adapter_impl.h"

#include <string>

namespace OHOS::NWeb {
class LocationInstanceImpl : public LocationInstance {
public:
    LocationInstanceImpl() = default;
   
    LocationInstanceImpl(const LocationInstanceImpl&) = delete;

    LocationInstanceImpl& operator=(const LocationInstanceImpl&) = delete;
 
    ~LocationInstanceImpl() = default;

    std::shared_ptr<LocationProxyAdapter> CreateLocationProxyAdapter() override;

    std::shared_ptr<LocationRequestConfig> CreateLocationRequestConfig() override;
};

LocationInstance& LocationInstance::GetInstance()
{
    static LocationInstanceImpl instance;
    return instance;
}

std::shared_ptr<LocationProxyAdapter> LocationInstanceImpl::CreateLocationProxyAdapter()
{
    return nullptr;
}

std::shared_ptr<LocationRequestConfig> LocationInstanceImpl::CreateLocationRequestConfig()
{
    return nullptr;
}

LocationRequestConfigImpl::LocationRequestConfigImpl() {}

void LocationRequestConfigImpl::SetScenario(int32_t scenario) {}

void LocationRequestConfigImpl::SetFixNumber(int32_t number) {}

void LocationRequestConfigImpl::SetMaxAccuracy(int32_t maxAccuary) {}

void LocationRequestConfigImpl::SetDistanceInterval(int32_t disInterval) {}

void LocationRequestConfigImpl::SetTimeInterval(int32_t timeInterval) {}

void LocationRequestConfigImpl::SetPriority(int32_t priority) {}

double LocationInfoImpl::GetLatitude()
{
    return -1;
}

double LocationInfoImpl::GetLongitude()
{
    return -1;
}

double LocationInfoImpl::GetAltitude()
{
    return -1;
}

float LocationInfoImpl::GetAccuracy()
{
    return -1;
}

float LocationInfoImpl::GetSpeed()
{
    return -1;
}

double LocationInfoImpl::GetDirection()
{
    return -1;
}

int64_t LocationInfoImpl::GetTimeStamp()
{
    return -1;
}

int64_t LocationInfoImpl::GetTimeSinceBoot()
{
    return -1;
}

std::vector<std::string> LocationInfoImpl::GetAdditions()
{
    std::vector<std::string> emptyLoc;
    return emptyLoc;
}

LocationProxyAdapterImpl::LocationProxyAdapterImpl() {}

int32_t LocationProxyAdapterImpl::StartLocating(
    std::shared_ptr<LocationRequestConfig> requestConfig, std::shared_ptr<LocationCallbackAdapter> callback)
{
    return -1;
}

bool LocationProxyAdapterImpl::StopLocating(int32_t callbackId)
{
    return false;
}

bool LocationProxyAdapterImpl::EnableAbility(bool isEnabled)
{
    return false;
}

bool LocationProxyAdapterImpl::IsLocationEnabled()
{
    return false;
}
} // namespace OHOS::NWeb
