// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_BLUETOOTH_OHOS_BLUETOOTH_REMOTE_GATT_SERVICE_OHOS_H_
#define DEVICE_BLUETOOTH_OHOS_BLUETOOTH_REMOTE_GATT_SERVICE_OHOS_H_

#include <stdint.h>

#include <string>
#include <vector>

#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "device/bluetooth/bluetooth_remote_gatt_service.h"
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
  void OnCreateCharacteristic(const std::vector<CharacteristicInfo>& infos);

  raw_ptr<BluetoothDevice> device_;
  std::string device_id_;
  BluetoothUUID uuid_;
  std::string identifier_;
  bool is_primary_;
  base::WeakPtrFactory<BluetoothRemoteGattServiceOHOS> weak_ptr_factory_{this};
};
}  // namespace device

#endif  // DEVICE_BLUETOOTH_OHOS_BLUETOOTH_REMOTE_GATT_SERVICE_OHOS_H_
