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

#ifndef BASE_OHOS_TASK_SCHEDULER_TASK_RUNNER_OHOS_H_
#define BASE_OHOS_TASK_SCHEDULER_TASK_RUNNER_OHOS_H_

#include <memory>

#include "base/ohos/task_scheduler/task_traits_ohos.h"
#include "base/base_export.h"
#include "base/functional/callback_forward.h"
#include "base/task/single_thread_task_runner.h"

namespace base {

class BASE_EXPORT TaskRunnerOHOS {
 public:
  explicit TaskRunnerOHOS(scoped_refptr<TaskRunner> task_runner);

  TaskRunnerOHOS(const TaskRunnerOHOS&) = delete;
  TaskRunnerOHOS& operator=(const TaskRunnerOHOS&) = delete;

  ~TaskRunnerOHOS();

  void Destroy();

  void PostDelayedTask(const Location& from_here, OnceClosure task, int delay);

  void PostTask(const Location& from_here, OnceClosure task);

  bool BelongsToCurrentThread();

  static std::unique_ptr<TaskRunnerOHOS> Create(int task_traits = ::TaskTraits::UI_DEFAULT);

  using UiThreadTaskRunnerCallback =
      RepeatingCallback<scoped_refptr<base::SingleThreadTaskRunner>(
          ::TaskTraits)>;

  static void SetUiThreadTaskRunnerCallback(
      UiThreadTaskRunnerCallback callback);

  static scoped_refptr<base::SingleThreadTaskRunner> GetUIThreadTaskRunner(
      int task_traits = ::TaskTraits::UI_DEFAULT);

 private:
  const scoped_refptr<TaskRunner> task_runner_;
};

}  // namespace base

#endif  // BASE_OHOS_TASK_SCHEDULER_TASK_RUNNER_OHOS_H_
