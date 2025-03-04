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

#include "nweb_custom_keyboard_handler_impl.h"

#include "base/logging.h"
#include "nweb_inputmethod_handler.h"
#include "res_sched_client_adapter.h"
#include "ui/events/keycodes/keyboard_codes_posix.h"

namespace {
constexpr char16_t DEL_CHAR = 127;
}

namespace OHOS::NWeb {
void NWebCustomKeyboardHandlerImpl::InsertText(const std::string& text) {
  LOG(DEBUG) << "WebCustomKeyboard insert text, text = " << text;

  if (!isAttached_) {
    LOG(ERROR) << "WebCustomKeyboard custom keyboard is not attached, insert "
                  "text return";
    return;
  }

  if (text.empty()) {
    LOG(ERROR) << "WebCustomKeyboard insert text empty!";
    return;
  }

  if (!browser_ || !browser_->GetHost()) {
    LOG(ERROR) << "WebCustomKeyboard insert text failed, browser_ is nullptr!";
    return;
  }

  CefKeyEvent keyEvent;
  keyEvent.windows_key_code = ui::VKEY_PROCESSKEY;
  keyEvent.modifiers = 0;
  keyEvent.is_system_key = false;
  keyEvent.type = KEYEVENT_RAWKEYDOWN;
  browser_->GetHost()->SendKeyEvent(keyEvent);

  browser_->GetHost()->ImeCommitText(text, CefRange(UINT32_MAX, UINT32_MAX), 0);

  if (text.length() > 1) {
    ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                       ResSchedSceneAdapter::CLICK);
  }

  keyEvent.type = KEYEVENT_KEYUP;
  browser_->GetHost()->SendKeyEvent(keyEvent);
}

// delete char on left of input cursor
void NWebCustomKeyboardHandlerImpl::DeleteForward(int32_t length) {
  LOG(DEBUG) << "WebCustomKeyboard delete forward, delete len = " << length;

  if (!isAttached_) {
    LOG(ERROR) << "WebCustomKeyboard custom keyboard is not attached, delete "
                  "forward return";
    return;
  }

  if (!browser_ || !browser_->GetHost()) {
    LOG(ERROR)
        << "WebCustomKeyboard delete forward failed, browser_ is nullptr!";
    return;
  }

  CefKeyEvent keyEvent;
  keyEvent.windows_key_code = ui::VKEY_BACK;
  keyEvent.native_key_code = static_cast<int>(ScanKeyCode::BACKSPACE_SCAN_CODE);
  keyEvent.modifiers = 0;
  keyEvent.is_system_key = false;
  keyEvent.character = keyEvent.unmodified_character = DEL_CHAR;

  for (int32_t i = 0; i < length; i++) {
    keyEvent.type = KEYEVENT_RAWKEYDOWN;
    browser_->GetHost()->SendKeyEvent(keyEvent);
    keyEvent.type = KEYEVENT_CHAR;
    browser_->GetHost()->SendKeyEvent(keyEvent);
  }
  keyEvent.type = KEYEVENT_KEYUP;
  browser_->GetHost()->SendKeyEvent(keyEvent);
}

// delete char on right of input cursor
void NWebCustomKeyboardHandlerImpl::DeleteBackward(int32_t length) {
  LOG(DEBUG) << "WebCustomKeyboard delete backward, delete len = " << length;

  if (!isAttached_) {
    LOG(ERROR) << "WebCustomKeyboard custom keyboard is not attached, delete "
                  "backward return";
    return;
  }

  if (!browser_ || !browser_->GetHost()) {
    LOG(ERROR)
        << "WebCustomKeyboard delete backward failed, browser_ is nullptr!";
    return;
  }

  CefKeyEvent keyEvent;
  keyEvent.windows_key_code = ui::VKEY_DELETE;
  keyEvent.native_key_code = static_cast<int>(ScanKeyCode::DELETE_SCAN_CODE);
  keyEvent.modifiers = 0;
  keyEvent.is_system_key = false;
  keyEvent.character = keyEvent.unmodified_character = DEL_CHAR;

  for (int32_t i = 0; i < length; i++) {
    keyEvent.type = KEYEVENT_RAWKEYDOWN;
    browser_->GetHost()->SendKeyEvent(keyEvent);
    keyEvent.type = KEYEVENT_CHAR;
    browser_->GetHost()->SendKeyEvent(keyEvent);
  }
  keyEvent.type = KEYEVENT_KEYUP;
  browser_->GetHost()->SendKeyEvent(keyEvent);
}

void NWebCustomKeyboardHandlerImpl::SendFunctionKey(int32_t key) {
  LOG(DEBUG) << "WebCustomKeyboard send function key, key = " << key;

  if (!isAttached_) {
    LOG(ERROR) << "WebCustomKeyboard custom keyboard is not attached, send "
                  "function key return";
    return;
  }

  if (!browser_ || !browser_->GetHost()) {
    LOG(ERROR)
        << "WebCustomKeyboard send function key failed, browser_ is nullptr!";
    return;
  }

  if (key == static_cast<int32_t>(IMFAdapterEnterKeyType::NEXT) &&
      input_flags_ & CEF_TEXT_INPUT_FLAG_HAVE_NEXT_FOCUSABLE_ELEMENT) {
    browser_->GetHost()->AdvanceFocusForIME(
        static_cast<int>(FocusType::FORWARD));
    return;
  } else if (key == static_cast<int32_t>(IMFAdapterEnterKeyType::PREVIOUS) &&
             input_flags_ &
                 CEF_TEXT_INPUT_FLAG_HAVE_PREVIOUS_FOCUSABLE_ELEMENT) {
    browser_->GetHost()->AdvanceFocusForIME(
        static_cast<int>(FocusType::BACKWARD));
    return;
  }

  CefKeyEvent keyEvent;
  keyEvent.windows_key_code = ui::VKEY_RETURN;
  keyEvent.native_key_code = static_cast<int>(ScanKeyCode::ENTER_SCAN_CODE);

  keyEvent.type = KEYEVENT_KEYDOWN;
  keyEvent.character = '\r';
  keyEvent.modifiers = 0;
  keyEvent.is_system_key = false;
  browser_->GetHost()->SendKeyEvent(keyEvent);

  keyEvent.type = KEYEVENT_CHAR;
  browser_->GetHost()->SendKeyEvent(keyEvent);

  keyEvent.type = KEYEVENT_KEYUP;
  browser_->GetHost()->SendKeyEvent(keyEvent);
}

void NWebCustomKeyboardHandlerImpl::Attach(CefRefPtr<CefBrowser> browser,
                                           bool show_keyboard,
                                           int32_t input_flags) {
  LOG(INFO) << "WebCustomKeyboard attach, show_keyboard = " << show_keyboard;

  if (isAttached_) {
    LOG(ERROR) << "WebCustomKeyboard attach, custom keyboard is already "
                  "attached, don't attach again";
    return;
  }

  isAttached_ = true;
  browser_ = browser;
  input_flags_ = input_flags;
  if (auto handler = nweb_handler_.lock()) {
    handler->OnCustomKeyboardAttach();
  }
}

void NWebCustomKeyboardHandlerImpl::Close() {
  LOG(INFO) << "WebCustomKeyboard close";

  if (!isAttached_) {
    LOG(ERROR) << "WebCustomKeyboard close, custom keyboard is already closed, "
                  "don't close again";
    return;
  }

  isAttached_ = false;
  if (auto handler = nweb_handler_.lock()) {
    handler->OnCustomKeyboardClose();
  }
}

void NWebCustomKeyboardHandlerImpl::CloseFromWebStateChange(
    WebCustomKeyboardState mode) {
  LOG(INFO) << "WebCustomKeyboard CloseFromWebStateChange, mode = "
            << static_cast<int>(mode);

  if (!isAttached_) {
    LOG(INFO) << "WebCustomKeyboard CloseFromWebStateChange, custom keyboard "
                 "is already closed, don't close again";
    if (mode != WebCustomKeyboardState::FROM_ONPAUSE) {
      LOG(INFO)
          << "WebCustomKeyboard, not from switch front and background, ingnore";
      return;
    }
    auto nowTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> diff =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            nowTime - lastCloseInputMethodTime_);
    // 100: received another hide textinput event in 100ms, we set
    // isCloseFromOnpause_ flag, onFocus miss, so need onContinue to reattach.
    if (diff.count() < 100) {
      isCloseFromOnpause_ = true;
      LOG(INFO) << "WebCustomKeyboard set isCloseFromOnpause_ flag, diff = "
                << diff.count() << "ms";
    }
    return;
  }

  isAttached_ = false;
  if (mode == WebCustomKeyboardState::FROM_ONBLUR) {
    isCloseFromOnblur_ = true;
  } else if (mode == WebCustomKeyboardState::FROM_ONPAUSE) {
    isCloseFromOnpause_ = true;
  }

  if (auto handler = nweb_handler_.lock()) {
    handler->OnCustomKeyboardClose();
  }
  lastCloseInputMethodTime_ = std::chrono::high_resolution_clock::now();
}

bool NWebCustomKeyboardHandlerImpl::AttachFromWebStateChange(
    WebCustomKeyboardState mode) {
  LOG(INFO) << "WebCustomKeyboard AttachFromWebStateChange, mode = "
            << static_cast<int>(mode);

  if (isAttached_) {
    LOG(INFO) << "WebCustomKeyboard AttachFromWebStateChange, custom keyboard "
                 "don't need reattch";
    return false;
  }

  bool isNeedReattach = false;
  if (mode == WebCustomKeyboardState::FROME_ONFOCUS && isCloseFromOnblur_) {
    isNeedReattach = true;
    isCloseFromOnblur_ = false;
  } else if (mode == WebCustomKeyboardState::FROME_ONCONTINUE &&
             isCloseFromOnpause_) {
    isNeedReattach = true;
    isCloseFromOnpause_ = false;
  }

  if (isNeedReattach) {
    isAttached_ = true;
    if (auto handler = nweb_handler_.lock()) {
      handler->OnCustomKeyboardAttach();
    }
  }

  return isNeedReattach;
}

bool NWebCustomKeyboardHandlerImpl::IsAttached() {
  return isAttached_;
}
}  // namespace OHOS::NWeb
