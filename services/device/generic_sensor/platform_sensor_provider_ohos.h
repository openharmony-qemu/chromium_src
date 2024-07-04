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

#ifndef SERVICES_DEVICE_GENERIC_SENSOR_PLATFORM_SENSOR_PROVIDER_OHOS_H_
#define SERVICES_DEVICE_GENERIC_SENSOR_PLATFORM_SENSOR_PROVIDER_OHOS_H_

#include "services/device/generic_sensor/platform_sensor_provider.h"

namespace device {

class PlatformSensorProviderOHOS : public PlatformSensorProvider {
 public:
  PlatformSensorProviderOHOS();

  PlatformSensorProviderOHOS(const PlatformSensorProviderOHOS&) = delete;
  PlatformSensorProviderOHOS& operator=(
      const PlatformSensorProviderOHOS&) = delete;

  ~PlatformSensorProviderOHOS() override;

 protected:
  void CreateSensorInternal(mojom::SensorType type,
                            SensorReadingSharedBuffer* reading_buffer,
                            CreateSensorCallback callback) override;

 private:
  void CreateGravitySensor(SensorReadingSharedBuffer* reading_buffer,
                           CreateSensorCallback callback);
  void CreateLinearAccelerationSensor(SensorReadingSharedBuffer* reading_buffer,
                                      CreateSensorCallback callback);
  void CreateAbsoluteOrientationEulerAnglesSensor(
      SensorReadingSharedBuffer* reading_buffer,
      CreateSensorCallback callback);
  void CreateAbsoluteOrientationQuaternionSensor(
      SensorReadingSharedBuffer* reading_buffer,
      CreateSensorCallback callback);
  void CreateRelativeOrientationEulerAnglesSensor(
      SensorReadingSharedBuffer* reading_buffer,
      CreateSensorCallback callback);
  void CreateRelativeOrientationQuaternionSensor(
      SensorReadingSharedBuffer* reading_buffer,
      CreateSensorCallback callback);
};
}  // namespace device

#endif  // SERVICES_DEVICE_GENERIC_SENSOR_PLATFORM_SENSOR_PROVIDER_OHOS_H_
