// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_BLUETOOTH_TEST_BLUETOOTH_TEST_OHOS_H_
#define DEVICE_BLUETOOTH_TEST_BLUETOOTH_TEST_OHOS_H_

#include "device/bluetooth/test/bluetooth_test.h"

namespace device {

class BluetoothTestOhos : public BluetoothTestBase {
 public:
  BluetoothTestOhos();
  ~BluetoothTestOhos() override;

  bool PlatformSupportsLowEnergy() override;
  void InitWithFakeAdapter();
  BluetoothDevice* SimulateLowEnergyDevice(int device_ordinal);
};

// Defines common test fixture name. Use TEST_F(BluetoothTest, YourTestName).
using BluetoothTest = BluetoothTestOhos;

}  // namespace device

#endif  // DEVICE_BLUETOOTH_TEST_BLUETOOTH_TEST_OHOS_H_
