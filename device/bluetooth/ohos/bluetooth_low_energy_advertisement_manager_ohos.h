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
