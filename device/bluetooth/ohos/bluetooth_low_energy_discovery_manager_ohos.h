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
  static std::unique_ptr<BluetoothLowEnergyDiscoveryManagerOhos> Create(Observer* observer);

 protected:
  // The device discovery can really be started when Bluetooth is powered on.
  // The method TryStartDiscovery() is called when it's a good time to try to
  // start the BLE device discovery. It will check if the discovery session has
  // been started and if the Bluetooth is powered and then really start the
  // CoreBluetooth BLE device discovery.
  virtual void TryStartDiscovery();

 private:
  explicit BluetoothLowEnergyDiscoveryManagerOhos(Observer* observer);

  void ProcessScanResult(const ohos::adapter::device::ScanResult &device);

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
