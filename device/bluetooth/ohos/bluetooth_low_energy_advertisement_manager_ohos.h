// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_BLUETOOTH_BLUETOOTH_LOW_ENERGY_ADVERTISEMENT_MANAGER_OHOS_H_
#define DEVICE_BLUETOOTH_BLUETOOTH_LOW_ENERGY_ADVERTISEMENT_MANAGER_OHOS_H_

#include "base/memory/scoped_refptr.h"
#include "base/task/single_thread_task_runner.h"
#include "device/bluetooth/ohos/bluetooth_advertisement_ohos.h"
#include "ohos/adapter/bluetooth/bluetooth_low_energy_adapter.h"

namespace device {

class BluetoothAdvertisementOhos;
// Class used by BluetoothAdapterOhos to manage LE advertisements.
// Currently, only a single concurrent BLE advertisement is supported.
class DEVICE_BLUETOOTH_EXPORT BluetoothLowEnergyAdvertisementManagerOhos {
 public:
  BluetoothLowEnergyAdvertisementManagerOhos();

  BluetoothLowEnergyAdvertisementManagerOhos(
      const BluetoothLowEnergyAdvertisementManagerOhos&) = delete;
  BluetoothLowEnergyAdvertisementManagerOhos& operator=(
      const BluetoothLowEnergyAdvertisementManagerOhos&) = delete;

  ~BluetoothLowEnergyAdvertisementManagerOhos();

  // Initializes the advertisement manager.
  void Init(scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner);

  // Registers a new BLE advertisement.
  void RegisterAdvertisement(
      std::unique_ptr<BluetoothAdvertisement::Data> advertisement_data,
      BluetoothAdapter::CreateAdvertisementCallback callback,
      BluetoothAdapter::AdvertisementErrorCallback error_callback);

  // Unregisters an existing BLE advertisement.
  void UnregisterAdvertisement(
      BluetoothAdvertisementOhos* advertisement,
      BluetoothAdvertisement::SuccessCallback success_callback,
      BluetoothAdvertisement::ErrorCallback error_callback);

  // Called when the peripheral manager state changes.
  void OnPeripheralManagerStateChanged();

 private:
  void StartAdvertising();

  int32_t advertising_id_;
  ohos::adapter::device::AdvertisingParams advertisement_param_;

  scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner_;

  scoped_refptr<BluetoothAdvertisementOhos> active_advertisement_;
};

}  // namespace device

#endif  // DEVICE_BLUETOOTH_BLUETOOTH_LOW_ENERGY_ADVERTISEMENT_MANAGER_OHOS_H_
