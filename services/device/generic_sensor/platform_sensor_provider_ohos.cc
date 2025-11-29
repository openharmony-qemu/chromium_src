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
#include "services/device/generic_sensor/platform_sensor_ambient_light_ohos.h"
#include "services/device/generic_sensor/platform_sensor_gyroscope_ohos.h"

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

void PlatformSensorProviderOhos::CreateSensorInternal(
    mojom::SensorType type,
    CreateSensorCallback callback) {
  // Determine whether the sensor exists.
  Sensor_Type type_ohos = PlatformSensorOhos::MojoTypeToOhType(type);
  auto sensor_iter = sensor_map_.find(type_ohos);
  if (sensor_iter == sensor_map_.end()) {
    LOG(WARNING) << "This sensor does not exist on this device.";
    std::move(callback).Run(nullptr);
    return;
  }

  // Create Sensors here.
  switch (type) {
    case mojom::SensorType::AMBIENT_LIGHT: {
      auto sensor = base::MakeRefCounted<PlatformSensorAmbientLightOhos>(
          GetSensorReadingSharedBufferForType(type), AsWeakPtr(),
          sensor_iter->second);
      std::move(callback).Run(std::move(sensor));
      break;
    }
    case mojom::SensorType::GYROSCOPE: {
      auto sensor = base::MakeRefCounted<PlatformSensorGyroscopeOhos>(
          GetSensorReadingSharedBufferForType(type), AsWeakPtr(),
          sensor_iter->second);
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

}  // namespace device
