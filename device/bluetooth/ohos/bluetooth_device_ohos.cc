// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/bluetooth/ohos/bluetooth_device_ohos.h"

#include "device/bluetooth/bluetooth_adapter.h"

namespace device {

BluetoothDeviceOhos::BluetoothDeviceOhos(BluetoothAdapter* adapter)
    : BluetoothDevice(adapter) {}

BluetoothDeviceOhos::~BluetoothDeviceOhos() {}
}  // namespace device
