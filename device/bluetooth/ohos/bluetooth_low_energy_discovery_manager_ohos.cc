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
    const ohos::adapter::device::ScanResult &device) {
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

  BluetoothLowEnergyAdapter::StartDiscoveryMonitor(
      [&](ScanResult device) {
        base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
            FROM_HERE,
            base::BindOnce(
                &BluetoothLowEnergyDiscoveryManagerOhos::ProcessScanResult,
                weak_ptr_factory_.GetWeakPtr(),
                std::move(device)));
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
