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

#ifndef SERVICES_DEVICE_GENERIC_SENSOR_PLATFORM_SENSOR_PROVIDER_OHOS_H_
#define SERVICES_DEVICE_GENERIC_SENSOR_PLATFORM_SENSOR_PROVIDER_OHOS_H_

#include <sensors/oh_sensor_type.h>

#include "services/device/generic_sensor/platform_sensor_provider.h"

namespace device {

class PlatformSensorProviderOhos : public PlatformSensorProvider {
 public:
  PlatformSensorProviderOhos();

  PlatformSensorProviderOhos(const PlatformSensorProviderOhos&) = delete;
  PlatformSensorProviderOhos& operator=(const PlatformSensorProviderOhos&) =
      delete;

  ~PlatformSensorProviderOhos() override;
  base::WeakPtr<PlatformSensorProvider> AsWeakPtr() override;

 protected:
  void CreateSensorInternal(mojom::SensorType type,
                            CreateSensorCallback callback) override;

 private:
  bool IsFusionSensorType(mojom::SensorType type) const;
  void CreateFusionSensor(mojom::SensorType type,
                          CreateSensorCallback callback);
  void CreateBaseSensor(mojom::SensorType type, CreateSensorCallback callback);
  Sensor_Info* GetSenorInfo(mojom::SensorType type);

  std::unordered_map <Sensor_Type, Sensor_Info*> sensor_map_;
  RAW_PTR_EXCLUSION Sensor_Info** sensors_;
  uint32_t count_;

  base::WeakPtrFactory<PlatformSensorProviderOhos> weak_factory_{this};
};

}  // namespace device

#endif  // SERVICES_DEVICE_GENERIC_SENSOR_PLATFORM_SENSOR_PROVIDER_OHOS_H_
