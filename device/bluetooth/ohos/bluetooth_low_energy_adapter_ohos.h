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

#ifndef DEVICE_BLUETOOTH_OHOS_BLUETOOTH_LOW_ENERGY_ADAPTER_OHOS_H_
#define DEVICE_BLUETOOTH_OHOS_BLUETOOTH_LOW_ENERGY_ADAPTER_OHOS_H_

#include "device/bluetooth/bluetooth_adapter.h"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "base/task/single_thread_task_runner.h"
#include "device/bluetooth/bluetooth_export.h"
#include "device/bluetooth/ohos/bluetooth_device_ohos.h"
#include "device/bluetooth/ohos/bluetooth_low_energy_advertisement_manager_ohos.h"
#include "device/bluetooth/ohos/bluetooth_low_energy_device_ohos.h"
#include "device/bluetooth/ohos/bluetooth_low_energy_discovery_manager_ohos.h"
#include "device/bluetooth/public/cpp/bluetooth_uuid.h"
#include "ohos/adapter/bluetooth/bluetooth_low_energy_adapter.h"

namespace device {

class DEVICE_BLUETOOTH_EXPORT BluetoothLowEnergyAdapterOhos
    : public BluetoothAdapter,
      public BluetoothLowEnergyDiscoveryManagerOhos::Observer {
 public:
  using DevicesInfo = std::map<std::string, std::string>;
  using GetDevicePairedStatusCallback =
      base::RepeatingCallback<bool(const std::string& address)>;
  BluetoothLowEnergyAdapterOhos(const BluetoothLowEnergyAdapterOhos&) = delete;
  BluetoothLowEnergyAdapterOhos& operator=(
      const BluetoothLowEnergyAdapterOhos&) = delete;

  // BluetoothAdapter overrides:
  std::string GetAddress() const override;
  std::string GetName() const override;
  void SetName(const std::string& name,
               base::OnceClosure callback,
               ErrorCallback error_callback) override;
  bool IsInitialized() const override;
  bool IsPresent() const override;
  bool IsPowered() const override;
  bool IsDiscoverable() const override;
  void SetDiscoverable(bool discoverable,
                       base::OnceClosure callback,
                       ErrorCallback error_callback) override;
  bool IsDiscovering() const override;
  std::unordered_map<BluetoothDevice*, BluetoothDevice::UUIDSet>
  RetrieveGattConnectedDevicesWithDiscoveryFilter(
      const BluetoothDiscoveryFilter& discovery_filter) override;

  void RegisterAdvertisement(
      std::unique_ptr<BluetoothAdvertisement::Data> advertisement_data,
      CreateAdvertisementCallback callback,
      AdvertisementErrorCallback error_callback) override;

  // Creates a GATT connection by calling CoreBluetooth APIs.
  void CreateGattConnection(BluetoothLowEnergyDeviceOhos* device_ohos);

  // Closes the GATT connection by calling CoreBluetooth APIs.
  void DisconnectGatt(BluetoothLowEnergyDeviceOhos* device_ohos);

  void UpdateKnownLowEnergyDevices(DevicesInfo updated_low_energy_devices_info);
  void LowEnergyDeviceUpdated(
      const ohos::adapter::device::ScanResult& result) override;

 protected:
  BluetoothLowEnergyAdapterOhos();
  ~BluetoothLowEnergyAdapterOhos() override;

  virtual void LazyInitialize();

  // Starts a low energy discovery session or update it if one is already
  // running.
  void StartScanLowEnergy();

  // Stops discovery and clears all advertisement data.
  void StopScanLowEnergy();

  // The Initialize() method intentionally does not initialize
  // |low_energy_central_manager_| or |low_energy_peripheral_manager_| because
  // Chromium might not have permission to access the Bluetooth adapter.
  // Methods which require these to be initialized must call LazyInitialize()
  // first.
  bool lazy_initialized_ = false;

 private:
  // Returns the BLE device associated with the uid
  BluetoothLowEnergyDeviceOhos* GetBluetoothLowEnergyDeviceOhos(
      std::string device_address);

  // Returns true if a new device collides with an existing device.
  bool DoesCollideWithKnownDevice(ohos::adapter::device::ScanResult result,
                                  BluetoothLowEnergyDeviceOhos* device_ohos);

  // BluetoothAdapterOhos overrides:
  void StartScanWithFilter(
      std::unique_ptr<BluetoothDiscoveryFilter> discovery_filter,
      DiscoverySessionResultCallback callback) override;
  void UpdateFilter(
      std::unique_ptr<device::BluetoothDiscoveryFilter> discovery_filter,
      DiscoverySessionResultCallback callback) override;
  void StopScan(DiscoverySessionResultCallback callback) override;

  // Discovery manager for Bluetooth Low Energy.
  std::unique_ptr<BluetoothLowEnergyDiscoveryManagerOhos>
      low_energy_discovery_manager_;
  // Advertisement manager for Bluetooth Low Energy.
  std::unique_ptr<BluetoothLowEnergyAdvertisementManagerOhos>
      low_energy_advertisement_manager_;

  DevicesInfo low_energy_devices_info_;
};

}  // namespace device

#endif  // DEVICE_BLUETOOTH_OHOS_BLUETOOTH_LOW_ENERGY_ADAPTER_OHOS_H_
