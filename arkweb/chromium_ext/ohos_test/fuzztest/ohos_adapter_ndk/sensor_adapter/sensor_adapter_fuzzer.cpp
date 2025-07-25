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

#include <vector>
#include <fuzzer/FuzzedDataProvider.h>

#define private public
#include "arkweb/ohos_adapter_ndk/sensor_adapter/sensor_adapter_impl.h"
#undef private

using namespace OHOS::NWeb;
namespace {
constexpr int MAX_SET_NUMBER = 1000;

class SensorCallbackAdapterMock : public SensorCallbackAdapter {
public:
    ~SensorCallbackAdapterMock() = default;
    void UpdateOhosSensorData(double timestamp,
        double value1, double value2, double value3, double value4) {}
};

void SensorAdapterFuzzTest(const uint8_t* data, size_t size)
{
    auto sensorCallbackAdapterMock = std::make_shared<SensorCallbackAdapterMock>();
    auto callback = std::make_shared<SensorCallbackImpl>(sensorCallbackAdapterMock);
    double timestamp = 100;
    double value1 = 0.0;
    double value2 = 0.0;
    double value3 = 0.0;
    double value4 = 0.0;
    callback->UpdateOhosSensorData(timestamp, value1, value2, value3, value4);

    FuzzedDataProvider dataProvider(data, size);
    int32_t sensorTypeId = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);
    int64_t samplingInterval = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_SET_NUMBER);
    SensorAdapterImpl sensorAdapterImpl;
    sensorAdapterImpl.IsOhosSensorSupported(sensorTypeId);
    sensorAdapterImpl.GetOhosSensorReportingMode(sensorTypeId);
    sensorAdapterImpl.GetOhosSensorDefaultSupportedFrequency(sensorTypeId);
    sensorAdapterImpl.GetOhosSensorMinSupportedFrequency(sensorTypeId);
    sensorAdapterImpl.GetOhosSensorMaxSupportedFrequency(sensorTypeId);
    sensorAdapterImpl.SubscribeOhosSensor(sensorTypeId, samplingInterval);
    auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
    sensorAdapterImpl.RegistOhosSensorCallback(sensorTypeId, callbackAdapter);
    sensorAdapterImpl.UnsubscribeOhosSensor(sensorTypeId);
}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    SensorAdapterFuzzTest(data, size);
    return 0;
}
