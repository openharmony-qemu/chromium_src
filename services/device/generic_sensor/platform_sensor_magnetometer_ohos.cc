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

#include "platform_sensor_magnetometer_ohos.h"

#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/time/time.h"
#include "services/device/public/cpp/generic_sensor/sensor_traits.h"

namespace device {

using mojom::SensorType;

PlatformSensorMagnetometerOhos::PlatformSensorMagnetometerOhos(
    SensorReadingSharedBuffer* reading_buffer,
    base::WeakPtr<PlatformSensorProvider> provider,
    Sensor_Info* sensor_info)
    : PlatformSensorOhos(SensorType::MAGNETOMETER,
                         reading_buffer,
                         std::move(provider),
                         sensor_info) {
  auto& instances = PlatformSensorOhos::GetInstances();
  instances[Sensor_Type::SENSOR_TYPE_MAGNETIC_FIELD] = this;
}

PlatformSensorMagnetometerOhos::~PlatformSensorMagnetometerOhos() {
  auto& instances = PlatformSensorOhos::GetInstances();
  instances.erase(Sensor_Type::SENSOR_TYPE_MAGNETIC_FIELD);
}

void PlatformSensorMagnetometerOhos::ReadAndUpdate(std::vector<float>& data,
                                                   uint32_t length) {
  SensorReading reading;
  if (!ReadData(data, length, reading)) {
    return;
  }
  UpdateSharedBufferAndNotifyClients(reading);
}

bool PlatformSensorMagnetometerOhos::ReadData(std::vector<float>& data, uint32_t length, SensorReading& reading) {
  if (length != kMagnDataLength) {
    LOG(WARNING) << "Magnetometer Sensor get data, length error. length: "
                 << length;
    return false;
  }
  reading.magn.x = data[0];
  reading.magn.y = data[1];
  reading.magn.z = data[2];
  reading.als.timestamp =
      (base::TimeTicks::Now() - base::TimeTicks()).InSecondsF();
  return true;
}

}  // namespace device