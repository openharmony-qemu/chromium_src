// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/pointer/pointer_device.h"

#include "arkweb/build/features/features.h"
#include "base/check_op.h"
#include "ui/events/devices/device_data_manager.h"

#if BUILDFLAG(ARKWEB_FLING)
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
#include "base/ohos/sys_info_utils_ext.h"
#endif

namespace ui {

namespace {

bool IsTouchDevicePresent() {
  return !DeviceDataManager::GetInstance()->GetTouchscreenDevices().empty();
}

bool IsMouseOrTouchpadPresent() {
  DeviceDataManager* device_data_manager = DeviceDataManager::GetInstance();
  for (const ui::InputDevice& device :
       device_data_manager->GetTouchpadDevices()) {
    if (device.enabled)
      return true;
  }
  // We didn't find a touchpad then let's look if there is a mouse connected.
  for (const ui::InputDevice& device : device_data_manager->GetMouseDevices()) {
    if (device.enabled)
      return true;
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

int GetAvailablePointerTypes() {
  int available_pointer_types = 0;
#if BUILDFLAG(ARKWEB_FLING)
  auto& system_properties_adapter = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                        .GetSystemPropertiesInstance();
  OHOS::NWeb::ProductDeviceType deviceType =
      system_properties_adapter.GetProductDeviceType();

  if (deviceType == OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_2IN1) {
    available_pointer_types |= POINTER_TYPE_FINE;
  }
  if (deviceType == OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_TABLET ||
      deviceType == OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_MOBILE) {
    available_pointer_types |= POINTER_TYPE_COARSE;
  }

#else
  if (IsMouseOrTouchpadPresent())
    available_pointer_types |= POINTER_TYPE_FINE;

  if (IsTouchDevicePresent())
    available_pointer_types |= POINTER_TYPE_COARSE;
#endif

  if (available_pointer_types == 0)
    available_pointer_types = POINTER_TYPE_NONE;

  DCHECK(available_pointer_types);
  return available_pointer_types;
}

int GetAvailableHoverTypes() {
  int available_pointer_types = 0;
#if BUILDFLAG(IS_ARKWEB)
  if (base::ohos::IsPcDevice()) {
    available_pointer_types |= POINTER_TYPE_FINE;
  }
  if (base::ohos::IsTabletDevice() || base::ohos::IsMobileDevice()) {
    available_pointer_types |= POINTER_TYPE_COARSE;
  }

#else
  if (IsMouseOrTouchpadPresent())
    available_pointer_types |= POINTER_TYPE_FINE;

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

TouchScreensAvailability GetTouchScreensAvailability() {
  if (!IsTouchDevicePresent())
    return TouchScreensAvailability::NONE;

  return DeviceDataManager::GetInstance()->AreTouchscreensEnabled()
             ? TouchScreensAvailability::ENABLED
             : TouchScreensAvailability::DISABLED;
}

int MaxTouchPoints() {
  int max_touch = 0;
  const std::vector<ui::TouchscreenDevice>& touchscreen_devices =
      ui::DeviceDataManager::GetInstance()->GetTouchscreenDevices();
  for (const ui::TouchscreenDevice& device : touchscreen_devices) {
    if (device.touch_points > max_touch)
      max_touch = device.touch_points;
  }
  return max_touch;
}

PointerType GetPrimaryPointerType(int available_pointer_types) {
  if (available_pointer_types & POINTER_TYPE_FINE)
    return POINTER_TYPE_FINE;
  if (available_pointer_types & POINTER_TYPE_COARSE)
    return POINTER_TYPE_COARSE;
  DCHECK_EQ(available_pointer_types, POINTER_TYPE_NONE);
  return POINTER_TYPE_NONE;
}

HoverType GetPrimaryHoverType(int available_hover_types) {
  if (available_hover_types & HOVER_TYPE_HOVER)
    return HOVER_TYPE_HOVER;
  DCHECK_EQ(available_hover_types, HOVER_TYPE_NONE);
  return HOVER_TYPE_NONE;
}

std::optional<PointerDevice> GetPointerDevice(PointerDevice::Key key) {
  return std::nullopt;
}

std::vector<PointerDevice> GetPointerDevices() {
  return {};
}

}  // namespace ui
