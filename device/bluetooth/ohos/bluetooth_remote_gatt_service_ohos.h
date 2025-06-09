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

#ifndef DEVICE_BLUETOOTH_OHOS_BLUETOOTH_REMOTE_GATT_SERVICE_OHOS_H_
#define DEVICE_BLUETOOTH_OHOS_BLUETOOTH_REMOTE_GATT_SERVICE_OHOS_H_

#include "device/bluetooth/bluetooth_remote_gatt_service.h"

#include <stdint.h>

#include <string>
#include <vector>

#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "device/bluetooth/ohos/bluetooth_adapter_types.h"
#include "ohos/adapter/bluetooth/bluetooth_ble_attribute_adapter.h"

namespace device {

class DEVICE_BLUETOOTH_EXPORT BluetoothRemoteGattServiceOHOS
    : public BluetoothRemoteGattService {
 public:
  static std::unique_ptr<BluetoothRemoteGattServiceOHOS> Create(
      BluetoothDevice* device,
      const std::string& device_id,
      BluetoothUUID uuid,
      const bool is_primary,
      const std::string& identifier);

  BluetoothRemoteGattServiceOHOS(const BluetoothRemoteGattServiceOHOS&) =
      delete;
  BluetoothRemoteGattServiceOHOS& operator=(
      const BluetoothRemoteGattServiceOHOS&) = delete;
  ~BluetoothRemoteGattServiceOHOS() override;
  std::string GetIdentifier() const override;
  BluetoothUUID GetUUID() const override;
  bool IsPrimary() const override;
  BluetoothDevice* GetDevice() const override;
  std::vector<BluetoothRemoteGattService*> GetIncludedServices() const override;

 private:
  BluetoothRemoteGattServiceOHOS(BluetoothDevice* device,
                                 const std::string device_id,
                                 BluetoothUUID service_uuid,
                                 const bool is_primary,
                                 const std::string identifier);
  void OnCreateCharacteristic(const std::vector<CharacteristicInfo> &infos);

  raw_ptr<BluetoothDevice> device_;
  std::string device_id_;
  BluetoothUUID uuid_;
  std::string identifier_;
  bool is_primary_;
  base::WeakPtrFactory<BluetoothRemoteGattServiceOHOS>
      weak_ptr_factory_{this};
};
}  // namespace device

#endif  // DEVICE_BLUETOOTH_OHOS_BLUETOOTH_REMOTE_GATT_SERVICE_OHOS_H_
