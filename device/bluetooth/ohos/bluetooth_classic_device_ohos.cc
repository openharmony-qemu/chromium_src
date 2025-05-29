// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/bluetooth/ohos/bluetooth_classic_device_ohos.h"

#include <string>

#include "base/functional/bind.h"
#include "base/hash/hash.h"
#include "base/logging.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/sys_string_conversions.h"
#include "base/time/time.h"
#include "device/bluetooth/ohos/bluetooth_adapter_ohos.h"
#include "device/bluetooth/public/cpp/bluetooth_address.h"
#include "device/bluetooth/public/cpp/bluetooth_uuid.h"
#include "ohos/adapter/bluetooth/bluetooth_adapter.h"

using namespace ohos::adapter::device;

namespace device {

namespace {
const char kApiUnavailable[] = "This API is not implemented on this platform.";
}

BluetoothClassicDeviceOhos::BluetoothClassicDeviceOhos(
    BluetoothAdapterOhos* adapter,
    const std::string& device_id,
    const std::string& device_name)
    : BluetoothDeviceOhos(adapter),
      device_id_(device_id),
      device_name_(device_name) {
  UpdateTimestamp();
}

BluetoothClassicDeviceOhos::~BluetoothClassicDeviceOhos() {}

uint32_t BluetoothClassicDeviceOhos::GetBluetoothClass() const {
  return BluetoothOhos::GetRemoteDeviceClass(device_id_);
}

void BluetoothClassicDeviceOhos::CreateGattConnectionImpl(
    std::optional<BluetoothUUID> service_uuid) {
  // Classic devices do not support GATT connection.
  DidConnectGatt(ERROR_UNSUPPORTED_DEVICE);
}

void BluetoothClassicDeviceOhos::DisconnectGatt() {}

std::string BluetoothClassicDeviceOhos::GetAddress() const {
  return device_id_;
}

BluetoothDevice::AddressType BluetoothClassicDeviceOhos::GetAddressType()
    const {
  NOTIMPLEMENTED();
  return ADDR_TYPE_UNKNOWN;
}

BluetoothDevice::VendorIDSource BluetoothClassicDeviceOhos::GetVendorIDSource()
    const {
  NOTIMPLEMENTED();
  return VENDOR_ID_UNKNOWN;
}

uint16_t BluetoothClassicDeviceOhos::GetVendorID() const {
  NOTIMPLEMENTED();
  return 0;
}

uint16_t BluetoothClassicDeviceOhos::GetProductID() const {
  NOTIMPLEMENTED();
  return 0;
}

uint16_t BluetoothClassicDeviceOhos::GetDeviceID() const {
  NOTIMPLEMENTED();
  return 0;
}

uint16_t BluetoothClassicDeviceOhos::GetAppearance() const {
  NOTIMPLEMENTED();
  return 0;
}

std::optional<std::string> BluetoothClassicDeviceOhos::GetName() const {
  if (device_id_.empty()) {
    return std::nullopt;
  }
  return device_name_;
}

bool BluetoothClassicDeviceOhos::IsPaired() const {
  BondState state = BluetoothOhos::GetPairedState(device_id_);
  if (state == BOND_STATE_BONDED) {
    return true;
  }
  return false;
}

bool BluetoothClassicDeviceOhos::IsConnected() const {
  NOTIMPLEMENTED();
  return false;
}

bool BluetoothClassicDeviceOhos::IsGattConnected() const {
  return false;  // Classic devices do not support GATT connection.
}

bool BluetoothClassicDeviceOhos::IsConnectable() const {
  NOTIMPLEMENTED();
  return false;
}

bool BluetoothClassicDeviceOhos::IsConnecting() const {
  NOTIMPLEMENTED();
  return false;
}

BluetoothDevice::UUIDSet BluetoothClassicDeviceOhos::GetUUIDs() const {
  return device_uuids_.GetUUIDs();
}

std::optional<int8_t> BluetoothClassicDeviceOhos::GetInquiryRSSI() const {
  return std::nullopt;
}

std::optional<int8_t> BluetoothClassicDeviceOhos::GetInquiryTxPower() const {
  NOTIMPLEMENTED();
  return std::nullopt;
}

bool BluetoothClassicDeviceOhos::ExpectingPinCode() const {
  NOTIMPLEMENTED();
  return false;
}

bool BluetoothClassicDeviceOhos::ExpectingPasskey() const {
  NOTIMPLEMENTED();
  return false;
}

bool BluetoothClassicDeviceOhos::ExpectingConfirmation() const {
  NOTIMPLEMENTED();
  return false;
}

void BluetoothClassicDeviceOhos::GetConnectionInfo(
    ConnectionInfoCallback callback) {
  ConnectionInfo connection_info;
  NOTIMPLEMENTED();
  std::move(callback).Run(connection_info);
}

void BluetoothClassicDeviceOhos::SetConnectionLatency(
    ConnectionLatency connection_latency,
    base::OnceClosure callback,
    ErrorCallback error_callback) {
  NOTIMPLEMENTED();
}

void BluetoothClassicDeviceOhos::Connect(PairingDelegate* pairing_delegate,
                                         ConnectCallback callback) {
  NOTIMPLEMENTED();
}

void BluetoothClassicDeviceOhos::SetPinCode(const std::string& pincode) {
  BluetoothOhos::SetDevicePinCode(device_id_, pincode, [](bool is_succeed) {
    if (!is_succeed) {
      LOG(ERROR) << "set device pin code failed.";
    }
  });
}

void BluetoothClassicDeviceOhos::SetPasskey(uint32_t passkey) {
  NOTIMPLEMENTED();
}

void BluetoothClassicDeviceOhos::ConfirmPairing() {
  NOTIMPLEMENTED();
}

void BluetoothClassicDeviceOhos::RejectPairing() {
  NOTIMPLEMENTED();
}

void BluetoothClassicDeviceOhos::CancelPairing() {
  NOTIMPLEMENTED();
}

void BluetoothClassicDeviceOhos::Pair(
    device::BluetoothDevice::PairingDelegate* pairing_delegate,
    ConnectCallback callback) {
  BluetoothOhos::PairDevice(device_id_, [this, &callback](bool is_paired) {
    if (is_paired) {
      base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
          FROM_HERE, base::BindOnce(std::move(callback), std::nullopt));
    } else {
      LOG(WARNING) << this->device_id_ << ": Paired Failed.";
      base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
          FROM_HERE,
          base::BindOnce(std::move(callback), BluetoothDevice::ERROR_FAILED));
    }
  });
}

void BluetoothClassicDeviceOhos::Disconnect(base::OnceClosure callback,
                                            ErrorCallback error_callback) {
  NOTIMPLEMENTED();
}

void BluetoothClassicDeviceOhos::Forget(base::OnceClosure callback,
                                        ErrorCallback error_callback) {
  NOTIMPLEMENTED();
}

void BluetoothClassicDeviceOhos::ConnectToService(
    const BluetoothUUID& uuid,
    ConnectToServiceCallback callback,
    ConnectToServiceErrorCallback error_callback) {
  NOTIMPLEMENTED();
  std::move(error_callback).Run(kApiUnavailable);
}

void BluetoothClassicDeviceOhos::ConnectToServiceInsecurely(
    const BluetoothUUID& uuid,
    ConnectToServiceCallback callback,
    ConnectToServiceErrorCallback error_callback) {
  NOTIMPLEMENTED();
  std::move(error_callback).Run(kApiUnavailable);
}

base::Time BluetoothClassicDeviceOhos::GetLastUpdateTime() const {
  // getLastInquiryUpdate returns nil unpredictably so just use the
  // cross platform implementation of last update time.
  return last_update_time_;
}

bool BluetoothClassicDeviceOhos::IsLowEnergyDevice() {
  return false;
}
}  // namespace device
