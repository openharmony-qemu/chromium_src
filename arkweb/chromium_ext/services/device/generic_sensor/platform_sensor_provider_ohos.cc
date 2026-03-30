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

#include "services/device/generic_sensor/platform_sensor_provider_ohos.h"
#include "services/device/generic_sensor/absolute_orientation_euler_angles_fusion_algorithm_using_accelerometer_and_magnetometer.h"
#include "services/device/generic_sensor/platform_sensor_ohos.h"
#include "services/device/generic_sensor/platform_sensor_fusion.h"
#include "services/device/generic_sensor/gravity_fusion_algorithm_using_accelerometer.h"
#include "services/device/generic_sensor/linear_acceleration_fusion_algorithm_using_accelerometer.h"
#include "services/device/generic_sensor/orientation_euler_angles_fusion_algorithm_using_quaternion.h"
#include "services/device/generic_sensor/orientation_quaternion_fusion_algorithm_using_euler_angles.h"
#include "services/device/generic_sensor/relative_orientation_euler_angles_fusion_algorithm_using_accelerometer.h"
#include "services/device/generic_sensor/relative_orientation_euler_angles_fusion_algorithm_using_accelerometer_and_gyroscope.h"

namespace device {

PlatformSensorProviderOHOS::PlatformSensorProviderOHOS() {
}

PlatformSensorProviderOHOS::~PlatformSensorProviderOHOS() = default;

base::WeakPtr<PlatformSensorProvider> PlatformSensorProviderOHOS::AsWeakPtr() {
  return weak_factory_.GetWeakPtr();
}

void PlatformSensorProviderOHOS::CreateSensorInternal(
    mojom::SensorType type,
    //SensorReadingSharedBuffer* reading_buffer,
    CreateSensorCallback callback) {
 SensorReadingSharedBuffer* reading_buffer =
      GetSensorReadingSharedBufferForType(type);
  LOG(INFO) << "CreateSensorInternal. type: " << type;
  switch (type) {
    case mojom::SensorType::GRAVITY:
      CreateGravitySensor(reading_buffer, std::move(callback));
      break;
    case mojom::SensorType::LINEAR_ACCELERATION:
      CreateLinearAccelerationSensor(reading_buffer, std::move(callback));
      break;
    case mojom::SensorType::ABSOLUTE_ORIENTATION_EULER_ANGLES:
      CreateAbsoluteOrientationEulerAnglesSensor(reading_buffer,
                                                 std::move(callback));
      break;
    case mojom::SensorType::ABSOLUTE_ORIENTATION_QUATERNION:
      CreateAbsoluteOrientationQuaternionSensor(reading_buffer,
                                                std::move(callback));
      break;
    case mojom::SensorType::RELATIVE_ORIENTATION_EULER_ANGLES:
      CreateRelativeOrientationEulerAnglesSensor(reading_buffer,
                                                 std::move(callback));
      break;
    case mojom::SensorType::RELATIVE_ORIENTATION_QUATERNION:
      CreateRelativeOrientationQuaternionSensor(reading_buffer,
                                                 std::move(callback));
      break;
    default: {
      auto sensor = PlatformSensorOHOS::Create(type, reading_buffer, AsWeakPtr());
      if (sensor) {
        std::move(callback).Run(sensor);
      }
      break;
    }
  }
}

void PlatformSensorProviderOHOS::CreateGravitySensor(
    SensorReadingSharedBuffer* reading_buffer,
    CreateSensorCallback callback) {
  auto sensor = PlatformSensorOHOS::Create(mojom::SensorType::GRAVITY,
                                              reading_buffer, AsWeakPtr());

  if (sensor) {
    std::move(callback).Run(std::move(sensor));
  } else {
    auto sensor_fusion_algorithm =
        std::make_unique<GravityFusionAlgorithmUsingAccelerometer>();

    PlatformSensorFusion::Create(//reading_buffer,
                                 AsWeakPtr(),
                                 std::move(sensor_fusion_algorithm),
                                 std::move(callback));
  }
}

void PlatformSensorProviderOHOS::CreateLinearAccelerationSensor(
    SensorReadingSharedBuffer* reading_buffer,
    CreateSensorCallback callback) {
  auto sensor = PlatformSensorOHOS::Create(
      mojom::SensorType::LINEAR_ACCELERATION, reading_buffer, AsWeakPtr());

  if (sensor) {
    std::move(callback).Run(std::move(sensor));
  } else {
    auto sensor_fusion_algorithm =
        std::make_unique<LinearAccelerationFusionAlgorithmUsingAccelerometer>();

    PlatformSensorFusion::Create(//reading_buffer,
                                 AsWeakPtr(),
                                 std::move(sensor_fusion_algorithm),
                                 std::move(callback));
  }
}

void PlatformSensorProviderOHOS::CreateAbsoluteOrientationEulerAnglesSensor(
    SensorReadingSharedBuffer* reading_buffer,
    CreateSensorCallback callback) {
  if (GetSensor(mojom::SensorType::ABSOLUTE_ORIENTATION_QUATERNION) != nullptr ||
      PlatformSensorOHOS::IsSupported(mojom::SensorType::ABSOLUTE_ORIENTATION_QUATERNION)) {
    auto sensor_fusion_algorithm =
        std::make_unique<OrientationEulerAnglesFusionAlgorithmUsingQuaternion>(
            true);
    PlatformSensorFusion::Create(//reading_buffer,
                                 AsWeakPtr(),
                                 std::move(sensor_fusion_algorithm),
                                 std::move(callback));
  } else {
    auto sensor_fusion_algorithm = std::make_unique<
        AbsoluteOrientationEulerAnglesFusionAlgorithmUsingAccelerometerAndMagnetometer>();
    PlatformSensorFusion::Create(//reading_buffer,
                                 AsWeakPtr(),
                                 std::move(sensor_fusion_algorithm),
                                 std::move(callback));
  }
}

void PlatformSensorProviderOHOS::CreateAbsoluteOrientationQuaternionSensor(
    SensorReadingSharedBuffer* reading_buffer,
    CreateSensorCallback callback) {
  auto sensor = PlatformSensorOHOS::Create(
      mojom::SensorType::ABSOLUTE_ORIENTATION_QUATERNION, reading_buffer, AsWeakPtr());

  if (sensor) {
    std::move(callback).Run(std::move(sensor));
  } else {
    auto sensor_fusion_algorithm =
        std::make_unique<OrientationQuaternionFusionAlgorithmUsingEulerAngles>(
            true);

    PlatformSensorFusion::Create(//reading_buffer,
                                 AsWeakPtr(),
                                 std::move(sensor_fusion_algorithm),
                                 std::move(callback));
  }
}


void PlatformSensorProviderOHOS::CreateRelativeOrientationEulerAnglesSensor(
    SensorReadingSharedBuffer* reading_buffer,
    CreateSensorCallback callback) {
  if (GetSensor(mojom::SensorType::RELATIVE_ORIENTATION_QUATERNION) != nullptr ||
      PlatformSensorOHOS::IsSupported(mojom::SensorType::RELATIVE_ORIENTATION_QUATERNION)) {
    auto sensor_fusion_algorithm =
        std::make_unique<OrientationEulerAnglesFusionAlgorithmUsingQuaternion>(
            false);
    PlatformSensorFusion::Create(//reading_buffer,
                                 AsWeakPtr(),
                                 std::move(sensor_fusion_algorithm),
                                 std::move(callback));
  } else {
    std::move(callback).Run(nullptr);
  }
}

void PlatformSensorProviderOHOS::CreateRelativeOrientationQuaternionSensor(
    SensorReadingSharedBuffer* reading_buffer,
    CreateSensorCallback callback) {
  auto sensor = PlatformSensorOHOS::Create(
      mojom::SensorType::RELATIVE_ORIENTATION_QUATERNION, reading_buffer, AsWeakPtr());

  if (sensor) {
    std::move(callback).Run(std::move(sensor));
  } else {
    std::move(callback).Run(nullptr);
  }
}
}  // namespace device