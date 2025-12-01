// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
