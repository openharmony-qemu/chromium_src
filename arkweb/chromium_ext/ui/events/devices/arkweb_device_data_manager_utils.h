/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ARKWEB_DEVICE_DATA_MANAGER_UTILS_H_
#define ARKWEB_DEVICE_DATA_MANAGER_UTILS_H_

#include "arkweb/build/features/features.h"
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
#include "arkweb/chromium_ext/ui/events/devices/mmi_device_info_adapter_impl.h"
#include "base/allocator/partition_allocator/src/partition_alloc/pointers/raw_ptr.h"
#include "base/logging.h"
#include "base/task/thread_pool.h"
#include "content/public/browser/browser_thread.h"
#include "ui/events/devices/device_hotplug_event_observer.h"
#include "ui/events/devices/events_devices_export.h"
#include "ui/events/devices/input_device.h"
#include "ui/events/devices/input_device_event_observer.h"
#include "ui/events/devices/keyboard_device.h"
#include "ui/events/devices/touch_device_transform.h"
#include "ui/events/devices/touchpad_device.h"
#include "ui/events/devices/touchscreen_device.h"

namespace ui {

class DeviceDataManager;

class ArkWebDeviceDataManagerUtils {
 public:
  friend class DeviceDataManager;
  explicit ArkWebDeviceDataManagerUtils(DeviceDataManager* device_data_manger);
  ~ArkWebDeviceDataManagerUtils();

  ArkWebDeviceDataManagerUtils(const ArkWebDeviceDataManagerUtils&) = delete;
  ArkWebDeviceDataManagerUtils& operator=(const ArkWebDeviceDataManagerUtils&) =
      delete;

  void AddKeyboardDevice(const KeyboardDevice& device);
  void AddMouseDevice(const InputDevice& device);
  void AddTouchpadDevice(const TouchpadDevice& device);
  void DeleteDevice(const InputDevice& devices);

 private:
  const raw_ptr<DeviceDataManager> device_data_manager_;
  std::unique_ptr<OHOS::NWeb::MMIAdapter> mmi_adapter_ = nullptr;
  std::shared_ptr<OHOS::NWeb::MMIListenerAdapter> dev_listener_ = nullptr;
  scoped_refptr<base::SequencedTaskRunner> sequenced_task_runner_;
  void SetupDeviceListeners();
  void CleanupDeviceListeners();
};
}  // namespace ui

#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#endif  // ARKWEB_DEVICE_DATA_MANAGER_UTILS_H_
