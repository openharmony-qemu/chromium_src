// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/bluetooth/ohos/bluetooth_low_energy_adapter_ohos.h"

#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "device/bluetooth/bluetooth_discovery_session.h"
#include "device/bluetooth/bluetooth_discovery_session_outcome.h"
#include "device/bluetooth/bluetooth_export.h"
#include "device/bluetooth/ohos/bluetooth_adapter_types.h"
#include "device/bluetooth/public/cpp/bluetooth_uuid.h"

using namespace ohos::adapter::device;

namespace {
const uint8_t kManufacturerLen = 2;
}
namespace device {

BluetoothLowEnergyAdapterOhos::BluetoothLowEnergyAdapterOhos()
    : low_energy_discovery_manager_(
          BluetoothLowEnergyDiscoveryManagerOhos::Create(this)),
      low_energy_advertisement_manager_(
          std::make_unique<BluetoothLowEnergyAdvertisementManagerOhos>()) {}

BluetoothLowEnergyAdapterOhos::~BluetoothLowEnergyAdapterOhos() {
  StopScanLowEnergy();
  devices_.clear();
}

void BluetoothLowEnergyAdapterOhos::StartScanLowEnergy() {
  devices_.clear();
  low_energy_discovery_manager_->StartDiscovery(BluetoothDevice::UUIDList());
}

void BluetoothLowEnergyAdapterOhos::StopScanLowEnergy() {
  low_energy_discovery_manager_->StopDiscovery();
  for (const auto& device_id_object_pair : devices_) {
    device_id_object_pair.second->ClearAdvertisementData();
  }
}

std::string BluetoothLowEnergyAdapterOhos::GetAddress() const {
  NOTIMPLEMENTED();
  return std::string();
}

std::string BluetoothLowEnergyAdapterOhos::GetName() const {
  NOTIMPLEMENTED();
  return std::string();
}

void BluetoothLowEnergyAdapterOhos::SetName(const std::string& name,
                                            base::OnceClosure callback,
                                            ErrorCallback error_callback) {
  NOTIMPLEMENTED();
}

bool BluetoothLowEnergyAdapterOhos::IsInitialized() const {
  NOTIMPLEMENTED();
  return true;
}

bool BluetoothLowEnergyAdapterOhos::IsPresent() const {
  NOTIMPLEMENTED();
  return true;
}

bool BluetoothLowEnergyAdapterOhos::IsPowered() const {
  return true;
}

bool BluetoothLowEnergyAdapterOhos::IsDiscoverable() const {
  return false;
}

void BluetoothLowEnergyAdapterOhos::SetDiscoverable(
    bool discoverable,
    base::OnceClosure callback,
    ErrorCallback error_callback) {
  NOTIMPLEMENTED();
}

bool BluetoothLowEnergyAdapterOhos::IsDiscovering() const {
  return low_energy_discovery_manager_->IsDiscovering();
}

std::unordered_map<BluetoothDevice*, BluetoothDevice::UUIDSet>
BluetoothLowEnergyAdapterOhos::RetrieveGattConnectedDevicesWithDiscoveryFilter(
    const BluetoothDiscoveryFilter& discovery_filter) {
  std::unordered_map<BluetoothDevice*, BluetoothDevice::UUIDSet>
      connected_devices;
  return connected_devices;
}

void BluetoothLowEnergyAdapterOhos::RegisterAdvertisement(
    std::unique_ptr<BluetoothAdvertisement::Data> advertisement_data,
    CreateAdvertisementCallback callback,
    AdvertisementErrorCallback error_callback) {
  LazyInitialize();
  low_energy_advertisement_manager_->RegisterAdvertisement(
      std::move(advertisement_data), std::move(callback),
      std::move(error_callback));
}

BluetoothLowEnergyDeviceOhos*
BluetoothLowEnergyAdapterOhos::GetBluetoothLowEnergyDeviceOhos(
    std::string device_address) {
  auto iter = devices_.find(device_address);
  if (iter == devices_.end()) {
    return nullptr;
  }

  BluetoothDeviceOhos* device_ohos =
      static_cast<BluetoothDeviceOhos*>(iter->second.get());
  return device_ohos->IsLowEnergyDevice()
             ? static_cast<BluetoothLowEnergyDeviceOhos*>(device_ohos)
             : nullptr;
}

void BluetoothLowEnergyAdapterOhos::LowEnergyDeviceUpdated(
    const ScanResult& result) {
  BluetoothLowEnergyDeviceOhos* device_ohos =
      GetBluetoothLowEnergyDeviceOhos(result.device_id);
  const bool is_new_device = device_ohos == nullptr;
  if (is_new_device) {
    // A new device has been found.
    device_ohos = new BluetoothLowEnergyDeviceOhos(this, result);
  } else if (DoesCollideWithKnownDevice(result, device_ohos)) {
    return;
  }
  DCHECK(device_ohos);

  // Get Advertised UUIDs
  BluetoothDevice::UUIDList advertised_uuids;

  // Get Service Data.
  BluetoothDevice::ServiceDataMap service_data_map;

  BluetoothDevice::ManufacturerDataMap manufacturer_data_map;

  const uint8_t* bytes = result.data;
  if (result.data_length > 1) {
    const uint16_t manufacturer_id = bytes[0] | (bytes[1] << 8);
    manufacturer_data_map.emplace(
        manufacturer_id, std::vector<uint8_t>(bytes + kManufacturerLen,
                                              bytes + result.data_length));
  }

  device_ohos->UpdateAdvertisementData(
      BluetoothDevice::ClampPower(result.rssi), std::nullopt /* flags */,
      std::move(advertised_uuids), std::nullopt, std::move(service_data_map),
      std::move(manufacturer_data_map));

  if (is_new_device) {
    std::string device_address = result.device_id;
    devices_[device_address] = base::WrapUnique(device_ohos);
    for (auto& observer : observers_) {
      observer.DeviceAdded(this, device_ohos);
    }
  } else {
    for (auto& observer : observers_) {
      observer.DeviceChanged(this, device_ohos);
    }
  }
}

bool BluetoothLowEnergyAdapterOhos::DoesCollideWithKnownDevice(
    ScanResult result,
    BluetoothLowEnergyDeviceOhos* device_ohos) {
  std::string stored_device_id = device_ohos->GetIdentifier();
  std::string updated_device_id = result.device_name;
  if (stored_device_id != updated_device_id) {
    DVLOG(1) << "LowEnergyDeviceUpdated stored_device_id != updated_device_id: "
             << "  " << stored_device_id << "  " << updated_device_id;
    return true;
  }
  return false;
}

void BluetoothLowEnergyAdapterOhos::StartScanWithFilter(
    std::unique_ptr<BluetoothDiscoveryFilter> discovery_filter,
    DiscoverySessionResultCallback callback) {
  NOTIMPLEMENTED();
}

void BluetoothLowEnergyAdapterOhos::StopScan(
    DiscoverySessionResultCallback callback) {
  StopScanLowEnergy();

  DVLOG(1) << "Discovery stopped";
  std::move(callback).Run(false, UMABluetoothDiscoverySessionOutcome::SUCCESS);
}

void BluetoothLowEnergyAdapterOhos::UpdateFilter(
    std::unique_ptr<BluetoothDiscoveryFilter> discovery_filter,
    DiscoverySessionResultCallback callback) {
  StartScanWithFilter(std::move(discovery_filter), std::move(callback));
}

void BluetoothLowEnergyAdapterOhos::CreateGattConnection(
    BluetoothLowEnergyDeviceOhos* device_ohos) {
  std::function<void(uint8_t)> on_connected_state_monitor =
      [device_ohos](uint8_t gatt_connected) {
        device_ohos->connected_state_ = gatt_connected;
        device_ohos->StartGattDiscovery();
      };
  BluetoothLowEnergyAdapter::CreateGattClientDevice(device_ohos->device_id_,
                                                    on_connected_state_monitor);
}

void BluetoothLowEnergyAdapterOhos::DisconnectGatt(
    BluetoothLowEnergyDeviceOhos* device_ohos) {
  BluetoothLowEnergyAdapter::DisConnectGatt(device_ohos->device_id_);
}

void BluetoothLowEnergyAdapterOhos::UpdateKnownLowEnergyDevices(
    DevicesInfo updated_low_energy_devices_info) {
  DevicesInfo changed_devices;
  // Notify DeviceChanged() to devices that have been newly paired as well as to
  // devices that have been removed from the pairing list.
  std::set_symmetric_difference(
      updated_low_energy_devices_info.begin(),
      updated_low_energy_devices_info.end(), low_energy_devices_info_.begin(),
      low_energy_devices_info_.end(),
      std::inserter(changed_devices, changed_devices.end()));

  low_energy_devices_info_ = std::move(updated_low_energy_devices_info);
  for (const auto& info : changed_devices) {
    auto it = devices_.find(info.first);
    if (it == devices_.end()) {
      continue;
    }
    NotifyDeviceChanged(it->second.get());
  }
}

void BluetoothLowEnergyAdapterOhos::LazyInitialize() {
  if (lazy_initialized_) {
    return;
  }
  lazy_initialized_ = true;
  low_energy_advertisement_manager_->Init(ui_task_runner_);
}

}  // namespace device
