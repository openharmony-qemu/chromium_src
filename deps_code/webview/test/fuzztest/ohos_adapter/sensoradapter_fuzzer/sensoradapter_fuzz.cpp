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

#include "sensoradapter_fuzz.h"

#include <vector>
#include <fuzzer/FuzzedDataProvider.h>

#define private public
#include "sensor_adapter_impl.h"
#undef private

namespace OHOS {
namespace NWeb {
constexpr int MAX_SET_NUMBER = 1000;

class SensorCallbackAdapterMock : public SensorCallbackAdapter {
public:
    ~SensorCallbackAdapterMock() = default;
    void UpdateOhosSensorData(double timestamp,
        double value1, double value2, double value3, double value4) {}
};

bool SensorAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    auto sensorCallbackAdapterMock = std::make_shared<SensorCallbackAdapterMock>();
    auto callback = std::make_shared<SensorCallbackImpl>(sensorCallbackAdapterMock);
    double timestamp = 100;
    double value1 = 0.0;
    double value2 = 0.0;
    double value3 = 0.0;
    double value4 = 0.0;
    callback->UpdateOhosSensorData(timestamp, value1, value2, value3, value4);
    NWeb::SensorAdapterImpl sensorAdapterImpl;
    std::vector<int32_t> sensorTypes = { 2, 3, 4, 5, 6, 8, 9, 11 };

    FuzzedDataProvider dataProvider(data, size);
    for (int32_t sensorTypeId : sensorTypes) {
        int64_t samplingInterval = dataProvider.ConsumeIntegralInRange<int64_t>(0, MAX_SET_NUMBER);
        sensorAdapterImpl.IsOhosSensorSupported(sensorTypeId);
        sensorAdapterImpl.GetOhosSensorReportingMode(sensorTypeId);
        sensorAdapterImpl.GetOhosSensorDefaultSupportedFrequency(sensorTypeId);
        sensorAdapterImpl.GetOhosSensorMinSupportedFrequency(sensorTypeId);
        sensorAdapterImpl.GetOhosSensorMaxSupportedFrequency(sensorTypeId);
        sensorAdapterImpl.SubscribeOhosSensor(sensorTypeId, samplingInterval);
        auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
        sensorAdapterImpl.RegistOhosSensorCallback(sensorTypeId, callbackAdapter);
        SensorEvent event;
        event.sensorTypeId = sensorTypeId;
        event.data = nullptr;
        sensorAdapterImpl.OhosSensorCallback(&event);
        sensorAdapterImpl.UnsubscribeOhosSensor(sensorTypeId);
    }
    SensorEvent event;
    event.data = nullptr;
    sensorAdapterImpl.handleAccelerometerData(callback, &event);
    sensorAdapterImpl.handleLinearAccelerometerData(callback, &event);
    sensorAdapterImpl.handleGravityData(callback, &event);
    sensorAdapterImpl.handleCyroscopeData(callback, &event);
    sensorAdapterImpl.handleMagnetometerData(callback, &event);
    sensorAdapterImpl.handleOrientationData(callback, &event);
    sensorAdapterImpl.handleRotationVectorData(callback, &event);
    sensorAdapterImpl.handleGameRotationVectorData(callback, &event);
    return true;
}
} // namespace NWeb
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::NWeb::SensorAdapterFuzzTest(data, size);
    return 0;
}
