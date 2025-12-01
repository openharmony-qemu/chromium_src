// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_BLUETOOTH_BLUETOOTH_DEVICE_OHOS_H_
#define DEVICE_BLUETOOTH_BLUETOOTH_DEVICE_OHOS_H_

#include "device/bluetooth/bluetooth_device.h"

namespace device {

class BluetoothAdapter;

class DEVICE_BLUETOOTH_EXPORT BluetoothDeviceOhos : public BluetoothDevice {
 public:
  BluetoothDeviceOhos(const BluetoothDeviceOhos&) = delete;
  BluetoothDeviceOhos& operator=(const BluetoothDeviceOhos&) = delete;
  ~BluetoothDeviceOhos() override;

  virtual bool IsLowEnergyDevice() = 0;

 protected:
  BluetoothDeviceOhos(BluetoothAdapter* adapter);
};
}  // namespace device

#endif  // DEVICE_BLUETOOTH_BLUETOOTH_DEVICE_OHOS_H_
