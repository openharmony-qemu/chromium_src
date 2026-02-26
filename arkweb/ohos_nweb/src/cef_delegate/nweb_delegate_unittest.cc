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
#include "capi/nweb_devtools_message_handler.h"
#include "capi/nweb_context_menus_item.h"
#include "cef/libcef/browser/menu_model_impl.h"
#include "include/cef_form_handler.h"
#include "include/cef_media_handler.h"
#include "include/cef_permission_request.h"
#include "include/cef_menu_model.h"
#include "nweb.h"
#include "nweb_delegate_adapter.h"
#include "nweb_delegate_interface.h"
#include "nweb_errors.h"
#include "nweb_download_callback.h"
#include "nweb_event_handler.h"
#include "nweb_find_delegate.h"
#include "nweb_handler_delegate.h"
#include "nweb_input_delegate.h"
#include "nweb_preference_delegate.h"
#include "nweb_render_handler.h"
#include "ohos_cef_ext/include/arkweb_client_ext.h"
#include "ohos_cef_ext/include/arkweb_display_handler_ext.h"
#include "ohos_cef_ext/include/arkweb_dialog_handler_ext.h"
#include "ohos_cef_ext/include/arkweb_load_handler_ext.h"
#include "ohos_cef_ext/include/arkweb_render_handler_ext.h"
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

class MockCefMenuModelImpl : public CefMenuModelImpl {
 public:
  MockCefMenuModelImpl() : CefMenuModelImpl(nullptr, nullptr, false) {}
  ~MockCefMenuModelImpl() override {}

  MOCK_METHOD(size_t, GetCount, (), (override));
  MOCK_METHOD(int, GetCommandIdAt, (size_t), (override));
  MOCK_METHOD(int, GetGroupIdAt, (size_t), (override));
  MOCK_METHOD(bool, IsSubMenu, (), (override));
  MOCK_METHOD(bool, IsEnabledAt, (size_t), (override));
  MOCK_METHOD(bool, IsVisibleAt, (size_t), (override));
  MOCK_METHOD(bool, IsCheckedAt, (size_t), (override));
  MOCK_METHOD(CefString, GetLabelAt, (size_t), (override));
  MOCK_METHOD(CefMenuModel::MenuItemType, GetTypeAt, (size_t), (override));
  MOCK_METHOD(CefRefPtr<CefMenuModel>, GetSubMenuAt, (size_t), (override));
};

class MockAlloyBrowserHostImpl {
 public:
  MOCK_METHOD(CefMenuManager*, GetMenuManager, (), ());
};

class MockCefMenuManager {
 public:
  MOCK_METHOD(CefRefPtr<CefMenuModelImpl>, GetContextMenuModel, (), ());
};

class MockArkWebBrowserHostExt : public ArkWebBrowserHostExt {
 public:
  MockArkWebBrowserHostExt() : alloy_host_(nullptr) {}
  ~MockArkWebBrowserHostExt() override = default;

  CefRefPtr<CefBrowser> GetBrowser() override { return nullptr; }
  void CloseBrowser(bool force_close) override {}
  bool TryCloseBrowser() override { return false; }
  void SetFocus(bool focus) override {}
  bool HasView() override { return false; }
  CefRefPtr<CefClient> GetClient() override { return nullptr; }
  CefRefPtr<CefRequestContext> GetRequestContext() override { return nullptr; }
  double GetZoomLevel() override { return 0.0; }
  void SetZoomLevel(double zoomLevel) override {}
  void RunFileDialog(FileDialogMode mode, const CefString& title,
                     const CefString& default_file_path,
                     const std::vector<CefString>& accept_filters,
                     CefRefPtr<CefRunFileDialogCallback> callback) override {}
  void StartDownload(const CefString& url) override {}
  void DownloadImage(const CefString& image_url, bool is_favicon,
                     uint32_t max_image_size, bool bypass_cache,
                     CefRefPtr<CefDownloadImageCallback> callback) override {}
  void Print() override {}
  void PrintToPDF(const CefString& path, const CefPdfPrintSettings& settings,
                  CefRefPtr<CefPdfPrintCallback> callback) override {}
  void Find(const CefString& searchText, bool forward, bool matchCase,
            bool findNext) override {}
  void StopFinding(bool clearSelection) override {}
  void ShowDevTools(const CefWindowInfo& windowInfo, CefRefPtr<CefClient> client,
                    const CefBrowserSettings& settings,
                    const CefPoint& inspect_element_at) override {}
  void CloseDevTools() override {}
  bool CanGoBack() override { return false; }
  void GoBack() override {}
  bool CanGoForward() override { return false; }
  void GoForward() override {}
  bool IsLoading() override { return false; }
  void Reload() override {}
  void ReloadIgnoreCache() override {}
  void StopLoad() override {}
  int GetIdentifier() override { return 0; }
  bool IsSame(CefRefPtr<CefBrowser> that) override { return false; }
  CefRefPtr<CefBrowser> GetFocusedFrame() override { return nullptr; }
  CefRefPtr<CefFrame> GetFrame(int64_t identifier) override { return nullptr; }
  CefRefPtr<CefFrame> GetFrame(const CefString& name) override { return nullptr; }
  CefRefPtr<ZoomObserver> GetZoomObserver() override { return nullptr; }
  void SendTouchEvent(const CefTouchEvent& event) override {}
  void SendMouseClickEvent(const CefMouseEvent& event,
                           cef_mouse_button_type_t type, int mouse_flags,
                           bool is_keyboard_accelerated) override {}
  void SendMouseMoveEvent(const CefMouseEvent& event, bool mouse_leave) override {}
  void SendMouseWheelEvent(const CefMouseEvent& event, int deltaX, int deltaY) override {}
  void SendKeyEvent(const CefKeyEvent& event) override {}
  CefWindowHandle GetWindowHandle() { return 0; }
  CefWindowHandle GetOpenerWindowHandle() { return 0; }

  void SetMockAlloyBrowserHostImpl(MockAlloyBrowserHostImpl* alloy_host) {
    alloy_host_ = alloy_host;
  }

  MockAlloyBrowserHostImpl* AsAlloyBrowserHostImpl() {
    return alloy_host_;
  }

 private:
  MockAlloyBrowserHostImpl* alloy_host_;
};

class MockNWebHandlerDelegate : public NWebHandlerDelegate {
 public:
  MockNWebHandlerDelegate() = default;
  ~MockNWebHandlerDelegate() override = default;

  CefRefPtr<ArkWebBrowserExt> GetBrowser() override {
    return browser_ext_;
  }

  void SetMockBrowserHostExt(MockArkWebBrowserHostExt* host_ext) {
    browser_ext_ = host_ext;
  }

 private:
  CefRefPtr<ArkWebBrowserExt> browser_ext_;
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

#if BUILDFLAG(ARKWEB_DEVTOOLS)
TEST_F(NWebDelegateTest, GetContextMenuItem001) {
  ASSERT_NE(nweb_delegate_, nullptr);
  
  auto mock_host = new MockArkWebBrowserHostExt();
  auto mock_alloy_host = new MockAlloyBrowserHostImpl();
  auto mock_menu_manager = new MockCefMenuManager();
  auto mock_menu_model = new MockCefMenuModelImpl();
  
  mock_host->SetMockAlloyBrowserHostImpl(mock_alloy_host);
  
  EXPECT_CALL(*mock_menu_model, GetCount()).WillRepeatedly(Return(0));
  EXPECT_CALL(*mock_menu_manager, GetContextMenuModel())
      .WillRepeatedly(Return(CefRefPtr<CefMenuModelImpl>(mock_menu_model)));
  EXPECT_CALL(*mock_alloy_host, GetMenuManager())
      .WillRepeatedly(Return(mock_menu_manager));
  
  auto mock_handler = new MockNWebHandlerDelegate();
  mock_handler->SetMockBrowserHostExt(mock_host);
  nweb_delegate_->handler_delegate_ = mock_handler;
  
  auto items = nweb_delegate_->GetContextMenuItem();
  EXPECT_TRUE(items.empty());
}

TEST_F(NWebDelegateTest, GetContextMenuItem002) {
  ASSERT_NE(nweb_delegate_, nullptr);
  
  auto mock_host = new MockArkWebBrowserHostExt();
  auto mock_alloy_host = new MockAlloyBrowserHostImpl();
  auto mock_menu_manager = new MockCefMenuManager();
  auto mock_menu_model = new MockCefMenuModelImpl();
  
  mock_host->SetMockAlloyBrowserHostImpl(mock_alloy_host);
  
  EXPECT_CALL(*mock_menu_model, GetCount()).WillRepeatedly(Return(1));
  EXPECT_CALL(*mock_menu_model, GetCommandIdAt(0)).WillRepeatedly(Return(100));
  EXPECT_CALL(*mock_menu_model, GetGroupIdAt(0)).WillRepeatedly(Return(1));
  EXPECT_CALL(*mock_menu_model, IsSubMenu()).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, IsEnabledAt(0)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsVisibleAt(0)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsCheckedAt(0)).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, GetLabelAt(0))
      .WillRepeatedly(Return(CefString("Test Item")));
  EXPECT_CALL(*mock_menu_model, GetTypeAt(0))
      .WillRepeatedly(Return(static_cast<CefMenuModel::MenuItemType>(CefMenuModel::MENUITEMTYPE_COMMAND)));
  
  EXPECT_CALL(*mock_menu_manager, GetContextMenuModel())
      .WillRepeatedly(Return(CefRefPtr<CefMenuModelImpl>(mock_menu_model)));
  EXPECT_CALL(*mock_alloy_host, GetMenuManager())
      .WillRepeatedly(Return(mock_menu_manager));
  
  auto mock_handler = new MockNWebHandlerDelegate();
  mock_handler->SetMockBrowserHostExt(mock_host);
  nweb_delegate_->handler_delegate_ = mock_handler;
  
  auto items = nweb_delegate_->GetContextMenuItem();
  EXPECT_EQ(items.size(), 1);
  EXPECT_EQ(items[0].commandId, 100);
  EXPECT_EQ(items[0].groupId, 1);
  EXPECT_EQ(items[0].parentId, 1);
  EXPECT_FALSE(items[0].isSubMenu);
  EXPECT_TRUE(items[0].enabled);
  EXPECT_TRUE(items[0].visible);
  EXPECT_FALSE(items[0].checked);
  EXPECT_EQ(items[0].label, "Test Item");
  EXPECT_EQ(items[0].type, WebExtensionMenusType::WEB_EXTENSION_MENUITEMTYPE_COMMAND);
}

TEST_F(NWebDelegateTest, GetContextMenuItem003) {
  ASSERT_NE(nweb_delegate_, nullptr);
  
  auto mock_host = new MockArkWebBrowserHostExt();
  auto mock_alloy_host = new MockAlloyBrowserHostImpl();
  auto mock_menu_manager = new MockCefMenuManager();
  auto mock_menu_model = new MockCefMenuModelImpl();
  
  mock_host->SetMockAlloyBrowserHostImpl(mock_alloy_host);
  
  EXPECT_CALL(*mock_menu_model, GetCount()).WillRepeatedly(Return(3));
  
  EXPECT_CALL(*mock_menu_model, GetCommandIdAt(0)).WillRepeatedly(Return(100));
  EXPECT_CALL(*mock_menu_model, GetGroupIdAt(0)).WillRepeatedly(Return(1));
  EXPECT_CALL(*mock_menu_model, IsSubMenu()).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, IsEnabledAt(0)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsVisibleAt(0)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsCheckedAt(0)).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, GetLabelAt(0)).WillRepeatedly(Return(CefString("Item 1")));
  EXPECT_CALL(*mock_menu_model, GetTypeAt(0))
      .WillRepeatedly(Return(static_cast<CefMenuModel::MenuItemType>(CefMenuModel::MENUITEMTYPE_COMMAND)));
  
  EXPECT_CALL(*mock_menu_model, GetCommandIdAt(1)).WillRepeatedly(Return(200));
  EXPECT_CALL(*mock_menu_model, GetGroupIdAt(1)).WillRepeatedly(Return(2));
  EXPECT_CALL(*mock_menu_model, IsEnabledAt(1)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsVisibleAt(1)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsCheckedAt(1)).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, GetLabelAt(1)).WillRepeatedly(Return(CefString("Item 2")));
  EXPECT_CALL(*mock_menu_model, GetTypeAt(1))
      .WillRepeatedly(Return(static_cast<CefMenuModel::MenuItemType>(CefMenuModel::MENUITEMTYPE_COMMAND)));
  
  EXPECT_CALL(*mock_menu_model, GetCommandIdAt(2)).WillRepeatedly(Return(300));
  EXPECT_CALL(*mock_menu_model, GetGroupIdAt(2)).WillRepeatedly(Return(3));
  EXPECT_CALL(*mock_menu_model, IsEnabledAt(2)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsVisibleAt(2)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsCheckedAt(2)).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, GetLabelAt(2)).WillRepeatedly(Return(CefString("Item 3")));
  EXPECT_CALL(*mock_menu_model, GetTypeAt(2))
      .WillRepeatedly(Return(static_cast<CefMenuModel::MenuItemType>(CefMenuModel::MENUITEMTYPE_COMMAND)));
  
  EXPECT_CALL(*mock_menu_manager, GetContextMenuModel())
      .WillRepeatedly(Return(CefRefPtr<CefMenuModelImpl>(mock_menu_model)));
  EXPECT_CALL(*mock_alloy_host, GetMenuManager())
      .WillRepeatedly(Return(mock_menu_manager));
  
  auto mock_handler = new MockNWebHandlerDelegate();
  mock_handler->SetMockBrowserHostExt(mock_host);
  nweb_delegate_->handler_delegate_ = mock_handler;
  
  auto items = nweb_delegate_->GetContextMenuItem();
  EXPECT_EQ(items.size(), 3);
  EXPECT_EQ(items[0].commandId, 100);
  EXPECT_EQ(items[0].label, "Item 1");
  EXPECT_EQ(items[1].commandId, 200);
  EXPECT_EQ(items[1].label, "Item 2");
  EXPECT_EQ(items[2].commandId, 300);
  EXPECT_EQ(items[2].label, "Item 3");
}

TEST_F(NWebDelegateTest, GetContextMenuItem004) {
  ASSERT_NE(nweb_delegate_, nullptr);
  
  auto mock_host = new MockArkWebBrowserHostExt();
  auto mock_alloy_host = new MockAlloyBrowserHostImpl();
  auto mock_menu_manager = new MockCefMenuManager();
  auto mock_menu_model = new MockCefMenuModelImpl();
  
  mock_host->SetMockAlloyBrowserHostImpl(mock_alloy_host);
  
  EXPECT_CALL(*mock_menu_model, GetCount()).WillRepeatedly(Return(1));
  EXPECT_CALL(*mock_menu_model, GetCommandIdAt(0)).WillRepeatedly(Return(100));
  EXPECT_CALL(*mock_menu_model, GetGroupIdAt(0)).WillRepeatedly(Return(1));
  EXPECT_CALL(*mock_menu_model, IsSubMenu()).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, IsEnabledAt(0)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsVisibleAt(0)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsCheckedAt(0)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, GetLabelAt(0)).WillRepeatedly(Return(CefString("Check Item")));
  EXPECT_CALL(*mock_menu_model, GetTypeAt(0))
      .WillRepeatedly(Return(static_cast<CefMenuModel::MenuItemType>(CefMenuModel::MENUITEMTYPE_CHECK)));
  
  EXPECT_CALL(*mock_menu_manager, GetContextMenuModel())
      .WillRepeatedly(Return(CefRefPtr<CefMenuModelImpl>(mock_menu_model)));
  EXPECT_CALL(*mock_alloy_host, GetMenuManager())
      .WillRepeatedly(Return(mock_menu_manager));
  
  auto mock_handler = new MockNWebHandlerDelegate();
  mock_handler->SetMockBrowserHostExt(mock_host);
  nweb_delegate_->handler_delegate_ = mock_handler;
  
  auto items = nweb_delegate_->GetContextMenuItem();
  EXPECT_EQ(items.size(), 1);
  EXPECT_EQ(items[0].commandId, 100);
  EXPECT_TRUE(items[0].checked);
  EXPECT_EQ(items[0].type, WebExtensionMenusType::WEB_EXTENSION_MENUITEMTYPE_CHECK);
}

TEST_F(NWebDelegateTest, GetContextMenuItem005) {
  ASSERT_NE(nweb_delegate_, nullptr);
  
  auto mock_host = new MockArkWebBrowserHostExt();
  auto mock_alloy_host = new MockAlloyBrowserHostImpl();
  auto mock_menu_manager = new MockCefMenuManager();
  auto mock_menu_model = new MockCefMenuModelImpl();
  
  mock_host->SetMockAlloyBrowserHostImpl(mock_alloy_host);
  
  EXPECT_CALL(*mock_menu_model, GetCount()).WillRepeatedly(Return(1));
  EXPECT_CALL(*mock_menu_model, GetCommandIdAt(0)).WillRepeatedly(Return(100));
  EXPECT_CALL(*mock_menu_model, GetGroupIdAt(0)).WillRepeatedly(Return(5));
  EXPECT_CALL(*mock_menu_model, IsSubMenu()).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, IsEnabledAt(0)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsVisibleAt(0)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsCheckedAt(0)).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, GetLabelAt(0)).WillRepeatedly(Return(CefString("Radio Item")));
  EXPECT_CALL(*mock_menu_model, GetTypeAt(0))
      .WillRepeatedly(Return(static_cast<CefMenuModel::MenuItemType>(CefMenuModel::MENUITEMTYPE_RADIO)));
  
  EXPECT_CALL(*mock_menu_manager, GetContextMenuModel())
      .WillRepeatedly(Return(CefRefPtr<CefMenuModelImpl>(mock_menu_model)));
  EXPECT_CALL(*mock_alloy_host, GetMenuManager())
      .WillRepeatedly(Return(mock_menu_manager));
  
  auto mock_handler = new MockNWebHandlerDelegate();
  mock_handler->SetMockBrowserHostExt(mock_host);
  nweb_delegate_->handler_delegate_ = mock_handler;
  
  auto items = nweb_delegate_->GetContextMenuItem();
  EXPECT_EQ(items.size(), 1);
  EXPECT_EQ(items[0].commandId, 100);
  EXPECT_EQ(items[0].groupId, 5);
  EXPECT_EQ(items[0].parentId, 5);
  EXPECT_EQ(items[0].type, WebExtensionMenusType::WEB_EXTENSION_MENUITEMTYPE_RADIO);
}

TEST_F(NWebDelegateTest, GetContextMenuItem006) {
  ASSERT_NE(nweb_delegate_, nullptr);
  
  auto mock_host = new MockArkWebBrowserHostExt();
  auto mock_alloy_host = new MockAlloyBrowserHostImpl();
  auto mock_menu_manager = new MockCefMenuManager();
  auto mock_menu_model = new MockCefMenuModelImpl();
  
  mock_host->SetMockAlloyBrowserHostImpl(mock_alloy_host);
  
  EXPECT_CALL(*mock_menu_model, GetCount()).WillRepeatedly(Return(1));
  EXPECT_CALL(*mock_menu_model, GetCommandIdAt(0)).WillRepeatedly(Return(0));
  EXPECT_CALL(*mock_menu_model, GetGroupIdAt(0)).WillRepeatedly(Return(0));
  EXPECT_CALL(*mock_menu_model, IsSubMenu()).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, IsEnabledAt(0)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsVisibleAt(0)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsCheckedAt(0)).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, GetLabelAt(0)).WillRepeatedly(Return(CefString("")));
  EXPECT_CALL(*mock_menu_model, GetTypeAt(0))
      .WillRepeatedly(Return(static_cast<CefMenuModel::MenuItemType>(CefMenuModel::MENUITEMTYPE_SEPARATOR)));
  
  EXPECT_CALL(*mock_menu_manager, GetContextMenuModel())
      .WillRepeatedly(Return(CefRefPtr<CefMenuModelImpl>(mock_menu_model)));
  EXPECT_CALL(*mock_alloy_host, GetMenuManager())
      .WillRepeatedly(Return(mock_menu_manager));
  
  auto mock_handler = new MockNWebHandlerDelegate();
  mock_handler->SetMockBrowserHostExt(mock_host);
  nweb_delegate_->handler_delegate_ = mock_handler;
  
  auto items = nweb_delegate_->GetContextMenuItem();
  EXPECT_EQ(items.size(), 1);
  EXPECT_EQ(items[0].commandId, 0);
  EXPECT_EQ(items[0].type, WebExtensionMenusType::WEB_EXTENSION_MENUITEMTYPE_SEPARATOR);
}

TEST_F(NWebDelegateTest, GetContextMenuItem007) {
  ASSERT_NE(nweb_delegate_, nullptr);
  
  auto mock_host = new MockArkWebBrowserHostExt();
  auto mock_alloy_host = new MockAlloyBrowserHostImpl();
  auto mock_menu_manager = new MockCefMenuManager();
  auto mock_menu_model = new MockCefMenuModelImpl();
  
  mock_host->SetMockAlloyBrowserHostImpl(mock_alloy_host);
  
  EXPECT_CALL(*mock_menu_model, GetCount()).WillRepeatedly(Return(1));
  EXPECT_CALL(*mock_menu_model, GetCommandIdAt(0)).WillRepeatedly(Return(100));
  EXPECT_CALL(*mock_menu_model, GetGroupIdAt(0)).WillRepeatedly(Return(1));
  EXPECT_CALL(*mock_menu_model, IsSubMenu()).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsEnabledAt(0)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsVisibleAt(0)).WillRepeatedly(Return(true));
  EXPECT_CALL(*mock_menu_model, IsCheckedAt(0)).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, GetLabelAt(0)).WillRepeatedly(Return(CefString("Submenu")));
  EXPECT_CALL(*mock_menu_model, GetTypeAt(0))
      .WillRepeatedly(Return(static_cast<CefMenuModel::MenuItemType>(CefMenuModel::MENUITEMTYPE_SUBMENU)));
  
  EXPECT_CALL(*mock_menu_manager, GetContextMenuModel())
      .WillRepeatedly(Return(CefRefPtr<CefMenuModelImpl>(mock_menu_model)));
  EXPECT_CALL(*mock_alloy_host, GetMenuManager())
      .WillRepeatedly(Return(mock_menu_manager));
  
  auto mock_handler = new MockNWebHandlerDelegate();
  mock_handler->SetMockBrowserHostExt(mock_host);
  nweb_delegate_->handler_delegate_ = mock_handler;
  
  auto items = nweb_delegate_->GetContextMenuItem();
  EXPECT_EQ(items.size(), 1);
  EXPECT_EQ(items[0].commandId, 100);
  EXPECT_TRUE(items[0].isSubMenu);
  EXPECT_EQ(items[0].type, WebExtensionMenusType::WEB_EXTENSION_MENUITEMTYPE_SUBMENU);
}

TEST_F(NWebDelegateTest, GetContextMenuItem008) {
  ASSERT_NE(nweb_delegate_, nullptr);
  
  auto mock_host = new MockArkWebBrowserHostExt();
  auto mock_alloy_host = new MockAlloyBrowserHostImpl();
  auto mock_menu_manager = new MockCefMenuManager();
  auto mock_menu_model = new MockCefMenuModelImpl();
  
  mock_host->SetMockAlloyBrowserHostImpl(mock_alloy_host);
  
  EXPECT_CALL(*mock_menu_model, GetCount()).WillRepeatedly(Return(1));
  EXPECT_CALL(*mock_menu_model, GetCommandIdAt(0)).WillRepeatedly(Return(100));
  EXPECT_CALL(*mock_menu_model, GetGroupIdAt(0)).WillRepeatedly(Return(1));
  EXPECT_CALL(*mock_menu_model, IsSubMenu()).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, IsEnabledAt(0)).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, IsVisibleAt(0)).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, IsCheckedAt(0)).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_menu_model, GetLabelAt(0)).WillRepeatedly(Return(CefString("Disabled Item")));
  EXPECT_CALL(*mock_menu_model, GetTypeAt(0))
      .WillRepeatedly(Return(static_cast<CefMenuModel::MenuItemType>(CefMenuModel::MENUITEMTYPE_COMMAND)));
  
  EXPECT_CALL(*mock_menu_manager, GetContextMenuModel())
      .WillRepeatedly(Return(CefRefPtr<CefMenuModelImpl>(mock_menu_model)));
  EXPECT_CALL(*mock_alloy_host, GetMenuManager())
      .WillRepeatedly(Return(mock_menu_manager));
  
  auto mock_handler = new MockNWebHandlerDelegate();
  mock_handler->SetMockBrowserHostExt(mock_host);
  nweb_delegate_->handler_delegate_ = mock_handler;
  
  auto items = nweb_delegate_->GetContextMenuItem();
  EXPECT_EQ(items.size(), 1);
  EXPECT_EQ(items[0].commandId, 100);
  EXPECT_FALSE(items[0].enabled);
  EXPECT_FALSE(items[0].visible);
}

TEST_F(NWebDelegateTest, OnContextMenuSelected001) {
  ASSERT_NE(nweb_delegate_, nullptr);
  nweb_delegate_->OnContextMenuSelected(1);
}

TEST_F(NWebDelegateTest, OnContextMenuSelected002) {
  ASSERT_NE(nweb_delegate_, nullptr);
  nweb_delegate_->OnContextMenuSelected(1);
}

TEST_F(NWebDelegateTest, OnContextMenuClosed001) {
  ASSERT_NE(nweb_delegate_, nullptr);
  nweb_delegate_->OnContextMenuClosed();
}

TEST_F(NWebDelegateTest, OnContextMenuClosed002) {
  ASSERT_NE(nweb_delegate_, nullptr);
  nweb_delegate_->OnContextMenuClosed();
}

TEST_F(NWebDelegateTest, OpenDevtoolsWithByPb001) {
  ASSERT_NE(nweb_delegate_, nullptr);
  std::unique_ptr<OpenDevToolsParam> param = std::make_unique<OpenDevToolsParam>();
  OpenDevToolsExtOpt ext_opt;
  nweb_delegate_->OpenDevtoolsWithByPb(nweb_delegate_, std::move(param), ext_opt);
}

TEST_F(NWebDelegateTest, OpenDevtoolsWithByPb002) {
  ASSERT_NE(nweb_delegate_, nullptr);
  std::unique_ptr<OpenDevToolsParam> param = std::make_unique<OpenDevToolsParam>();
  OpenDevToolsExtOpt ext_opt;
  nweb_delegate_->OpenDevtoolsWithByPb(nweb_delegate_, std::move(param), ext_opt);
}
#endif

}  // namespace OHOS::NWeb
