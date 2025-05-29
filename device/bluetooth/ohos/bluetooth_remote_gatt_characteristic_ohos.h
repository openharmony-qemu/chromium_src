// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_BLUETOOTH_OHOS_BLUETOOTH_REMOTE_GATT_CHARACTERISTIC_OHOS_H_
#define DEVICE_BLUETOOTH_OHOS_BLUETOOTH_REMOTE_GATT_CHARACTERISTIC_OHOS_H_

#include "base/memory/ptr_util.h"
#include "base/memory/weak_ptr.h"
#include "device/bluetooth/bluetooth_remote_gatt_characteristic.h"
#include "device/bluetooth/ohos/bluetooth_adapter_types.h"
#include "ohos/adapter/bluetooth/bluetooth_ble_attribute_adapter.h"

namespace device {

class DEVICE_BLUETOOTH_EXPORT BluetoothRemoteGattCharacteristicOHOS
    : public BluetoothRemoteGattCharacteristic {
 public:
  enum class GattWriteType : int32_t {
    WriteWithResponse = 1,
    WriteWithoutResponse = 2,
  };
  static std::unique_ptr<BluetoothRemoteGattCharacteristicOHOS> Create(
      BluetoothRemoteGattService* service,
      const std::string device_id,
      BluetoothUUID uuid,
      Properties properties,
      const std::string& identifier);

  BluetoothRemoteGattCharacteristicOHOS(
      const BluetoothRemoteGattCharacteristicOHOS&) = delete;

  BluetoothRemoteGattCharacteristicOHOS& operator=(
      const BluetoothRemoteGattCharacteristicOHOS&) = delete;

  ~BluetoothRemoteGattCharacteristicOHOS() override;

  std::string GetIdentifier() const override;

  std::string GetDeviceId() const;

  BluetoothUUID GetUUID() const override;

  Properties GetProperties() const override;

  Permissions GetPermissions() const override;

  const std::vector<uint8_t>& GetValue() const override;

  BluetoothRemoteGattService* GetService() const override;

  void ReadRemoteCharacteristic(ValueCallback callback) override;

  void WriteRemoteCharacteristic(base::span<const uint8_t> value,
                                 WriteType write_type,
                                 base::OnceClosure callback,
                                 ErrorCallback error_callback) override;

  void DeprecatedWriteRemoteCharacteristic(
      base::span<const uint8_t> value,
      base::OnceClosure callback,
      ErrorCallback error_callback) override;

  bool IsReadable() const;

 protected:
  void SubscribeToNotifications(BluetoothRemoteGattDescriptor* ccc_descriptor,
                                base::OnceClosure callback,
                                ErrorCallback error_callback) override;

  void UnsubscribeFromNotifications(
      BluetoothRemoteGattDescriptor* ccc_descriptor,
      base::OnceClosure callback,
      ErrorCallback error_callback) override;

 private:
  BluetoothRemoteGattCharacteristicOHOS(BluetoothRemoteGattService* service,
                                        const std::string device_id,
                                        BluetoothUUID uuid,
                                        Properties properties,
                                        const std::string identifier);
  bool HasPendingRead() const {
    return !read_characteristic_value_callback_.is_null();
  }
  bool HasPendingWrite() const {
    return !write_characteristic_value_callbacks_.first.is_null();
  }

  void ProcessReadResult(int error_code, const std::vector<uint8_t>& value);
  void ProcessWriteResult(int error_code);
  void OnCreateDescriptor(const std::vector<DescriptorInfo>& descriptor_infos);
  void SetValue(const std::vector<uint8_t>& value);

  bool destructor_called_ = false;
  ValueCallback read_characteristic_value_callback_;
  std::pair<base::OnceClosure, ErrorCallback>
      write_characteristic_value_callbacks_;
  raw_ptr<BluetoothRemoteGattService> service_;
  std::string device_id_;
  BluetoothUUID uuid_;
  Properties properties_;
  std::string identifier_;
  std::vector<uint8_t> value_;
  base::WeakPtrFactory<BluetoothRemoteGattCharacteristicOHOS> weak_ptr_factory_{
      this};
};
}  // namespace device
#endif  // DEVICE_BLUETOOTH_OHOS_BLUETOOTH_REMOTE_GATT_DESCRIPTOR_OHOS_H_
