// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_DEVICE_WATCHER_DEVICE_WATCHER_H_
#define DEVICE_DEVICE_WATCHER_DEVICE_WATCHER_H_

#include <memory>
#include <vector>

#include "base/component_export.h"
#include "base/memory/raw_ptr.h"
#include "base/sequence_checker.h"
#include "ohos/adapter/device/device_adapter.h"

namespace device {
using DeviceAdapter = ohos::adapter::device::DeviceAdapter;
using HotplugCallback = ohos::adapter::device::HotplugCallback;

using USBDevice = ohos::adapter::device::USBDevice;
using USBInterface = ohos::adapter::device::USBInterface;

using DeviceFilter = std::function<bool(const USBDevice&)>;

class COMPONENT_EXPORT(DEVICE_DEVICE_WATCHER) DeviceWatcher
    : public HotplugCallback {
 public:
  class Observer {
   public:
    virtual void OnDeviceAdded(const USBDevice& device) = 0;
    virtual void OnDeviceRemoved(const USBDevice& device) = 0;
    virtual void OnDeviceChanged(const USBDevice& device) = 0;

   protected:
    virtual ~Observer();
  };

  static std::unique_ptr<DeviceWatcher> StartWatching(Observer* observer,
                                                      DeviceFilter filter);

  DeviceWatcher(const DeviceWatcher&) = delete;
  DeviceWatcher& operator=(const DeviceWatcher&) = delete;

  ~DeviceWatcher() override;

  void EnumerateExistingDevices();
  void OnDeviceAttatched(USBDevice& device) override;
  void OnDeviceRemoved(USBDevice& device) override;

 private:
  explicit DeviceWatcher(Observer* observer, DeviceFilter filter);

  raw_ptr<Observer> observer_;
  std::unique_ptr<DeviceAdapter> device_adapter_ = nullptr;
  DeviceFilter filter_ = nullptr;
  SEQUENCE_CHECKER(sequence_checker_);
};
}  // namespace device
#endif  // DEVICE_DEVICE_WATCHER_DEVICE_WATCHER_H_
