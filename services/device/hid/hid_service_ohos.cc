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

#include "services/device/hid/hid_service_ohos.h"

#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/sequence_checker.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/thread_pool.h"
#include "base/threading/scoped_blocking_call.h"
#include "device/device_watcher/device_watcher.h"

namespace device {
constexpr int32_t kHidInterfaceId = 0x03;
bool IsHidInterface(const USBInterface& interface) {
  if (interface.clazz == kHidInterfaceId) {
    return true;
  }
  return false;
}

bool FilterHidDevice(USBDevice device) {
  if (device.clazz != 0 || device.subClass != 0 || device.protocol != 0 ||
      device.configs.empty()) {
    return false;
  }
  bool is_hid = false;
  for (auto iter_config = device.configs.begin();
       iter_config != device.configs.end();) {
    if (iter_config->interfaces.empty()) {
      device.configs.erase(iter_config);
      continue;
    }

    for (auto iter_iface = iter_config->interfaces.begin();
         iter_iface != iter_config->interfaces.end();) {
      if (!IsHidInterface(*iter_iface)) {
        iter_config->interfaces.erase(iter_iface);
        continue;
      }
      is_hid = true;
      iter_iface++;
    }
    iter_config++;
  }
  return is_hid;
}

class HidServiceOhos::BlockingTaskRunnerHelper
    : public DeviceWatcher::Observer {
 public:
  BlockingTaskRunnerHelper(base::WeakPtr<HidServiceOhos> service,
                           scoped_refptr<base::SequencedTaskRunner> task_runner)
      : service_(std::move(service)), task_runner_(std::move(task_runner)) {
    watcher_ = DeviceWatcher::StartWatching(this, FilterHidDevice);
    watcher_->EnumerateExistingDevices();
    task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&HidServiceOhos::FirstEnumerationComplete, service_));
  }

  BlockingTaskRunnerHelper(const BlockingTaskRunnerHelper&) = delete;
  BlockingTaskRunnerHelper& operator=(const BlockingTaskRunnerHelper&) = delete;

  ~BlockingTaskRunnerHelper() override {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  }

 private:
  void OnDeviceAdded(const USBDevice& device) override {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    base::ScopedBlockingCall scoped_blocking_call(
        FROM_HERE, base::BlockingType::MAY_BLOCK);
    for (const auto& config : device.configs) {
      for (const auto& iface : config.interfaces) {
        auto hid_info = base::MakeRefCounted<HidDeviceInfo>(
            device.name, std::to_string(device.devAddress),
            static_cast<uint16_t>(device.vendorId),
            static_cast<uint16_t>(device.productId), device.productName,
            device.serial, static_cast<mojom::HidBusType>(0),
            std::vector<uint8_t>());

        task_runner_->PostTask(
            FROM_HERE,
            base::BindOnce(&HidServiceOhos::AddDevice, service_, hid_info));
      }
    }
  }

  void OnDeviceRemoved(const USBDevice& device) override {
    DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
    base::ScopedBlockingCall scoped_blocking_call(
        FROM_HERE, base::BlockingType::MAY_BLOCK);
    for (const auto& config : device.configs) {
      for (const auto& iface : config.interfaces) {
        task_runner_->PostTask(
            FROM_HERE, base::BindOnce(&HidServiceOhos::RemoveDevice, service_,
                                      std::string(device.name)));
      }
    }
  }

  void OnDeviceChanged(const USBDevice& device) override {}

  SEQUENCE_CHECKER(sequence_checker_);
  std::unique_ptr<DeviceWatcher> watcher_;

  // This weak pointer is only valid when checked on this task runner.
  base::WeakPtr<HidServiceOhos> service_;
  scoped_refptr<base::SequencedTaskRunner> task_runner_;
};

HidServiceOhos::HidServiceOhos() {
  helper_ = base::SequenceBound<BlockingTaskRunnerHelper>(
      base::ThreadPool::CreateSequencedTaskRunner(kBlockingTaskTraits),
      weak_factory_.GetWeakPtr(),
      base::SequencedTaskRunner::GetCurrentDefault());
}

HidServiceOhos::~HidServiceOhos() = default;

void HidServiceOhos::Connect(const std::string& device_guid,
                             bool allow_protected_reports,
                             bool allow_fido_reports,
                             ConnectCallback callback) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  NOTIMPLEMENTED();
}

base::WeakPtr<HidService> HidServiceOhos::GetWeakPtr() {
  return weak_factory_.GetWeakPtr();
}
}  // namespace device
