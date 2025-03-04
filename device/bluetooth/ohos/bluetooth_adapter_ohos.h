// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_BLUETOOTH_BLUETOOTH_ADAPTER_OHOS_H_
#define DEVICE_BLUETOOTH_BLUETOOTH_ADAPTER_OHOS_H_

#include <memory>

#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/task/sequenced_task_runner.h"
#include "device/bluetooth/bluetooth_adapter.h"
#include "device/bluetooth/ohos/bluetooth_discovery_manager_ohos.h"
#include "device/bluetooth/ohos/bluetooth_low_energy_adapter_ohos.h"

namespace device {

class DEVICE_BLUETOOTH_EXPORT BluetoothAdapterOhos
    : public BluetoothLowEnergyAdapterOhos,
      public BluetoothDiscoveryManagerOhos::Observer {
 public:
  BluetoothAdapterOhos(const BluetoothAdapterOhos&) = delete;
  BluetoothAdapterOhos& operator=(const BluetoothAdapterOhos&) = delete;
  BluetoothAdapterOhos();
  ~BluetoothAdapterOhos() override;

  // BluetoothAdapter:
  void Initialize(base::OnceClosure callback) override;
  std::string GetAddress() const override;
  std::string GetName() const override;
  void SetName(const std::string& name,
               base::OnceClosure callback,
               ErrorCallback error_callback) override;
  bool IsInitialized() const override;
  bool IsPresent() const override;
  bool IsPowered() const override;
  PermissionStatus GetOsPermissionStatus() const override;
  bool IsDiscoverable() const override;
  void SetDiscoverable(bool discoverable,
                       base::OnceClosure callback,
                       ErrorCallback error_callback) override;
  bool IsDiscovering() const override;
  UUIDList GetUUIDs() const override;
  void CreateRfcommService(const BluetoothUUID& uuid,
                           const ServiceOptions& options,
                           CreateServiceCallback callback,
                           CreateServiceErrorCallback error_callback) override;
  void CreateL2capService(const BluetoothUUID& uuid,
                          const ServiceOptions& options,
                          CreateServiceCallback callback,
                          CreateServiceErrorCallback error_callback) override;
  void RegisterAdvertisement(
      std::unique_ptr<BluetoothAdvertisement::Data> advertisement_data,
      CreateAdvertisementCallback callback,
      AdvertisementErrorCallback error_callback) override;
  BluetoothLocalGattService* GetGattService(
      const std::string& identifier) const override;

  // BluetoothDiscoveryManagerOhos::Observer overrides:
  void ClassicDeviceFound(const std::string& address) override;
  void ClassicDiscoveryStopped(bool unexpected) override;

  void NotifyBluetoothChanged(int32_t bluetooth_state);

 protected:
  // BluetoothAdapter override:
  base::WeakPtr<BluetoothAdapter> GetWeakPtr() override;

  bool SetPoweredImpl(bool powered) override;
  void StartScanWithFilter(
      std::unique_ptr<BluetoothDiscoveryFilter> discovery_filter,
      DiscoverySessionResultCallback callback) override;
  void UpdateFilter(std::unique_ptr<BluetoothDiscoveryFilter> discovery_filter,
                    DiscoverySessionResultCallback callback) override;
  void StopScan(DiscoverySessionResultCallback callback) override;
  void RemovePairingDelegateInternal(
      BluetoothDevice::PairingDelegate* pairing_delegate) override;

 private:
  void ClassicDeviceAdded(const std::string& address);
  void OnBluetoothChanged(int32_t bluetooth_state);

  bool is_powered_{false};
  bool is_initialized_{false};

  // Discovery manager for Bluetooth Classic.
  std::unique_ptr<BluetoothDiscoveryManagerOhos> classic_discovery_manager_;

  base::WeakPtrFactory<BluetoothAdapterOhos> weak_ptr_factory_{this};
};

}  // namespace device

#endif  // DEVICE_BLUETOOTH_BLUETOOTH_ADAPTER_OHOS_H_
