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

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#include "cef_browser.h"
#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "nweb_file_selector_params_impl.h"
#include "nweb_handler.h"
#include "nweb_js_dialog_result_impl.h"
#include "ohos_nweb/include/nweb_errors.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#define private public
#include "nweb_handler_delegate.h"

using namespace testing;
using namespace OHOS::NWeb;

class MockNWebInputMethodClient : public NWebInputMethodClient {
 public:
  ~MockNWebInputMethodClient() = default;

  void Attach(CefRefPtr<CefBrowser> browser,
              InputInfo inputInfo,
              bool is_need_reset_listener,
              int32_t enterKeyType) override {}

  void ShowTextInput() override {}

  void HideTextInput(
      uint32_t nweb_id = 0,
      HideTextinputType hideType = HideTextinputType::FROM_KERNEL,
      bool noNeedKeyboardByInput = false) override {}

  void HideTextInputForce() override {}

  void OnTextSelectionChanged(CefRefPtr<CefBrowser> browser,
                              const CefString& selected_text,
                              const CefRange& selected_range) override {}

  void OnCursorUpdate(const CefRect& rect) override {}

  void OnSelectionChanged(CefRefPtr<CefBrowser> browser,
                          const CefString& text,
                          const CefRange& selected_range) override {}

  void SetFocusStatus(bool focus_status) override {}

  void OnEditableChanged(CefRefPtr<CefBrowser> browser,
                         bool is_editable_node) override {}

  bool GetIsEditableNode() override { return true; }

  bool HasComposition() override { return true; }

  void OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser,
                                    const CefRange& selected_range) override {}

  void OnUpdateTextInputStateCalled(CefRefPtr<CefBrowser> browser,
                                    const CefString& text,
                                    const CefRange& selected_range,
                                    const CefRange& compositon_range) override {
  }

  bool IsAttached() override { return true; }

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  virtual void SetFillContent(const std::string& content,
                              int32_t node_id) override {}
#endif
};

class MockEventHandler : public NWebEventHandler {
 public:
  MOCK_METHOD1(SetIsFocus, void(bool));
};

class MockRenderHandler : public NWebRenderHandler {
 public:
  MOCK_METHOD1(SetFocusStatus, void(bool isFocus));
};

class MockFileSelectorCallbackImpl : public NWebStringVectorValueCallback {
 public:
  MOCK_METHOD1(Continue, void(const std::vector<std::string>&));
  MOCK_METHOD0(Cancel, void());
};

class MockNWebHandler : public NWebHandler {
 public:
  MOCK_METHOD1(OnPreKeyEvent, bool(std::shared_ptr<NWebKeyEvent>));
  MOCK_METHOD(bool,
              OnUnProcessedKeyEvent,
              (std::shared_ptr<NWebKeyEvent>),
              (override));
  MOCK_METHOD1(OnFocus, bool(NWebFocusSource source));
  MOCK_METHOD3(OnAlertDialogByJS,
               bool(const std::string&,
                    const std::string&,
                    std::shared_ptr<NWebJSDialogResult>));
  MOCK_METHOD3(OnConfirmDialogByJS,
               bool(const std::string&,
                    const std::string&,
                    std::shared_ptr<NWebJSDialogResult>));
  MOCK_METHOD4(OnPromptDialogByJS,
               bool(const std::string&,
                    const std::string&,
                    const std::string&,
                    std::shared_ptr<NWebJSDialogResult>));
  using NWebHandler::OnBeforeUnloadByJS;

  MOCK_METHOD3(OnBeforeUnloadByJS,
               bool(const CefString&,
                    const CefString&,
                    std::shared_ptr<NWebJSDialogResult>));
  MOCK_METHOD2(OnFileSelectorShow,
               bool(std::shared_ptr<NWebStringVectorValueCallback>,
                    std::shared_ptr<NWebFileSelectorParams>));
};

class MockWebAppClientExtensionListener
    : public NWebAppClientExtensionCallback {
 public:
  MOCK_METHOD3(OnSaveOrUpdatePassword, void(bool, const std::string&, int));
};

class MockCefBrowser : public ArkWebBrowserExt {
 public:
  bool IsValid() override { return false; }
  MOCK_METHOD0(GetHost, CefRefPtr<ArkWebBrowserHostExt>());
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
  bool IsPopup() override { return false; }
  bool HasDocument() override { return false; }
  CefRefPtr<CefFrame> GetMainFrame() override { return nullptr; }
  CefRefPtr<CefFrame> GetFocusedFrame() override { return nullptr; }
  CefRefPtr<CefFrame> GetFrameByIdentifier(const CefString& identifier) override { return nullptr; }
  CefRefPtr<CefFrame> GetFrameByName(const CefString& name) override {
    return nullptr;
  }
  size_t GetFrameCount() override { return 0; }
  void GetFrameIdentifiers(std::vector<CefString>& identifiers) override {}
  void GetFrameNames(std::vector<CefString>& names) override {}
  CefRefPtr<CefBrowserPermissionRequestDelegate> GetPermissionRequestDelegate()
      override {
    return nullptr;
  }
  CefRefPtr<CefGeolocationAcess> GetGeolocationPermissions() override {
    return nullptr;
  }
  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return false; }
  bool HasAtLeastOneRef() const override { return false; }
#if BUILDFLAG(IS_OHOS)
  bool CanGoBackOrForward(int num_steps) override { return false; }
  void GoBackOrForward(int num_steps) override {}
  void DeleteHistory() override {}
  void SelectAndCopy() {}
  bool ShouldShowFreeCopy() { return false; }
  void PasswordSuggestionSelected(int list_index) override {}
  void UpdateBrowserControlsState(int constraints,
                                  int current,
                                  bool animate) override {}
  void UpdateBrowserControlsHeight(int height, bool animate) override {}
  void PrefetchPage(const OHOS::NWeb::PrefetchOptions& prefetch_options) override {
  }
  void ReloadOriginalUrl() override {}
  bool CanStoreWebArchive() override { return false; }
  void SetBrowserUserAgentString(const CefString& user_agent) override {}
  bool ShouldShowLoadingUI() override { return false; }
  void SetForceEnableZoom(bool forceEnableZoom) override {}
  bool GetForceEnableZoom() override { return false; }
  int GetNWebId() override { return 0; }
  void SetEnableBlankTargetPopupIntercept(
      bool enableBlankTargetPopup) {}
  bool GetSavePasswordAutomatically() override { return false; }
  void SetSavePasswordAutomatically(bool enable) override {}
  void SaveOrUpdatePassword(bool is_update) override {}
  bool GetSavePassword() override { return false; }
  void SetSavePassword(bool enable) override {}
  int GetSecurityLevel() override { return 0; }
  void EnableSafeBrowsing(bool enable) override {}
  bool IsSafeBrowsingEnabled() override { return false; }
  void EnableIntelligentTrackingPrevention(bool enable) override {}
  bool IsIntelligentTrackingPreventionEnabled() override { return false; }
  bool IsAdsBlockEnabled() override { return false; }
  bool IsAdsBlockEnabledForCurPage() override { return false; }
  void EnableAdsBlock(bool enable) override {}
  int SetUrlTrustListWithErrMsg(const CefString& urlTrustList,
                                CefString& detailErrMsg) override {
    return 0;
  }
  void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) override {}
#endif  // BUILDFLAG(IS_OHOS)
  bool NeedToFireBeforeUnloadOrUnloadEvents() override {}
  void DispatchBeforeUnload() override {}
  void ShowFreeCopyMenu() override {}
  bool ShouldShowFreeCopyMenu() override {}
  void EnableSafeBrowsingDetection(bool enable, bool strictMode) override {}
  void ExtensionSetTabId(int tab_id) override {}
  int ExtensionGetTabId() override {}
  uint32_t GetAcceleratedWidget(bool isPopup) override {}
  void SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                        int main_frame_tree_node_id) override {}
#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  int PrerenderPage(const CefString& url,
                    const CefString& additional_headers) override { return OHOS::NWeb::NWEB_OK; };
  void CancelAllPrerendering() override {};
#endif
};

class NWebHandlerDelegateTest : public ::testing::Test {
 protected:
  void SetUp();
  void TearDown();

  CefRefPtr<NWebHandlerDelegate> delegate;
  MockNWebHandler* mock_handler_;
  MockEventHandler* mock_event_handler_;
  MockRenderHandler* mock_render_handler_;

  CefRefPtr<CefJSDialogCallback> callback_;
  CefKeyEvent event_;
  CefRefPtr<CefBrowser> browser_ = nullptr;
  CefEventHandle os_event_ = nullptr;
  CefString message_text_;
  bool is_reload_ = false;
  bool suppress_message = true;
  std::shared_ptr<MockWebAppClientExtensionListener> listener;
};

void NWebHandlerDelegateTest::SetUp() {
  delegate = new NWebHandlerDelegate(nullptr, nullptr, nullptr, nullptr, false,
                                     nullptr);

  mock_handler_ = new MockNWebHandler();
  mock_event_handler_ = new MockEventHandler();
  mock_render_handler_ = new MockRenderHandler();

  callback_ = CefRefPtr<CefJSDialogCallback>();

  delegate->nweb_handler_ = std::shared_ptr<MockNWebHandler>(mock_handler_);
  delegate->event_handler_ =
      std::shared_ptr<MockEventHandler>(mock_event_handler_);

  listener = std::shared_ptr<MockWebAppClientExtensionListener>();
  delegate->web_app_client_extension_listener_ = listener;
}

void NWebHandlerDelegateTest::TearDown() {
  delegate = nullptr;
  mock_handler_ = nullptr;
}

// Test cases
TEST_F(NWebHandlerDelegateTest, GetFocusHandler) {
  CefRefPtr<CefFocusHandler> focusHandler = delegate->GetFocusHandler();
  EXPECT_EQ(focusHandler.get(), delegate);
}

TEST_F(NWebHandlerDelegateTest, GetJSDialogHandler) {
  CefRefPtr<CefJSDialogHandler> jsDialogHandler =
      delegate->GetJSDialogHandler();
  EXPECT_EQ(jsDialogHandler.get(), delegate);
}

TEST_F(NWebHandlerDelegateTest, GetDialogHandler) {
  CefRefPtr<CefDialogHandler> dialogHandler = delegate->GetDialogHandler();
  EXPECT_EQ(dialogHandler.get(), delegate);
}

TEST_F(NWebHandlerDelegateTest, GetContextMenuHandler) {
  CefRefPtr<CefContextMenuHandler> contextMenuHandler =
      delegate->GetContextMenuHandler();
  EXPECT_EQ(contextMenuHandler.get(), delegate);
}

TEST_F(NWebHandlerDelegateTest, GetKeyboardHandler) {
  CefRefPtr<CefKeyboardHandler> keyboardHandler =
      delegate->GetKeyboardHandler();
  EXPECT_EQ(keyboardHandler.get(), delegate);
}

#if BUILDFLAG(ARKWEB_PRINT)
TEST_F(NWebHandlerDelegateTest, GetPrintHandler) {
  CefRefPtr<CefPrintHandler> printHandler = delegate->GetPrintHandler();
  EXPECT_EQ(printHandler.get(), delegate);
}
#endif  // BUILDFLAG(ARKWEB_PRINT)

TEST_F(NWebHandlerDelegateTest, GetFormHandler) {
  CefRefPtr<CefFormHandler> formHandler = delegate->GetFormHandler();
  EXPECT_EQ(formHandler.get(), delegate);
}

TEST_F(NWebHandlerDelegateTest, GetFrameHandler) {
  CefRefPtr<CefFrameHandler> frameHandler = delegate->GetFrameHandler();
  EXPECT_EQ(frameHandler.get(), delegate);
}

TEST_F(NWebHandlerDelegateTest, OnPreKeyEvent_TEST001) {
  CefKeyEvent event;
  event.type = KEYEVENT_RAWKEYDOWN;
  event.windows_key_code = 65;

  EXPECT_CALL(*mock_handler_, OnPreKeyEvent(::testing::_))
      .WillOnce(::testing::Return(true));

  bool is_keyboard_shortcut = false;
  bool result =
      delegate->OnPreKeyEvent(nullptr, event, nullptr, &is_keyboard_shortcut);

  EXPECT_TRUE(result);
}

TEST_F(NWebHandlerDelegateTest, OnPreKeyEvent_TEST002) {
  CefKeyEvent event;
  event.type = static_cast<cef_key_event_type_t>(-1);
  event.windows_key_code = 65;

  bool is_keyboard_shortcut = false;
  bool result =
      delegate->OnPreKeyEvent(nullptr, event, nullptr, &is_keyboard_shortcut);

  EXPECT_FALSE(result);
}

TEST_F(NWebHandlerDelegateTest, OnPreKeyEvent_TEST003) {
  CefKeyEvent event;
  event.type = KEYEVENT_RAWKEYDOWN;
  event.windows_key_code = -1;

  bool is_keyboard_shortcut = false;
  bool result =
      delegate->OnPreKeyEvent(nullptr, event, nullptr, &is_keyboard_shortcut);

  EXPECT_FALSE(result);
}

TEST_F(NWebHandlerDelegateTest, OnPreKeyEvent_TEST004) {
  delegate->nweb_handler_ = nullptr;

  CefKeyEvent event;
  event.type = KEYEVENT_RAWKEYDOWN;
  event.windows_key_code = 65;

  bool is_keyboard_shortcut = false;
  bool result =
      delegate->OnPreKeyEvent(nullptr, event, nullptr, &is_keyboard_shortcut);

  EXPECT_FALSE(result);
}

// OnKeyEvent
TEST_F(NWebHandlerDelegateTest, OnKeyEvent_TEST001) {
  event_.type = KEYEVENT_RAWKEYDOWN;
  event_.windows_key_code = 65;

  EXPECT_CALL(*mock_handler_, OnUnProcessedKeyEvent(::testing::_))
      .WillOnce(::testing::Return(true));

  bool result = delegate->OnKeyEvent(browser_, event_, os_event_);
  EXPECT_TRUE(result);
}

TEST_F(NWebHandlerDelegateTest, OnKeyEvent_TEST002) {
  event_.type = static_cast<cef_key_event_type_t>(-1);
  event_.windows_key_code = 65;

  bool result = delegate->OnKeyEvent(browser_, event_, os_event_);
  EXPECT_FALSE(result);
}

TEST_F(NWebHandlerDelegateTest, OnKeyEvent_TEST003) {
  event_.type = KEYEVENT_RAWKEYDOWN;
  event_.windows_key_code = -1;

  bool result = delegate->OnKeyEvent(browser_, event_, os_event_);
  EXPECT_FALSE(result);
}

TEST_F(NWebHandlerDelegateTest, OnKeyEvent_TEST004) {
  delegate->nweb_handler_ = nullptr;

  event_.type = KEYEVENT_RAWKEYDOWN;
  event_.windows_key_code = 65;

  bool result = delegate->OnKeyEvent(browser_, event_, os_event_);
  EXPECT_FALSE(result);
}

// showpassword
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
TEST_F(NWebHandlerDelegateTest, ShowPasswordDialog_TEST001) {
  EXPECT_CALL(*listener, OnSaveOrUpdatePassword(true, "http://example.com", _))
      .Times(1);
  delegate->ShowPasswordDialog(true, "http://example.com");
}

TEST_F(NWebHandlerDelegateTest, ShowPasswordDialog_TEST002) {
  EXPECT_CALL(*listener, OnSaveOrUpdatePassword(false, "http://example.com", _))
      .Times(1);
  delegate->ShowPasswordDialog(false, "http://example.com");
}
#endif  // ARKWEB_EXT_PASSWORD

#if BUILDFLAG(ARKWEB_FOCUS)
TEST_F(NWebHandlerDelegateTest, GetFocusState) {
  EXPECT_FALSE(delegate->GetFocusState());
}

TEST_F(NWebHandlerDelegateTest, SetFocusState_TEST001) {
  delegate->SetFocusState(true);
  EXPECT_TRUE(delegate->GetFocusState());
}

TEST_F(NWebHandlerDelegateTest, SetFocusState_TEST002) {
  delegate->SetFocusState(false);
  EXPECT_FALSE(delegate->GetFocusState());
}
#endif  // #if BUILDFLAG(ARKWEB_FOCUS)

TEST_F(NWebHandlerDelegateTest, OnSetFocus_TEST001) {
  CefRefPtr<MockCefBrowser> browser =
      CefRefPtr<MockCefBrowser>(new MockCefBrowser());
  EXPECT_CALL(*mock_handler_, OnFocus(::testing::_))
      .WillOnce(::testing::Return(false));

  delegate->OnSetFocus(browser_, FOCUS_SOURCE_NAVIGATION);
}

TEST_F(NWebHandlerDelegateTest, OnSetFocus_TEST002) {
  CefRefPtr<MockCefBrowser> browser =
      CefRefPtr<MockCefBrowser>(new MockCefBrowser());
  delegate->render_handler_ = nullptr;
  EXPECT_CALL(*mock_handler_, OnFocus(::testing::_))
      .WillOnce(::testing::Return(true));
  ON_CALL(*mock_event_handler_, SetIsFocus(true));

  delegate->OnSetFocus(browser_, FOCUS_SOURCE_NAVIGATION);
}

TEST_F(NWebHandlerDelegateTest, OnJSDialog_TEST001) {
  CefRefPtr<MockCefBrowser> browser =
      CefRefPtr<MockCefBrowser>(new MockCefBrowser());
  EXPECT_CALL(*mock_handler_, OnAlertDialogByJS("http://example.com",
                                                "Alert Message", ::testing::_))
      .WillOnce(::testing::Return(true));

  bool result =
      delegate->OnJSDialog(browser_, "http://example.com", JSDIALOGTYPE_ALERT,
                           "Alert Message", "", callback_, suppress_message);
  EXPECT_TRUE(result);
  EXPECT_FALSE(suppress_message);
}

TEST_F(NWebHandlerDelegateTest, OnJSDialog_TEST002) {
  CefRefPtr<MockCefBrowser> browser =
      CefRefPtr<MockCefBrowser>(new MockCefBrowser());
  EXPECT_CALL(*mock_handler_,
              OnConfirmDialogByJS("http://example.com", "Confirm Message",
                                  ::testing::_))
      .WillOnce(::testing::Return(true));

  bool result =
      delegate->OnJSDialog(browser_, "http://example.com", JSDIALOGTYPE_CONFIRM,
                           "Confirm Message", "", callback_, suppress_message);
  EXPECT_TRUE(result);
  EXPECT_FALSE(suppress_message);
}

TEST_F(NWebHandlerDelegateTest, OnJSDialog_TEST003) {
  CefRefPtr<MockCefBrowser> browser =
      CefRefPtr<MockCefBrowser>(new MockCefBrowser());
  EXPECT_CALL(*mock_handler_,
              OnPromptDialogByJS("http://example.com", "Prompt Message",
                                 "Default Text", ::testing::_))
      .WillOnce(::testing::Return(true));

  bool result = delegate->OnJSDialog(
      browser_, "http://example.com", JSDIALOGTYPE_PROMPT, "Prompt Message",
      "Default Text", callback_, suppress_message);
  EXPECT_TRUE(result);
  EXPECT_FALSE(suppress_message);
}

TEST_F(NWebHandlerDelegateTest, OnJSDialog_TEST004) {
  CefRefPtr<MockCefBrowser> browser =
      CefRefPtr<MockCefBrowser>(new MockCefBrowser());
  delegate->nweb_handler_ = nullptr;
  bool result =
      delegate->OnJSDialog(browser_, "http://example.com", JSDIALOGTYPE_ALERT,
                           "Alert Message", "", callback_, suppress_message);
  EXPECT_FALSE(result);
  EXPECT_TRUE(suppress_message);
}

TEST_F(NWebHandlerDelegateTest, OnJSDialog_TEST005) {
  CefRefPtr<MockCefBrowser> browser =
      CefRefPtr<MockCefBrowser>(new MockCefBrowser());
  bool result =
      delegate->OnJSDialog(browser_, "http://example.com",
                           static_cast<CefJSDialogHandler::JSDialogType>(-1),
                           "Unknown Message", "", callback_, suppress_message);
  EXPECT_FALSE(result);
  EXPECT_FALSE(suppress_message);
}

TEST_F(NWebHandlerDelegateTest, OnBeforeUnloadDialog) {
  mock_handler_ = nullptr;
  bool result = delegate->OnBeforeUnloadDialog(browser_, message_text_,
                                               is_reload_, callback_);
  EXPECT_FALSE(result);
}

TEST_F(NWebHandlerDelegateTest, OnFileDialog) {
  CefString title = "Select a file";
  CefString default_path = "default_path";
  std::vector<CefString> accept_filters = {"*.txt", "*.jpg"};
  std::vector<CefString> accept_extensions;
  std::vector<CefString> accept_descriptions;
  std::vector<CefString> mime_filters;
  CefString accepts = u"";
  CefString start_in = u"";
  bool is_exclude_accept_all_options = false;
  bool capture = false;
  CefRefPtr<CefFileDialogCallback> callback;

  EXPECT_FALSE(delegate->OnFileDialog(browser_, FILE_DIALOG_OPEN, title,
                                      default_path, accept_filters, accept_extensions,
                                      accept_descriptions, accepts, start_in, is_exclude_accept_all_options,
                                      capture, mime_filters, callback));
}

TEST_F(NWebHandlerDelegateTest, OnBeforeContextMenu) {
  CefRefPtr<CefBrowser> browserPtr = nullptr;
  CefRefPtr<CefFrame> framePtr = nullptr;
  CefRefPtr<CefContextMenuParams> paramsPtr = nullptr;
  CefRefPtr<CefMenuModel> modelPtr = nullptr;

  delegate->OnBeforeContextMenu(browserPtr, framePtr, paramsPtr, modelPtr);
  ASSERT_NE(delegate, nullptr);
}

TEST_F(NWebHandlerDelegateTest, RunContextMenu) {
  CefRefPtr<CefBrowser> browserPtr = nullptr;
  CefRefPtr<CefFrame> framePtr = nullptr;
  CefRefPtr<CefContextMenuParams> paramsPtr = nullptr;
  CefRefPtr<CefMenuModel> modelPtr = nullptr;

  delegate->OnBeforeContextMenu(browserPtr, framePtr, paramsPtr, modelPtr);
  ASSERT_NE(delegate, nullptr);
}

TEST_F(NWebHandlerDelegateTest, HasJavaScriptObjectMethods) {
  int32_t object_id = 1;
  CefString method_name = "testMethod";

  bool result = delegate->HasJavaScriptObjectMethods(object_id, method_name);
  EXPECT_FALSE(result);
}

TEST_F(NWebHandlerDelegateTest, GetJavaScriptObjectMethods) {
  int32_t object_id = 1;
  CefRefPtr<CefValue> returned_method_names = nullptr;

  delegate->GetJavaScriptObjectMethods(object_id, returned_method_names);
  ASSERT_NE(delegate, nullptr);
}

TEST_F(NWebHandlerDelegateTest, IsShowHandle) {
  ASSERT_NE(delegate, nullptr);
  auto result = delegate->IsShowHandle();
  EXPECT_FALSE(result);
}
