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

#include "nweb_delegate.h"

#include "arkweb/build/features/features.h"
#include "nweb.h"
#include "nweb_delegate_adapter.h"
#include "nweb_delegate_interface.h"
#include "nweb_errors.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::AtMost;
using ::testing::Invoke;
using ::testing::Return;

namespace OHOS::NWeb {

namespace {

class MockNWebTouchPointInfo : public NWebTouchPointInfo {
 public:
  MockNWebTouchPointInfo() = default;
  ~MockNWebTouchPointInfo() {}
  MockNWebTouchPointInfo(const MockNWebTouchPointInfo&) = delete;
  MockNWebTouchPointInfo& operator=(const MockNWebTouchPointInfo&) = delete;

  MOCK_METHOD(int, GetId, (), (override));
  MOCK_METHOD(double, GetX, (), (override));
  MOCK_METHOD(double, GetY, (), (override));
};

class NWebDelegateTest : public ::testing::Test {
 public:
  NWebDelegateTest() = default;

 protected:
  void ReleaseArgs() {
    argc_ = 0;
    if (argv_) {
      delete[] argv_;
      argv_ = NULL;
    }
  }

  void CreateDelegate(const std::list<std::string>& args_list) {
    ReleaseArgs();
    argc_ = args_list.size();
    if (argc_ > 0) {
      int i = 0;
      argv_ = new char*[argc_];
      for (auto it = args_list.begin(); i < argc_; ++i, ++it) {
        argv_[i] = (char*)it->c_str();
      }
    }
    nweb_delegate_ = std::make_shared<NWebDelegate>(argc_, (const char**)argv_);
  }

  void SetUp() override {
    web_engine_args_.emplace_back("--no-sandbox");
    CreateDelegate(web_engine_args_);
  }

  void TearDown() override { ReleaseArgs(); }

 private:
  int argc_ = 0;
  char** argv_ = NULL;
  std::list<std::string> web_engine_args_;

 public:
  std::shared_ptr<NWebDelegate> nweb_delegate_ = nullptr;
};

}  // namespace

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
TEST_F(NWebDelegateTest, SetDrawRect) {
  ASSERT_NE(nweb_delegate_, nullptr);
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
  nweb_delegate_->SetDrawRect(x, y, width, height);
}
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
TEST_F(NWebDelegateTest, WebPageSnapshot) {
  ASSERT_NE(nweb_delegate_, nullptr);

  const char* id = "test_id";
  int width = 1024;
  int height = 768;
  bool result = false;
  result = nweb_delegate_->WebPageSnapshot(
      id, PixelUnit::PX, width, height,
      [](const char* str, bool is, float flo, void* ptr, int a, int b) {});
  EXPECT_FALSE(result);
}
#endif  // BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)

TEST_F(NWebDelegateTest, Resize) {
  ASSERT_NE(nweb_delegate_, nullptr);
  uint32_t width = 100;
  uint32_t height = 100;
  bool isKeyboard = false;
  nweb_delegate_->Resize(width, height, isKeyboard);
}

TEST_F(NWebDelegateTest, SetIsOfflineWebComponent) {
  ASSERT_NE(nweb_delegate_, nullptr);
  nweb_delegate_->SetIsOfflineWebComponent();
}

TEST_F(NWebDelegateTest, OnTouchPress) {
  ASSERT_NE(nweb_delegate_, nullptr);
  int32_t id = 1;
  double x = 100.0;
  double y = 100.0;
  bool from_overlay = false;
  nweb_delegate_->OnTouchPress(id, x, y, from_overlay);
}

TEST_F(NWebDelegateTest, OnTouchRelease) {
  ASSERT_NE(nweb_delegate_, nullptr);
  int32_t id = 1;
  double x = 100.0;
  double y = 100.0;
  bool from_overlay = false;
  nweb_delegate_->OnTouchRelease(id, x, y, from_overlay);
}

TEST_F(NWebDelegateTest, OnTouchMove1) {
  ASSERT_NE(nweb_delegate_, nullptr);
  int32_t id = 1;
  double x = 100.0;
  double y = 100.0;
  bool from_overlay = false;
  nweb_delegate_->OnTouchMove(id, x, y, from_overlay);
}

TEST_F(NWebDelegateTest, OnTouchMove2) {
  ASSERT_NE(nweb_delegate_, nullptr);
  std::vector<std::shared_ptr<NWebTouchPointInfo>> points;
  points.push_back(std::make_shared<MockNWebTouchPointInfo>());
  bool from_overlay = false;
  nweb_delegate_->OnTouchMove(points, from_overlay);
}

TEST_F(NWebDelegateTest, OnTouchCancel) {
  ASSERT_NE(nweb_delegate_, nullptr);
  nweb_delegate_->OnTouchCancel();
}

TEST_F(NWebDelegateTest, OnTouchCancelById) {
  ASSERT_NE(nweb_delegate_, nullptr);
  int32_t id = 1;
  double x = 100.0;
  double y = 100.0;
  bool from_overlay = false;
  nweb_delegate_->OnTouchCancelById(id, x, y, from_overlay);
}

TEST_F(NWebDelegateTest, SendKeyEvent) {
  ASSERT_NE(nweb_delegate_, nullptr);
  int32_t keyCode = 2024;
  int32_t keyAction = 0;
  bool result = nweb_delegate_->SendKeyEvent(keyCode, keyAction);
  EXPECT_FALSE(result);
}

TEST_F(NWebDelegateTest, SendTouchpadFlingEvent) {
  ASSERT_NE(nweb_delegate_, nullptr);
  double x = 100.0;
  double y = 100.0;
  double vx = 10.0;
  double vy = 10.0;
  nweb_delegate_->SendTouchpadFlingEvent(x, y, vx, vy);
}

TEST_F(NWebDelegateTest, SendMouseWheelEvent) {
  ASSERT_NE(nweb_delegate_, nullptr);
  double x = 100.0;
  double y = 100.0;
  double deltaX = 10.0;
  double deltaY = 10.0;
  nweb_delegate_->SendMouseWheelEvent(x, y, deltaX, deltaY);
}

TEST_F(NWebDelegateTest, SendMouseEvent) {
  ASSERT_NE(nweb_delegate_, nullptr);
  int x = 100;
  int y = 100;
  int button = 1;
  int action = 1;
  int count = 1;
  nweb_delegate_->SendMouseEvent(x, y, button, action, count);
}

TEST_F(NWebDelegateTest, Zoom) {
  ASSERT_NE(nweb_delegate_, nullptr);
  float zoomFactor = 1.0f;
  int result = nweb_delegate_->Zoom(zoomFactor);
  EXPECT_EQ(result, NWEB_ERR);
}

TEST_F(NWebDelegateTest, ZoomIn) {
  ASSERT_NE(nweb_delegate_, nullptr);
  int result = nweb_delegate_->ZoomIn();
  EXPECT_EQ(result, NWEB_ERR);
}

TEST_F(NWebDelegateTest, ZoomOut) {
  ASSERT_NE(nweb_delegate_, nullptr);
  int result = nweb_delegate_->ZoomOut();
  EXPECT_EQ(result, NWEB_ERR);
}

TEST_F(NWebDelegateTest, SetZoomInFactor) {
  ASSERT_NE(nweb_delegate_, nullptr);
  float factor = 1.0f;
  bool result = nweb_delegate_->SetZoomInFactor(factor);
  EXPECT_TRUE(result);
  factor = -1.0f;
  result = nweb_delegate_->SetZoomInFactor(factor);
  EXPECT_FALSE(result);
}

TEST_F(NWebDelegateTest, SetZoomOutFactor) {
  ASSERT_NE(nweb_delegate_, nullptr);
  float factor = 1.0f;
  bool result = nweb_delegate_->SetZoomOutFactor(factor);
  EXPECT_FALSE(result);
  factor = -1.0f;
  result = nweb_delegate_->SetZoomOutFactor(factor);
  EXPECT_TRUE(result);
}

TEST_F(NWebDelegateTest, InitialScale) {
  ASSERT_NE(nweb_delegate_, nullptr);
  float scale = 0;
  nweb_delegate_->InitialScale(scale);
}

TEST_F(NWebDelegateTest, OnFocus) {
  ASSERT_NE(nweb_delegate_, nullptr);
  const FocusReason focusReason = FocusReason::FOCUS_DEFAULT;
  bool result = nweb_delegate_->OnFocus(focusReason);
  EXPECT_FALSE(result);
}

TEST_F(NWebDelegateTest, OnBlur) {
  ASSERT_NE(nweb_delegate_, nullptr);
  nweb_delegate_->OnBlur();
}

TEST_F(NWebDelegateTest, SendDragEvent) {}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(NWebDelegateTest, ScrollTo) {
  ASSERT_NE(nweb_delegate_, nullptr);
  float x = 100.0f;
  float y = 100.0f;
  nweb_delegate_->ScrollTo(x, y);
}

TEST_F(NWebDelegateTest, ScrollBy) {
  ASSERT_NE(nweb_delegate_, nullptr);
  float delta_x = 10.0f;
  float delta_y = 10.0f;
  nweb_delegate_->ScrollBy(delta_x, delta_y);
}

TEST_F(NWebDelegateTest, ScrollByRefScreen) {
  ASSERT_NE(nweb_delegate_, nullptr);
  float delta_x = 10.0f;
  float delta_y = 10.0f;
  float vx = 10.0f;
  float vy = 10.0f;
  nweb_delegate_->ScrollByRefScreen(delta_x, delta_y, vx, vy);
}

TEST_F(NWebDelegateTest, SlideScroll) {
  ASSERT_NE(nweb_delegate_, nullptr);
  float vx = 10.0f;
  float vy = 10.0f;
  nweb_delegate_->SlideScroll(vx, vy);
}

TEST_F(NWebDelegateTest, WebSendKeyEvent) {
  ASSERT_NE(nweb_delegate_, nullptr);
  std::vector<int32_t> codes;
  codes.push_back(2048);
  int32_t keyCode = 2024;
  int32_t keyAction = 0;
  bool result = nweb_delegate_->WebSendKeyEvent(keyCode, keyAction, codes);
  EXPECT_FALSE(result);
}

TEST_F(NWebDelegateTest, WebSendMouseWheelEvent) {
  ASSERT_NE(nweb_delegate_, nullptr);
  double x = 100.0;
  double y = 100.0;
  double deltaX = 10.0;
  double deltaY = 10.0;
  std::vector<int32_t> pressedCodes;
  pressedCodes.push_back(2048);
  nweb_delegate_->WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes);
}

TEST_F(NWebDelegateTest, WebSendTouchpadFlingEvent) {
  ASSERT_NE(nweb_delegate_, nullptr);
  double x = 100.0;
  double y = 100.0;
  double vx = 10.0;
  double vy = 10.0;
  std::vector<int32_t> pressedCodes;
  pressedCodes.push_back(2048);
  nweb_delegate_->WebSendTouchpadFlingEvent(x, y, vx, vy, pressedCodes);
}

TEST_F(NWebDelegateTest, WebSendCancelFlingEvent) {
  ASSERT_NE(nweb_delegate_, nullptr);
  nweb_delegate_->WebSendCancelFlingEvent();
}

TEST_F(NWebDelegateTest, SetVirtualKeyBoardArg) {
  ASSERT_NE(nweb_delegate_, nullptr);
  int32_t width = 100;
  int32_t height = 100;
  double keyboard = 10.0;
  nweb_delegate_->SetVirtualKeyBoardArg(width, height, keyboard);
}

TEST_F(NWebDelegateTest, ShouldVirtualKeyboardOverlay) {
  ASSERT_NE(nweb_delegate_, nullptr);
  bool result = nweb_delegate_->ShouldVirtualKeyboardOverlay();
  EXPECT_FALSE(result);
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_CLIPBOARD)
TEST_F(NWebDelegateTest, SetIsRichText) {
  ASSERT_NE(nweb_delegate_, nullptr);
  nweb_delegate_->SetIsRichText(false);
}

TEST_F(NWebDelegateTest, GetSelectInfo) {
  ASSERT_NE(nweb_delegate_, nullptr);
  std::string result = nweb_delegate_->GetSelectInfo();
  EXPECT_TRUE(result.empty());
}
#endif

#if BUILDFLAG(ARKWEB_AI)
TEST_F(NWebDelegateTest, OnTextSelected) {
  ASSERT_NE(nweb_delegate_, nullptr);
  nweb_delegate_->OnTextSelected();
}

TEST_F(NWebDelegateTest, RegisterOnLoadStartedCbForHighlightContent_001) {
  ASSERT_NE(nweb_delegate_, nullptr);
  std::function<void(void)> callback = []() {};
  nweb_delegate_->handler_delegate_ = nullptr;
  nweb_delegate_->RegisterOnLoadStartedCbForHighlightContent(std::move(callback));
}

TEST_F(NWebDelegateTest, RegisterOnLoadStartedCbForHighlightContent_002) {
  ASSERT_NE(nweb_delegate_, nullptr);
  std::function<void(void)> callback = []() {};
  nweb_delegate_->RegisterOnLoadStartedCbForHighlightContent(std::move(callback));
}
#endif

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
TEST_F(NWebDelegateTest, SetFocusWebId) {
  ASSERT_NE(nweb_delegate_, nullptr);
  int32_t nweb_id = 1;
  nweb_delegate_->SetFocusWebId(nweb_id);
}
#endif

#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
TEST_F(NWebDelegateTest, SetScreenOffset) {
  ASSERT_NE(nweb_delegate_, nullptr);
  double x = 20.0;
  double y = 30.0;
  nweb_delegate_->SetScreenOffset(x, y);
}
#endif

}  // namespace OHOS::NWeb
