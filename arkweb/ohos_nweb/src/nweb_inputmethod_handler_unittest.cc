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

#include "cef_delegate/nweb_inputmethod_client.h"
#include "condition_variable"
#define private public

#include "nweb_inputmethod_handler.h"

#include <gmock/gmock.h>

#include "gtest/gtest.h"

namespace OHOS::NWeb {

class NWebInputMethodHandlerTest : public testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();

 private:
  CefRefPtr<NWebInputMethodHandler> inputmethod_handler_ = nullptr;
};

class MockCefBrowser : public CefBrowser {
 public:
  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return false; }
  bool HasAtLeastOneRef() const override { return false; }
  bool IsValid() override { return false; }
  bool CanGoBack() override { return false; }
  void GoBack() override {}
  bool CanGoForward() override { return false; }
  void GoForward() override {}
  bool IsLoading() override { return false; }
  void Reload() override {}
  void ReloadIgnoreCache() override {}
  void StopLoad() override {}
  int GetIdentifier() override { return -1; }
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
  bool NeedToFireBeforeUnloadOrUnloadEvents() override { return false; }
  void DispatchBeforeUnload() override {}
  MOCK_METHOD(CefRefPtr<ArkWebBrowserHostExt>, GetHost, (), (override));
};

class MockCefBrowserExt : public ArkWebBrowserExt {
 public:
  CefRefPtr<CefBrowserPermissionRequestDelegate> GetPermissionRequestDelegate()
      override {
    return nullptr;
  }
  CefRefPtr<CefGeolocationAcess> GetGeolocationPermissions() override {
    return nullptr;
  }
#if BUILDFLAG(IS_OHOS)
  bool CanGoBackOrForward(int num_steps) override { return false; }
  void GoBackOrForward(int num_steps) override {}
  void DeleteHistory() override {}
  void ShowFreeCopyMenu() override {}
  bool ShouldShowFreeCopyMenu() override { return false; }
  void PasswordSuggestionSelected(int list_index) override {}
  void UpdateBrowserControlsState(int constraints,
                                  int current,
                                  bool animate) override {}
  void UpdateBrowserControlsHeight(int height, bool animate) override {}
  void PrefetchPage(CefString& url, CefString& additionalHttpHeaders) override {
  }
  void ReloadOriginalUrl() override {}
  bool CanStoreWebArchive() override { return false; }
  void SetBrowserUserAgentString(const CefString& user_agent) override {}
  bool ShouldShowLoadingUI() override { return false; }
  void SetForceEnableZoom(bool forceEnableZoom) override {}
  bool GetForceEnableZoom() override { return false; }
  int GetNWebId() override { return -1; }
  bool GetSavePasswordAutomatically() override { return false; }
  void SetSavePasswordAutomatically(bool enable) override {}
  void SaveOrUpdatePassword(bool is_update) override {}
  bool GetSavePassword() override { return false; }
  void SetSavePassword(bool enable) override {}
  int GetSecurityLevel() override { return -1; }
  void EnableSafeBrowsing(bool enable) override {}
  bool IsSafeBrowsingEnabled() override { return false; }
  void EnableIntelligentTrackingPrevention(bool enable) override {}
  bool IsIntelligentTrackingPreventionEnabled() override { return false; }
  bool IsAdsBlockEnabled() override { return false; }
  bool IsAdsBlockEnabledForCurPage() override { return false; }
  void EnableAdsBlock(bool enable) override {}
  int SetUrlTrustListWithErrMsg(const CefString& urlTrustList,
                                CefString& detailErrMsg) override {
    return -1;
  }
  void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) override {}
#endif  // BUILDFLAG(IS_OHOS)
};

class MockOnTextChangedListenerImpl : public IMFTextListenerAdapter {
 public:
  MockOnTextChangedListenerImpl() = default;
  ~MockOnTextChangedListenerImpl() = default;
  void InsertText(const std::u16string& text) override {}
  void DeleteForward(int32_t length) override {}
  void DeleteBackward(int32_t length) override {}
  void SendKeyEventFromInputMethod() override {}
  void SendKeyboardStatus(const IMFAdapterKeyboardStatus& status) override {}
  void SendFunctionKey(
      std::shared_ptr<IMFAdapterFunctionKeyAdapter> functionKey) override {}
  void SetKeyboardStatus(bool status) override {}
  void MoveCursor(const IMFAdapterDirection direction) override {}
  void HandleSetSelection(int32_t start, int32_t end) override {}
  void HandleExtendAction(int32_t action) override {}
  void HandleSelect(int32_t keyCode, int32_t cursorMoveSkip) override {}

  int32_t GetTextIndexAtCursor() override {
    return handler_->GetTextIndexAtCursor();
  }

  std::u16string GetLeftTextOfCursor(int32_t number) override {
    return handler_->GetLeftTextOfCursor(number);
  }

  std::u16string GetRightTextOfCursor(int32_t number) override {
    return handler_->GetRightTextOfCursor(number);
  }

  int32_t SetPreviewText(const std::u16string& text,
                         int32_t start,
                         int32_t end) override {
    return handler_->SetPreviewText(text, start, end);
  }

  void FinishTextPreview() override { return handler_->FinishTextPreview(); }

  void SetNeedUnderLine(bool is_need_underline) override {
    handler_->SetNeedUnderLine(is_need_underline);
  }

 private:
  NWebInputMethodHandler* handler_;
};

class MockIMFAdapterImpl : public IMFAdapter {
 public:
  MockIMFAdapterImpl() = default;
  ~MockIMFAdapterImpl() override = default;
  MOCK_METHOD(bool,
              Attach,
              (std::shared_ptr<IMFTextListenerAdapter> listener,
               bool isShowKeyboard),
              (override));
  MOCK_METHOD(bool,
              Attach,
              (std::shared_ptr<IMFTextListenerAdapter> listener,
               bool isShowKeyboard,
               const std::shared_ptr<IMFTextConfigAdapter> config,
               bool isResetListener),
              (override));
  MOCK_METHOD(bool,
              AttachWithRequestKeyboardReason,
              (std::shared_ptr<IMFTextListenerAdapter> listener,
               bool isShowKeyboard,
               const std::shared_ptr<IMFTextConfigAdapter> config,
               bool isResetListener,
               int32_t requestKeyboardReason),
              (override));
  MOCK_METHOD(void,
              ShowCurrentInput,
              (const IMFAdapterTextInputType& inputType),
              (override));
  MOCK_METHOD(void, HideTextInput, (), (override));
  MOCK_METHOD(void, Close, (), (override));
  MOCK_METHOD(void,
              OnCursorUpdate,
              (const std::shared_ptr<IMFCursorInfoAdapter> cursorInfo),
              (override));
  MOCK_METHOD(void,
              OnSelectionChange,
              (std::u16string text, int start, int end),
              (override));
};

void NWebInputMethodHandlerTest::SetUpTestCase(void) {}

void NWebInputMethodHandlerTest::TearDownTestCase(void) {}

void NWebInputMethodHandlerTest::SetUp(void) {
  inputmethod_handler_ = new NWebInputMethodHandler();
  ASSERT_NE(inputmethod_handler_, nullptr);
}

void NWebInputMethodHandlerTest::TearDown(void) {
  if (inputmethod_handler_) {
    inputmethod_handler_ = nullptr;
  }
}

TEST_F(NWebInputMethodHandlerTest, GetCursorInfo) {
  if (!inputmethod_handler_) {
  }
  inputmethod_handler_->focus_rect_.Set(10, 10, 10, 10);
  inputmethod_handler_->device_pixel_ratio_ = 2;
  inputmethod_handler_->offset_x_ = 100;
  inputmethod_handler_->offset_y_ = 100;

  std::shared_ptr<IMFCursorInfoAdapter> cursorInfo =
      inputmethod_handler_->GetCursorInfo();
  EXPECT_NE(cursorInfo, nullptr);

  EXPECT_EQ(cursorInfo->GetLeft(), 140.0);
  EXPECT_EQ(cursorInfo->GetTop(), 120.0);
  EXPECT_EQ(cursorInfo->GetWidth(), 20.0);
  EXPECT_EQ(cursorInfo->GetHeight(), 20.0);
}

TEST_F(NWebInputMethodHandlerTest, Attach_inputmethod_adapter__Null) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->composing_text_.size(), 0);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_EnterKeyTypeValid) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = (int32_t)CEF_TEXT_INPUT_ACTION_DEFAULT;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::UNSPECIFIED);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::NONE;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NONE);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::GO;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::SEARCH;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::SEARCH);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::SEND;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::SEND);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::NEXT;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NEXT);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::DONE;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::DONE);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::PREVIOUS;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::PREVIOUS);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::NEW_LINE;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NEW_LINE);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_01) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_02) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TEXT;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_03) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_PASSWORD;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::VISIBLE_PASSWORD);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_04) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_SEARCH;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::SEARCH);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_05) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_EMAIL;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::EMAIL_ADDRESS);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_06) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NUMBER;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::NUMBER);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_07) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TELEPHONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::PHONE);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_08) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_URL;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::URL);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_09) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_DATE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_10) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_DATE_TIME;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_11) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_DATE_TIME_LOCAL;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_12) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_MONTH;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_13) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TIME;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_14) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_WEEK;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_15) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TEXT_AREA;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NEW_LINE);
  EXPECT_EQ(inputmethod_handler_->type_text_flag_multi_line_, true);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_16) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_CONTENT_EDITABLE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NEW_LINE);
  EXPECT_EQ(inputmethod_handler_->type_text_flag_multi_line_, true);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_17) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_DATE_TIME_FIELD;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_18) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TYPE_NULL;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_19) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_MAX;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_20) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_NONE;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NEW_LINE);
  EXPECT_EQ(inputmethod_handler_->type_text_flag_multi_line_, true);
}

TEST_F(NWebInputMethodHandlerTest, Attach_inputmethod_listener__Null) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;

  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener,
                               enterKeyType);
}

TEST_F(NWebInputMethodHandlerTest, Attach_Normal_01) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ =
      std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(
      mock_inputmethod_adapter_.get());
  inputmethod_handler_->inputmethod_listener_ =
      std::make_shared<MockOnTextChangedListenerImpl>();
  inputmethod_handler_->isAttached_ = false;

  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_29) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_PASSWORD;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::VISIBLE_PASSWORD);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_Normal_02) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;
  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ =
      std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(
      mock_inputmethod_adapter_.get());
  inputmethod_handler_->inputmethod_listener_ =
      std::make_shared<MockOnTextChangedListenerImpl>();

  inputmethod_handler_->focus_status_ = false;
  inputmethod_handler_->focus_rect_status_ = false;
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, Attach_Normal_03) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;
  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ =
      std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(
      mock_inputmethod_adapter_.get());
  inputmethod_handler_->inputmethod_listener_ =
      std::make_shared<MockOnTextChangedListenerImpl>();

  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->focus_rect_status_ = false;
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_28) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_MAX;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NEW_LINE);
}

TEST_F(NWebInputMethodHandlerTest, Attach_Normal_04) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ =
      std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(
      mock_inputmethod_adapter_.get());
  inputmethod_handler_->inputmethod_listener_ =
      std::make_shared<MockOnTextChangedListenerImpl>();

  inputmethod_handler_->focus_status_ = false;
  inputmethod_handler_->focus_rect_status_ = true;
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_27) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_SEARCH;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::NEW_LINE);
  EXPECT_EQ(inputmethod_handler_->type_text_flag_multi_line_, true);
}

TEST_F(NWebInputMethodHandlerTest, Attach_Normal_05) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ =
      std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(
      mock_inputmethod_adapter_.get());
  inputmethod_handler_->inputmethod_listener_ =
      std::make_shared<MockOnTextChangedListenerImpl>();

  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->focus_rect_status_ = true;
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_CONTINUE_False) {
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_CONTINUE;
  inputmethod_handler_->isNeedReattachOncontinue_ = false;
  inputmethod_handler_->is_editable_node_ = false;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(result, false);

  inputmethod_handler_->isNeedReattachOncontinue_ = false;
  inputmethod_handler_->is_editable_node_ = true;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(result, false);

  inputmethod_handler_->isNeedReattachOncontinue_ = true;
  inputmethod_handler_->is_editable_node_ = false;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_CONTINUE_False3) {
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_CONTINUE;

  inputmethod_handler_->isNeedReattachOncontinue_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = true;

  inputmethod_handler_->lastInputMode_ = IMFAdapterTextInputType::TEXT;
  inputmethod_handler_->imf_input_mode_ = IMFAdapterTextInputType::NUMBER;

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ =
      std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(
      mock_inputmethod_adapter_.get());
  EXPECT_CALL(*mock_inputmethod_adapter_, Close()).Times(1);

  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOncontinue_, false);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_CONTINUE_False2) {
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_CONTINUE;

  inputmethod_handler_->isNeedReattachOncontinue_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = false;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOncontinue_, false);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_CONTINUE_True) {
  uint32_t nwebId = 1;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_CONTINUE;

  inputmethod_handler_->isNeedReattachOncontinue_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = true;

  inputmethod_handler_->lastInputMode_ = IMFAdapterTextInputType::TEXT;
  inputmethod_handler_->imf_input_mode_ = IMFAdapterTextInputType::NUMBER;

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ =
      std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(
      mock_inputmethod_adapter_.get());

  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOncontinue_, false);
  EXPECT_EQ(inputmethod_handler_->isAttached_, true);
  EXPECT_EQ(inputmethod_handler_->lastAttachNWebId_, nwebId);
  EXPECT_EQ(inputmethod_handler_->lastInputMode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_ONFOCUS_False) {
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_ONFOCUS;
  inputmethod_handler_->isNeedReattachOnfocus_ = false;
  inputmethod_handler_->is_editable_node_ = false;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(result, false);

  inputmethod_handler_->isNeedReattachOnfocus_ = false;
  inputmethod_handler_->is_editable_node_ = true;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(result, false);

  inputmethod_handler_->isNeedReattachOnfocus_ = true;
  inputmethod_handler_->is_editable_node_ = false;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_ONFOCUS_False2) {
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_ONFOCUS;

  inputmethod_handler_->isNeedReattachOnfocus_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = false;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOnfocus_, true);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_ONFOCUS_False3) {
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_ONFOCUS;

  inputmethod_handler_->isNeedReattachOnfocus_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = true;

  inputmethod_handler_->lastInputMode_ = IMFAdapterTextInputType::TEXT;
  inputmethod_handler_->imf_input_mode_ = IMFAdapterTextInputType::NUMBER;

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ =
      std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(
      mock_inputmethod_adapter_.get());

  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOnfocus_, true);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, ShowTextInput) {
  inputmethod_handler_->ShowTextInput();
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_inputmethod_adapter__Null) {
  inputmethod_handler_->isAttached_ = true;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  uint32_t nwebId = 0;
  NWebInputMethodClient::HideTextinputType hideType =
      NWebInputMethodClient::HideTextinputType::FROM_KERNEL;
  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, true);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_ONFOCUS_True) {
  uint32_t nwebId = 1;
  bool result = false;
  NWebInputMethodHandler::ReattachType type =
      NWebInputMethodHandler::ReattachType::FROM_ONFOCUS;

  inputmethod_handler_->isNeedReattachOnfocus_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = true;

  inputmethod_handler_->lastInputMode_ = IMFAdapterTextInputType::TEXT;
  inputmethod_handler_->imf_input_mode_ = IMFAdapterTextInputType::NUMBER;

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ =
      std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(
      mock_inputmethod_adapter_.get());

  EXPECT_CALL(*mock_inputmethod_adapter_, Close()).Times(1);

  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOnfocus_, true);
  EXPECT_EQ(inputmethod_handler_->isAttached_, true);
  EXPECT_EQ(inputmethod_handler_->lastAttachNWebId_, nwebId);
  EXPECT_EQ(inputmethod_handler_->lastInputMode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_21) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_TEXT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_22) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_TEL;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::PHONE);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_isAttached_False) {
  inputmethod_handler_->isAttached_ = false;
  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ =
      std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(
      mock_inputmethod_adapter_.get());

  uint32_t nwebId = 0;
  NWebInputMethodClient::HideTextinputType hideType =
      NWebInputMethodClient::HideTextinputType::FROM_KERNEL;
  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_23) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_URL;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::URL);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_isAttached_False2) {
  inputmethod_handler_->isAttached_ = false;
  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ =
      std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(
      mock_inputmethod_adapter_.get());

  uint32_t nwebId = 0;
  NWebInputMethodClient::HideTextinputType hideType =
      NWebInputMethodClient::HideTextinputType::FROM_ONPAUSE;
  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}
TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_24) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_EMAIL;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::EMAIL_ADDRESS);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_isAttached_False3) {
  inputmethod_handler_->isAttached_ = false;
  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ =
      std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(
      mock_inputmethod_adapter_.get());

  uint32_t nwebId = 0;
  NWebInputMethodClient::HideTextinputType hideType =
      NWebInputMethodClient::HideTextinputType::FROM_ONPAUSE;
  inputmethod_handler_->lastCloseInputMethodTime_ =
      std::chrono::high_resolution_clock::now();
  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}
TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_25) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_NUMERIC;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::NUMBER);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_Normal) {
  inputmethod_handler_->isAttached_ = true;
  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ =
      std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(
      mock_inputmethod_adapter_.get());

  uint32_t nwebId = 0;
  NWebInputMethodClient::HideTextinputType hideType =
      NWebInputMethodClient::HideTextinputType::FROM_ONPAUSE;
  inputmethod_handler_->lastCloseInputMethodTime_ =
      std::chrono::high_resolution_clock::now();
  inputmethod_handler_->lastAttachNWebId_ = 0;

  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}
TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_26) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DECIMAL;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener,
                               enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_,
            IMFAdapterTextInputType::NUMBER);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_,
            IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_Normal02) {
  inputmethod_handler_->isAttached_ = true;
  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ =
      std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(
      mock_inputmethod_adapter_.get());

  uint32_t nwebId = 0;
  NWebInputMethodClient::HideTextinputType hideType =
      NWebInputMethodClient::HideTextinputType::FROM_ONBLUR;
  inputmethod_handler_->lastCloseInputMethodTime_ =
      std::chrono::high_resolution_clock::now();
  inputmethod_handler_->lastAttachNWebId_ = 0;

  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest,
       HideTextInputForce_inputmethod_adapter_Null) {
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputmethod_handler_->HideTextInputForce();
}

TEST_F(NWebInputMethodHandlerTest, HideTextInputForce_Normal) {
  inputmethod_handler_->HideTextInputForce();
}

TEST_F(NWebInputMethodHandlerTest, OnTextSelectionChanged) {
  CefString selected_text = CefString("test");
  CefRange selected_range = CefRange(0, 3);
  inputmethod_handler_->OnTextSelectionChanged(nullptr, selected_text,
                                               selected_range);
  EXPECT_EQ(inputmethod_handler_->selected_text_.size(), 4);
}

TEST_F(NWebInputMethodHandlerTest, OnCursorUpdate) {
  CefRect rect;
  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->OnCursorUpdate(rect);
}

TEST_F(NWebInputMethodHandlerTest, OnSelectionChanged) {
  inputmethod_handler_->OnSelectionChanged(nullptr, CefString(""),
                                           CefRange(0, 1));
}

TEST_F(NWebInputMethodHandlerTest, SendEnterKeyEvent) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  inputmethod_handler_->SendEnterKeyEvent(1);

  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->SendEnterKeyEvent(1);
}

#if BUILDFLAG(ARKWEB_CLIPBOARD)
TEST_F(NWebInputMethodHandlerTest, GetSelectInfo) {
  inputmethod_handler_->GetSelectInfo();
}
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

TEST_F(NWebInputMethodHandlerTest, ResetTextSelectiondata) {
  inputmethod_handler_->is_need_notify_all_ = false;
  bool result = inputmethod_handler_->ResetTextSelectiondata();
  EXPECT_EQ(result, false);

  inputmethod_handler_->is_need_notify_all_ = true;
  result = inputmethod_handler_->ResetTextSelectiondata();
  EXPECT_EQ(result, true);
  EXPECT_EQ(inputmethod_handler_->is_need_notify_all_, false);
}

TEST_F(NWebInputMethodHandlerTest, IsCorrectParam) {
  inputmethod_handler_->whole_text_ = u"test text";
  int32_t selectBegin = 5;
  int32_t selectEnd = 0;
  EXPECT_FALSE(inputmethod_handler_->IsCorrectParam(-1, selectBegin, selectEnd));

  EXPECT_TRUE(inputmethod_handler_->IsCorrectParam(1, selectBegin, selectEnd));
  EXPECT_EQ(selectBegin, 0);
  EXPECT_EQ(selectEnd, 5);

  selectBegin = 0;
  selectEnd = 5;
  EXPECT_TRUE(inputmethod_handler_->IsCorrectParam(1, selectBegin, selectEnd));
}

TEST_F(NWebInputMethodHandlerTest, SetWindowIdForIME) {
  uint32_t testWindowId = 12345;
  inputmethod_handler_->SetWindowIdForIME(testWindowId);
  EXPECT_EQ(inputmethod_handler_->windowId_, testWindowId);
}

TEST_F(NWebInputMethodHandlerTest, GetAllTextInfo) {
  inputmethod_handler_->whole_text_ = u"Hello 你好! 123";
  std::string result = inputmethod_handler_->GetAllTextInfo();
  EXPECT_EQ(result, "Hello 你好! 123");
}

TEST_F(NWebInputMethodHandlerTest, GetSelectEndIndex) {
  inputmethod_handler_->selected_to_ = 10;
  int32_t result = inputmethod_handler_->GetSelectEndIndex();
  EXPECT_EQ(result, 10);
}

TEST_F(NWebInputMethodHandlerTest, GetSelectStartIndex) {
  inputmethod_handler_->selected_from_ = 5;
  int32_t result = inputmethod_handler_->GetSelectStartIndex();
  EXPECT_EQ(result, 5);
}

TEST_F(NWebInputMethodHandlerTest, AutoFillWithIMFEventOnUI) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  EXPECT_TRUE(inputmethod_handler_->browser_);
  bool is_username = true;
  bool is_other_account = false;
  bool is_new_password = true;
  std::string content = "test_content";
  inputmethod_handler_->AutoFillWithIMFEventOnUI(is_username, is_other_account, is_new_password, content);
}

TEST_F(NWebInputMethodHandlerTest, AutoFillWithIMFEvent) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  EXPECT_TRUE(inputmethod_handler_->browser_);
  inputmethod_handler_->AutoFillWithIMFEvent(true, true, true, "test_content");
  EXPECT_NE(inputmethod_handler_->browser_->GetHost(), nullptr);

  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->AutoFillWithIMFEvent(true, true, true, "test_content");
  EXPECT_TRUE(inputmethod_handler_->browser_ == nullptr);
}

TEST_F(NWebInputMethodHandlerTest, FinishTextPreview) {
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->FinishTextPreview();

  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  EXPECT_TRUE(inputmethod_handler_->browser_);
  inputmethod_handler_->FinishTextPreview();
}

TEST_F(NWebInputMethodHandlerTest, SetPreviewText) {
  NWebInputMethodHandler handler;
  std::u16string text = u"test";
  int32_t start = 0;
  int32_t end = 4;
  EXPECT_EQ(inputmethod_handler_->SetPreviewText(text, start, end), inputmethod_handler_->ERROR);

  inputmethod_handler_->composition_type_ = COMPOSITION_CANCEL;
  EXPECT_EQ(inputmethod_handler_->SetPreviewText(text, start, end), inputmethod_handler_->ERROR);

  inputmethod_handler_->composition_type_ = COMPOSITION_DELETE;
  inputmethod_handler_->selected_from_ = 0;
  EXPECT_EQ(inputmethod_handler_->SetPreviewText(text, start, end), inputmethod_handler_->ERROR);

  inputmethod_handler_->composition_type_ = COMPOSITION_DELETE;
  inputmethod_handler_->selected_from_ = 4;
  EXPECT_EQ(inputmethod_handler_->SetPreviewText(text, start, end), inputmethod_handler_->ERROR);

  inputmethod_handler_->composition_type_ = COMPOSITION_INVALID;
  EXPECT_EQ(inputmethod_handler_->SetPreviewText(text, start, end), inputmethod_handler_->ERROR);
}

TEST_F(NWebInputMethodHandlerTest, UpdateCompositionInfo) {
  std::u16string text = u"test";
  int32_t start = 0;
  int32_t end = 4;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->ERROR);

  inputmethod_handler_->composition_type_ = COMPOSITION_INVALID;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->ERROR);

  inputmethod_handler_->composition_type_ = COMPOSITION_CANCEL;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->ERROR);

  inputmethod_handler_->composition_type_ = COMPOSITION_DELETE;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->ERROR);

  inputmethod_handler_->composition_type_ = COMPOSITION_CURRENT;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->ERROR);

  inputmethod_handler_->composition_type_ = COMPOSITION_POSITION;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->ERROR);

  inputmethod_handler_->composition_type_ = COMPOSITION_REPLACE;
  EXPECT_EQ(inputmethod_handler_->UpdateCompositionInfo(text, start, end), inputmethod_handler_->ERROR);
}

TEST_F(NWebInputMethodHandlerTest, GetCompositionTypeAndCheckInput) {
  inputmethod_handler_->has_composition_ = false;
  inputmethod_handler_->composition_range_start_ = 0;
  inputmethod_handler_->composition_range_end_ = 0;
  inputmethod_handler_->whole_text_ = u"";
  inputmethod_handler_->preview_text_cache_ = u"";
  CompositionType type;
  int32_t result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"", -1, -1, type);
  EXPECT_EQ(result, inputmethod_handler_->ERROR);
  EXPECT_EQ(type, COMPOSITION_INVALID);

  result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"test", -1, -1, type);
  EXPECT_EQ(result, inputmethod_handler_->OK);
  EXPECT_EQ(type, COMPOSITION_CURRENT);

  inputmethod_handler_->composition_range_end_ = 3;
  inputmethod_handler_->whole_text_ = u"test";
  result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"test", 0, 0, type);
  EXPECT_EQ(result, inputmethod_handler_->OK);
  EXPECT_EQ(type, COMPOSITION_POSITION);

  inputmethod_handler_->has_composition_ = true;
  result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"test", 0, 2, type);
  EXPECT_EQ(result, inputmethod_handler_->OK);
  EXPECT_EQ(type, COMPOSITION_REPLACE);

  inputmethod_handler_->preview_text_cache_ = u"a";
  result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"", 0, 0, type);
  EXPECT_EQ(result, inputmethod_handler_->OK);
  EXPECT_EQ(type, COMPOSITION_POSITION);

  result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"test", 4, 5, type);
  EXPECT_EQ(result, inputmethod_handler_->ERROR);
  EXPECT_EQ(type, COMPOSITION_REPLACE);

  result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"test", 2, 1, type);
  EXPECT_EQ(result, inputmethod_handler_->ERROR);
  EXPECT_EQ(type, COMPOSITION_INVALID);

  inputmethod_handler_->composition_range_start_ = 1;
  result = inputmethod_handler_->GetCompositionTypeAndCheckInput(u"test", 0, 4, type);
  EXPECT_EQ(result, inputmethod_handler_->ERROR);
  EXPECT_EQ(type, COMPOSITION_REPLACE);
}

TEST_F(NWebInputMethodHandlerTest, GetRightTextOfCursor) {
  inputmethod_handler_->textCursorReady_ = 1;
  inputmethod_handler_->selected_from_ = 0;
  inputmethod_handler_->selected_to_ = 5;
  inputmethod_handler_->whole_text_ = u"Hello, World!";
  inputmethod_handler_->is_need_notify_all_ = true;
  std::u16string result = inputmethod_handler_->GetRightTextOfCursor(5);
  EXPECT_EQ(result, u"");

  result = inputmethod_handler_->GetRightTextOfCursor(-5);
  EXPECT_EQ(result, u"");

  inputmethod_handler_->textCursorReady_ = 1;
  result = inputmethod_handler_->GetRightTextOfCursor(5);
  EXPECT_EQ(result, u"");
}

TEST_F(NWebInputMethodHandlerTest, GetLeftTextOfCursor) {
  inputmethod_handler_->textCursorReady_ = 0;
  inputmethod_handler_->selected_from_ = 10;
  inputmethod_handler_->selected_to_ = 20;
  inputmethod_handler_->whole_text_ = u"Hello, World!";
  inputmethod_handler_->textCursorReady_ = 1;
  inputmethod_handler_->is_need_notify_all_ = true;
  EXPECT_EQ(inputmethod_handler_->GetLeftTextOfCursor(5), u"");

  inputmethod_handler_->textCursorReady_ = 1;
  EXPECT_EQ(inputmethod_handler_->GetLeftTextOfCursor(5), u"");

  EXPECT_EQ(inputmethod_handler_->GetLeftTextOfCursor(-5), u"");

  EXPECT_EQ(inputmethod_handler_->GetLeftTextOfCursor(10), u"");
}

TEST_F(NWebInputMethodHandlerTest, GetTextIndexAtCursor) {
  inputmethod_handler_->is_need_notify_all_ = false;
  inputmethod_handler_->textCursorReady_ = 0;
  EXPECT_EQ(inputmethod_handler_->GetTextIndexAtCursor(), inputmethod_handler_->selected_to_);

  inputmethod_handler_->is_need_notify_all_ = true;
  EXPECT_EQ(inputmethod_handler_->GetTextIndexAtCursor(), 0);
}

TEST_F(NWebInputMethodHandlerTest, HasComposition) {
  inputmethod_handler_->has_composition_ = false;
  EXPECT_EQ(inputmethod_handler_->HasComposition(), inputmethod_handler_->has_composition_);
}

TEST_F(NWebInputMethodHandlerTest, GetIsEditableNode) {
  inputmethod_handler_->is_editable_node_ = true;
  bool result = inputmethod_handler_->GetIsEditableNode();
  EXPECT_TRUE(result);
}

TEST_F(NWebInputMethodHandlerTest, OnEditableChanged) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->OnEditableChanged(browser, true);
  EXPECT_TRUE(inputmethod_handler_->is_editable_node_);
}

TEST_F(NWebInputMethodHandlerTest, SetFocusStatus) {
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputmethod_handler_->focus_rect_.x = 10;
  inputmethod_handler_->focus_rect_.y = 20;
  inputmethod_handler_->focus_rect_.width = 30;
  inputmethod_handler_->focus_rect_.height = 40;
  inputmethod_handler_->device_pixel_ratio_ = 2.0;
  inputmethod_handler_->offset_x_ = 5;
  inputmethod_handler_->offset_y_ = 6;
  inputmethod_handler_->SetFocusStatus(true);
  EXPECT_TRUE(inputmethod_handler_->focus_status_);

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());
  inputmethod_handler_->SetFocusStatus(true);
  EXPECT_TRUE(inputmethod_handler_->focus_status_);

  inputmethod_handler_->SetFocusStatus(false);
  EXPECT_FALSE(inputmethod_handler_->focus_status_);
}

TEST_F(NWebInputMethodHandlerTest, SetVirtualDeviceRatio) {
  const float expectedRatio = -1.5f;
  inputmethod_handler_->SetVirtualDeviceRatio(expectedRatio);
  EXPECT_EQ(expectedRatio, inputmethod_handler_->device_pixel_ratio_);
}

TEST_F(NWebInputMethodHandlerTest, SetScreenOffSet) {
  inputmethod_handler_->focus_rect_status_ = true;
  inputmethod_handler_->offset_x_ = 0.0;
  inputmethod_handler_->offset_y_ = 0.0;
  inputmethod_handler_->focus_rect_ = {0, 0, 100, 50};
  inputmethod_handler_->device_pixel_ratio_ = 1.0;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputmethod_handler_->focus_status_ = false;
  inputmethod_handler_->SetScreenOffSet(10.0, 20.0);
  EXPECT_EQ(inputmethod_handler_->offset_x_, 10.0);
  EXPECT_EQ(inputmethod_handler_->offset_y_, 20.0);

  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->focus_rect_status_ = false;
  inputmethod_handler_->offset_x_ = 0.0;
  inputmethod_handler_->offset_y_ = 0.0;
  inputmethod_handler_->SetScreenOffSet(10.0, 20.0);
  EXPECT_EQ(inputmethod_handler_->offset_x_, 10.0);
  EXPECT_EQ(inputmethod_handler_->offset_y_, 20.0);

  inputmethod_handler_->focus_rect_status_ = true;
  inputmethod_handler_->SetScreenOffSet(10.0, 20.0);
  EXPECT_EQ(inputmethod_handler_->offset_x_, 10.0);
  EXPECT_EQ(inputmethod_handler_->offset_y_, 20.0);

  inputmethod_handler_->offset_x_ = 0.0;
  inputmethod_handler_->offset_y_ = 0.0;
  inputmethod_handler_->focus_rect_ = {0, 0, 100, 50};
  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());
  inputmethod_handler_->SetScreenOffSet(10.0, 20.0);
  EXPECT_EQ(inputmethod_handler_->offset_x_, 10.0);
  EXPECT_EQ(inputmethod_handler_->offset_y_, 20.0);
}

TEST_F(NWebInputMethodHandlerTest, MoveCursor) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  IMFAdapterDirection direction = IMFAdapterDirection::UP;
  inputmethod_handler_->MoveCursor(direction);
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 1);

  direction = IMFAdapterDirection::LEFT;
  inputmethod_handler_->MoveCursor(direction);
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 2);

  direction = IMFAdapterDirection::RIGHT;
  inputmethod_handler_->MoveCursor(direction);
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 3);

  direction = IMFAdapterDirection::DOWN;
  inputmethod_handler_->MoveCursor(direction);
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 4);

  direction = static_cast<IMFAdapterDirection>(4);
  inputmethod_handler_->MoveCursor(direction);
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 5);
}

TEST_F(NWebInputMethodHandlerTest, DeleteBackwardHandlerOnUI) {
  inputmethod_handler_->selected_from_ = 0;
  inputmethod_handler_->text_cursor_length_ = 0;
  inputmethod_handler_->preview_text_cache_ = u"";
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->DeleteBackwardHandlerOnUI(1);

  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  inputmethod_handler_->selected_from_ = 2;
  int32_t length = 3;
  inputmethod_handler_->DeleteBackwardHandlerOnUI(length);
  EXPECT_EQ(inputmethod_handler_->text_cursor_length_, 0);

  inputmethod_handler_->selected_from_ = 5;
  length = 3;
  inputmethod_handler_->DeleteBackwardHandlerOnUI(length);
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 0);

  inputmethod_handler_->preview_text_cache_ = u"A";
  length = 1;
  inputmethod_handler_->DeleteBackwardHandlerOnUI(length);
  EXPECT_EQ(inputmethod_handler_->preview_text_cache_.length(), 1);
}

TEST_F(NWebInputMethodHandlerTest, DeleteForwardHandlerOnUI) {
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->DeleteForwardHandlerOnUI(1);
  EXPECT_EQ(inputmethod_handler_->text_cursor_length_, 0);

  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  inputmethod_handler_->whole_text_ = u"test";
  inputmethod_handler_->selected_from_ = 2;
  inputmethod_handler_->DeleteForwardHandlerOnUI(3);
  EXPECT_EQ(inputmethod_handler_->text_cursor_length_, 0);
  EXPECT_FALSE(inputmethod_handler_->is_need_notify_all_);

  inputmethod_handler_->whole_text_ = u"test";
  inputmethod_handler_->selected_from_ = 0;
  inputmethod_handler_->DeleteForwardHandlerOnUI(2);
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 0);

  inputmethod_handler_->preview_text_cache_ = u"a";
  inputmethod_handler_->DeleteForwardHandlerOnUI(1);
  EXPECT_FALSE(inputmethod_handler_->preview_text_cache_.empty());
}

TEST_F(NWebInputMethodHandlerTest, SetNeedUnderLineOnUI) {
  inputmethod_handler_->SetNeedUnderLineOnUI(true);
  EXPECT_TRUE(inputmethod_handler_->is_need_underline_);
}

TEST_F(NWebInputMethodHandlerTest, FinishPreviewTextOnUI) {
  inputmethod_handler_->preview_text_cache_ = u"a";
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->FinishPreviewTextOnUI();
  EXPECT_FALSE(inputmethod_handler_->preview_text_cache_.empty());

  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  inputmethod_handler_->FinishPreviewTextOnUI();
  EXPECT_FALSE(inputmethod_handler_->preview_text_cache_.empty());
}

TEST_F(NWebInputMethodHandlerTest, CancelPreviewHandlerOnUI) {
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->CancelPreviewHandlerOnUI();
  EXPECT_TRUE(inputmethod_handler_->preview_text_cache_.empty());

  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  inputmethod_handler_->CancelPreviewHandlerOnUI();
  EXPECT_TRUE(inputmethod_handler_->preview_text_cache_.empty());
}

TEST_F(NWebInputMethodHandlerTest, PreviewTextHandlerOnUI) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  inputmethod_handler_->is_need_underline_ = true;
  inputmethod_handler_->composition_type_ = COMPOSITION_REPLACE;
  std::u16string text = u"test";
  int32_t start = 0;
  int32_t end = 4;
  inputmethod_handler_->PreviewTextHandlerOnUI(text, start, end);

  inputmethod_handler_->composition_type_ = COMPOSITION_CANCEL;
  inputmethod_handler_->PreviewTextHandlerOnUI(text, start, end);

  inputmethod_handler_->is_need_underline_ = false;
  inputmethod_handler_->composition_type_ = COMPOSITION_REPLACE;
  inputmethod_handler_->PreviewTextHandlerOnUI(text, start, end);

  inputmethod_handler_->composition_type_ = COMPOSITION_CANCEL;
  start = 1;
  end = 3;
  inputmethod_handler_->PreviewTextHandlerOnUI(text, start, end);
}

TEST_F(NWebInputMethodHandlerTest, ClearComposingStatus) {
  inputmethod_handler_->has_composition_ = true;
  inputmethod_handler_->preview_text_cache_ = u"test";
  inputmethod_handler_->composition_range_start_ = 1;
  inputmethod_handler_->composition_range_end_ = 4;
  inputmethod_handler_->ClearComposingStatus();
  EXPECT_FALSE(inputmethod_handler_->has_composition_);
  EXPECT_EQ(inputmethod_handler_->preview_text_cache_, std::u16string());
  EXPECT_EQ(inputmethod_handler_->composition_range_start_, 0);
  EXPECT_EQ(inputmethod_handler_->composition_range_end_, 0);
}

TEST_F(NWebInputMethodHandlerTest, InsertTextHandlerOnUI) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  inputmethod_handler_->ime_text_composing_ = false;
  inputmethod_handler_->composing_text_.clear();
  inputmethod_handler_->textCursorReady_ = 0;
  inputmethod_handler_->selected_from_ = 0;
  inputmethod_handler_->keycode_map = {
      {'A', 1},
      {'B', 2},
  };
  const std::u16string empty_text = u"";
  inputmethod_handler_->InsertTextHandlerOnUI(empty_text);

  std::u16string text = u"A";
  inputmethod_handler_->InsertTextHandlerOnUI(text);

  text = u"test";

  inputmethod_handler_->InsertTextHandlerOnUI(text);

  text = u"A";
  inputmethod_handler_->InsertTextHandlerOnUI(text);
}

TEST_F(NWebInputMethodHandlerTest, WebBlurKeyboardHideOnUI) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  inputmethod_handler_->WebBlurKeyboardHideOnUI();
  EXPECT_TRUE(inputmethod_handler_->isManualCloseKeyboard_);
}

TEST_F(NWebInputMethodHandlerTest, SetIMEStatusOnUI) {
  bool enable_ime = true;
  inputmethod_handler_->isManualCloseKeyboard_ = true;
  inputmethod_handler_->SetIMEStatusOnUI(enable_ime);
  EXPECT_FALSE(inputmethod_handler_->isManualCloseKeyboard_);
  EXPECT_TRUE(inputmethod_handler_->ime_shown_);

  bool disable_ime = false;
  inputmethod_handler_->ime_text_composing_ = true;
  inputmethod_handler_->composing_text_ = u"test";
  inputmethod_handler_->SetIMEStatusOnUI(disable_ime);
  EXPECT_FALSE(inputmethod_handler_->ime_text_composing_);
  EXPECT_TRUE(inputmethod_handler_->composing_text_.empty());
  EXPECT_FALSE(inputmethod_handler_->ime_shown_);
}

TEST_F(NWebInputMethodHandlerTest, DeleteForward) {
  int32_t test_length = 5;
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->DeleteForward(test_length);

  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  inputmethod_handler_->DeleteForward(test_length);
}

TEST_F(NWebInputMethodHandlerTest, DeleteBackward) {
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->DeleteBackward(10);

  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  inputmethod_handler_->DeleteBackward(10);
}

TEST_F(NWebInputMethodHandlerTest, InsertText) {
  std::u16string empty_text;
  testing::internal::CaptureStderr();
  inputmethod_handler_->InsertText(empty_text);
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("insert text empty!"), std::string::npos);

  inputmethod_handler_->browser_ = nullptr;
  std::u16string text = u"test";
  inputmethod_handler_->InsertText(text);

  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  text = u"test";
  inputmethod_handler_->InsertText(text);
}

TEST_F(NWebInputMethodHandlerTest, WebBlurKeyboardHide) {
  inputmethod_handler_->browser_ = nullptr;
  inputmethod_handler_->WebBlurKeyboardHide();
  EXPECT_EQ(inputmethod_handler_->browser_, nullptr);

  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  inputmethod_handler_->WebBlurKeyboardHide();
  EXPECT_NE(inputmethod_handler_->browser_->GetHost(), nullptr);
}


TEST_F(NWebInputMethodHandlerTest, SetIMEStatus) {
  inputmethod_handler_->browser_ = nullptr;
  bool status = true;
  inputmethod_handler_->SetIMEStatus(status);
  EXPECT_EQ(inputmethod_handler_->browser_, nullptr);

  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->browser_ = browser;
  inputmethod_handler_->SetIMEStatus(status);
  EXPECT_NE(inputmethod_handler_->browser_->GetHost(), nullptr);
}

TEST_F(NWebInputMethodHandlerTest, OnUpdateTextInputStateCalled) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  CefString text = "test";
  testing::internal::CaptureStderr();
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(0, 4), CefRange(0, 4));

  CefRange compositon_range = CefRange::InvalidRange();
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(0, 4), compositon_range);
  EXPECT_FALSE(inputmethod_handler_->has_composition_);
  EXPECT_EQ(inputmethod_handler_->composition_range_start_, 0);
  EXPECT_EQ(inputmethod_handler_->composition_range_end_, 0);
  EXPECT_EQ(inputmethod_handler_->preview_text_cache_, u"");

  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(0, 4), CefRange(1, 3));
  EXPECT_TRUE(inputmethod_handler_->has_composition_);
  EXPECT_EQ(inputmethod_handler_->composition_range_start_, 1);
  EXPECT_EQ(inputmethod_handler_->composition_range_end_, 3);
  EXPECT_EQ(inputmethod_handler_->preview_text_cache_, u"es");

  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(0, 4), CefRange(1, 3));

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(0, 4), CefRange(1, 3));
  EXPECT_EQ(inputmethod_handler_->whole_text_, u"test");

  std::unique_lock<std::mutex> lock(inputmethod_handler_->textCursorMutex_);
  inputmethod_handler_->textCursorReady_ = 1;
  inputmethod_handler_->OnUpdateTextInputStateCalled(browser, text, CefRange(0, 4), CefRange(1, 3));
  EXPECT_EQ(inputmethod_handler_->textCursorReady_, 1);
}

TEST_F(NWebInputMethodHandlerTest, IsTextInputStateChange) {
  inputmethod_handler_->whole_text_ = u"old text";
  std::u16string text = u"new text";
  bool result = inputmethod_handler_->IsTextInputStateChange(text, CefRange(0, 0), CefRange(0, 0));
  EXPECT_TRUE(result);

  inputmethod_handler_->whole_text_ = u"text";
  text = u"text";
  result = inputmethod_handler_->IsTextInputStateChange(text, CefRange(1, 3), CefRange(0, 0));
  EXPECT_TRUE(result);

  result = inputmethod_handler_->IsTextInputStateChange(text, CefRange(0, 0), CefRange(1, 3));
  EXPECT_TRUE(result);

  CefRange selected_range = CefRange(inputmethod_handler_->selected_from_,
    inputmethod_handler_->selected_to_);
  CefRange composition_range = CefRange(inputmethod_handler_->composition_range_start_,
    inputmethod_handler_->composition_range_end_);
  result = inputmethod_handler_->IsTextInputStateChange(text, selected_range, composition_range);
  EXPECT_FALSE(result);

  selected_range = CefRange(1, 3);
  result = inputmethod_handler_->IsTextInputStateChange(text, selected_range, composition_range);
  EXPECT_TRUE(result);

  inputmethod_handler_->whole_text_ = u"";
  text = u"";
  result = inputmethod_handler_->IsTextInputStateChange(text, CefRange(0, 0), CefRange(0, 0));
  EXPECT_FALSE(result);

  inputmethod_handler_->whole_text_ = u"text";
  text = u"text";
  selected_range.from = std::numeric_limits<uint32_t>::max();
  selected_range.to = std::numeric_limits<uint32_t>::max();
  composition_range.from = std::numeric_limits<uint32_t>::max();
  composition_range.to = std::numeric_limits<uint32_t>::max();
  result = inputmethod_handler_->IsTextInputStateChange(text, selected_range, composition_range);
  EXPECT_TRUE(result);
}

TEST_F(NWebInputMethodHandlerTest, OnImeCompositionRangeChanged) {

  CefRange selected_range = CefRange(0, 0);
  testing::internal::CaptureStderr();
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->OnImeCompositionRangeChanged(browser, selected_range);
  std::string output = testing::internal::GetCapturedStderr();
  EXPECT_NE(output.find("NWebInputMethodHandler::OnImeCompositionRangeChanged"), std::string::npos);
}

TEST_F(NWebInputMethodHandlerTest, OnCursorUpdate_Test_001) {
  inputmethod_handler_->focus_status_ = false;
  CefRect rect;
  rect.x = 0;
  rect.y = 0;
  rect.width = 100;
  rect.height = 20;
  inputmethod_handler_->OnCursorUpdate(rect);
  EXPECT_EQ(inputmethod_handler_->focus_rect_.x, rect.x);
  EXPECT_EQ(inputmethod_handler_->focus_rect_.y, rect.y);
  EXPECT_EQ(inputmethod_handler_->focus_rect_.width, rect.width);
  EXPECT_EQ(inputmethod_handler_->focus_rect_.height, rect.height);
  EXPECT_TRUE(inputmethod_handler_->focus_rect_status_);

  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputmethod_handler_->OnCursorUpdate(rect);
  EXPECT_TRUE(inputmethod_handler_->focus_rect_status_);
}

TEST_F(NWebInputMethodHandlerTest, SetNeedReattach) {
  inputmethod_handler_->SetNeedReattach(NWebInputMethodClient::HideTextinputType::FROM_ONPAUSE);
  EXPECT_TRUE(inputmethod_handler_->isNeedReattachOncontinue_);
  EXPECT_FALSE(inputmethod_handler_->isNeedReattachOnfocus_);

  inputmethod_handler_->SetNeedReattach(NWebInputMethodClient::HideTextinputType::FROM_ONBLUR);
  EXPECT_TRUE(inputmethod_handler_->isNeedReattachOncontinue_);
  EXPECT_TRUE(inputmethod_handler_->isNeedReattachOnfocus_);

  inputmethod_handler_->SetNeedReattach(NWebInputMethodClient::HideTextinputType::FROM_KERNEL);
  EXPECT_TRUE(inputmethod_handler_->isNeedReattachOncontinue_);
  EXPECT_TRUE(inputmethod_handler_->isNeedReattachOnfocus_);
}

TEST_F(NWebInputMethodHandlerTest, AttachTest) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->focus_rect_status_ = true;
  inputmethod_handler_->nweb_id_ = 123;
  NWebInputMethodClient::InputInfo inputInfo;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  inputmethod_handler_->Attach(browser, inputInfo, true, 0, 0);
  EXPECT_EQ(inputmethod_handler_->browser_, browser);
  EXPECT_FALSE(inputmethod_handler_->isAttached_);
  EXPECT_EQ(inputmethod_handler_->lastAttachNWebId_, 0);

  inputmethod_handler_->Attach(browser, inputInfo, true, 0, 0);
  EXPECT_FALSE(inputmethod_handler_->isAttached_);

  inputmethod_handler_->focus_status_ = false;
  inputmethod_handler_->focus_rect_status_ = false;
  inputmethod_handler_->Attach(browser, inputInfo, true, 0, 0);
}

TEST_F(NWebInputMethodHandlerTest, IsKeyboardShow) {
  inputmethod_handler_->isAttachSuccess_ = true;
  inputmethod_handler_->show_keyboard_ = true;
  inputmethod_handler_->isManualCloseKeyboard_ = true;
  inputmethod_handler_->cef_text_input_mode_ = CEF_TEXT_INPUT_MODE_DEFAULT;
  EXPECT_FALSE(inputmethod_handler_->IsKeyboardShow());
}

TEST_F(NWebInputMethodHandlerTest, AttachToSystemIME) {
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  bool result = inputmethod_handler_->AttachToSystemIME(true, 0);
  EXPECT_FALSE(result);

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ =
    std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());
  inputmethod_handler_->focus_status_ = false;
  result = inputmethod_handler_->AttachToSystemIME(true, 0);
  EXPECT_FALSE(result);
  EXPECT_TRUE(inputmethod_handler_->isNeedReattachOnfocus_);

  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->isAttachSuccess_ = false;
  result = inputmethod_handler_->AttachToSystemIME(true, 0);
  EXPECT_FALSE(result);
  EXPECT_TRUE(inputmethod_handler_->isNeedReattachOnfocus_);

  inputmethod_handler_->inputmethod_listener_ =
    std::make_shared<MockOnTextChangedListenerImpl>();
  inputmethod_handler_->isAttachSuccess_ = true;
  result = inputmethod_handler_->AttachToSystemIME(true, 0);
  EXPECT_FALSE(inputmethod_handler_->isFocusSwitchOnBlur_);

  result = inputmethod_handler_->AttachToSystemIME(true, 0);
  EXPECT_FALSE(result);
  EXPECT_FALSE(inputmethod_handler_->isAttachSuccess_);
  EXPECT_TRUE(inputmethod_handler_->fill_content_.empty());
}

TEST_F(NWebInputMethodHandlerTest, ComputeEditorInfo) {
  NWebInputMethodClient::InputInfo inputInfo;
  inputInfo.show_keyboard = true;
  inputInfo.input_flags = CEF_TEXT_INPUT_FLAG_HAS_BEEN_PASSWORD;
  inputInfo.node_id = 42;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_EMAIL;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TEXT;
  int32_t customEnterKeyType = 1;
  inputmethod_handler_->ComputeEditorInfo(inputInfo, customEnterKeyType);
  EXPECT_FALSE(inputmethod_handler_->type_text_flag_multi_line_);
  EXPECT_TRUE(inputmethod_handler_->show_keyboard_);
  EXPECT_EQ(inputmethod_handler_->input_flags_, CEF_TEXT_INPUT_FLAG_HAS_BEEN_PASSWORD);
  EXPECT_EQ(inputmethod_handler_->input_node_id_, 42);
  EXPECT_FALSE(inputmethod_handler_->input_is_password_);

  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputmethod_handler_->ComputeEditorInfo(inputInfo, customEnterKeyType);

  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_PASSWORD;
  inputmethod_handler_->ComputeEditorInfo(inputInfo, customEnterKeyType);
  EXPECT_TRUE(inputmethod_handler_->input_is_password_);

  customEnterKeyType = 9;
  inputmethod_handler_->ComputeEditorInfo(inputInfo, customEnterKeyType);
}

TEST_F(NWebInputMethodHandlerTest, HandleSecurityLayerHandlerOnUI) {
    inputmethod_handler_->browser_ = nullptr;
    inputmethod_handler_->HandleSecurityLayerHandlerOnUI();

    CefRefPtr<CefBrowser> browser = new MockCefBrowser();
    inputmethod_handler_->browser_ = browser;
    inputmethod_handler_->HandleSecurityLayerHandlerOnUI();
    EXPECT_NE(inputmethod_handler_->browser_, nullptr);
}

TEST_F(NWebInputMethodHandlerTest, HandleSecurityLayer) {
    inputmethod_handler_->browser_ = nullptr;
    inputmethod_handler_->HandleSecurityLayer();

    CefRefPtr<CefBrowser> browser = new MockCefBrowser();
    inputmethod_handler_->browser_ = browser;
    inputmethod_handler_->HandleSecurityLayer();
    EXPECT_NE(inputmethod_handler_->browser_, nullptr);
}
}  // namespace OHOS::NWeb
