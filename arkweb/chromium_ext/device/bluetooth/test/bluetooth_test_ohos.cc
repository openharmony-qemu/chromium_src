// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/device/bluetooth/test/bluetooth_test_ohos.h"

namespace device {
BluetoothTestOhos::BluetoothTestOhos() = default;

BluetoothTestOhos::~BluetoothTestOhos() = default;

bool BluetoothTestOhos::PlatformSupportsLowEnergy() {
  return true;
}

void BluetoothTestOhos::InitWithFakeAdapter() {}

BluetoothDevice* BluetoothTestOhos::SimulateLowEnergyDevice(int device_ordinal) {
  return nullptr;
}

}  // namespace device