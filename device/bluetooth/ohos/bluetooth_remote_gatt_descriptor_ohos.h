// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
  base::WeakPtrFactory<BluetoothRemoteGattDescriptorOHOS> weak_ptr_factory_{
      this};
};
}  // namespace device
#endif  // DEVICE_BLUETOOTH_OHOS_BLUETOOTH_REMOTE_GATT_DESCRIPTOR_OHOS_H_
