// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_BASE_IME_INPUT_METHOD_OHOS_H_
#define UI_BASE_IME_INPUT_METHOD_OHOS_H_

#include "base/memory/weak_ptr.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "base/task/thread_pool.h"
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
  void MoveCursor(int direction);
  ohos::adapter::IMFAdapterCursorInfo GetCursorInfo();
  ohos::adapter::IMFAdapterInputAttribute GetInputAttribute();
  void SetVirtualKeyboardVisibilityIfEnabled(bool should_show) override;
  void DetachTextInputTask();
  void AttachTextInputTask();
  void UpdateAttributeTask();
  void SetVirtualKeyboardVisibilityTask(bool should_show);
  void UpdateCursorTask(const gfx::Rect& rect);

 private:
  float GetPixelRatio(const gfx::Rect& rect);

 private:
  TextInputType text_input_type_ = ui::TEXT_INPUT_TYPE_NONE;
  gfx::Rect focus_rect_;
  bool is_attach_ = false;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  scoped_refptr<base::SequencedTaskRunner> sequenced_task_runner_ =
      base::ThreadPool::CreateSequencedTaskRunner(
          {base::MayBlock(), base::TaskShutdownBehavior::CONTINUE_ON_SHUTDOWN});
};
}  // namespace ui
#endif  // UI_BASE_IME_INPUT_METHOD_OHOS_H_
