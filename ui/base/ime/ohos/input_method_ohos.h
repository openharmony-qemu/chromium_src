/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UI_BASE_IME_INPUT_METHOD_OHOS_H_
#define UI_BASE_IME_INPUT_METHOD_OHOS_H_

#include <inputmethod/inputmethod_attach_options_capi.h>
#include <inputmethod/inputmethod_controller_capi.h>
#include <inputmethod/inputmethod_cursor_info_capi.h>
#include <inputmethod/inputmethod_inputmethod_proxy_capi.h>
#include <inputmethod/inputmethod_text_config_capi.h>
#include <inputmethod/inputmethod_text_editor_proxy_capi.h>
#include <inputmethod/inputmethod_types_capi.h>
#include <cstdint>

#include "base/memory/weak_ptr.h"
#include "base/timer/timer.h"
#include "ohos/adapter/ime_adapter/input_method_ohos_adapter.h"
#include "ui/base/ime/input_method_base.h"
#include "ui/base/ime/text_input_client.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/native_widget_types.h"

namespace ui {

bool IsValidInputMethodOhosNAPI();
std::unique_ptr<InputMethod> CreateInputMethodOHOS(
    ImeKeyEventDispatcher* ime_key_event_dispatcher,
    gfx::AcceleratedWidget widget);

class COMPONENT_EXPORT(UI_BASE_IME_OHOS) InputMethodOHOS
    : public InputMethodBase {
 public:
  explicit InputMethodOHOS(ImeKeyEventDispatcher* ime_key_event_dispatcher,
                           gfx::AcceleratedWidget widget);
  ~InputMethodOHOS() override = default;

  InputMethodOHOS(InputMethodOHOS&) = delete;
  InputMethodOHOS operator=(InputMethodOHOS&) = delete;

  virtual void MoveCursor(int direction) = 0;
  virtual void DetachTextInputTask() = 0;
  virtual void AttachTextInputTask(ui::RequestKeyboardReason reason) = 0;
  virtual void SetVirtualKeyboardVisibilityTask(bool should_show, ui::RequestKeyboardReason reason) = 0;
  virtual void UpdateCursorTask(const gfx::Rect& rect) = 0;
  virtual base::WeakPtr<InputMethodOHOS> GetWeakPtr() = 0;

  // InputMethodBase interface implementation.
  ui::EventDispatchDetails DispatchKeyEvent(ui::KeyEvent* event) override;
  void OnCaretBoundsChanged(const TextInputClient* client) override;
  bool IsCandidatePopupOpen() const override;
  void OnTextInputTypeChanged(TextInputClient* client) override;
  void OnCursorUpdate(const gfx::Rect& rect);
  void OnDidChangeFocusedClient(TextInputClient* focused_before,
                                TextInputClient* focused) override;
  void CancelComposition(const TextInputClient* client) override;
  void UpdateContextFocusState();
  void InsertText(const std::string& text);
  void InsertU16Text(const std::u16string& inputText);
  void DeleteBackward(int32_t length);
  void DeleteForward(int32_t length);
  void SendEnterKeyEvent();
  void SetVirtualKeyboardVisibilityIfEnabled(bool should_show) override;
  gfx::AcceleratedWidget GetWidgetId() const;
  void RegistKeyboardHeightEvent(base::WeakPtr<InputMethodOHOS> wptr);
  void UnRegistKeyboardHeightEvent();
  gfx::Rect GetRectPixel();

 private:
  void SetVirtualKeyboardBoundsTask(int32_t keyboard_height);
  bool IsDispatchedPressAndReleaseKeyEvents(int32_t length,
                                            KeyboardCode key_code,
                                            DomCode dom_Code);

 protected:
  TextInputType text_input_type_ = ui::TEXT_INPUT_TYPE_NONE;
  gfx::Rect focus_rect_;
  bool is_attach_ = false;
  gfx::AcceleratedWidget widget_id_;
  base::OneShotTimer delayed_attach_timer_;
};

class COMPONENT_EXPORT(UI_BASE_IME_OHOS) InputMethodOHOSCAPI
    : public InputMethodOHOS {
 public:
  explicit InputMethodOHOSCAPI(ImeKeyEventDispatcher* ime_key_event_dispatcher,
                           gfx::AcceleratedWidget widget);
  ~InputMethodOHOSCAPI() override;
  InputMethodOHOSCAPI(InputMethodOHOS&) = delete;
  InputMethodOHOSCAPI operator=(InputMethodOHOS&) = delete;

  // InputMethodOHOS interface implementation.
  void MoveCursor(int direction) override;
  void DetachTextInputTask() override;
  void AttachTextInputTask(ui::RequestKeyboardReason reason) override;
  void SetVirtualKeyboardVisibilityTask(
      bool should_show,
      ui::RequestKeyboardReason reason) override;
  void UpdateCursorTask(const gfx::Rect& rect) override;
  base::WeakPtr<InputMethodOHOS> GetWeakPtr() override{
    return weak_ptr_factory_.GetWeakPtr();
  }
  InputMethod_TextConfig* SetTextConfig(InputMethod_TextConfig* text_config);

 private:
  void CheckAndReleaseApiResources();
  InputMethod_AttachOptions* UpdateAttachOptions(
      ui::RequestKeyboardReason reason);
  InputMethod_CursorInfo* CreateCursorInfo();
  InputMethod_CursorInfo* UpdateCursorInfo(InputMethod_CursorInfo* cursor_info);

  RAW_PTR_EXCLUSION InputMethod_TextEditorProxy* text_editor_proxy_ = nullptr;
  RAW_PTR_EXCLUSION InputMethod_AttachOptions* attach_options_ = nullptr;
  RAW_PTR_EXCLUSION InputMethod_InputMethodProxy* input_method_proxy_ = nullptr;
  RAW_PTR_EXCLUSION InputMethod_CursorInfo* cursor_info_ = nullptr;
  bool is_create_cursor_info_ = false;
  base::WeakPtrFactory<InputMethodOHOSCAPI> weak_ptr_factory_{this};
};

class COMPONENT_EXPORT(UI_BASE_IME_OHOS) InputMethodOHOSJSAPI
    : public InputMethodOHOS {
 public:
  explicit InputMethodOHOSJSAPI(ImeKeyEventDispatcher* ime_key_event_dispatcher,
                           gfx::AcceleratedWidget widget);
  ~InputMethodOHOSJSAPI() override;
  InputMethodOHOSJSAPI(InputMethodOHOS&) = delete;
  InputMethodOHOSJSAPI operator=(InputMethodOHOS&) = delete;

  // InputMethodOHOS interface implementation.
  void MoveCursor(int direction) override;
  void DetachTextInputTask() override;
  void AttachTextInputTask(ui::RequestKeyboardReason reason) override;
  void SetVirtualKeyboardVisibilityTask(
      bool should_show,
      ui::RequestKeyboardReason reason) override;
  void UpdateCursorTask(const gfx::Rect& rect) override;
  base::WeakPtr<InputMethodOHOS> GetWeakPtr()override {
    return weak_ptr_factory_.GetWeakPtr();
  }

private:
  ohos::adapter::IMFAdapterInputAttribute GetInputAttribute();
  ohos::adapter::IMFAdapterCursorInfo GetCursorInfo();
  void UpdateAttributeTask();

  base::WeakPtrFactory<InputMethodOHOSJSAPI> weak_ptr_factory_{this};
};
}  // namespace ui
#endif  // UI_BASE_IME_INPUT_METHOD_OHOS_H_
