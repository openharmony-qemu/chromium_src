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

#include "cancelable_delayed_task_manager.h"

#include "base/logging.h"
#include "base/task/thread_pool.h"

namespace viz {
CancelableDelayedTaskManager::CancelableDelayedTaskManager()
  : sequenced_task_runner_(base::ThreadPool::CreateSequencedTaskRunner(
    {base::TaskPriority::USER_VISIBLE,  // Ensure foreground thread execution
     base::MayBlock(),                  // Allow blocking if needed
     base::TaskShutdownBehavior::SKIP_ON_SHUTDOWN})) {}

CancelableDelayedTaskManager::~CancelableDelayedTaskManager()
{
  // Cancel any pending tasks during destruction
  CancelAllPendingTask();
}

void CancelableDelayedTaskManager::PostNewDelayedTask(uint64_t blankless_key,
                                                      base::OnceClosure task,
                                                      base::TimeDelta delay)
{
  if (!sequenced_task_runner_) {
    LOG(WARNING) << "blankless snapshot dump task runner is null.";
    return;
  }

  // Create a wrapped task with weak reference check
  auto wrapped_task = base::BindOnce(
    &CancelableDelayedTaskManager::ExecuteTaskWrapper,
    weak_factory_.GetWeakPtr(),
    blankless_key,
    std::move(task));

  {
    std::lock_guard<std::mutex> pending_tasks_guard(pending_tasks_mutex_);
    // Store task in cancelable closure
    auto it = pending_tasks_.find(blankless_key);
    if (it == pending_tasks_.end()) {
      pending_tasks_.emplace(blankless_key, std::move(wrapped_task));
    } else {
      it->second.Reset(std::move(wrapped_task));
    }

    // Post delayed task to thread pool
    sequenced_task_runner_->PostDelayedTask(
      FROM_HERE,
      pending_tasks_[blankless_key].callback(),  // Get cancelable callback
      delay);
  }
}

void CancelableDelayedTaskManager::PostNewRemoveDelayedTask(uint64_t blankless_key,
                                                      base::OnceClosure task,
                                                      base::TimeDelta delay)
{
  if (!sequenced_task_runner_) {
    LOG(WARNING) << "blankless remove task runner is null.";
    return;
  }

  // Create a wrapped task with weak reference check
  auto wrapped_task = base::BindOnce(
    &CancelableDelayedTaskManager::ExecuteRemoveTaskWrapper,
    weak_factory_.GetWeakPtr(),
    blankless_key,
    std::move(task));

  {
    std::lock_guard<std::mutex> remove_pending_tasks_guard(remove_pending_tasks_mutex_);
    // Store task in cancelable closure
    auto it = remove_pending_tasks_.find(blankless_key);
    if (it == remove_pending_tasks_.end()) {
      remove_pending_tasks_.emplace(blankless_key, std::move(wrapped_task));
    } else {
      it->second.Reset(std::move(wrapped_task));
    }

    // Post delayed task to thread pool
    sequenced_task_runner_->PostDelayedTask(
      FROM_HERE,
      remove_pending_tasks_[blankless_key].callback(),  // Get cancelable callback
      delay);
  }
}

void CancelableDelayedTaskManager::ExecuteTaskWrapper(uint64_t blankless_key, base::OnceClosure task)
{
  // Check if the manager is still valid
  if (!weak_factory_.HasWeakPtrs()) {
    LOG(WARNING) << "blankless Skipping task execution - manager destroyed";
    return;
  }

  // Clear pending task before execution
  CancelPendingTask(blankless_key);

  // Execute the actual task logic
  std::move(task).Run();
}

void CancelableDelayedTaskManager::CancelPendingTask(uint64_t blankless_key)
{
  std::lock_guard<std::mutex> pending_tasks_guard(pending_tasks_mutex_);
  auto it = pending_tasks_.find(blankless_key);
  if (it != pending_tasks_.end()) {
    it->second.Cancel();
    pending_tasks_.erase(it);
  }
}

void CancelableDelayedTaskManager::ExecuteRemoveTaskWrapper(uint64_t blankless_key, base::OnceClosure task)
{
  // Check if the manager is still valid
  if (!weak_factory_.HasWeakPtrs()) {
    LOG(WARNING) << "blankless remove task Skipping task execution - manager destroyed";
    return;
  }

  {
    std::lock_guard<std::mutex> remove_pending_tasks_guard(remove_pending_tasks_mutex_);
    auto it = remove_pending_tasks_.find(blankless_key);
    if (it != remove_pending_tasks_.end()) {
      it->second.Cancel();
      remove_pending_tasks_.erase(it);
    }
  }

  // Execute the actual task logic
  std::move(task).Run();
}

void CancelableDelayedTaskManager::CancelAllPendingTask()
{
  {
    std::lock_guard<std::mutex> pending_tasks_guard(pending_tasks_mutex_);
    for (auto it = pending_tasks_.begin(); it != pending_tasks_.end(); it++) {
      it->second.Cancel();
    }
    pending_tasks_.clear();
  }

  {
    std::lock_guard<std::mutex> remove_pending_tasks_guard(remove_pending_tasks_mutex_);
    for (auto it = remove_pending_tasks_.begin(); it != remove_pending_tasks_.end(); it++) {
      it->second.Cancel();
    }
    remove_pending_tasks_.clear();
  }
}
}  // namespace viz