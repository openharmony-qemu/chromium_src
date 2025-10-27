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

#include "ohos_adapter/bridge/ark_sensor_adapter_impl.h"

#include "ohos_adapter/bridge/ark_sensor_callback_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkSensorAdapterImpl::ArkSensorAdapterImpl(std::shared_ptr<OHOS::NWeb::SensorAdapter> ref)
    : real_(ref)
{}

int32_t ArkSensorAdapterImpl::IsOhosSensorSupported(int32_t type)
{
    return real_->IsOhosSensorSupported(type);
}

int32_t ArkSensorAdapterImpl::GetOhosSensorReportingMode(int32_t type)
{
    return real_->GetOhosSensorReportingMode(type);
}

double ArkSensorAdapterImpl::GetOhosSensorDefaultSupportedFrequency(int32_t type)
{
    return real_->GetOhosSensorDefaultSupportedFrequency(type);
}

double ArkSensorAdapterImpl::GetOhosSensorMinSupportedFrequency(int32_t type)
{
    return real_->GetOhosSensorMinSupportedFrequency(type);
}

double ArkSensorAdapterImpl::GetOhosSensorMaxSupportedFrequency(int32_t type)
{
    return real_->GetOhosSensorMaxSupportedFrequency(type);
}

int32_t ArkSensorAdapterImpl::SubscribeOhosSensor(int32_t type, int64_t samplingInterval)
{
    return real_->SubscribeOhosSensor(type, samplingInterval);
}

int32_t ArkSensorAdapterImpl::RegistOhosSensorCallback(int32_t sensorTypeId,
    ArkWebRefPtr<ArkSensorCallbackAdapter> callbackAdapter)
{
    if (!(CHECK_REF_PTR_IS_NULL(callbackAdapter))) {
        return real_->RegistOhosSensorCallback(sensorTypeId,
            std::make_shared<ArkSensorCallbackAdapterWrapper>(callbackAdapter));
    }
    return false;
}

int32_t ArkSensorAdapterImpl::UnsubscribeOhosSensor(int32_t type)
{
    return real_->UnsubscribeOhosSensor(type);
}

} // namespace OHOS::ArkWeb
