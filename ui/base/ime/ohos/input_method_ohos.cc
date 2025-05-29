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
#include "ui/display/display.h"
#include "ui/display/screen.h"
#include "ui/events/base_event_utils.h"
#include "ui/events/event.h"
#include "ui/events/keycodes/dom/dom_code.h"
#include "ui/events/keycodes/keyboard_codes_posix.h"
#include "ui/events/ozone/layout/keyboard_layout_engine.h"
#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"
#include "ui/events/types/event_type.h"

namespace ui {
const float kDefaultRatio = 1.0f;

InputMethodOHOS::InputMethodOHOS(
    ImeKeyEventDispatcher* ime_key_event_dispatcher)
    : InputMethodBase(ime_key_event_dispatcher),
      task_runner_(base::TaskRunnerOHOS::GetUIThreadTaskRunner()) {}

InputMethodOHOS::~InputMethodOHOS() {}

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
  } else {
    input_mode = ohos::adapter::IMFAdapterTextInputType::TEXT;
  }
  ohos::adapter::IMFAdapterInputAttribute inputAttribute = {
      static_cast<int32_t>(input_mode),
      static_cast<int32_t>(ohos::adapter::IMFAdapterEnterKeyType::DONE)};
  return inputAttribute;
}

void InputMethodOHOS::DetachTextInputTask() {
  auto& ime_instance = ohos::adapter::InputMethodOHOSAdapter::GetInstance();
  ime_instance.DetachTextInput();
  is_attach_ = false;
}

void InputMethodOHOS::AttachTextInputTask() {
  auto& ime_instance = ohos::adapter::InputMethodOHOSAdapter::GetInstance();
  ohos::adapter::IMFAdapterCursorInfo cursorInfo = GetCursorInfo();
  ohos::adapter::IMFAdapterTextConfig textConfig = {GetInputAttribute(),
                                                    cursorInfo};
  ime_instance.AttachTextInput(textConfig);
  ime_instance.RegisterInsertTextCallback(
      std::bind(&InputMethodOHOS::InsertText, this, std::placeholders::_1));
  ime_instance.RegisterDeleteBackwardCallback(
      std::bind(&InputMethodOHOS::DeleteBackward, this, std::placeholders::_1));
  ime_instance.RegisterDeleteForwardCallback(
      std::bind(&InputMethodOHOS::DeleteForward, this, std::placeholders::_1));
  ime_instance.RegisterSendEnterKeyEventCallback(
      std::bind(&InputMethodOHOS::SendEnterKeyEvent, this));
  ime_instance.RegisterMoveCursorCallback(
      std::bind(&InputMethodOHOS::MoveCursor, this, std::placeholders::_1));
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
    auto task = base::BindOnce(&InputMethodOHOS::DetachTextInputTask,
                               base::Unretained(this));
    sequenced_task_runner_->PostTask(FROM_HERE, std::move(task));
  } else if (old_text_input_type == TEXT_INPUT_TYPE_NONE &&
             text_input_type_ != TEXT_INPUT_TYPE_NONE &&
             GetTextInputClient() != nullptr) {
    auto task = base::BindOnce(&InputMethodOHOS::AttachTextInputTask,
                               base::Unretained(this));
    sequenced_task_runner_->PostTask(FROM_HERE, std::move(task));
  } else if (old_text_input_type != TEXT_INPUT_TYPE_NONE &&
             text_input_type_ != TEXT_INPUT_TYPE_NONE &&
             old_text_input_type != text_input_type_ &&
             GetTextInputClient() != nullptr) {
    auto task = base::BindOnce(&InputMethodOHOS::UpdateAttributeTask,
                               base::Unretained(this));
    sequenced_task_runner_->PostTask(FROM_HERE, std::move(task));
  }
}

void InputMethodOHOS::SetVirtualKeyboardVisibilityTask(bool should_show) {
  if (should_show && is_attach_) {
    ohos::adapter::InputMethodOHOSAdapter::GetInstance().ShowTextInput();
  }
  InputMethodBase::SetVirtualKeyboardVisibilityIfEnabled(should_show);
}

void InputMethodOHOS::SetVirtualKeyboardVisibilityIfEnabled(bool should_show) {
  auto task = base::BindOnce(&InputMethodOHOS::SetVirtualKeyboardVisibilityTask,
                             base::Unretained(this), should_show);
  sequenced_task_runner_->PostTask(FROM_HERE, std::move(task));
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

  auto task = base::BindOnce(
      [](const std::u16string& inputText, TextInputClient* client,
         InputMethodOHOS* input) {
        TRACE_EVENT0("ui", "InputMethodOHOS::InsertText");
        ui::KeyEvent key_down_event{
            EventType::kKeyPressed, ui::VKEY_PROCESSKEY, ui::DomCode::NONE,
            ui::EF_IS_SYNTHESIZED,  ui::DomKey::PROCESS, ui::EventTimeForNow()};

        input->DispatchKeyEvent(&key_down_event);
        client->InsertText(inputText,
                           ui::TextInputClient::InsertTextCursorBehavior::
                               kMoveCursorAfterText);

        ui::KeyEvent key_up_event{
            EventType::kKeyReleased, ui::VKEY_PROCESSKEY,
            ui::DomCode::NONE,       ui::EF_IS_SYNTHESIZED,
            ui::DomKey::PROCESS,     ui::EventTimeForNow()};
        input->DispatchKeyEvent(&key_up_event);
      },
      inputText, client, this);
  task_runner_->PostTask(FROM_HERE, std::move(task));
}

void InputMethodOHOS::DeleteBackward(int32_t length) {
  auto task = base::BindOnce(
      [](int32_t length, InputMethodOHOS* input) {
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
        ui::KeyEvent event(type, key_code, dom_Code, 0, dom_key,
                           EventTimeForNow());
        ui::KeyEvent event_release(type_release, key_code, dom_Code, 0, dom_key,
                                   EventTimeForNow());
        for (int32_t i = 0; i < length; i++) {
          input->DispatchKeyEvent(&event);
          input->DispatchKeyEvent(&event_release);
        }
      },
      length, this);
  task_runner_->PostTask(FROM_HERE, std::move(task));
}

void InputMethodOHOS::DeleteForward(int32_t length) {
  auto task = base::BindOnce(
      [](int32_t length, InputMethodOHOS* input) {
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
        ui::KeyEvent event(type, key_code, dom_Code, 0, dom_key,
                           EventTimeForNow());
        ui::KeyEvent event_release(type_release, key_code, dom_Code, 0, dom_key,
                                   EventTimeForNow());
        for (int32_t i = 0; i < length; i++) {
          input->DispatchKeyEvent(&event);
          input->DispatchKeyEvent(&event_release);
        }
      },
      length, this);
  task_runner_->PostTask(FROM_HERE, std::move(task));
}

void InputMethodOHOS::SendEnterKeyEvent() {
  auto task = base::BindOnce(
      [](InputMethodOHOS* input) {
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
        input->DispatchKeyEvent(&key_down_event);

        type = EventType::kKeyReleased;
        ui::KeyEvent key_up_event(type, key_code, dom_Code, 0, dom_key,
                                  EventTimeForNow());
        input->DispatchKeyEvent(&key_up_event);
      },
      this);
  task_runner_->PostTask(FROM_HERE, std::move(task));
}

void InputMethodOHOS::MoveCursor(int direction) {
  auto task = base::BindOnce(
      [](int direction, InputMethodOHOS* input) {
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
        input->DispatchKeyEvent(&event);
        ui::KeyEvent event_release(type_release, key_code, dom_Code, 0, dom_key,
                                   EventTimeForNow());
        input->DispatchKeyEvent(&event_release);
      },
      direction, this);
  task_runner_->PostTask(FROM_HERE, std::move(task));
}

float InputMethodOHOS::GetPixelRatio(const gfx::Rect& rect) {
  display::Screen* screen = display::Screen::GetScreen();
  if (!screen) {
    return kDefaultRatio;
  }
  display::Display display = screen->GetDisplayNearestPoint(rect.origin());
  return display.device_scale_factor();
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
                             base::Unretained(this), rect);
  sequenced_task_runner_->PostTask(FROM_HERE, std::move(task));
}

}  // namespace ui
                  