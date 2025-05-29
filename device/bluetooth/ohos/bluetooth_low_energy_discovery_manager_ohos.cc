// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/bluetooth/ohos/bluetooth_low_energy_discovery_manager_ohos.h"

#include "base/logging.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "device/bluetooth/ohos/bluetooth_adapter_ohos.h"
#include "device/bluetooth/ohos/bluetooth_adapter_types.h"
#include "device/bluetooth/ohos/bluetooth_low_energy_device_ohos.h"

using namespace ohos::adapter::device;

namespace device {

BluetoothLowEnergyDiscoveryManagerOhos::
    ~BluetoothLowEnergyDiscoveryManagerOhos() {}

bool BluetoothLowEnergyDiscoveryManagerOhos::IsDiscovering() const {
  return discovering_;
}

void BluetoothLowEnergyDiscoveryManagerOhos::StartDiscovery(
    BluetoothDevice::UUIDList services_uuids) {
  discovering_ = true;
  pending_ = true;
  services_uuids_ = std::move(services_uuids);
  TryStartDiscovery();
}

void BluetoothLowEnergyDiscoveryManagerOhos::ProcessScanResult(
    const ohos::adapter::device::ScanResult& device) {
  if (observer_) {
    observer_->LowEnergyDeviceUpdated(device);
  }
}

void BluetoothLowEnergyDiscoveryManagerOhos::TryStartDiscovery() {
  if (!discovering_) {
    DVLOG(1) << "TryStartDiscovery !discovering_";
    return;
  }

  if (!pending_) {
    DVLOG(1) << "TryStartDiscovery !pending_";
    return;
  }

  BluetoothLowEnergyAdapter::StartDiscoveryMonitor([&](ScanResult device) {
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(
            &BluetoothLowEnergyDiscoveryManagerOhos::ProcessScanResult,
            weak_ptr_factory_.GetWeakPtr(), std::move(device)));
  });

  BluetoothLowEnergyAdapter::StartDiscovery();

  pending_ = false;
}

void BluetoothLowEnergyDiscoveryManagerOhos::StopDiscovery() {
  discovering_ = false;
  BluetoothLowEnergyAdapter::StopDiscovery();
  BluetoothLowEnergyAdapter::StopDiscoveryMonitor();
}

std::unique_ptr<BluetoothLowEnergyDiscoveryManagerOhos>
BluetoothLowEnergyDiscoveryManagerOhos::Create(Observer* observer) {
  return std::unique_ptr<BluetoothLowEnergyDiscoveryManagerOhos>(
      new BluetoothLowEnergyDiscoveryManagerOhos(observer));
}

BluetoothLowEnergyDiscoveryManagerOhos::BluetoothLowEnergyDiscoveryManagerOhos(
    Observer* observer)
    : observer_(observer) {
  discovering_ = false;
}

}  // namespace device
