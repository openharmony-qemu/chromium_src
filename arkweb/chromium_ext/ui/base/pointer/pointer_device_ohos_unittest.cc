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

#include "ui/base/pointer/pointer_device.h"
#include "arkweb/build/features/features.h"

#include <gtest/gtest.h>
#if BUILDFLAG(ARKWEB_TEST)
#define protected public
#include "arkweb/chromium_ext/ui/base/pointer/pointer_device_ohos.cc"
#endif

namespace ui {
constexpr int TYPE_VAL = 0;
class PointerDeviceOhosTest: public testing::Test {
public:
  void SetUp() {
    DeviceDataManager::CreateInstance();
    manager_ = DeviceDataManager::GetInstance();
    ASSERT_TRUE(manager_);
  }

  void TearDown() {
    DeviceDataManager::DeleteInstance();
  }

  DeviceDataManager* GetDeviceDataManager() {
    return manager_;
  }

private:
  DeviceDataManager* manager_;
};

TEST_F(PointerDeviceOhosTest, MaxTouchPoints) {
  EXPECT_EQ(MaxTouchPoints(), 0);
}

TEST_F(PointerDeviceOhosTest, GetPrimaryPointerType) {
  EXPECT_EQ(GetPrimaryPointerType(POINTER_TYPE_FINE), POINTER_TYPE_FINE);
  EXPECT_EQ(GetPrimaryPointerType(POINTER_TYPE_COARSE), POINTER_TYPE_COARSE);
  EXPECT_EQ(GetPrimaryPointerType(POINTER_TYPE_NONE), POINTER_TYPE_NONE);
}

TEST_F(PointerDeviceOhosTest, GetPrimaryHoverType) {
  EXPECT_EQ(GetPrimaryHoverType(HOVER_TYPE_HOVER), HOVER_TYPE_HOVER);
  EXPECT_EQ(GetPrimaryHoverType(HOVER_TYPE_NONE), HOVER_TYPE_NONE);
}

TEST_F(PointerDeviceOhosTest, GetAvailableHoverTypes) {
  GetDeviceDataManager()->ResetDeviceListsForTest();
  TouchpadDevice device;
  device.enabled = true;
  std::vector<TouchpadDevice> touchpadDeviceList;
  touchpadDeviceList.push_back(device);
  GetDeviceDataManager()->OnTouchpadDevicesUpdated(touchpadDeviceList);
  EXPECT_EQ(GetAvailableHoverTypes(), HOVER_TYPE_HOVER);
  GetDeviceDataManager()->ResetDeviceListsForTest();

  InputDevice input;
  input.enabled = true;
  std::vector<InputDevice> inputDeviceList;
  inputDeviceList.push_back(input);
  GetDeviceDataManager()->OnMouseDevicesUpdated(inputDeviceList);
  EXPECT_EQ(GetAvailableHoverTypes(), HOVER_TYPE_HOVER);
  GetDeviceDataManager()->ResetDeviceListsForTest();

  GetDeviceDataManager()->OnPointingStickDevicesUpdated(inputDeviceList);
  EXPECT_EQ(GetAvailableHoverTypes(), HOVER_TYPE_HOVER);
  GetDeviceDataManager()->ResetDeviceListsForTest();

  EXPECT_EQ(GetAvailableHoverTypes(), HOVER_TYPE_NONE);

  device.enabled = false;
  touchpadDeviceList.clear();
  touchpadDeviceList.push_back(device);
  GetDeviceDataManager()->OnTouchpadDevicesUpdated(touchpadDeviceList);
  EXPECT_EQ(GetAvailableHoverTypes(), HOVER_TYPE_NONE);
  GetDeviceDataManager()->ResetDeviceListsForTest();

  inputDeviceList.clear();
  input.enabled = false;
  inputDeviceList.push_back(input);
  GetDeviceDataManager()->OnMouseDevicesUpdated(inputDeviceList);
  EXPECT_EQ(GetAvailableHoverTypes(), HOVER_TYPE_NONE);
  GetDeviceDataManager()->ResetDeviceListsForTest();

  GetDeviceDataManager()->OnPointingStickDevicesUpdated(inputDeviceList);
  EXPECT_EQ(GetAvailableHoverTypes(), HOVER_TYPE_NONE);
  GetDeviceDataManager()->ResetDeviceListsForTest();
}

TEST_F(PointerDeviceOhosTest, GetTouchScreensAvailability) {
  GetDeviceDataManager()->ResetDeviceListsForTest();
  EXPECT_EQ(GetTouchScreensAvailability(), TouchScreensAvailability::NONE);

  TouchscreenDevice device;
  std::vector<TouchscreenDevice> touchscreenDeviceList;
  touchscreenDeviceList.push_back(device);
  GetDeviceDataManager()->OnTouchscreenDevicesUpdated(touchscreenDeviceList);
  GetDeviceDataManager()->SetTouchscreensEnabled(true);
  EXPECT_EQ(GetTouchScreensAvailability(), TouchScreensAvailability::ENABLED);

  GetDeviceDataManager()->SetTouchscreensEnabled(false);
  EXPECT_EQ(GetTouchScreensAvailability(), TouchScreensAvailability::DISABLED);
  GetDeviceDataManager()->ResetDeviceListsForTest();
}

TEST_F(PointerDeviceOhosTest, GetAvailablePointerTypes) {
  TouchpadDevice device;
  device.enabled = true;
  std::vector<TouchpadDevice> touchpadDeviceList;
  touchpadDeviceList.push_back(device);
  GetDeviceDataManager()->OnTouchpadDevicesUpdated(touchpadDeviceList);

  int atypes = TYPE_VAL;
  atypes |= POINTER_TYPE_COARSE;
  EXPECT_EQ(GetAvailablePointerTypes(), atypes);
  GetDeviceDataManager()->ResetDeviceListsForTest();

  atypes = TYPE_VAL;
  atypes |= POINTER_TYPE_COARSE;
  EXPECT_EQ(GetAvailablePointerTypes(), atypes);
}

TEST_F(PointerDeviceOhosTest, IsMouseOrTouchpadPresent001) {
  EXPECT_FALSE(IsMouseOrTouchpadPresent());
}

TEST_F(PointerDeviceOhosTest, IsMouseOrTouchpadPresent002) {
  GetDeviceDataManager()->ResetDeviceListsForTest();
  TouchpadDevice device;
  device.enabled = true;
  std::vector<TouchpadDevice> touchpadDeviceList;
  touchpadDeviceList.push_back(device);
  GetDeviceDataManager()->OnTouchpadDevicesUpdated(touchpadDeviceList);
  EXPECT_TRUE(IsMouseOrTouchpadPresent());
  GetDeviceDataManager()->ResetDeviceListsForTest();

  InputDevice input;
  input.enabled = true;
  std::vector<InputDevice> inputDeviceList;
  inputDeviceList.push_back(input);
  GetDeviceDataManager()->OnMouseDevicesUpdated(inputDeviceList);
  EXPECT_TRUE(IsMouseOrTouchpadPresent());
  GetDeviceDataManager()->ResetDeviceListsForTest();

  GetDeviceDataManager()->OnPointingStickDevicesUpdated(inputDeviceList);
  EXPECT_TRUE(IsMouseOrTouchpadPresent());
  GetDeviceDataManager()->ResetDeviceListsForTest();

  EXPECT_FALSE(IsMouseOrTouchpadPresent());

  device.enabled = false;
  touchpadDeviceList.clear();
  touchpadDeviceList.push_back(device);
  GetDeviceDataManager()->OnTouchpadDevicesUpdated(touchpadDeviceList);
  EXPECT_FALSE(IsMouseOrTouchpadPresent());
  GetDeviceDataManager()->ResetDeviceListsForTest();

  inputDeviceList.clear();
  input.enabled = false;
  inputDeviceList.push_back(input);
  GetDeviceDataManager()->OnMouseDevicesUpdated(inputDeviceList);
  EXPECT_FALSE(IsMouseOrTouchpadPresent());
  GetDeviceDataManager()->ResetDeviceListsForTest();

  GetDeviceDataManager()->OnPointingStickDevicesUpdated(inputDeviceList);
  EXPECT_FALSE(IsMouseOrTouchpadPresent());
  GetDeviceDataManager()->ResetDeviceListsForTest();
}

// GetPointerDevice and GetPointerDevices basic tests (stub functions)
TEST_F(PointerDeviceOhosTest, GetPointerDeviceStubReturnsNullopt) {
  auto result = GetPointerDevice(0);
  EXPECT_FALSE(result.has_value());
}

TEST_F(PointerDeviceOhosTest, GetPointerDevicesStubReturnsEmpty) {
  auto result = GetPointerDevices();
  EXPECT_TRUE(result.empty());
}

// GetAvailablePointerTypes with no devices
TEST_F(PointerDeviceOhosTest, GetAvailablePointerTypesNoDevices) {
  GetDeviceDataManager()->ResetDeviceListsForTest();

  // No devices at all
  EXPECT_EQ(GetAvailablePointerTypes(), POINTER_TYPE_NONE);
}

// GetAvailablePointerTypes with touchscreen only
TEST_F(PointerDeviceOhosTest, GetAvailablePointerTypesTouchscreenOnly) {
  GetDeviceDataManager()->ResetDeviceListsForTest();

  // Add only a touchscreen device
  TouchscreenDevice touchscreen;
  std::vector<TouchscreenDevice> touchscreenDeviceList;
  touchscreenDeviceList.push_back(touchscreen);
  GetDeviceDataManager()->OnTouchscreenDevicesUpdated(touchscreenDeviceList);

  // Only touchscreen present, should return POINTER_TYPE_COARSE
  EXPECT_EQ(GetAvailablePointerTypes(), POINTER_TYPE_COARSE);
  GetDeviceDataManager()->ResetDeviceListsForTest();
}

// GetAvailablePointerTypes with touchpad only
TEST_F(PointerDeviceOhosTest, GetAvailablePointerTypesTouchpadOnly) {
  GetDeviceDataManager()->ResetDeviceListsForTest();

  // Add only a touchpad device
  TouchpadDevice touchpad;
  touchpad.enabled = true;
  std::vector<TouchpadDevice> touchpadDeviceList;
  touchpadDeviceList.push_back(touchpad);
  GetDeviceDataManager()->OnTouchpadDevicesUpdated(touchpadDeviceList);

  // Touchpad is a fine pointer device, should return POINTER_TYPE_FINE | POINTER_TYPE_COARSE
  int expected = POINTER_TYPE_FINE | POINTER_TYPE_COARSE;
  EXPECT_EQ(GetAvailablePointerTypes(), expected);
  GetDeviceDataManager()->ResetDeviceListsForTest();
}

// GetAvailablePointerTypes with pointing stick only
TEST_F(PointerDeviceOhosTest, GetAvailablePointerTypesPointingStickOnly) {
  GetDeviceDataManager()->ResetDeviceListsForTest();

  // Add only a pointing stick device
  InputDevice pointingStick;
  pointingStick.enabled = true;
  std::vector<InputDevice> pointingStickDeviceList;
  pointingStickDeviceList.push_back(pointingStick);
  GetDeviceDataManager()->OnPointingStickDevicesUpdated(pointingStickDeviceList);

  // Pointing stick is a fine pointer device, should return POINTER_TYPE_FINE | POINTER_TYPE_COARSE
  int expected = POINTER_TYPE_FINE | POINTER_TYPE_COARSE;
  EXPECT_EQ(GetAvailablePointerTypes(), expected);
  GetDeviceDataManager()->ResetDeviceListsForTest();
}

// GetAvailablePointerTypes with both mouse and touchscreen
TEST_F(PointerDeviceOhosTest, GetAvailablePointerTypesMouseAndTouchscreen) {
  GetDeviceDataManager()->ResetDeviceListsForTest();

  // Add a mouse device
  InputDevice mouse;
  mouse.enabled = true;
  std::vector<InputDevice> mouseDeviceList;
  mouseDeviceList.push_back(mouse);
  GetDeviceDataManager()->OnMouseDevicesUpdated(mouseDeviceList);

  // Add a touchscreen device
  TouchscreenDevice touchscreen;
  std::vector<TouchscreenDevice> touchscreenDeviceList;
  touchscreenDeviceList.push_back(touchscreen);
  GetDeviceDataManager()->OnTouchscreenDevicesUpdated(touchscreenDeviceList);

  // Both mouse and touchscreen present, should return POINTER_TYPE_FINE | POINTER_TYPE_COARSE
  int expected = POINTER_TYPE_FINE | POINTER_TYPE_COARSE;
  EXPECT_EQ(GetAvailablePointerTypes(), expected);
  GetDeviceDataManager()->ResetDeviceListsForTest();
}

// IsTouchDevicePresent tests
TEST_F(PointerDeviceOhosTest, IsTouchDevicePresentNoDevice) {
  GetDeviceDataManager()->ResetDeviceListsForTest();
  EXPECT_FALSE(IsTouchDevicePresent());
}

TEST_F(PointerDeviceOhosTest, IsTouchDevicePresentWithDevice) {
  GetDeviceDataManager()->ResetDeviceListsForTest();
  TouchscreenDevice device;
  device.enabled = true;
  std::vector<TouchscreenDevice> touchscreenDeviceList;
  touchscreenDeviceList.push_back(device);
  GetDeviceDataManager()->OnTouchscreenDevicesUpdated(touchscreenDeviceList);
  EXPECT_TRUE(IsTouchDevicePresent());
  GetDeviceDataManager()->ResetDeviceListsForTest();
}

// GetTouchScreensAvailability with disabled touchscreen
TEST_F(PointerDeviceOhosTest, GetTouchScreensAvailabilityDisabled) {
  GetDeviceDataManager()->ResetDeviceListsForTest();

  // Add touchscreen device but disable it
  TouchscreenDevice device;
  device.enabled = true;
  std::vector<TouchscreenDevice> touchscreenDeviceList;
  touchscreenDeviceList.push_back(device);
  GetDeviceDataManager()->OnTouchscreenDevicesUpdated(touchscreenDeviceList);
  GetDeviceDataManager()->SetTouchscreensEnabled(false);

  EXPECT_EQ(GetTouchScreensAvailability(), TouchScreensAvailability::DISABLED);
  GetDeviceDataManager()->ResetDeviceListsForTest();
}

}  // namespace ui

#if BUILDFLAG(ARKWEB_TEST)
#undef protected
#endif
