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

#ifndef UI_BASE_IME_INPUT_METHOD_OHOS_MANAGER_H_
#define UI_BASE_IME_INPUT_METHOD_OHOS_MANAGER_H_

#include "ui/base/ime/ohos/input_method_ohos.h"
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "base/task/thread_pool.h"

namespace ui {
class COMPONENT_EXPORT(UI_BASE_IME_OHOS) InputMethodOHOSManager {
 public:
  static InputMethodOHOSManager& GetInstance();

  void SetActiveInstance(base::WeakPtr<InputMethodOHOS> instance);
  bool IsActiveInstance(const base::WeakPtr<InputMethodOHOS>& instance);
  bool ReleaseActiveInstance(const base::WeakPtr<InputMethodOHOS>& instance);

  scoped_refptr<base::SingleThreadTaskRunner>& GetTaskRunner();

 private:
  base::WeakPtr<InputMethodOHOS> active_instance_;
  std::once_flag task_runner_flag_;
  std::once_flag sequenced_task_runner_flag_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
};
}  // namespace ui
#endif  // UI_BASE_IME_INPUT_METHOD_OHOS_MANAGER_H_
