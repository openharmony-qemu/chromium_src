// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
