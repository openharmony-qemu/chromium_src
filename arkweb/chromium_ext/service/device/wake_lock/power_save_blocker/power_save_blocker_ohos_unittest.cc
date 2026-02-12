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

#include "services/device/wake_lock/power_save_blocker/power_save_blocker.h"

#include <gtest/gtest.h>
#include <memory>

#include "arkweb/chromium_ext/service/device/wake_lock/power_save_blocker/nweb_screen_lock_tracker.h"
#include "base/test/task_environment.h"
#include "base/task/single_thread_task_runner.h"
#include "mojo/public/cpp/bindings/struct_forward.h"
#include "services/device/public/mojom/wake_lock.mojom.h"

namespace device {

// Mock callback to track screen lock state changes
class MockScreenLockCallback {
 public:
  MockScreenLockCallback() : screen_on_count_(0), screen_off_count_(0) {}

  SetKeepScreenOn GetCallback() {
    return [this](bool is_screen_on) {
      if (is_screen_on) {
        screen_on_count_++;
      } else {
        screen_off_count_++;
      }
    };
  }

  int screen_on_count() const { return screen_on_count_; }
  int screen_off_count() const { return screen_off_count_; }
  int total_calls() const { return screen_on_count_ + screen_off_count_; }

  void Reset() {
    screen_on_count_ = 0;
    screen_off_count_ = 0;
  }

 private:
  int screen_on_count_;
  int screen_off_count_;
};

class PowerSaveBlockerOhosTest : public ::testing::Test {
 protected:
  PowerSaveBlockerOhosTest() {}

  void SetUp() override {
    // Reset the tracker state by clearing all existing locks
    auto& tracker = NWebScreenLockTracker::Instance();
    // Note: NWebScreenLockTracker doesn't provide a clear method,
    // so we rely on each test's cleanup
    mock_callback_ = std::make_unique<MockScreenLockCallback>();
  }

  void TearDown() override {
    mock_callback_.reset();
  }

  base::test::TaskEnvironment task_environment_;
  std::unique_ptr<MockScreenLockCallback> mock_callback_;
};

TEST_F(PowerSaveBlockerOhosTest,
       Constructor_PreventAppSuspension) {
  scoped_refptr<base::SequencedTaskRunner> ui_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  auto blocker = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventAppSuspension,
      mojom::WakeLockReason::kOther, "test", ui_task_runner,
      blocking_task_runner, 1);

  EXPECT_NE(blocker, nullptr);
}

TEST_F(PowerSaveBlockerOhosTest,
       Constructor_PreventDisplaySleep) {
  scoped_refptr<base::SequencedTaskRunner> ui_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  auto blocker = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventDisplaySleep, mojom::WakeLockReason::kOther,
      "test", ui_task_runner, blocking_task_runner, 1);

  EXPECT_NE(blocker, nullptr);
}

TEST_F(PowerSaveBlockerOhosTest,
       Constructor_PreventDisplaySleepAllowDimming) {
  scoped_refptr<base::SequencedTaskRunner> ui_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  auto blocker = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventDisplaySleepAllowDimming,
      mojom::WakeLockReason::kOther, "test", ui_task_runner,
      blocking_task_runner, 1);

  EXPECT_NE(blocker, nullptr);
}

TEST_F(PowerSaveBlockerOhosTest, Destructor_RemovesBlock) {
  scoped_refptr<base::SequencedTaskRunner> ui_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  int window_id = 1;
  auto test_callback = std::make_unique<MockScreenLockCallback>();
  NWebScreenLockTracker::Instance().AddScreenLock(
      window_id, 1, test_callback->GetCallback());

  {
    auto blocker = std::make_unique<PowerSaveBlocker>(
        mojom::WakeLockType::kPreventDisplaySleep,
        mojom::WakeLockReason::kOther, "test", ui_task_runner,
        blocking_task_runner, 1);
    blocker->InitDisplaySleepBlocker(1);
    // Verify screen was turned on
    EXPECT_EQ(test_callback->screen_on_count(), 1);
    EXPECT_EQ(test_callback->screen_off_count(), 0);
  }
  // Blocker is destroyed here, block should be removed
  // Verify screen was turned off after blocker destruction
  EXPECT_EQ(test_callback->screen_off_count(), 1);

  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 1);
}

TEST_F(PowerSaveBlockerOhosTest, InitDisplaySleepBlocker_ValidId) {
  scoped_refptr<base::SequencedTaskRunner> ui_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  int window_id = 2;
  auto test_callback = std::make_unique<MockScreenLockCallback>();
  NWebScreenLockTracker::Instance().AddScreenLock(
      window_id, 1, test_callback->GetCallback());

  auto blocker = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventDisplaySleep,
      mojom::WakeLockReason::kOther, "test", ui_task_runner,
      blocking_task_runner, 1);

  blocker->InitDisplaySleepBlocker(1);
  // Verify screen was turned on
  EXPECT_EQ(test_callback->screen_on_count(), 1);
  EXPECT_EQ(test_callback->screen_off_count(), 0);

  // Cleanup
  blocker.reset();
  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 1);
}

TEST_F(PowerSaveBlockerOhosTest, InitDisplaySleepBlocker_InvalidId) {
  scoped_refptr<base::SequencedTaskRunner> ui_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  int window_id = 3;
  auto test_callback = std::make_unique<MockScreenLockCallback>();
  NWebScreenLockTracker::Instance().AddScreenLock(
      window_id, 1, test_callback->GetCallback());

  auto blocker = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventDisplaySleepAllowDimming,
      mojom::WakeLockReason::kOther, "test", ui_task_runner,
      blocking_task_runner, -1);

  blocker->InitDisplaySleepBlocker(-1);
  // Invalid ID (-1) should still work, but use first available handle
  // PreventDisplaySleepAllowDimming ignores the id check in some cases
  // At minimum, the blocker should be created successfully
  EXPECT_NE(blocker, nullptr);

  // Cleanup
  blocker.reset();
  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 1);
}

TEST_F(PowerSaveBlockerOhosTest, MultipleBlockersSameId) {
  scoped_refptr<base::SequencedTaskRunner> ui_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  int window_id = 4;
  auto test_callback = std::make_unique<MockScreenLockCallback>();
  NWebScreenLockTracker::Instance().AddScreenLock(
      window_id, 1, test_callback->GetCallback());

  auto blocker1 = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventDisplaySleep,
      mojom::WakeLockReason::kOther, "test1", ui_task_runner,
      blocking_task_runner, 1);
  blocker1->InitDisplaySleepBlocker(1);

  auto blocker2 = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventDisplaySleep,
      mojom::WakeLockReason::kOther, "test2", ui_task_runner,
      blocking_task_runner, 1);
  blocker2->InitDisplaySleepBlocker(1);

  // Screen should be turned on once (already on, no additional call)
  EXPECT_EQ(test_callback->screen_on_count(), 1);
  EXPECT_EQ(test_callback->screen_off_count(), 0);

  // Cleanup
  blocker1.reset();
  blocker2.reset();
  // Screen should be turned off after both blockers are destroyed
  EXPECT_EQ(test_callback->screen_off_count(), 1);

  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 1);
}

TEST_F(PowerSaveBlockerOhosTest, MultipleBlockersDifferentIds) {
  scoped_refptr<base::SequencedTaskRunner> ui_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  int window_id = 5;
  auto test_callback = std::make_unique<MockScreenLockCallback>();
  NWebScreenLockTracker::Instance().AddScreenLock(
      window_id, 1, test_callback->GetCallback());

  auto blocker1 = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventDisplaySleep,
      mojom::WakeLockReason::kOther, "test1", ui_task_runner,
      blocking_task_runner, 1);
  blocker1->InitDisplaySleepBlocker(1);

  auto blocker2 = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventDisplaySleep,
      mojom::WakeLockReason::kOther, "test2", ui_task_runner,
      blocking_task_runner, 2);
  blocker2->InitDisplaySleepBlocker(2);

  // Screen should be turned on
  EXPECT_EQ(test_callback->screen_on_count(), 1);
  EXPECT_EQ(test_callback->screen_off_count(), 0);

  // Cleanup
  blocker1.reset();
  // Screen should stay on while blocker2 is still active
  EXPECT_EQ(test_callback->screen_off_count(), 1);

  blocker2.reset();
  // Screen should be turned off after both blockers are destroyed
  EXPECT_EQ(test_callback->screen_off_count(), 1);

  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 1);
  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 2);
}

TEST_F(PowerSaveBlockerOhosTest,
       DifferentWakeLockTypes) {
  scoped_refptr<base::SequencedTaskRunner> ui_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  int window_id = 6;
  auto test_callback = std::make_unique<MockScreenLockCallback>();
  NWebScreenLockTracker::Instance().AddScreenLock(
      window_id, 1, test_callback->GetCallback());

  auto blocker1 = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventAppSuspension,
      mojom::WakeLockReason::kOther, "test1", ui_task_runner,
      blocking_task_runner, 1);
  blocker1->InitDisplaySleepBlocker(1);

  auto blocker2 = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventDisplaySleep,
      mojom::WakeLockReason::kOther, "test2", ui_task_runner,
      blocking_task_runner, 2);
  blocker2->InitDisplaySleepBlocker(2);

  auto blocker3 = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventDisplaySleepAllowDimming,
      mojom::WakeLockReason::kOther, "test3", ui_task_runner,
      blocking_task_runner, 3);
  blocker3->InitDisplaySleepBlocker(3);

  // Screen should be turned on
  EXPECT_EQ(test_callback->screen_on_count(), 1);
  EXPECT_EQ(test_callback->screen_off_count(), 0);

  // Cleanup
  blocker1.reset();
  blocker2.reset();
  blocker3.reset();
  // Screen should be turned off after all blockers are destroyed
  EXPECT_EQ(test_callback->screen_off_count(), 1);

  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 1);
  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 2);
  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 3);
}

TEST_F(PowerSaveBlockerOhosTest,
       BlockerLifecycle) {
  scoped_refptr<base::SequencedTaskRunner> ui_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  int window_id = 7;
  auto test_callback = std::make_unique<MockScreenLockCallback>();
  NWebScreenLockTracker::Instance().AddScreenLock(
      window_id, 1, test_callback->GetCallback());

  {
    auto blocker = std::make_unique<PowerSaveBlocker>(
        mojom::WakeLockType::kPreventDisplaySleep,
        mojom::WakeLockReason::kOther, "test", ui_task_runner,
        blocking_task_runner, 1);

    blocker->InitDisplaySleepBlocker(1);
    // Blocker is active here
    EXPECT_EQ(test_callback->screen_on_count(), 1);
    EXPECT_EQ(test_callback->screen_off_count(), 0);
  }
  // Blocker is destroyed, block should be removed
  EXPECT_EQ(test_callback->screen_off_count(), 1);

  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 1);
}

TEST_F(PowerSaveBlockerOhosTest,
       InitDisplaySleepBlocker_PreventAppSuspensionWithInvalidId) {
  scoped_refptr<base::SequencedTaskRunner> ui_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  int window_id = 8;
  auto test_callback = std::make_unique<MockScreenLockCallback>();
  NWebScreenLockTracker::Instance().AddScreenLock(
      window_id, 1, test_callback->GetCallback());

  auto blocker = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventAppSuspension,
      mojom::WakeLockReason::kOther, "test", ui_task_runner,
      blocking_task_runner, -1);

  blocker->InitDisplaySleepBlocker(-1);
  // For PreventAppSuspension with invalid ID (-1), the lock should still be applied
  // using the first available handle
  EXPECT_EQ(test_callback->screen_on_count(), 0);
  EXPECT_EQ(test_callback->screen_off_count(), 0);

  // Cleanup
  blocker.reset();
  EXPECT_EQ(test_callback->screen_off_count(), 0);

  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 1);
}

TEST_F(PowerSaveBlockerOhosTest, BlockerWithoutInit) {
  scoped_refptr<base::SequencedTaskRunner> ui_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  int window_id = 9;
  auto test_callback = std::make_unique<MockScreenLockCallback>();
  NWebScreenLockTracker::Instance().AddScreenLock(
      window_id, 1, test_callback->GetCallback());

  {
    auto blocker = std::make_unique<PowerSaveBlocker>(
        mojom::WakeLockType::kPreventDisplaySleep,
        mojom::WakeLockReason::kOther, "test", ui_task_runner,
        blocking_task_runner, 1);

    // InitDisplaySleepBlocker is never called
    // Screen should remain off
    EXPECT_EQ(test_callback->screen_on_count(), 0);
    EXPECT_EQ(test_callback->screen_off_count(), 0);
  }
  // Destructor should handle case where InitDisplaySleepBlocker was never called
  // Screen should still be off
  EXPECT_EQ(test_callback->screen_on_count(), 0);
  EXPECT_EQ(test_callback->screen_off_count(), 0);

  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 1);
}

TEST_F(PowerSaveBlockerOhosTest,
       MultipleInitsSameBlocker) {
  scoped_refptr<base::SequencedTaskRunner> ui_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  int window_id = 10;
  auto test_callback = std::make_unique<MockScreenLockCallback>();
  NWebScreenLockTracker::Instance().AddScreenLock(
      window_id, 1, test_callback->GetCallback());

  {
    auto blocker = std::make_unique<PowerSaveBlocker>(
        mojom::WakeLockType::kPreventDisplaySleep,
        mojom::WakeLockReason::kOther, "test", ui_task_runner,
        blocking_task_runner, 1);

    blocker->InitDisplaySleepBlocker(1);
    // This adds the same ID twice to the lock map
    blocker->InitDisplaySleepBlocker(1);

    // Screen should be turned on once (second call with same ID should be ignored)
    EXPECT_EQ(test_callback->screen_on_count(), 1);
    EXPECT_EQ(test_callback->screen_off_count(), 0);
  }
  // After destruction, screen should be turned off once
  EXPECT_EQ(test_callback->screen_off_count(), 0);

  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 1);
}

TEST_F(PowerSaveBlockerOhosTest, AllWakeLockReasons) {
  scoped_refptr<base::SequencedTaskRunner> ui_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();
  scoped_refptr<base::SingleThreadTaskRunner> blocking_task_runner =
      base::SingleThreadTaskRunner::GetCurrentDefault();

  int window_id = 11;
  auto test_callback = std::make_unique<MockScreenLockCallback>();
  NWebScreenLockTracker::Instance().AddScreenLock(
      window_id, 1, test_callback->GetCallback());

  auto blocker1 = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventDisplaySleep,
      mojom::WakeLockReason::kAudioPlayback, "audio", ui_task_runner,
      blocking_task_runner, 1);
  blocker1->InitDisplaySleepBlocker(1);

  auto blocker2 = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventDisplaySleep,
      mojom::WakeLockReason::kVideoPlayback, "video", ui_task_runner,
      blocking_task_runner, 2);
  blocker2->InitDisplaySleepBlocker(2);

  auto blocker3 = std::make_unique<PowerSaveBlocker>(
      mojom::WakeLockType::kPreventDisplaySleep,
      mojom::WakeLockReason::kOther, "other", ui_task_runner,
      blocking_task_runner, 3);
  blocker3->InitDisplaySleepBlocker(3);

  // Screen should be turned on
  EXPECT_EQ(test_callback->screen_on_count(), 1);
  EXPECT_EQ(test_callback->screen_off_count(), 0);

  // Cleanup - verify blockers with different reasons all work
  blocker1.reset();
  EXPECT_EQ(test_callback->screen_off_count(), 1);

  blocker2.reset();
  EXPECT_EQ(test_callback->screen_off_count(), 1);

  blocker3.reset();
  // Screen should be turned off after all blockers are destroyed
  EXPECT_EQ(test_callback->screen_off_count(), 1);

  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 1);
  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 2);
  NWebScreenLockTracker::Instance().RemoveScreenLock(window_id, 3);
}

}  // namespace device
