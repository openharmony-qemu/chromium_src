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

#include "ui/base/ime/ohos/input_method_ohos_manager.h"

namespace ui {
InputMethodOHOSManager& InputMethodOHOSManager::GetInstance() {
  static InputMethodOHOSManager instance;
  return instance;
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

}  // namespace ui
