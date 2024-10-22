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

#define private public

#include "nweb_inputmethod_handler.h"

#include "gtest/gtest.h"
#include <gmock/gmock.h>

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
  CefRefPtr<CefBrowserHost> GetHost() override { return nullptr; }
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
  CefRefPtr<CefFrame> GetFrame(int64 identifier) override { return nullptr; }
  CefRefPtr<CefFrame> GetFrame(const CefString& name) override { return nullptr; }
  size_t GetFrameCount() override { return 0; }
  void GetFrameIdentifiers(std::vector<int64>& identifiers) override {}
  void GetFrameNames(std::vector<CefString>& names) override {}
  CefRefPtr<CefBrowserPermissionRequestDelegate>
    GetPermissionRequestDelegate() override { return nullptr; }
  CefRefPtr<CefGeolocationAcess> GetGeolocationPermissions() override { return nullptr; }
#if BUILDFLAG(IS_OHOS)
  bool CanGoBackOrForward(int num_steps) override { return false; }
  void GoBackOrForward(int num_steps) override {}
  void DeleteHistory() override {}
  void SelectAndCopy() override {}
  bool ShouldShowFreeCopy() override { return false; }
  void PasswordSuggestionSelected(int list_index) override {}
  void UpdateBrowserControlsState(int constraints,
                                          int current,
                                          bool animate) override {}
  void UpdateBrowserControlsHeight(int height, bool animate) override {}
  void PrefetchPage(CefString& url,
                            CefString& additionalHttpHeaders) override {}
  void ReloadOriginalUrl() override {}
  bool CanStoreWebArchive() override { return false; }
  void SetBrowserUserAgentString(const CefString& user_agent) override {}
  bool ShouldShowLoadingUI() override { return false; }
  void SetForceEnableZoom(bool forceEnableZoom) override {}
  bool GetForceEnableZoom() override { return false; }
  int GetNWebId() override { return -1; }
  void SetEnableBlankTargetPopupIntercept(
      bool enableBlankTargetPopup) override {}
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
  int SetUrlTrustListWithErrMsg(
    const CefString& urlTrustList, CefString& detailErrMsg) override { return -1; }
  void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) override {}
#endif  // BUILDFLAG(IS_OHOS)
};

class MockOnTextChangedListenerImpl : public IMFTextListenerAdapter {
 public:
  MockOnTextChangedListenerImpl() = default;
  ~MockOnTextChangedListenerImpl() = default;
  void InsertText(const std::u16string& text) override {
  }
  void DeleteForward(int32_t length) override {
  }
  void DeleteBackward(int32_t length) override {
  }
  void SendKeyEventFromInputMethod() override {
  }
  void SendKeyboardStatus(const IMFAdapterKeyboardStatus& status) override {
  }
  void SendFunctionKey(
      std::shared_ptr<IMFAdapterFunctionKeyAdapter> functionKey) override {
  }
  void SetKeyboardStatus(bool status) override {
  }
  void MoveCursor(const IMFAdapterDirection direction) override {
  }
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
    MOCK_METHOD(bool, Attach, (std::shared_ptr<IMFTextListenerAdapter> listener, bool isShowKeyboard), (override));
    MOCK_METHOD(bool, Attach, (std::shared_ptr<IMFTextListenerAdapter> listener, bool isShowKeyboard,
        const std::shared_ptr<IMFTextConfigAdapter> config, bool isResetListener), (override));
    MOCK_METHOD(void, ShowCurrentInput, (const IMFAdapterTextInputType& inputType), (override));
    MOCK_METHOD(void, HideTextInput, (), (override));
    MOCK_METHOD(void, Close, (), (override));
    MOCK_METHOD(void, OnCursorUpdate, (const std::shared_ptr<IMFCursorInfoAdapter> cursorInfo), (override));
    MOCK_METHOD(void, OnSelectionChange, (std::u16string text, int start, int end), (override));
};

void NWebInputMethodHandlerTest::SetUpTestCase(void)
{
}

void NWebInputMethodHandlerTest::TearDownTestCase(void)
{
}

void NWebInputMethodHandlerTest::SetUp(void)
{
  inputmethod_handler_ = new NWebInputMethodHandler();
  ASSERT_NE(inputmethod_handler_, nullptr);
}

void NWebInputMethodHandlerTest::TearDown(void)
{
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

  std::shared_ptr<IMFCursorInfoAdapter> cursorInfo = inputmethod_handler_->GetCursorInfo();
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
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->composing_text_.size(), 0);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_EnterKeyTypeValid) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = (int32_t)CEF_TEXT_INPUT_ACTION_DEFAULT;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::UNSPECIFIED);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::NONE;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::NONE);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::GO;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::SEARCH;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::SEARCH);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::SEND;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::SEND);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::NEXT;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::NEXT);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::DONE;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::DONE);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::PREVIOUS;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::PREVIOUS);

  enterKeyType = (int32_t)IMFAdapterEnterKeyType::NEW_LINE;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::NEW_LINE);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_01) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_02) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TEXT;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_03) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_PASSWORD;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::VISIBLE_PASSWORD);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_04) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_SEARCH;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::SEARCH);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_05) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_EMAIL;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::EMAIL_ADDRESS);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_06) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NUMBER;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::NUMBER);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_07) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TELEPHONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::PHONE);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_08) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_URL;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::URL);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_09) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_DATE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_10) {
  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_DATE_TIME;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_11) {
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_DATE_TIME_LOCAL;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_12) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_MONTH;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_13) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TIME;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_14) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_WEEK;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_15) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TEXT_AREA;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::NEW_LINE);
  EXPECT_EQ(inputmethod_handler_->type_text_flag_multi_line_, true);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_16) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_CONTENT_EDITABLE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::NEW_LINE);
  EXPECT_EQ(inputmethod_handler_->type_text_flag_multi_line_, true);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_17) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_DATE_TIME_FIELD;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_18) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_TYPE_NULL;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_19) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_MAX;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_20) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_NONE;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::NEW_LINE);
  EXPECT_EQ(inputmethod_handler_->type_text_flag_multi_line_, true);
}

TEST_F(NWebInputMethodHandlerTest, Attach_inputmethod_listener__Null) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;

  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener, enterKeyType);
}

TEST_F(NWebInputMethodHandlerTest, Attach_Normal_01) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());
  inputmethod_handler_->inputmethod_listener_ =
    std::make_shared<MockOnTextChangedListenerImpl>();
  inputmethod_handler_->isAttached_ = false;
  EXPECT_CALL(*mock_inputmethod_adapter_, Attach(testing::_, testing::_, testing::_, testing::_))
    .WillOnce(testing::Return(false));
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_29) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_DEFAULT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_PASSWORD;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::VISIBLE_PASSWORD);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_Normal_02) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;
  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());
  inputmethod_handler_->inputmethod_listener_ =
    std::make_shared<MockOnTextChangedListenerImpl>();
  EXPECT_CALL(*mock_inputmethod_adapter_, Attach(testing::_, testing::_, testing::_, testing::_))
    .WillOnce(testing::Return(true));
  inputmethod_handler_->focus_status_ = false;
  inputmethod_handler_->focus_rect_status_ =false;
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, true);
}

TEST_F(NWebInputMethodHandlerTest, Attach_Normal_03) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;
  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());
  inputmethod_handler_->inputmethod_listener_ =
    std::make_shared<MockOnTextChangedListenerImpl>();
  
  EXPECT_CALL(*mock_inputmethod_adapter_, Attach(testing::_, testing::_, testing::_, testing::_))
    .WillOnce(testing::Return(true));
  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->focus_rect_status_ =false;
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, true);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_28) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_MAX;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::NEW_LINE);
}

TEST_F(NWebInputMethodHandlerTest, Attach_Normal_04) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());
  inputmethod_handler_->inputmethod_listener_ =
    std::make_shared<MockOnTextChangedListenerImpl>();
  
  EXPECT_CALL(*mock_inputmethod_adapter_, Attach(testing::_, testing::_, testing::_, testing::_))
    .WillOnce(testing::Return(true));
  inputmethod_handler_->focus_status_ = false;
  inputmethod_handler_->focus_rect_status_ =true;
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, true);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_27) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_SEARCH;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::NEW_LINE);
  EXPECT_EQ(inputmethod_handler_->type_text_flag_multi_line_, true);
}

TEST_F(NWebInputMethodHandlerTest, Attach_Normal_05) {
  CefRefPtr<CefBrowser> browser = new MockCefBrowser();
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  int32_t enterKeyType = 1;

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());
  inputmethod_handler_->inputmethod_listener_ =
    std::make_shared<MockOnTextChangedListenerImpl>();
  
  EXPECT_CALL(*mock_inputmethod_adapter_, Attach(testing::_, testing::_, testing::_, testing::_))
    .WillOnce(testing::Return(true));
  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->focus_rect_status_ =true;
  inputmethod_handler_->Attach(browser, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, true);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_CONTINUE_False) {
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type = NWebInputMethodHandler::ReattachType::FROM_CONTINUE;
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
  NWebInputMethodHandler::ReattachType type = NWebInputMethodHandler::ReattachType::FROM_CONTINUE;

  inputmethod_handler_->isNeedReattachOncontinue_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = true;

  inputmethod_handler_->lastInputMode_ = IMFAdapterTextInputType::TEXT;
  inputmethod_handler_->imf_input_mode_ = IMFAdapterTextInputType::NUMBER;

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());
  
  EXPECT_CALL(*mock_inputmethod_adapter_, Attach(testing::_, testing::_, testing::_, testing::_))
    .WillOnce(testing::Return(false));
  EXPECT_CALL(*mock_inputmethod_adapter_, Close()).Times(1);

  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOncontinue_, false);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_CONTINUE_False2) {  
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type = NWebInputMethodHandler::ReattachType::FROM_CONTINUE;

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
  NWebInputMethodHandler::ReattachType type = NWebInputMethodHandler::ReattachType::FROM_CONTINUE;

  inputmethod_handler_->isNeedReattachOncontinue_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = true;

  inputmethod_handler_->lastInputMode_ = IMFAdapterTextInputType::TEXT;
  inputmethod_handler_->imf_input_mode_ = IMFAdapterTextInputType::NUMBER;

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());
  
  EXPECT_CALL(*mock_inputmethod_adapter_, Attach(testing::_, testing::_, testing::_, testing::_))
    .WillOnce(testing::Return(true));
  EXPECT_CALL(*mock_inputmethod_adapter_, Close()).Times(1);
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOncontinue_, false);
  EXPECT_EQ(inputmethod_handler_->isAttached_, true);
  EXPECT_EQ(inputmethod_handler_->lastAttachNWebId_, nwebId);
  EXPECT_EQ(inputmethod_handler_->lastInputMode_, IMFAdapterTextInputType::NUMBER);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_ONFOCUS_False) {  
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type = NWebInputMethodHandler::ReattachType::FROM_ONFOCUS;
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
  NWebInputMethodHandler::ReattachType type = NWebInputMethodHandler::ReattachType::FROM_ONFOCUS;

  inputmethod_handler_->isNeedReattachOnfocus_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = false;
  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOnfocus_, false);
  EXPECT_EQ(result, false);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_ONFOCUS_False3) {  
  uint32_t nwebId = 0;
  bool result = false;
  NWebInputMethodHandler::ReattachType type = NWebInputMethodHandler::ReattachType::FROM_ONFOCUS;

  inputmethod_handler_->isNeedReattachOnfocus_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = true;

  inputmethod_handler_->lastInputMode_ = IMFAdapterTextInputType::TEXT;
  inputmethod_handler_->imf_input_mode_ = IMFAdapterTextInputType::NUMBER;

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());
  
  EXPECT_CALL(*mock_inputmethod_adapter_, Attach(testing::_, testing::_, testing::_, testing::_))
    .WillOnce(testing::Return(false));

  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOnfocus_, false);
  EXPECT_EQ(result, false);
}


TEST_F(NWebInputMethodHandlerTest, ShowTextInput) {  
  inputmethod_handler_->ShowTextInput();
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_inputmethod_adapter__Null) {  
  inputmethod_handler_->isAttached_ = true;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  uint32_t nwebId = 0;
  NWebInputMethodClient::HideTextinputType hideType = NWebInputMethodClient::HideTextinputType::FROM_KERNEL;
  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, true);
}

TEST_F(NWebInputMethodHandlerTest, Reattach_ReattachType_FROM_ONFOCUS_True) {  
  uint32_t nwebId = 1;
  bool result = false;
  NWebInputMethodHandler::ReattachType type = NWebInputMethodHandler::ReattachType::FROM_ONFOCUS;

  inputmethod_handler_->isNeedReattachOnfocus_ = true;
  inputmethod_handler_->is_editable_node_ = true;
  inputmethod_handler_->inputmethod_listener_ = nullptr;
  inputmethod_handler_->show_keyboard_ = false;
  inputmethod_handler_->isAttached_ = true;

  inputmethod_handler_->lastInputMode_ = IMFAdapterTextInputType::TEXT;
  inputmethod_handler_->imf_input_mode_ = IMFAdapterTextInputType::NUMBER;

  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());
  
  EXPECT_CALL(*mock_inputmethod_adapter_, Attach(testing::_, testing::_, testing::_, testing::_))
    .WillOnce(testing::Return(true));
  EXPECT_CALL(*mock_inputmethod_adapter_, Close()).Times(1);

  result = inputmethod_handler_->Reattach(nwebId, type);
  EXPECT_EQ(inputmethod_handler_->nweb_id_, nwebId);
  EXPECT_EQ(inputmethod_handler_->isNeedReattachOnfocus_, false);
  EXPECT_EQ(inputmethod_handler_->isAttached_, true);
  EXPECT_EQ(inputmethod_handler_->lastAttachNWebId_, nwebId);
  EXPECT_EQ(inputmethod_handler_->lastInputMode_, IMFAdapterTextInputType::NUMBER);
  EXPECT_EQ(result, false);
}


TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_21) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_TEXT;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::TEXT);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, Attach_InputInfo_22) {  
  NWebInputMethodClient::InputInfo inputInfo;
  bool is_need_reset_listener = false;
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputInfo.input_mode = CEF_TEXT_INPUT_MODE_TEL;
  inputInfo.input_type = CEF_TEXT_INPUT_TYPE_NONE;
  int32_t enterKeyType = -1;
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::PHONE);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_isAttached_False) {  
  inputmethod_handler_->isAttached_ = false;
  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());

  uint32_t nwebId = 0;
  NWebInputMethodClient::HideTextinputType hideType = NWebInputMethodClient::HideTextinputType::FROM_KERNEL;
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
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::URL);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_isAttached_False2) {  
  inputmethod_handler_->isAttached_ = false;
  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());

  uint32_t nwebId = 0;
  NWebInputMethodClient::HideTextinputType hideType = NWebInputMethodClient::HideTextinputType::FROM_ONPAUSE;
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
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::EMAIL_ADDRESS);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_isAttached_False3) {  
  inputmethod_handler_->isAttached_ = false;
  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());

  uint32_t nwebId = 0;
  NWebInputMethodClient::HideTextinputType hideType = NWebInputMethodClient::HideTextinputType::FROM_ONPAUSE;
  inputmethod_handler_->lastCloseInputMethodTime_ = std::chrono::high_resolution_clock::now();
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
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::NUMBER);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_Normal) {  
  inputmethod_handler_->isAttached_ = true;
  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());

  uint32_t nwebId = 0;
  NWebInputMethodClient::HideTextinputType hideType = NWebInputMethodClient::HideTextinputType::FROM_ONPAUSE;
  inputmethod_handler_->lastCloseInputMethodTime_ = std::chrono::high_resolution_clock::now();
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
  inputmethod_handler_->Attach(nullptr, inputInfo, is_need_reset_listener, enterKeyType);
  EXPECT_EQ(inputmethod_handler_->imf_input_mode_, IMFAdapterTextInputType::NUMBER);
  EXPECT_EQ(inputmethod_handler_->imf_input_action_, IMFAdapterEnterKeyType::GO);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInput_Normal02) {  
  inputmethod_handler_->isAttached_ = true;
  std::shared_ptr<MockIMFAdapterImpl> mock_inputmethod_adapter_ = std::make_shared<MockIMFAdapterImpl>();
  inputmethod_handler_->inputmethod_adapter_.reset(mock_inputmethod_adapter_.get());

  uint32_t nwebId = 0;
  NWebInputMethodClient::HideTextinputType hideType = NWebInputMethodClient::HideTextinputType::FROM_ONBLUR;
  inputmethod_handler_->lastCloseInputMethodTime_ = std::chrono::high_resolution_clock::now();
  inputmethod_handler_->lastAttachNWebId_ = 0;
  
  inputmethod_handler_->HideTextInput(nwebId, hideType);
  EXPECT_EQ(inputmethod_handler_->isAttached_, false);
}

TEST_F(NWebInputMethodHandlerTest, HideTextInputForce_inputmethod_adapter_Null) {  
  inputmethod_handler_->inputmethod_adapter_ = nullptr;
  inputmethod_handler_->HideTextInputForce();
}

TEST_F(NWebInputMethodHandlerTest, HideTextInputForce_Normal) {  
  inputmethod_handler_->HideTextInputForce();
}

TEST_F(NWebInputMethodHandlerTest, OnTextSelectionChanged) {
  CefString selected_text = CefString("test");
  CefRange selected_range = CefRange(0, 3);
  inputmethod_handler_->OnTextSelectionChanged(nullptr, selected_text, selected_range);
  EXPECT_EQ(inputmethod_handler_->selected_text_.size(), 4);
}

TEST_F(NWebInputMethodHandlerTest, OnCursorUpdate) {
  CefRect rect;
  inputmethod_handler_->focus_status_ = true;
  inputmethod_handler_->OnCursorUpdate(rect);
}

TEST_F(NWebInputMethodHandlerTest, OnSelectionChanged) {
  inputmethod_handler_->OnSelectionChanged(nullptr, CefString(""), CefRange(0, 1));
}

TEST_F(NWebInputMethodHandlerTest, SendEnterKeyEvent) {
  inputmethod_handler_->SendEnterKeyEvent(1);
}

#if defined(OHOS_CLIPBOARD)
TEST_F(NWebInputMethodHandlerTest, GetSelectInfo) {
  inputmethod_handler_->GetSelectInfo();
}
#endif // defined(OHOS_CLIPBOARD)

TEST_F(NWebInputMethodHandlerTest, ResetTextSelectiondata) {
  inputmethod_handler_->is_need_notify_all_ = false;
  bool result = inputmethod_handler_->ResetTextSelectiondata();
  EXPECT_EQ(result, false);

  inputmethod_handler_->is_need_notify_all_ = true;
  result = inputmethod_handler_->ResetTextSelectiondata();
  EXPECT_EQ(result, true);
  EXPECT_EQ(inputmethod_handler_->is_need_notify_all_, false);
}
}