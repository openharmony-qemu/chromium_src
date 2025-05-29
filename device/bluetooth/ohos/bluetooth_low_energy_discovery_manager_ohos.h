// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_BLUETOOTH_OHOS_BLUETOOTH_LOW_ENERGY_DISCOVERY_MANAGER_OHOS_H_
#define DEVICE_BLUETOOTH_OHOS_BLUETOOTH_LOW_ENERGY_DISCOVERY_MANAGER_OHOS_H_

#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "device/bluetooth/bluetooth_device.h"
#include "ohos/adapter/bluetooth/bluetooth_low_energy_adapter.h"

namespace device {

// This class will scan for Bluetooth LE device on ohos.
class BluetoothLowEnergyDiscoveryManagerOhos {
 public:
  // Interface for being notified of events during a device discovery session.
  class Observer {
   public:
    // Called when |this| manager has found a device or an update on a device.
    virtual void LowEnergyDeviceUpdated(
        const ohos::adapter::device::ScanResult& result) = 0;

   protected:
    virtual ~Observer() {}
  };

  BluetoothLowEnergyDiscoveryManagerOhos(
      const BluetoothLowEnergyDiscoveryManagerOhos&) = delete;
  BluetoothLowEnergyDiscoveryManagerOhos& operator=(
      const BluetoothLowEnergyDiscoveryManagerOhos&) = delete;

  virtual ~BluetoothLowEnergyDiscoveryManagerOhos();

  // Returns true, if discovery is currently being performed.
  virtual bool IsDiscovering() const;

  // Initiates a discovery session.
  // BluetoothLowEnergyDeviceMac objects discovered within a previous
  // discovery session will be invalid.
  virtual void StartDiscovery(BluetoothDevice::UUIDList services_uuids);

  // Stops a discovery session.
  virtual void StopDiscovery();

  // Returns a new BluetoothLowEnergyDiscoveryManagerOhos.
  static std::unique_ptr<BluetoothLowEnergyDiscoveryManagerOhos> Create(
      Observer* observer);

 protected:
  // The device discovery can really be started when Bluetooth is powered on.
  // The method TryStartDiscovery() is called when it's a good time to try to
  // start the BLE device discovery. It will check if the discovery session has
  // been started and if the Bluetooth is powered and then really start the
  // CoreBluetooth BLE device discovery.
  virtual void TryStartDiscovery();

 private:
  explicit BluetoothLowEnergyDiscoveryManagerOhos(Observer* observer);

  void ProcessScanResult(const ohos::adapter::device::ScanResult& device);

  // Observer interested in notifications from us.
  raw_ptr<Observer> observer_;

  // Discovery has been initiated by calling the API StartDiscovery().
  bool discovering_;

  // A discovery has been initiated but has not started yet because it's
  // waiting for Bluetooth to turn on.
  bool pending_;

  // List of service UUIDs to scan.
  BluetoothDevice::UUIDList services_uuids_;

  base::WeakPtrFactory<BluetoothLowEnergyDiscoveryManagerOhos>
      weak_ptr_factory_{this};
};

}  // namespace device

#endif  // DEVICE_BLUETOOTH_OHOS_BLUETOOTH_LOW_ENERGY_DISCOVERY_MANAGER_OHOS_H_
