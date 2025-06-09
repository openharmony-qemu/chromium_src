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

#include "device/device_watcher/device_watcher.h"
#include "base/functional/bind.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/sequence_checker.h"
#include "base/threading/scoped_blocking_call.h"
#include "ohos/adapter/device/device_adapter.h"

namespace device {
DeviceWatcher::Observer::~Observer() = default;

std::unique_ptr<DeviceWatcher> DeviceWatcher::StartWatching(
    Observer* observer,
    DeviceFilter filter) {
  base::ScopedBlockingCall scoped_blocking_call(FROM_HERE,
                                                base::BlockingType::MAY_BLOCK);
  return base::WrapUnique(new DeviceWatcher(observer, filter));
}

DeviceWatcher::~DeviceWatcher() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (device_adapter_) {
    device_adapter_->UnregisterHotplugCallback();
  }
}

void DeviceWatcher::EnumerateExistingDevices() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  base::ScopedBlockingCall scoped_blocking_call(FROM_HERE,
                                                base::BlockingType::MAY_BLOCK);
  std::vector<USBDevice> device_list;
  if (!device_adapter_ || !device_adapter_->GetDevices(device_list)) {
    LOG(WARNING) << "GetDevices Failed.";
    return;
  }

  for (const auto& device : device_list) {
    if (filter_ && !filter_(device)) {
      continue;
    }
    if (observer_) {
      observer_->OnDeviceAdded(device);
    }
  }
}

DeviceWatcher::DeviceWatcher(Observer* observer, DeviceFilter filter)
    : observer_(observer), filter_(filter) {
  device_adapter_ = std::make_unique<DeviceAdapter>();
  if (device_adapter_) {
    device_adapter_->RegisterHotplugCallback(this);
  }
}

void DeviceWatcher::OnDeviceAttatched(USBDevice& device) {
  if (!observer_) {
    LOG(WARNING) << "Please set observer first.";
    return;
  }

  if (!filter_ || filter_(device)) {
    observer_->OnDeviceAdded(device);
  }
}

void DeviceWatcher::OnDeviceRemoved(USBDevice& device) {
  if (!observer_) {
    LOG(WARNING) << "Please set observer first.";
    return;
  }

  if (!filter_ || filter_(device)) {
    observer_->OnDeviceRemoved(device);
  }
}
}  // namespace device
