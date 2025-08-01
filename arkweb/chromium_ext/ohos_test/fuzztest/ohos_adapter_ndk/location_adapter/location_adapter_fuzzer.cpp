/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include "fuzzer/FuzzedDataProvider.h"
#define private public
#include "arkweb/ohos_adapter_ndk/location_adapter/location_instance_impl.h"
#include "arkweb/ohos_adapter_ndk/location_adapter/location_proxy_adapter_impl.h"

using namespace OHOS::NWeb;
namespace {
class LocationCallbackAdapterMock : public LocationCallbackAdapter {
public:
    LocationCallbackAdapterMock() = default;
    ~LocationCallbackAdapterMock() override = default;
    void OnLocationReport(const std::shared_ptr<LocationInfo> location) override {}
    void OnLocatingStatusChange(const int status) override {}
    void OnErrorReport(const int errorCode) override {}
};

void LocationRequestConfigImplFuzzTest(FuzzedDataProvider& fdp)
{
    auto requestConfig = LocationInstance::GetInstance().CreateLocationRequestConfig();
    requestConfig->SetScenario(fdp.ConsumeIntegral<int32_t>());
    requestConfig->SetFixNumber(fdp.ConsumeIntegral<int32_t>());
    requestConfig->SetMaxAccuracy(fdp.ConsumeIntegral<int32_t>());
    requestConfig->SetDistanceInterval(fdp.ConsumeIntegral<int32_t>());
    requestConfig->SetTimeInterval(fdp.ConsumeIntegral<int32_t>());
    requestConfig->SetPriority(fdp.ConsumeIntegral<int32_t>());
}

void LocationInfoImplFuzzTest(FuzzedDataProvider& fdp)
{
    LocationInfoImpl info;
    Location_BasicInfo basicInfo;
    info.SetBasicInfo(&basicInfo);
    info.GetLatitude();
    info.GetLongitude();
    info.GetAltitude();
    info.GetAccuracy();
    info.GetSpeed();
    info.GetDirection();
    info.GetTimeStamp();
    info.GetTimeSinceBoot();
    info.GetAdditions();
}

void LocationProxyAdapterImplFuzzTest(FuzzedDataProvider& fdp)
{
    auto requestConfig = LocationInstance::GetInstance().CreateLocationRequestConfig();
    auto proxyAdapter = LocationInstance::GetInstance().CreateLocationProxyAdapter();
    std::shared_ptr<LocationCallbackAdapter> callback = std::make_shared<LocationCallbackAdapterMock>();
    proxyAdapter->StartLocating(requestConfig, callback);
    proxyAdapter->StopLocating(fdp.ConsumeIntegral<int32_t>());
    proxyAdapter->EnableAbility(fdp.ConsumeBool());
    proxyAdapter->IsLocationEnabled();
}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    LocationRequestConfigImplFuzzTest(fdp);
    LocationInfoImplFuzzTest(fdp);
    LocationProxyAdapterImplFuzzTest(fdp);
    return 0;
}