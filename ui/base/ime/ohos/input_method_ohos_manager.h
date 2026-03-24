// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_BASE_IME_INPUT_METHOD_OHOS_MANAGER_H_
#define UI_BASE_IME_INPUT_METHOD_OHOS_MANAGER_H_

#include "ui/base/ime/ohos/input_method_ohos.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "base/task/thread_pool.h"

namespace ui {
class COMPONENT_EXPORT(UI_BASE_IME_OHOS) InputMethodOHOSManager
    : public ohos::adapter::InputMethodOHOSAdapter::Delegate {
 public:
  static InputMethodOHOSManager& GetInstance();
  InputMethodOHOSManager();
  void SetActiveInstance(base::WeakPtr<InputMethodOHOS> instance);
  bool IsActiveInstance(const base::WeakPtr<InputMethodOHOS>& instance);
  bool ReleaseActiveInstance(const base::WeakPtr<InputMethodOHOS>& instance);

  void InsertText(const std::string& text) override;
  void DeleteBackward(int32_t length) override;
  void DeleteForward(int32_t length) override;
  void SendEnterKeyEvent() override;
  void MoveCursor(int direction) override;

  void InsertTextOnUIThread(const std::string& text);
  void DeleteBackwardOnUIThread(int32_t length);
  void DeleteForwardOnUIThread(int32_t length);
  void SendEnterKeyEventOnUIThread();
  void MoveCursorOnUIThread(int direction);

  scoped_refptr<base::SingleThreadTaskRunner>& GetTaskRunner();

 private:
  base::WeakPtr<InputMethodOHOS> active_instance_;
  std::once_flag task_runner_flag_;
  std::once_flag sequenced_task_runner_flag_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::WeakPtrFactory<InputMethodOHOSManager> weak_factory_{this};
};
}  // namespace ui
#endif  // UI_BASE_IME_INPUT_METHOD_OHOS_MANAGER_H_