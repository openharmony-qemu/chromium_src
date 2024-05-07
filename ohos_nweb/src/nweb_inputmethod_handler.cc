/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nweb_inputmethod_handler.h"
#include <codecvt>
#include "nweb_imf_cursor_info_adapter_impl.h"
#include "nweb_imf_input_attribute_adapter_impl.h"
#include "nweb_imf_selection_range_adapter_impl.h"
#include "nweb_imf_text_config_adapter_impl.h"

#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "cef/include/cef_task.h"
#include "content/public/browser/browser_thread.h"
#include "libcef/browser/thread_util.h"
#include "ohos_adapter_helper.h"
#include "res_sched_client_adapter.h"
#include "ui/events/keycodes/keyboard_codes_posix.h"

namespace OHOS::NWeb {

static constexpr char16_t DEL_CHAR = 127;

class OnTextChangedListenerImpl : public IMFTextListenerAdapter {
 public:
  OnTextChangedListenerImpl(NWebInputMethodHandler* handler)
      : handler_(handler) {}
  ~OnTextChangedListenerImpl() = default;

  // All following listenser callbacks should invoke call on UI thread
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
    handler_->SendEnterKeyEvent();
  }

  void SetKeyboardStatus(bool status) override {
    handler_->SetIMEStatus(status);
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

 private:
  NWebInputMethodHandler* handler_;
};

class InputMethodTask : public CefTask {
 public:
  explicit InputMethodTask(base::OnceClosure closure)
      : closure_(std::move(closure)) {}

  InputMethodTask(const InputMethodTask&) = delete;
  InputMethodTask& operator=(const InputMethodTask&) = delete;

  virtual void Execute() override {
    std::move(closure_).Run();
    // closure_.Reset();
  }

 private:
  base::OnceClosure closure_;

  IMPLEMENT_REFCOUNTING(InputMethodTask);
};

NWebInputMethodHandler::NWebInputMethodHandler()
    : selected_from_(0), selected_to_(0) {
  inputmethod_adapter_ = OhosAdapterHelper::GetInstance().CreateIMFAdapter();
  if (inputmethod_adapter_ == nullptr) {
    LOG(ERROR) << "inputmethod_adapter_ create failed";
  }
}

NWebInputMethodHandler::~NWebInputMethodHandler() {}

uint32_t NWebInputMethodHandler::lastAttachNWebId_ = 0;
IMFAdapterTextInputType NWebInputMethodHandler::lastInputMode_ =
    IMFAdapterTextInputType::NONE;

std::shared_ptr<IMFCursorInfoAdapter> NWebInputMethodHandler::GetCursorInfo() {
  std::shared_ptr<NWebIMFCursorInfoAdapterImpl> cursorInfo =
      std::make_shared<NWebIMFCursorInfoAdapterImpl>();
  double left =
      (focus_rect_.x + focus_rect_.width) * device_pixel_ratio_ + offset_x_;
  double top = focus_rect_.y * device_pixel_ratio_ + offset_y_;
  double width = focus_rect_.width * device_pixel_ratio_;
  double height = focus_rect_.height * device_pixel_ratio_;

  cursorInfo->SetLeft(left);
  cursorInfo->SetTop(top);
  cursorInfo->SetWidth(width);
  cursorInfo->SetHeight(height);

  LOG(DEBUG) << "NWebInputMethodHandler::Attach cursorInfo.left = " << left
             << ", cursorInfo.top = " << top << ", cursorInfo.width = " << width
             << ", cursorInfo.height = " << height;
  return cursorInfo;
}

IMFAdapterTextInputType TextInputModeToIMFAdapter(cef_text_input_mode_t mode) {
  switch (mode) {
    case CEF_TEXT_INPUT_MODE_TEXT:
      return IMFAdapterTextInputType::TEXT;
    case CEF_TEXT_INPUT_MODE_TEL:
      return IMFAdapterTextInputType::PHONE;
    case CEF_TEXT_INPUT_MODE_URL:
      return IMFAdapterTextInputType::URL;
    case CEF_TEXT_INPUT_MODE_EMAIL:
      return IMFAdapterTextInputType::EMAIL_ADDRESS;
    case CEF_TEXT_INPUT_MODE_NUMERIC:
    case CEF_TEXT_INPUT_MODE_DECIMAL:
      return IMFAdapterTextInputType::NUMBER;
    case CEF_TEXT_INPUT_MODE_SEARCH:
    default:
      return IMFAdapterTextInputType::TEXT;
  }
}

IMFAdapterTextInputType TextInputTypeToIMFAdapter(cef_text_input_type_t type) {
  switch (type) {
    case CEF_TEXT_INPUT_TYPE_TEXT:
      return IMFAdapterTextInputType::TEXT;
    case CEF_TEXT_INPUT_TYPE_PASSWORD:
      return IMFAdapterTextInputType::VISIBLE_PASSWORD;
    case CEF_TEXT_INPUT_TYPE_EMAIL:
      return IMFAdapterTextInputType::EMAIL_ADDRESS;
    case CEF_TEXT_INPUT_TYPE_NUMBER:
      return IMFAdapterTextInputType::NUMBER;
    case CEF_TEXT_INPUT_TYPE_TELEPHONE:
      return IMFAdapterTextInputType::PHONE;
    case CEF_TEXT_INPUT_TYPE_URL:
      return IMFAdapterTextInputType::URL;
    default:
      return IMFAdapterTextInputType::TEXT;
  }
}

void NWebInputMethodHandler::Attach(CefRefPtr<CefBrowser> browser,
                                    bool show_keyboard,
                                    cef_text_input_mode_t input_mode,
                                    cef_text_input_type_t input_type) {
  show_keyboard_ = show_keyboard;
  if (input_mode != CEF_TEXT_INPUT_MODE_DEFAULT &&
      input_type != CEF_TEXT_INPUT_TYPE_PASSWORD) {
    input_mode_ = TextInputModeToIMFAdapter(input_mode);
  } else {
    input_mode_ = TextInputTypeToIMFAdapter(input_type);
  }
  composing_text_.clear();
  browser_ = browser;
  if (inputmethod_adapter_ == nullptr) {
    LOG(ERROR) << "inputmethod_adapter_ is nullptr";
    return;
  }
  if (inputmethod_listener_ == nullptr) {
    inputmethod_listener_ = std::make_shared<OnTextChangedListenerImpl>(this);
  }

  std::shared_ptr<NWebIMFInputAttributeAdapterImpl> inputAttribute =
      std::make_shared<NWebIMFInputAttributeAdapterImpl>();
  inputAttribute->SetInputPattern(static_cast<int32_t>(input_mode_));
  inputAttribute->SetEnterKeyType(
      static_cast<int32_t>(IMFAdapterEnterKeyType::DONE));

  std::shared_ptr<IMFCursorInfoAdapter> cursorInfo = GetCursorInfo();

  std::shared_ptr<NWebIMFTextConfigAdapterImpl> textConfig =
      std::make_shared<NWebIMFTextConfigAdapterImpl>();
  textConfig->SetInputAttribute(inputAttribute);
  textConfig->SetCursorInfo(cursorInfo);
  textConfig->SetWindowId(windowId_);

  if (!inputmethod_adapter_->Attach(inputmethod_listener_, show_keyboard_,
                                    textConfig)) {
    LOG(ERROR) << "inputmethod_adapter_ attach failed";
    return;
  }
  isAttached_ = true;
  lastAttachNWebId_ = nweb_id_;
  lastInputMode_ = input_mode_;

  if (focus_status_ && focus_rect_status_) {
    if (inputmethod_adapter_) {
      inputmethod_adapter_->OnCursorUpdate(cursorInfo);
    }
  }
}

bool NWebInputMethodHandler::Reattach(uint32_t nwebId, ReattachType type) {
  nweb_id_ = nwebId;
  if (type == ReattachType::FROM_CONTINUE) {
    if (!isNeedReattachOncontinue_ || !is_editable_node_) {
      LOG(INFO) << "don't need reattach input method";
      return false;
    }
    isNeedReattachOncontinue_ = false;
  }

  if (type == ReattachType::FROM_ONFOCUS) {
    if (!isNeedReattachOnfocus_ || !is_editable_node_) {
      LOG(INFO) << "ReAttchOnfocus, don't need reattach input method";
      return false;
    }
    isNeedReattachOnfocus_ = false;
  }

  LOG(INFO) << "need to reattach input method, show_keyboard_ = "
            << show_keyboard_ << ", \
    reattach type = "
            << static_cast<int>(type);
  composing_text_.clear();
  if (inputmethod_listener_ == nullptr) {
    inputmethod_listener_ = std::make_shared<OnTextChangedListenerImpl>(this);
  }

  if (!show_keyboard_ && !isAttached_) {
    inputmethod_adapter_->HideTextInput();
    inputmethod_adapter_->Close();
    return false;
  }

  std::shared_ptr<NWebIMFInputAttributeAdapterImpl> inputAttribute =
      std::make_shared<NWebIMFInputAttributeAdapterImpl>();
  inputAttribute->SetInputPattern(static_cast<int32_t>(input_mode_));
  inputAttribute->SetEnterKeyType(
      static_cast<int32_t>(IMFAdapterEnterKeyType::DONE));

  std::shared_ptr<IMFCursorInfoAdapter> cursorInfo = GetCursorInfo();

  std::shared_ptr<NWebIMFTextConfigAdapterImpl> textConfig =
      std::make_shared<NWebIMFTextConfigAdapterImpl>();
  textConfig->SetInputAttribute(inputAttribute);
  textConfig->SetCursorInfo(cursorInfo);
  textConfig->SetWindowId(windowId_);

  if (!show_keyboard_ && isAttached_ && input_mode_ != lastInputMode_) {
    LOG(ERROR) << "do not need attach";
    inputmethod_adapter_->Close();
  }
  if (!inputmethod_adapter_->Attach(inputmethod_listener_, show_keyboard_,
                                    textConfig)) {
    LOG(ERROR) << "inputmethod_adapter_ attach failed";
    return false;
  }
  isAttached_ = true;
  lastAttachNWebId_ = nwebId;
  lastInputMode_ = input_mode_;

  return show_keyboard_;
}

void NWebInputMethodHandler::ShowTextInput() {
  LOG(INFO) << "NWebInputMethodHandler::ShowTextInput";
}

void NWebInputMethodHandler::HideTextInput(uint32_t nwebId,
                                           HideTextinputType hideType) {
  if (inputmethod_adapter_ == nullptr) {
    LOG(ERROR) << "inputmethod_adapter_ is nullptr";
    return;
  }
  if (!isAttached_) {
    LOG(INFO) << "keyboard is not attach";
    if (hideType != HideTextinputType::FROM_ONPAUSE) {
      LOG(INFO) << "not from switch front and background, ingnore";
      return;
    }
    auto nowTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> diff =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            nowTime - lastCloseInputMethodTime_);
    // 100: received another hide textinput event in 100ms, we set
    // isNeedReattachOncontinue_ flag.
    if (diff.count() < 100) {
      isNeedReattachOncontinue_ = true;
      LOG(INFO) << "set isNeedReattachOncontinue_ flag, diff = " << diff.count()
                << "ms";
    }
    return;
  }

  if (lastAttachNWebId_ == 0 || lastAttachNWebId_ == nwebId ||
      hideType == HideTextinputType::FROM_KERNEL) {
    LOG(INFO) << "need to hide text input, hidetype = "
              << static_cast<int>(hideType);
    inputmethod_adapter_->HideTextInput();
    inputmethod_adapter_->Close();
  }

  lastCloseInputMethodTime_ = std::chrono::high_resolution_clock::now();
  isAttached_ = false;
  if (hideType == HideTextinputType::FROM_ONPAUSE) {
    isNeedReattachOncontinue_ = true;
  }

  if (hideType == HideTextinputType::FROM_ONBLUR) {
    isNeedReattachOnfocus_ = true;
  }
}

void NWebInputMethodHandler::OnTextSelectionChanged(
    CefRefPtr<CefBrowser> browser,
    const CefString& selected_text,
    const CefRange& selected_range) {
  if (ime_text_composing_ && !composing_text_.empty()) {
    if (browser_ != nullptr && browser_->GetHost() != nullptr) {
      browser_->GetHost()->ImeFinishComposingText(false);
    }
  }
  selected_text_ = selected_text.ToString16();
  selected_from_ = selected_range.from;
  selected_to_ = selected_range.to;
  ime_text_composing_ = false;
  LOG(DEBUG) << "NWebInputMethodHandler::OnTextSelectionChanged text: "
             << selected_text_ << ", selected_from_: " << selected_from_
             << ", selected_to_: " << selected_to_;
  composing_text_.clear();
}

void NWebInputMethodHandler::OnCursorUpdate(const CefRect& rect) {
  focus_rect_ = rect;
  focus_rect_status_ = true;
  if (focus_status_) {
    std::shared_ptr<NWebIMFCursorInfoAdapterImpl> cursorInfo =
        std::make_shared<NWebIMFCursorInfoAdapterImpl>();
    double left = (rect.x + rect.width) * device_pixel_ratio_ + offset_x_;
    double top = rect.y * device_pixel_ratio_ + offset_y_;
    double width = rect.width * device_pixel_ratio_;
    double height = rect.height * device_pixel_ratio_;

    cursorInfo->SetLeft(left);
    cursorInfo->SetTop(top);
    cursorInfo->SetWidth(width);
    cursorInfo->SetHeight(height);

    LOG(DEBUG) << "NWebInputMethodHandler::OnCursorUpdate cursorInfo.left = "
               << left << ", cursorInfo.top = " << top
               << ", cursorInfo.width = " << width
               << ", cursorInfo.height = " << height;
    if (inputmethod_adapter_) {
      inputmethod_adapter_->OnCursorUpdate(cursorInfo);
    }
  }
}

void NWebInputMethodHandler::OnSelectionChanged(
    CefRefPtr<CefBrowser> browser,
    const CefString& text,
    const CefRange& selected_range) {
  whole_text_ = text;
  if (inputmethod_adapter_) {
    inputmethod_adapter_->OnSelectionChange(
        text.ToString16(), selected_range.from, selected_range.to);
  }
  {
    std::unique_lock<std::mutex> lock(textCursorMutex_);
    if (textCursorReady_ > 0) {
      textCursorReady_--;
    } else {
      LOG(INFO) << "OnSelectionChanged do not need notify_all";
      return;
    }
    if (textCursorReady_ == 0) {
      LOG(INFO) << "OnSelectionChanged notify_all";
      textCursorCv_.notify_all();
    }
  }
}

void NWebInputMethodHandler::SetIMEStatus(bool status) {
  if (browser_ != nullptr && browser_->GetHost() != nullptr) {
    CefRefPtr<CefTask> insert_task = new InputMethodTask(base::BindOnce(
        &NWebInputMethodHandler::SetIMEStatusOnUI, this, std::move(status)));
    browser_->GetHost()->PostTaskToUIThread(insert_task);
  }
}

void NWebInputMethodHandler::InsertText(const std::u16string& text) {
  if (text.empty()) {
    LOG(ERROR) << "insert text empty!";
    return;
  }

  if (browser_ != nullptr && browser_->GetHost() != nullptr) {
    CefRefPtr<CefTask> insert_task = new InputMethodTask(base::BindOnce(
        &NWebInputMethodHandler::InsertTextHandlerOnUI, this, std::move(text)));
    browser_->GetHost()->PostTaskToUIThread(insert_task);
  }
}

void NWebInputMethodHandler::DeleteBackward(int32_t length) {
  if (browser_ != nullptr && browser_->GetHost() != nullptr) {
    CefRefPtr<CefTask> delete_task = new InputMethodTask(base::BindOnce(
        &NWebInputMethodHandler::DeleteBackwardHandlerOnUI, this, length));
    browser_->GetHost()->PostTaskToUIThread(delete_task);
  }
}

void NWebInputMethodHandler::DeleteForward(int32_t length) {
  if (browser_ != nullptr && browser_->GetHost() != nullptr) {
    CefRefPtr<CefTask> delete_task = new InputMethodTask(base::BindOnce(
        &NWebInputMethodHandler::DeleteForwardHandlerOnUI, this, length));
    browser_->GetHost()->PostTaskToUIThread(delete_task);
  }
}

void NWebInputMethodHandler::SetIMEStatusOnUI(bool status) {
  if (!status && ime_text_composing_) {
    browser_->GetHost()->ImeFinishComposingText(false);
    ime_text_composing_ = false;
    composing_text_.clear();
  }
  ime_shown_ = status;
}

void NWebInputMethodHandler::InsertTextHandlerOnUI(const std::u16string& text) {
  if (text.empty()) {
    LOG(ERROR) << "insert text empty!";
    return;
  }
  {
    std::unique_lock<std::mutex> lock(textCursorMutex_);
    textCursorReady_++;
  }

  CefKeyEvent keyEvent;
  keyEvent.windows_key_code = ui::VKEY_PROCESSKEY;
  keyEvent.modifiers = 0;
  keyEvent.is_system_key = false;
  keyEvent.type = KEYEVENT_RAWKEYDOWN;
  browser_->GetHost()->SendKeyEvent(keyEvent);

  if (!ime_text_composing_) {
    ime_text_composing_ = true;
    composing_text_.clear();
  }
  composing_text_.append(text);
  browser_->GetHost()->ImeCommitText(composing_text_,
                                     CefRange(UINT32_MAX, UINT32_MAX), 0);

  if (text.length() > 1) {
    ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                       ResSchedSceneAdapter::CLICK);
  }

  // no selection
  ime_text_composing_ = false;
  selected_from_ += static_cast<int>(composing_text_.length());
  selected_to_ += static_cast<int>(composing_text_.length());
  composing_text_.clear();

  keyEvent.type = KEYEVENT_KEYUP;
  browser_->GetHost()->SendKeyEvent(keyEvent);
}

void NWebInputMethodHandler::PreviewTextHandlerOnUI(const std::u16string& text,
                                                    int32_t start,
                                                    int32_t end) {
  if (text.empty() || (start > end)) {
    LOG(ERROR) << "input para invalid";
    return;
  }
  {
    std::unique_lock<std::mutex> lock(textCursorMutex_);
    textCursorReady_++;
  }
  bool is_need_replace = ((start == -1) && (end == -1)) ? false : true;
  LOG(DEBUG) << "NWebInputMethodHandler::text " << text << ", start " << start
             << ", end " << end << ", is_need_replace " << is_need_replace;

  std::vector<CefCompositionUnderline> underlines;
  CefCompositionUnderline underline;
  underline.range.from = 0;
  underline.range.to = text.length();
  if (is_need_underline_) {
    underline.style = CEF_CUS_SOLID;
  } else {
    underline.style = CEF_CUS_NONE;
  }
  underlines.push_back(underline);

  CefRange replace_range = CefRange::InvalidRange();
  if (is_need_replace) {
    replace_range.Set(start, end);
  }
  CefRange selection_range(text.length(), text.length());
  LOG(DEBUG) << "NWebInputMethodHandler::underline style: " << underline.style
             << ", from: " << underline.range.from
             << ", to: " << underline.range.to;
  LOG(DEBUG) << "NWebInputMethodHandler::replace_range from "
             << replace_range.from << ", to " << replace_range.to;
  LOG(DEBUG) << "NWebInputMethodHandler::selection_range from "
             << selection_range.from << ", to " << selection_range.to;

  browser_->GetHost()->ImeSetComposition(text, underlines, replace_range,
                                         selection_range);
}

void NWebInputMethodHandler::FinishPreviewTextOnUI() {
  if (browser_ != nullptr && browser_->GetHost() != nullptr) {
    LOG(DEBUG) << "NWebInputMethodHandler::FinishPreviewTextOnUI";
    browser_->GetHost()->ImeFinishComposingText(false);
  }
  // InsertTextHandlerOnUI(preview_text_);
}

void NWebInputMethodHandler::SetNeedUnderLineOnUI(bool is_need_underline) {
  LOG(DEBUG) << "NWebInputMethodHandler::SetNeedUnderLine "
             << is_need_underline;
  is_need_underline_ = is_need_underline;
}

void NWebInputMethodHandler::DeleteForwardHandlerOnUI(int32_t length) {
  CefKeyEvent keyEvent;
  keyEvent.windows_key_code = ui::VKEY_DELETE;
  keyEvent.native_key_code = static_cast<int>(ScanKeyCode::DELETE_SCAN_CODE);
  keyEvent.modifiers = 0;
  keyEvent.is_system_key = false;
  keyEvent.character = keyEvent.unmodified_character = DEL_CHAR;
  LOG(DEBUG) << "NWebInputMethodHandler::DeleteForwardHandlerOnUI";

  if (!browser_ || !browser_->GetHost()) {
    LOG(ERROR) << "delete backward browser get failed";
    return;
  }
  is_need_notify_all_ = false;
  text_cursor_length_ = length;
  selected_from_ = selected_from_ >= whole_text_.size() ? whole_text_.size()
                                                        : selected_from_;
  if (whole_text_.substr(selected_from_).size() <= length) {
    text_cursor_length_ = whole_text_.substr(selected_from_).size();
    if (selected_from_ == 0) {
      is_need_notify_all_ = true;
    }
  } else {
    std::unique_lock<std::mutex> lock(textCursorMutex_);
    textCursorReady_ += text_cursor_length_;
  }
  for (int32_t i = 0; i < length; i++) {
    keyEvent.type = KEYEVENT_RAWKEYDOWN;
    browser_->GetHost()->SendKeyEvent(keyEvent);
    keyEvent.type = KEYEVENT_CHAR;
    browser_->GetHost()->SendKeyEvent(keyEvent);
  }
  keyEvent.type = KEYEVENT_KEYUP;
  browser_->GetHost()->SendKeyEvent(keyEvent);
}

void NWebInputMethodHandler::DeleteBackwardHandlerOnUI(int32_t length) {
  CefKeyEvent keyEvent;
  keyEvent.windows_key_code = ui::VKEY_BACK;
  keyEvent.native_key_code = static_cast<int>(ScanKeyCode::BACKSPACE_SCAN_CODE);
  keyEvent.modifiers = 0;
  keyEvent.is_system_key = false;
  keyEvent.character = keyEvent.unmodified_character = DEL_CHAR;
  LOG(DEBUG) << "NWebInputMethodHandler::DeleteBackwardHandlerOnUI";

  if (!browser_ || !browser_->GetHost()) {
    LOG(ERROR) << "delete forward browser get failed";
    return;
  }
  text_cursor_length_ = length;
  if (selected_from_ <= length) {
    text_cursor_length_ = selected_from_;
  } else {
    std::unique_lock<std::mutex> lock(textCursorMutex_);
    textCursorReady_ += text_cursor_length_;
  }
  for (int32_t i = 0; i < length; i++) {
    keyEvent.type = KEYEVENT_RAWKEYDOWN;
    browser_->GetHost()->SendKeyEvent(keyEvent);
    keyEvent.type = KEYEVENT_CHAR;
    browser_->GetHost()->SendKeyEvent(keyEvent);
  }
  keyEvent.type = KEYEVENT_KEYUP;
  browser_->GetHost()->SendKeyEvent(keyEvent);
}

void NWebInputMethodHandler::SendEnterKeyEvent() {
  CefKeyEvent keyEvent;
  keyEvent.windows_key_code = ui::VKEY_RETURN;
  keyEvent.native_key_code = static_cast<int>(ScanKeyCode::ENTER_SCAN_CODE);
  LOG(DEBUG) << "NWebInputMethodHandler::SendEnterKeyEvent";

  keyEvent.type = KEYEVENT_KEYDOWN;
  keyEvent.character = '\r';
  keyEvent.modifiers = 0;
  keyEvent.is_system_key = false;
  if (browser_ && browser_->GetHost()) {
    browser_->GetHost()->SendKeyEvent(keyEvent);
  }

  keyEvent.type = KEYEVENT_CHAR;
  if (browser_ && browser_->GetHost()) {
    browser_->GetHost()->SendKeyEvent(keyEvent);
  }

  keyEvent.type = KEYEVENT_KEYUP;
  if (browser_ && browser_->GetHost()) {
    browser_->GetHost()->SendKeyEvent(keyEvent);
  }
}

void NWebInputMethodHandler::MoveCursor(const IMFAdapterDirection direction) {
  LOG(INFO) << "NWebInputMethodHandler::MoveCursor called";
  CefKeyEvent keyEvent;

  switch (direction) {
    case IMFAdapterDirection::UP: {
      keyEvent.windows_key_code = ui::VKEY_UP;
      keyEvent.native_key_code = static_cast<int>(ScanKeyCode::UP_SCAN_CODE);
      break;
    }
    case IMFAdapterDirection::LEFT: {
      keyEvent.windows_key_code = ui::VKEY_LEFT;
      keyEvent.native_key_code = static_cast<int>(ScanKeyCode::LEFT_SCAN_CODE);
      break;
    }
    case IMFAdapterDirection::RIGHT: {
      keyEvent.windows_key_code = ui::VKEY_RIGHT;
      keyEvent.native_key_code = static_cast<int>(ScanKeyCode::RIGHT_SCAN_CODE);
      break;
    }
    case IMFAdapterDirection::DOWN: {
      keyEvent.windows_key_code = ui::VKEY_DOWN;
      keyEvent.native_key_code = static_cast<int>(ScanKeyCode::DOWN_SCAN_CODE);
      break;
    }
    default: {
      LOG(ERROR) << "invalid direction";
      return;
    }
  }
  {
    std::unique_lock<std::mutex> lock(textCursorMutex_);
    textCursorReady_++;
  }
  keyEvent.type = KEYEVENT_KEYDOWN;
  keyEvent.modifiers = 0;
  keyEvent.is_system_key = false;
  if (browser_ && browser_->GetHost()) {
    browser_->GetHost()->SendKeyEvent(keyEvent);
  }

  keyEvent.type = KEYEVENT_KEYUP;
  if (browser_ && browser_->GetHost()) {
    browser_->GetHost()->SendKeyEvent(keyEvent);
  }
}

void NWebInputMethodHandler::SetScreenOffSet(double x, double y) {
  if (focus_status_) {
    if (focus_rect_status_ && (offset_x_ != x || offset_y_ != y)) {
      std::shared_ptr<NWebIMFCursorInfoAdapterImpl> cursorInfo =
          std::make_shared<NWebIMFCursorInfoAdapterImpl>();
      double left =
          (focus_rect_.x + focus_rect_.width) * device_pixel_ratio_ + x;
      double top = focus_rect_.y * device_pixel_ratio_ + y;
      double width = focus_rect_.width * device_pixel_ratio_;
      double height = focus_rect_.height * device_pixel_ratio_;

      cursorInfo->SetLeft(left);
      cursorInfo->SetTop(top);
      cursorInfo->SetWidth(width);
      cursorInfo->SetHeight(height);

      LOG(DEBUG) << "NWebInputMethodHandler::SetScreenOffSet cursorInfo.left = "
                 << left << ", cursorInfo.top = " << top
                 << ", cursorInfo.width = " << width
                 << ", cursorInfo.height = " << height;
      if (inputmethod_adapter_) {
        inputmethod_adapter_->OnCursorUpdate(cursorInfo);
      }
    }
  }
  offset_x_ = x;
  offset_y_ = y;
}

void NWebInputMethodHandler::SetVirtualDeviceRatio(float device_pixel_ratio) {
  device_pixel_ratio_ = device_pixel_ratio;
}

void NWebInputMethodHandler::SetFocusStatus(bool focus_status) {
  if (focus_status && focus_rect_status_) {
    std::shared_ptr<NWebIMFCursorInfoAdapterImpl> cursorInfo =
        std::make_shared<NWebIMFCursorInfoAdapterImpl>();
    double left =
        (focus_rect_.x + focus_rect_.width) * device_pixel_ratio_ + offset_x_;
    double top = focus_rect_.y * device_pixel_ratio_ + offset_y_;
    double width = focus_rect_.width * device_pixel_ratio_;
    double height = focus_rect_.height * device_pixel_ratio_;

    cursorInfo->SetLeft(left);
    cursorInfo->SetTop(top);
    cursorInfo->SetWidth(width);
    cursorInfo->SetHeight(height);
    LOG(DEBUG) << "NWebInputMethodHandler::SetFocusStatus cursorInfo.left = "
               << left << ", cursorInfo.top = " << top << ", width = " << width
               << ", height = " << height;
    if (inputmethod_adapter_) {
      inputmethod_adapter_->OnCursorUpdate(cursorInfo);
    }
  }
  focus_status_ = focus_status;
}

void NWebInputMethodHandler::OnEditableChanged(CefRefPtr<CefBrowser> browser,
                                               bool is_editable_node) {
  is_editable_node_ = is_editable_node;
}

bool NWebInputMethodHandler::GetIsEditableNode() {
  LOG(INFO) << "NWebInputMethodHandler is_editable_node_ = "
            << is_editable_node_;
  return is_editable_node_;
}

int32_t NWebInputMethodHandler::GetTextIndexAtCursor() {
  std::unique_lock<std::mutex> lock(textCursorMutex_);
  bool istextCursorReady = textCursorCv_.wait_for(
      lock, std::chrono::seconds(1), [this] { return textCursorReady_ == 0; });
  textCursorReady_ = 0;
  if (!istextCursorReady) {
    LOG(ERROR) << "GetTextIndexAtCursor wait_for timeout";
    if (ResetTextSelectiondata()) {
      return 0;
    }
  }
  LOG(DEBUG) << "NWebInputMethodHandler::GetTextIndexAtCursor selected_to_ "
             << selected_to_;
  return selected_to_;
}

std::u16string NWebInputMethodHandler::GetLeftTextOfCursor(int32_t number) {
  std::unique_lock<std::mutex> lock(textCursorMutex_);
  bool istextCursorReady = textCursorCv_.wait_for(
      lock, std::chrono::seconds(1), [this] { return textCursorReady_ == 0; });
  textCursorReady_ = 0;
  if (!istextCursorReady) {
    LOG(ERROR) << "GetLeftTextOfCursor wait_for timeout";
    if (ResetTextSelectiondata()) {
      return u"";
    }
  }
  int32_t selectBegin = selected_from_;
  int32_t selectEnd = selected_to_;
  if (!IsCorrectParam(number, selectBegin, selectEnd)) {
    return u"";
  }

  int32_t startPos = (number <= selectBegin ? (selectBegin - number) : 0);
  int32_t length = (number <= selectBegin ? number : selectBegin);
  return whole_text_.substr(startPos, length);
}

std::u16string NWebInputMethodHandler::GetRightTextOfCursor(int32_t number) {
  std::unique_lock<std::mutex> lock(textCursorMutex_);

  bool istextCursorReady = textCursorCv_.wait_for(
      lock, std::chrono::seconds(1), [this] { return textCursorReady_ == 0; });
  textCursorReady_ = 0;
  if (!istextCursorReady) {
    LOG(ERROR) << "GetRightTextOfCursor wait_for timeout";
    if (ResetTextSelectiondata()) {
      return u"";
    }
  }
  int32_t selectBegin = selected_from_;
  int32_t selectEnd = selected_to_;
  if (!IsCorrectParam(number, selectBegin, selectEnd)) {
    return u"";
  }
  return whole_text_.substr(selectEnd, number);
}

int32_t NWebInputMethodHandler::SetPreviewText(const std::u16string& text,
                                               int32_t start,
                                               int32_t end) {
  if (text.empty()) {
    LOG(ERROR) << "insert text empty!";
    return ERROR;
  }

  if (browser_ != nullptr && browser_->GetHost() != nullptr) {
    CefRefPtr<CefTask> task = new InputMethodTask(
        base::BindOnce(&NWebInputMethodHandler::PreviewTextHandlerOnUI, this,
                       std::move(text), start, end));
    browser_->GetHost()->PostTaskToUIThread(task);
  }

  return OK;
}

void NWebInputMethodHandler::FinishTextPreview() {
  if (browser_ != nullptr && browser_->GetHost() != nullptr) {
    CefRefPtr<CefTask> task = new InputMethodTask(
        base::BindOnce(&NWebInputMethodHandler::FinishPreviewTextOnUI, this));
    browser_->GetHost()->PostTaskToUIThread(task);
  }
}

void NWebInputMethodHandler::SetNeedUnderLine(bool is_need_underline) {
  if (browser_ != nullptr && browser_->GetHost() != nullptr) {
    CefRefPtr<CefTask> task = new InputMethodTask(
        base::BindOnce(&NWebInputMethodHandler::SetNeedUnderLineOnUI, this,
                       is_need_underline));
    browser_->GetHost()->PostTaskToUIThread(task);
  }
}

#if defined(OHOS_CLIPBOARD)
std::string NWebInputMethodHandler::GetSelectInfo() {
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
  return converter.to_bytes(selected_text_);
}
#endif

void NWebInputMethodHandler::SetWindowIdForIME(uint32_t windowId) {
  LOG(INFO) << "NWebInputMethodHandler::SetWindowIdForIME windowId: "
            << windowId;
  windowId_ = windowId;
}

bool NWebInputMethodHandler::IsCorrectParam(int32_t number,
                                            int32_t& selectBegin,
                                            int32_t& selectEnd) {
  if (number < 0 || selectBegin < 0 || selectEnd < 0) {
    LOG(ERROR) << "param error, number:" << number
               << ", selectBegin:" << selectBegin
               << ", selectEnd:" << selectEnd;
    return false;
  }

  if (selectBegin > selectEnd) {
    std::swap(selectBegin, selectEnd);
  }
  if (selectEnd > whole_text_.size()) {
    LOG(ERROR) << "param error, end:" << selectEnd;
    return false;
  }
  return true;
}

bool NWebInputMethodHandler::ResetTextSelectiondata() {
  if (is_need_notify_all_) {
    LOG(ERROR) << "NWebInputMethodHandler::ResetTextSelectiondata";
    whole_text_ = u"";
    selected_from_ = 0;
    selected_to_ = 0;
    textCursorReady_ = 0;
    is_need_notify_all_ = false;
    return true;
  }
  return false;
}
}  // namespace OHOS::NWeb
