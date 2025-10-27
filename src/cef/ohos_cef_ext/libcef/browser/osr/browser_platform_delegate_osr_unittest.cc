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

#define protected public
#include "cef/libcef/browser/osr/browser_platform_delegate_osr.h"
#include "cef/ohos_cef_ext/include/arkweb_render_handler_ext.h"

#include "arkweb/build/features/features.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "components/input/native_web_keyboard_event.h"
#include "third_party/blink/public/common/input/web_mouse_event.h"
#include "third_party/blink/public/common/input/web_mouse_wheel_event.h"
#include "third_party/blink/public/common/input/web_mouse_event.h"
#include "third_party/blink/public/common/input/web_gesture_event.h"

namespace {
std::shared_ptr<CefBrowserPlatformDelegateOsr> g_platform_delegate;
}

class CefBrowserPlatformDelegateNativeMock
    : public CefBrowserPlatformDelegateNative {
 public:
  input::NativeWebKeyboardEvent TranslateWebKeyEvent(
      const CefKeyEvent& key_event) const override;
  blink::WebMouseEvent TranslateWebClickEvent(
      const CefMouseEvent& mouse_event,
      CefBrowserHost::MouseButtonType type,
      bool mouseUp,
      int clickCount) const override;
  blink::WebMouseEvent TranslateWebMoveEvent(const CefMouseEvent& mouse_event,
                                             bool mouseLeave) const override;
  blink::WebMouseWheelEvent TranslateWebWheelEvent(
      const CefMouseEvent& mouse_event,
      int deltaX,
      int deltaY) const override;
  blink::WebGestureEvent TranslateTouchpadFlingEvent(
      const CefMouseEvent& mouse_event) const override;
  CefBrowserPlatformDelegateNativeMock(const CefWindowInfo& window_info,
                                       SkColor background_color)
      : CefBrowserPlatformDelegateNative(window_info, background_color) {}
  CefBrowserPlatformDelegateNativeMock() = delete;
  ~CefBrowserPlatformDelegateNativeMock() {}
};

input::NativeWebKeyboardEvent
CefBrowserPlatformDelegateNativeMock::TranslateWebKeyEvent(
    const CefKeyEvent& key_event) const {
  gfx::NativeEvent native_event = nullptr;
  input::NativeWebKeyboardEvent nativewebkeyboard(native_event);
  return nativewebkeyboard;
}

blink::WebMouseEvent
CefBrowserPlatformDelegateNativeMock::TranslateWebClickEvent(
    const CefMouseEvent& mouse_event,
    CefBrowserHost::MouseButtonType type,
    bool mouseUp,
    int clickCount) const {
  return blink::WebMouseEvent();
}

blink::WebMouseEvent
CefBrowserPlatformDelegateNativeMock::TranslateWebMoveEvent(
    const CefMouseEvent& mouse_event,
    bool mouseLeave) const {
  return blink::WebMouseEvent();
}

blink::WebMouseWheelEvent
CefBrowserPlatformDelegateNativeMock::TranslateWebWheelEvent(
    const CefMouseEvent& mouse_event,
    int deltaX,
    int deltaY) const {
  return blink::WebMouseWheelEvent();
}

blink::WebGestureEvent
CefBrowserPlatformDelegateNativeMock::TranslateTouchpadFlingEvent(
    const CefMouseEvent& mouse_event) const {
  return blink::WebGestureEvent();
}

class CefBrowserPlatformDelegateOsrTest : public testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();
};
void CefBrowserPlatformDelegateOsrTest::SetUpTestCase(void) {}

void CefBrowserPlatformDelegateOsrTest::TearDownTestCase(void) {}

void CefBrowserPlatformDelegateOsrTest::SetUp(void) {
  CefWindowInfo window_info;
  auto native_delegate =
      std::make_unique<CefBrowserPlatformDelegateNativeMock>(window_info, 0);
  g_platform_delegate = std::make_shared<CefBrowserPlatformDelegateOsr>(
      std::move(native_delegate), true, true);
}

void CefBrowserPlatformDelegateOsrTest::TearDown(void) {
  g_platform_delegate = nullptr;
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(CefBrowserPlatformDelegateOsrTest, OnNativeEmbedLifecycleChange) {
  ASSERT_NE(g_platform_delegate, nullptr);
  ArkWebRenderHandlerExt::CefNativeEmbedData data_info;
  data_info.status = CREATE;
  data_info.surfaceId = "";
  data_info.embedId = "";
  data_info.info.id = "";
  data_info.info.x = 0;
  data_info.info.y = 0;
  data_info.info.width = 0;
  data_info.info.height = 0;
  data_info.info.type = "";
  data_info.info.src = "";
  data_info.info.url = "";
  data_info.info.tag = "";
  data_info.info.params = {{"key1", "value1"}, {"key2", "value2"}};
  g_platform_delegate->OnNativeEmbedLifecycleChange(data_info);
}
#endif

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
TEST_F(CefBrowserPlatformDelegateOsrTest, SetDrawRect) {
  ASSERT_NE(g_platform_delegate, nullptr);
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
  g_platform_delegate->SetDrawRect(x, y, width, height);
}
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
TEST_F(CefBrowserPlatformDelegateOsrTest, WebPageSnapshot) {
  ASSERT_NE(g_platform_delegate, nullptr);
  const char* id = "test_id";
  int width = 1024;
  int height = 768;
  g_platform_delegate->WebPageSnapshot(
      id, width, height,
      base::BindOnce([](const char* str, bool is, void* ptr, int a, int b) {}));
}
#endif

TEST_F(CefBrowserPlatformDelegateOsrTest, SendKeyEvent) {
  ASSERT_NE(g_platform_delegate, nullptr);
  CefKeyEvent event;
  g_platform_delegate->SendKeyEvent(event);
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(CefBrowserPlatformDelegateOsrTest, ScrollFocusedEditableNodeIntoView) {
  ASSERT_NE(g_platform_delegate, nullptr);
  g_platform_delegate->ScrollFocusedEditableNodeIntoView();
}

TEST_F(CefBrowserPlatformDelegateOsrTest, SetVirtualKeyBoardArg) {
  ASSERT_NE(g_platform_delegate, nullptr);
  int32_t width = 1;
  int32_t height = 2;
  double keyboard = 10;
  g_platform_delegate->SetVirtualKeyBoardArg(width, height, keyboard);
}
#endif
