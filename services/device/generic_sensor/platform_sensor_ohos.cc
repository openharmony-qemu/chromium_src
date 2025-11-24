/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "services/device/generic_sensor/platform_sensor_ohos.h"

#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/time/time.h"
#include "content/public/browser/browser_thread.h"
#include "ohos_sdk/openharmony/native/sysroot/usr/include/sensors/oh_sensor_type.h"
#include "services/device/public/cpp/generic_sensor/sensor_traits.h"

namespace {

// Frequency hertz, time interval nanoseconds interconversion
double HertzNanosecondConvert(double arg) {
  return 1e9 / arg;
}

}  // namespace

namespace device {

using mojom::SensorType;

//static
std::unordered_map<Sensor_Type, PlatformSensorOhos*>& PlatformSensorOhos::GetInstances() {
    static auto* instances = new std::unordered_map<Sensor_Type, PlatformSensorOhos*>();
    return *instances;
}

PlatformSensorOhos::PlatformSensorOhos(
    mojom::SensorType sensor_type,
    SensorReadingSharedBuffer* reading_buffer,
    base::WeakPtr<PlatformSensorProvider> provider,
    Sensor_Info* sensor_info)
    : PlatformSensor(sensor_type, reading_buffer, std::move(provider)),
      sensor_type_(sensor_type),
      sensor_info_(sensor_info) {
  int64_t interval;
  int32_t result =
      OH_SensorInfo_GetMinSamplingInterval(sensor_info_, &interval);
  if (result != SENSOR_SUCCESS || interval <= kMinInterval) {
    LOG(ERROR) << "OH_SensorInfo_GetMinSamplingInterval failed"
                  ", result code: "
               << result << ", sensor type: " << ToString(sensor_type_);
    max_frequency_ = GetSensorMaxAllowedFrequency(sensor_type_);
  } else {
    max_frequency_ = HertzNanosecondConvert(static_cast<double>(interval));
  }

  result = OH_SensorInfo_GetMaxSamplingInterval(sensor_info_, &interval);
  if (result != SENSOR_SUCCESS || interval <= kMinInterval) {
    LOG(ERROR) << "OH_SensorInfo_GetMaxSamplingInterval failed"
                  ", result code: "
               << result << ", sensor type: " << ToString(sensor_type_);
    min_frequency_ = kMinInterval;
  } else {
    min_frequency_ = HertzNanosecondConvert(static_cast<double>(interval));
  }

  default_configuration_.set_frequency(GetSensorDefaultFrequency(sensor_type_));

  LOG(INFO) << "PlatformSensorOhos::PlatformSensorOhos, init done";
}

PlatformSensorOhos::~PlatformSensorOhos() {
  StopSensor();
}

mojom::ReportingMode PlatformSensorOhos::GetReportingMode() {
  return mojom::ReportingMode::ON_CHANGE;
}

double PlatformSensorOhos::GetMaximumSupportedFrequency() {
  return max_frequency_;
}

double PlatformSensorOhos::GetMinimumSupportedFrequency() {
  return min_frequency_;
}

bool PlatformSensorOhos::CheckSensorConfiguration(
    const PlatformSensorConfiguration& configuration) {
  return configuration.frequency() >= min_frequency_&&
         configuration.frequency() <= max_frequency_;
}

PlatformSensorConfiguration PlatformSensorOhos::GetDefaultConfiguration() {
  return default_configuration_;
}

bool PlatformSensorOhos::SubscribeParameterInit(
    const PlatformSensorConfiguration& configuration) {
  sensor_subscription_id_ = OH_Sensor_CreateSubscriptionId();
  if (sensor_subscription_id_ == nullptr) {
    LOG(ERROR) << "OH_Sensor_CreateSubscriptionId failed, sensor type: "
               << ToString(sensor_type_);
    return false;
  }
  int32_t result = OH_SensorSubscriptionId_SetType(
      sensor_subscription_id_, MojoTypeToOhType(sensor_type_));
  if (result != SENSOR_SUCCESS) {
    LOG(ERROR) << "OH_SensorSubscriptionId_SetType failed"
                  ", result code: "
               << result << ", sensor type: " << ToString(sensor_type_);
    return false;
  }

  sensor_subscription_attr_ = OH_Sensor_CreateSubscriptionAttribute();
  if (sensor_subscription_attr_ == nullptr) {
    LOG(ERROR) << "OH_Sensor_CreateSubscriptionAttribute failed, sensor type: "
               << ToString(sensor_type_);
    return false;
  }
  result = OH_SensorSubscriptionAttribute_SetSamplingInterval(
      sensor_subscription_attr_,
      HertzNanosecondConvert(configuration.frequency()));
  if (result != SENSOR_SUCCESS) {
    LOG(ERROR) << "OH_SensorSubscriptionAttribute_SetSamplingInterval failed"
                  ", result code: "
               << result << ", sensor type: " << ToString(sensor_type_);
    return false;
  }

  sensor_subscriber_ = OH_Sensor_CreateSubscriber();
  if (sensor_subscriber_ == nullptr) {
    LOG(ERROR) << "OH_Sensor_CreateSubscriber failed, sensor type: "
               << ToString(sensor_type_);
    return false;
  }
  result = OH_SensorSubscriber_SetCallback(sensor_subscriber_, *SensorCallback);
  if (result != SENSOR_SUCCESS) {
    LOG(ERROR) << "OH_SensorSubscriber_SetCallback failed"
                  ", result code: "
               << result << ", sensor type: " << ToString(sensor_type_);
    return false;
  }
  return true;
}

bool PlatformSensorOhos::StartSensor(
    const PlatformSensorConfiguration& configuration) {
  if (!SubscribeParameterInit(configuration)) {
    StopSensor();
    return false;
  }

  int32_t result = OH_Sensor_Subscribe(
      sensor_subscription_id_, sensor_subscription_attr_, sensor_subscriber_);
  if (result != SENSOR_SUCCESS) {
    LOG(ERROR) << "OH_Sensor_Subscribe failed"
                  ", result code: "
               << result << ", sensor type: " << ToString(sensor_type_);
    StopSensor();
    return false;
  }
  LOG(INFO) << "Start Sensor Successfully! Sensor type: "
            << ToString(sensor_type_);
  return true;
}

void PlatformSensorOhos::StopSensor() {
  int32_t result;
  if (sensor_subscription_id_ != nullptr && sensor_subscriber_ != nullptr) {
    result = OH_Sensor_Unsubscribe(sensor_subscription_id_, sensor_subscriber_);
    if (result != SENSOR_SUCCESS) {
      LOG(ERROR) << "OH_Sensor_Unsubscribe failed"
                    ", result code: " << result <<
                    ", sensor type: " << ToString(sensor_type_);
    }
  }
 
  if (sensor_subscription_id_ != nullptr) {
    result = OH_Sensor_DestroySubscriptionId(sensor_subscription_id_);
    if (result != SENSOR_SUCCESS) {
      LOG(ERROR) << "OH_Sensor_DestroySubscriptionId failed"
                    ", result code: " << result <<
                    ", sensor type: " << ToString(sensor_type_);
    }
    sensor_subscription_id_ = nullptr;
  }
  if (sensor_subscription_attr_ != nullptr) {
    result = OH_Sensor_DestroySubscriptionAttribute(sensor_subscription_attr_);
    if (result != SENSOR_SUCCESS) {
      LOG(ERROR) << "OH_Sensor_DestroySubscriptionAttribute failed"
                    ", result code: " << result <<
                    ", sensor type: " << ToString(sensor_type_);
    }
    sensor_subscription_attr_ = nullptr;
  }
  if (sensor_subscriber_ != nullptr) {
    result = OH_Sensor_DestroySubscriber(sensor_subscriber_);
    if (result != SENSOR_SUCCESS) {
      LOG(ERROR) << "OH_Sensor_DestroySubscriber failed"
                    ", result code: " << result <<
                    ", sensor type: " << ToString(sensor_type_);
    }
    sensor_subscriber_ = nullptr;
  }
}

//static
Sensor_Type PlatformSensorOhos::MojoTypeToOhType(
    const mojom::SensorType type) {
  switch (type) {
    case mojom::SensorType::AMBIENT_LIGHT:
      return SENSOR_TYPE_AMBIENT_LIGHT;
    case mojom::SensorType::ACCELEROMETER:
      return SENSOR_TYPE_ACCELEROMETER;
    case mojom::SensorType::GRAVITY:
      return SENSOR_TYPE_GRAVITY;
    case mojom::SensorType::GYROSCOPE:
      return SENSOR_TYPE_GYROSCOPE;
    case mojom::SensorType::MAGNETOMETER:
      return SENSOR_TYPE_MAGNETIC_FIELD;
    default:
      return SENSOR_TYPE_HEART_RATE;
  }
}

//static
void PlatformSensorOhos::SensorCallback(Sensor_Event* event) {
  if (event == nullptr) {
    LOG(WARNING) << "Sensor_Event is nullptr";
    return;
  }

  Sensor_Type sensor_type;
  int32_t result = OH_SensorEvent_GetType(event, &sensor_type);
  if (result != SENSOR_SUCCESS) {
    LOG(ERROR) << "OH_SensorEvent_GetType failed, result code: " << result;
    return;
  }
  float* data = nullptr;
  uint32_t length = 0;
  result = OH_SensorEvent_GetData(event, &data, &length);
  if (result != SENSOR_SUCCESS || data == nullptr || length <= 0) {
    LOG(ERROR) << "OH_SensorEvent_GetData failed, result code: " << result
               << ", sensor type: " << static_cast<int>(sensor_type);
    return;
  }
  std::vector<float> data_array(length);
  for (uint32_t i = 0; i < length; i++) {
    data_array[i] = data[i];
  }

  auto task = base::BindOnce(
      [](Sensor_Type sensor_type, std::vector<float> data_array,
         uint32_t length) {
        auto& instances = PlatformSensorOhos::GetInstances();
        auto it = instances.find(sensor_type);
        if (it == instances.end()) {
          LOG(ERROR) << "Can't find instance, sensor type: "
                     << static_cast<int>(sensor_type);
          return;
        }
        it->second->ReadAndUpdate(data_array, length);
      },
      std::move(sensor_type), std::move(data_array), std::move(length));

  content::GetUIThreadTaskRunner({})->PostTask(FROM_HERE, std::move(task));
}

}  // namespace device