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
#include "fuzzer/FuzzedDataProvider.h"

#include "arkweb/ohos_adapter_ndk/battery_mgr_adapter/battery_mgr_client_adapter_impl.h"

using namespace OHOS::NWeb;
namespace {
constexpr int MAX_SET_NUMBER = 1000;
class EmptyCallback : public WebBatteryEventCallback {
public:
    EmptyCallback() = default;
private:
    void BatteryInfoChanged(std::shared_ptr<WebBatteryInfo>) {}
};

void DisChargingFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider dataProvider(data, size);
    double level = 0;
    bool isCharging = 0;
    int disChargingTime = dataProvider.ConsumeIntegralInRange<int>(0, MAX_SET_NUMBER);
    int chargingTime = dataProvider.ConsumeIntegralInRange<int>(0, MAX_SET_NUMBER);
    WebBatteryInfoImpl batter(level, isCharging, disChargingTime, chargingTime);
    batter.ChargingTime();
    batter.DisChargingTime();
    batter.GetLevel();
    batter.IsCharging();
}

void RegBatteryEventFuzzTest(const uint8_t* data, size_t size)
{
    size_t callCount = data[0] % 10;
    for (size_t i = 0; i < callCount; i++) {
        BatteryMgrClientAdapterImpl battery;
        std::shared_ptr<WebBatteryEventCallback> callback = std::make_shared<EmptyCallback>();
        battery.RegBatteryEvent(callback);
        battery.StartListen();
        battery.RequestBatteryInfo();
    }
    BatteryMgrClientAdapterImpl battery;
    std::shared_ptr<WebBatteryEventCallback> callback = std::make_shared<EmptyCallback>();
    battery.RegBatteryEvent(callback);
    battery.StartListen();
    battery.RequestBatteryInfo();
}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return 0;
    }
    DisChargingFuzzTest(data, size);
    RegBatteryEventFuzzTest(data, size);
    return 0;
}
