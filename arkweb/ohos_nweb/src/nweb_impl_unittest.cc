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
#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <map>
#include <string>
#include <thread>

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#include "nweb.h"
#include "capi/nweb_download_delegate_callback.h"
#define private public
#include "nweb_impl.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

using namespace testing;
using namespace OHOS::NWeb;

namespace OHOS::NWeb {
class MockNWebDragEvent : public NWebDragEvent {
 public:
  ~MockNWebDragEvent() = default;
  MOCK_METHOD(DragAction, GetAction, (), (override));
  MOCK_METHOD(double, GetX, (), (override));
  MOCK_METHOD(double, GetY, (), (override));
};

class MockNWebInputHandler : public NWebInputHandler {
 public:
  MockNWebInputHandler(std::shared_ptr<NWebDelegateInterface> nweb_delegate)
      : NWebInputHandler(nweb_delegate) {}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  MOCK_METHOD3(WebSendKeyEvent,
               bool(int32_t, int32_t, const std::vector<int32_t>&));
  MOCK_METHOD5(
      WebSendMouseWheelEvent,
      void(double, double, double, double, const std::vector<int32_t>&));
  MOCK_METHOD5(
      WebSendTouchpadFlingEvent,
      void(double, double, double, double, const std::vector<int32_t>&));
#endif
};

class MockNWebDragData : public NWebDragData {
 public:
  MOCK_METHOD(std::string, GetLinkURL, (), (override));
  MOCK_METHOD(std::string, GetFragmentText, (), (override));
  MOCK_METHOD(std::string, GetFragmentHtml, (), (override));
  MOCK_METHOD(bool,
              GetPixelMapSetting,
              (const void** data, size_t& len, int& width, int& height),
              (override));
  MOCK_METHOD(bool, SetFragmentHtml, (const std::string& html), (override));
  MOCK_METHOD(bool,
              SetPixelMapSetting,
              (const void* data, size_t len, int width, int height),
              (override));
  MOCK_METHOD(bool, SetLinkURL, (const std::string& url), (override));
  MOCK_METHOD(bool, SetFragmentText, (const std::string& Text), (override));
  MOCK_METHOD(std::string, GetLinkTitle, (), (override));
  MOCK_METHOD(bool, SetLinkTitle, (const std::string& title), (override));
  MOCK_METHOD(void, GetDragStartPosition, (int& x, int& y), (override));
  MOCK_METHOD(bool, IsSingleImageContent, (), (override));
  MOCK_METHOD(bool, SetFileUri, (const std::string& uri), (override));
  MOCK_METHOD(std::string, GetImageFileName, (), (override));
  MOCK_METHOD(void, ClearImageFileNames, (), (override));
};

class MockNWebInputMethodHandler : public NWebInputMethodHandler {
 public:
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  MOCK_METHOD0(GetSelectInfo, std::string());
#endif
};

class MockNWebCreateInfo : public NWebCreateInfo {
 public:
  ~MockNWebCreateInfo() = default;
  MOCK_METHOD(uint32_t, GetWidth, (), (override));
  MOCK_METHOD(uint32_t, GetHeight, (), (override));
  MOCK_METHOD(std::shared_ptr<NWebOutputFrameCallback>,
              GetOutputFrameCallback,
              (),
              (override));
  MOCK_METHOD(std::shared_ptr<NWebEngineInitArgs>,
              GetEngineInitArgs,
              (),
              (override));
  MOCK_METHOD(void*, GetProducerSurface, (), (override));
  MOCK_METHOD(void*, GetEnhanceSurfaceInfo, (), (override));
  MOCK_METHOD(bool, GetIsIncognitoMode, (), (override));
};

class MockNWebAccessRequest : public NWebAccessRequest {
  public:
   ~MockNWebAccessRequest() = default;
   MOCK_METHOD(std::string, Origin, (), (override));
   MOCK_METHOD(int, ResourceAcessId, (), (override));
   MOCK_METHOD(void, Agree, (int), (override));
   MOCK_METHOD(void, Refuse, (), (override));
}

class NWebImplTest : public ::testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp(void);
  void TearDown(void);
  int32_t id = 1;
  std::shared_ptr<NWebImpl> nweb_impl_;
  std::shared_ptr<MockNWebDelegate> mock_delegate_;
  std::shared_ptr<MockNWebInputHandler> input_handler_;
  static bool MockFrameCallback(const char*, uint32_t, uint32_t) { return true; }
};

void NWebImplTest::SetUpTestCase(void) {}

void NWebImplTest::TearDownTestCase(void) {}

void NWebImplTest::SetUp() {
  mock_delegate_ = std::make_shared<MockNWebDelegate>();
  nweb_impl_ = std::make_shared<NWebImpl>(id++);
  ASSERT_NE(nweb_impl_, nullptr);
}

void NWebImplTest::TearDown() {
  nweb_impl_ = nullptr;
}

TEST_F(NWebImplTest, NWebImplTest_InitialScale_001) {
  float scale = 1.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, InitialScale(scale)).Times(1);

  nweb_impl_->InitialScale(scale);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_InitialScale_002) {
  float scale = 1.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, InitialScale(scale)).Times(0);

  nweb_impl_->InitialScale(scale);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_Scale_001) {
  float scale = 1.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, Scale()).WillOnce(::testing::Return(scale));

  auto result = nweb_impl_->Scale();
  EXPECT_EQ(result, scale);
}

TEST_F(NWebImplTest, NWebImplTest_Scale_002) {
  float scale = 1.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, InitialScale(scale)).Times(0);

  auto result = nweb_impl_->Scale();
  EXPECT_EQ(result, 0);
}

TEST_F(NWebImplTest, NWebImplTest_OnFocus_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, IsCustomKeyboard()).Times(0);

  nweb_impl_->OnFocus(OHOS::NWeb::FocusReason::FOCUS_DEFAULT);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_OnBlur_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnBlur()).Times(0);

  nweb_impl_->OnBlur(OHOS::NWeb::BlurReason::FOCUS_SWITCH);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SendDragEvent_001) {
  auto dragEvent = std::make_shared<MockNWebDragEvent>();
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*dragEvent, GetAction()).Times(0);
  EXPECT_CALL(*dragEvent, GetX()).Times(0);
  EXPECT_CALL(*dragEvent, GetY()).Times(0);
  EXPECT_CALL(*mock_delegate_, SendDragEvent(::testing::_)).Times(0);

  nweb_impl_->SendDragEvent(dragEvent);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(NWebImplTest, NWebImplTest_ScrollTo_001) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ScrollTo(::testing::_, ::testing::_)).Times(0);

  nweb_impl_->ScrollTo(x, y);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollTo_002) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ScrollTo(::testing::_, ::testing::_)).Times(1);

  nweb_impl_->ScrollTo(x, y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollBy_001) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ScrollBy(::testing::_, ::testing::_)).Times(0);

  nweb_impl_->ScrollBy(x, y);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollBy_002) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ScrollBy(::testing::_, ::testing::_)).Times(1);

  nweb_impl_->ScrollBy(x, y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SlideScroll_001) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SlideScroll(::testing::_, ::testing::_))
      .Times(0);

  nweb_impl_->SlideScroll(x, y);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SlideScroll_002) {
  float x = 1.5f;
  float y = 2.5f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SlideScroll(::testing::_, ::testing::_))
      .Times(1);

  nweb_impl_->SlideScroll(x, y);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_WebSendKeyEvent_001) {
  int32_t keyCode = 1;
  int32_t keyAction = 1;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  ON_CALL(*input_handler_, WebSendKeyEvent(keyCode, keyAction, pressedCodes))
      .WillByDefault(::testing::Return(true));
  nweb_impl_->input_handler_ = nullptr;

  auto ret = nweb_impl_->WebSendKeyEvent(keyCode, keyAction, pressedCodes);
  EXPECT_EQ(ret, false);
}

TEST_F(NWebImplTest, NWebImplTest_WebSendMouseWheelEvent_001) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  nweb_impl_->input_handler_ = nullptr;
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  EXPECT_CALL(*input_handler_,
              WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes))
      .Times(0);

  nweb_impl_->WebSendMouseWheelEvent(x, y, deltaX, deltaY, pressedCodes);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_WebSendTouchpadFlingEvent_001) {
  double x = 1.0;
  double y = 2.0;
  double deltaX = 3.0;
  double deltaY = 4.0;
  std::vector<int32_t> pressedCodes = {1, 2, 3};
  nweb_impl_->input_handler_ = nullptr;
  input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  EXPECT_CALL(*input_handler_,
              WebSendTouchpadFlingEvent(x, y, deltaX, deltaY, pressedCodes))
      .Times(0);

  nweb_impl_->WebSendTouchpadFlingEvent(x, y, deltaX, deltaY, pressedCodes);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollToWithAnime_001) {
  float x = 1.5f;
  float y = 2.5f;
  int32_t duration_ = 1000;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              ScrollToWithAnime(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  nweb_impl_->ScrollToWithAnime(x, y, duration_);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollToWithAnime_002) {
  float x = 1.5f;
  float y = 2.5f;
  int32_t duration_ = 1000;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_,
              ScrollToWithAnime(::testing::_, ::testing::_, ::testing::_))
      .Times(1);

  nweb_impl_->ScrollToWithAnime(x, y, duration_);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollByWithAnime_001) {
  float x = 1.5f;
  float y = 2.5f;
  int32_t duration_ = 1000;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              ScrollByWithAnime(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  nweb_impl_->ScrollByWithAnime(x, y, duration_);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ScrollByWithAnime_002) {
  float x = 1.5f;
  float y = 2.5f;
  int32_t duration_ = 1000;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_,
              ScrollByWithAnime(::testing::_, ::testing::_, ::testing::_))
      .Times(1);

  nweb_impl_->ScrollByWithAnime(x, y, duration_);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif

TEST_F(NWebImplTest, NWebImplTest_NotifyForNextTouchEvent_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, NotifyForNextTouchEvent()).Times(0);

  nweb_impl_->NotifyForNextTouchEvent();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_NotifyForNextTouchEvent_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, NotifyForNextTouchEvent()).Times(1);

  nweb_impl_->NotifyForNextTouchEvent();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SetNestedScrollMode_001) {
  auto ret = OHOS::NWeb::NestedScrollMode::SELF_ONLY;
  nweb_impl_->SetNestedScrollMode(ret);
  EXPECT_NE(nweb_impl_, nullptr);
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(NWebImplTest, NWebImplTest_SetVirtualKeyBoardArg_001) {
  int32_t width = 1;
  int32_t height = 2;
  double keyboard = 1.1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              SetVirtualKeyBoardArg(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  nweb_impl_->SetVirtualKeyBoardArg(width, height, keyboard);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_SetVirtualKeyBoardArg_002) {
  int32_t width = 1;
  int32_t height = 2;
  double keyboard = 1.1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_,
              SetVirtualKeyBoardArg(::testing::_, ::testing::_, ::testing::_))
      .Times(1);

  nweb_impl_->SetVirtualKeyBoardArg(width, height, keyboard);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ShouldVirtualKeyboardOverlay_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, ShouldVirtualKeyboardOverlay()).Times(0);

  nweb_impl_->ShouldVirtualKeyboardOverlay();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_ShouldVirtualKeyboardOverlay_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ShouldVirtualKeyboardOverlay()).Times(1);

  nweb_impl_->ShouldVirtualKeyboardOverlay();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
TEST_F(NWebImplTest, NWebImplTest_GetOrCreateDragData_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  auto drag_data_mock = std::make_shared<MockNWebDragData>();
  ON_CALL(*mock_delegate_, GetOrCreateDragData())
      .WillByDefault(::testing::Return(drag_data_mock));

  auto ret = nweb_impl_->GetOrCreateDragData();
  EXPECT_EQ(ret, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_GetOrCreateDragData_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto drag_data_mock = std::make_shared<MockNWebDragData>();
  ON_CALL(*mock_delegate_, GetOrCreateDragData())
      .WillByDefault(::testing::Return(drag_data_mock));

  auto ret = nweb_impl_->GetOrCreateDragData();
  EXPECT_EQ(ret, drag_data_mock);
}
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
TEST_F(NWebImplTest, NWebImplTest_GetSelectInfo_001) {
  std::string select_info = "test";
  std::string select_info_empty = "";
  MockNWebInputMethodHandler input_method_handler;
  ON_CALL(input_method_handler, GetSelectInfo())
      .WillByDefault(::testing::Return(select_info));
  nweb_impl_->inputmethod_handler_ = nullptr;

  auto ret = nweb_impl_->GetSelectInfo();
  EXPECT_EQ(ret, select_info_empty);
}
#endif

TEST_F(NWebImplTest, NWebImplTest_ScaleGestureChange_001) {
  int temp = -1;
  double scale = 1;
  double centerX = 1.5;
  double centerY = 2.5;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_,
              ScaleGestureChange(::testing::_, ::testing::_, ::testing::_))
      .Times(0);

  auto ret = nweb_impl_->ScaleGestureChange(scale, centerX, centerY);
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, NWebImplTest_ScaleGestureChange_002) {
  int temp = 1;
  double scale = 1;
  double centerX = 1.5;
  double centerY = 2.5;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_,
              ScaleGestureChange(::testing::_, ::testing::_, ::testing::_))
      .WillOnce(::testing::Return(temp));

  auto ret = nweb_impl_->ScaleGestureChange(scale, centerX, centerY);
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, NWebImplTest_OnTextSelected_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnTextSelected()).Times(0);

  nweb_impl_->OnTextSelected();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NWebImplTest_OnTextSelected_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnTextSelected()).Times(1);

  nweb_impl_->OnTextSelected();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebPageSnapshot) {
  const char* id = "test_id";
  int width = 1024;
  int height = 768;
  bool result = false;

  result = nweb_impl_->WebPageSnapshot(
      id, PixelUnit::PX, width, height,
      [](const char* str, bool is, float flo, void* ptr, int a, int b) {});
}

TEST_F(NWebImplTest, OnOccludedCall_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnOccluded()).Times(0);

  nweb_impl_->OnOccluded();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnOccludedCall_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnOccluded()).Times(1);

  nweb_impl_->OnOccluded();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnUnoccludedCall_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnUnoccluded()).Times(0);

  nweb_impl_->OnUnoccluded();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnUnoccludedCall_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnUnoccluded()).Times(1);

  nweb_impl_->OnUnoccluded();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetEnableLowerFrameRateCall_001) {
  bool enabled = true;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, SetEnableLowerFrameRate(::testing::_)).Times(0);

  nweb_impl_->SetEnableLowerFrameRate(enabled);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetEnableLowerFrameRateCall_002) {
  bool enabled = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, SetEnableLowerFrameRate(::testing::_)).Times(1);

  nweb_impl_->SetEnableLowerFrameRate(enabled);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearPrefetchedResourceCall) {
  std::vector<std::string> cache_key_list = {"sdfslllllfds", "dfsfdsddddddd",
                                             "eeeefdsfffffffff"};
  nweb_impl_->ClearPrefetchedResource(cache_key_list);
}

TEST_F(NWebImplTest, OnRenderToBackgroundCall_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnWindowHide()).Times(0);

  nweb_impl_->OnRenderToBackground();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnRenderToBackgroundCall_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnWindowHide()).Times(1);

  nweb_impl_->OnRenderToBackground();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnRenderToForegroundCall_001) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_CALL(*mock_delegate_, OnWindowShow()).Times(0);

  nweb_impl_->OnRenderToForeground();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnRenderToForegroundCall_002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, OnWindowShow()).Times(1);

  nweb_impl_->OnRenderToForeground();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PrefetchResourceCall) {
  std::shared_ptr<NWebEnginePrefetchArgs> pre_args;
  std::map<std::string, std::string> additional_http_headers = {
      {"aaa", "bbb"}, {"ccc", "dddd"}, {"eee", "ffff"}};
  std::string cache_key = "aaaaaaaaadddddddfff";
  uint32_t cache_valid_time = 30;
  nweb_impl_->PrefetchResource(pre_args, additional_http_headers, cache_key,
                               cache_valid_time);
}
TEST_F(NWebImplTest, TestCreateNWebWithNullCreateInfo) {
  std::shared_ptr<MockNWebCreateInfo> nullCreateInfo = nullptr;
  auto result = NWebImpl::CreateNWeb(nullCreateInfo);
  EXPECT_EQ(result, nullptr);
}

TEST_F(NWebImplTest, TestGetNWebNonExistingId) {
  int32_t existingId = 1;
  auto result = NWebImpl::GetNWeb(existingId);
  EXPECT_EQ(result, nullptr);
}

TEST_F(NWebImplTest, OnWebviewHideWithNullInputHandler) {
  nweb_impl_->OnWebviewHide();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, OnWebviewShowWithNullInputHandler) {
  nweb_impl_->OnWebviewShow();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, TestOnTextSelected) {
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->OnTextSelected();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, ResizeWithNullInputHandler) {
  uint32_t width = 100;
  uint32_t height = 200;
  bool isKeyboard = true;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = nullptr;
  nweb_impl_->Resize(width, height, isKeyboard);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, OnTouchPressWithoutHandler) {
  nweb_impl_->input_handler_ = nullptr;
  int32_t id = 1;
  double x = 1.0;
  double y = 1.0;
  bool from_overlay = true;
  nweb_impl_->OnTouchPress(id, x, y, from_overlay);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, TestSetDrawRect) {
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->SetDrawRect(x, y, width, height);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, TestOnTouchMoveWithNullInputHandlerSingleParams) {
  nweb_impl_->input_handler_ = nullptr;
  int32_t id = 1;
  double x = 2.0;
  double y = 3.0;
  bool from_overlay = true;
  nweb_impl_->OnTouchMove(id, x, y, from_overlay);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, TestOnTouchMoveWithNullInputHandlerVectorParams) {
  nweb_impl_->input_handler_ = nullptr;
  std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos;
  bool from_overlay = true;
  nweb_impl_->OnTouchMove(touch_point_infos, from_overlay);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, OnTouchCancelnputHandler) {
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->OnTouchCancel();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, SendKeyEventWithNullInputHandler) {
  nweb_impl_->input_handler_ = nullptr;
  int32_t keyCode = 1;
  int32_t keyAction = 2;
  nweb_impl_->SendKeyEvent(keyCode, keyAction);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, SendTouchpadFlingEventWithNullInputHandler) {
  nweb_impl_->input_handler_ = nullptr;
  double x = 1.0;
  double y = 1.0;
  double vx = 1.0;
  double vy = 1.0;
  nweb_impl_->SendTouchpadFlingEvent(x, y, vx, vy);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, SendMouseWheelEventWithNullInputHandler) {
  nweb_impl_->input_handler_ = nullptr;
  double x = 1.0;
  double y = 1.0;
  double deltaX = 1.0;
  double deltaY = 1.0;
  nweb_impl_->SendMouseWheelEvent(x, y, deltaX, deltaY);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, SendMouseEventWithNullInputHandler) {
  nweb_impl_->input_handler_ = nullptr;
  int x = 1;
  int y = 1;
  int button = 1;
  int action = 1;
  int count = 1;
  nweb_impl_->SendMouseEvent(x, y, button, action, count);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, LoadWithNullInputHandler) {
  std::string url = "https://example.com";
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = nullptr;
  int ret = nweb_impl_->Load(url);
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, ZoomWithNullNwebDelegate) {
  float zoomFactor = 1.0;
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  int ret = nweb_impl_->Zoom(zoomFactor);
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, ZoomInWithNullNwebDelegate) {
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  int ret = nweb_impl_->ZoomIn();
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, ZoomOutWithNullNwebDelegate) {
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  int ret = nweb_impl_->ZoomOut();
  EXPECT_EQ(ret, temp);
}
TEST_F(NWebImplTest, GetRenderProcessModeWithNullInputHandler) {
  nweb_impl_->nweb_delegate_ = nullptr;
  auto temp = RenderProcessMode::SINGLE_MODE;
  auto ret = nweb_impl_->GetRenderProcessMode();
  EXPECT_EQ(ret, temp);
}

TEST_F(NWebImplTest, OnDestroyWithNullInitArgs) {
  nweb_impl_->destroyCallback_ = [](const char* str) {};
  nweb_impl_->OnDestroy();
  EXPECT_EQ(nweb_impl_->destroyCallback_, nullptr);
}

#if BUILDFLAG(ARKWEB_EXT_PERMISSION)
TEST_F(NWebImplTest, GetOrigin001) {
  NWebPermissionRequest* request = nullptr;
  auto res = nweb_impl_->GetOrigin(request);
  EXPECT_EQ(res, "");
}

TEST_F(NWebImplTest, GetOrigin002) {
  NWebPermissionRequest* request = new NWebPermissionRequest(0, nullptr);
  auto res = nweb_impl_->GetOrigin(request);
  EXPECT_EQ(res, "");
  delete request;
}

TEST_F(NWebImplTest, GetOrigin003) {
  auto temp = std::make_shared<MockNWebAccessRequest>();
  NWebPermissionRequest* request = new NWebPermissionRequest(0, temp);
  EXPECT_CALL(*temp, Origin())
    .WillOnce(::testing::Return("https://example.com"));
  auto res = nweb_impl_->GetOrigin(request);
  EXPECT_EQ(res, "https://example.com");
  delete request;
}

TEST_F(NWebImplTest, GetResouceId001) {
  NWebPermissionRequest* request = nullptr;
  auto res = nweb_impl_->GetResourceId(request);
  EXPECT_EQ(res, -1);
}

TEST_F(NWebImplTest, GetResouceId002) {
  NWebPermissionRequest* request = new NWebPermissionRequest(0, nullptr);
  auto res = nweb_impl_->GetResourceId(request);
  EXPECT_EQ(res, -1);
  delete request;
}

TEST_F(NWebImplTest, GetResouceId003) {
  auto temp = std::make_shared<MockNWebAccessRequest>();
  NWebPermissionRequest* request = new NWebPermissionRequest(0, temp);
  EXPECT_CALL(*temp, ResourceAcessId())
    .WillOnce(::testing::Return(999));
  auto res = nweb_impl_->GetResourceId(request);
  EXPECT_EQ(res, 999);
  delete request;
}

TEST_F(NWebImplTest, Grant001) {
  NWebPermissionRequest* request = nullptr;
  int32_t resourse_id = -1;
  nweb_impl_->Grant(request, resourse_id);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, Grant002) {
  NWebPermissionRequest* request = new NWebPermissionRequest(0, nullptr);
  int32_t resourse_id = -1;
  nweb_impl_->Grant(request, resourse_id);
  EXPECT_NE(nweb_impl_, nullptr);
  delete request;
}

TEST_F(NWebImplTest, Grant003) {
  auto temp = std::make_shared<MockNWebAccessRequest>();
  NWebPermissionRequest* request = new NWebPermissionRequest(0, temp);
  int32_t resourse_id = -1;
  EXPECT_CALL(*temp, Agree(resourse_id)).Times(1);
  nweb_impl_->Grant(request, resourse_id);
  EXPECT_NE(nweb_impl_, nullptr);
  delete request;
}

TEST_F(NWebImplTest, Deny001) {
  NWebPermissionRequest* request = nullptr;
  nweb_impl_->Deny(request);
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, Deny002) {
  NWebPermissionRequest* request = new NWebPermissionRequest(0, nullptr);
  nweb_impl_->Deny(request);
  EXPECT_NE(nweb_impl_, nullptr);
  delete request;
}

TEST_F(NWebImplTest, Deny003) {
  auto temp = std::make_shared<MockNWebAccessRequest>();
  NWebPermissionRequest* request = new NWebPermissionRequest(0, temp);
  EXPECT_CALL(*temp, Refuse()).Times(1);
  nweb_impl_->Deny(request);
  EXPECT_NE(nweb_impl_, nullptr);
  delete request;
}
#endif

TEST_F(NWebImplTest, OnDestroyWithNativeDestroyCallback) {
  nweb_impl_->destroyCallback_ = nullptr;
  EXPECT_EQ(nweb_impl_->destroyCallback_, nullptr);
  nweb_impl_->nativeDestroyCallback_ = []() {};
  EXPECT_NE(nweb_impl_->nativeDestroyCallback_, nullptr);
  nweb_impl_->OnDestroy();
}

TEST_F(NWebImplTest, OnDestroyWithNullNativeCallback) {
  nweb_impl_->destroyCallback_ = nullptr;
  EXPECT_EQ(nweb_impl_->destroyCallback_, nullptr);
  nweb_impl_->nativeDestroyCallback_ = nullptr;
  EXPECT_EQ(nweb_impl_->nativeDestroyCallback_, nullptr);
  nweb_impl_->OnDestroy();
}

TEST_F(NWebImplTest, TestCreateNWebWithCreateInfo) {
  std::shared_ptr<MockNWebCreateInfo> createInfo = std::make_shared<MockNWebCreateInfo>();
  auto result = NWebImpl::CreateNWeb(createInfo);
  EXPECT_NE(createInfo, nullptr);
}

TEST_F(NWebImplTest, AddNWebToMap001) {
  uint32_t id = 0;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
}

TEST_F(NWebImplTest, AddNWebToMap002) {
  uint32_t id = 0;
  std::shared_ptr<NWebImpl> nweb = nullptr;
  EXPECT_EQ(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
}

TEST_F(NWebImplTest, FromID001) {
  int32_t id = 0;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  auto result = NWebImpl::FromID(id);
  EXPECT_NE(result, nullptr);
}

TEST_F(NWebImplTest, FromID002) {
  int32_t id = 0;
  int32_t nweb_id = 2;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  auto result = NWebImpl::FromID(nweb_id);
  EXPECT_EQ(result, nullptr);
}

TEST_F(NWebImplTest, GetNWebSharedPtr001) {
  int32_t id = 0;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  auto result = NWebImpl::GetNWebSharedPtr(id);
  EXPECT_NE(result, nullptr);
}

TEST_F(NWebImplTest, GetNWebSharedPtr002) {
  int32_t id = 0;
  int32_t nweb_id = 2;
  std::shared_ptr<NWebImpl> nweb = std::make_shared<NWebImpl>(id);
  EXPECT_NE(nweb, nullptr);
  nweb->AddNWebToMap(id, nweb);
  auto result = NWebImpl::GetNWebSharedPtr(nweb_id);
  EXPECT_EQ(result, nullptr);
}

TEST_F(NWebImplTest, PutDownloadCallback001) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  std::shared_ptr<NWebDownloadCallback> downloadListener = nullptr;
  nweb_impl_->PutDownloadCallback(downloadListener);
}

TEST_F(NWebImplTest, PutDownloadCallback002) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  std::shared_ptr<NWebDownloadCallback> downloadListener = nullptr;
  nweb_impl_->PutDownloadCallback(downloadListener);
}

TEST_F(NWebImplTest, SetNWebHandler001) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  std::shared_ptr<NWebHandler> client = std::make_shared<NWebHandler>();
  nweb_impl_->SetNWebHandler(client);
  EXPECT_EQ(nweb_impl_->nweb_handle_, client);
}

TEST_F(NWebImplTest, SetNWebHandler002) {
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  std::shared_ptr<NWebHandler> client = std::make_shared<NWebHandler>();
  nweb_impl_->SetNWebHandler(client);
  EXPECT_NE(nweb_impl_->nweb_handle_, client);
}

TEST_F(NWebImplTest, Resize001) {
  uint32_t width = 100;
  uint32_t height = 200;
  bool isKeyboard = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->OnPause();
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = nullptr;
  nweb_impl_->Resize(width, height, isKeyboard);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Resize002) {
  uint32_t width = 100;
  uint32_t height = 200;
  bool isKeyboard = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
  nweb_impl_->output_handler_ = nullptr;
  EXPECT_EQ(nweb_impl_->output_handler_, nullptr);
  nweb_impl_->Resize(width, height, isKeyboard);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Resize003) {
  uint32_t width = 100;
  uint32_t height = 200;
  bool isKeyboard = true;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
  nweb_impl_->output_handler_ = std::make_shared<NWebOutputHandler>(MockFrameCallback);
  EXPECT_NE(nweb_impl_->output_handler_, nullptr);
  nweb_impl_->Resize(width, height, isKeyboard);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Resize004) {
  uint32_t width = 100;
  uint32_t height = 8000;
  bool isKeyboard = true;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
  nweb_impl_->output_handler_ = std::make_shared<NWebOutputHandler>(MockFrameCallback);
  EXPECT_NE(nweb_impl_->output_handler_, nullptr);
  int32_t mode = 0;
  nweb_impl_->SetDrawMode(mode);
  nweb_impl_->Resize(width, height, isKeyboard);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetDrawRect001) {
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->SetDrawRect(x, y, width, height);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetDrawMode001) {
  int32_t mode = 1;
  nweb_impl_->nweb_delegate_ = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->SetDrawMode(mode);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetDrawMode002) {
  int32_t mode = 1;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->SetDrawMode(mode);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetPendingSizeStatus001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->GetPendingSizeStatus();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, false);
}

TEST_F(NWebImplTest, GetPendingSizeStatus002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->GetPendingSizeStatus();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, nweb_impl_->nweb_delegate_->GetPendingSizeStatus());
}

TEST_F(NWebImplTest, SetFitContentMode001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  int mode = 0;
  nweb_impl_->SetFitContentMode(mode);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetFitContentMode002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  int mode = 0;
  nweb_impl_->SetFitContentMode(mode);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnTouchRelease001) {
  EXPECT_NE(nweb_impl_, nullptr);
  int32_t id = 0;
  double x = 0.2;
  double y = 0.1;
  bool from_overlay = false;
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->OnTouchRelease(id, x, y, from_overlay);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, OnTouchRelease002) {
  EXPECT_NE(nweb_impl_, nullptr);
  int32_t id = 0;
  double x = 0.2;
  double y = 0.1;
  bool from_overlay = false;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->OnTouchRelease(id, x, y, from_overlay);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, OnTouchMove001) {
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  int32_t id = 1;
  double x = 2.0;
  double y = 3.0;
  bool from_overlay = true;
  nweb_impl_->OnTouchMove(id, x, y, from_overlay);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, OnTouchCancel001) {
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->OnTouchCancel();
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, OnNavigateBack001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  nweb_impl_->OnNavigateBack();
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, OnNavigateBack002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->OnNavigateBack();
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, SendKeyEvent001) {
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  int32_t keyCode = 1;
  int32_t keyAction = 2;
  auto result = nweb_impl_->SendKeyEvent(keyCode, keyAction);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
  EXPECT_EQ(result, nweb_impl_->input_handler_->SendKeyEvent(keyCode, keyAction));
}

TEST_F(NWebImplTest, SendTouchpadFlingEvent001) {
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  double x = 1.0;
  double y = 1.0;
  double vx = 1.0;
  double vy = 1.0;
  nweb_impl_->SendTouchpadFlingEvent(x, y, vx, vy);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, SendMouseWheelEvent001) {
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  double x = 1.0;
  double y = 1.0;
  double deltaX = 1.0;
  double deltaY = 1.0;
  nweb_impl_->SendMouseWheelEvent(x, y, deltaX, deltaY);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, SendMouseEvent001) {
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  int x = 1;
  int y = 1;
  int button = 1;
  int action = 1;
  int count = 1;
  nweb_impl_->SendMouseEvent(x, y, button, action, count);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, SendMouseEvent002) {
  nweb_impl_->input_handler_ = std::make_shared<MockNWebInputHandler>(mock_delegate_);
  int x = 1;
  int y = 1;
  int button = 1;
  int action = 3;
  int count = 1;
  nweb_impl_->SendMouseEvent(x, y, button, action, count);
  EXPECT_NE(nweb_impl_, nullptr);
  EXPECT_NE(nweb_impl_->input_handler_, nullptr);
}

TEST_F(NWebImplTest, Load001) {
  std::string url = "https://example.com";
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = nullptr;
  int ret = nweb_impl_->Load(url);
  EXPECT_EQ(ret, temp);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
  EXPECT_EQ(nweb_impl_->output_handler_, nullptr);
}

TEST_F(NWebImplTest, Load002) {
  std::string url = "https://example.com";
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = nullptr;
  int ret = nweb_impl_->Load(url);
  EXPECT_EQ(ret, temp);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
  EXPECT_EQ(nweb_impl_->output_handler_, nullptr);
}

TEST_F(NWebImplTest, Load003) {
  std::string url = "https://example.com";
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = std::make_shared<NWebOutputHandler>(MockFrameCallback);
  int ret = nweb_impl_->Load(url);
  EXPECT_EQ(ret, temp);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
  EXPECT_NE(nweb_impl_->output_handler_, nullptr);
}

TEST_F(NWebImplTest, Load004) {
  std::string url = "https://example.com";
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->input_handler_ = nullptr;
  nweb_impl_->output_handler_ = std::make_shared<NWebOutputHandler>(MockFrameCallback);
  uint32_t width = 100;
  uint32_t height = 100;
  nweb_impl_->output_handler_->Resize(width, height);
  int ret = nweb_impl_->Load(url);
  EXPECT_EQ(ret, nweb_impl_->nweb_delegate_->Load(url));
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(nweb_impl_->input_handler_, nullptr);
  EXPECT_NE(nweb_impl_->output_handler_, nullptr);
}

TEST_F(NWebImplTest, IsNavigatebackwardAllowed001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->IsNavigatebackwardAllowed();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, false);
}

TEST_F(NWebImplTest, IsNavigatebackwardAllowed002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->IsNavigatebackwardAllowed();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CanNavigateBackOrForward001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  int numSteps = 1;
  auto result = nweb_impl_->CanNavigateBackOrForward(numSteps);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, false);
}

TEST_F(NWebImplTest, CanNavigateBackOrForward002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  int numSteps = 1;
  nweb_impl_->CanNavigateBackOrForward(numSteps);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NavigateBack001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->NavigateBack();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NavigateBack002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->NavigateBack();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NavigateForward001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->NavigateForward();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NavigateForward002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->NavigateForward();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NavigateBackOrForward001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  int step = 1;
  nweb_impl_->NavigateBackOrForward(step);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, NavigateBackOrForward002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  int step = 1;
  nweb_impl_->NavigateBackOrForward(step);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, DeleteNavigateHistory001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->DeleteNavigateHistory();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, DeleteNavigateHistory002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->DeleteNavigateHistory();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_BGTASK)
TEST_F(NWebImplTest, OnBrowserForegroundWithNullInitArgs)
{
  nweb_impl_->OnBrowserForeground();
  EXPECT_NE(nweb_impl_, nullptr);
}

TEST_F(NWebImplTest, OnBrowserBackgroundWithNullInitArgs)
{
  nweb_impl_->OnBrowserBackground();
  EXPECT_NE(nweb_impl_, nullptr);
}
#endif

TEST_F(NWebImplTest, Zoom001) {
  float zoomFactor = 1.0f;
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  int ret = nweb_impl_->Zoom(zoomFactor);
  EXPECT_EQ(ret, temp);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Zoom002) {
  float zoomFactor = 1.0f;
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  int ret = nweb_impl_->Zoom(zoomFactor);
  EXPECT_EQ(ret, nweb_impl_->nweb_delegate_->Zoom(zoomFactor));
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ZoomIn001) {
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  int ret = nweb_impl_->ZoomIn();
  EXPECT_EQ(ret, temp);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ZoomIn002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  int ret = nweb_impl_->ZoomIn();
  EXPECT_EQ(ret, nweb_impl_->nweb_delegate_->ZoomIn());
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ZoomOut001) {
  auto temp = OHOS::NWeb::NWEB_ERR;
  nweb_impl_->nweb_delegate_ = nullptr;
  int ret = nweb_impl_->ZoomOut();
  EXPECT_EQ(ret, temp);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ZoomOut002) {
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  int ret = nweb_impl_->ZoomOut();
  EXPECT_EQ(ret, nweb_impl_->nweb_delegate_->ZoomOut());
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearSslCache001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->ClearSslCache();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearSslCache002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->ClearSslCache();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearClientAuthenticationCache001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->ClearClientAuthenticationCache();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClearClientAuthenticationCache002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->ClearClientAuthenticationCache();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Reload001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->Reload();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Reload002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->Reload();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Stop001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->Stop();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, Stop002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->Stop();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ExecuteJavaScript001) {
  std::string code = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->ExecuteJavaScript(code);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ExecuteJavaScript002) {
  std::string code = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->ExecuteJavaScript(code);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutBackgroundColor001) {
  int color = 1;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->PutBackgroundColor(color);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutBackgroundColor002) {
  int color = 1;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->PutBackgroundColor(color);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutOptimizeParserBudgetEnabled001) {
  bool enable = true;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->PutOptimizeParserBudgetEnabled(enable);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PutOptimizeParserBudgetEnabled002) {
  bool enable = true;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->PutOptimizeParserBudgetEnabled(enable);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnContinue001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->OnContinue();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebComponentsBlur001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->WebComponentsBlur();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, WebComponentsBlur002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->WebComponentsBlur();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetEnableHalfFrameRate001) {
  bool enabled = true;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->SetEnableHalfFrameRate(enabled);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetEnableHalfFrameRate002) {
  bool enabled = true;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->SetEnableHalfFrameRate(enabled);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, GetPreference001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->GetPreference();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, nullptr);
}

TEST_F(NWebImplTest, GetPreference002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->GetPreference();
  EXPECT_EQ(result, nweb_impl_->nweb_delegate_->GetPreference());
}

TEST_F(NWebImplTest, Title001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->Title();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_TRUE(result.empty());
}

TEST_F(NWebImplTest, Title002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->Title();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ExecuteJavaScriptExt001) {
  const int fd = 0;
  const size_t scriptLength = 10;
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  bool extention = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->ExecuteJavaScriptExt(fd, scriptLength, callback, extention);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ExecuteJavaScriptExt002) {
  const int fd = 0;
  const size_t scriptLength = 10;
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  bool extention = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->ExecuteJavaScriptExt(fd, scriptLength, callback, extention);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_MSGPORT)
TEST_F(NWebImplTest, ExecuteJavaScript0001) {
  const std::string code = " test";
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  bool extention = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->ExecuteJavaScript(code, callback, extention);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ExecuteJavaScript0002) {
  const std::string code = " test";
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  bool extention = false;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->ExecuteJavaScript(code, callback, extention);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, CreateWebMessagePorts001) {
  std::vector<std::string> empty;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->CreateWebMessagePorts();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, empty);
}

TEST_F(NWebImplTest, CreateWebMessagePorts002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->CreateWebMessagePorts();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PostWebMessage001) {
  const std::string message = "testmessage";
  const std::vector<std::string> ports ={"Default", "IncludeSensitive", "Everything"};
  const std::string targetUri = "testuri";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->PostWebMessage(message, ports, targetUri);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PostWebMessage002) {
  const std::string message = "testmessage";
  const std::vector<std::string> ports ={"Default", "IncludeSensitive", "Everything"};
  const std::string targetUri = "testuri";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->PostWebMessage(message, ports, targetUri);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClosePort001) {
  const std::string portHandle = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->ClosePort(portHandle);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, ClosePort002) {
  const std::string portHandle = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->ClosePort(portHandle);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PostPortMessage001) {
  const std::string portHandle = "test";
  std::shared_ptr<NWebMessage> data = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->PostPortMessage(portHandle, data);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, PostPortMessage002) {
  const std::string portHandle = "test";
  std::shared_ptr<NWebMessage> data = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->PostPortMessage(portHandle, data);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetPortMessageCallback001) {
  const std::string portHandle = "test";
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->SetPortMessageCallback(portHandle, callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetPortMessageCallback002) {
  const std::string portHandle = "test";
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->SetPortMessageCallback(portHandle, callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif // BUILDFLAG(ARKWEB_MSGPORT)

TEST_F(NWebImplTest, SetAutofillCallback001) {
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->SetAutofillCallback(callback);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, SetAutofillCallback002) {
  std::shared_ptr<NWebMessageValueCallback> callback = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->SetAutofillCallback(callback);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, FillAutofillData001) {
  std::shared_ptr<NWebMessage> data = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->FillAutofillData(data);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, FillAutofillData002) {
  std::shared_ptr<NWebMessage> data = nullptr;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->FillAutofillData(data);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

TEST_F(NWebImplTest, OnAutofillCancel001) {
  const std::string fillContent = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  nweb_impl_->OnAutofillCancel(fillContent);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
TEST_F(NWebImplTest, OnAutofillCancel002) {
  const std::string fillContent = "test";
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  EXPECT_CALL(*mock_delegate_, ProcessAutofillCancel(fillContent)).WillOnce(::testing::Return());
  nweb_impl_->OnAutofillCancel(fillContent);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}
#endif

TEST_F(NWebImplTest, GetHitTestResult001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->GetHitTestResult();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(result, nullptr);
}

TEST_F(NWebImplTest, GetHitTestResult002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->GetHitTestResult();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, nweb_impl_->nweb_delegate_->GetHitTestResult());
}

TEST_F(NWebImplTest, GetLastHitTestResult001) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->GetLastHitTestResult();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_NE(result, nullptr);
}

TEST_F(NWebImplTest, GetLastHitTestResult002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->GetLastHitTestResult();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, nweb_impl_->nweb_delegate_->GetLastHitTestResult());
}

TEST_F(NWebImplTest, PageLoadProgress001) {
  int temp = 0;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->PageLoadProgress();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, PageLoadProgress002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->PageLoadProgress();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, nweb_impl_->nweb_delegate_->PageLoadProgress());
}

TEST_F(NWebImplTest, ContentHeight001) {
  int temp = 0;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->ContentHeight();
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, ContentHeight002) {
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->ContentHeight();
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, nweb_impl_->nweb_delegate_->ContentHeight());
}

TEST_F(NWebImplTest, Load0001) {
  const std::string url = "https://example.com";
  const std::map<std::string, std::string> additionalHttpHeaders = {
        {"Last-Modified", "def"},
        {"Access-Control-Allow-Origin", "ghi"}
  };
  auto temp = OHOS::NWeb::NWEB_ERR;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->Load(url, additionalHttpHeaders);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, Load0002) {
  const std::string url = "https://example.com";
  const std::map<std::string, std::string> additionalHttpHeaders = {
        {"Last-Modified", "def"},
        {"Access-Control-Allow-Origin", "ghi"}
  };
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  nweb_impl_->Load(url, additionalHttpHeaders);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
}

#if BUILDFLAG(ARKWEB_POST_URL)
TEST_F(NWebImplTest, PostUrl001) {
  const std::string url = "https://example.com";
  const std::vector<char> postData = {'m', 'a', 'r', 'k', '1'};
  auto temp = OHOS::NWeb::NWEB_ERR;
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = nullptr;
  auto result = nweb_impl_->PostUrl(url, postData);
  EXPECT_EQ(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebImplTest, PostUrl002) {
  const std::string url = "https://example.com";
  const std::vector<char> postData = {'m', 'a', 'r', 'k', '1'};
  EXPECT_NE(nweb_impl_, nullptr);
  nweb_impl_->nweb_delegate_ = mock_delegate_;
  auto result = nweb_impl_->PostUrl(url, postData);
  EXPECT_NE(nweb_impl_->nweb_delegate_, nullptr);
  EXPECT_EQ(result, nweb_impl_->nweb_delegate_->PostUrl(url, postData));
}

}  // namespace OHOS::NWeb
                          
