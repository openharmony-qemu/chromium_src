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

}  // namespace ui

#if BUILDFLAG(ARKWEB_TEST)
#undef protected
#endif