/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "nweb_screen_lock_tracker.h"

#include <gtest/gtest.h>
#include <memory>

namespace {

class NWebScreenLockTrackerTest : public ::testing::Test {
 protected:
  NWebScreenLockTrackerTest() : screen_on_count_(0), screen_off_count_(0) {}

  void SetUp() override {
    screen_on_count_ = 0;
    screen_off_count_ = 0;
  }

  SetKeepScreenOn CreateHandle() {
    return [this](bool is_on) {
      if (is_on) {
        screen_on_count_++;
      } else {
        screen_off_count_++;
      }
    };
  }

  void ResetCounts() {
    screen_on_count_ = 0;
    screen_off_count_ = 0;
  }

  int screen_on_count_;
  int screen_off_count_;
};

TEST_F(NWebScreenLockTrackerTest, SetKeepScreenLockHandle_Constructor) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  EXPECT_FALSE(handle.IsEmpty());
}

TEST_F(NWebScreenLockTrackerTest, SetKeepScreenLockHandle_AddScreenLockHandle) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  handle.AddScreenLockHandle(2, CreateHandle());
  EXPECT_FALSE(handle.IsEmpty());
}

TEST_F(NWebScreenLockTrackerTest,
       SetKeepScreenLockHandle_AddDuplicateScreenLockHandle) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  handle.AddScreenLockHandle(1, CreateHandle());
  EXPECT_FALSE(handle.IsEmpty());
}

TEST_F(NWebScreenLockTrackerTest, SetKeepScreenLockHandle_RemoveScreenLockHandle) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  handle.RemoveScreenLockHandle(1);
  EXPECT_TRUE(handle.IsEmpty());
}

TEST_F(NWebScreenLockTrackerTest,
       SetKeepScreenLockHandle_RemoveNonExistentHandle) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  handle.RemoveScreenLockHandle(999);
  EXPECT_FALSE(handle.IsEmpty());
}

TEST_F(NWebScreenLockTrackerTest, SetKeepScreenLockHandle_Lock) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  bool result = handle.Lock(1);
  EXPECT_TRUE(result);
  EXPECT_EQ(screen_on_count_, 1);
}

TEST_F(NWebScreenLockTrackerTest, SetKeepScreenLockHandle_LockInvalidId) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  bool result = handle.Lock(-1);
  EXPECT_FALSE(result);
  EXPECT_EQ(screen_on_count_, 1);
}

TEST_F(NWebScreenLockTrackerTest,
       SetKeepScreenLockHandle_LockNonExistentId) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  bool result = handle.Lock(999);
  EXPECT_FALSE(result);
  EXPECT_EQ(screen_on_count_, 0);
}

TEST_F(NWebScreenLockTrackerTest, SetKeepScreenLockHandle_UnLock) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  handle.Lock(1);
  ResetCounts();
  bool result = handle.UnLock(1);
  EXPECT_TRUE(result);
  EXPECT_EQ(screen_off_count_, 1);
}

TEST_F(NWebScreenLockTrackerTest, SetKeepScreenLockHandle_UnLockInvalidId) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  handle.Lock(-1);
  ResetCounts();
  bool result = handle.UnLock(-1);
  EXPECT_FALSE(result);
  EXPECT_EQ(screen_off_count_, 1);
}

TEST_F(NWebScreenLockTrackerTest,
       SetKeepScreenLockHandle_UnLockNonExistentId) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  bool result = handle.UnLock(999);
  EXPECT_FALSE(result);
  EXPECT_EQ(screen_off_count_, 0);
}

TEST_F(NWebScreenLockTrackerTest,
       SetKeepScreenLockHandle_MultipleLocksSameId) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  handle.Lock(1);
  handle.Lock(1);
  handle.Lock(1);
  EXPECT_EQ(screen_on_count_, 1);  // Screen should only turn on once
}

TEST_F(NWebScreenLockTrackerTest,
       SetKeepScreenLockHandle_MultipleUnlocksSameId) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  handle.Lock(1);
  handle.Lock(1);
  handle.Lock(1);
  ResetCounts();
  handle.UnLock(1);
  EXPECT_EQ(screen_off_count_, 0);  // Screen stays on
  handle.UnLock(1);
  EXPECT_EQ(screen_off_count_, 0);  // Screen still on
  handle.UnLock(1);
  EXPECT_EQ(screen_off_count_, 1);  // Screen turns off
}

TEST_F(NWebScreenLockTrackerTest, SetKeepScreenLockHandle_Empty) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  handle.RemoveScreenLockHandle(1);
  EXPECT_TRUE(handle.IsEmpty());
}

TEST_F(NWebScreenLockTrackerTest, NWebScreenLockTracker_Singleton) {
  NWebScreenLockTracker& tracker1 = NWebScreenLockTracker::Instance();
  NWebScreenLockTracker& tracker2 = NWebScreenLockTracker::Instance();
  EXPECT_EQ(&tracker1, &tracker2);
}

TEST_F(NWebScreenLockTrackerTest, NWebScreenLockTracker_AddScreenLock) {
  NWebScreenLockTracker& tracker = NWebScreenLockTracker::Instance();
  tracker.AddScreenLock(1, 1, CreateHandle());
}

TEST_F(NWebScreenLockTrackerTest,
       NWebScreenLockTracker_AddScreenLockSameWindow) {
  NWebScreenLockTracker& tracker = NWebScreenLockTracker::Instance();
  tracker.AddScreenLock(1, 1, CreateHandle());
  tracker.AddScreenLock(1, 2, CreateHandle());
}

TEST_F(NWebScreenLockTrackerTest,
       NWebScreenLockTracker_AddScreenLockDifferentWindows) {
  NWebScreenLockTracker& tracker = NWebScreenLockTracker::Instance();
  tracker.AddScreenLock(1, 1, CreateHandle());
  tracker.AddScreenLock(2, 1, CreateHandle());
}

TEST_F(NWebScreenLockTrackerTest, NWebScreenLockTracker_RemoveScreenLock) {
  NWebScreenLockTracker& tracker = NWebScreenLockTracker::Instance();
  tracker.AddScreenLock(1, 1, CreateHandle());
  tracker.RemoveScreenLock(1, 1);
}

TEST_F(NWebScreenLockTrackerTest,
       NWebScreenLockTracker_RemoveNonExistentScreenLock) {
  NWebScreenLockTracker& tracker = NWebScreenLockTracker::Instance();
  tracker.RemoveScreenLock(999, 999);
}

TEST_F(NWebScreenLockTrackerTest,
       NWebScreenLockTracker_RemoveScreenLockFromNonExistentWindow) {
  NWebScreenLockTracker& tracker = NWebScreenLockTracker::Instance();
  tracker.AddScreenLock(1, 1, CreateHandle());
  tracker.RemoveScreenLock(999, 1);
}

TEST_F(NWebScreenLockTrackerTest, NWebScreenLockTracker_Lock) {
  NWebScreenLockTracker& tracker = NWebScreenLockTracker::Instance();
  tracker.AddScreenLock(1, 1, CreateHandle());
  tracker.Lock(1);
  EXPECT_EQ(screen_on_count_, 1);
}

TEST_F(NWebScreenLockTrackerTest, NWebScreenLockTracker_UnLock) {
  NWebScreenLockTracker& tracker = NWebScreenLockTracker::Instance();
  tracker.AddScreenLock(1, 1, CreateHandle());
  tracker.Lock(1);
  ResetCounts();
  tracker.UnLock(1);
  EXPECT_EQ(screen_off_count_, 1);
}

TEST_F(NWebScreenLockTrackerTest, NWebScreenLockTracker_LockInvalidId) {
  NWebScreenLockTracker& tracker = NWebScreenLockTracker::Instance();
  tracker.AddScreenLock(1, 1, CreateHandle());
  tracker.Lock(-1);
  EXPECT_EQ(screen_on_count_, 1);
}

TEST_F(NWebScreenLockTrackerTest, NWebScreenLockTracker_UnLockInvalidId) {
  NWebScreenLockTracker& tracker = NWebScreenLockTracker::Instance();
  tracker.AddScreenLock(1, 1, CreateHandle());
  tracker.Lock(-1);
  ResetCounts();
  tracker.UnLock(-1);
  EXPECT_EQ(screen_off_count_, 1);
}

TEST_F(NWebScreenLockTrackerTest,
       NWebScreenLockTracker_MultipleWindowsSameId) {
  NWebScreenLockTracker& tracker = NWebScreenLockTracker::Instance();
  tracker.AddScreenLock(1, 1, CreateHandle());
  tracker.AddScreenLock(2, 1, CreateHandle());
  tracker.Lock(1);
  // Lock should be applied to first matching window
}

TEST_F(NWebScreenLockTrackerTest,
       SetKeepScreenLockHandle_RemoveWithScreenOn) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  handle.Lock(1);
  ResetCounts();
  handle.RemoveScreenLockHandle(1);
  EXPECT_EQ(screen_off_count_, 1);
}

TEST_F(NWebScreenLockTrackerTest,
       SetKeepScreenLockHandle_RemoveWithoutScreenOn) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  ResetCounts();
  handle.RemoveScreenLockHandle(1);
  EXPECT_EQ(screen_off_count_, 0);
}

TEST_F(NWebScreenLockTrackerTest,
       SetKeepScreenLockHandle_UnlockWhenAlreadyUnlocked) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  bool result = handle.UnLock(1);
  EXPECT_FALSE(result);
}

TEST_F(NWebScreenLockTrackerTest,
       SetKeepScreenLockHandle_LockWhenAlreadyLocked) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  handle.Lock(1);
  ResetCounts();
  handle.Lock(1);
  EXPECT_EQ(screen_on_count_, 0);
}

TEST_F(NWebScreenLockTrackerTest,
       SetKeepScreenLockHandle_UnlockMoreThanLocked) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  handle.Lock(1);
  handle.UnLock(1);
  ResetCounts();
  bool result = handle.UnLock(1);
  EXPECT_FALSE(result);
  EXPECT_EQ(screen_off_count_, 0);
}

TEST_F(NWebScreenLockTrackerTest,
       SetKeepScreenLockHandle_InvalidIdMultipleLocks) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  handle.Lock(-1);
  ResetCounts();
  handle.Lock(-1);
  EXPECT_EQ(screen_on_count_, 0);
}

TEST_F(NWebScreenLockTrackerTest,
       SetKeepScreenLockHandle_InvalidIdMultipleUnlocks) {
  SetKeepScreenLockHandle handle(1, CreateHandle());
  handle.Lock(-1);
  handle.Lock(-1);
  ResetCounts();
  handle.UnLock(-1);
  EXPECT_EQ(screen_off_count_, 0);  // Screen still on
  handle.UnLock(-1);
  EXPECT_EQ(screen_off_count_, 1);  // Screen turns off
}

}  // namespace
