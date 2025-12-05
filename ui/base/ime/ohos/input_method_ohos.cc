// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/ime/ohos/input_method_ohos.h"

#include <cstdint>
#include <functional>

#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/trace_event/trace_event.h"
#include "content/public/browser/browser_thread.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"
#include "ui/base/ime/ohos/input_method_ohos_manager.h"
#include "ui/display/display.h"
#include "ui/display/screen.h"
#include "ui/display/screen_ohos.h"
#include "ui/events/base_event_utils.h"
#include "ui/events/event.h"
#include "ui/events/keycodes/dom/dom_code.h"
#include "ui/events/keycodes/keyboard_codes_posix.h"
#include "ui/events/ozone/layout/keyboard_layout_engine.h"
#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"
#include "ui/events/types/event_type.h"
#include "ui/gfx/geometry/transform.h"

namespace ui {
const float kDefaultRatio = 1.0f;
const gfx::PointF kInvalidPoint(-1.0f, -1.0f);
const int kAttachVirtualKeyboardDelay = 100;

InputMethodOHOS::InputMethodOHOS(
    ImeKeyEventDispatcher* ime_key_event_dispatcher,
    gfx::AcceleratedWidget widget)
    : InputMethodBase(ime_key_event_dispatcher), widget_id_(widget) {
  RegistKeyboardHeightEvent();
}

InputMethodOHOS::~InputMethodOHOS() {
  UnRegistKeyboardHeightEvent();
}

ui::EventDispatchDetails InputMethodOHOS::DispatchKeyEvent(
    ui::KeyEvent* event) {
  if (!GetTextInputClient()) {
    return DispatchKeyEventPostIME(event);
  }
  ui::EventDispatchDetails dispatch_details = DispatchKeyEventPostIME(event);
  if (!event->stopped_propagation() && !dispatch_details.dispatcher_destroyed &&
      event->type() == EventType::kKeyPressed && GetTextInputClient()) {
    const uint16_t ch = event->GetCharacter();
    if (ch) {
      GetTextInputClient()->InsertChar(*event);
      event->StopPropagation();
    }
  }
  return dispatch_details;
}

ohos::adapter::IMFAdapterInputAttribute InputMethodOHOS::GetInputAttribute() {
  int32_t input_mode = ohos::adapter::IMFAdapterTextInputType::NONE_INPUT;
  if (text_input_type_ == ui::TEXT_INPUT_TYPE_NUMBER ||
      text_input_type_ == ui::TEXT_INPUT_TYPE_TELEPHONE) {
    input_mode = ohos::adapter::IMFAdapterTextInputType::NUMBER;
  } else if (text_input_type_ == ui::TEXT_INPUT_TYPE_PASSWORD) {
    input_mode = ohos::adapter::IMFAdapterTextInputType::VISIBLE_PASSWORD;
  } else {
    input_mode = ohos::adapter::IMFAdapterTextInputType::TEXT;
  }
  ohos::adapter::IMFAdapterInputAttribute inputAttribute = {
      static_cast<int32_t>(input_mode),
      static_cast<int32_t>(ohos::adapter::IMFAdapterEnterKeyType::DONE)};
  return inputAttribute;
}

void SetSoftKeyboardForWidget(int32_t widget_id, bool need_soft_keyboard) {
  std::string platform_id =
      ohos::adapter::xcomponent::WindowAdapter::GetInstance().GetWindowId(
          widget_id);
  auto manager = ohos::adapter::xcomponent::XComponentManager::GetInstance();
  if (!manager) {
    LOG(ERROR) << "[XComponentManager] is nullptr";
    return;
  }

  auto impl = manager->GetXComponent(platform_id);
  if (!impl) {
    LOG(ERROR) << "manager [GetXComponent] return nullptr";
    return;
  }

  auto xcomponent = impl->GetComponent();
  if (!xcomponent) {
    LOG(ERROR) << "impl [GetComponent] return nullptr";
    return;
  }

  int32_t res =
      OH_NativeXComponent_SetNeedSoftKeyboard(xcomponent, need_soft_keyboard);
  if (res != 0) {
    LOG(ERROR) << "OH_NativeXComponent_SetNeedSoftKeyboard error return: "
               << res;
  }
}

void InputMethodOHOS::DetachTextInputTask() {
  if (InputMethodOHOSManager::GetInstance().ReleaseActiveInstance(
      weak_ptr_factory_.GetWeakPtr())) {
    auto& ime_instance = ohos::adapter::InputMethodOHOSAdapter::GetInstance();
    ime_instance.DetachTextInput();
    is_attach_ = false;
  }

  SetSoftKeyboardForWidget(widget_id_, false);
}

void InputMethodOHOS::AttachTextInputTask(ui::RequestKeyboardReason reason) {
  SetSoftKeyboardForWidget(widget_id_, true);
  ohos::adapter::IMFAdapterCursorInfo cursorInfo = GetCursorInfo();
  ohos::adapter::IMFAdapterTextConfig textConfig = {GetInputAttribute(), cursorInfo};
  ohos::adapter::InputMethodOHOSAdapter::GetInstance().AttachTextInput(textConfig, reason);
  InputMethodOHOSManager::GetInstance().SetActiveInstance(
      weak_ptr_factory_.GetWeakPtr());
  is_attach_ = true;
}
 
void InputMethodOHOS::UpdateAttributeTask() {
  auto& ime_instance = ohos::adapter::InputMethodOHOSAdapter::GetInstance();
  ime_instance.UpdateAttribute(GetInputAttribute());
}

void InputMethodOHOS::UpdateContextFocusState() {
  TextInputType old_text_input_type = text_input_type_;
  text_input_type_ = GetTextInputType();
  TRACE_EVENT2("ui", "UpdateContextFocusState", "old_type", old_text_input_type,
               "new_type", text_input_type_);
  if (old_text_input_type != TEXT_INPUT_TYPE_NONE &&
      text_input_type_ == TEXT_INPUT_TYPE_NONE) {
    delayed_attach_timer_.Stop();
    delayed_attach_timer_.Start(
        FROM_HERE, base::Milliseconds(kAttachVirtualKeyboardDelay),
        base::BindOnce(&InputMethodOHOS::DetachTextInputTask,
                       weak_ptr_factory_.GetWeakPtr()));
  } else if (text_input_type_ != TEXT_INPUT_TYPE_NONE) {
    if (GetTextInputClient() != nullptr) {
      ui::RequestKeyboardReason reason =
          GetTextInputClient()->GetRequestKeyboardReason();
      delayed_attach_timer_.Stop();
      delayed_attach_timer_.Start(
          FROM_HERE, base::Milliseconds(kAttachVirtualKeyboardDelay),
          base::BindOnce(&InputMethodOHOS::AttachTextInputTask,
                         weak_ptr_factory_.GetWeakPtr(), reason));
    } else {
      LOG(ERROR) << "[GetTextInputClient] is nullptr";
    }
  }
}

void InputMethodOHOS::SetVirtualKeyboardVisibilityTask(
    bool should_show, ui::RequestKeyboardReason reason) {
  if (should_show && is_attach_) {
    ohos::adapter::InputMethodOHOSAdapter::GetInstance().ShowTextInput(reason);
  }
  InputMethodBase::SetVirtualKeyboardVisibilityIfEnabled(should_show);
}

void InputMethodOHOS::SetVirtualKeyboardVisibilityIfEnabled(bool should_show) {
  ui::RequestKeyboardReason reason =
      ui::RequestKeyboardReason::REQUEST_KEYBOARD_REASON_NONE;
  if (GetTextInputClient()) {
    reason = GetTextInputClient()->GetRequestKeyboardReason();
  } else {
    LOG(ERROR) << "[InputMethodOHOS] SetVirtualKeyboardVisibilityTask "
                  "[GetTextInputClient] is nullptr";
  }

  auto task =
      base::BindOnce(&InputMethodOHOS::SetVirtualKeyboardVisibilityTask,
                     weak_ptr_factory_.GetWeakPtr(), should_show, reason);
  InputMethodOHOSManager::GetInstance().GetTaskRunner()->PostTask(
      FROM_HERE, std::move(task));
}

void InputMethodOHOS::OnDidChangeFocusedClient(TextInputClient* focused_before,
                                               TextInputClient* focused) {
  UpdateContextFocusState();
  if (text_input_type_ != ui::TEXT_INPUT_TYPE_NONE) {
    OnCaretBoundsChanged(GetTextInputClient());
  }
  InputMethodBase::OnDidChangeFocusedClient(focused_before, focused);
}

void InputMethodOHOS::CancelComposition(const TextInputClient* client) {}

void InputMethodOHOS::OnTextInputTypeChanged(TextInputClient* client) {
  UpdateContextFocusState();
  InputMethodBase::OnTextInputTypeChanged(client);
}

void InputMethodOHOS::OnCaretBoundsChanged(const TextInputClient* client) {
  if (!IsTextInputClientFocused(client)) {
    return;
  }
  NotifyTextInputCaretBoundsChanged(client);
  OnCursorUpdate(GetTextInputClient()->GetCaretBounds());
}

bool InputMethodOHOS::IsCandidatePopupOpen() const {
  return false;
}

void InputMethodOHOS::InsertText(const std::string& text) {
  TRACE_EVENT0("ui", "InputMethodOHOS::InsertText");
  if (text.empty()) {
    LOG(ERROR) << "[InputMethodOHOS] insert text is empty!";
    return;
  }
  std::u16string inputText = base::UTF8ToUTF16(text);
  auto client = GetTextInputClient();
  if (client == nullptr) {
    LOG(ERROR) << "[InputMethodOHOS] client ptr is nullptr";
    return;
  }

  if (GetTextInputType() == TEXT_INPUT_TYPE_NONE) {
    LOG(ERROR) << "[InputMethodOHOS] text input type is null";
    return;
  }

  ui::KeyEvent key_down_event{EventType::kKeyPressed,  ui::VKEY_PROCESSKEY,
                              ui::DomCode::NONE,   ui::EF_IS_SYNTHESIZED,
                              ui::DomKey::PROCESS, ui::EventTimeForNow()};
  DispatchKeyEvent(&key_down_event);

  client->InsertText(inputText,
                     ui::TextInputClient::InsertTextCursorBehavior::
                         kMoveCursorAfterText);

  ui::KeyEvent key_up_event{EventType::kKeyReleased, ui::VKEY_PROCESSKEY,
                            ui::DomCode::NONE,   ui::EF_IS_SYNTHESIZED,
                            ui::DomKey::PROCESS, ui::EventTimeForNow()};
  DispatchKeyEvent(&key_up_event);
}

bool InputMethodOHOS::IsDispatchedPressAndReleaseKeyEvents(
    int32_t length,
    KeyboardCode key_code,
    DomCode dom_Code) {
  DomKey dom_key;
  auto* layout_engine = KeyboardLayoutEngineManager::GetKeyboardLayoutEngine();
  if (layout_engine == nullptr ||
      !layout_engine->Lookup(dom_Code, 0, &dom_key, &key_code)) {
    return false;
  }

  EventType type = EventType::kKeyPressed;
  EventType type_release = EventType::kKeyReleased;
  for (int32_t i = 0; i < length; i++) {
    ui::KeyEvent event(type, key_code, dom_Code, 0, dom_key, EventTimeForNow());
    ui::KeyEvent event_release(type_release, key_code, dom_Code, 0, dom_key,
                               EventTimeForNow());
    DispatchKeyEvent(&event);
    DispatchKeyEvent(&event_release);
  }
  return true;
}

void InputMethodOHOS::DeleteBackward(int32_t length) {
  TRACE_EVENT0("ui", "InputMethodOHOS::DeleteBackward");
  if (!IsDispatchedPressAndReleaseKeyEvents(length, ui::VKEY_BACK,
                                            ui::DomCode::BACKSPACE)) {
    LOG(ERROR) << "[InputMethodOHOS] DeleteBackward failed to decode key_code";
  }
}

void InputMethodOHOS::DeleteForward(int32_t length) {
  TRACE_EVENT0("ui", "InputMethodOHOS::DeleteForward");
  if (!IsDispatchedPressAndReleaseKeyEvents(length, ui::VKEY_DELETE,
                                            ui::DomCode::DEL)) {
    LOG(ERROR) << "[InputMethodOHOS] DeleteForward failed to decode key_code";
  }
}

void InputMethodOHOS::SendEnterKeyEvent() {
  TRACE_EVENT0("ui", "InputMethodOHOS::SendEnterKeyEvent");
  EventType type = EventType::kKeyPressed;
  KeyboardCode key_code = ui::VKEY_RETURN;
  DomCode dom_Code = ui::DomCode::ENTER;
  DomKey dom_key;
  auto* layout_engine =
      KeyboardLayoutEngineManager::GetKeyboardLayoutEngine();
  if (layout_engine == nullptr ||
      !layout_engine->Lookup(dom_Code, 0, &dom_key, &key_code)) {
    LOG(ERROR) << "[InputMethodOHOS] SendEnterKeyEvent failed to decode "
                  "key_code";
    return;
  }

  ui::KeyEvent key_down_event(type, key_code, dom_Code, 0, dom_key,
                                    EventTimeForNow());
  DispatchKeyEvent(&key_down_event);

  type = EventType::kKeyReleased;
  ui::KeyEvent key_up_event(type, key_code, dom_Code, 0, dom_key,
                                  EventTimeForNow());
  DispatchKeyEvent(&key_up_event);
}

void InputMethodOHOS::ExitFullscreenEvent() {
  TRACE_EVENT0("ui", "InputMethodOHOS::ExitFullscreenEvent");
  EventType type = ui::EventType::kKeyPressed;
  KeyboardCode key_code = ui::VKEY_F11;
  DomCode dom_Code = ui::DomCode::F11;
  DomKey dom_key;
  auto* layout_engine =
      KeyboardLayoutEngineManager::GetKeyboardLayoutEngine();

  if (layout_engine == nullptr ||
      !layout_engine->Lookup(dom_Code, 0, &dom_key, &key_code)) {
    LOG(ERROR) << "[InputMethodOHOS] ExitFullscreenEvent failed to decode "
                  "key_code";
    return;
  }

  ui::KeyEvent key_down_event(type, key_code, dom_Code, 0, dom_key,
                              EventTimeForNow());
  DispatchKeyEvent(&key_down_event);

  type = ui::EventType::kKeyReleased;
  ui::KeyEvent key_up_event(type, key_code, dom_Code, 0, dom_key,
                            EventTimeForNow());
  DispatchKeyEvent(&key_up_event);
}

void InputMethodOHOS::MoveCursor(int direction) {
  TRACE_EVENT0("ui", "InputMethodOHOS::MoveCursor");
  EventType type = EventType::kKeyPressed;
  EventType type_release = EventType::kKeyReleased;
  KeyboardCode key_code;
  DomCode dom_Code;

  switch (direction) {
    case ohos::adapter::IMFAdapterDirection::UP: {
      key_code = ui::VKEY_UP;
      dom_Code = ui::DomCode::ARROW_UP;
      break;
    }
    case ohos::adapter::IMFAdapterDirection::LEFT: {
      key_code = ui::VKEY_LEFT;
      dom_Code = ui::DomCode::ARROW_LEFT;
      break;
    }
    case ohos::adapter::IMFAdapterDirection::RIGHT: {
      key_code = ui::VKEY_RIGHT;
      dom_Code = ui::DomCode::ARROW_RIGHT;
      break;
    }
    case ohos::adapter::IMFAdapterDirection::DOWN: {
      key_code = ui::VKEY_DOWN;
      dom_Code = ui::DomCode::ARROW_DOWN;
      break;
    }
    default: {
      LOG(ERROR) << "[InputMethodOHOS] MoveCursor unknown direction:"
                 << direction;
      return;
    }
  }

  DomKey dom_key;
  auto* layout_engine =
      KeyboardLayoutEngineManager::GetKeyboardLayoutEngine();

  if (layout_engine == nullptr ||
      !layout_engine->Lookup(dom_Code, 0, &dom_key, &key_code)) {
    LOG(ERROR) << "[InputMethodOHOS] SendEnterKeyEvent failed to decode "
                  "key_code";
    return;
  }

  ui::KeyEvent event(type, key_code, dom_Code, 0, dom_key,
                     EventTimeForNow());
  DispatchKeyEvent(&event);

  ui::KeyEvent event_release(type_release, key_code, dom_Code, 0, dom_key,
                     EventTimeForNow());
  DispatchKeyEvent(&event_release);
}

ohos::adapter::IMFAdapterCursorInfo InputMethodOHOS::GetCursorInfo() {
  display::Display current_display;
  if (GetTextInputClient()) {
    current_display = GetTextInputClient()->GetDisplayForClient();
  } else {
    current_display = display::Screen::GetScreen()->GetPrimaryDisplay();
  }
  gfx::Rect rect_pixel = display::ohos::ScreenOhos::ConvertDipToPixel(
      current_display, focus_rect_);
  if (rect_pixel.width() == 0 && rect_pixel.height() == 0) {
    LOG(ERROR) << "[InputMethodOHOS] " << __FUNCTION__
                 << ", The width and height of rect_pixel are both 0";
    rect_pixel = focus_rect_;
  }
  ohos::adapter::IMFAdapterCursorInfo cursorInfo{.left = rect_pixel.x(),
                                                 .top = rect_pixel.y(),
                                                 .width = rect_pixel.width(),
                                                 .height = rect_pixel.height()};
  return cursorInfo;
}

void InputMethodOHOS::UpdateCursorTask(const gfx::Rect& rect) {
  // Do not notify cursor update if cursor rect unchanged or cursor rect is 0000
  // which means input method will be detached
  if (focus_rect_.ApproximatelyEqual(rect, 0) ||
      rect == gfx::Rect(0, 0, 0, 0)) {
    return;
  }
  focus_rect_ = rect;
  ohos::adapter::InputMethodOHOSAdapter::GetInstance().NotifyCursorUpdate(
      GetCursorInfo());
}

void InputMethodOHOS::OnCursorUpdate(const gfx::Rect& rect) {
  auto task = base::BindOnce(&InputMethodOHOS::UpdateCursorTask,
      weak_ptr_factory_.GetWeakPtr(), rect);
  InputMethodOHOSManager::GetInstance().GetTaskRunner()->PostTask(
      FROM_HERE, std::move(task));
}

gfx::AcceleratedWidget InputMethodOHOS::GetWidgetId() const {
  return widget_id_;
}

void InputMethodOHOS::RegistKeyboardHeightEvent() {
  auto weak_this = weak_ptr_factory_.GetWeakPtr();
  ohos::adapter::xcomponent::WindowAdapter::GetInstance()
      .RegistKeyboardHeightEvent(
      GetWidgetId(), [weak_this](gfx::AcceleratedWidget widget_id,
                                 int32_t keyboard_height) {
            // Handle arkui keyboard height events on the browser UI thread
            auto task =
                base::BindOnce(&InputMethodOHOS::SetVirtualKeyboardBoundsTask,
                               weak_this, keyboard_height);
            InputMethodOHOSManager::GetInstance().GetTaskRunner()->PostTask(
                FROM_HERE, std::move(task));
          });
}

void InputMethodOHOS::SetVirtualKeyboardBoundsTask(int32_t keyboard_height) {
  if (!GetTextInputClient()) {
    LOG(ERROR) << "[InputMethodOHOS::SetVirtualKeyboardBoundsTask] "
                  "GetTextInputClient is nullptr";
    return;
  }

  gfx::Rect window_bounds = GetTextInputClient()->GetToplevelWindowBounds();
  if (window_bounds.IsEmpty()) {
    LOG(ERROR) << "[InputMethodOHOS::SetVirtualKeyboardBoundsTask] "
                  "Window bounds retrieval failed: null/empty result.";
    return;
  }
  display::Display current_display;
  if (GetTextInputClient()) {
    current_display = GetTextInputClient()->GetDisplayForClient();
  } else {
    current_display = display::Screen::GetScreen()->GetPrimaryDisplay();
  }
  gfx::PointF transformed_point = display::ohos::ScreenOhos::ConvertPixelToDip(
      current_display, gfx::PointF(0, keyboard_height));
  if (transformed_point == kInvalidPoint) {
    return;
  }

  float logical_keyboard_height = transformed_point.y();

  gfx::Rect keyboard_rect;
  if (logical_keyboard_height > 0) {
    keyboard_rect = gfx::Rect(
        window_bounds.x(),
        window_bounds.y() + window_bounds.height() - logical_keyboard_height,
        window_bounds.width(), logical_keyboard_height);
  }
  LOG(INFO) << "[InputMethodOHOS::SetVirtualKeyboardBoundsTask] "
               "keyboard_rect positionX: "
            << keyboard_rect.x() << " positionY: " << keyboard_rect.y()
            << " keyboardWidth: " << keyboard_rect.width()
            << " keyboardHeight: " << keyboard_rect.height();

  InputMethodBase::SetVirtualKeyboardBounds(keyboard_rect);
}

void InputMethodOHOS::UnRegistKeyboardHeightEvent() {
  ohos::adapter::xcomponent::WindowAdapter::GetInstance()
      .UnRegistKeyboardHeightEvent(widget_id_);
}

}  // namespace ui