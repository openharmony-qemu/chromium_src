// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/bluetooth/ohos/bluetooth_low_energy_device_ohos.h"

#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "base/ranges/algorithm.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/sys_string_conversions.h"
#include "device/bluetooth/bluetooth_device.h"
#include "device/bluetooth/ohos/bluetooth_adapter_types.h"
#include "device/bluetooth/ohos/bluetooth_low_energy_adapter_ohos.h"
#include "device/bluetooth/ohos/bluetooth_remote_gatt_service_ohos.h"
#include "device/bluetooth/public/cpp/bluetooth_address.h"
#include "ohos/adapter/bluetooth/bluetooth_adapter.h"
#include "ohos/adapter/bluetooth/bluetooth_ble_attribute_adapter.h"

using namespace ohos::adapter::device;

namespace device {

BluetoothLowEnergyDeviceOhos::BluetoothLowEnergyDeviceOhos(
    BluetoothAdapter* adapter,
    const ScanResult& result)
    : BluetoothDeviceOhos(adapter),
      connected_state_(0),
      discovery_pending_count_(0) {
  UpdateTimestamp();
  device_name_ = result.device_name;
  device_id_ = result.device_id;
  connectable_ = result.connectable;
  rssi_ = result.rssi;
  task_runner_ = base::TaskRunnerOHOS::GetUIThreadTaskRunner();
}

BluetoothLowEnergyDeviceOhos::~BluetoothLowEnergyDeviceOhos() {}

std::string BluetoothLowEnergyDeviceOhos::GetIdentifier() const {
  return device_name_;
}

uint32_t BluetoothLowEnergyDeviceOhos::GetBluetoothClass() const {
  return BluetoothOhos::GetRemoteDeviceClass(device_id_);
}

std::string BluetoothLowEnergyDeviceOhos::GetAddress() const {
  return device_id_;
}

BluetoothDevice::AddressType BluetoothLowEnergyDeviceOhos::GetAddressType()
    const {
  return ADDR_TYPE_UNKNOWN;
}

BluetoothDevice::VendorIDSource
BluetoothLowEnergyDeviceOhos::GetVendorIDSource() const {
  return VENDOR_ID_UNKNOWN;
}

uint16_t BluetoothLowEnergyDeviceOhos::GetVendorID() const {
  NOTIMPLEMENTED();
  return 0;
}

uint16_t BluetoothLowEnergyDeviceOhos::GetProductID() const {
  NOTIMPLEMENTED();
  return 0;
}

uint16_t BluetoothLowEnergyDeviceOhos::GetDeviceID() const {
  NOTIMPLEMENTED();
  return 0;
}

uint16_t BluetoothLowEnergyDeviceOhos::GetAppearance() const {
  NOTIMPLEMENTED();
  return 0;
}

std::optional<std::string> BluetoothLowEnergyDeviceOhos::GetName() const {
  if (!device_name_.empty()) {
    return device_name_;
  }
  return std::nullopt;
}

bool BluetoothLowEnergyDeviceOhos::IsPaired() const {
  NOTIMPLEMENTED();
  return false;
}

bool BluetoothLowEnergyDeviceOhos::IsConnected() const {
  return IsGattConnected();
}

bool BluetoothLowEnergyDeviceOhos::IsGattConnected() const {
  return connected_state_ == STATE_CONNECTED;
  ;
}

bool BluetoothLowEnergyDeviceOhos::IsConnectable() const {
  return connectable_;
}

bool BluetoothLowEnergyDeviceOhos::IsConnecting() const {
  return connected_state_ == STATE_CONNECTING;
}

bool BluetoothLowEnergyDeviceOhos::ExpectingPinCode() const {
  NOTIMPLEMENTED();
  return false;
}

bool BluetoothLowEnergyDeviceOhos::ExpectingPasskey() const {
  NOTIMPLEMENTED();
  return false;
}

bool BluetoothLowEnergyDeviceOhos::ExpectingConfirmation() const {
  NOTIMPLEMENTED();
  return false;
}

void BluetoothLowEnergyDeviceOhos::GetConnectionInfo(
    ConnectionInfoCallback callback) {
  NOTIMPLEMENTED();
}

void BluetoothLowEnergyDeviceOhos::SetConnectionLatency(
    ConnectionLatency connection_latency,
    base::OnceClosure callback,
    ErrorCallback error_callback) {
  NOTIMPLEMENTED();
}

void BluetoothLowEnergyDeviceOhos::Connect(PairingDelegate* pairing_delegate,
                                           ConnectCallback callback) {
  NOTIMPLEMENTED();
}

void BluetoothLowEnergyDeviceOhos::SetPinCode(const std::string& pincode) {
  NOTIMPLEMENTED();
}

void BluetoothLowEnergyDeviceOhos::SetPasskey(uint32_t passkey) {
  NOTIMPLEMENTED();
}

void BluetoothLowEnergyDeviceOhos::ConfirmPairing() {
  NOTIMPLEMENTED();
}

void BluetoothLowEnergyDeviceOhos::RejectPairing() {
  NOTIMPLEMENTED();
}

void BluetoothLowEnergyDeviceOhos::CancelPairing() {
  NOTIMPLEMENTED();
}

void BluetoothLowEnergyDeviceOhos::Disconnect(base::OnceClosure callback,
                                              ErrorCallback error_callback) {
  NOTIMPLEMENTED();
}

void BluetoothLowEnergyDeviceOhos::Forget(base::OnceClosure callback,
                                          ErrorCallback error_callback) {
  NOTIMPLEMENTED();
}

void BluetoothLowEnergyDeviceOhos::ConnectToService(
    const BluetoothUUID& uuid,
    ConnectToServiceCallback callback,
    ConnectToServiceErrorCallback error_callback) {
  NOTIMPLEMENTED();
}

void BluetoothLowEnergyDeviceOhos::ConnectToServiceInsecurely(
    const BluetoothUUID& uuid,
    ConnectToServiceCallback callback,
    ConnectToServiceErrorCallback error_callback) {
  NOTIMPLEMENTED();
}

bool BluetoothLowEnergyDeviceOhos::IsLowEnergyDevice() {
  return true;
}

void BluetoothLowEnergyDeviceOhos::SetConnectedState(uint8_t connected_state) {
  connected_state_ = connected_state;
}

void BluetoothLowEnergyDeviceOhos::CreateGattConnectionImpl(
    std::optional<BluetoothUUID> service_uuid) {
  if (!IsGattConnected()) {
    GetLowEnergyAdapter()->CreateGattConnection(this);
  }
}

void BluetoothLowEnergyDeviceOhos::DisconnectGatt() {
  GetLowEnergyAdapter()->DisconnectGatt(this);
}

BluetoothLowEnergyAdapterOhos*
BluetoothLowEnergyDeviceOhos::GetLowEnergyAdapter() {
  return static_cast<BluetoothLowEnergyAdapterOhos*>(this->GetAdapter());
}

BluetoothLowEnergyAdapterOhos*
BluetoothLowEnergyDeviceOhos::GetLowEnergyAdapter() const {
  return static_cast<BluetoothLowEnergyAdapterOhos*>(this->GetAdapter());
}

void BluetoothLowEnergyDeviceOhos::ProcessDiscoveryResult(
    const std::vector<ohos::adapter::device::ServiceInfo>& service) {
  GattServiceMap gatt_services;

  for (const auto& info : service) {
    auto gatt_service_ohos = BluetoothRemoteGattServiceOHOS::Create(
        this, info.device_id, BluetoothUUID(info.service_uuid), info.is_primary,
        info.identifier);
    if (!gatt_service_ohos) {
      continue;
    }
    GetLowEnergyAdapter()->NotifyGattDiscoveryComplete(gatt_service_ohos.get());
    std::string identifier = gatt_service_ohos->GetIdentifier();
    auto iter = gatt_services_.find(identifier);
    if (iter != gatt_services_.end()) {
      iter = gatt_services.emplace(std::move(*iter)).first;
    } else {
      iter = gatt_services
                 .emplace(std::move(identifier), std::move(gatt_service_ohos))
                 .first;
    }
  }
  std::swap(gatt_services, gatt_services_);
  device_uuids_.ReplaceServiceUUIDs(gatt_services_);
  SetGattServicesDiscoveryComplete(true);
  GetLowEnergyAdapter()->NotifyGattServicesDiscovered(this);
  GetLowEnergyAdapter()->NotifyDeviceChanged(this);
}

void BluetoothLowEnergyDeviceOhos::StartGattDiscovery() {
  if (!IsGattConnected()) {
    return;
  }
  if (!create_gatt_connection_callbacks_.empty()) {
    task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&BluetoothLowEnergyDeviceOhos::DidConnectGatt,
                       weak_ptr_factory_.GetWeakPtr(), std::nullopt));
  }
  auto service_callback = [this](const std::vector<ServiceInfo>& service) {
    if (task_runner_) {
      task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(&BluetoothLowEnergyDeviceOhos::ProcessDiscoveryResult,
                         weak_ptr_factory_.GetWeakPtr(), std::move(service)));
    }
  };
  BluetoothBleAttributeAdapter::StartGattDiscovery(device_id_,
                                                   std::move(service_callback));
}
}  // namespace device
