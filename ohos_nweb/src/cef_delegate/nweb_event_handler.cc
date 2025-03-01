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

#include "nweb_event_handler.h"

#include <cmath>
#include <vector>

#include "cef/include/base/cef_logging.h"
#include "cef/include/base/cef_macros.h"
#include "cef/include/internal/cef_types.h"
#include "cef/include/internal/cef_types_wrappers.h"

#include "base/trace_event/common/trace_event_common.h"
#include "base/trace_event/trace_event.h"

#include "ui/events/keycodes/keyboard_code_conversion_x.h"
#include "ui/events/keycodes/keysym_to_unicode.h"

namespace OHOS::NWeb {

// static
std::shared_ptr<NWebEventHandler> NWebEventHandler::Create() {
  auto event_handler = std::make_shared<NWebEventHandler>();
  if (event_handler == nullptr) {
    LOG(ERROR) << "fail to create NWebEventHandler instance";
    return nullptr;
  }
  return event_handler;
}

NWebEventHandler::NWebEventHandler() {
#if defined(OHOS_INPUT_EVENTS)
  mmi_adapter_ =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateMMIAdapter();
  if (mmi_adapter_ == nullptr) {
    LOG(ERROR) << "display_manager_adapter is nullptr";
    return;
  }
#endif  // defined(OHOS_INPUT_EVENTS)
}

void NWebEventHandler::OnDestroy() {
#if defined(OHOS_INPUT_EVENTS)
  if (mmi_id_ > 0 && mmi_adapter_ != nullptr) {
    mmi_adapter_->UnregisterMMIInputListener(mmi_id_);
  }
#endif  // defined(OHOS_INPUT_EVENTS)
  browser_ = nullptr;
}

void NWebEventHandler::SetBrowser(CefRefPtr<CefBrowser> browser) {
  browser_ = browser;
}

void NWebEventHandler::OnTouchPress(int32_t id,
                                    double x,
                                    double y,
                                    bool from_overlay) {
#if defined(OHOS_SLIDE) || defined(OHOS_PER_DFX)
  TRACE_EVENT0("input",
               "NWebEventHandler::OnTouchPress sliding response begin");
  LOG(DEBUG) << "NWebEventHandler::OnTouchPress sliding response begin. id = "
             << id << ", x = " << x << ", y = " << y
             << ", from_overlay = " << from_overlay;
#endif
  CefTouchEvent touch_pressed;
  touch_pressed.type = CEF_TET_PRESSED;
  touch_pressed.pointer_type = CEF_POINTER_TYPE_TOUCH;
  touch_pressed.id = id;
  touch_pressed.x = x;
  touch_pressed.y = y;
  touch_pressed.modifiers = EVENTFLAG_NONE;
  touch_pressed.from_overlay = from_overlay;
  if (browser_ && browser_->GetHost()) {
    browser_->GetHost()->SendTouchEvent(touch_pressed);
  }
}

void NWebEventHandler::OnTouchMove(int32_t id,
                                   double x,
                                   double y,
                                   bool from_overlay) {
  LOG(DEBUG) << "NWebEventHandler::OnTouchMove id = " << id << ", x = " << x
             << ", y = " << y << ", from_overlay = " << from_overlay;
  CefTouchEvent touch_move;
  touch_move.type = CEF_TET_MOVED;
  touch_move.pointer_type = CEF_POINTER_TYPE_TOUCH;
  touch_move.id = id;
  touch_move.x = x;
  touch_move.y = y;
  touch_move.modifiers = EVENTFLAG_NONE;
  touch_move.from_overlay = from_overlay;
  if (browser_ && browser_->GetHost()) {
    browser_->GetHost()->SendTouchEvent(touch_move);
  }
}

void NWebEventHandler::OnTouchMove(const std::vector<std::shared_ptr<NWebTouchPointInfo>> &touch_point_infos,
                                   bool from_overlay, float virtual_pixel_ratio) {
  std::vector<CefTouchEvent> event_list {};

  std::string touch_point_info_str {"NWebEventHandler::OnTouchMove"};
  for (const auto& touch_point : touch_point_infos) {
    CefTouchEvent touch_move;
    touch_move.type = CEF_TET_MOVED;
    touch_move.pointer_type = CEF_POINTER_TYPE_TOUCH;
    touch_move.id = touch_point->GetId();
    touch_move.x = touch_point->GetX() / virtual_pixel_ratio;
    touch_move.y = touch_point->GetY() / virtual_pixel_ratio;
    touch_move.modifiers = EVENTFLAG_NONE;
    touch_move.from_overlay = from_overlay;
    touch_point_info_str += " id = " + std::to_string(touch_point->GetId()) + ", x = " +
        std::to_string(touch_move.x) + ", y = " + std::to_string(touch_move.y);
    event_list.emplace_back(touch_move);
  }
  LOG(DEBUG) << touch_point_info_str << ", from_overlay = " << from_overlay;

  if (browser_ && browser_->GetHost()) {
    browser_->GetHost()->SendTouchEventList(event_list);
  }
}

void NWebEventHandler::OnTouchRelease(int32_t id,
                                      double x,
                                      double y,
                                      bool from_overlay) {
  LOG(DEBUG) << "NWebEventHandler::OnTouchRelease";
  CefTouchEvent touch_end;
  touch_end.type = CEF_TET_RELEASED;
  touch_end.pointer_type = CEF_POINTER_TYPE_TOUCH;
  touch_end.id = id;
  touch_end.x = x;
  touch_end.y = y;
  touch_end.modifiers = EVENTFLAG_NONE;
  touch_end.from_overlay = from_overlay;
  if (browser_ && browser_->GetHost()) {
    browser_->GetHost()->SendTouchEvent(touch_end);
  }
}

void NWebEventHandler::OnTouchCancel() {
  LOG(DEBUG) << "NWebEventHandler::OnTouchCancel";
  CefTouchEvent touch_cancelled;
  touch_cancelled.type = CEF_TET_CANCELLED;
  if (browser_ && browser_->GetHost()) {
    browser_->GetHost()->SendTouchEvent(touch_cancelled);
  }
}

void NWebEventHandler::OnTouchCancelById(int32_t id,
                                         double x,
                                         double y,
                                         bool from_overlay) {
  (void)from_overlay;
  CefTouchEvent touch_cancelled;
  touch_cancelled.type = CEF_TET_CANCELLED;
  touch_cancelled.pointer_type = CEF_POINTER_TYPE_TOUCH;
  touch_cancelled.id = id;
  touch_cancelled.x = x;
  touch_cancelled.y = y;
  if (browser_ && browser_->GetHost()) {
    browser_->GetHost()->SendTouchEvent(touch_cancelled);
  }
}

#if defined(OHOS_INPUT_EVENTS)
void NWebEventHandler::SendKeyEventFromMMI(int32_t keyCode, int32_t keyAction) {
  if (!isFocus_ || !NWebInputDelegate::IsMMIKeyEvent(keyCode)) {
    return;
  }
  LOG(DEBUG) << "SendKeyEventFromMMI keyCode = " << keyCode
             << " keyAction = " << keyAction;
  SendKeyEvent(keyCode, keyAction);
}

bool NWebEventHandler::SendKeyEventFromAce(int32_t keyCode, int32_t keyAction) {
  if (mmi_id_ >= 0 && NWebInputDelegate::IsMMIKeyEvent(keyCode)) {
    return true;
  }
  LOG(DEBUG) << "SendKeyEventFromAce keyCode = " << keyCode
             << " keyAction = " << keyAction;
  return SendKeyEvent(keyCode, keyAction);
}

bool NWebEventHandler::WebSendKeyEventFromAce(int32_t keyCode, int32_t keyAction,
                                              const std::vector<int32_t>& pressedCodes) {
  if (mmi_id_ >= 0 && NWebInputDelegate::IsMMIKeyEvent(keyCode)) {
    return true;
  }
  LOG(DEBUG) << "WebSendKeyEventFromAce keyCode = " << keyCode
             << " keyAction = " << keyAction;
  return WebSendKeyEvent(keyCode, keyAction, pressedCodes);
}

bool NWebEventHandler::CreateCefKeyEvent(CefKeyEvent& keyEvent,
                                         int32_t keyCode,
                                         int32_t keyAction,
                                         int32_t modifiers) {
  keyEvent.windows_key_code =
      NWebInputDelegate::CefConverter("keycode", keyCode);
  if (keyEvent.windows_key_code == -1) {
    LOG(ERROR) << "WebSendKeyEvent keyCode conversion failed";
    return false;
  }
  keyEvent.type = static_cast<cef_key_event_type_t>(
      NWebInputDelegate::CefConverter("keyaction", keyAction));
  keyEvent.modifiers = modifiers;
  LOG(DEBUG) << "WebSendKeyEvent modifiers = " << keyEvent.modifiers;
  keyEvent.is_system_key = false;
  keyEvent.native_key_code =
      NWebInputDelegate::CefConverter("keyscancode", keyCode);
  ui::KeyboardCode key_code =
      static_cast<ui::KeyboardCode>(keyEvent.windows_key_code);
  int keysym = ui::XKeysymForWindowsKeyCode(
      key_code, keyEvent.modifiers & EVENTFLAG_SHIFT_DOWN);
  char16_t character = ui::GetUnicodeCharacterFromXKeySym(keysym);
  keyEvent.character = keyEvent.unmodified_character = character;
  return true;
}

void NWebEventHandler::SendCefKeyEvent(CefKeyEvent& keyEvent) {
  if (browser_ && browser_->GetHost()) {
    browser_->GetHost()->SendKeyEvent(keyEvent);
  }

  if (keyEvent.type == KEYEVENT_RAWKEYDOWN) {
    keyEvent.type = KEYEVENT_CHAR;
    if (browser_ && browser_->GetHost()) {
      browser_->GetHost()->SendKeyEvent(keyEvent);
    }
  }
}

bool NWebEventHandler::WebSendKeyEvent(int32_t keyCode, int32_t keyAction,
                                       const std::vector<int32_t>& pressedCodes) {
  LOG(DEBUG) << "WebSendKeyEvent keyCode = " << keyCode
             << " keyAction = " << keyAction;
  if (keyCode < 0) {
    LOG(ERROR) << "WebSendKeyEvent obtaining invalid keyCode";
    return false;
  }
  CefKeyEvent keyEvent;
  int32_t modifiers = NWebInputDelegate::GetWebModifiers(keyCode, keyAction, pressedCodes);
  input_delegate_.SetModifiers(keyCode, keyAction);
  if (!CreateCefKeyEvent(keyEvent, keyCode, keyAction, modifiers)) {
    return false;
  }
  SendCefKeyEvent(keyEvent);
  return true;
}
void NWebEventHandler::SendCefMouseWheelEvent(double x,
                                              double y,
                                              double deltaX,
                                              double deltaY,
                                              int32_t modifiers,
                                              int32_t source) {
  CefMouseEvent mouseEvent;
  mouseEvent.x = x;
  mouseEvent.y = y;
  mouseEvent.modifiers = modifiers;
  mouseEvent.source = source;
  LOG(DEBUG) << "WebSendMouseWheelEvent modifiers = " << mouseEvent.modifiers;
  LOG(DEBUG) << "WebSendMouseWheelEventV2 source = " << mouseEvent.source;
  if (!browser_ || !browser_->GetHost()) {
    return;
  }

  double horizontalDelta;
  double verticalDelta;
  if (mmi_id_ > 0 && (mouseEvent.modifiers & EVENTFLAG_SHIFT_DOWN)) {
    horizontalDelta = deltaY * input_delegate_.GetMouseWheelRatio();
    verticalDelta = deltaX * input_delegate_.GetMouseWheelRatio();
  } else {
    horizontalDelta = deltaX * input_delegate_.GetMouseWheelRatio();
    verticalDelta = deltaY * input_delegate_.GetMouseWheelRatio();
  }
  browser_->GetHost()->SendMouseWheelEvent(mouseEvent, horizontalDelta,
                                           verticalDelta);
}

void NWebEventHandler::WebSendMouseWheelEvent(double x,
                                              double y,
                                              double deltaX,
                                              double deltaY,
                                              const std::vector<int32_t>& pressedCodes) {
  int32_t modifiers = NWebInputDelegate::GetWebModifiersByPressedCode(pressedCodes);
  SendCefMouseWheelEvent(x, y, deltaX, deltaY, modifiers);
}

void NWebEventHandler::WebSendMouseWheelEventV2(double x,
                                              double y,
                                              double deltaX,
                                              double deltaY,
                                              const std::vector<int32_t>& pressedCodes,
                                              int32_t source) {
  int32_t modifiers = NWebInputDelegate::GetWebModifiersByPressedCode(pressedCodes);
  SendCefMouseWheelEvent(x, y, deltaX, deltaY, modifiers, source);
}

void NWebEventHandler::WebSendTouchpadFlingEvent(double x,
                                                 double y,
                                                 double vx,
                                                 double vy,
                                                 const std::vector<int32_t>& pressedCodes) {
  CefMouseEvent mouseEvent;
  mouseEvent.x = x;
  mouseEvent.y = y;
  mouseEvent.modifiers = NWebInputDelegate::GetWebModifiersByPressedCode(pressedCodes);
  LOG(DEBUG) << "WebSendTouchpadFlingEvent modifiers = " << mouseEvent.modifiers;
  if (!browser_ || !browser_->GetHost()) {
    return;
  }

  browser_->GetHost()->SendTouchpadFlingEvent(mouseEvent, vx, vy);
}

bool NWebEventHandler::SendKeyboardEvent(
    const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>& keyboardEvent) {
  if (!keyboardEvent) {
    return false;
  }
  if (keyboardEvent->GetKeyCode() < 0) {
    LOG(ERROR) << "SendKeyboardEvent obtaining invalid keyCode";
    return false;
  }
  CefKeyEvent keyEvent;
  int32_t modifiers =  NWebInputDelegate::GetModifiersByKeyEvent(keyboardEvent);
  if (!CreateCefKeyEvent(keyEvent,
                        keyboardEvent->GetKeyCode(),
                        keyboardEvent->GetAction(),
                        modifiers)) {
    return false;
  }
  SendCefKeyEvent(keyEvent);
  return true;
}
#endif  // defined(OHOS_INPUT_EVENTS)

bool NWebEventHandler::SendKeyEvent(int32_t keyCode, int32_t keyAction) {
  LOG(DEBUG) << "SendKeyEvent keyCode = " << keyCode
             << " keyAction = " << keyAction;
  if (keyCode < 0) {
    LOG(ERROR) << "SendKeyEvent obtaining invalid keyCode";
    return false;
  }
  CefKeyEvent keyEvent;
  input_delegate_.SetModifiers(keyCode, keyAction);
  int32_t modifiers = input_delegate_.GetModifiers();
  if (!CreateCefKeyEvent(keyEvent, keyCode, keyAction, modifiers)) {
    return false;
  }
  SendCefKeyEvent(keyEvent);
  return true;
}

void NWebEventHandler::SendTouchpadFlingEvent(double x, double y, double vx, double vy) {
  CefMouseEvent mouseEvent;
  mouseEvent.x = x;
  mouseEvent.y = y;
  mouseEvent.modifiers = input_delegate_.GetModifiers();
 
#if defined(OHOS_INPUT_EVENTS)
  if (!browser_ || !browser_->GetHost()) {
    return;
  }
  browser_->GetHost()->SendTouchpadFlingEvent(mouseEvent, vx, vy);
#endif  // defined(OHOS_INPUT_EVENTS)
}

void NWebEventHandler::SendMouseWheelEvent(double x,
                                           double y,
                                           double deltaX,
                                           double deltaY) {
#if defined(OHOS_INPUT_EVENTS)
  int32_t modifiers = input_delegate_.GetModifiers();
  SendCefMouseWheelEvent(x, y, deltaX, deltaY, modifiers);
#endif  // defined(OHOS_INPUT_EVENTS)
}

void NWebEventHandler::SendMouseEvent(int x,
                                      int y,
                                      int button,
                                      int action,
                                      int count) {
  CefMouseEvent mouseEvent;
  mouseEvent.x = x;
  mouseEvent.y = y;
#ifdef OHOS_EX_TOPCONTROLS
  if (browser_ && browser_->GetHost()) {
    mouseEvent.y -= browser_->GetHost()->GetShrinkViewportHeight();
  }
#endif
  cef_mouse_button_type_t buttonType = static_cast<cef_mouse_button_type_t>(
      NWebInputDelegate::CefConverter("mousebutton", button));
  mouseEvent.modifiers = input_delegate_.GetModifiers(buttonType);
  if (NWebInputDelegate::IsMouseLeave(action)) {
    is_in_web_ = false;
  } else if (NWebInputDelegate::IsMouseEnter(action)) {
    is_in_web_ = true;
  }
  if (browser_ && browser_->GetHost()) {
    if (NWebInputDelegate::IsMouseDown(action)) {
      previous_action_ = action;
      previous_button_ = buttonType;
#ifdef OHOS_CLIPBOARD
      if (buttonType == MBT_LEFT) {
        browser_->GetHost()->SetFocus(true);
      }
#endif  // #ifdef OHOS_CLIPBOARD
      browser_->GetHost()->SendMouseClickEvent(mouseEvent, buttonType, false,
                                               count);
    } else if (NWebInputDelegate::IsMouseUp(action)) {
       previous_action_ = action;
       previous_button_ = buttonType;
      browser_->GetHost()->SendMouseClickEvent(mouseEvent, buttonType, true, 1);
      if (!is_in_web_) {
        browser_->GetHost()->SendMouseMoveEvent(mouseEvent, true);
      }
    } else if (NWebInputDelegate::IsMouseMove(action)) {
      if (last_mouse_x_ == x && last_mouse_y_ == y) {
        LOG(DEBUG) << "no change in coordinates, cancel mouse move event";
        return;
      }

      last_mouse_x_ = x;
      last_mouse_y_ = y;
      browser_->GetHost()->SendMouseMoveEvent(mouseEvent, false);
    } else if (NWebInputDelegate::IsMouseLeave(action)) {
      if (previous_button_ == MBT_LEFT || previous_button_ == MBT_RIGHT) {
        browser_->GetHost()->SendMouseMoveEvent(mouseEvent, true);
      }
    } else {
      previous_action_ = action;
      LOG(DEBUG) << "mouse event action: " << action;
    }
  }
}

void NWebEventHandler::NotifyForNextTouchEvent() {
  if (browser_ && browser_->GetHost()) {
    browser_->GetHost()->NotifyForNextTouchEvent();
  }
}
}  // namespace OHOS::NWeb
