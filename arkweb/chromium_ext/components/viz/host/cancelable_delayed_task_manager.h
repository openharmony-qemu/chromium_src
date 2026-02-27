/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef COMPONENTS_VIZ_HOST_CANCELABLE_DELAYED_TASK_MANAGER_H_
#define COMPONENTS_VIZ_HOST_CANCELABLE_DELAYED_TASK_MANAGER_H_

#include <map>
#include <mutex>
#include "base/cancelable_callback.h"
#include "base/functional/callback_forward.h"
#include "base/memory/weak_ptr.h"
#include "base/memory/scoped_refptr.h"
#include "base/task/single_thread_task_runner.h"
#include "base/time/time.h"

namespace viz {
// Manages delayed tasks that can be canceled when new tasks arrive
class CancelableDelayedTaskManager {
public:
  CancelableDelayedTaskManager();
  ~CancelableDelayedTaskManager();

  // Posts a new delayed task, automatically canceling any previous pending task by the blankless_key
  void PostNewDelayedTask(uint64_t blankless_key, base::OnceClosure task, base::TimeDelta delay);

  // Posts a new remove delayed task, automatically canceling any previous pending task by the blankless_key
  void PostNewRemoveDelayedTask(uint64_t blankless_key, base::OnceClosure task, base::TimeDelta delay);

private:
  // Wrapper that executes the actual task after safety checks
  void ExecuteTaskWrapper(uint64_t blankless_key, base::OnceClosure task);

  // Wrapper that executes the actual task after safety checks
  void ExecuteRemoveTaskWrapper(uint64_t blankless_key, base::OnceClosure task);

  // Explicitly cancels the current pending task by the blankless_key
  void CancelPendingTask(uint64_t blankless_key);

  // Explicitly cancels all the current pending task
  void CancelAllPendingTask();

  std::mutex pending_tasks_mutex_;

  std::mutex remove_pending_tasks_mutex_;

  // Sequenced task runner for thread pool
  scoped_refptr<base::SequencedTaskRunner> sequenced_task_runner_;

  // Holds current cancelable tasks
  std::map<uint64_t, base::CancelableOnceClosure> pending_tasks_;

  // Holds current remove cancelable tasks
  std::map<uint64_t, base::CancelableOnceClosure> remove_pending_tasks_;

  // Weak pointer factory (MUST be the last member)
  base::WeakPtrFactory<CancelableDelayedTaskManager> weak_factory_{this};
};
}  // namespace viz

#endif  // COMPONENTS_VIZ_HOST_CANCELABLE_DELAYED_TASK_MANAGER_H_