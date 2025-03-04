// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_BLUETOOTH_OHOS_BLUETOOTH_LOW_ENERGY_DEVICE_OHOS_H_
#define DEVICE_BLUETOOTH_OHOS_BLUETOOTH_LOW_ENERGY_DEVICE_OHOS_H_

#include <stdint.h>

#include <optional>
#include <set>

#include "build/build_config.h"
#include "device/bluetooth/ohos/bluetooth_device_ohos.h"
#include "ohos/adapter/bluetooth/bluetooth_ble_attribute_adapter.h"
#include "ohos/adapter/bluetooth/bluetooth_low_energy_adapter.h"

namespace device {

class BluetoothLowEnergyAdapterOhos;
class DEVICE_BLUETOOTH_EXPORT BluetoothLowEnergyDeviceOhos
    : public BluetoothDeviceOhos {
 public:
  BluetoothLowEnergyDeviceOhos(BluetoothAdapter* adapter,
                               const ohos::adapter::device::ScanResult& result);

  BluetoothLowEnergyDeviceOhos(const BluetoothLowEnergyDeviceOhos&) = delete;
  BluetoothLowEnergyDeviceOhos& operator=(const BluetoothLowEnergyDeviceOhos&) =
      delete;

  ~BluetoothLowEnergyDeviceOhos() override;

  // BluetoothDevice overrides.
  std::string GetIdentifier() const override;
  uint32_t GetBluetoothClass() const override;
  std::string GetAddress() const override;
  AddressType GetAddressType() const override;
  BluetoothDevice::VendorIDSource GetVendorIDSource() const override;
  uint16_t GetVendorID() const override;
  uint16_t GetProductID() const override;
  uint16_t GetDeviceID() const override;
  uint16_t GetAppearance() const override;
  std::optional<std::string> GetName() const override;
  bool IsPaired() const override;
  bool IsConnected() const override;
  bool IsGattConnected() const override;
  bool IsConnectable() const override;
  bool IsConnecting() const override;
  bool ExpectingPinCode() const override;
  bool ExpectingPasskey() const override;
  bool ExpectingConfirmation() const override;
  void GetConnectionInfo(ConnectionInfoCallback callback) override;
  void SetConnectionLatency(ConnectionLatency connection_latency,
                            base::OnceClosure callback,
                            ErrorCallback error_callback) override;
  void Connect(PairingDelegate* pairing_delegate,
               ConnectCallback callback) override;
  void SetPinCode(const std::string& pincode) override;
  void SetPasskey(uint32_t passkey) override;
  void ConfirmPairing() override;
  void RejectPairing() override;
  void CancelPairing() override;
  void Disconnect(base::OnceClosure callback,
                  ErrorCallback error_callback) override;
  void Forget(base::OnceClosure callback,
              ErrorCallback error_callback) override;
  void ConnectToService(const BluetoothUUID& uuid,
                        ConnectToServiceCallback callback,
                        ConnectToServiceErrorCallback error_callback) override;
  void ConnectToServiceInsecurely(
      const device::BluetoothUUID& uuid,
      ConnectToServiceCallback callback,
      ConnectToServiceErrorCallback error_callback) override;
  bool IsLowEnergyDevice() override;
  void SetConnectedState(uint8_t connected_state);
  void StartGattDiscovery();

 protected:
  // BluetoothDevice override.
  void CreateGattConnectionImpl(
      std::optional<BluetoothUUID> service_uuid) override;
  void DisconnectGatt() override;

 private:
  friend class BluetoothLowEnergyAdapterOhos;

  // Returns the Bluetooth adapter.
  BluetoothLowEnergyAdapterOhos* GetLowEnergyAdapter();
  BluetoothLowEnergyAdapterOhos* GetLowEnergyAdapter() const;

  void ProcessDiscoveryResult(
      const std::vector<ohos::adapter::device::ServiceInfo>& service);

  // Whether the device is connected.
  uint8_t connected_state_;

  int discovery_pending_count_;

  std::string device_id_;

  std::string device_name_;

  int8_t rssi_;

  bool connectable_;
  scoped_refptr<base::SequencedTaskRunner> task_runner_{nullptr};
  base::WeakPtrFactory<BluetoothLowEnergyDeviceOhos> weak_ptr_factory_{this};
};

}  // namespace device

#endif  // DEVICE_BLUETOOTH—_OHOS_BLUETOOTH_LOW_ENERGY_DEVICE_OHOS_H_
