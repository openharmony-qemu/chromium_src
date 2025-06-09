/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
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

#include "device/bluetooth/ohos/bluetooth_remote_gatt_service_ohos.h"

#include <utility>

#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "base/strings/stringprintf.h"
#include "device/bluetooth/bluetooth_device.h"
#include "device/bluetooth/ohos/bluetooth_adapter_types.h"
#include "device/bluetooth/ohos/bluetooth_remote_gatt_characteristic_ohos.h"

using namespace ohos::adapter::device;

namespace device {
std::unique_ptr<BluetoothRemoteGattServiceOHOS>
BluetoothRemoteGattServiceOHOS::Create(BluetoothDevice* device,
                                       const std::string& device_id,
                                       BluetoothUUID uuid,
                                       const bool is_primary,
                                       const std::string& identifier) {
  return base::WrapUnique(new BluetoothRemoteGattServiceOHOS(
      device, device_id, uuid, is_primary, identifier));
}

BluetoothRemoteGattServiceOHOS::~BluetoothRemoteGattServiceOHOS() = default;

std::string BluetoothRemoteGattServiceOHOS::GetIdentifier() const {
  return identifier_;
}

BluetoothUUID BluetoothRemoteGattServiceOHOS::GetUUID() const {
  return uuid_;
}

bool BluetoothRemoteGattServiceOHOS::IsPrimary() const {
  return is_primary_;
}

BluetoothDevice* BluetoothRemoteGattServiceOHOS::GetDevice() const {
  return device_;
}

std::vector<BluetoothRemoteGattService*>
BluetoothRemoteGattServiceOHOS::GetIncludedServices() const {
  NOTIMPLEMENTED();
  return {};
}

void BluetoothRemoteGattServiceOHOS::OnCreateCharacteristic(
    const std::vector<CharacteristicInfo> &infos) {
  CharacteristicMap characteristics;
  for (const CharacteristicInfo &info : infos) {
    auto characteristic = BluetoothRemoteGattCharacteristicOHOS::Create(
        this, info.device_id, BluetoothUUID(info.characteristic_uuid),
        info.properties, info.identifier);
    std::string identifier = characteristic->GetIdentifier();
    auto iter = characteristics_.find(identifier);
    if (iter != characteristics_.end()) {
      iter = characteristics.emplace(std::move(*iter)).first;
    } else {
      iter = characteristics
                 .emplace(std::move(identifier), std::move(characteristic))
                 .first;
    }
  }
  std::swap(characteristics, characteristics_);
  SetDiscoveryComplete(true);
}

BluetoothRemoteGattServiceOHOS::BluetoothRemoteGattServiceOHOS(
    BluetoothDevice* device,
    const std::string device_id,
    BluetoothUUID uuid,
    const bool is_primary,
    const std::string identifier)
    : device_(device),
      device_id_(device_id),
      uuid_(std::move(uuid)),
      identifier_(identifier),
      is_primary_(is_primary) {
  auto callback = [this](std::vector<CharacteristicInfo> infos) {
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(&BluetoothRemoteGattServiceOHOS::OnCreateCharacteristic,
                       weak_ptr_factory_.GetWeakPtr(),
                       std::move(infos)));
  };
  BluetoothBleAttributeAdapter::CreateCharacteristic(identifier,
                                                     device_id,
                                                     std::move(callback));
}

}  // namespace device
