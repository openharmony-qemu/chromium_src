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

#include "ohos_sdk/openharmony/native/llvm/bin/../include/libcxx-ohos/include/c++/v1/__ranges/lazy_split_view.h"
#define private public
#include "../include/d_vsync_controller.h"
#undef private
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "base/test/task_environment.h"
#include "base/test/test_simple_task_runner.h"

namespace base {
namespace ohos {
class DVsyncControllerTest : public ::testing::Test
{
protected:
  void SetUp() override {}
  void TearDown() override {}

  DVsyncController& controller = DVsyncController::GetInstance();
};

TEST_F(DVsyncControllerTest, SetIsFlingImpl) {
  controller.SetIsFlingImpl(true);
  EXPECT_TRUE(controller.is_fling_);

  controller.SetIsFlingImpl(false);
  EXPECT_FALSE(controller.is_fling_);
}

TEST_F(DVsyncControllerTest, GetIsFling) {
  controller.is_fling_ = true;
  EXPECT_TRUE(controller.GetIsFling());

  controller.is_fling_ = false;
  EXPECT_FALSE(controller.GetIsFling());
}

TEST_F(DVsyncControllerTest, Init001) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  controller.Init();
  EXPECT_NE(controller.curent_task_runner_, nullptr);
}

TEST_F(DVsyncControllerTest, Init002) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  controller.curent_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  ASSERT_NO_FATAL_FAILURE(controller.Init());
}

TEST_F(DVsyncControllerTest, SetIsFling001) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  controller.SetIsFling(true);
  EXPECT_TRUE(controller.curent_task_runner_);
}

TEST_F(DVsyncControllerTest, SetIsFling002) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  controller.curent_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  testing::internal::CaptureStderr();
  controller.SetIsFling(false);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("curent_task_runner_ is nullptr, try to start init"), std::string::npos);
  EXPECT_NE(log_output.find("DVsyncController::SetIsFling:"), std::string::npos);
}

}  // namespace ohos
}  // namespace base