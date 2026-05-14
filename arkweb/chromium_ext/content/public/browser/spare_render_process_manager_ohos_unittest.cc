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
 
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
 
#include "arkweb/build/features/features.h"
 
#if BUILDFLAG(ARKWEB_WARMUP_SPARE_RENDER_PROCESS)
 
#include "arkweb/chromium_ext/content/public/browser/spare_render_process_manager_ohos.h"
#include "base/run_loop.h"
#include "content/public/test/browser_task_environment.h"
 
using namespace testing;
 
namespace OHOS::NWeb {
 
const pid_t TEST_SPARE_PID = 12345;
const pid_t TEST_OTHER_PID = 54321;
const int32_t TEST_WINDOW_ID = 42;
 
class SpareRenderProcessManagerOhosTest : public ::testing::Test {
 protected:
  SpareRenderProcessManagerOhosTest() {}
 
  void SetUp() override {
    original_enabled_ = SpareRenderProcessManagerOhos::IsWarmupEnabled();
  }
 
  void TearDown() override {
    SpareRenderProcessManagerOhos::SetWarmupEnabled(original_enabled_);
    base::RunLoop().RunUntilIdle();
  }
 
  bool original_enabled_ = false;
};
 
// ---------- Static enable flag tests ----------
 
TEST_F(SpareRenderProcessManagerOhosTest, SetWarmupEnabled_True) {
  SpareRenderProcessManagerOhos::SetWarmupEnabled(true);
  EXPECT_TRUE(SpareRenderProcessManagerOhos::IsWarmupEnabled());
}
 
TEST_F(SpareRenderProcessManagerOhosTest, SetWarmupEnabled_False) {
  SpareRenderProcessManagerOhos::SetWarmupEnabled(false);
  EXPECT_FALSE(SpareRenderProcessManagerOhos::IsWarmupEnabled());
}
 
TEST_F(SpareRenderProcessManagerOhosTest, SetWarmupEnabled_Toggle) {
  SpareRenderProcessManagerOhos::SetWarmupEnabled(true);
  EXPECT_TRUE(SpareRenderProcessManagerOhos::IsWarmupEnabled());
  SpareRenderProcessManagerOhos::SetWarmupEnabled(false);
  EXPECT_FALSE(SpareRenderProcessManagerOhos::IsWarmupEnabled());
  SpareRenderProcessManagerOhos::SetWarmupEnabled(true);
  EXPECT_TRUE(SpareRenderProcessManagerOhos::IsWarmupEnabled());
}
 
// ---------- Window ID tests ----------
 
TEST_F(SpareRenderProcessManagerOhosTest, SetWarmupWindowId_PositiveValue) {
  auto& manager = SpareRenderProcessManagerOhos::GetInstance();
  manager.SetWarmupWindowId(TEST_WINDOW_ID);
  EXPECT_EQ(manager.GetWarmupWindowId(), TEST_WINDOW_ID);
}
 
TEST_F(SpareRenderProcessManagerOhosTest, SetWarmupWindowId_NegativeValue) {
  auto& manager = SpareRenderProcessManagerOhos::GetInstance();
  manager.SetWarmupWindowId(-1);
  EXPECT_EQ(manager.GetWarmupWindowId(), -1);
}
 
TEST_F(SpareRenderProcessManagerOhosTest, SetWarmupWindowId_Zero) {
  auto& manager = SpareRenderProcessManagerOhos::GetInstance();
  manager.SetWarmupWindowId(0);
  EXPECT_EQ(manager.GetWarmupWindowId(), 0);
}
 
// ---------- Singleton test ----------
 
TEST_F(SpareRenderProcessManagerOhosTest, GetInstance_ReturnsSameSingleton) {
  auto& instance1 = SpareRenderProcessManagerOhos::GetInstance();
  auto& instance2 = SpareRenderProcessManagerOhos::GetInstance();
  EXPECT_EQ(&instance1, &instance2);
}
 
// ---------- Constants test ----------
 
TEST_F(SpareRenderProcessManagerOhosTest, Constants_AreExpectedValues) {
  EXPECT_EQ(WARMUP_SPARE_RENDER_REPORT_NWEB_ID, 0);
  EXPECT_EQ(kSpareRenderTimeout, base::Seconds(2));
}
 
// ---------- State machine lifecycle test ----------
// Tests the complete state machine: IDLE -> ACTIVE -> RELEASED
// plus guard conditions for invalid state transitions.
 
TEST_F(SpareRenderProcessManagerOhosTest, StateMachine_FullLifecycle) {
  auto& manager = SpareRenderProcessManagerOhos::GetInstance();
 
  // Phase 1: Test IDLE state (only if still in initial state)
  if (manager.GetState() == SpareRenderProcessManagerOhos::State::IDLE) {
    EXPECT_EQ(manager.GetSpareRenderPid(), -1);
    EXPECT_FALSE(manager.IsSpareRenderProcess(TEST_SPARE_PID));
    EXPECT_FALSE(manager.IsSpareRenderProcess(0));
    EXPECT_FALSE(manager.IsSpareRenderProcess(-1));
 
    // Transition: IDLE -> ACTIVE
    manager.NotifyWarmupSpareLaunched(TEST_SPARE_PID);
    EXPECT_EQ(manager.GetState(), SpareRenderProcessManagerOhos::State::ACTIVE);
    EXPECT_EQ(manager.GetSpareRenderPid(), TEST_SPARE_PID);
  }
 
  // Phase 2: Test ACTIVE state
  if (manager.GetState() == SpareRenderProcessManagerOhos::State::ACTIVE) {
    // IsSpareRenderProcess returns true for matching pid
    EXPECT_TRUE(manager.IsSpareRenderProcess(TEST_SPARE_PID));
 
    // IsSpareRenderProcess returns false for non-matching pid
    EXPECT_FALSE(manager.IsSpareRenderProcess(TEST_OTHER_PID));
 
    // IsSpareRenderProcess returns false for invalid pid values
    EXPECT_FALSE(manager.IsSpareRenderProcess(-1));
    EXPECT_FALSE(manager.IsSpareRenderProcess(0));
 
    // OnSpareRenderProcessHostReady when ACTIVE should not change state
    EXPECT_NO_FATAL_FAILURE(manager.OnSpareRenderProcessHostReady(nullptr));
    EXPECT_EQ(manager.GetState(), SpareRenderProcessManagerOhos::State::ACTIVE);
 
    // Transition: ACTIVE -> RELEASED via OnSpareTaken
    manager.OnSpareTaken();
    EXPECT_EQ(manager.GetState(), SpareRenderProcessManagerOhos::State::RELEASED);
  }
 
  // Phase 3: Test RELEASED state and guard conditions
  if (manager.GetState() == SpareRenderProcessManagerOhos::State::RELEASED) {
    // IsSpareRenderProcess should return false even for previously valid pid
    EXPECT_FALSE(manager.IsSpareRenderProcess(TEST_SPARE_PID));
 
    // NotifyWarmupSpareLaunched should be no-op when not IDLE
    pid_t pid_before = manager.GetSpareRenderPid();
    manager.NotifyWarmupSpareLaunched(TEST_OTHER_PID);
    EXPECT_EQ(manager.GetState(), SpareRenderProcessManagerOhos::State::RELEASED);
    EXPECT_EQ(manager.GetSpareRenderPid(), pid_before);
 
    // OnSpareRenderProcessHostReady should be no-op when not ACTIVE/INACTIVE
    EXPECT_NO_FATAL_FAILURE(manager.OnSpareRenderProcessHostReady(nullptr));
    EXPECT_EQ(manager.GetState(), SpareRenderProcessManagerOhos::State::RELEASED);
 
    // OnSpareRenderProcessHostRemoved should be no-op when RELEASED
    EXPECT_NO_FATAL_FAILURE(manager.OnSpareRenderProcessHostRemoved(nullptr));
    EXPECT_EQ(manager.GetState(), SpareRenderProcessManagerOhos::State::RELEASED);
 
    // OnSpareTaken when RELEASED should not change state
    manager.OnSpareTaken();
    EXPECT_EQ(manager.GetState(), SpareRenderProcessManagerOhos::State::RELEASED);
  }
}
 
// ---------- OnSpareTaken before NotifyWarmupSpareLaunched ----------
// Tests that OnSpareTaken can be called safely even when no spare was launched.
 
TEST_F(SpareRenderProcessManagerOhosTest, OnSpareTaken_WhenIdle_NoStateChange) {
  auto& manager = SpareRenderProcessManagerOhos::GetInstance();
  if (manager.GetState() != SpareRenderProcessManagerOhos::State::IDLE) {
    // State already changed, cannot test IDLE guard
    return;
  }
 
  auto state_before = manager.GetState();
  // OnSpareTaken sets spare_render_taken_ = true, but state stays IDLE
  // since IDLE is not ACTIVE or INACTIVE
  EXPECT_NO_FATAL_FAILURE(manager.OnSpareTaken());
  EXPECT_EQ(manager.GetState(), state_before);
}
 
// ---------- OnSpareRenderProcessHostReady in wrong states ----------
 
TEST_F(SpareRenderProcessManagerOhosTest,
       OnSpareRenderProcessHostReady_WhenIdle_NoOp) {
  auto& manager = SpareRenderProcessManagerOhos::GetInstance();
  if (manager.GetState() != SpareRenderProcessManagerOhos::State::IDLE) {
    return;
  }
  auto state_before = manager.GetState();
  EXPECT_NO_FATAL_FAILURE(manager.OnSpareRenderProcessHostReady(nullptr));
  EXPECT_EQ(manager.GetState(), state_before);
}
 
// ---------- OnSpareRenderProcessHostRemoved in wrong states ----------
 
TEST_F(SpareRenderProcessManagerOhosTest,
       OnSpareRenderProcessHostRemoved_WhenIdle_NoOp) {
  auto& manager = SpareRenderProcessManagerOhos::GetInstance();
  if (manager.GetState() != SpareRenderProcessManagerOhos::State::IDLE) {
    return;
  }
  auto state_before = manager.GetState();
  EXPECT_NO_FATAL_FAILURE(manager.OnSpareRenderProcessHostRemoved(nullptr));
  EXPECT_EQ(manager.GetState(), state_before);
}
 
}  // namespace OHOS::NWeb
 
#endif  // BUILDFLAG(ARKWEB_WARMUP_SPARE_RENDER_PROCESS)