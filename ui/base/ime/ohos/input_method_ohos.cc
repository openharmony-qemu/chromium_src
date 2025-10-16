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
#include "ui/base/ime/ohos/input_method_ohos_manager.h"
#include "ui/display/display.h"
#include "ui/display/screen.h"
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

void InputMethodOHOS::DetachTextInputTask() {
  if (InputMethodOHOSManager::GetInstance().ReleaseActiveInstance(
      weak_ptr_factory_.GetWeakPtr())) {
    auto& ime_instance = ohos::adapter::InputMethodOHOSAdapter::GetInstance();
    ime_instance.DetachTextInput();
    is_attach_ = false;
  }
}

void InputMethodOHOS::AttachTextInputTask(ui::RequestKeyboardReason reason) {
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
  TRACE_EVENT2("ui", "UpdateContextFocusState",
               "old_type", old_text_input_type, "new_type", text_input_type_);
  if (old_text_input_type != TEXT_INPUT_TYPE_NONE &&
      text_input_type_ == TEXT_INPUT_TYPE_NONE) {
    auto task = base::BindOnce(&InputMethodOHOS::DetachTextInputTask,
        weak_ptr_factory_.GetWeakPtr());
    InputMethodOHOSManager::GetInstance().GetTaskRunner()->PostTask(
        FROM_HERE, std::move(task));
  } else if (text_input_type_ != TEXT_INPUT_TYPE_NONE) {
    if (GetTextInputClient() != nullptr) {
      ui::RequestKeyboardReason reason =
          GetTextInputClient()->GetRequestKeyboardReason();
      auto task = base::BindOnce(&InputMethodOHOS::AttachTextInputTask,
          weak_ptr_factory_.GetWeakPtr(), reason);
      InputMethodOHOSManager::GetInstance().GetTaskRunner()->PostTask(
          FROM_HERE, std::move(task));
    } else {
      LOG(ERROR) << "[GetTextInputClient] is nullptr";
    }
  }
}

void InputMethodOHOS::SetVirtualKeyboardVisibilityTask(bool should_show) {
  if (should_show && is_attach_) {
    if (GetTextInputClient()) {
      ui::RequestKeyboardReason request_keyboard_reason =
          GetTextInputClient()->GetRequestKeyboardReason();
      ohos::adapter::InputMethodOHOSAdapter::GetInstance().ShowTextInput(
          request_keyboard_reason);
    } else {
      LOG(ERROR) << "[InputMethodOHOS] SetVirtualKeyboardVisibilityTask "
                    "[GetTextInputClient] is nullptr";
    }
  }
  InputMethodBase::SetVirtualKeyboardVisibilityIfEnabled(should_show);
}

void InputMethodOHOS::SetVirtualKeyboardVisibilityIfEnabled(bool should_show) {
  auto task = base::BindOnce(&InputMethodOHOS::SetVirtualKeyboardVisibilityTask,
      weak_ptr_factory_.GetWeakPtr(), should_show);
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

void InputMethodOHOS::DeleteBackward(int32_t length) {
  TRACE_EVENT0("ui", "InputMethodOHOS::DeleteBackward");
  EventType type = EventType::kKeyPressed;
  EventType type_release = EventType::kKeyReleased;
  KeyboardCode key_code = ui::VKEY_BACK;
  DomCode dom_Code = ui::DomCode::BACKSPACE;
  DomKey dom_key;
  auto* layout_engine =
      KeyboardLayoutEngineManager::GetKeyboardLayoutEngine();
  
  if (layout_engine == nullptr ||
      !layout_engine->Lookup(dom_Code, 0, &dom_key, &key_code)) {
    LOG(ERROR)
        << "[InputMethodOHOS] DeleteBackward failed to decode key_code";
    return;
  }

  for (int32_t i = 0; i < length; i++) {
    ui::KeyEvent event(type, key_code, dom_Code, 0, dom_key,
                       EventTimeForNow());
    ui::KeyEvent event_release(type_release, key_code, dom_Code, 0, dom_key,
                       EventTimeForNow());
    DispatchKeyEvent(&event);
    DispatchKeyEvent(&event_release);
  }
}

void InputMethodOHOS::DeleteForward(int32_t length) {
  TRACE_EVENT0("ui", "InputMethodOHOS::DeleteForward");
  EventType type = EventType::kKeyPressed;
  EventType type_release = EventType::kKeyReleased;
  KeyboardCode key_code = ui::VKEY_DELETE;
  DomCode dom_Code = ui::DomCode::DEL;
  DomKey dom_key;
  auto* layout_engine =
      KeyboardLayoutEngineManager::GetKeyboardLayoutEngine();

  if (layout_engine == nullptr ||
      !layout_engine->Lookup(dom_Code, 0, &dom_key, &key_code)) {
    LOG(ERROR)
        << "[InputMethodOHOS] DeleteForward failed to decode key_code";
    return;
  }

  for (int32_t i = 0; i < length; i++) {
    ui::KeyEvent event(type, key_code, dom_Code, 0, dom_key,
                       EventTimeForNow());
    ui::KeyEvent event_release(type_release, key_code, dom_Code, 0, dom_key,
                       EventTimeForNow());
    DispatchKeyEvent(&event);
    DispatchKeyEvent(&event_release);
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

float InputMethodOHOS::GetPixelRatio(const gfx::Rect& rect) {
  display::Screen* screen = display::Screen::GetScreen();
  if (!screen) {
    return kDefaultRatio;
  }
  display::Display display = screen->GetDisplayNearestPoint(rect.origin());
  return display.device_scale_factor();
}

gfx::PointF InputMethodOHOS::GetLogicalPointF(const gfx::Point& point) {
  float device_scale_factor =
      display::Screen::GetScreen()->GetPrimaryDisplay().device_scale_factor();
  gfx::PointF point_f(point);
  gfx::Transform trans;
  trans.PostScale(device_scale_factor, device_scale_factor);
  std::optional<gfx::PointF> transformed_point =
      trans.InverseMapPoint(point_f);
  if (!transformed_point) {
    LOG(ERROR) << "[InputMethodOHOS::GetLogicalPointF] Failed to invert "
                  "transform for point: ("
               << point_f.x() << ", " << point_f.y()
               << "). Using kInvalidPoint (-1.0f, -1.0f).";
    return kInvalidPoint;
  }
  return transformed_point.value();
}

ohos::adapter::IMFAdapterCursorInfo InputMethodOHOS::GetCursorInfo() {
  float device_scale_factor = GetPixelRatio(focus_rect_);
  ohos::adapter::IMFAdapterCursorInfo cursorInfo{
      .left = focus_rect_.x() * device_scale_factor,
      .top = focus_rect_.y() * device_scale_factor,
      .width = focus_rect_.width() * device_scale_factor,
      .height = focus_rect_.height() * device_scale_factor};
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

  gfx::PointF transformed_point =
      GetLogicalPointF(gfx::Point(0, keyboard_height));
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