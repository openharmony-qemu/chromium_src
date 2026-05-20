// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/viz/client/frame_eviction_manager.h"

#include <vector>

#include "base/memory/memory_pressure_listener.h"
#include "base/memory/memory_pressure_monitor.h"
#include "base/memory/raw_ptr.h"
#include "base/ranges/algorithm.h"
#include "base/test/test_mock_time_task_runner.h"
#include "base/trace_event/memory_dump_request_args.h"
#include "base/trace_event/process_memory_dump.h"
#include "testing/gtest/include/gtest/gtest.h"
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
#include "components/viz/common/viz_utils.h"
#endif

namespace viz {

namespace {

class TestFrameEvictionManagerClient : public FrameEvictionManagerClient {
 public:
  TestFrameEvictionManagerClient() = default;
  explicit TestFrameEvictionManagerClient(FrameEvictionManager* manager)
      : manager_(manager) {}

  TestFrameEvictionManagerClient(const TestFrameEvictionManagerClient&) =
      delete;
  TestFrameEvictionManagerClient& operator=(
      const TestFrameEvictionManagerClient&) = delete;

  ~TestFrameEvictionManagerClient() override {
    if (has_frame_)
      manager_->RemoveFrame(this);
  }

#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  void EvictCurrentFrame(bool isProcessMemoryPressure) override {
#else
  void EvictCurrentFrame() override {
#endif
    manager_->RemoveFrame(this);
    has_frame_ = false;
  }

  bool has_frame() const { return has_frame_; }

 private:
  raw_ptr<FrameEvictionManager> manager_ = FrameEvictionManager::GetInstance();
  bool has_frame_ = true;
};

#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
class TestFrameEvictionManagerClientWithEvictControl
    : public FrameEvictionManagerClient {
 public:
  TestFrameEvictionManagerClientWithEvictControl() = default;
  explicit TestFrameEvictionManagerClientWithEvictControl(
      FrameEvictionManager* manager)
      : manager_(manager) {}

  TestFrameEvictionManagerClientWithEvictControl(
      const TestFrameEvictionManagerClientWithEvictControl&) = delete;
  TestFrameEvictionManagerClientWithEvictControl& operator=(
      const TestFrameEvictionManagerClientWithEvictControl&) = delete;

  ~TestFrameEvictionManagerClientWithEvictControl() override {
    if (has_frame_)
      manager_->RemoveFrame(this);
  }

  void EvictCurrentFrame(bool isProcessMemoryPressure) override {
    if (should_remove_self_) {
      manager_->RemoveFrame(this);
      has_frame_ = false;
    }
  }

  bool has_frame() const { return has_frame_; }
  void set_should_remove_self(bool should_remove) {
    should_remove_self_ = should_remove;
  }

 private:
  raw_ptr<FrameEvictionManager> manager_ = FrameEvictionManager::GetInstance();
  bool has_frame_ = true;
  bool should_remove_self_ = true;
};
#endif

class FakeMemoryPressureMonitor : public base::MemoryPressureMonitor {
 public:
  explicit FakeMemoryPressureMonitor(MemoryPressureLevel level)
      : level_(level) {}

  MemoryPressureLevel GetCurrentPressureLevel() const override {
    return level_;
  }

  void SetPressureLevel(MemoryPressureLevel level) { level_ = level; }

 private:
  MemoryPressureLevel level_ =
      base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_NONE;
};

}  // namespace

class FrameEvictionManagerTest : public testing::Test {};

TEST_F(FrameEvictionManagerTest, ScopedPause) {
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  int kMaxSavedFrames = 0;
  if (IsEvictUnlockFrameEnabled()) {
    kMaxSavedFrames = 0;
  } else {
    kMaxSavedFrames = 1;
  }
#else
  constexpr int kMaxSavedFrames = 1;
#endif
  constexpr int kFrames = 2;

  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(kMaxSavedFrames);

  std::vector<TestFrameEvictionManagerClient> frames(kFrames);
  {
    FrameEvictionManager::ScopedPause scoped_pause;

    for (auto& frame : frames)
      manager->AddFrame(&frame, /*locked=*/false);

    EXPECT_EQ(kFrames, base::ranges::count_if(
                           frames, &TestFrameEvictionManagerClient::has_frame));
  }

  EXPECT_EQ(kMaxSavedFrames,
            base::ranges::count_if(frames,
                                   &TestFrameEvictionManagerClient::has_frame));
}

TEST_F(FrameEvictionManagerTest, PeriodicCulling) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);

  TestFrameEvictionManagerClient frame1;
  TestFrameEvictionManagerClient frame2;
  TestFrameEvictionManagerClient frame3;
  manager->AddFrame(&frame1, false);
  manager->AddFrame(&frame2, true);
  manager->AddFrame(&frame3, false);

  EXPECT_TRUE(frame1.has_frame());
  EXPECT_TRUE(frame2.has_frame());
  EXPECT_TRUE(frame3.has_frame());

  manager->UnlockFrame(&frame2);
  EXPECT_TRUE(frame2.has_frame());

  {
    FrameEvictionManager::ScopedPause scoped_pause;
  }

  manager->RemoveFrame(&frame2);
  manager->RemoveFrame(&frame3);
}

TEST_F(FrameEvictionManagerTest, MemoryPressure) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();

  manager->set_max_number_of_saved_frames(5);
  TestFrameEvictionManagerClient frame1;
  TestFrameEvictionManagerClient frame2;
  manager->AddFrame(&frame1, false);
  manager->AddFrame(&frame2, false);

  manager->OnMemoryPressure(
      base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_CRITICAL);
  EXPECT_FALSE(frame1.has_frame());
  EXPECT_FALSE(frame2.has_frame());
}

// Tests that AddFrame with locked=true adds frames to the locked_frames_ map.
// Locked frames are protected from eviction and should remain even when
// exceeding the max saved frames limit. Both frames should remain valid.
TEST_F(FrameEvictionManagerTest, AddFrameWithLockedTrue) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(1);

  TestFrameEvictionManagerClient frame1;
  TestFrameEvictionManagerClient frame2;
  manager->AddFrame(&frame1, true);
  manager->AddFrame(&frame2, true);

  EXPECT_TRUE(frame1.has_frame());
  EXPECT_TRUE(frame2.has_frame());

  manager->RemoveFrame(&frame1);
  manager->RemoveFrame(&frame2);
}

// Tests that AddFrame with locked=false triggers culling when exceeding the
// max saved frames limit. The oldest unlocked frame (frame1) should be evicted
// when frame2 is added, demonstrating LRU eviction behavior for unlocked frames.
TEST_F(FrameEvictionManagerTest, AddFrameWithLockedFalseTriggersCulling) {
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  int kMaxSavedFrames = 0;
  if (IsEvictUnlockFrameEnabled()) {
    kMaxSavedFrames = 0;
  } else {
    kMaxSavedFrames = 1;
  }
#else
  constexpr int kMaxSavedFrames = 1;
#endif

  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(kMaxSavedFrames);

  TestFrameEvictionManagerClient frame1;
  TestFrameEvictionManagerClient frame2;
  manager->AddFrame(&frame1, false);
  EXPECT_TRUE(frame1.has_frame());

  manager->AddFrame(&frame2, false);
  EXPECT_FALSE(frame1.has_frame());
  EXPECT_TRUE(frame2.has_frame());
}

// Tests that RemoveFrame correctly removes a frame from the locked_frames_ map.
// After removal, the same frame can be added again as unlocked. The frame should
// transition from locked to unlocked state correctly.
TEST_F(FrameEvictionManagerTest, RemoveFrameFromLockedFrames) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);

  TestFrameEvictionManagerClient frame;
  manager->AddFrame(&frame, true);
  EXPECT_TRUE(frame.has_frame());

  manager->RemoveFrame(&frame);
  manager->AddFrame(&frame, false);

  EXPECT_TRUE(frame.has_frame());
  manager->RemoveFrame(&frame);
}

// Tests that RemoveFrame correctly removes a frame from the unlocked_frames_ map.
// After removal, the same frame can be added again as locked. The frame should
// transition from unlocked to locked state correctly.
TEST_F(FrameEvictionManagerTest, RemoveFrameFromUnlockedFrames) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);

  TestFrameEvictionManagerClient frame;
  manager->AddFrame(&frame, false);
  EXPECT_TRUE(frame.has_frame());

  manager->RemoveFrame(&frame);
  manager->AddFrame(&frame, true);

  EXPECT_TRUE(frame.has_frame());
  manager->RemoveFrame(&frame);
}

// Tests that LockFrame moves a frame from unlocked_frames_ to locked_frames_.
// Calling LockFrame on an unlocked frame should protect it from eviction.
// The frame should remain valid after locking.
TEST_F(FrameEvictionManagerTest, LockFrameFromUnlocked) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);

  TestFrameEvictionManagerClient frame;
  manager->AddFrame(&frame, false);
  EXPECT_TRUE(frame.has_frame());

  manager->LockFrame(&frame);

  manager->AddFrame(&frame, false);

  EXPECT_TRUE(frame.has_frame());
  manager->RemoveFrame(&frame);
}

// Tests that LockFrame supports nested locking (multiple locks on same frame).
// Each LockFrame call should increment the lock count. The frame should remain
// protected until all locks are released with corresponding UnlockFrame calls.
TEST_F(FrameEvictionManagerTest, LockFrameNested) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);

  TestFrameEvictionManagerClient frame;
  manager->AddFrame(&frame, true);
  EXPECT_TRUE(frame.has_frame());

  manager->LockFrame(&frame);
  manager->LockFrame(&frame);

  manager->RemoveFrame(&frame);
}

// Tests that UnlockFrame decrements the lock count for nested locked frames.
// UnlockFrame should not evict the frame until the lock count reaches zero.
// The frame should remain valid after each unlock when count > 0.
TEST_F(FrameEvictionManagerTest, UnlockFrameDecrementCount) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);

  TestFrameEvictionManagerClient frame;
  manager->AddFrame(&frame, true);
  manager->LockFrame(&frame);
  manager->LockFrame(&frame);

  manager->UnlockFrame(&frame);

  EXPECT_TRUE(frame.has_frame());

  manager->UnlockFrame(&frame);
  EXPECT_TRUE(frame.has_frame());

  manager->RemoveFrame(&frame);
}

// Tests that UnlockFrame moves a frame from locked_frames_ to unlocked_frames_
// when the lock count reaches zero. The frame becomes eligible for eviction
// after unlocking, but should not be immediately evicted without exceeding limits.
TEST_F(FrameEvictionManagerTest, UnlockFrameMovesToUnlocked) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);

  TestFrameEvictionManagerClient frame;
  manager->AddFrame(&frame, true);
  EXPECT_TRUE(frame.has_frame());

  manager->UnlockFrame(&frame);

  EXPECT_TRUE(frame.has_frame());

  manager->RemoveFrame(&frame);
}

// Tests that UnlockFrame triggers culling when max saved frames is exceeded.
// When multiple locked frames are unlocked simultaneously, culling should evict
// frames to respect the limit. When eviction is enabled, all frames may be evicted.
TEST_F(FrameEvictionManagerTest, UnlockFrameTriggersCulling) {
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  int kMaxSavedFrames = 0;
  if (IsEvictUnlockFrameEnabled()) {
    kMaxSavedFrames = 0;
  } else {
    kMaxSavedFrames = 1;
  }
#else
  constexpr int kMaxSavedFrames = 1;
#endif

  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(kMaxSavedFrames);

  TestFrameEvictionManagerClient frame1;
  TestFrameEvictionManagerClient frame2;
  manager->AddFrame(&frame1, true);
  manager->AddFrame(&frame2, true);

  EXPECT_TRUE(frame1.has_frame());
  EXPECT_TRUE(frame2.has_frame());

  manager->UnlockFrame(&frame1);
  manager->UnlockFrame(&frame2);

#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  if (IsEvictUnlockFrameEnabled()) {
    EXPECT_FALSE(frame1.has_frame());
    EXPECT_FALSE(frame2.has_frame());
  } else {
    EXPECT_TRUE(frame1.has_frame() || frame2.has_frame());
  }
#else
  EXPECT_TRUE(frame1.has_frame() || frame2.has_frame());
#endif
}

// Tests that GetMaxNumberOfSavedFrames returns the configured value when no
// memory pressure monitor is present. Without pressure monitoring, the limit
// should reflect the explicitly set max_number_of_saved_frames value.
TEST_F(FrameEvictionManagerTest, GetMaxNumberOfSavedFramesNoMonitor) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(10);

  size_t result = manager->GetMaxNumberOfSavedFrames();
  EXPECT_EQ(10u, result);
}

// Tests that GetMaxNumberOfSavedFrames returns the configured value when memory
// pressure level is NONE. At no pressure, no reduction in saved frames should
// occur; the limit should equal the explicitly set value.
TEST_F(FrameEvictionManagerTest, GetMaxNumberOfSavedFramesWithNonePressure) {
  FakeMemoryPressureMonitor monitor(
      base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_NONE);
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(10);

  size_t result = manager->GetMaxNumberOfSavedFrames();
  EXPECT_EQ(10u, result);
}

// Tests that GetMaxNumberOfSavedFrames returns half the configured value under
// MODERATE memory pressure. Moderate pressure should reduce the saved frames
// limit to conserve memory (e.g., 10 -> 5).
TEST_F(FrameEvictionManagerTest, GetMaxNumberOfSavedFramesWithModeratePressure) {
  FakeMemoryPressureMonitor monitor(
      base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_MODERATE);
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(10);

  size_t result = manager->GetMaxNumberOfSavedFrames();
  EXPECT_EQ(5u, result);
}

// Tests that GetMaxNumberOfSavedFrames returns 1 under CRITICAL memory pressure.
// Critical pressure should aggressively reduce saved frames to minimum (1)
// to free memory, regardless of the configured limit.
TEST_F(FrameEvictionManagerTest, GetMaxNumberOfSavedFramesWithCriticalPressure) {
  FakeMemoryPressureMonitor monitor(
      base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_CRITICAL);
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(10);

  size_t result = manager->GetMaxNumberOfSavedFrames();
  EXPECT_EQ(1u, result);
}

// Tests that GetMaxNumberOfSavedFrames enforces a minimum of 1 even under
// critical pressure when the configured limit is already 1. The minimum value
// ensures at least one frame can be saved regardless of pressure conditions.
TEST_F(FrameEvictionManagerTest, GetMaxNumberOfSavedFramesMinimumOne) {
  FakeMemoryPressureMonitor monitor(
      base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_CRITICAL);
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(1);

  size_t result = manager->GetMaxNumberOfSavedFrames();
  EXPECT_EQ(1u, result);
}

// Tests that PurgeAllUnlockedFrames does not immediately evict frames when
// the manager is paused. During pause, eviction operations are deferred.
// After unpause, previously scheduled evictions may still occur.
TEST_F(FrameEvictionManagerTest, CullUnlockedFramesWhenPaused) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(1);

  TestFrameEvictionManagerClient frame1;
  TestFrameEvictionManagerClient frame2;
  manager->AddFrame(&frame1, false);
  manager->AddFrame(&frame2, false);

  {
    FrameEvictionManager::ScopedPause scoped_pause;
    manager->PurgeAllUnlockedFrames();
    EXPECT_TRUE(frame1.has_frame() || frame2.has_frame());
  }

  EXPECT_FALSE(frame1.has_frame() || frame2.has_frame());
}

// Tests that culling handles clients that refuse to remove themselves during
// eviction. When a client's EvictCurrentFrame doesn't remove the frame, the
// manager should not crash and other frames should still be processed correctly.
TEST_F(FrameEvictionManagerTest, CullUnlockedFramesFrameDoesNotRemove) {
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  int kMaxSavedFrames = 0;
  if (IsEvictUnlockFrameEnabled()) {
    kMaxSavedFrames = 0;
  } else {
    kMaxSavedFrames = 1;
  }
#else
  constexpr int kMaxSavedFrames = 1;
#endif

  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(kMaxSavedFrames);

  TestFrameEvictionManagerClientWithEvictControl frame1;
  frame1.set_should_remove_self(false);
  TestFrameEvictionManagerClient frame2;

  manager->AddFrame(&frame1, false);
  manager->AddFrame(&frame2, false);

  EXPECT_TRUE(frame1.has_frame());
  EXPECT_TRUE(frame2.has_frame());
}

// Tests that OnMemoryPressure with MODERATE level evicts frames to reduce count
// to half the limit. When 10 frames exist and moderate pressure is signaled,
// exactly 5 frames should remain after pressure-induced eviction.
TEST_F(FrameEvictionManagerTest, OnMemoryPressureModerate) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(10);

  TestFrameEvictionManagerClient frames[10];
  for (int i = 0; i < 10; ++i) {
    manager->AddFrame(&frames[i], false);
  }

  manager->OnMemoryPressure(
      base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_MODERATE);

  int frames_remaining = 0;
  for (int i = 0; i < 10; ++i) {
    if (frames[i].has_frame()) {
        frames_remaining++;
    }
  }
  EXPECT_EQ(5, frames_remaining);
}

// Tests that OnMemoryPressure with NONE level does not evict any frames.
// Zero pressure should preserve all existing frames without triggering culling.
TEST_F(FrameEvictionManagerTest, OnMemoryPressureNone) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);

  TestFrameEvictionManagerClient frame1;
  TestFrameEvictionManagerClient frame2;
  manager->AddFrame(&frame1, false);
  manager->AddFrame(&frame2, false);

  manager->OnMemoryPressure(
      base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_NONE);

  EXPECT_TRUE(frame1.has_frame());
  EXPECT_TRUE(frame2.has_frame());
}

// Tests that OnMemoryPressure with MODERATE level does not evict when limit is
// already 1. When the saved frames limit equals the minimum (1), moderate pressure
// cannot further reduce the count; the single frame should remain.
TEST_F(FrameEvictionManagerTest, PurgeMemoryWhenLimitIsOne) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(1);

  TestFrameEvictionManagerClient frame;
  manager->AddFrame(&frame, false);

  manager->OnMemoryPressure(
      base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_MODERATE);

  EXPECT_TRUE(frame.has_frame());
}

// Tests that nested Pause calls require corresponding Unpause calls to resume
// eviction. Each Pause increments a pause count; eviction is deferred until
// all pauses are released. The frame should survive until final Unpause.
TEST_F(FrameEvictionManagerTest, NestedPause) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);

  TestFrameEvictionManagerClient frame;
  manager->AddFrame(&frame, false);

  {
    FrameEvictionManager::ScopedPause scoped_pause1;
    {
      FrameEvictionManager::ScopedPause scoped_pause2;
      manager->PurgeAllUnlockedFrames();
      EXPECT_TRUE(frame.has_frame());
    }
    EXPECT_TRUE(frame.has_frame());
  }
  EXPECT_FALSE(frame.has_frame());
}

// Tests that Unpause processes pending purge operations scheduled during pause.
// When PurgeAllUnlockedFrames is called while paused, the eviction executes
// after Unpause, resulting in the frame being evicted.
TEST_F(FrameEvictionManagerTest, UnpauseWithPendingLimit) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);

  TestFrameEvictionManagerClient frame;
  manager->AddFrame(&frame, false);

  {
    FrameEvictionManager::ScopedPause scoped_pause;
    manager->PurgeAllUnlockedFrames();
  }

  EXPECT_FALSE(frame.has_frame());
}

// Tests that OnMemoryDump creates memory dump entries for frame tracking.
// Memory dump should include allocator dumps for frame_evictor showing
// locked and unlocked frame counts. Returns true for successful dump creation.
// Note: This test is disabled because OnMemoryDump is a private method.
// It can be tested indirectly through MemoryDumpManager::RequestGlobalMemoryDump.
TEST_F(FrameEvictionManagerTest, DISABLED_OnMemoryDump) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);

  TestFrameEvictionManagerClient frame1;
  TestFrameEvictionManagerClient frame2;
  manager->AddFrame(&frame1, true);
  manager->AddFrame(&frame2, false);
  manager->RemoveFrame(&frame1);
  manager->RemoveFrame(&frame2);
}

#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
// Tests that when IsEvictUnlockFrameEnabled is true, newly added unlocked frames
// may be immediately evicted based on memory limits. This verifies the aggressive
// eviction behavior for the ARKWEB_EVICT_UNLOCK_FRAMES feature.
TEST_F(FrameEvictionManagerTest, ProcessCullUnlockedFramesWhenEnabled) {
  if (!IsEvictUnlockFrameEnabled()) {
    GTEST_SKIP() << "Test only runs when IsEvictUnlockFrameEnabled is true";
  }

  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);

  TestFrameEvictionManagerClient frame1;
  TestFrameEvictionManagerClient frame2;
  TestFrameEvictionManagerClient frame3;
  manager->AddFrame(&frame1, false);
  manager->AddFrame(&frame2, false);
  manager->AddFrame(&frame3, false);

  EXPECT_TRUE(frame1.has_frame() || frame2.has_frame() || frame3.has_frame());
}

// Tests that when IsEvictUnlockFrameEnabled is false, standard LRU culling
// applies based on max saved frames limit. With limit of 1, only one frame
// should remain after adding multiple unlocked frames.
TEST_F(FrameEvictionManagerTest, ProcessCullUnlockedFramesWhenDisabled) {
  if (IsEvictUnlockFrameEnabled()) {
    GTEST_SKIP() << "Test only runs when IsEvictUnlockFrameEnabled is false";
  }

  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(1);

  TestFrameEvictionManagerClient frame1;
  TestFrameEvictionManagerClient frame2;
  manager->AddFrame(&frame1, false);
  manager->AddFrame(&frame2, false);

  int frames_remaining = 0;
  if (frame1.has_frame()) frames_remaining++;
  if (frame2.has_frame()) frames_remaining++;
  EXPECT_EQ(1, frames_remaining);
}

// Tests that EvictCurrentFrame receives the memory pressure parameter when
// ARKWEB_EVICT_UNLOCK_FRAMES is enabled. Critical memory pressure should
// evict frames with the isProcessMemoryPressure flag set appropriately.
TEST_F(FrameEvictionManagerTest, EvictCurrentFrameWithMemoryPressureParam) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);

  TestFrameEvictionManagerClient frame;
  manager->AddFrame(&frame, false);

  manager->OnMemoryPressure(
      base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_CRITICAL);

  EXPECT_FALSE(frame.has_frame());
}
#endif

// Tests that multiple LockFrame and UnlockFrame calls correctly manage the
// lock count. Four locks followed by four unlocks should leave the frame
// unlocked but still valid, demonstrating proper count tracking.
TEST_F(FrameEvictionManagerTest, MultipleLocksAndUnlocks) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);

  TestFrameEvictionManagerClient frame;
  manager->AddFrame(&frame, true);
  manager->LockFrame(&frame);
  manager->LockFrame(&frame);
  manager->LockFrame(&frame);

  manager->UnlockFrame(&frame);
  EXPECT_TRUE(frame.has_frame());

  manager->UnlockFrame(&frame);
  EXPECT_TRUE(frame.has_frame());

  manager->UnlockFrame(&frame);
  EXPECT_TRUE(frame.has_frame());

  manager->UnlockFrame(&frame);
  EXPECT_TRUE(frame.has_frame());

  manager->RemoveFrame(&frame);
}

// Tests that AddFrame removes an existing entry before adding the new one.
// When the same frame is added twice (first unlocked, then locked), the
// second add should update the frame's state from unlocked to locked.
TEST_F(FrameEvictionManagerTest, AddFrameRemovesExisting) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);

  TestFrameEvictionManagerClient frame;
  manager->AddFrame(&frame, false);
  EXPECT_TRUE(frame.has_frame());

  manager->AddFrame(&frame, true);

  EXPECT_TRUE(frame.has_frame());

  manager->RemoveFrame(&frame);
}

// Tests that locked frames are protected from eviction during culling.
// When adding a locked frame followed by an unlocked frame that exceeds limit,
// the locked frame should survive while the unlocked frame may be evicted.
TEST_F(FrameEvictionManagerTest, LockedFrameNotEvictedOnCulling) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(1);

  TestFrameEvictionManagerClient locked_frame;
  TestFrameEvictionManagerClient unlocked_frame;
  manager->AddFrame(&locked_frame, true);
  manager->AddFrame(&unlocked_frame, false);

  EXPECT_TRUE(locked_frame.has_frame());

#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  if (!IsEvictUnlockFrameEnabled()) {
    EXPECT_FALSE(unlocked_frame.has_frame());
  }
#else
  EXPECT_TRUE(unlocked_frame.has_frame());
#endif

  manager->RemoveFrame(&locked_frame);
}

// Tests that PurgeAllUnlockedFrames evicts all unlocked frames while preserving
// locked frames. Unlocked frames should be evicted immediately, but locked
// frames remain protected until explicitly unlocked.
TEST_F(FrameEvictionManagerTest, PurgeAllUnlockedFrames) {
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(10);

  TestFrameEvictionManagerClient frame1;
  TestFrameEvictionManagerClient frame2;
  TestFrameEvictionManagerClient locked_frame;
  manager->AddFrame(&frame1, false);
  manager->AddFrame(&frame2, false);
  manager->AddFrame(&locked_frame, true);

  manager->PurgeAllUnlockedFrames();

  EXPECT_FALSE(frame1.has_frame());
  EXPECT_FALSE(frame2.has_frame());
  EXPECT_TRUE(locked_frame.has_frame());

  manager->RemoveFrame(&locked_frame);
}

}  // namespace viz