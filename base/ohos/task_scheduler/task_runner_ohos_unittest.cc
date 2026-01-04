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
 
#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "base/threading/thread.h"
#include "base/memory/scoped_refptr.h"
#include "base/run_loop.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "base/test/gtest_util.h"
#include "base/test/task_environment.h"
#include "base/test/test_simple_task_runner.h"
#include <memory>
#include "testing/gtest/include/gtest/gtest.h"

class TaskRunnerOHOSTest : public ::testing::Test {
protected:
  std::unique_ptr<base::Thread> ui_thread;
  void SetUp() override {
    ui_thread = std::make_unique<base::Thread>("UI_Thread");
    ASSERT_TRUE(ui_thread->Start());
    scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner = ui_thread->task_runner();
    base::TaskRunnerOHOS::SetUiThreadTaskRunnerCallback(
      base::BindRepeating(
        [](scoped_refptr<base::SingleThreadTaskRunner> task_runner,
          ::TaskTraits traits) -> scoped_refptr<base::SingleThreadTaskRunner> {
            if (traits == ::TaskTraits::UI_DEFAULT || traits == ::TaskTraits::UI_USER_VISIBLE) {
              return task_runner;
            }
            return nullptr;
          },
          std::move(ui_task_runner)
      )
    );
  }

  void TearDown() override {
    if (ui_thread) {
      ui_thread->Stop();
      ui_thread.reset();
    }
  }
};

TEST_F(TaskRunnerOHOSTest, TaskRunnerOHOSTest001) {
  auto task_runner_ohos = base::TaskRunnerOHOS::Create(::TaskTraits::UI_DEFAULT);
  EXPECT_TRUE(task_runner_ohos != nullptr);
}

TEST_F(TaskRunnerOHOSTest, TaskRunnerOHOSTest002) {
  auto task_runner_ohos = base::TaskRunnerOHOS::Create(::TaskTraits::UI_DEFAULT);

  scoped_refptr<base::SingleThreadTaskRunner> runner =
      base::TaskRunnerOHOS::GetUIThreadTaskRunner(::TaskTraits::UI_DEFAULT);
  EXPECT_TRUE(runner.get() != nullptr);
}

TEST_F(TaskRunnerOHOSTest, TaskRunnerOHOSTest003) {
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_1(
      base::MakeRefCounted<base::TestSimpleTaskRunner>());
  base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  auto task_runner_ohos = base::TaskRunnerOHOS::Create(::TaskTraits::UI_DEFAULT);

  scoped_refptr<base::SingleThreadTaskRunner> runner =
      base::TaskRunnerOHOS::GetUIThreadTaskRunner(::TaskTraits::UI_DEFAULT);
  EXPECT_EQ(runner.get(), base::SingleThreadTaskRunner::GetCurrentDefault());
}

TEST_F(TaskRunnerOHOSTest, TaskRunnerOHOSTest004) {
  auto task_runner_ohos = base::TaskRunnerOHOS::Create(::TaskTraits::UI_USER_VISIBLE);

  scoped_refptr<base::SingleThreadTaskRunner> runner =
      base::TaskRunnerOHOS::GetUIThreadTaskRunner(::TaskTraits::UI_USER_VISIBLE);
  EXPECT_TRUE(runner.get() != nullptr);
}

TEST_F(TaskRunnerOHOSTest, TaskRunnerOHOSTest005) {
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_1(
      base::MakeRefCounted<base::TestSimpleTaskRunner>());
  base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  auto task_runner_ohos = base::TaskRunnerOHOS::Create(::TaskTraits::UI_USER_VISIBLE);

  scoped_refptr<base::SingleThreadTaskRunner> runner =
      base::TaskRunnerOHOS::GetUIThreadTaskRunner(::TaskTraits::UI_USER_VISIBLE);
  EXPECT_TRUE(runner.get() != nullptr);
}