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
