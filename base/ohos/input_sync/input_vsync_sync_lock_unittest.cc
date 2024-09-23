/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#define private public
#include "input_vsync_sync_lock.h"
#undef private

#include "testing/gtest/include/gtest/gtest.h"

namespace base {
namespace ohos {

class InputSyncLockTest : public testing::Test {
 public:
  void SetUp() override {
    InputSyncLock::GetInstance().SetNeedWaitForInput(false);
    InputSyncLock::GetInstance().SetHandledTouchEvent(false);
  }
};

TEST_F(InputSyncLockTest, InputSyncLockTest_GetInstanceTest001) {
  InputSyncLock& instance1 = InputSyncLock::GetInstance();
  InputSyncLock& instance2 = InputSyncLock::GetInstance();
  EXPECT_EQ(&instance1, &instance2);
}

TEST_F(InputSyncLockTest, SetNeedWaitForInput_NoChange) {
  InputSyncLock& instance = InputSyncLock::GetInstance();

  instance.SetNeedWaitForInput(false);
  EXPECT_FALSE(instance.NeedWaitForInput());

  instance.SetNeedWaitForInput(false);
  EXPECT_FALSE(instance.NeedWaitForInput());
}

TEST_F(InputSyncLockTest, SetNeedWaitForInput_Change) {
  InputSyncLock& instance = InputSyncLock::GetInstance();

  instance.SetNeedWaitForInput(false);
  EXPECT_FALSE(instance.NeedWaitForInput());

  instance.SetNeedWaitForInput(true);
  EXPECT_TRUE(instance.NeedWaitForInput());
}

TEST_F(InputSyncLockTest, SetAndHandledTouchEventTest) {
  InputSyncLock& instance = InputSyncLock::GetInstance();

  EXPECT_FALSE(instance.HandledTouchEvent());

  instance.SetHandledTouchEvent(true);
  EXPECT_TRUE(instance.HandledTouchEvent());

  instance.SetHandledTouchEvent(false);
  EXPECT_FALSE(instance.HandledTouchEvent());
}

TEST_F(InputSyncLockTest, HandledTouchEventTest) {
  InputSyncLock& instance = InputSyncLock::GetInstance();

  EXPECT_FALSE(instance.HandledTouchEvent());

  instance.SetHandledTouchEvent(true);
  EXPECT_TRUE(instance.HandledTouchEvent());

  instance.SetHandledTouchEvent(false);
  EXPECT_FALSE(instance.HandledTouchEvent());
}
}  // namespace ohos
}  // namespace base
