// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
                       weak_ptr_factory_.GetWeakPtr(), address));
  }

 private:
  // The requested discovery state.
  bool discovering_;
  base::WeakPtrFactory<BluetoothDiscoveryManagerOhosClassic> weak_ptr_factory_{
      this};
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
