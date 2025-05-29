// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
