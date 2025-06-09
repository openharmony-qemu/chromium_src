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

#include "device/bluetooth/ohos/bluetooth_adapter_ohos.h"

#include <functional>
#include <string>

#include "base/functional/callback_helpers.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "device/bluetooth/bluetooth_common.h"
#include "device/bluetooth/bluetooth_discovery_session.h"
#include "device/bluetooth/bluetooth_discovery_session_outcome.h"
#include "device/bluetooth/ohos/bluetooth_classic_device_ohos.h"
#include "net/base/completion_repeating_callback.h"
#include "ohos/adapter/bluetooth/bluetooth_adapter.h"

using namespace ohos::adapter::device;
namespace device {

// static
scoped_refptr<BluetoothAdapter> BluetoothAdapter::CreateAdapter() {
  return base::WrapRefCounted(new BluetoothAdapterOhos());
}

BluetoothAdapterOhos::BluetoothAdapterOhos()
    : classic_discovery_manager_(
          BluetoothDiscoveryManagerOhos::CreateClassic(this)) {
  DCHECK(classic_discovery_manager_);
  ui_task_runner_ = base::TaskRunnerOHOS::GetUIThreadTaskRunner();
  DCHECK(ui_task_runner_);
}

base::WeakPtr<BluetoothAdapter> BluetoothAdapterOhos::GetWeakPtr() {
  return weak_ptr_factory_.GetWeakPtr();
}

BluetoothAdapterOhos::~BluetoothAdapterOhos() {
  is_initialized_ = false;
  BluetoothOhos::StopBluetoothStateMonitor();
  if (classic_discovery_manager_ &&
      classic_discovery_manager_->IsDiscovering()) {
    classic_discovery_manager_->StopDiscovery();
  }
  devices_.clear();
}

void BluetoothAdapterOhos::Initialize(base::OnceClosure callback) {
  NotifyBluetoothChanged(BluetoothOhos::GetBluetoothState());
  // start monitor bluetooth state
  BluetoothOhos::StartBluetoothStateMonitor(
      std::bind(&BluetoothAdapterOhos::NotifyBluetoothChanged,
                weak_ptr_factory_.GetWeakPtr(), std::placeholders::_1));
  std::move(callback).Run();
  is_initialized_ = true;
}

std::string BluetoothAdapterOhos::GetAddress() const {
  // Does not support obtaining addresses on the ohos platform
  NOTIMPLEMENTED();
  return "";
}

std::string BluetoothAdapterOhos::GetName() const {
  return BluetoothOhos::GetAdapterName();
}

void BluetoothAdapterOhos::SetName(const std::string& name,
                                   base::OnceClosure callback,
                                   ErrorCallback error_callback) {
  if (!IsPresent()) {
    LOG(ERROR) << "SetName: " << name << ". Not Present!";
    std::move(error_callback).Run();
    return;
  }

  bool result = BluetoothOhos::SetAdapterName(name);
  if (!result) {
    LOG(ERROR) << "SetName: " << name << ". Failed!";
    std::move(error_callback).Run();
    return;
  }
  std::move(callback).Run();
}

bool BluetoothAdapterOhos::IsInitialized() const {
  return is_initialized_;
}

bool BluetoothAdapterOhos::IsPresent() const {
  return true;
}

bool BluetoothAdapterOhos::IsPowered() const {
  return is_powered_;
}

BluetoothAdapter::PermissionStatus BluetoothAdapterOhos::GetOsPermissionStatus()
    const {
  if (BluetoothOhos::IsOsGranted()) {
    return PermissionStatus::kAllowed;
  }
  return PermissionStatus::kDenied;
}

bool BluetoothAdapterOhos::IsDiscoverable() const {
  int32_t mode = BluetoothOhos::GetBluetoothScanMode();
  if (mode == SCAN_MODE_GENERAL_DISCOVERABLE ||
      mode == SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE) {
    return true;
  }
  return false;
}

void BluetoothAdapterOhos::SetDiscoverable(bool discoverable,
                                           base::OnceClosure callback,
                                           ErrorCallback error_callback) {
  const int32_t duration = 100;
  if (discoverable) {
    BluetoothOhos::SetBluetoothScanMode(
        SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE, duration);
  } else {
    BluetoothOhos::SetBluetoothScanMode(SCAN_MODE_LIMITED_DISCOVERABLE,
                                        duration);
  }
}

bool BluetoothAdapterOhos::IsDiscovering() const {
  return BluetoothOhos::IsDiscovering();
}

BluetoothAdapter::UUIDList BluetoothAdapterOhos::GetUUIDs() const {
  NOTIMPLEMENTED();
  return UUIDList();
}

void BluetoothAdapterOhos::CreateRfcommService(
    const BluetoothUUID& uuid,
    const ServiceOptions& options,
    CreateServiceCallback callback,
    CreateServiceErrorCallback error_callback) {
  NOTIMPLEMENTED();
  std::move(error_callback).Run("Not Implemented");
}

void BluetoothAdapterOhos::CreateL2capService(
    const BluetoothUUID& uuid,
    const ServiceOptions& options,
    CreateServiceCallback callback,
    CreateServiceErrorCallback error_callback) {
  NOTIMPLEMENTED();
  std::move(error_callback).Run("Not Implemented");
}

void BluetoothAdapterOhos::RegisterAdvertisement(
    std::unique_ptr<BluetoothAdvertisement::Data> advertisement_data,
    CreateAdvertisementCallback callback,
    AdvertisementErrorCallback error_callback) {
  NOTIMPLEMENTED();
  std::move(error_callback)
      .Run(BluetoothAdvertisement::ERROR_UNSUPPORTED_PLATFORM);
}

BluetoothLocalGattService* BluetoothAdapterOhos::GetGattService(
    const std::string& identifier) const {
  NOTIMPLEMENTED();
  return nullptr;
}

void BluetoothAdapterOhos::ClassicDeviceFound(const std::string& address) {
  ClassicDeviceAdded(address);
}

void BluetoothAdapterOhos::ClassicDiscoveryStopped(bool unexpected) {
  if (unexpected) {
    DVLOG(1) << "Discovery stopped unexpectedly";
    MarkDiscoverySessionsAsInactive();
  }
  for (auto& observer : observers_) {
    observer.AdapterDiscoveringChanged(this, false);
  }
}

void BluetoothAdapterOhos::ClassicDeviceAdded(const std::string& address) {
  std::string device_address = address;

  BluetoothDevice* device_classic = GetDevice(device_address);

  // Only notify observers once per device.
  if (device_classic != nullptr) {
    device_classic->UpdateTimestamp();
    return;
  }

  std::string device_name = BluetoothOhos::GetRemoteDeviceName(device_address);
  device_classic =
      new BluetoothClassicDeviceOhos(this, device_address, device_name);
  devices_[device_address] = base::WrapUnique(device_classic);

  for (auto& observer : observers_) {
    if (!is_initialized_) {
      break;
    }
    observer.DeviceAdded(this, device_classic);
  }
}

void BluetoothAdapterOhos::OnBluetoothChanged(int32_t bluetooth_state) {
  if (bluetooth_state == STATE_ON || bluetooth_state == STATE_BLE_ON) {
    is_powered_ = true;
  } else if (bluetooth_state == STATE_OFF || bluetooth_state == STATE_ERROR) {
    is_powered_ = false;
  }
}

void BluetoothAdapterOhos::NotifyBluetoothChanged(int32_t bluetooth_state) {
  ui_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&BluetoothAdapterOhos::OnBluetoothChanged,
                     weak_ptr_factory_.GetWeakPtr(),
                     bluetooth_state));
}

bool BluetoothAdapterOhos::SetPoweredImpl(bool powered) {
  if (powered) {
    return BluetoothOhos::EnableBluetooth();
  }
  return BluetoothOhos::DisableBluetooth();
}

void BluetoothAdapterOhos::StartScanWithFilter(
    std::unique_ptr<BluetoothDiscoveryFilter> discovery_filter,
    DiscoverySessionResultCallback callback) {
  BluetoothTransport transport = BLUETOOTH_TRANSPORT_DUAL;
  if (discovery_filter) {
    transport = discovery_filter->GetTransport();
  }

  if ((transport & BLUETOOTH_TRANSPORT_CLASSIC) &&
      !classic_discovery_manager_->IsDiscovering()) {
    if (!classic_discovery_manager_->StartDiscovery()) {
      DVLOG(1) << "Failed to add a classic discovery session";
      ui_task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(std::move(callback), true,
                         UMABluetoothDiscoverySessionOutcome::UNKNOWN));
      return;
    }
  }

  // Add the BLE discovery
  if (transport & BLUETOOTH_TRANSPORT_LE) {
    StartScanLowEnergy();
  }

  for (auto& observer : observers_) {
    observer.AdapterDiscoveringChanged(this, true);
  }
  DCHECK(callback);
  ui_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(std::move(callback), false,
                                UMABluetoothDiscoverySessionOutcome::SUCCESS));
}

void BluetoothAdapterOhos::UpdateFilter(
    std::unique_ptr<BluetoothDiscoveryFilter> discovery_filter,
    DiscoverySessionResultCallback callback) {
  NOTIMPLEMENTED();
}

void BluetoothAdapterOhos::StopScan(DiscoverySessionResultCallback callback) {
  StopScanLowEnergy();
  if (classic_discovery_manager_ &&
      classic_discovery_manager_->IsDiscovering() &&
      !classic_discovery_manager_->StopDiscovery()) {
    DVLOG(1) << "Failed to stop classic discovery";
    ui_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(std::move(callback), true,
                       UMABluetoothDiscoverySessionOutcome::UNKNOWN));
    return;
  }

  DVLOG(1) << "Discovery stopped";
  ui_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(std::move(callback), false,
                     UMABluetoothDiscoverySessionOutcome::SUCCESS));
}

void BluetoothAdapterOhos::RemovePairingDelegateInternal(
    device::BluetoothDevice::PairingDelegate* pairing_delegate) {
  NOTIMPLEMENTED();
}

}  // namespace device
