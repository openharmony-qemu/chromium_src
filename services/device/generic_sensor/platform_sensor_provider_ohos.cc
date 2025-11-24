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

#include "services/device/generic_sensor/platform_sensor_provider_ohos.h"

#include <sensors/oh_sensor.h>

#include "base/logging.h"
#include "services/device/generic_sensor/absolute_orientation_euler_angles_fusion_algorithm_using_accelerometer_and_magnetometer.h"
#include "services/device/generic_sensor/gravity_fusion_algorithm_using_accelerometer.h"
#include "services/device/generic_sensor/linear_acceleration_fusion_algorithm_using_accelerometer.h"
#include "services/device/generic_sensor/orientation_quaternion_fusion_algorithm_using_euler_angles.h"
#include "services/device/generic_sensor/platform_sensor_accelerometer_ohos.h"
#include "services/device/generic_sensor/platform_sensor_ambient_light_ohos.h"
#include "services/device/generic_sensor/platform_sensor_fusion.h"
#include "services/device/generic_sensor/platform_sensor_gravity_ohos.h"
#include "services/device/generic_sensor/platform_sensor_gyroscope_ohos.h"
#include "services/device/generic_sensor/platform_sensor_magnetometer_ohos.h"
#include "services/device/generic_sensor/relative_orientation_euler_angles_fusion_algorithm_using_accelerometer.h"
#include "services/device/generic_sensor/relative_orientation_euler_angles_fusion_algorithm_using_accelerometer_and_gyroscope.h"

namespace device {

PlatformSensorProviderOhos::PlatformSensorProviderOhos() {
  // First invocation,obtaining the number of sensors.
  LOG(ERROR) << "PlatformSensorProviderOhos::PlatformSensorProviderOhos()";
  int32_t result = OH_Sensor_GetInfos(nullptr, &count_);
  if (result != SENSOR_SUCCESS) {
    LOG(ERROR) << "OH_Sensor_GetInfos failed, result code: " << result;
    return;
  }
  if (count_ == 0) {
    LOG(WARNING) << "There are no sensors on this device.";
    return;
  }

  sensors_ = OH_Sensor_CreateInfos(count_);
  if (sensors_ == nullptr) {
    LOG(ERROR) << "OH_Sensor_CreateInfos failed";
    return;
  }
  // Again invocation,obtaining information about all sensors.
  result = OH_Sensor_GetInfos(sensors_, &count_);
  if (result != SENSOR_SUCCESS) {
    LOG(ERROR) << "OH_Sensor_GetInfos failed, result code: " << result;
    return;
  }

  for (uint32_t i = 0; i < count_; i++) {
    Sensor_Type sensor_type;
    if (sensors_[i] == nullptr) {
      LOG(ERROR) << "sensors_[" << i << "] is nullptr";
      continue;
    }
    result = OH_SensorInfo_GetType(sensors_[i], &sensor_type);
    if (result != SENSOR_SUCCESS) {
      LOG(ERROR) << "sensors_[" << i
                 << "] OH_SensorInfo_GetType failed, result code: " << result;
      continue;
    }
    sensor_map_[sensor_type] = sensors_[i];
  }
}

PlatformSensorProviderOhos::~PlatformSensorProviderOhos() {
  if (sensors_ == nullptr) {
    return;
  }
  int32_t result = OH_Sensor_DestroyInfos(sensors_, count_);
  if (result != SENSOR_SUCCESS) {
    LOG(ERROR) << "OH_Sensor_DestroyInfos failed, result code: " << result;
  }
  sensors_ = nullptr;
}

base::WeakPtr<PlatformSensorProvider> PlatformSensorProviderOhos::AsWeakPtr() {
  return weak_factory_.GetWeakPtr();
}

Sensor_Info* PlatformSensorProviderOhos::GetSenorInfo(mojom::SensorType type) {
  Sensor_Type type_ohos = PlatformSensorOhos::MojoTypeToOhType(type);
  auto sensor_iter = sensor_map_.find(type_ohos);
  if (sensor_iter == sensor_map_.end()) {
    LOG(WARNING) << "Sensor does not exist on this device. sensor type: "
                 << PlatformSensorOhos::ToString(type);
    return nullptr;
  }
  return sensor_iter->second;
}

bool PlatformSensorProviderOhos::IsFusionSensorType(
    mojom::SensorType type) const {
  switch (type) {
    case mojom::SensorType::ABSOLUTE_ORIENTATION_EULER_ANGLES:
    case mojom::SensorType::ABSOLUTE_ORIENTATION_QUATERNION:
    case mojom::SensorType::RELATIVE_ORIENTATION_EULER_ANGLES:
    case mojom::SensorType::RELATIVE_ORIENTATION_QUATERNION:
      return true;
    default:
      return false;
  }
}

void PlatformSensorProviderOhos::CreateFusionSensor(
    mojom::SensorType type,
    CreateSensorCallback callback) {
  std::unique_ptr<PlatformSensorFusionAlgorithm> fusion_algorithm;
  switch (type) {
    case mojom::SensorType::ABSOLUTE_ORIENTATION_EULER_ANGLES:
      fusion_algorithm = std::make_unique<
          AbsoluteOrientationEulerAnglesFusionAlgorithmUsingAccelerometerAndMagnetometer>();
      break;
    case mojom::SensorType::ABSOLUTE_ORIENTATION_QUATERNION:
      fusion_algorithm = std::make_unique<
          OrientationQuaternionFusionAlgorithmUsingEulerAngles>(
          true /* absolute */);
      break;
    case mojom::SensorType::RELATIVE_ORIENTATION_EULER_ANGLES:
      if (GetSenorInfo(mojom::SensorType::GYROSCOPE)) {
        fusion_algorithm = std::make_unique<
            RelativeOrientationEulerAnglesFusionAlgorithmUsingAccelerometerAndGyroscope>();
      } else {
        fusion_algorithm = std::make_unique<
            RelativeOrientationEulerAnglesFusionAlgorithmUsingAccelerometer>();
      }
      break;
    case mojom::SensorType::RELATIVE_ORIENTATION_QUATERNION:
      fusion_algorithm = std::make_unique<
          OrientationQuaternionFusionAlgorithmUsingEulerAngles>(
          false /* absolute */);
      break;
    default:
      NOTREACHED();
  }
  PlatformSensorFusion::Create(AsWeakPtr(), std::move(fusion_algorithm),
                               std::move(callback));
}

void PlatformSensorProviderOhos::CreateBaseSensor(
    mojom::SensorType type,
    CreateSensorCallback callback) {
  // Determine whether the base sensor exists.
  Sensor_Info* sensor_info = GetSenorInfo(type);
  if (sensor_info == nullptr) {
    LOG(ERROR) << "SensorInfo is nullptr. sensor type: "
                 << PlatformSensorOhos::ToString(type);
    std::move(callback).Run(nullptr);
    return;
  }

  switch (type) {
    case mojom::SensorType::ACCELEROMETER: {
      auto sensor = base::MakeRefCounted<PlatformSensorAccelerometerOhos>(
          GetSensorReadingSharedBufferForType(type), AsWeakPtr(), sensor_info);
      std::move(callback).Run(std::move(sensor));
      break;
    }
    case mojom::SensorType::AMBIENT_LIGHT: {
      auto sensor = base::MakeRefCounted<PlatformSensorAmbientLightOhos>(
          GetSensorReadingSharedBufferForType(type), AsWeakPtr(), sensor_info);
      std::move(callback).Run(std::move(sensor));
      break;
    }
    case mojom::SensorType::GRAVITY: {
      auto sensor = base::MakeRefCounted<PlatformSensorGravityOhos>(
          GetSensorReadingSharedBufferForType(type), AsWeakPtr(), sensor_info);
      std::move(callback).Run(std::move(sensor));
      break;
    }
    case mojom::SensorType::GYROSCOPE: {
      auto sensor = base::MakeRefCounted<PlatformSensorGyroscopeOhos>(
          GetSensorReadingSharedBufferForType(type), AsWeakPtr(), sensor_info);
      std::move(callback).Run(std::move(sensor));
      break;
    }
    case mojom::SensorType::MAGNETOMETER: {
      auto sensor = base::MakeRefCounted<PlatformSensorMagnetometerOhos>(
          GetSensorReadingSharedBufferForType(type), AsWeakPtr(), sensor_info);
      std::move(callback).Run(std::move(sensor));
      break;
    }
    default: {
      LOG(WARNING) << "Sensor Type: " << PlatformSensorOhos::ToString(type)
                   << ", OHOS doesn't support";
      std::move(callback).Run(nullptr);
    }
  }
}

void PlatformSensorProviderOhos::CreateSensorInternal(
    mojom::SensorType type,
    CreateSensorCallback callback) {
  if (IsFusionSensorType(type)) {
    CreateFusionSensor(type, std::move(callback));
    return;
  }
  CreateBaseSensor(type, std::move(callback));
}

}  // namespace device
