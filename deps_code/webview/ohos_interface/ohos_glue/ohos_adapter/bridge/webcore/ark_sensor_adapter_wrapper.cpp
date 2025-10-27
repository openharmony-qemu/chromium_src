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

#include "ohos_adapter/bridge/ark_sensor_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_sensor_callback_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkSensorAdapterWrapper::ArkSensorAdapterWrapper(ArkWebRefPtr<ArkSensorAdapter> ref)
    : ctocpp_(ref)
{}

int32_t ArkSensorAdapterWrapper::IsOhosSensorSupported(int32_t sensorTypeId)
{
    return ctocpp_->IsOhosSensorSupported(sensorTypeId);
}

int32_t ArkSensorAdapterWrapper::GetOhosSensorReportingMode(int32_t sensorTypeId)
{
    return ctocpp_->GetOhosSensorReportingMode(sensorTypeId);
}

double ArkSensorAdapterWrapper::GetOhosSensorDefaultSupportedFrequency(int32_t sensorTypeId)
{
    return ctocpp_->GetOhosSensorDefaultSupportedFrequency(sensorTypeId);
}

double ArkSensorAdapterWrapper::GetOhosSensorMinSupportedFrequency(int32_t sensorTypeId)
{
    return ctocpp_->GetOhosSensorMinSupportedFrequency(sensorTypeId);
}

double ArkSensorAdapterWrapper::GetOhosSensorMaxSupportedFrequency(int32_t sensorTypeId)
{
    return ctocpp_->GetOhosSensorMaxSupportedFrequency(sensorTypeId);
}

int32_t ArkSensorAdapterWrapper::SubscribeOhosSensor(int32_t sensorTypeId, int64_t samplingInterval)
{
    return ctocpp_->SubscribeOhosSensor(sensorTypeId, samplingInterval);
}

int32_t ArkSensorAdapterWrapper::UnsubscribeOhosSensor(int32_t sensorTypeId)
{
    return ctocpp_->UnsubscribeOhosSensor(sensorTypeId);
}

int32_t ArkSensorAdapterWrapper::RegistOhosSensorCallback(int32_t sensorTypeId,
    std::shared_ptr<NWeb::SensorCallbackAdapter> callbackAdapter)
{
    if (CHECK_SHARED_PTR_IS_NULL(callbackAdapter)) {
        return ctocpp_->RegistOhosSensorCallback(-1, nullptr);
    }

    return ctocpp_->RegistOhosSensorCallback(sensorTypeId,
        new ArkSensorCallbackAdapterImpl(callbackAdapter));
}
} // namespace OHOS::ArkWeb
