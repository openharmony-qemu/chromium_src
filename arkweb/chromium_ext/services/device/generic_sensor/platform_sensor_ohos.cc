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

#include "services/device/generic_sensor/platform_sensor_ohos.h"

#include "base/functional/bind.h"
#include "base/metrics/field_trial_params.h"
#include "services/device/public/cpp/device_features.h"

#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace device {

constexpr double SECONDS_IN_NANOSECOND = 0.000000001;
constexpr double NANOSECONDS_IN_SECOND = 1000000000.0;

OHOSSensorCallback::OHOSSensorCallback(
  const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
  base::WeakPtr<PlatformSensorOHOS> platform_sensor_ohos)
  : platform_sensor_ohos_(platform_sensor_ohos), task_runner_(task_runner) {
  DCHECK(task_runner_.get());
  DCHECK(platform_sensor_ohos_);
}

OHOSSensorCallback::~OHOSSensorCallback() {
}

void OHOSSensorCallback::UpdateOhosSensorData(double timestamp,
    double value1, double value2, double value3, double value4) {
  if (task_runner_) {
    task_runner_->PostTask(
      FROM_HERE, base::BindOnce(
        &PlatformSensorOHOS::UpdatePlatformSensorReading,
        platform_sensor_ohos_,
        timestamp, value1, value2, value3, value4));
  }
}

// static
scoped_refptr<PlatformSensorOHOS> PlatformSensorOHOS::Create(
    mojom::SensorType type,
    SensorReadingSharedBuffer* reading_buffer,
    base::WeakPtr<PlatformSensorProvider> provider) {
  auto sensor = base::MakeRefCounted<PlatformSensorOHOS>(
      type, reading_buffer, provider);
  if (!sensor->IsSupported()) {
    return nullptr;
  }
  return sensor;
}

// static
bool PlatformSensorOHOS::IsSupported(
      mojom::SensorType type) {
  std::unique_ptr<OHOS::NWeb::SensorAdapter> sensor_adapter =
    OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateSensorAdapter();
  if (sensor_adapter) {
    int32_t ret =
      sensor_adapter->IsOhosSensorSupported(static_cast<int32_t>(type));
    if (ret == OHOS::NWeb::SENSOR_SUCCESS) {
      return true;
    }
  }
  return false;
}

PlatformSensorOHOS::PlatformSensorOHOS(
    mojom::SensorType type,
    SensorReadingSharedBuffer* reading_buffer,
    base::WeakPtr<PlatformSensorProvider> provider)
    : PlatformSensor(type, reading_buffer, std::move(provider)) {
  LOG(INFO) << "PlatformSensorOHOS. Type: " << GetType();
  sensor_adapter_ =
    OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateSensorAdapter();
  if (sensor_adapter_) {
    int32_t ret =
      sensor_adapter_->IsOhosSensorSupported(static_cast<int32_t>(type));
    if (ret == OHOS::NWeb::SENSOR_SUCCESS) {
      is_supported_ = true;
    }
  } else {
    LOG(ERROR) << "sensor adapter create failed.";
  }
}

PlatformSensorOHOS::~PlatformSensorOHOS() {
}

mojom::ReportingMode PlatformSensorOHOS::GetReportingMode() {
  if (!sensor_adapter_) {
    LOG(ERROR) << "GetReportingMode Error, sensor_adapter_ is null, Type: "
      << GetType();
    return mojom::ReportingMode::CONTINUOUS;
  }
  int32_t ret = sensor_adapter_->GetOhosSensorReportingMode(
      static_cast<int32_t>(GetType()));
  if (ret == OHOS::NWeb::SENSOR_DATA_REPORT_ON_CHANGE) {
    return mojom::ReportingMode::ON_CHANGE;
  }
  return mojom::ReportingMode::CONTINUOUS;
}

PlatformSensorConfiguration PlatformSensorOHOS::GetDefaultConfiguration() {
  if (!sensor_adapter_) {
    LOG(ERROR) << "GetDefaultConfiguration Error, sensor_adapter_ is null, "
      << "Type: " << GetType();
    return PlatformSensorConfiguration(0.0);
  }
  double frequency = sensor_adapter_->GetOhosSensorDefaultSupportedFrequency(
      static_cast<int32_t>(GetType()));
  LOG(INFO) << "GetDefaultConfiguration. frequency: " << frequency;
  return PlatformSensorConfiguration(frequency);
}

double PlatformSensorOHOS::GetMaximumSupportedFrequency() {
  if (!sensor_adapter_) {
    LOG(ERROR) << "GetMaximumSupportedFrequency Error, sensor_adapter_ is null, "
      << "Type: " << GetType();
    return 0.0;
  }
  double frequency = sensor_adapter_->GetOhosSensorMaxSupportedFrequency(
      static_cast<int32_t>(GetType()));
  LOG(INFO) << "GetMaximumSupportedFrequency. frequency: " << frequency;
  return frequency;
}

double PlatformSensorOHOS::GetMinimumSupportedFrequency() {
  if (!sensor_adapter_) {
    LOG(ERROR) << "GetMinimumSupportedFrequency Error, sensor_adapter_ is null, "
      << "Type: " << GetType();
    return 0.0;
  }
  double frequency = sensor_adapter_->GetOhosSensorMinSupportedFrequency(
      static_cast<int32_t>(GetType()));
  LOG(INFO) << "GetMinimumSupportedFrequency. frequency: " << frequency;
  return frequency;
}

bool PlatformSensorOHOS::StartSensor(
    const PlatformSensorConfiguration& configuration) {
  LOG(INFO) << "StartSensor Start. Type: " << GetType()
    << ", frequency: " << configuration.frequency();
  if (!sensor_adapter_) {
    LOG(ERROR) << "StartSensor Error, sensor_adapter_ is null, "
      << "Type: " << GetType();
    return false;
  }
  if (configuration.frequency() <= 0.001f) {
    LOG(ERROR) << "StartSensor Error, frequency is invalid, "
      << "Type: " << GetType();
    return false;
  }
  int64_t sampling_interval =
    (int64_t)(NANOSECONDS_IN_SECOND / configuration.frequency());
  if (current_sampling_interval_ == sampling_interval) {
    LOG(INFO) << "StartSensor, Already run with same sampling interval."
      << " Type: "<< GetType();
    return true;
  }

  if (current_sampling_interval_ != 0) {
    StopSensor();
  }

  task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  auto sensor_callback = std::make_unique<OHOSSensorCallback>(task_runner_,
    weak_factory_.GetWeakPtr());
  int32_t ret = sensor_adapter_->RegistOhosSensorCallback(
      static_cast<int32_t>(GetType()), std::move(sensor_callback));
  if (ret != OHOS::NWeb::SENSOR_SUCCESS) {
    LOG(ERROR) << "RegistOhosSensorCallback Error, Type: "
      << GetType() << ", ret = " << ret;
    return false;
  }

  ret = sensor_adapter_->SubscribeOhosSensor(static_cast<int32_t>(GetType()),
    sampling_interval);
  current_sampling_interval_ = sampling_interval;
  if (ret != OHOS::NWeb::SENSOR_SUCCESS) {
    LOG(ERROR) << "SubscribeOhosSensor Error, Type: "
      << GetType() << ", ret = " << ret;
    StopSensor();
    NotifyPlatformSensorError();
    return false;
  }
  LOG(INFO) << "StartSensor End. Type: " << GetType();
  return true;
}

void PlatformSensorOHOS::StopSensor() {
  if (!sensor_adapter_) {
    LOG(ERROR) << "StopSensor Error, sensor_adapter_ is null, "
      << "Type: " << GetType();
    return;
  }
  int32_t ret = sensor_adapter_->UnsubscribeOhosSensor(
      static_cast<int32_t>(GetType()));
  if (ret != OHOS::NWeb::SENSOR_SUCCESS) {
    LOG(ERROR) << "UnsubscribeOhosSensor Error, Type: "
      << GetType() << ", ret: " << ret;
  }
  current_sampling_interval_ = 0;
  LOG(INFO) << "StopSensor. Type: " << GetType();
}

bool PlatformSensorOHOS::CheckSensorConfiguration(
    const PlatformSensorConfiguration& configuration) {
  if (!sensor_adapter_) {
    LOG(ERROR) << "CheckSensorConfiguration Error, sensor_adapter_ is null, "
      << "Type: " << GetType();
    return false;
  }
  double frequency = configuration.frequency();
  double max_frequency = sensor_adapter_->GetOhosSensorMaxSupportedFrequency(
      static_cast<int32_t>(GetType()));
  double min_frequency = sensor_adapter_->GetOhosSensorMinSupportedFrequency(
      static_cast<int32_t>(GetType()));

  if ((frequency < min_frequency) || (frequency > max_frequency)) {
    LOG(INFO) << "CheckSensorConfiguration false, frequency: " << frequency;
    return false;
  }
  LOG(INFO) << "CheckSensorConfiguration true, frequency: " << frequency;
  return true;
}

void PlatformSensorOHOS::NotifyPlatformSensorError(){
  PostTaskToMainSequence(
    FROM_HERE,
    base::BindOnce(&PlatformSensorOHOS::NotifySensorError, this));
}


void PlatformSensorOHOS::UpdatePlatformSensorReading(double timestamp,
    double value1, double value2, double value3, double value4) {
  SensorReading reading;
  reading.raw.timestamp = timestamp * SECONDS_IN_NANOSECOND;
  reading.raw.values[0] = value1;
  reading.raw.values[1] = value2;
  reading.raw.values[2] = value3;
  reading.raw.values[3] = value4;
  UpdateSharedBufferAndNotifyClients(reading);
}

}  // namespace device