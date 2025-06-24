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
 
 #include "content/public/browser/content_browser_client.h"
 #include "content/public/common/content_client.h"
 #include "gmock/gmock.h"
 #include "gtest/gtest.h"
 #include "ohos_nweb/include/nweb.h"
 #include "ohos_nweb/src/nweb_impl.h"
 #include "ohos_nweb/src/nweb_inputmethod_handler.h"
 
 using namespace OHOS::NWeb;
 namespace OHOS::NWeb {
 
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
 
 class MockFunctionKeyAdapter : public IMFAdapterFunctionKeyAdapter {
  public:
   MockFunctionKeyAdapter(FuzzedDataProvider& fdp) : fdp_(fdp) {}
 
   IMFAdapterEnterKeyType GetEnterKeyType() override {
     static const std::vector<IMFAdapterEnterKeyType> types = {
         IMFAdapterEnterKeyType::UNSPECIFIED, IMFAdapterEnterKeyType::NONE,
         IMFAdapterEnterKeyType::GO,          IMFAdapterEnterKeyType::SEARCH,
         IMFAdapterEnterKeyType::SEND,        IMFAdapterEnterKeyType::NEXT,
         IMFAdapterEnterKeyType::DONE,        IMFAdapterEnterKeyType::PREVIOUS,
         IMFAdapterEnterKeyType::NEW_LINE};
     return types[fdp_.ConsumeIntegralInRange<size_t>(0, types.size() - 1)];
   }
 
  private:
   FuzzedDataProvider& fdp_;
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
 
   const std::vector<cef_text_input_mode_t> allModes = {
       CEF_TEXT_INPUT_MODE_DEFAULT, CEF_TEXT_INPUT_MODE_NONE,
       CEF_TEXT_INPUT_MODE_TEXT,    CEF_TEXT_INPUT_MODE_TEL,
       CEF_TEXT_INPUT_MODE_URL,     CEF_TEXT_INPUT_MODE_EMAIL,
       CEF_TEXT_INPUT_MODE_NUMERIC, CEF_TEXT_INPUT_MODE_DECIMAL,
       CEF_TEXT_INPUT_MODE_SEARCH};
 
   const std::vector<cef_text_input_type_t> allTypes = {
       CEF_TEXT_INPUT_TYPE_TEXT,      CEF_TEXT_INPUT_TYPE_PASSWORD,
       CEF_TEXT_INPUT_TYPE_EMAIL,     CEF_TEXT_INPUT_TYPE_NUMBER,
       CEF_TEXT_INPUT_TYPE_TELEPHONE, CEF_TEXT_INPUT_TYPE_URL,
       CEF_TEXT_INPUT_TYPE_TEXT_AREA, CEF_TEXT_INPUT_TYPE_CONTENT_EDITABLE};
 
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
 
   {
     NWebInputMethodHandler handler;
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
 
     size_t dirIndex =
         fdp.ConsumeIntegralInRange<size_t>(0, directions.size() - 1);
     listener.MoveCursor(directions[dirIndex]);
 
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
 
     CefRect cursorRect;
     cursorRect.x = fdp.ConsumeIntegral<int>();
     cursorRect.y = fdp.ConsumeIntegral<int>();
     cursorRect.width = fdp.ConsumeIntegral<int>();
     cursorRect.height = fdp.ConsumeIntegral<int>();
     handler.OnCursorUpdate(cursorRect);
 
     handler.OnSelectionChanged(browser.get(), selectedText, selectedRange);
 
     bool focusStatus = fdp.ConsumeBool();
     handler.SetFocusStatus(focusStatus);
 
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
   }
 
   {
     NWebInputMethodHandler handler;
     OnTextChangedListenerImpl listener(&handler);
 
     std::u16string fixedInsertText = u"Fixed Test Text";
     listener.InsertText(fixedInsertText);
     listener.InsertText(u"\n");
 
     listener.DeleteForward(3);
     listener.DeleteBackward(2);
 
     listener.SendKeyEventFromInputMethod();
     listener.SendKeyboardStatus(IMFAdapterKeyboardStatus::SHOW);
 
     listener.MoveCursor(IMFAdapterDirection::LEFT);
     listener.MoveCursor(IMFAdapterDirection::RIGHT);
 
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
     bool resetListener = true;
     cef_text_input_mode_t mode = CEF_TEXT_INPUT_MODE_TEXT;
     cef_text_input_type_t type = CEF_TEXT_INPUT_TYPE_TEXT;
     cef_text_input_action_t action = CEF_TEXT_INPUT_ACTION_DEFAULT;
     uint32_t flagsValue =
         CEF_TEXT_INPUT_FLAG_AUTOCOMPLETE_ON | CEF_TEXT_INPUT_FLAG_SPELLCHECK_ON;
     NWebInputMethodClient::InputInfo inputInfo;
     inputInfo.node_id = 12345;
     inputInfo.show_keyboard = true;
     inputInfo.input_mode = mode;
     inputInfo.input_type = type;
     inputInfo.input_action = action;
     inputInfo.input_flags = static_cast<cef_text_input_flags_t>(flagsValue);
     inputInfo.always_hide_ime = false;
     int32_t enterKeyType = static_cast<int32_t>(IMFAdapterEnterKeyType::DONE);
 
     handler.Attach(browser.get(), inputInfo, resetListener, enterKeyType);
     handler.Attach(browser.get(), inputInfo, resetListener, enterKeyType, 1);
 
     handler.Reattach(9999, NWebInputMethodHandler::ReattachType::FROM_ONFOCUS);
 
     handler.ShowTextInput();
     handler.HideTextInput(
         0, NWebInputMethodHandler::HideTextinputType::FROM_KERNEL, false);
     handler.HideTextInputForce();
 
     std::string selectedTextUtf8 = "Fixed Selected Text";
     CefString selectedText(selectedTextUtf8);
     CefRange selectedRange = {2, 8};
     handler.OnTextSelectionChanged(browser.get(), selectedText, selectedRange);
 
     CefRect cursorRect = {100, 200, 20, 30};
     handler.OnCursorUpdate(cursorRect);
 
     handler.OnSelectionChanged(browser.get(), selectedText, selectedRange);
 
     handler.SetFocusStatus(true);
 
     handler.OnEditableChanged(browser.get(), true);
     handler.GetIsEditableNode();
 
     handler.SetIMEStatus(true);
 
     handler.WebBlurKeyboardHide();
 
     handler.HandleSecurityLayer();
 
     handler.SetScreenOffSet(10.5, 20.5);
 
     handler.SetVirtualDeviceRatio(1.5f);
 
     handler.SetWindowIdForIME(5000);
 
     handler.SetNeedUnderLine(true);
 
     handler.HasComposition();
 
     CefRange compositionRange = {3, 7};
     handler.OnImeCompositionRangeChanged(browser.get(), compositionRange);
 
     handler.OnUpdateTextInputStateCalled(browser.get(), selectedText,
                                          selectedRange, compositionRange);
 
     handler.IsAttached();
 
     handler.SetNeedReattachOnfocus();
 
 #if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
     handler.SetFillContent("Fixed Fill Content", 6789);
 #endif
 
 #if BUILDFLAG(ARKWEB_CLIPBOARD)
     handler.GetSelectInfo();
 #endif
 
 #if BUILDFLAG(ARKWEB_AI_WRITE)
     handler.GetSelectStartIndex();
     handler.GetSelectEndIndex();
     handler.GetAllTextInfo();
 #endif
   }
 }
 
 }  // namespace OHOS::NWeb
 
 extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
   if (data == nullptr || size < sizeof(uint32_t)) {
     return;
   }
   FuzzedDataProvider fdp(data, size);
   NWebInputMethodHandlerFuzzTest(data, size);
   return 0;
 }
 