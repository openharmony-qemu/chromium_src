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
 
#ifndef SERVICES_DEVICE_GENERIC_SENSOR_PLATFORM_SENSOR_OHOS_H_
#define SERVICES_DEVICE_GENERIC_SENSOR_PLATFORM_SENSOR_OHOS_H_
#include <mutex>
#include <unordered_map>

#include <sensors/oh_sensor.h>
#include <sensors/oh_sensor_type.h>
 
#include "services/device/generic_sensor/platform_sensor.h"
 
namespace device {
 
// Implementation of PlatformSensor for OHOS to query sensor.
class PlatformSensorOhos : public PlatformSensor {
 public:
  PlatformSensorOhos(mojom::SensorType sensor_type,
                     SensorReadingSharedBuffer* reading_buffer,
                     base::WeakPtr<PlatformSensorProvider> provider,
                     Sensor_Info* sensor_info);
 
  PlatformSensorOhos(const PlatformSensorOhos&) = delete;
  PlatformSensorOhos& operator=(const PlatformSensorOhos&) = delete;
 
  bool StartSensor(const PlatformSensorConfiguration& configuration) override;
  void StopSensor() override;

  mojom::ReportingMode GetReportingMode() override;

  static std::string ToString(mojom::SensorType sensor_type) {
    switch (sensor_type) {
      case mojom::SensorType::AMBIENT_LIGHT:
        return "AMBIENT_LIGHT";
      case mojom::SensorType::ACCELEROMETER:
        return "ACCELEROMETER";
      case mojom::SensorType::LINEAR_ACCELERATION:
        return "LINEAR_ACCELERATION";
      case mojom::SensorType::GRAVITY:
        return "GRAVITY";
      case mojom::SensorType::GYROSCOPE:
        return "GYROSCOPE";
      case mojom::SensorType::MAGNETOMETER:
        return "MAGNETOMETER";
      case mojom::SensorType::ABSOLUTE_ORIENTATION_EULER_ANGLES:
        return "ABSOLUTE_ORIENTATION_EULER_ANGLES";
      case mojom::SensorType::ABSOLUTE_ORIENTATION_QUATERNION:
        return "ABSOLUTE_ORIENTATION_QUATERNION";
      case mojom::SensorType::RELATIVE_ORIENTATION_EULER_ANGLES:
        return "RELATIVE_ORIENTATION_EULER_ANGLES";
      case mojom::SensorType::RELATIVE_ORIENTATION_QUATERNION:
        return "RELATIVE_ORIENTATION_QUATERNION";
      default:
        return "UNKNOWN";
    }
  }

  static Sensor_Type MojoTypeToOhType(const mojom::SensorType type);

  static std::unordered_map<Sensor_Type, PlatformSensorOhos*>& GetInstances();

  double GetMaximumSupportedFrequency() override;
  double GetMinimumSupportedFrequency() override;

 protected:
  ~PlatformSensorOhos() override;
  bool CheckSensorConfiguration(
      const PlatformSensorConfiguration& configuration) override;
  PlatformSensorConfiguration GetDefaultConfiguration() override;

  RAW_PTR_EXCLUSION Sensor_SubscriptionId* sensor_subscription_id_ = nullptr;
  RAW_PTR_EXCLUSION Sensor_SubscriptionAttribute* sensor_subscription_attr_ = nullptr;
  RAW_PTR_EXCLUSION Sensor_Subscriber* sensor_subscriber_ = nullptr;

  mojom::SensorType sensor_type_;

  virtual void ReadAndUpdate(std::vector<float>& data, uint32_t length) = 0;

 private:
  bool SubscribeParameterInit(const PlatformSensorConfiguration& configuration);

  static void SensorCallback(Sensor_Event* event);

  RAW_PTR_EXCLUSION Sensor_Info* sensor_info_;

  PlatformSensorConfiguration default_configuration_;

  double max_frequency_;
  double min_frequency_;
  const int64_t kMinInterval = 0;
};
 
}  // namespace device
 
#endif  // SERVICES_DEVICE_GENERIC_SENSOR_PLATFORM_SENSOR_OHOS_H_