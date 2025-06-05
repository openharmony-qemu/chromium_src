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

#include "base/memory/weak_ptr.h"
#include "ohos/adapter/ime_adapter/input_method_ohos_adapter.h"
#include "ui/base/ime/input_method_base.h"
#include "ui/base/ime/text_input_client.h"
#include "ui/gfx/geometry/rect.h"

namespace ui {
class COMPONENT_EXPORT(UI_BASE_IME_OHOS) InputMethodOHOS
    : public InputMethodBase {
 public:
  explicit InputMethodOHOS(ImeKeyEventDispatcher* ime_key_event_dispatcher);
  ~InputMethodOHOS() override;

  InputMethodOHOS(InputMethodOHOS&) = delete;
  InputMethodOHOS operator=(InputMethodOHOS&) = delete;

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
  void DeleteBackward(int32_t length);
  void DeleteForward(int32_t length);
  void SendEnterKeyEvent();
  void ExitFullscreenEvent();
  void MoveCursor(int direction);
  ohos::adapter::IMFAdapterCursorInfo GetCursorInfo();
  ohos::adapter::IMFAdapterInputAttribute GetInputAttribute();
  void SetVirtualKeyboardVisibilityIfEnabled(bool should_show) override;
  void DetachTextInputTask();
  void AttachTextInputTask(ui::RequestKeyboardReason reason);
  void UpdateAttributeTask();
  void SetVirtualKeyboardVisibilityTask(bool should_show);
  void UpdateCursorTask(const gfx::Rect& rect);

 private:
  float GetPixelRatio(const gfx::Rect& rect);

 private:
  TextInputType text_input_type_ = ui::TEXT_INPUT_TYPE_NONE;
  gfx::Rect focus_rect_;
  bool is_attach_ = false;
  base::WeakPtrFactory<InputMethodOHOS> weak_ptr_factory_{this};
};
}  // namespace ui
#endif  // UI_BASE_IME_INPUT_METHOD_OHOS_H_
