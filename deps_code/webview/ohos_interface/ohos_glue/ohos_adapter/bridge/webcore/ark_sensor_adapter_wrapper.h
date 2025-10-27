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

#ifndef ARK_SENSOR_ADAPTER_WRAPPER_H
#define ARK_SENSOR_ADAPTER_WRAPPER_H
#pragma once

#include "sensor_adapter.h"
#include "ohos_adapter/include/ark_sensor_adapter.h"

namespace OHOS::ArkWeb {

class ArkSensorAdapterWrapper : public OHOS::NWeb::SensorAdapter {
public:
    ArkSensorAdapterWrapper(ArkWebRefPtr<ArkSensorAdapter>);

    int32_t IsOhosSensorSupported(int32_t sensorTypeId) override;
    int32_t GetOhosSensorReportingMode(int32_t sensorTypeId) override;
    double GetOhosSensorDefaultSupportedFrequency(int32_t sensorTypeId) override;
    double GetOhosSensorMinSupportedFrequency(int32_t sensorTypeId) override;
    double GetOhosSensorMaxSupportedFrequency(int32_t sensorTypeId) override;
    int32_t SubscribeOhosSensor(int32_t sensorTypeId, int64_t samplingInterval) override;
    int32_t RegistOhosSensorCallback(int32_t sensorTypeId,
        std::shared_ptr<NWeb::SensorCallbackAdapter> callbackAdapter) override;
    int32_t UnsubscribeOhosSensor(int32_t sensorTypeId) override;

private:
    ArkWebRefPtr<ArkSensorAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_SENSOR_ADAPTER_WRAPPER_H
