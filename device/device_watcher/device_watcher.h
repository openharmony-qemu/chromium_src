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
