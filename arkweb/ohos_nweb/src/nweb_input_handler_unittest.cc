/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "cef_browser.h"
#include "capi/nweb_download_delegate_callback.h"
#include "mock_nweb_delegate.h"
#define private public
#include "arkweb/build/features/features.h"
#include "arkweb/ohos_nweb/src/capi/nweb_devtools_message_handler.h"
#include "nweb_delegate_interface.h"
#include "nweb_input_handler.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

using namespace testing;
namespace OHOS::NWeb {
class MockNWebTouchPointInfo : public NWebTouchPointInfo {
 public:
  ~MockNWebTouchPointInfo() = default;
  MOCK_METHOD(int, GetId, (), (override));
  MOCK_METHOD(double, GetX, (), (override));
  MOCK_METHOD(double, GetY, (), (override));
};

class MockNWebMouseEvent : public NWebMouseEvent {
 public:
  virtual ~MockNWebMouseEvent() = default;
  MOCK_METHOD(int32_t, GetX, (), (override));
  MOCK_METHOD(int32_t, GetY, (), (override));
  MOCK_METHOD(int32_t, GetButton, (), (override));
  MOCK_METHOD(int32_t, GetAction, (), (override));
  MOCK_METHOD(int32_t, GetClickNum, (), (override));
  MOCK_METHOD(std::vector<int32_t>, GetPressKeyCodes, (), (override));
  MOCK_METHOD(int32_t, GetRawX, (), (override));
  MOCK_METHOD(int32_t, GetRawY, (), (override));
};

class MockNWebKeyboardEvent : public NWebKeyboardEvent {
 public:
  virtual ~MockNWebKeyboardEvent() = default;
  MOCK_METHOD(int32_t, GetKeyCode, (), (override));
  MOCK_METHOD(int32_t, GetAction, (), (override));
  MOCK_METHOD(int32_t, GetUnicode, (), (override));
  MOCK_METHOD(bool, IsEnableCapsLock, (), (override));
  MOCK_METHOD(std::vector<int32_t>, GetPressKeyCodes, (), (override));
};

class NWebInputHandlerTest : public ::testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp(void) override;
  void TearDown(void) override;
  std::shared_ptr<MockNWebDelegate> mock_delegate_;
  std::shared_ptr<NWebInputHandler> input_handler_;
};

class MockNWebStylusTouchPointInfo : public NWebStylusTouchPointInfo {
 public:
  ~MockNWebStylusTouchPointInfo() = default;
  MOCK_METHOD(int, GetId, (), (override));
  MOCK_METHOD(float, GetForce, (), (override));
  MOCK_METHOD(float, GetTiltX, (), (override));
  MOCK_METHOD(float, GetTiltY, (), (override));
  MOCK_METHOD(float, GetRollAngle, (), (override));
  MOCK_METHOD(int, GetWidth, (), (override));
  MOCK_METHOD(int, GetHeight, (), (override));
  MOCK_METHOD(SourceTool, GetSourceTool, (), (override));

  double GetX() override {
    return x_;
  }

  double GetY() override {
    return y_;
  }

  void SetX(double a) {
    x_ = a;
  }

  void SetY(double a) {
    y_ = a;
  }

private:
  double x_;
  double y_;
};

void NWebInputHandlerTest::SetUpTestCase(void) {}

void NWebInputHandlerTest::TearDownTestCase(void) {}

void NWebInputHandlerTest::SetUp() {
  mock_delegate_ = std::make_shared<MockNWebDelegate>();
  ASSERT_NE(mock_delegate_, nullptr);
  input_handler_ = std::make_shared<NWebInputHandler>(mock_delegate_);
  ASSERT_NE(input_handler_, nullptr);
}

void NWebInputHandlerTest::TearDown() {
  input_handler_ = nullptr;
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_Create_001) {
  std::shared_ptr<NWebInputHandler> input_handler = NWebInputHandler::Create(nullptr);
  EXPECT_NE(input_handler, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnDestroy_001) {
  input_handler_->OnDestroy();
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchPress_001) {
  int32_t id = 1;
  double x = 10.0;
  double y = 20.0;
  bool from_overlay = false;

  input_handler_->OnTouchPress(id, x, y, from_overlay);

  EXPECT_TRUE(input_handler_->touch_press_id_map_[id]);
  EXPECT_EQ(input_handler_->last_touch_start_x_, x);
  EXPECT_EQ(input_handler_->last_x_, x);
  EXPECT_EQ(input_handler_->last_y_, y);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchPress_002) {
  int32_t id = 1;
  double x = 10.0;
  double y = 20.0;
  bool from_overlay = false;
  input_handler_->nweb_delegate_ = nullptr;
  input_handler_->touch_press_id_map_[id] = false;

  input_handler_->OnTouchPress(id, x, y, from_overlay);

  EXPECT_FALSE(input_handler_->touch_press_id_map_[id]);
  EXPECT_NE(input_handler_->last_touch_start_x_, x);
  EXPECT_NE(input_handler_->last_x_, x);
  EXPECT_NE(input_handler_->last_y_, y);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchRelease_001) {
  int32_t id = 1;
  double x = 10.0;
  double y = 20.0;
  bool from_overlay = false;
  input_handler_->nweb_delegate_ = nullptr;
  input_handler_->touch_press_id_map_[id] = false;

  input_handler_->OnTouchRelease(id, x, y, from_overlay);

  EXPECT_EQ(input_handler_->touch_press_id_map_[id], false);
  input_handler_->touch_press_id_map_.erase(id);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchRelease_002) {
  int32_t id = 1;
  double x = 10.0;
  double y = 20.0;
  bool from_overlay = false;
  input_handler_->touch_press_id_map_[id] = false;

  input_handler_->OnTouchRelease(id, x, y, from_overlay);

  auto it = input_handler_->touch_press_id_map_.find(id);
  EXPECT_TRUE(it == input_handler_->touch_press_id_map_.end());
  input_handler_->touch_press_id_map_.erase(id);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchRelease_003) {
  int32_t id = 1;
  double x = 0;
  double y = 20.0;
  bool from_overlay = false;
  input_handler_->touch_press_id_map_[id] = false;

  input_handler_->OnTouchRelease(id, x, y, from_overlay);

  auto it = input_handler_->touch_press_id_map_.find(id);
  EXPECT_TRUE(it == input_handler_->touch_press_id_map_.end());
  input_handler_->touch_press_id_map_.erase(id);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchRelease_004) {
  int32_t id = 1;
  double x = 10.0;
  double y = 0;
  bool from_overlay = false;
  input_handler_->touch_press_id_map_[id] = false;

  input_handler_->OnTouchRelease(id, x, y, from_overlay);

  auto it = input_handler_->touch_press_id_map_.find(id);
  EXPECT_TRUE(it == input_handler_->touch_press_id_map_.end());
  input_handler_->touch_press_id_map_.erase(id);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchRelease_005) {
  int32_t id = 1;
  double x = 0;
  double y = 0;
  bool from_overlay = false;
  input_handler_->touch_press_id_map_[id] = false;

  input_handler_->OnTouchRelease(id, x, y, from_overlay);

  auto it = input_handler_->touch_press_id_map_.find(id);
  EXPECT_TRUE(it == input_handler_->touch_press_id_map_.end());
  input_handler_->touch_press_id_map_.erase(id);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchMove_001) {
  int32_t id = 1;
  double x = 1;
  double y = 1;
  bool from_overlay = false;
  input_handler_->nweb_delegate_ = nullptr;

  input_handler_->OnTouchMove(id, x, y, from_overlay);

  EXPECT_NE(input_handler_->last_x_, x);
  EXPECT_NE(input_handler_->last_y_, y);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchMove_002) {
  int32_t id = 1;
  double x = 1;
  double y = 1;
  bool from_overlay = false;

  input_handler_->OnTouchMove(id, x, y, from_overlay);

  EXPECT_EQ(input_handler_->last_x_, x);
  EXPECT_EQ(input_handler_->last_y_, y);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchMove_003) {
  double temp = -1;
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos = {};
  input_handler_->nweb_delegate_ = nullptr;

  input_handler_->OnTouchMove(touch_point_infos, true);

  EXPECT_EQ(input_handler_->last_x_, temp);
  EXPECT_EQ(input_handler_->last_y_, temp);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchMove_004) {
  double temp = -1;
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos = {};

  input_handler_->OnTouchMove(touch_point_infos, true);

  EXPECT_EQ(input_handler_->last_x_, temp);
  EXPECT_EQ(input_handler_->last_y_, temp);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchMove_005) {
  double temp = 100;
  std::shared_ptr<MockNWebTouchPointInfo> touch_point =
      std::make_shared<MockNWebTouchPointInfo>();
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos = {
      touch_point};
  EXPECT_CALL(*touch_point, GetX()).Times(1).WillOnce(Return(temp));
  EXPECT_CALL(*touch_point, GetY()).Times(1).WillOnce(Return(temp));

  input_handler_->OnTouchMove(touch_point_infos, true);

  EXPECT_EQ(input_handler_->last_x_, temp);
  EXPECT_EQ(input_handler_->last_y_, temp);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchCancel_001) {
  int32_t id = 1;
  input_handler_->nweb_delegate_ = nullptr;
  input_handler_->touch_press_id_map_[id] = false;
  EXPECT_CALL(*mock_delegate_, OnTouchCancelById(::testing::_, ::testing::_,
                                                 ::testing::_, ::testing::_))
      .Times(0);

  input_handler_->OnTouchCancel();

  EXPECT_EQ(input_handler_->touch_press_id_map_[id], false);
  input_handler_->touch_press_id_map_.erase(id);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchCancel_002) {
  int32_t id = 1;
  input_handler_->touch_press_id_map_[id] = false;

  input_handler_->OnTouchCancel();

  auto it = input_handler_->touch_press_id_map_.find(id);
  EXPECT_TRUE(it == input_handler_->touch_press_id_map_.end());
  input_handler_->touch_press_id_map_.erase(id);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnTouchCancel_003) {
  int32_t id = 1;
  input_handler_->touch_press_id_map_[id] = true;
  EXPECT_CALL(*mock_delegate_, OnTouchCancelById(::testing::_, ::testing::_,
                                                 ::testing::_, ::testing::_))
      .Times(1);

  input_handler_->OnTouchCancel();

  input_handler_->touch_press_id_map_.erase(id);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnNavigateBack_001) {
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  input_handler_->OnNavigateBack();
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnNavigateBack_002) {
  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed())
      .WillOnce(::testing::Return(true));
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(1);
  input_handler_->OnNavigateBack();
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_OnNavigateBack_003) {
  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed())
      .WillOnce(::testing::Return(false));
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  input_handler_->OnNavigateBack();
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_WebSendKeyEvent_001) {
  int32_t keyCode = 1;
  int32_t keyAction = 1;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  ON_CALL(*mock_delegate_, WebSendKeyEvent(keyCode, keyAction, pressedCodes))
      .WillByDefault(::testing::Return(true));
  input_handler_->nweb_delegate_ = nullptr;

  auto ret = input_handler_->WebSendKeyEvent(keyCode, keyAction, pressedCodes);
  EXPECT_EQ(ret, false);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_WebSendKeyEvent_002) {
  int32_t keyCode = 1;
  int32_t keyAction = 1;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  EXPECT_CALL(*mock_delegate_,
              WebSendKeyEvent(keyCode, keyAction, pressedCodes))
      .WillOnce(::testing::Return(true));

  auto ret = input_handler_->WebSendKeyEvent(keyCode, keyAction, pressedCodes);
  EXPECT_EQ(ret, true);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_WebSendMouseWheelEvent_001) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes))
      .Times(0);

  input_handler_->WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes);
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_WebSendMouseWheelEvent_002) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  EXPECT_CALL(*mock_delegate_,
              WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes))
      .Times(1);

  input_handler_->WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_WebSendMouseWheelEventV2_001) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  int32_t source = 0;
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              WebSendMouseWheelEventV2(x, y, deltaX, deltaY, pressedCodes, source))
      .Times(0);

  input_handler_->WebSendMouseWheelEventV2(x, y, deltaX, deltaY, pressedCodes, source);
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_WebSendMouseWheelEventV2_002) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  int32_t source = 0;
  EXPECT_CALL(*mock_delegate_,
              WebSendMouseWheelEventV2(x, y, deltaX, deltaY, pressedCodes, source))
      .Times(1);

  input_handler_->WebSendMouseWheelEventV2(x, y, deltaX, deltaY, pressedCodes, source);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest,
       NWebInputHandlerTest_WebSendTouchpadFlingEvent_001) {
  double x = 1.0;
  double y = 2.0;
  double vx = 3.0;
  double vy = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              WebSendTouchpadFlingEvent(x, y, vx, vy, pressedCodes))
      .Times(0);

  input_handler_->WebSendTouchpadFlingEvent(x, y, vx, vy, pressedCodes);
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest,
       NWebInputHandlerTest_WebSendTouchpadFlingEvent_002) {
  double x = 1.0;
  double y = 2.0;
  double vx = 3.0;
  double vy = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  EXPECT_CALL(*mock_delegate_,
              WebSendTouchpadFlingEvent(x, y, vx, vy, pressedCodes))
      .Times(1);

  input_handler_->WebSendTouchpadFlingEvent(x, y, vx, vy, pressedCodes);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest,
       NWebInputHandlerTest_WebSendCancelFlingEvent_001) {
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, WebSendCancelFlingEvent())
      .Times(0);

  input_handler_->WebSendCancelFlingEvent();
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest,
       NWebInputHandlerTest_WebSendCancelFlingEvent_002) {
  EXPECT_CALL(*mock_delegate_, WebSendCancelFlingEvent())
      .Times(1);

  input_handler_->WebSendCancelFlingEvent();
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_WebSendMouseEvent_001) {
  std::shared_ptr<OHOS::NWeb::NWebMouseEvent> mouseEvent = std::make_shared<MockNWebMouseEvent>();
  EXPECT_CALL(*mock_delegate_, WebSendMouseEvent(mouseEvent))
      .Times(0);
  input_handler_->nweb_delegate_ = nullptr;

  input_handler_->WebSendMouseEvent(mouseEvent);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_WebSendMouseEvent_002) {
  std::shared_ptr<OHOS::NWeb::NWebMouseEvent> mouseEvent = std::make_shared<MockNWebMouseEvent>();
  EXPECT_CALL(*mock_delegate_, WebSendMouseEvent(mouseEvent))
      .Times(1);

  input_handler_->WebSendMouseEvent(mouseEvent);
}
#endif

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendKeyEvent_001) {
  int32_t keyCode = 1;
  int32_t keyAction = 1;
  ON_CALL(*mock_delegate_, SendKeyEvent(keyCode, keyAction))
      .WillByDefault(::testing::Return(true));
  input_handler_->nweb_delegate_ = nullptr;

  auto ret = input_handler_->SendKeyEvent(keyCode, keyAction);
  EXPECT_EQ(ret, false);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendKeyEvent_002) {
  int32_t keyCode = 1;
  int32_t keyAction = 1;
  EXPECT_CALL(*mock_delegate_, SendKeyEvent(keyCode, keyAction))
      .WillOnce(::testing::Return(true));

  auto ret = input_handler_->SendKeyEvent(keyCode, keyAction);
  EXPECT_EQ(ret, true);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendTouchpadFlingEvent_001) {
  double x = 1.0;
  double y = 2.0;
  double vx = 3.0;
  double vy = 4.0;
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SendTouchpadFlingEvent(x, y, vx, vy)).Times(0);

  input_handler_->SendTouchpadFlingEvent(x, y, vx, vy);
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendTouchpadFlingEvent_002) {
  double x = 1.0;
  double y = 2.0;
  double vx = 3.0;
  double vy = 4.0;
  EXPECT_CALL(*mock_delegate_, SendTouchpadFlingEvent(x, y, vx, vy)).Times(1);

  input_handler_->SendTouchpadFlingEvent(x, y, vx, vy);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendKeyboardEvent_001) {
  std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent> keyboardEvent = std::make_shared<MockNWebKeyboardEvent>();
  EXPECT_CALL(*mock_delegate_, SendKeyboardEvent(keyboardEvent))
      .Times(0);
  input_handler_->nweb_delegate_ = nullptr;

  input_handler_->SendKeyboardEvent(keyboardEvent);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendKeyboardEvent_002) {
  std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent> keyboardEvent = std::make_shared<MockNWebKeyboardEvent>();
  EXPECT_CALL(*mock_delegate_, SendKeyboardEvent(keyboardEvent))
      .Times(1);

  input_handler_->SendKeyboardEvent(keyboardEvent);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendMouseWheelEvent_001) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SendMouseWheelEvent(x, y, deltaX, deltaY))
      .Times(0);

  input_handler_->SendMouseWheelEvent(x, y, deltaX, deltaY);
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendMouseWheelEvent_002) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  EXPECT_CALL(*mock_delegate_, SendMouseWheelEvent(x, y, deltaX, deltaY))
      .Times(1);

  input_handler_->SendMouseWheelEvent(x, y, deltaX, deltaY);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendMouseEvent_001) {
  int x = 1;
  int y = 2;
  int button = 3;
  int action = 4;
  int count = 5;
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SendMouseEvent(x, y, button, action, count))
      .Times(0);

  input_handler_->SendMouseEvent(x, y, button, action, count);
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_SendMouseEvent_002) {
  int x = 1;
  int y = 2;
  int button = 3;
  int action = 4;
  int count = 5;
  EXPECT_CALL(*mock_delegate_, SendMouseEvent(x, y, button, action, count))
      .Times(1);

  input_handler_->SendMouseEvent(x, y, button, action, count);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_001) {
  int16_t start_x = 1;
  int16_t end_x = 2;
  input_handler_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_EQ(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_002) {
  int16_t start_x = 1;
  int16_t end_x = 200;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed())
      .WillOnce(::testing::Return(true));
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(1);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_003) {
  int16_t start_x = 1;
  int16_t end_x = 200;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed())
      .WillOnce(::testing::Return(false));
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_004) {
  int16_t start_x = 1;
  int16_t end_x = 10;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_005) {
  int16_t start_x = 11;
  int16_t end_x = 200;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_006) {
  int16_t start_x = 11;
  int16_t end_x = 12;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_007) {
  int16_t start_x = 3000;
  int16_t end_x = 2000;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed())
      .WillOnce(::testing::Return(true));
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(1);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_008) {
  int16_t start_x = 3000;
  int16_t end_x = 2000;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed())
      .WillOnce(::testing::Return(false));
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_009) {
  int16_t start_x = 3000;
  int16_t end_x = 2950;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_010) {
  int16_t start_x = 2500;
  int16_t end_x = 2000;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, NWebInputHandlerTest_CheckSlideNavigation_011) {
  int16_t start_x = 2500;
  int16_t end_x = 2450;

  EXPECT_CALL(*mock_delegate_, IsNavigatebackwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateBack()).Times(0);
  EXPECT_CALL(*mock_delegate_, IsNavigateForwardAllowed()).Times(0);
  EXPECT_CALL(*mock_delegate_, NavigateForward()).Times(0);

  input_handler_->CheckSlideNavigation(start_x, end_x);
  EXPECT_NE(input_handler_->nweb_delegate_, nullptr);
}

TEST_F(NWebInputHandlerTest, OnStylusTouchPress001) {
  input_handler_->OnStylusTouchPress(nullptr, true);

  std::shared_ptr<MockNWebStylusTouchPointInfo> touch_point =
      std::make_shared<MockNWebStylusTouchPointInfo>();
  touch_point->SetX(1); // 1:value of x
  input_handler_->OnStylusTouchPress(touch_point, true);

  input_handler_->nweb_delegate_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(input_handler_->OnStylusTouchPress(nullptr, true));
}

TEST_F(NWebInputHandlerTest, OnStylusTouchRelease001) {
  input_handler_->OnStylusTouchRelease(nullptr, true);
  std::shared_ptr<MockNWebStylusTouchPointInfo> touch_point =
      std::make_shared<MockNWebStylusTouchPointInfo>();
  touch_point->SetX(0); // 0:value of x
  touch_point->SetY(0); // 0:value of y
  input_handler_->OnStylusTouchRelease(touch_point, true);

  touch_point->SetX(1); // 1:value of x
  touch_point->SetY(0); // 0:value of y
  input_handler_->OnStylusTouchRelease(touch_point, true);

  touch_point->SetX(0); // 0:value of x
  touch_point->SetY(1); // 1:value of y
  input_handler_->OnStylusTouchRelease(touch_point, true);

  input_handler_->nweb_delegate_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(input_handler_->OnStylusTouchRelease(nullptr, true));
}

TEST_F(NWebInputHandlerTest, OnStylusTouchMove001) {
  const std::vector<std::shared_ptr<NWebStylusTouchPointInfo>> touch_point_infos_one;
  ASSERT_NO_FATAL_FAILURE(input_handler_->OnStylusTouchMove(touch_point_infos_one, true));

  std::shared_ptr<MockNWebStylusTouchPointInfo> touch_point =
      std::make_shared<MockNWebStylusTouchPointInfo>();
  touch_point->SetX(1); // 1:value of x
  const std::vector<std::shared_ptr<NWebStylusTouchPointInfo>> touch_point_infos_two{touch_point};
  ASSERT_NO_FATAL_FAILURE(input_handler_->OnStylusTouchMove(touch_point_infos_two, true));

  input_handler_->nweb_delegate_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(input_handler_->OnStylusTouchMove(touch_point_infos_two, true));
}

}  // namespace OHOS::NWeb
