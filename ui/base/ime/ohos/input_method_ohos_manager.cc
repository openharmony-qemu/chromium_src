// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/ime/ohos/input_method_ohos_manager.h"
#include "ohos/adapter/ime_adapter/input_method_ohos_adapter.h"

namespace ui {
InputMethodOHOSManager& InputMethodOHOSManager::GetInstance() {
  static InputMethodOHOSManager instance;
  return instance;
}

InputMethodOHOSManager::InputMethodOHOSManager() {
  ohos::adapter::InputMethodOHOSAdapter::GetInstance().Register(this);
  task_runner_ = base::TaskRunnerOHOS::GetUIThreadTaskRunner();
}

void InputMethodOHOSManager::SetActiveInstance(
    base::WeakPtr<InputMethodOHOS> instance) {
  active_instance_ = std::move(instance);
}

bool InputMethodOHOSManager::IsActiveInstance(
    const base::WeakPtr<InputMethodOHOS>& instance) {
  const bool is_valid = instance.get() != nullptr;
  return is_valid && (active_instance_.get() == instance.get());
}

bool InputMethodOHOSManager::ReleaseActiveInstance(
    const base::WeakPtr<InputMethodOHOS>& instance) {
  if (IsActiveInstance(instance)) {
    active_instance_.reset();
    return true;
  }
  return false;
}

scoped_refptr<base::SingleThreadTaskRunner>&
InputMethodOHOSManager::GetTaskRunner() {
  std::call_once(task_runner_flag_, [this] {
    task_runner_ = base::TaskRunnerOHOS::GetUIThreadTaskRunner();
  });
  return task_runner_;
}

void InputMethodOHOSManager::InsertTextOnUIThread(const std::string& text) {
  if (active_instance_) {
    active_instance_->InsertText(text);
  }
}

void InputMethodOHOSManager::DeleteBackwardOnUIThread(int32_t length) {
  if (active_instance_) {
    active_instance_->DeleteBackward(length);
  }
}

void InputMethodOHOSManager::DeleteForwardOnUIThread(int32_t length) {
  if (active_instance_) {
    active_instance_->DeleteForward(length);
  }
}

void InputMethodOHOSManager::SendEnterKeyEventOnUIThread() {
  if (active_instance_) {
    active_instance_->SendEnterKeyEvent();
  }
}

void InputMethodOHOSManager::MoveCursorOnUIThread(int direction) {
  if (active_instance_) {
    active_instance_->MoveCursor(direction);
  }
}

void InputMethodOHOSManager::InsertText(const std::string& text) {
  task_runner_->PostTask(
    FROM_HERE,
    base::BindOnce(&InputMethodOHOSManager::InsertTextOnUIThread,
                   weak_factory_.GetWeakPtr(), text)
  );
}

void InputMethodOHOSManager::DeleteBackward(int32_t length) {
  task_runner_->PostTask(
    FROM_HERE,
    base::BindOnce(&InputMethodOHOSManager::DeleteBackwardOnUIThread,
                   weak_factory_.GetWeakPtr(), length)
  );
}

void InputMethodOHOSManager::DeleteForward(int32_t length) {
  task_runner_->PostTask(
    FROM_HERE,
    base::BindOnce(&InputMethodOHOSManager::DeleteForwardOnUIThread,
                   weak_factory_.GetWeakPtr(), length)
  );
}

void InputMethodOHOSManager::SendEnterKeyEvent() {
  task_runner_->PostTask(
    FROM_HERE,
    base::BindOnce(&InputMethodOHOSManager::SendEnterKeyEventOnUIThread,
                   weak_factory_.GetWeakPtr())
  );
}

void InputMethodOHOSManager::MoveCursor(int direction) {
  task_runner_->PostTask(
    FROM_HERE,
    base::BindOnce(&InputMethodOHOSManager::MoveCursorOnUIThread,
                   weak_factory_.GetWeakPtr(), direction)
  );
}

}  // namespace ui