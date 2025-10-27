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

#include <fuzzer/FuzzedDataProvider.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#include "base/strings/string_util.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/common/content_client.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#define private public
#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"
#include "ohos_nweb/src/nweb_inputmethod_handler.h"

using namespace OHOS::NWeb;
namespace OHOS::NWeb {

class FuzzNWebInputMethodHandler : public NWebInputMethodHandler {
 public:
  void SetHasCompositionForTest(bool value) { has_composition_ = value; }
  void SetWholeTextForTest(const std::u16string& text) { whole_text_ = text; }
  void SetPreviewTextCacheForTest(const std::u16string& text) {
    preview_text_cache_ = text;
  }

  void SetCompositionRangeStartForTest(int32_t start) {
    composition_range_start_ = start;
  }
  int32_t GetCompositionRangeStartForTest() const {
    return composition_range_start_;
  }

  void SetCompositionRangeEndForTest(int32_t end) {
    composition_range_end_ = end;
  }
  int32_t GetCompositionRangeEndForTest() const {
    return composition_range_end_;
  }

  void SetCompositionTypeForTest(CompositionType type) {
    composition_type_ = type;
  }
  CompositionType GetCompositionTypeForTest() const {
    return composition_type_;
  }

  void SetCompositionCursorIndexForTest(int32_t index) {
    composition_cursor_index_ = index;
  }
  int32_t GetCompositionCursorIndexForTest() const {
    return composition_cursor_index_;
  }

  void SetFocusRectForTest(const CefRect& rect) { focus_rect_ = rect; }
  void SetDevicePixelRatioForTest(double ratio) { device_pixel_ratio_ = ratio; }
  void SetOffsetXForTest(double x) { offset_x_ = x; }
  void SetOffsetYForTest(double y) { offset_y_ = y; }
};
class MockCefBrowser : public CefBrowser {
  void AddRef() const override {}
  bool Release() const override { return false; }
  bool HasOneRef() const override { return false; }
  bool HasAtLeastOneRef() const override { return false; }
  bool IsValid() override { return false; }
  CefRefPtr<ArkWebBrowserHostExt> GetHost() override { return nullptr; }
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
  CefRefPtr<CefFrame> GetFrameByIdentifier(
      const CefString& identifier) override {
    return nullptr;
  }
  CefRefPtr<CefFrame> GetFrameByName(const CefString& name) override {
    return nullptr;
  }
  size_t GetFrameCount() override { return 0; }
  void GetFrameIdentifiers(std::vector<CefString>& identifiers) override {}
  void GetFrameNames(std::vector<CefString>& names) override {}
  bool NeedToFireBeforeUnloadOrUnloadEvents() override { return false; }
  void DispatchBeforeUnload() override {}
  int PrerenderPage(const CefString& url, const CefString& additional_headers) {
    return 0;
  }
  void CancelAllPrerendering() {}
};

class MockIMFAdapterFunctionKeyAdapter : public IMFAdapterFunctionKeyAdapter {
 public:
  IMFAdapterEnterKeyType GetEnterKeyType() override {
    return storedKeyType;
  }
  void SetEnterKeyType(IMFAdapterEnterKeyType keyType) {
    storedKeyType = keyType;
  }
 private:
  IMFAdapterEnterKeyType storedKeyType = IMFAdapterEnterKeyType::UNSPECIFIED;
};

class OnTextChangedListenerImpl : public IMFTextListenerAdapter {
 public:
  OnTextChangedListenerImpl(NWebInputMethodHandler* handler)
      : handler_(handler) {}
  ~OnTextChangedListenerImpl() = default;

  void InsertText(const std::u16string& text) override {
    if (text.size() == 1 && text.front() == '\n') {
      return;
    }
    handler_->InsertText(text);
  }

  void DeleteForward(int32_t length) override {
    handler_->DeleteForward(length);
  }

  void DeleteBackward(int32_t length) override {
    handler_->DeleteBackward(length);
  }

  void SendKeyEventFromInputMethod() override {
    LOG(INFO) << "NWebInputMethodHandler::SendKeyEventFromInputMethod";
  }

  void SendKeyboardStatus(const IMFAdapterKeyboardStatus& status) override {
    if (status == IMFAdapterKeyboardStatus::SHOW) {
      handler_->SetIMEStatus(true);
    } else if (status == IMFAdapterKeyboardStatus::HIDE) {
      handler_->SetIMEStatus(false);
    }
  }

  void SendFunctionKey(
      std::shared_ptr<IMFAdapterFunctionKeyAdapter> functionKey) override {
    if (handler_ && functionKey) {
      LOG(DEBUG) << "SendFunctionKey enterkeytype = "
                 << static_cast<int32_t>(functionKey->GetEnterKeyType());
      handler_->SendEnterKeyEvent(
          static_cast<int32_t>(functionKey->GetEnterKeyType()));
    }
  }

  void SetKeyboardStatus(bool status) override {
    handler_->SetIMEStatus(status);
  }

  void KeyboardUpperRightCornerHide() override {
    handler_->WebBlurKeyboardHide();
  }

  void MoveCursor(const IMFAdapterDirection direction) override {
    if (direction == IMFAdapterDirection::NONE) {
      LOG(ERROR) << "NWebInputMethodHandler::MoveCursor got none direction";
      return;
    }
    handler_->MoveCursor(direction);
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

  void AutoFillWithIMFEvent(bool is_username,
                            bool is_other_account,
                            bool is_new_password,
                            const std::string& content) override {
    LOG(INFO) << "receive autofill event from IMF";
    handler_->AutoFillWithIMFEvent(is_username, is_other_account,
                                   is_new_password, content);
  }

 private:
  raw_ptr<NWebInputMethodHandler> handler_;
};

void NWebInputMethodHandlerFuzzTest(const uint8_t* data, size_t size) {
  if (data == nullptr || size < sizeof(uint32_t)) {
    return;
  }
  FuzzedDataProvider fdp(data, size);

  const std::vector<cef_text_input_flags_t> allFlags = {
      CEF_TEXT_INPUT_FLAG_AUTOCOMPLETE_ON,
      CEF_TEXT_INPUT_FLAG_AUTOCOMPLETE_OFF,
      CEF_TEXT_INPUT_FLAG_AUTOCORRECT_ON,
      CEF_TEXT_INPUT_FLAG_AUTOCORRECT_OFF,
      CEF_TEXT_INPUT_FLAG_SPELLCHECK_ON,
      CEF_TEXT_INPUT_FLAG_SPELLCHECK_OFF,
      CEF_TEXT_INPUT_FLAG_AUTOCAPITALIZE_NONE,
      CEF_TEXT_INPUT_FLAG_AUTOCAPITALIZE_CHARACTERS,
      CEF_TEXT_INPUT_FLAG_AUTOCAPITALIZE_WORDS,
      CEF_TEXT_INPUT_FLAG_AUTOCAPITALIZE_SENTENCES,
      CEF_TEXT_INPUT_FLAG_HAVE_NEXT_FOCUSABLE_ELEMENT,
      CEF_TEXT_INPUT_FLAG_HAVE_PREVIOUS_FOCUSABLE_ELEMENT,
      CEF_TEXT_INPUT_FLAG_HAS_BEEN_PASSWORD,
      CEF_TEXT_INPUT_FLAG_VERTICAL};

  const std::vector<IMFAdapterDirection> directions = {
      IMFAdapterDirection::LEFT, IMFAdapterDirection::RIGHT,
      IMFAdapterDirection::UP, IMFAdapterDirection::DOWN,
      IMFAdapterDirection::NONE};

  const std::vector<NWebInputMethodHandler::HideTextinputType> hideTypes = {
      NWebInputMethodHandler::HideTextinputType::FROM_KERNEL,
      NWebInputMethodHandler::HideTextinputType::FROM_ONBLUR,
      NWebInputMethodHandler::HideTextinputType::FROM_ONPAUSE};

  const std::vector<cef_text_input_type_t> allTypes = {
    CEF_TEXT_INPUT_TYPE_TEXT,      CEF_TEXT_INPUT_TYPE_PASSWORD,
    CEF_TEXT_INPUT_TYPE_EMAIL,     CEF_TEXT_INPUT_TYPE_NUMBER,
    CEF_TEXT_INPUT_TYPE_TELEPHONE, CEF_TEXT_INPUT_TYPE_URL,
    CEF_TEXT_INPUT_TYPE_TEXT_AREA, CEF_TEXT_INPUT_TYPE_CONTENT_EDITABLE};

  const std::vector<cef_text_input_mode_t> allModes = {
      CEF_TEXT_INPUT_MODE_DEFAULT, CEF_TEXT_INPUT_MODE_NONE,
      CEF_TEXT_INPUT_MODE_TEXT,    CEF_TEXT_INPUT_MODE_TEL,
      CEF_TEXT_INPUT_MODE_URL,     CEF_TEXT_INPUT_MODE_EMAIL,
      CEF_TEXT_INPUT_MODE_NUMERIC, CEF_TEXT_INPUT_MODE_DECIMAL,
      CEF_TEXT_INPUT_MODE_SEARCH};

  const std::vector<int32_t> allEnterKeyTypes = {
      static_cast<int32_t>(IMFAdapterEnterKeyType::UNSPECIFIED),
      static_cast<int32_t>(IMFAdapterEnterKeyType::NONE),
      static_cast<int32_t>(IMFAdapterEnterKeyType::GO),
      static_cast<int32_t>(IMFAdapterEnterKeyType::SEARCH),
      static_cast<int32_t>(IMFAdapterEnterKeyType::SEND),
      static_cast<int32_t>(IMFAdapterEnterKeyType::NEXT),
      static_cast<int32_t>(IMFAdapterEnterKeyType::DONE),
      static_cast<int32_t>(IMFAdapterEnterKeyType::PREVIOUS),
      static_cast<int32_t>(IMFAdapterEnterKeyType::NEW_LINE)};

  const std::vector<cef_text_input_action_t> allActions = {
      CEF_TEXT_INPUT_ACTION_DEFAULT, CEF_TEXT_INPUT_ACTION_ENTER,
      CEF_TEXT_INPUT_ACTION_DONE,    CEF_TEXT_INPUT_ACTION_GO,
      CEF_TEXT_INPUT_ACTION_NEXT,    CEF_TEXT_INPUT_ACTION_PREVIOUS,
      CEF_TEXT_INPUT_ACTION_SEARCH,  CEF_TEXT_INPUT_ACTION_SEND,
  };

  {
    FuzzNWebInputMethodHandler handler;
    OnTextChangedListenerImpl listener(&handler);
    std::string insertTextUtf8 = fdp.ConsumeRandomLengthString(100);
    std::u16string insertText(insertTextUtf8.begin(), insertTextUtf8.end());
    listener.InsertText(insertText);
    listener.InsertText(u"\n");
    int32_t deleteLength = fdp.ConsumeIntegralInRange<int32_t>(0, 10);
    listener.DeleteForward(deleteLength);
    listener.DeleteBackward(deleteLength);
    listener.SendKeyEventFromInputMethod();
    IMFAdapterKeyboardStatus keyboardStatus =
        fdp.ConsumeBool() ? IMFAdapterKeyboardStatus::SHOW
                          : IMFAdapterKeyboardStatus::HIDE;
    listener.SendKeyboardStatus(keyboardStatus);
    auto mockFunctionKey = std::make_shared<MockIMFAdapterFunctionKeyAdapter>();

    for (int32_t typeValue =
             static_cast<int32_t>(IMFAdapterEnterKeyType::UNSPECIFIED);
         typeValue <= static_cast<int32_t>(IMFAdapterEnterKeyType::NEW_LINE);
         typeValue++) {
      IMFAdapterEnterKeyType keyType =
          static_cast<IMFAdapterEnterKeyType>(typeValue);
      mockFunctionKey->SetEnterKeyType(keyType);
      listener.SendFunctionKey(mockFunctionKey);
    }

    listener.SetKeyboardStatus(fdp.ConsumeBool());
    listener.KeyboardUpperRightCornerHide();
    size_t dirIndex =
        fdp.ConsumeIntegralInRange<size_t>(0, directions.size() - 1);
    listener.MoveCursor(directions[dirIndex]);
    listener.HandleSetSelection(fdp.ConsumeIntegral<int32_t>(),
                                fdp.ConsumeIntegral<int32_t>());
    listener.HandleExtendAction(fdp.ConsumeIntegral<int32_t>());
    listener.HandleSelect(fdp.ConsumeIntegral<int32_t>(),
                          fdp.ConsumeIntegral<int32_t>());
    listener.SetNeedUnderLine(fdp.ConsumeBool());
    listener.GetTextIndexAtCursor();
    int32_t textNumber = fdp.ConsumeIntegralInRange<int32_t>(0, 20);
    listener.GetLeftTextOfCursor(textNumber);
    listener.GetRightTextOfCursor(textNumber);
    std::string previewTextUtf8 = fdp.ConsumeRandomLengthString(100);
    std::u16string previewText(previewTextUtf8.begin(), previewTextUtf8.end());
    int32_t start = fdp.ConsumeIntegral<int32_t>();
    int32_t end = fdp.ConsumeIntegral<int32_t>();
    listener.SetPreviewText(previewText, start, end);
    listener.FinishTextPreview();
    bool isUsername = fdp.ConsumeBool();
    bool isOtherAccount = fdp.ConsumeBool();
    bool isNewPassword = fdp.ConsumeBool();
    std::string content = fdp.ConsumeRandomLengthString(100);
    listener.AutoFillWithIMFEvent(isUsername, isOtherAccount, isNewPassword,
                                  content);
    CefRefPtr<CefBrowser> browser =
        fdp.ConsumeBool() ? nullptr : new MockCefBrowser();
    bool resetListener = fdp.ConsumeBool();
    cef_text_input_mode_t mode = static_cast<cef_text_input_mode_t>(
        fdp.ConsumeIntegralInRange<int32_t>(0, allModes.size() - 1));
    cef_text_input_type_t type = static_cast<cef_text_input_type_t>(
        fdp.ConsumeIntegralInRange<int32_t>(0, allTypes.size() - 1));
    cef_text_input_action_t action = static_cast<cef_text_input_action_t>(
        fdp.ConsumeIntegralInRange<int32_t>(0, allActions.size() - 1));
    uint32_t flagsValue = fdp.ConsumeIntegral<uint32_t>();
    cef_text_input_flags_t flags =
        static_cast<cef_text_input_flags_t>(flagsValue);
    NWebInputMethodClient::InputInfo inputInfo;
    inputInfo.node_id = fdp.ConsumeIntegral<int>();
    inputInfo.show_keyboard = fdp.ConsumeBool();
    inputInfo.input_mode = mode;
    inputInfo.input_type = type;
    inputInfo.input_action = action;
    inputInfo.input_flags = flags;
    inputInfo.always_hide_ime = fdp.ConsumeBool();
    int32_t enterKeyType =
        fdp.ConsumeIntegralInRange<int32_t>(0, allEnterKeyTypes.size() - 1);
    handler.Attach(browser.get(), inputInfo, resetListener, enterKeyType);
    int32_t requestKeyboardReason = fdp.ConsumeIntegral<int32_t>();
    handler.Attach(browser.get(), inputInfo, resetListener, enterKeyType,
                   requestKeyboardReason);
    uint32_t nwebId = fdp.ConsumeIntegral<uint32_t>();
    NWebInputMethodHandler::ReattachType reattach_type =
        static_cast<NWebInputMethodHandler::ReattachType>(
            fdp.ConsumeIntegralInRange<int32_t>(0, 2));
    handler.Reattach(nwebId, reattach_type);
    handler.ShowTextInput();
    uint32_t hideNwebId = fdp.ConsumeIntegral<uint32_t>();
    size_t hideTypeIndex =
        fdp.ConsumeIntegralInRange<size_t>(0, hideTypes.size() - 1);
    bool noNeedKeyboardByInput = fdp.ConsumeBool();
    handler.HideTextInput(hideNwebId, hideTypes[hideTypeIndex],
                          noNeedKeyboardByInput);
    handler.HideTextInputForce();
    std::string selectedTextUtf8 = fdp.ConsumeRandomLengthString(100);
    CefString selectedText(selectedTextUtf8);
    CefRange selectedRange;
    selectedRange.from = fdp.ConsumeIntegral<int64_t>();
    selectedRange.to = fdp.ConsumeIntegral<int64_t>();
    handler.OnTextSelectionChanged(browser.get(), selectedText, selectedRange);
    handler.OnSelectionChanged(browser.get(), selectedText, selectedRange);
    bool focusStatus = fdp.ConsumeBool();
    handler.SetFocusStatus(focusStatus);
    CefRect cursorRect;
    cursorRect.x = fdp.ConsumeIntegral<int>();
    cursorRect.y = fdp.ConsumeIntegral<int>();
    cursorRect.width = fdp.ConsumeIntegral<int>();
    cursorRect.height = fdp.ConsumeIntegral<int>();
    handler.OnCursorUpdate(cursorRect);
    bool isEditableNode = fdp.ConsumeBool();
    handler.OnEditableChanged(browser.get(), isEditableNode);
    handler.GetIsEditableNode();
    bool imeStatus = fdp.ConsumeBool();
    handler.SetIMEStatus(imeStatus);
    handler.WebBlurKeyboardHide();
    handler.HandleSecurityLayer();
    double screenX = fdp.ConsumeFloatingPoint<double>();
    double screenY = fdp.ConsumeFloatingPoint<double>();
    handler.SetScreenOffSet(screenX, screenY);
    float deviceRatio = fdp.ConsumeFloatingPoint<float>();
    handler.SetVirtualDeviceRatio(deviceRatio);
    uint32_t windowId = fdp.ConsumeIntegral<uint32_t>();
    handler.SetWindowIdForIME(windowId);
    bool needUnderline = fdp.ConsumeBool();
    handler.SetNeedUnderLine(needUnderline);
    handler.HasComposition();
    CefRange compositionRange;
    compositionRange.from = fdp.ConsumeIntegral<uint32_t>();
    compositionRange.to = fdp.ConsumeIntegral<uint32_t>();
    handler.OnImeCompositionRangeChanged(browser.get(), compositionRange);
    handler.OnUpdateTextInputStateCalled(browser.get(), selectedText,
                                         selectedRange, compositionRange);
    handler.IsAttached();
    handler.SetNeedReattachOnfocus();
    handler.SetFillContent(content, fdp.ConsumeIntegral<int32_t>());
    handler.GetSelectInfo();
    handler.GetSelectStartIndex();
    handler.GetSelectEndIndex();
    handler.GetAllTextInfo();
    handler.SetHasCompositionForTest(fdp.ConsumeBool());
    std::string whole_text_str = fdp.ConsumeRandomLengthString(100);
    handler.SetWholeTextForTest(
        std::u16string(whole_text_str.begin(), whole_text_str.end()));
    std::string preview_str = fdp.ConsumeRandomLengthString(50);
    handler.SetPreviewTextCacheForTest(
        std::u16string(preview_str.begin(), preview_str.end()));
    handler.SetCompositionRangeStartForTest(fdp.ConsumeIntegral<int32_t>());
    handler.SetCompositionRangeEndForTest(fdp.ConsumeIntegral<int32_t>());
    std::string text_str = fdp.ConsumeRandomLengthString(100);
    std::u16string text(text_str.begin(), text_str.end());
    int32_t comp_start = fdp.ConsumeIntegral<int32_t>();
    int32_t comp_end = fdp.ConsumeIntegral<int32_t>();
    const std::vector<CompositionType> allCompTypes = {
        COMPOSITION_CURRENT, COMPOSITION_POSITION, COMPOSITION_REPLACE,
        COMPOSITION_CANCEL,  COMPOSITION_DELETE,   COMPOSITION_INVALID};
    size_t comp_type_index =
        fdp.ConsumeIntegralInRange<size_t>(0, allCompTypes.size() - 1);
    CompositionType comp_type = allCompTypes[comp_type_index];
    int32_t result = handler.GetCompositionTypeAndCheckInput(
        text, comp_start, comp_end, comp_type);
    (void)result;
    (void)comp_type;

    for (auto inputMode : allModes) {
      handler.TextInputModeToIMFAdapter(inputMode);
    }

    for (auto inputType : allTypes) {
      handler.TextInputTypeToIMFAdapter(inputType);
    }

    for (auto inputAction : allActions) {
      inputInfo.input_action = inputAction;
      inputInfo.input_mode = static_cast<cef_text_input_mode_t>(
          fdp.ConsumeIntegralInRange<int32_t>(0, allModes.size() - 1));
      inputInfo.input_type = static_cast<cef_text_input_type_t>(
          fdp.ConsumeIntegralInRange<int32_t>(0, allTypes.size() - 1));
      inputInfo.input_flags =
          static_cast<cef_text_input_flags_t>(fdp.ConsumeIntegral<uint32_t>());
      handler.TextInputActionToIMFAdapter(inputInfo);
    }

    CefRect rect(fdp.ConsumeIntegral<int>(), fdp.ConsumeIntegral<int>(),
                 fdp.ConsumeIntegral<int>(), fdp.ConsumeIntegral<int>());
    double ratio = fdp.ConsumeFloatingPoint<double>();
    double offsetX = fdp.ConsumeFloatingPoint<double>();
    double offsetY = fdp.ConsumeFloatingPoint<double>();
    handler.SetFocusRectForTest(rect);
    handler.SetDevicePixelRatioForTest(ratio);
    handler.SetOffsetXForTest(offsetX);
    handler.SetOffsetYForTest(offsetY);
    std::shared_ptr<IMFCursorInfoAdapter> cursorInfo = handler.GetCursorInfo();
    handler.HandleSecurityLayerHandlerOnUI();
    handler.AttachToSystemIME(fdp.ConsumeBool(),
                              fdp.ConsumeIntegralInRange<int32_t>(0, 10));
    handler.NeedKeyboardShow();
    handler.IsKeyboardShow();
    handler.SetNeedReattach(hideTypes[1]);
    handler.SetNeedReattach(hideTypes[2]);
    handler.SetIMEStatusOnUI(fdp.ConsumeBool());
    handler.CancelPreviewHandlerOnUI();
    handler.FinishPreviewTextOnUI();
    handler.DeleteForwardHandlerOnUI(
        fdp.ConsumeIntegralInRange<int32_t>(0, 100));
    handler.DeleteBackwardHandlerOnUI(
        fdp.ConsumeIntegralInRange<int32_t>(0, 100));
    handler.SendEnterKeyEvent(fdp.ConsumeIntegralInRange<int32_t>(
        static_cast<int32_t>(IMFAdapterEnterKeyType::UNSPECIFIED),
        static_cast<int32_t>(IMFAdapterEnterKeyType::NEW_LINE)));
    handler.SendEnterKeyEventOnUI(fdp.ConsumeIntegralInRange<int32_t>(
        static_cast<int32_t>(IMFAdapterEnterKeyType::UNSPECIFIED),
        static_cast<int32_t>(IMFAdapterEnterKeyType::NEW_LINE)));
    handler.AutoFillWithIMFEventOnUI(true, true, true, "text");
  }

  {
    FuzzNWebInputMethodHandler handler;
    OnTextChangedListenerImpl listener(&handler);

    std::u16string fixedInsertText = u"Fixed Test Text";
    listener.InsertText(fixedInsertText);
    listener.InsertText(u"\n");
    listener.DeleteForward(3);
    listener.DeleteBackward(2);
    listener.SendKeyEventFromInputMethod();
    listener.SendKeyboardStatus(IMFAdapterKeyboardStatus::SHOW);
    listener.SendKeyboardStatus(IMFAdapterKeyboardStatus::HIDE);
    listener.MoveCursor(IMFAdapterDirection::LEFT);
    listener.MoveCursor(IMFAdapterDirection::RIGHT);
    listener.MoveCursor(IMFAdapterDirection::UP);
    listener.MoveCursor(IMFAdapterDirection::DOWN);
    listener.GetTextIndexAtCursor();
    listener.GetLeftTextOfCursor(5);
    listener.GetRightTextOfCursor(5);
    std::u16string fixedPreviewText = u"Fixed Preview Text";
    listener.SetPreviewText(fixedPreviewText, -2147483648, -2147483648);
    listener.FinishTextPreview();
    listener.SetPreviewText(fixedPreviewText, 2, 7);
    listener.FinishTextPreview();
    listener.AutoFillWithIMFEvent(true, false, true, "Fixed Autofill Content");
    CefRefPtr<CefBrowser> browser = new MockCefBrowser();
    CefRefPtr<CefBrowser> nullBrowser = nullptr;
    bool resetListener = true;
    bool noResetListener = false;

    for (auto mode : allModes) {
      for (auto type : allTypes) {
        for (auto action : allActions) {
          uint32_t flagsValue = CEF_TEXT_INPUT_FLAG_AUTOCOMPLETE_ON |
                                CEF_TEXT_INPUT_FLAG_SPELLCHECK_ON |
                                CEF_TEXT_INPUT_FLAG_AUTOCORRECT_ON;

          NWebInputMethodClient::InputInfo inputInfo;
          inputInfo.node_id = 12345;
          inputInfo.show_keyboard = true;
          inputInfo.input_mode = mode;
          inputInfo.input_type = type;
          inputInfo.input_action = action;
          inputInfo.input_flags =
              static_cast<cef_text_input_flags_t>(flagsValue);
          inputInfo.always_hide_ime = false;

          for (int enterKeyType = 0;
               enterKeyType <=
               static_cast<int>(IMFAdapterEnterKeyType::NEW_LINE);
               enterKeyType++) {
            handler.Attach(browser.get(), inputInfo, resetListener,
                           enterKeyType);
            handler.Attach(nullBrowser.get(), inputInfo, noResetListener,
                           enterKeyType);
            handler.Attach(browser.get(), inputInfo, resetListener,
                           enterKeyType, 1);
            handler.Attach(browser.get(), inputInfo, resetListener,
                           enterKeyType, 100);
          }
        }
      }
    }

    handler.Reattach(9999, NWebInputMethodHandler::ReattachType::FROM_ONFOCUS);
    handler.Reattach(9999, NWebInputMethodHandler::ReattachType::FROM_CONTINUE);
    handler.Reattach(9999, NWebInputMethodHandler::ReattachType::FROM_ONDRAG);
    handler.ShowTextInput();
    handler.HideTextInput(
        0, NWebInputMethodHandler::HideTextinputType::FROM_KERNEL, false);
    handler.HideTextInput(
        12345, NWebInputMethodHandler::HideTextinputType::FROM_ONBLUR, true);
    handler.HideTextInputForce();
    std::string selectedTextUtf8 = "Fixed Selected Text";
    CefString selectedText(selectedTextUtf8);
    CefRange selectedRange = {2, 8};
    handler.OnTextSelectionChanged(browser.get(), selectedText, selectedRange);
    handler.OnTextSelectionChanged(nullBrowser.get(), selectedText,
                                   selectedRange);
    CefRange reversedRange = {8, 2};
    handler.OnSelectionChanged(browser.get(), selectedText, reversedRange);
    handler.SetFocusStatus(true);
    handler.SetFocusStatus(false);
    CefRect cursorRect = {100, 200, 20, 30};
    handler.OnCursorUpdate(cursorRect);
    CefRect zeroRect = {0, 0, 0, 0};
    handler.OnCursorUpdate(zeroRect);
    handler.OnEditableChanged(browser.get(), true);
    handler.OnEditableChanged(browser.get(), false);
    handler.GetIsEditableNode();
    handler.SetIMEStatus(true);
    handler.SetIMEStatus(false);
    handler.WebBlurKeyboardHide();
    handler.HandleSecurityLayer();
    handler.SetScreenOffSet(10.5, 20.5);
    handler.SetScreenOffSet(0.0, 0.0);
    handler.SetVirtualDeviceRatio(1.5f);
    handler.SetVirtualDeviceRatio(2.0f);
    handler.SetWindowIdForIME(5000);
    handler.SetWindowIdForIME(0);
    handler.SetNeedUnderLine(true);
    handler.SetNeedUnderLine(false);
    handler.HasComposition();
    CefRange compositionRange = {3, 7};
    handler.OnImeCompositionRangeChanged(browser.get(), compositionRange);
    handler.OnImeCompositionRangeChanged(nullBrowser.get(), compositionRange);
    handler.OnUpdateTextInputStateCalled(browser.get(), selectedText,
                                         selectedRange, compositionRange);
    handler.IsAttached();
    handler.SetNeedReattachOnfocus();
#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
    handler.SetFillContent("Fixed Fill Content", 6789);
    handler.SetFillContent("", 0);
#endif
#if BUILDFLAG(ARKWEB_CLIPBOARD)
    handler.GetSelectInfo();
#endif
#if BUILDFLAG(ARKWEB_AI_WRITE)
    handler.GetSelectStartIndex();
    handler.GetSelectEndIndex();
    handler.GetAllTextInfo();
#endif
    handler.SetHasCompositionForTest(true);
    handler.SetHasCompositionForTest(false);
    std::string whole_text_str = "This is a test string";
    handler.SetWholeTextForTest(
        std::u16string(whole_text_str.begin(), whole_text_str.end()));
    std::string preview_str = "preview";
    handler.SetPreviewTextCacheForTest(
        std::u16string(preview_str.begin(), preview_str.end()));
    handler.SetCompositionRangeStartForTest(5);
    handler.SetCompositionRangeEndForTest(8);
    std::string text_str = "test";
    std::u16string text(text_str.begin(), text_str.end());

    struct {
      int32_t start;
      int32_t end;
      CompositionType type;
    } testCases[] = {{0, 5, COMPOSITION_CURRENT},
                     {2, 4, COMPOSITION_POSITION},
                     {1, 6, COMPOSITION_REPLACE},
                     {-1, 3, COMPOSITION_CANCEL},
                     {0, 100, COMPOSITION_DELETE},
                     {5, 2, COMPOSITION_INVALID},
                     {0, text.length(), COMPOSITION_CURRENT},
                     {2, text.length() - 1, COMPOSITION_POSITION}};

    for (const auto& testCase : testCases) {
      CompositionType tempType = testCase.type;
      int32_t result = handler.GetCompositionTypeAndCheckInput(
          text, testCase.start, testCase.end, tempType);
    }

    {
      std::u16string empty_text;
      CompositionType comp_type = COMPOSITION_CURRENT;
      int32_t result =
          handler.GetCompositionTypeAndCheckInput(empty_text, 0, 0, comp_type);
    }

    handler.CancelPreviewHandlerOnUI();
    handler.FinishPreviewTextOnUI();
    handler.DeleteForwardHandlerOnUI(3);
    handler.DeleteBackwardHandlerOnUI(2);

    for (int i = 0; i <= static_cast<int>(IMFAdapterEnterKeyType::NEW_LINE);i++) {
      int32_t enterKeyType = static_cast<int32_t>(i);
      handler.SendEnterKeyEvent(enterKeyType);
      handler.SendEnterKeyEventOnUI(enterKeyType);
    }

    handler.AutoFillWithIMFEventOnUI(true, true, true, "fixed_autofill");
    handler.AutoFillWithIMFEventOnUI(false, false, false, "");
  }
}
}  // namespace OHOS::NWeb

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size < sizeof(uint32_t)) {
    return;
  }
  NWebInputMethodHandlerFuzzTest(data, size);
  return 0;
}
