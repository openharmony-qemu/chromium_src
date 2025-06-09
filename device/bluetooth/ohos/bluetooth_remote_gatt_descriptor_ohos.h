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

#ifndef DEVICE_BLUETOOTH_OHOS_BLUETOOTH_REMOTE_GATT_DESCRIPTOR_OHOS_H_
#define DEVICE_BLUETOOTH_OHOS_BLUETOOTH_REMOTE_GATT_DESCRIPTOR_OHOS_H_

#include <vector>

#include "base/memory/raw_ptr.h"
#include "device/bluetooth/bluetooth_remote_gatt_descriptor.h"
#include "ohos/adapter/bluetooth/bluetooth_ble_attribute_adapter.h"

namespace device {

class DEVICE_BLUETOOTH_EXPORT BluetoothRemoteGattDescriptorOHOS
    : public BluetoothRemoteGattDescriptor {
 public:
  static std::unique_ptr<BluetoothRemoteGattDescriptorOHOS> Create(
      BluetoothRemoteGattCharacteristic* characteristic,
      BluetoothUUID uuid,
      const std::string& identifier,
      const std::string& device_id);

  BluetoothRemoteGattDescriptorOHOS(const BluetoothRemoteGattDescriptorOHOS&) =
      delete;

  BluetoothRemoteGattDescriptorOHOS& operator=(
      const BluetoothRemoteGattDescriptorOHOS&) = delete;

  ~BluetoothRemoteGattDescriptorOHOS() override;

  std::string GetIdentifier() const override;

  BluetoothUUID GetUUID() const override;

  BluetoothGattCharacteristic::Permissions GetPermissions() const override;

  const std::vector<uint8_t>& GetValue() const override;

  BluetoothRemoteGattCharacteristic* GetCharacteristic() const override;

  void ReadRemoteDescriptor(ValueCallback callback) override;

  void WriteRemoteDescriptor(base::span<const uint8_t> new_value,
                             base::OnceClosure callback,
                             ErrorCallback error_callback) override;

 private:
  BluetoothRemoteGattDescriptorOHOS(
      BluetoothRemoteGattCharacteristic* characteristic,
      BluetoothUUID uuid,
      const std::string identifier,
      const std::string device_id);

  bool HasPendingRead() const { return !read_value_callback_.is_null(); }

  bool HasPendingWrite() const {
    return !write_value_callbacks_.first.is_null();
  }

  void ProcessReadResult(int error_code, const std::vector<uint8_t>& value);
  void ProcessWriteResult(int error_code);

  bool destructor_called_ = false;
  ValueCallback read_value_callback_;
  std::pair<base::OnceClosure, ErrorCallback> write_value_callbacks_;
  raw_ptr<BluetoothRemoteGattCharacteristic> characteristic_;
  std::string identifier_;
  BluetoothUUID uuid_;
  std::vector<uint8_t> value_;
  std::string device_id_;
  base::WeakPtrFactory<BluetoothRemoteGattDescriptorOHOS>
      weak_ptr_factory_{this};
};
}  // namespace device
#endif  // DEVICE_BLUETOOTH_OHOS_BLUETOOTH_REMOTE_GATT_DESCRIPTOR_OHOS_H_
