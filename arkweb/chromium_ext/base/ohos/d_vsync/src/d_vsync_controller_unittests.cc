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

}  // namespace ohos
}  // namespace base