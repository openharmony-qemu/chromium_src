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

#include "base/ohos/task_scheduler/task_runner_ohos.h"

#include "base/check.h"
#include "base/compiler_specific.h"
#include "base/functional/bind.h"
#include "base/no_destructor.h"
#include "base/time/time.h"

namespace base {

namespace {

TaskRunnerOHOS::UiThreadTaskRunnerCallback& GetUiThreadTaskRunnerCallback() {
  static base::NoDestructor<TaskRunnerOHOS::UiThreadTaskRunnerCallback>
      callback;
  return *callback;
}

}  // namespace

long Init(int task_traits) {
  TaskRunnerOHOS* task_runner =
      TaskRunnerOHOS::Create(task_traits).release();
  return reinterpret_cast<intptr_t>(task_runner);
}

TaskRunnerOHOS::TaskRunnerOHOS(scoped_refptr<TaskRunner> task_runner)
    : task_runner_(std::move(task_runner)) {}

TaskRunnerOHOS::~TaskRunnerOHOS() = default;

void TaskRunnerOHOS::Destroy() {
  delete this;
}

void TaskRunnerOHOS::PostDelayedTask(const Location& from_here, OnceClosure task, int delay) {
  task_runner_->PostDelayedTask(
      from_here,
      std::move(task),
      Milliseconds(delay));
}

void TaskRunnerOHOS::PostTask(const Location& from_here, OnceClosure task) {
  PostDelayedTask(from_here, std::move(task), 0);
}

bool TaskRunnerOHOS::BelongsToCurrentThread() {
  return static_cast<SequencedTaskRunner*>(task_runner_.get())
      ->RunsTasksInCurrentSequence();
}

// static
std::unique_ptr<TaskRunnerOHOS> TaskRunnerOHOS::Create(int task_traits) {
  CHECK(GetUiThreadTaskRunnerCallback());
  scoped_refptr<TaskRunner> task_runner = GetUiThreadTaskRunnerCallback().Run(
      static_cast<::TaskTraits>(task_traits));

  return std::make_unique<TaskRunnerOHOS>(task_runner);
}

// static
void TaskRunnerOHOS::SetUiThreadTaskRunnerCallback(
    UiThreadTaskRunnerCallback callback) {
  GetUiThreadTaskRunnerCallback() = std::move(callback);
}

// static
scoped_refptr<base::SingleThreadTaskRunner>
TaskRunnerOHOS::GetUIThreadTaskRunner(int task_traits) {
  if (task_traits == ::TaskTraits::UI_DEFAULT &&
      base::SingleThreadTaskRunner::HasCurrentDefault()) {
    return base::SingleThreadTaskRunner::GetCurrentDefault();
  }
  CHECK(GetUiThreadTaskRunnerCallback());
  return GetUiThreadTaskRunnerCallback().Run(static_cast<::TaskTraits>(task_traits));
}

}  // namespace base
