// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/ime/ohos/input_method_ohos.h"

#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "base/logging.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/synchronization/lock.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "content/public/browser/browser_thread.h"
#include "deviceinfo.h"
#include "ohos/adapter/device_info/device_info.h"
#include "ohos/adapter/ime_adapter/native_apis.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ohos/adapter/xcomponent/xcomponent_manager.h"
#include "ui/accessibility/accessibility_features.h"
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
    : InputMethodBase(ime_key_event_dispatcher), widget_id_(widget) {}

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

void SetSoftKeyboardForWidget(int32_t widget_id, bool need_soft_keyboard) {
  std::string platform_id =
      ohos::adapter::xcomponent::WindowAdapter::GetInstance().GetWindowId(
          widget_id);
  auto manager = ohos::adapter::xcomponent::XComponentManager::GetInstance();
  if (!manager) {
    LOG(ERROR) << "[InputMethodOHOS] " << __FUNCTION__ << " manager is nullptr";
    return;
  }

  auto impl = manager->GetXComponent(platform_id);
  if (!impl) {
    LOG(ERROR) << "[InputMethodOHOS] " << __FUNCTION__
               << " GetXComponent return nullptr";
    return;
  }

  auto xcomponent = impl->GetComponent();
  if (!xcomponent) {
    LOG(ERROR) << "[InputMethodOHOS] " << __FUNCTION__
               << " GetComponent return nullptr";
    return;
  }

  int32_t res =
      OH_NativeXComponent_SetNeedSoftKeyboard(xcomponent, need_soft_keyboard);
  if (res != 0) {
    LOG(ERROR) << "[InputMethodOHOS] " << __FUNCTION__
               << " OH_NativeXComponent_SetNeedSoftKeyboard error return: "
               << res;
  }
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
        base::BindOnce(&InputMethodOHOS::DetachTextInputTask, GetWeakPtr()));
  } else if (text_input_type_ != TEXT_INPUT_TYPE_NONE) {
    if (GetTextInputClient() != nullptr) {
      ui::RequestKeyboardReason reason =
          GetTextInputClient()->GetRequestKeyboardReason();
      delayed_attach_timer_.Stop();
      delayed_attach_timer_.Start(
          FROM_HERE, base::Milliseconds(kAttachVirtualKeyboardDelay),
          base::BindOnce(&InputMethodOHOS::AttachTextInputTask, GetWeakPtr(),
                         reason));
    } else {
      LOG(ERROR) << "[GetTextInputClient] is nullptr";
    }
  }
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

  auto task = base::BindOnce(&InputMethodOHOS::SetVirtualKeyboardVisibilityTask,
                             GetWeakPtr(), should_show, reason);
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

void InputMethodOHOS::InsertU16Text(const std::u16string& inputText) {
  auto client = GetTextInputClient();
  if (client == nullptr) {
    LOG(ERROR) << "[InputMethodOHOS] client ptr is nullptr";
    return;
  }

  if (GetTextInputType() == TEXT_INPUT_TYPE_NONE) {
    LOG(ERROR) << "[InputMethodOHOS] text input type is null";
    return;
  }

  ui::KeyEvent key_down_event{EventType::kKeyPressed, ui::VKEY_PROCESSKEY,
                              ui::DomCode::NONE,      ui::EF_IS_SYNTHESIZED,
                              ui::DomKey::PROCESS,    ui::EventTimeForNow()};
  DispatchKeyEvent(&key_down_event);

  client->InsertText(
      inputText,
      ui::TextInputClient::InsertTextCursorBehavior::kMoveCursorAfterText);

  ui::KeyEvent key_up_event{EventType::kKeyReleased, ui::VKEY_PROCESSKEY,
                            ui::DomCode::NONE,       ui::EF_IS_SYNTHESIZED,
                            ui::DomKey::PROCESS,     ui::EventTimeForNow()};
  DispatchKeyEvent(&key_up_event);
}

void InputMethodOHOS::InsertText(const std::string& text) {
  TRACE_EVENT0("ui", "InputMethodOHOS::InsertText");
  if (text.empty()) {
    LOG(ERROR) << "[InputMethodOHOS] insert text is empty";
    return;
  }
  std::u16string inputText = base::UTF8ToUTF16(text);
  InsertU16Text(inputText);
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
  auto* layout_engine = KeyboardLayoutEngineManager::GetKeyboardLayoutEngine();
  if (layout_engine == nullptr ||
      !layout_engine->Lookup(dom_Code, 0, &dom_key, &key_code)) {
    LOG(ERROR)
        << "[InputMethodOHOS] SendEnterKeyEvent failed to decode key_code";
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

void InputMethodOHOS::OnCursorUpdate(const gfx::Rect& rect) {
  auto task =
      base::BindOnce(&InputMethodOHOS::UpdateCursorTask, GetWeakPtr(), rect);
  InputMethodOHOSManager::GetInstance().GetTaskRunner()->PostTask(
      FROM_HERE, std::move(task));
}

gfx::AcceleratedWidget InputMethodOHOS::GetWidgetId() const {
  return widget_id_;
}

void InputMethodOHOS::RegistKeyboardHeightEvent(
    base::WeakPtr<InputMethodOHOS> weak_this) {
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

bool IsValidInputMethodOhosNAPI() {
  return ohos::adapter::IsNativeAPIsSufficientForInputMethod() &&
         !base::FeatureList::IsEnabled(features::kUseInputMethodJSApi);
}

std::unique_ptr<InputMethod> CreateInputMethodOHOS(
    ImeKeyEventDispatcher* ime_key_event_dispatcher,
    gfx::AcceleratedWidget widget) {
  if (IsValidInputMethodOhosNAPI()) {
    LOG(INFO) << "[InputMethodOHOSCAPI] Create InputMethodOHOSCAPI obj";
    return std::make_unique<InputMethodOHOSCAPI>(ime_key_event_dispatcher,
                                                 widget);
  }
  LOG(INFO) << "[InputMethodOHOS] Create InputMethodOHOS obj";
  return std::make_unique<InputMethodOHOSJSAPI>(ime_key_event_dispatcher,
                                                widget);
}

InputMethodOHOSCAPI::InputMethodOHOSCAPI(
    ImeKeyEventDispatcher* ime_key_event_dispatcher,
    gfx::AcceleratedWidget widget)
    : InputMethodOHOS(ime_key_event_dispatcher, widget) {
  RegistKeyboardHeightEvent(GetWeakPtr());
}

InputMethodOHOSCAPI::~InputMethodOHOSCAPI() {
  UnRegistKeyboardHeightEvent();
  CheckAndReleaseApiResources();
}

class InputRelationSingleton {
 private:
  std::unordered_map<InputMethod_TextEditorProxy*,
                     base::WeakPtr<InputMethodOHOSCAPI>>
      editor_proxy_to_input_method_ohos_map_;
  InputRelationSingleton() = default;
  InputRelationSingleton(const InputRelationSingleton&) = delete;
  InputRelationSingleton& operator=(const InputRelationSingleton&) = delete;
  mutable base::Lock lock_;

 public:
  static InputRelationSingleton& GetInstance() {
    static InputRelationSingleton instance;
    return instance;
  }

  void AddTextEditorProxyToInputMethodOHOSCAPIMapping(
      InputMethod_TextEditorProxy* text_editor_proxy,
      base::WeakPtr<InputMethodOHOSCAPI> input_method_ohos) {
    base::AutoLock auto_lock(lock_);
    editor_proxy_to_input_method_ohos_map_[text_editor_proxy] =
        std::move(input_method_ohos);
  }

  base::WeakPtr<InputMethodOHOSCAPI> GetInputMethodOHOSCAPIByTextEditorProxy(
      InputMethod_TextEditorProxy* text_editor_proxy) {
    base::AutoLock auto_lock(lock_);
    auto it = editor_proxy_to_input_method_ohos_map_.find(text_editor_proxy);
    if (it != editor_proxy_to_input_method_ohos_map_.end()) {
      return it->second;
    }
    LOG(ERROR) << "[InputMethodOHOSCAPI] "
                  "GetInputMethodOHOSCAPIByTextEditorProxy return nullptr";
    return nullptr;
  }

  void DeleteInputRelations(InputMethod_TextEditorProxy* text_editor_proxy) {
    base::AutoLock auto_lock(lock_);
    auto proxy_it =
        editor_proxy_to_input_method_ohos_map_.find(text_editor_proxy);
    if (proxy_it != editor_proxy_to_input_method_ohos_map_.end()) {
      proxy_it->second.reset();
      editor_proxy_to_input_method_ohos_map_.erase(proxy_it);
    }
  }
};

void GetTextConfigFunc(InputMethod_TextEditorProxy* text_editor_proxy,
                       InputMethod_TextConfig* config) {
  if (!text_editor_proxy || !config) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << (!text_editor_proxy ? " text_editor_proxy nullptr error"
                                      : " config nullptr error");
    return;
  }

  auto input_method_ohos =
      InputRelationSingleton::GetInstance()
          .GetInputMethodOHOSCAPIByTextEditorProxy(text_editor_proxy);
  if (!input_method_ohos) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << " input_method_ohos nullptr error";
    return;
  }
  input_method_ohos->SetTextConfig(config);
}

void InsertTextFunc(InputMethod_TextEditorProxy* text_editor_proxy,
                    const char16_t* text,
                    size_t length) {
  if (!text_editor_proxy || !text) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << (!text_editor_proxy ? " text_editor_proxy nullptr error"
                                      : " text nullptr error");
    return;
  }

  std::u16string inputText(text, length);
  if (inputText.empty() || inputText.length() != length) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << (inputText.empty() ? " insert text is empty"
                                     : " length error");
  }

  auto input_method_ohos =
      InputRelationSingleton::GetInstance()
          .GetInputMethodOHOSCAPIByTextEditorProxy(text_editor_proxy);
  if (!input_method_ohos) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << " input_method_ohos error";
    return;
  }
  auto task = base::BindOnce(&InputMethodOHOS::InsertU16Text,
                             input_method_ohos, inputText);
  InputMethodOHOSManager::GetInstance().GetTaskRunner()->PostTask(
      FROM_HERE, std::move(task));
}

void DeleteForwardFunc(InputMethod_TextEditorProxy* text_editor_proxy,
                       int32_t length) {
  if (!text_editor_proxy) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << " text_editor_proxy nullptr error";
    return;
  }

  auto input_method_ohos =
      InputRelationSingleton::GetInstance()
          .GetInputMethodOHOSCAPIByTextEditorProxy(text_editor_proxy);
  if (!input_method_ohos) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << " input_method_ohos nullptr error";
    return;
  }
  auto task = base::BindOnce(&InputMethodOHOS::DeleteForward,
                             input_method_ohos, length);
  InputMethodOHOSManager::GetInstance().GetTaskRunner()->PostTask(
      FROM_HERE, std::move(task));
}

void DeleteBackwardFunc(InputMethod_TextEditorProxy* text_editor_proxy,
                        int32_t length) {
  if (!text_editor_proxy) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << " text_editor_proxy nullptr error";
    return;
  }

  auto input_method_ohos =
      InputRelationSingleton::GetInstance()
          .GetInputMethodOHOSCAPIByTextEditorProxy(text_editor_proxy);
  if (!input_method_ohos) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << " input_method_ohos nullptr error";
    return;
  }
  auto task = base::BindOnce(&InputMethodOHOS::DeleteBackward,
                             input_method_ohos, length);
  InputMethodOHOSManager::GetInstance().GetTaskRunner()->PostTask(
      FROM_HERE, std::move(task));
}

void SendEnterKeyEventFunc(InputMethod_TextEditorProxy* text_editor_proxy,
                           InputMethod_EnterKeyType enter_key_type) {
  if (!text_editor_proxy) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << " text_editor_proxy nullptr error";
    return;
  }

  auto input_method_ohos =
      InputRelationSingleton::GetInstance()
          .GetInputMethodOHOSCAPIByTextEditorProxy(text_editor_proxy);
  if (!input_method_ohos) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << " input_method_ohos nullptr error";
    return;
  }
  auto task = base::BindOnce(&InputMethodOHOS::SendEnterKeyEvent,
                             input_method_ohos);
  InputMethodOHOSManager::GetInstance().GetTaskRunner()->PostTask(
      FROM_HERE, std::move(task));
}

void MoveCursorFunc(InputMethod_TextEditorProxy* text_editor_proxy,
                    InputMethod_Direction direction) {
  if (!text_editor_proxy) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << " text_editor_proxy nullptr error";
    return;
  }

  auto input_method_ohos =
      InputRelationSingleton::GetInstance()
          .GetInputMethodOHOSCAPIByTextEditorProxy(text_editor_proxy);
  if (!input_method_ohos) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << " input_method_ohos nullptr error";
    return;
  }
  auto task = base::BindOnce(&InputMethodOHOS::MoveCursor,
                             input_method_ohos, direction);
  InputMethodOHOSManager::GetInstance().GetTaskRunner()->PostTask(
      FROM_HERE, std::move(task));
}

void SendKeyboardStatusFunc(InputMethod_TextEditorProxy* proxy,
                            InputMethod_KeyboardStatus status) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void HandleSetSelection(InputMethod_TextEditorProxy* proxy,
                        int32_t start,
                        int32_t end) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void HandleExtendAction(InputMethod_TextEditorProxy* proxy,
                        InputMethod_ExtendAction action) {
  NOTIMPLEMENTED_LOG_ONCE();
}

int32_t ReceivePrivateCommand(InputMethod_TextEditorProxy* proxy,
                              InputMethod_PrivateCommand* private_command[],
                              size_t size) {
  NOTIMPLEMENTED_LOG_ONCE();
  return 0;
}

void GetLeftTextOfCursor(InputMethod_TextEditorProxy* proxy,
                         int32_t number,
                         char16_t text[],
                         size_t* length) {
  NOTIMPLEMENTED_LOG_ONCE();
}

void GetRightTextOfCursor(InputMethod_TextEditorProxy* proxy,
                          int32_t number,
                          char16_t text[],
                          size_t* length) {
  NOTIMPLEMENTED_LOG_ONCE();
}

int32_t GetTextIndexAtCursor(InputMethod_TextEditorProxy* proxy) {
  NOTIMPLEMENTED_LOG_ONCE();
  return 0;
}

int32_t SetPreviewText(InputMethod_TextEditorProxy* proxy,
                       const char16_t* text,
                       size_t length,
                       int32_t start,
                       int32_t end) {
  NOTIMPLEMENTED_LOG_ONCE();
  return 0;
}

void FinishTextPreview(InputMethod_TextEditorProxy* proxy) {
  NOTIMPLEMENTED_LOG_ONCE();
}

InputMethod_ErrorCode RegisterNotImplementedFuncs(
    InputMethod_TextEditorProxy* text_editor_proxy) {
  InputMethod_ErrorCode res = OH_TextEditorProxy_SetSendKeyboardStatusFunc(
      text_editor_proxy, SendKeyboardStatusFunc);
  if (res != IME_ERR_OK) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << " OH_TextEditorProxy_SetSendKeyboardStatusFunc fail, code: "
               << res;
    return res;
  }

  res = OH_TextEditorProxy_SetHandleSetSelectionFunc(text_editor_proxy,
                                                     HandleSetSelection);
  if (res != IME_ERR_OK) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << " OH_TextEditorProxy_SetHandleSetSelectionFunc fail, code: "
               << res;
    return res;
  }

  res = OH_TextEditorProxy_SetHandleExtendActionFunc(text_editor_proxy,
                                                     HandleExtendAction);
  if (res != IME_ERR_OK) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << "OH_TextEditorProxy_SetHandleExtendActionFunc  fail, code: "
               << res;
    return res;
  }

  res = OH_TextEditorProxy_SetReceivePrivateCommandFunc(text_editor_proxy,
                                                        ReceivePrivateCommand);
  if (res != IME_ERR_OK) {
    LOG(ERROR)
        << "[InputMethodOHOSCAPI] " << __FUNCTION__
        << "OH_TextEditorProxy_SetReceivePrivateCommandFunc  fail, code: "
        << res;
    return res;
  }

  res = OH_TextEditorProxy_SetGetLeftTextOfCursorFunc(text_editor_proxy,
                                                      GetLeftTextOfCursor);
  if (res != IME_ERR_OK) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << "OH_TextEditorProxy_SetGetLeftTextOfCursorFunc  fail, code: "
               << res;
    return res;
  }

  res = OH_TextEditorProxy_SetGetRightTextOfCursorFunc(text_editor_proxy,
                                                       GetRightTextOfCursor);
  if (res != IME_ERR_OK) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << "OH_TextEditorProxy_SetGetRightTextOfCursorFunc  fail, code: "
               << res;
    return res;
  }

  res = OH_TextEditorProxy_SetGetTextIndexAtCursorFunc(text_editor_proxy,
                                                       GetTextIndexAtCursor);
  if (res != IME_ERR_OK) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << "OH_TextEditorProxy_SetGetTextIndexAtCursorFunc  fail, code: "
               << res;
    return res;
  }

  res = OH_TextEditorProxy_SetSetPreviewTextFunc(text_editor_proxy,
                                                 SetPreviewText);
  if (res != IME_ERR_OK) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << "OH_TextEditorProxy_SetSetPreviewTextFunc  fail, code: "
               << res;
    return res;
  }

  res = OH_TextEditorProxy_SetFinishTextPreviewFunc(text_editor_proxy,
                                                    FinishTextPreview);
  if (res != IME_ERR_OK) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << "OH_TextEditorProxy_SetFinishTextPreviewFunc  fail, code: "
               << res;
    return res;
  }

  return IME_ERR_OK;
}

InputMethod_TextEditorProxy* CreateTextEditorProxy() {
  InputMethod_TextEditorProxy* text_editor_proxy = OH_TextEditorProxy_Create();
  if (!text_editor_proxy) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
               << "OH_TextEditorProxy_Create fail";
    return nullptr;
  }

  do {
    InputMethod_ErrorCode err_code = OH_TextEditorProxy_SetGetTextConfigFunc(
        text_editor_proxy, GetTextConfigFunc);
    if (err_code != IME_ERR_OK) {
      LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
                 << "OH_TextEditorProxy_SetGetTextConfigFunc fail, err_code: "
                 << err_code;
      break;
    }

    err_code =
        OH_TextEditorProxy_SetInsertTextFunc(text_editor_proxy, InsertTextFunc);
    if (err_code != IME_ERR_OK) {
      LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
                 << "OH_TextEditorProxy_SetInsertTextFunc fail, err_code: "
                 << err_code;
      break;
    }

    err_code = OH_TextEditorProxy_SetDeleteForwardFunc(text_editor_proxy,
                                                       DeleteForwardFunc);
    if (err_code != IME_ERR_OK) {
      LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
                 << "OH_TextEditorProxy_SetDeleteForwardFunc fail, err_code: "
                 << err_code;
      break;
    }

    err_code = OH_TextEditorProxy_SetDeleteBackwardFunc(text_editor_proxy,
                                                        DeleteBackwardFunc);
    if (err_code != IME_ERR_OK) {
      LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
                 << "OH_TextEditorProxy_SetDeleteBackwardFunc fail, err_code: "
                 << err_code;
      break;
    }

    err_code = OH_TextEditorProxy_SetSendEnterKeyFunc(text_editor_proxy,
                                                      SendEnterKeyEventFunc);
    if (err_code != IME_ERR_OK) {
      LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
                 << "OH_TextEditorProxy_SetSendEnterKeyFunc fail, err_code: "
                 << err_code;
      break;
    }

    err_code =
        OH_TextEditorProxy_SetMoveCursorFunc(text_editor_proxy, MoveCursorFunc);
    if (err_code != IME_ERR_OK) {
      LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
                 << "OH_TextEditorProxy_SetMoveCursorFunc fail, err_code: "
                 << err_code;
      break;
    }

    err_code = RegisterNotImplementedFuncs(text_editor_proxy);
    if (err_code != IME_ERR_OK) {
      LOG(ERROR) << "[InputMethodOHOSCAPI] " << __FUNCTION__
                 << " RegisterNotImplementedFuncs fail,  err_code: "
                 << err_code;
      break;
    }

    return text_editor_proxy;
  } while (false);

  OH_TextEditorProxy_Destroy(text_editor_proxy);
  text_editor_proxy = nullptr;
  return text_editor_proxy;
}

InputMethod_TextInputType InputTypeChromiumToOH(TextInputType ctype) {
  switch (ctype) {
    case TextInputType::TEXT_INPUT_TYPE_NONE:
      return InputMethod_TextInputType::IME_TEXT_INPUT_TYPE_NONE;
    case TextInputType::TEXT_INPUT_TYPE_TEXT:
      return InputMethod_TextInputType::IME_TEXT_INPUT_TYPE_TEXT;
    case TextInputType::TEXT_INPUT_TYPE_NUMBER:
      return InputMethod_TextInputType::IME_TEXT_INPUT_TYPE_NUMBER;
    case TextInputType::TEXT_INPUT_TYPE_TELEPHONE:
      return InputMethod_TextInputType::IME_TEXT_INPUT_TYPE_PHONE;
    case TextInputType::TEXT_INPUT_TYPE_DATE_TIME:
      return InputMethod_TextInputType::IME_TEXT_INPUT_TYPE_DATETIME;
    case TextInputType::TEXT_INPUT_TYPE_EMAIL:
      return InputMethod_TextInputType::IME_TEXT_INPUT_TYPE_EMAIL_ADDRESS;
    case TextInputType::TEXT_INPUT_TYPE_URL:
      return InputMethod_TextInputType::IME_TEXT_INPUT_TYPE_URL;
    case TextInputType::TEXT_INPUT_TYPE_TEXT_AREA:
      return InputMethod_TextInputType::IME_TEXT_INPUT_TYPE_MULTILINE;
    case TextInputType::TEXT_INPUT_TYPE_PASSWORD:
      return InputMethod_TextInputType::IME_TEXT_INPUT_TYPE_VISIBLE_PASSWORD;
    default:
      return InputMethod_TextInputType::IME_TEXT_INPUT_TYPE_TEXT;
  }
}

InputMethod_RequestKeyboardReason RequestKeyboardReasonChromiumToOH(
    ui::RequestKeyboardReason reason) {
  switch (reason) {
    case ui::RequestKeyboardReason::REQUEST_KEYBOARD_REASON_NONE:
      return InputMethod_RequestKeyboardReason::IME_REQUEST_REASON_NONE;
    case ui::RequestKeyboardReason::REQUEST_KEYBOARD_REASON_MOUSE:
      return InputMethod_RequestKeyboardReason::IME_REQUEST_REASON_MOUSE;
    case ui::RequestKeyboardReason::REQUEST_KEYBOARD_REASON_TOUCH:
      return InputMethod_RequestKeyboardReason::IME_REQUEST_REASON_TOUCH;
    case ui::RequestKeyboardReason::REQUEST_KEYBOARD_REASON_OTHER:
      return InputMethod_RequestKeyboardReason::IME_REQUEST_REASON_OTHER;
    default:
      return InputMethod_RequestKeyboardReason::IME_REQUEST_REASON_MOUSE;
  }
}

gfx::Rect InputMethodOHOS::GetRectPixel() {
  display::Display current_display;
  if (GetTextInputClient()) {
    current_display = GetTextInputClient()->GetDisplayForClient();
  } else {
    current_display = display::Screen::GetScreen()->GetPrimaryDisplay();
  }
  gfx::Rect rect_pixel = display::ohos::ScreenOhos::ConvertDipToPixel(
      current_display, focus_rect_);
  if (rect_pixel.width() == 0 && rect_pixel.height() == 0) {
    LOG(INFO) << "[InputMethodOHOSCAPI] " << __FUNCTION__
              << ", The width and height of rect_pixel are both 0";
    rect_pixel = focus_rect_;
  }
  return rect_pixel;
}

InputMethod_CursorInfo* InputMethodOHOSCAPI::UpdateCursorInfo(
    InputMethod_CursorInfo* cursor_info) {
  if (!cursor_info) {
    return nullptr;
  }

  auto rect_pixel = GetRectPixel();
  if (OH_CursorInfo_SetRect(cursor_info, rect_pixel.x(), rect_pixel.y(),
                            rect_pixel.width(),
                            rect_pixel.height()) != IME_ERR_OK) {
    LOG(ERROR) << "UpdateCursorInfo OH_CursorInfo_SetRect fail";
    return nullptr;
  }
  return cursor_info;
}

InputMethod_TextConfig* InputMethodOHOSCAPI::SetTextConfig(
    InputMethod_TextConfig* text_config) {
  auto res = OH_TextConfig_SetInputType(
      text_config, InputTypeChromiumToOH(text_input_type_));
  if (res != IME_ERR_OK) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] OH_TextConfig_SetInputType error: "
               << res;
    return nullptr;
  }
  res = OH_TextConfig_SetEnterKeyType(
      text_config, InputMethod_EnterKeyType::IME_ENTER_KEY_DONE);
  if (res != IME_ERR_OK) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] OH_TextConfig_SetEnterKeyType error: "
               << res;
    return nullptr;
  }
  InputMethod_CursorInfo* cursor_info = nullptr;
  res = OH_TextConfig_GetCursorInfo(text_config, &cursor_info);
  if (res != IME_ERR_OK) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] OH_TextConfig_GetCursorInfo error: "
               << res;
    return nullptr;
  }
  if (cursor_info && !UpdateCursorInfo(cursor_info)) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] UpdateCursorInfo error";
    return nullptr;
  }
  return text_config;
}

void InputMethodOHOSCAPI::CheckAndReleaseApiResources() {
  if (input_method_proxy_) {
    InputMethod_ErrorCode res =
        OH_InputMethodController_Detach(input_method_proxy_);
    if (res == IME_ERR_OK) {
      LOG(INFO)
          << "[InputMethodOHOSCAPI] OH_InputMethodController_Detach success";
      input_method_proxy_ = nullptr;
    } else {
      LOG(WARNING)
          << "[InputMethodOHOSCAPI] OH_InputMethodController_Detach failed: "
          << res;
    }
    is_attach_ = false;
  }

  if (text_editor_proxy_) {
    InputRelationSingleton::GetInstance().DeleteInputRelations(
        text_editor_proxy_);
    OH_TextEditorProxy_Destroy(text_editor_proxy_);
    text_editor_proxy_ = nullptr;
  }

  if (attach_options_) {
    OH_AttachOptions_Destroy(attach_options_);
    attach_options_ = nullptr;
  }
  if (is_create_cursor_info_) {
    OH_CursorInfo_Destroy(cursor_info_);
    cursor_info_ = nullptr;
    is_create_cursor_info_ = false;
  }
}

InputMethod_AttachOptions* InputMethodOHOSCAPI::UpdateAttachOptions(
    ui::RequestKeyboardReason reason) {
  InputMethod_RequestKeyboardReason keyboard_reason =
      RequestKeyboardReasonChromiumToOH(reason);
  int keyboard_reason_int =
      static_cast<InputMethod_RequestKeyboardReason>(keyboard_reason);
  if (!attach_options_) {
    attach_options_ =
        ohos::adapter::OhAttachOptionsCreateWithRequestKeyboardReason(true, keyboard_reason);
    if (!attach_options_) {
      LOG(ERROR) << "[InputMethodOHOSCAPI] Attach fail, attach_option is null";
      return nullptr;
    }
    LOG(INFO) << "[InputMethodOHOSCAPI] "
                 "OhAttachOptionsCreateWithRequestKeyboardReason success";
    return attach_options_;
  }
  int last_reason_int = static_cast<InputMethod_RequestKeyboardReason>(
      InputMethod_RequestKeyboardReason::IME_REQUEST_REASON_NONE);
  InputMethod_ErrorCode res = ohos::adapter::OhAttachOptionsGetRequestKeyboardReason(
      attach_options_, &last_reason_int);
  if (res == IME_ERR_OK && keyboard_reason_int == last_reason_int) {
    return attach_options_;
  }
  OH_AttachOptions_Destroy(attach_options_);
  attach_options_ = nullptr;

  LOG(INFO) << "[InputMethodOHOSCAPI]  OH_AttachOptions_Destroy success";
  attach_options_ =
      ohos::adapter::OhAttachOptionsCreateWithRequestKeyboardReason(true, keyboard_reason);
  if (!attach_options_) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] "
                  "OhAttachOptionsCreateWithRequestKeyboardReason  fail, "
                  "attach_option is null";
    return nullptr;
  }
  LOG(INFO) << "[InputMethodOHOSCAPI]  "
               "OhAttachOptionsCreateWithRequestKeyboardReason success";
  return attach_options_;
}

void InputMethodOHOSCAPI::DetachTextInputTask() {
  if (InputMethodOHOSManager::GetInstance().ReleaseActiveInstance(
          weak_ptr_factory_.GetWeakPtr())) {
    CheckAndReleaseApiResources();
  }
  SetSoftKeyboardForWidget(widget_id_, false);
}

void InputMethodOHOSCAPI::AttachTextInputTask(
    ui::RequestKeyboardReason reason) {
  SetSoftKeyboardForWidget(widget_id_, true);
  CheckAndReleaseApiResources();

  text_editor_proxy_ = CreateTextEditorProxy();
  if (!text_editor_proxy_) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] AttachTextInputTask fail, "
                  "text_editor_proxy_ is null";
    return;
  }
  InputRelationSingleton::GetInstance()
      .AddTextEditorProxyToInputMethodOHOSCAPIMapping(
          text_editor_proxy_, weak_ptr_factory_.GetWeakPtr());
  if (!UpdateAttachOptions(reason)) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] UpdateAttachOptions fail, "
                  "attach_option is null";
    return;
  }

  InputMethod_ErrorCode res = OH_InputMethodController_Attach(
      text_editor_proxy_, attach_options_, &input_method_proxy_);
  if (res != IME_ERR_OK) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] Attach "
                  "OH_InputMethodController_Attach  fail, code: "
               << res;
    return;
  }
  LOG(INFO) << "[InputMethodOHOSCAPI] OH_InputMethodController_Attach success";

  if (!CreateCursorInfo()) {
    LOG(INFO) << "[InputMethodOHOSCAPI] CreateCursorInfo fail";
    return;
  }

  res =
      OH_InputMethodProxy_NotifyCursorUpdate(input_method_proxy_, cursor_info_);
  if (res != IME_ERR_OK) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] UpdateCursorInfo "
                  "OH_InputMethodProxy_NotifyCursorUpdate fail, code: "
               << res;
    return;
  }
  LOG(INFO)
      << "[InputMethodOHOSCAPI] OH_InputMethodProxy_NotifyCursorUpdate success";

  InputMethodOHOSManager::GetInstance().SetActiveInstance(
      weak_ptr_factory_.GetWeakPtr());
  is_attach_ = true;
}

void InputMethodOHOSCAPI::SetVirtualKeyboardVisibilityTask(
    bool should_show,
    ui::RequestKeyboardReason reason) {
  if (should_show && is_attach_) {
    if (!UpdateAttachOptions(reason)) {
      LOG(ERROR) << "[InputMethodOHOSCAPI] UpdateAttachOptions fail, "
                    "attach_option is null";
      return;
    }
    InputMethod_ErrorCode res =
        ohos::adapter::OhInputMethodProxyShowTextInput(input_method_proxy_, attach_options_);
    if (res != IME_ERR_OK) {
      LOG(ERROR)
          << "[InputMethodOHOSCAPI] OhInputMethodProxyShowTextInput error: "
          << res;
      return;
    }
    LOG(INFO)
        << "[InputMethodOHOSCAPI] OhInputMethodProxyShowTextInput success";
  }
  InputMethodBase::SetVirtualKeyboardVisibilityIfEnabled(should_show);
}

void InputMethodOHOSCAPI::MoveCursor(int direction) {
  TRACE_EVENT0("ui", "InputMethodOHOSCAPI::MoveCursor");
  EventType type = EventType::kKeyPressed;
  EventType type_release = EventType::kKeyReleased;
  KeyboardCode key_code;
  DomCode dom_Code;

  switch (direction) {
    case InputMethod_Direction::IME_DIRECTION_UP: {
      key_code = ui::VKEY_UP;
      dom_Code = ui::DomCode::ARROW_UP;
      break;
    }
    case InputMethod_Direction::IME_DIRECTION_LEFT: {
      key_code = ui::VKEY_LEFT;
      dom_Code = ui::DomCode::ARROW_LEFT;
      break;
    }
    case InputMethod_Direction::IME_DIRECTION_RIGHT: {
      key_code = ui::VKEY_RIGHT;
      dom_Code = ui::DomCode::ARROW_RIGHT;
      break;
    }
    case InputMethod_Direction::IME_DIRECTION_DOWN: {
      key_code = ui::VKEY_DOWN;
      dom_Code = ui::DomCode::ARROW_DOWN;
      break;
    }
    default: {
      LOG(ERROR) << "[InputMethodOHOSCAPI] MoveCursor unknown direction: "
                 << direction;
      return;
    }
  }

  DomKey dom_key;
  auto* layout_engine = KeyboardLayoutEngineManager::GetKeyboardLayoutEngine();

  if (layout_engine == nullptr ||
      !layout_engine->Lookup(dom_Code, 0, &dom_key, &key_code)) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] SendEnterKeyEvent failed to decode "
                  "key_code";
    return;
  }

  ui::KeyEvent event(type, key_code, dom_Code, 0, dom_key, EventTimeForNow());
  DispatchKeyEvent(&event);

  ui::KeyEvent event_release(type_release, key_code, dom_Code, 0, dom_key,
                             EventTimeForNow());
  DispatchKeyEvent(&event_release);
}

InputMethod_CursorInfo* InputMethodOHOSCAPI::CreateCursorInfo() {
  auto rect_pixel = GetRectPixel();
  if (cursor_info_) {
    OH_CursorInfo_Destroy(cursor_info_);
    cursor_info_ = nullptr;
  }
  cursor_info_ = OH_CursorInfo_Create(rect_pixel.x(), rect_pixel.y(),
                                      rect_pixel.width(), rect_pixel.height());
  if (!cursor_info_) {
    LOG(ERROR)
        << "[InputMethodOHOSCAPI] UpdateCursorInfo OH_CursorInfo_Create fail";
    return nullptr;
  }
  is_create_cursor_info_ = true;
  LOG(INFO)
      << "[InputMethodOHOSCAPI] UpdateCursorInfo OH_CursorInfo_Create success";
  return cursor_info_;
}

void InputMethodOHOSCAPI::UpdateCursorTask(const gfx::Rect& rect) {
  // Do not notify cursor update if cursor rect unchanged or cursor rect is 0000
  // which means input method will be detached
  if (focus_rect_.ApproximatelyEqual(rect, 0) ||
      rect == gfx::Rect(0, 0, 0, 0)) {
    return;
  }
  focus_rect_ = rect;
  LOG(INFO) << "[InputMethodOHOSCAPI] UpdateCursorTask change focus_rect, x: "
            << focus_rect_.x() << " , y:" << focus_rect_.y()
            << " , width:" << focus_rect_.width()
            << ", height: " << focus_rect_.height();

  if (!input_method_proxy_) {
    LOG(INFO) << "[InputMethodOHOSCAPI] input_method_proxy_ is nullptr";
    return;
  }

  if (!CreateCursorInfo()) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] CreateCursorInfo return nullptr";
    return;
  }

  if (!is_attach_) {
    return;
  }

  InputMethod_ErrorCode res =
      OH_InputMethodProxy_NotifyCursorUpdate(input_method_proxy_, cursor_info_);
  if (res != IME_ERR_OK) {
    LOG(ERROR) << "[InputMethodOHOSCAPI] UpdateCursorInfo "
                  "OH_InputMethodProxy_NotifyCursorUpdate "
                  "fail, code: "
               << res;
    return;
  }
  LOG(INFO)
      << "[InputMethodOHOSCAPI] OH_InputMethodProxy_NotifyCursorUpdate success";
}

InputMethodOHOSJSAPI::InputMethodOHOSJSAPI(
    ImeKeyEventDispatcher* ime_key_event_dispatcher,
    gfx::AcceleratedWidget widget)
    : InputMethodOHOS(ime_key_event_dispatcher, widget) {
  RegistKeyboardHeightEvent(GetWeakPtr());
}

InputMethodOHOSJSAPI::~InputMethodOHOSJSAPI() {
  UnRegistKeyboardHeightEvent();
}

void InputMethodOHOSJSAPI::MoveCursor(int direction) {
  TRACE_EVENT0("ui", "InputMethodOHOSJSAPI::MoveCursor");
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
      LOG(ERROR) << "[InputMethodOHOSJSAPI] MoveCursor unknown direction:"
                 << direction;
      return;
    }
  }

  DomKey dom_key;
  auto* layout_engine = KeyboardLayoutEngineManager::GetKeyboardLayoutEngine();

  if (layout_engine == nullptr ||
      !layout_engine->Lookup(dom_Code, 0, &dom_key, &key_code)) {
    LOG(ERROR)
        << "[InputMethodOHOSJSAPI] SendEnterKeyEvent failed to decode key_code";
    return;
  }

  ui::KeyEvent event(type, key_code, dom_Code, 0, dom_key, EventTimeForNow());
  DispatchKeyEvent(&event);

  ui::KeyEvent event_release(type_release, key_code, dom_Code, 0, dom_key,
                             EventTimeForNow());
  DispatchKeyEvent(&event_release);
}

void InputMethodOHOSJSAPI::DetachTextInputTask() {
  if (InputMethodOHOSManager::GetInstance().ReleaseActiveInstance(
          weak_ptr_factory_.GetWeakPtr())) {
    auto& ime_instance = ohos::adapter::InputMethodOHOSAdapter::GetInstance();
    ime_instance.DetachTextInput();
    is_attach_ = false;
  }

  SetSoftKeyboardForWidget(widget_id_, false);
}

void InputMethodOHOSJSAPI::AttachTextInputTask(
    ui::RequestKeyboardReason reason) {
  SetSoftKeyboardForWidget(widget_id_, true);
  ohos::adapter::IMFAdapterCursorInfo cursorInfo = GetCursorInfo();
  ohos::adapter::IMFAdapterTextConfig textConfig = {GetInputAttribute(),
                                                    cursorInfo};
  ohos::adapter::InputMethodOHOSAdapter::GetInstance().AttachTextInput(
      textConfig, reason);
  InputMethodOHOSManager::GetInstance().SetActiveInstance(
      weak_ptr_factory_.GetWeakPtr());
  is_attach_ = true;
}

void InputMethodOHOSJSAPI::SetVirtualKeyboardVisibilityTask(
    bool should_show,
    ui::RequestKeyboardReason reason) {
  if (should_show && is_attach_) {
    ohos::adapter::InputMethodOHOSAdapter::GetInstance().ShowTextInput(reason);
  }
  InputMethodBase::SetVirtualKeyboardVisibilityIfEnabled(should_show);
}

void InputMethodOHOSJSAPI::UpdateCursorTask(const gfx::Rect& rect) {
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

ohos::adapter::IMFAdapterCursorInfo InputMethodOHOSJSAPI::GetCursorInfo() {
  auto rect_pixel = GetRectPixel();
  ohos::adapter::IMFAdapterCursorInfo cursorInfo{.left = rect_pixel.x(),
                                                 .top = rect_pixel.y(),
                                                 .width = rect_pixel.width(),
                                                 .height = rect_pixel.height()};
  return cursorInfo;
}

ohos::adapter::IMFAdapterInputAttribute InputMethodOHOSJSAPI::GetInputAttribute() {
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

void InputMethodOHOSJSAPI::UpdateAttributeTask() {
  auto& ime_instance = ohos::adapter::InputMethodOHOSAdapter::GetInstance();
  ime_instance.UpdateAttribute(GetInputAttribute());
}

}  // namespace ui
