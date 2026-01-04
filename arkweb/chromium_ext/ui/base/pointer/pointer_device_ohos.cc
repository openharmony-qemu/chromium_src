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

#include <dlfcn.h>

#include "arkweb/build/features/features.h"
#include "base/check_op.h"
#include "base/logging.h"
#include "multimodalinput/oh_input_manager.h"
#include "ui/base/pointer/pointer_device.h"
#include "ui/events/devices/device_data_manager.h"

#if BUILDFLAG(IS_ARKWEB)
#include "base/ohos/sys_info_utils_ext.h"
#endif

#if BUILDFLAG(ARKWEB_FLING)
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif


namespace ui {

const int DEFAULT_TOUCH_POINT_NUM = 0;
using OHInputQueryMaxTouchPoints = Input_Result (*)(int32_t* count);

namespace {

bool IsTouchDevicePresent()
{
  return !DeviceDataManager::GetInstance()->GetTouchscreenDevices().empty();
}

bool IsMouseOrTouchpadPresent()
{
  DeviceDataManager* device_data_manager = DeviceDataManager::GetInstance();
  for (const ui::InputDevice& device :
       device_data_manager->GetTouchpadDevices()) {
    if (device.enabled) {
      return true;
    }
  }
  // We didn't find a touchpad then let's look if there is a mouse connected.
  for (const ui::InputDevice& device : device_data_manager->GetMouseDevices()) {
    if (device.enabled) {
      return true;
    }
  }
  // We didn't find a mouse then let's look if there is a pointing stick
  // connected.
  for (const ui::InputDevice& device :
       device_data_manager->GetPointingStickDevices()) {
    if (device.enabled) {
      return true;
    }
  }
  return false;
}

}  // namespace

int GetAvailablePointerTypes()
{
  int available_pointer_types = 0;
#if BUILDFLAG(ARKWEB_FLING)
  if (base::ohos::IsPcDevice()) {
    available_pointer_types |= POINTER_TYPE_FINE;
  }
  if (base::ohos::IsTabletDevice() || base::ohos::IsMobileDevice()) {
    available_pointer_types |= POINTER_TYPE_COARSE;
  }
#else
  if (IsMouseOrTouchpadPresent()) {
    available_pointer_types |= POINTER_TYPE_FINE;
  }

  if (IsTouchDevicePresent()) {
    available_pointer_types |= POINTER_TYPE_COARSE;
  }
#endif

  if (available_pointer_types == 0) {
    available_pointer_types = POINTER_TYPE_NONE;
  }

  DCHECK(available_pointer_types);
  return available_pointer_types;
}

int GetAvailableHoverTypes()
{
  if (IsMouseOrTouchpadPresent()) {
    return HOVER_TYPE_HOVER;
  }

  return HOVER_TYPE_NONE;
}

TouchScreensAvailability GetTouchScreensAvailability()
{
  if (!IsTouchDevicePresent()) {
    return TouchScreensAvailability::NONE;
  }

  return DeviceDataManager::GetInstance()->AreTouchscreensEnabled()
             ? TouchScreensAvailability::ENABLED
             : TouchScreensAvailability::DISABLED;
}

NO_SANITIZE("cfi-icall") int MaxTouchPoints()
{
  int point_num = DEFAULT_TOUCH_POINT_NUM;
  OHInputQueryMaxTouchPoints get_max_touch_point = nullptr;

#if defined(__aarch64__)
  void* dl = dlopen("/system/lib64/ndk/libohinput.so", RTLD_LAZY);
#else
  void* dl = dlopen("/system/lib/ndk/libohinput.so", RTLD_LAZY);
#endif

  if (dl == nullptr) {
    LOG(WARNING) << "cannot find the MultiModal's path";
    return DEFAULT_TOUCH_POINT_NUM;
  }

  get_max_touch_point = reinterpret_cast<OHInputQueryMaxTouchPoints>(
      dlsym(dl, "OH_Input_QueryMaxTouchPoints"));
  if (get_max_touch_point == nullptr) {
    LOG(WARNING)
        << "cannot find the function called OH_Input_QueryMaxTouchPoints";
    dlclose(dl);
    return DEFAULT_TOUCH_POINT_NUM;
  }

  Input_Result input_res = get_max_touch_point(&point_num);
  dlclose(dl);

  if (input_res != INPUT_SUCCESS) {
    LOG(WARNING) << "Get max touch points failed.";
    return DEFAULT_TOUCH_POINT_NUM;
  }

  if (point_num < 0) {
    LOG(WARNING) << "MultiModal returns -1 unknown touch points ";
    return DEFAULT_TOUCH_POINT_NUM;
  }

  LOG(INFO) << "MultiModal returns " << point_num;
  return point_num;
}

PointerType GetPrimaryPointerType(int available_pointer_types)
{
  if (available_pointer_types & POINTER_TYPE_FINE) {
    return POINTER_TYPE_FINE;
  }
  if (available_pointer_types & POINTER_TYPE_COARSE) {
    return POINTER_TYPE_COARSE;
  }
  DCHECK_EQ(available_pointer_types, POINTER_TYPE_NONE);
  return POINTER_TYPE_NONE;
}

HoverType GetPrimaryHoverType(int available_hover_types)
{
  if (available_hover_types & HOVER_TYPE_HOVER) {
    return HOVER_TYPE_HOVER;
  }
  DCHECK_EQ(available_hover_types, HOVER_TYPE_NONE);
  return HOVER_TYPE_NONE;
}

std::optional<PointerDevice> GetPointerDevice(PointerDevice::Key key)
{
  return std::nullopt;
}

std::vector<PointerDevice> GetPointerDevices()
{
  return {};
}

}  // namespace ui