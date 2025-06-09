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

#ifndef DEVICE_BLUETOOTH_BLUETOOTH_CLASSIC_DEVICE_OHOS_H_
#define DEVICE_BLUETOOTH_BLUETOOTH_CLASSIC_DEVICE_OHOS_H_

#include <optional>

#include "device/bluetooth/ohos/bluetooth_device_ohos.h"

namespace device {

class BluetoothAdapterOhos;
class BluetoothUUID;
class BluetoothClassicDeviceOhos : public BluetoothDeviceOhos {
 public:
  explicit BluetoothClassicDeviceOhos(
      BluetoothAdapterOhos* adapter,
      const std::string& device_id,
      const std::string& device_name);

  BluetoothClassicDeviceOhos(const BluetoothClassicDeviceOhos&) = delete;
  BluetoothClassicDeviceOhos& operator=(const BluetoothClassicDeviceOhos&) =
      delete;

  ~BluetoothClassicDeviceOhos() override;

  // BluetoothDevice override
  uint32_t GetBluetoothClass() const override;
  std::string GetAddress() const override;
  AddressType GetAddressType() const override;
  VendorIDSource GetVendorIDSource() const override;
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
  UUIDSet GetUUIDs() const override;
  std::optional<int8_t> GetInquiryRSSI() const override;
  std::optional<int8_t> GetInquiryTxPower() const override;
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

  void Pair(device::BluetoothDevice::PairingDelegate* pairing_delegate,
            ConnectCallback callback) override;
  void Disconnect(base::OnceClosure callback,
                  ErrorCallback error_callback) override;
  void Forget(base::OnceClosure callback,
              ErrorCallback error_callback) override;
  void ConnectToService(const BluetoothUUID& uuid,
                        ConnectToServiceCallback callback,
                        ConnectToServiceErrorCallback error_callback) override;
  void ConnectToServiceInsecurely(
      const BluetoothUUID& uuid,
      ConnectToServiceCallback callback,
      ConnectToServiceErrorCallback error_callback) override;

  base::Time GetLastUpdateTime() const override;

  bool IsLowEnergyDevice() override;

 protected:
  // BluetoothDevice override
  void CreateGattConnectionImpl(
      std::optional<BluetoothUUID> service_uuid) override;
  void DisconnectGatt() override;

 private:
  friend class BluetoothAdapterOhos;
  std::string device_id_;
  std::string device_name_;
};
}  // namespace device

#endif  // DEVICE_BLUETOOTH_BLUETOOTH_CLASSIC_DEVICE_OHOS_H_
