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

#include "device/bluetooth/ohos/bluetooth_discovery_manager_ohos.h"

#include "base/check_op.h"
#include "base/logging.h"
#include "base/memory/weak_ptr.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "ohos/adapter/bluetooth/bluetooth_adapter.h"

using namespace ohos::adapter::device;
namespace device {

class BluetoothDiscoveryManagerOhosClassic
    : public BluetoothDiscoveryManagerOhos {
 public:
  explicit BluetoothDiscoveryManagerOhosClassic(Observer* observer)
      : BluetoothDiscoveryManagerOhos(observer), discovering_(false) {}

  BluetoothDiscoveryManagerOhosClassic(
      const BluetoothDiscoveryManagerOhosClassic&) = delete;
  BluetoothDiscoveryManagerOhosClassic& operator=(
      const BluetoothDiscoveryManagerOhosClassic&) = delete;

  ~BluetoothDiscoveryManagerOhosClassic() override {}
  bool IsDiscovering() const override { return discovering_; }

  bool StartDiscovery() override {
    DVLOG(1) << "Bluetooth Classic: StartDiscovery";
    DCHECK(!discovering_);

    DVLOG(1) << "Discovery requested";
    discovering_ = true;

    BluetoothOhos::StartDiscoveryMonitor(
        [=](std::vector<std::string> deviceList) {
          for (const auto& device : deviceList) {
            if (!discovering_) {
              break;
            }
            DeviceFound(device);
          }
        });
    BluetoothOhos::StartDiscovery();

    return true;
  }

  bool StopDiscovery() override {
    DVLOG(1) << "Bluetooth Classic: StopDiscovery";
    DCHECK(discovering_);

    discovering_ = false;

    BluetoothOhos::StopDiscovery();
    BluetoothOhos::StopDiscoveryMonitor();

    return true;
  }

  void ProcessScanResult(const std::string& address) {
    if (observer_) {
      observer_->ClassicDeviceFound(address);
    }
  }

  void DeviceFound(const std::string& address) {
    DCHECK(observer_);
    base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(&BluetoothDiscoveryManagerOhosClassic::ProcessScanResult,
                       weak_ptr_factory_.GetWeakPtr(),
                       address));
  }

 private:
  // The requested discovery state.
  bool discovering_;
  base::WeakPtrFactory<BluetoothDiscoveryManagerOhosClassic>
      weak_ptr_factory_{this};
};

BluetoothDiscoveryManagerOhos::BluetoothDiscoveryManagerOhos(Observer* observer)
    : observer_(observer) {
  DCHECK(observer);
}

BluetoothDiscoveryManagerOhos::~BluetoothDiscoveryManagerOhos() {}

// static
BluetoothDiscoveryManagerOhos* BluetoothDiscoveryManagerOhos::CreateClassic(
    Observer* observer) {
  return new BluetoothDiscoveryManagerOhosClassic(observer);
}

}  // namespace device
