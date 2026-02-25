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

#ifndef SERVICES_DEVICE_GENERIC_SENSOR_PLATFORM_SENSOR_MAGNETOMETER_OHOS_H_
#define SERVICES_DEVICE_GENERIC_SENSOR_PLATFORM_SENSOR_MAGNETOMETER_OHOS_H_

#include "services/device/generic_sensor/platform_sensor_ohos.h"

namespace device {

// Implementation of PlatformSensor for OHOS to query the gyroscope sensor.
// This is a single instance object per browser process which is created by
// PlatformSensorProviderOhos.
class PlatformSensorMagnetometerOhos : public PlatformSensorOhos {
 public:
  PlatformSensorMagnetometerOhos(SensorReadingSharedBuffer* reading_buffer,
                              base::WeakPtr<PlatformSensorProvider> provider,
                              Sensor_Info* sensor_info);

  PlatformSensorMagnetometerOhos(const PlatformSensorMagnetometerOhos&) = delete;
  PlatformSensorMagnetometerOhos& operator=(const PlatformSensorMagnetometerOhos&) =
      delete;
  ~PlatformSensorMagnetometerOhos();

protected:
  void ReadAndUpdate(std::vector<float>& data, uint32_t length) override;

 private:
  const uint32_t kMagnDataLength = 3;

  bool ReadData(std::vector<float>& data, uint32_t length, SensorReading& reading);
};

}  // namespace device

#endif  // SERVICES_DEVICE_GENERIC_SENSOR_PLATFORM_SENSOR_MAGNETOMETER_OHOS_H_