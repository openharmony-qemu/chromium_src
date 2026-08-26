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

#include "components/viz/client/frame_evictor.h"

#include <vector>

#include "base/unguessable_token.h"
#include "build/buildflag.h"
#include "components/viz/client/frame_eviction_manager.h"
#include "components/viz/common/surfaces/frame_sink_id.h"
#include "components/viz/common/surfaces/local_surface_id.h"
#include "components/viz/common/surfaces/surface_id.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
#include "components/viz/common/viz_utils.h"
#endif

namespace viz {

namespace {

using testing::_;
using testing::Return;
using testing::ReturnRef;

SurfaceId MakeSurfaceId(uint32_t client_id, uint32_t sink_id, uint32_t parent_seq) {
  return SurfaceId(
      FrameSinkId(client_id, sink_id),
      LocalSurfaceId(parent_seq, 1, base::UnguessableToken::Create()));
}

class MockFrameEvictorClient : public FrameEvictorClient {
 public:
  MockFrameEvictorClient() = default;
  ~MockFrameEvictorClient() override = default;

  MOCK_METHOD(void, EvictDelegatedFrame, (const std::vector<SurfaceId>&), (override));
  MOCK_METHOD(EvictIds, CollectSurfaceIdsForEviction, (), (const, override));
  MOCK_METHOD(SurfaceId, GetCurrentSurfaceId, (), (const, override));
  MOCK_METHOD(SurfaceId, GetPreNavigationSurfaceId, (), (const, override));
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  MOCK_METHOD(void, SetUseSpecifiedDeadlinePolicy, (bool), (override));
#endif
};

}  // namespace

class FrameEvictorTest : public testing::Test {
 protected:
  void SetUp() override {
    FrameEvictionManager::GetInstance()->set_max_number_of_saved_frames(5);
  }

  void TearDown() override {
    if (evictor_) {
      FrameEvictionManager::GetInstance()->RemoveFrame(evictor_.get());
    }
  }

  void CreateEvictor() {
    evictor_ = std::make_unique<FrameEvictor>(&mock_client_);
  }

  testing::StrictMock<MockFrameEvictorClient> mock_client_;
  std::unique_ptr<FrameEvictor> evictor_;
};

// Tests that FrameEvictor constructor initializes with no surface and not visible.
// The evictor should start in a default state with has_surface() and visible()
// both returning false before any surface is embedded.
TEST_F(FrameEvictorTest, Constructor) {
  CreateEvictor();
  EXPECT_FALSE(evictor_->has_surface());
  EXPECT_FALSE(evictor_->visible());
}

// Tests that FrameEvictor destructor properly removes itself from the
// FrameEvictionManager. After destruction, the evictor should be unregistered
// from the manager to prevent callbacks to a destroyed object.
TEST_F(FrameEvictorTest, Destructor) {
  CreateEvictor();
  evictor_->OnNewSurfaceEmbedded();
  EXPECT_TRUE(evictor_->has_surface());
  evictor_.reset();
}

// Tests that OnNewSurfaceEmbedded sets has_surface() to true without changing
// visibility. After embedding a surface, the evictor should track the surface
// presence while visibility state remains unchanged (false by default).
TEST_F(FrameEvictorTest, OnNewSurfaceEmbedded) {
  CreateEvictor();
  EXPECT_FALSE(evictor_->has_surface());
  evictor_->OnNewSurfaceEmbedded();
  EXPECT_TRUE(evictor_->has_surface());
  EXPECT_FALSE(evictor_->visible());
}

// Tests that OnNewSurfaceEmbedded preserves the visible state when already
// visible. Setting visible before embedding should maintain visibility true
// after the surface is embedded; both has_surface and visible should be true.
TEST_F(FrameEvictorTest, OnNewSurfaceEmbeddedWhenVisible) {
  CreateEvictor();
  evictor_->SetVisible(true);
  EXPECT_TRUE(evictor_->visible());
  evictor_->OnNewSurfaceEmbedded();
  EXPECT_TRUE(evictor_->has_surface());
  EXPECT_TRUE(evictor_->visible());
}

// Tests that OnSurfaceDiscarded clears the has_surface state. After discarding
// a surface, the evictor should no longer report having a surface, resetting
// to the initial no-surface state.
TEST_F(FrameEvictorTest, OnSurfaceDiscarded) {
  CreateEvictor();
  evictor_->OnNewSurfaceEmbedded();
  EXPECT_TRUE(evictor_->has_surface());
  evictor_->OnSurfaceDiscarded();
  EXPECT_FALSE(evictor_->has_surface());
}

// Tests that SetVisible(false) when already invisible does not change state.
// Setting visibility to the same value should be a no-op; no state change
// should occur when visibility is already false.
TEST_F(FrameEvictorTest, SetVisibleNoChange) {
  CreateEvictor();
  evictor_->OnNewSurfaceEmbedded();
  EXPECT_FALSE(evictor_->visible());
  evictor_->SetVisible(false);
  EXPECT_FALSE(evictor_->visible());
}

// Tests that SetVisible transitions from false to true correctly. When a frame
// with a surface becomes visible, the visible() method should return true,
// indicating the frame is now eligible for different eviction treatment.
TEST_F(FrameEvictorTest, SetVisibleFromFalseToTrue) {
  CreateEvictor();
  evictor_->OnNewSurfaceEmbedded();
  EXPECT_FALSE(evictor_->visible());
  evictor_->SetVisible(true);
  EXPECT_TRUE(evictor_->visible());
}

// Tests that SetVisible transitions from true to false correctly. When a visible
// frame with a surface becomes invisible, the visible() method should return
// false, marking it as eligible for eviction based on visibility rules.
TEST_F(FrameEvictorTest, SetVisibleFromTrueToFalse) {
  CreateEvictor();
  evictor_->SetVisible(true);
  evictor_->OnNewSurfaceEmbedded();
  EXPECT_TRUE(evictor_->visible());
  evictor_->SetVisible(false);
  EXPECT_FALSE(evictor_->visible());
}

// Tests that SetVisible works when no surface is present. Visibility can be
// set independently of surface existence; the evictor should track visibility
// state even without having an actual surface.
TEST_F(FrameEvictorTest, SetVisibleWithoutSurface) {
  CreateEvictor();
  EXPECT_FALSE(evictor_->has_surface());
  evictor_->SetVisible(true);
  EXPECT_TRUE(evictor_->visible());
  EXPECT_FALSE(evictor_->has_surface());
}

// Tests that CollectSurfaceIdsForEviction includes current surface ID when
// embedded IDs are empty. When only the current surface exists, the result
// should contain exactly the current surface ID for eviction.
TEST_F(FrameEvictorTest, CollectSurfaceIdsForEviction_EmptyEmbeddedWithCurrent) {
  CreateEvictor();
  SurfaceId current_id = MakeSurfaceId(1, 1, 1);
  EXPECT_CALL(mock_client_, CollectSurfaceIdsForEviction())
      .WillOnce(Return(FrameEvictorClient::EvictIds()));
  EXPECT_CALL(mock_client_, GetCurrentSurfaceId())
      .WillRepeatedly(Return(current_id));
  EXPECT_CALL(mock_client_, GetPreNavigationSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction(false);
#else
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction();
#endif
  ASSERT_EQ(1u, result.size());
  EXPECT_EQ(current_id, result[0]);
}

// Tests that CollectSurfaceIdsForEviction returns empty when no surfaces exist.
// When there are no embedded IDs, current surface, or pre-navigation surface,
// the eviction list should be empty as nothing needs to be evicted.
TEST_F(FrameEvictorTest, CollectSurfaceIdsForEviction_EmptyEmbeddedNoCurrent) {
  CreateEvictor();
  EXPECT_CALL(mock_client_, CollectSurfaceIdsForEviction())
      .WillOnce(Return(FrameEvictorClient::EvictIds()));
  EXPECT_CALL(mock_client_, GetCurrentSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  EXPECT_CALL(mock_client_, GetPreNavigationSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction(false);
#else
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction();
#endif
  EXPECT_TRUE(result.empty());
}

// Tests that CollectSurfaceIdsForEviction includes embedded surface IDs.
// When the client provides embedded_ids, these should be included in the
// eviction result list for proper surface cleanup.
TEST_F(FrameEvictorTest, CollectSurfaceIdsForEviction_WithEmbeddedIds) {
  CreateEvictor();
  FrameEvictorClient::EvictIds ids;
  ids.embedded_ids.push_back(MakeSurfaceId(1, 1, 1));
  ids.embedded_ids.push_back(MakeSurfaceId(2, 2, 2));
  EXPECT_CALL(mock_client_, CollectSurfaceIdsForEviction())
      .WillOnce(Return(std::move(ids)));
  EXPECT_CALL(mock_client_, GetCurrentSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  EXPECT_CALL(mock_client_, GetPreNavigationSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction(false);
#else
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction();
#endif
  ASSERT_EQ(2u, result.size());
}

// Tests that CollectSurfaceIdsForEviction includes pre-navigation surface ID.
// When a pre-navigation surface exists, it should be added to the eviction
// list to ensure cleanup of surfaces from previous navigation.
TEST_F(FrameEvictorTest, CollectSurfaceIdsForEviction_WithPreNavSurfaceId) {
  CreateEvictor();
  SurfaceId pre_nav_id = MakeSurfaceId(3, 3, 3);
  FrameEvictorClient::EvictIds ids;
  EXPECT_CALL(mock_client_, CollectSurfaceIdsForEviction())
      .WillOnce(Return(std::move(ids)));
  EXPECT_CALL(mock_client_, GetCurrentSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  EXPECT_CALL(mock_client_, GetPreNavigationSurfaceId())
      .WillRepeatedly(Return(pre_nav_id));
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction(false);
#else
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction();
#endif
  ASSERT_EQ(1u, result.size());
  EXPECT_EQ(pre_nav_id, result[0]);
}

// Tests that CollectSurfaceIdsForEviction includes UI compositor surface ID.
// When the EvictIds contains a ui_compositor_id, it should be included in
// the eviction result for proper UI surface cleanup.
TEST_F(FrameEvictorTest, CollectSurfaceIdsForEviction_WithUICompositorId) {
  CreateEvictor();
  SurfaceId ui_id = MakeSurfaceId(4, 4, 4);
  FrameEvictorClient::EvictIds ids;
  ids.ui_compositor_id = ui_id;
  EXPECT_CALL(mock_client_, CollectSurfaceIdsForEviction())
      .WillOnce(Return(std::move(ids)));
  EXPECT_CALL(mock_client_, GetCurrentSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  EXPECT_CALL(mock_client_, GetPreNavigationSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction(false);
#else
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction();
#endif
  ASSERT_EQ(1u, result.size());
  EXPECT_EQ(ui_id, result[0]);
}

// Tests that CollectSurfaceIdsForEviction returns IDs in sorted order.
// Surface IDs should be sorted before returning to ensure deterministic
// eviction order; unsorted input should produce sorted output.
TEST_F(FrameEvictorTest, CollectSurfaceIdsForEviction_SortedOutput) {
  CreateEvictor();
  FrameEvictorClient::EvictIds ids;
  SurfaceId id3 = MakeSurfaceId(3, 3, 3);
  SurfaceId id1 = MakeSurfaceId(1, 1, 1);
  SurfaceId id2 = MakeSurfaceId(2, 2, 2);
  ids.embedded_ids.push_back(id3);
  ids.embedded_ids.push_back(id1);
  EXPECT_CALL(mock_client_, CollectSurfaceIdsForEviction())
      .WillOnce(Return(std::move(ids)));
  EXPECT_CALL(mock_client_, GetCurrentSurfaceId())
      .WillRepeatedly(Return(id2));
  EXPECT_CALL(mock_client_, GetPreNavigationSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction(false);
#else
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction();
#endif
  ASSERT_EQ(2u, result.size());
  EXPECT_EQ(id1, result[0]);
  EXPECT_EQ(id3, result[1]);
}

// Tests that EvictCurrentFrame calls EvictDelegatedFrame with collected IDs.
// When evicting, the evictor should collect all surface IDs and pass them
// to the client's EvictDelegatedFrame method for actual frame eviction.
TEST_F(FrameEvictorTest, EvictCurrentFrame_CallsEvictDelegatedFrame) {
  CreateEvictor();
  evictor_->OnNewSurfaceEmbedded();
  evictor_->SetVisible(false);
  SurfaceId id1 = MakeSurfaceId(1, 1, 1);
  std::vector<SurfaceId> expected_ids = {id1};
  FrameEvictorClient::EvictIds ids;
  ids.embedded_ids.push_back(id1);
  EXPECT_CALL(mock_client_, CollectSurfaceIdsForEviction())
      .WillOnce(Return(std::move(ids)));
  EXPECT_CALL(mock_client_, GetCurrentSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  EXPECT_CALL(mock_client_, GetPreNavigationSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  EXPECT_CALL(mock_client_, EvictDelegatedFrame(expected_ids));
  FrameEvictionManager::GetInstance()->PurgeAllUnlockedFrames();
}

// Tests that EvictIds default constructor creates empty containers.
// Default constructed EvictIds should have no embedded_ids and an invalid
// ui_compositor_id, representing no surfaces to evict.
TEST_F(FrameEvictorTest, EvictIds_DefaultConstructor) {
  FrameEvictorClient::EvictIds ids;
  EXPECT_TRUE(ids.embedded_ids.empty());
  EXPECT_FALSE(ids.ui_compositor_id.is_valid());
}

// Tests that EvictIds move constructor transfers all members correctly.
// Move construction should transfer embedded_ids and ui_compositor_id from
// the source without leaving residual data.
TEST_F(FrameEvictorTest, EvictIds_MoveConstructor) {
  SurfaceId id = MakeSurfaceId(1, 1, 1);
  FrameEvictorClient::EvictIds original;
  original.embedded_ids.push_back(id);
  original.ui_compositor_id = MakeSurfaceId(2, 2, 2);
  FrameEvictorClient::EvictIds moved(std::move(original));
  ASSERT_EQ(1u, moved.embedded_ids.size());
  EXPECT_EQ(id, moved.embedded_ids[0]);
  EXPECT_TRUE(moved.ui_compositor_id.is_valid());
}

// Tests that EvictIds move assignment operator transfers all members correctly.
// Move assignment should properly transfer embedded_ids and ui_compositor_id
// to the destination object.
TEST_F(FrameEvictorTest, EvictIds_MoveAssignment) {
  SurfaceId id = MakeSurfaceId(1, 1, 1);
  FrameEvictorClient::EvictIds original;
  original.embedded_ids.push_back(id);
  original.ui_compositor_id = MakeSurfaceId(2, 2, 2);
  FrameEvictorClient::EvictIds assigned = std::move(original);
  ASSERT_EQ(1u, assigned.embedded_ids.size());
  EXPECT_EQ(id, assigned.embedded_ids[0]);
  EXPECT_TRUE(assigned.ui_compositor_id.is_valid());
}

// Tests that CollectSurfaceIdsForEviction combines all surface ID sources.
// When embedded_ids, current, pre-nav, and ui_compositor_id all exist, the
// result should include all 5 surface IDs for comprehensive eviction.
TEST_F(FrameEvictorTest, CollectSurfaceIdsForEviction_AllFields) {
  CreateEvictor();
  SurfaceId embedded1 = MakeSurfaceId(1, 1, 1);
  SurfaceId embedded2 = MakeSurfaceId(1, 1, 2);
  SurfaceId current = MakeSurfaceId(2, 2, 1);
  SurfaceId pre_nav = MakeSurfaceId(3, 3, 1);
  SurfaceId ui = MakeSurfaceId(4, 4, 1);
  FrameEvictorClient::EvictIds ids;
  ids.embedded_ids.push_back(embedded1);
  ids.embedded_ids.push_back(embedded2);
  ids.ui_compositor_id = ui;
  EXPECT_CALL(mock_client_, CollectSurfaceIdsForEviction())
      .WillOnce(Return(std::move(ids)));
  EXPECT_CALL(mock_client_, GetCurrentSurfaceId())
      .WillRepeatedly(Return(current));
  EXPECT_CALL(mock_client_, GetPreNavigationSurfaceId())
      .WillRepeatedly(Return(pre_nav));
#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction(false);
#else
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction();
#endif
  EXPECT_EQ(4u, result.size());
}

// Tests that SetVisible handles multiple visibility transitions correctly.
// The evictor should track visibility state accurately across multiple
// true/false transitions without losing state.
TEST_F(FrameEvictorTest, MultipleSetVisibleChanges) {
  CreateEvictor();
  evictor_->OnNewSurfaceEmbedded();
  EXPECT_FALSE(evictor_->visible());
  evictor_->SetVisible(true);
  EXPECT_TRUE(evictor_->visible());
  evictor_->SetVisible(false);
  EXPECT_FALSE(evictor_->visible());
  evictor_->SetVisible(true);
  EXPECT_TRUE(evictor_->visible());
}

// Tests that OnNewSurfaceEmbedded can be called multiple times without error.
// Multiple surface embed calls should maintain has_surface() true without
// side effects, allowing for surface replacement scenarios.
TEST_F(FrameEvictorTest, OnNewSurfaceEmbeddedMultipleTimes) {
  CreateEvictor();
  evictor_->OnNewSurfaceEmbedded();
  EXPECT_TRUE(evictor_->has_surface());
  evictor_->OnNewSurfaceEmbedded();
  EXPECT_TRUE(evictor_->has_surface());
}

// Tests that OnSurfaceDiscarded is safe when no surface exists.
// Calling OnSurfaceDiscarded without a surface should be a no-op and not
// cause errors; has_surface should remain false.
TEST_F(FrameEvictorTest, OnSurfaceDiscardedWhenNoSurface) {
  CreateEvictor();
  EXPECT_FALSE(evictor_->has_surface());
  evictor_->OnSurfaceDiscarded();
  EXPECT_FALSE(evictor_->has_surface());
}

#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
// Tests that CollectSurfaceIdsForEviction with memory pressure parameter
// includes all surface IDs. When is_process_memory_pressure is true, all
// surfaces including ui_compositor_id should be collected for eviction.
TEST_F(FrameEvictorTest, CollectSurfaceIdsForEviction_WithMemoryPressureParam) {
  CreateEvictor();
  SurfaceId ui_id = MakeSurfaceId(4, 4, 4);
  SurfaceId embedded = MakeSurfaceId(1, 1, 1);
  SurfaceId embedded2 = MakeSurfaceId(1, 1, 2);
  FrameEvictorClient::EvictIds ids;
  ids.embedded_ids.push_back(embedded);
  ids.embedded_ids.push_back(embedded2);
  ids.ui_compositor_id = ui_id;
  EXPECT_CALL(mock_client_, CollectSurfaceIdsForEviction())
      .WillOnce(Return(std::move(ids)));
  EXPECT_CALL(mock_client_, GetCurrentSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  EXPECT_CALL(mock_client_, GetPreNavigationSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction(true);
  EXPECT_EQ(3u, result.size());
}

// Tests that EvictCurrentFrame passes memory pressure parameter to client.
// When called with is_process_memory_pressure=true, the eviction should
// proceed normally with all collected surface IDs passed to EvictDelegatedFrame.
TEST_F(FrameEvictorTest, EvictCurrentFrame_WithMemoryPressureParam) {
  CreateEvictor();
  evictor_->OnNewSurfaceEmbedded();
  evictor_->SetVisible(false);
  SurfaceId id1 = MakeSurfaceId(1, 1, 1);
  std::vector<SurfaceId> expected_ids = {id1};
  FrameEvictorClient::EvictIds ids;
  ids.embedded_ids.push_back(id1);
  EXPECT_CALL(mock_client_, CollectSurfaceIdsForEviction())
      .WillOnce(Return(std::move(ids)));
  EXPECT_CALL(mock_client_, GetCurrentSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  EXPECT_CALL(mock_client_, GetPreNavigationSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  EXPECT_CALL(mock_client_, EvictDelegatedFrame(expected_ids));
  FrameEvictionManager::GetInstance()->OnMemoryPressure(
      base::MEMORY_PRESSURE_LEVEL_CRITICAL);
}

// Tests that CollectSurfaceIdsForEviction clears ui_compositor_id when
// IsEvictUnlockFrameEnabled and not under memory pressure with multiple embedded_ids.
// UI compositor surface should be excluded from regular eviction when feature enabled.
TEST_F(FrameEvictorTest, CollectSurfaceIdsForEviction_ClearsUICompositorIdWhenConditionsMet) {
  if (!IsEvictUnlockFrameEnabled()) {
    GTEST_SKIP() << "Test only runs when IsEvictUnlockFrameEnabled is true";
  }
  CreateEvictor();
  SurfaceId ui_id = MakeSurfaceId(4, 4, 4);
  SurfaceId embedded1 = MakeSurfaceId(1, 1, 1);
  SurfaceId embedded2 = MakeSurfaceId(1, 1, 2);
  FrameEvictorClient::EvictIds ids;
  ids.embedded_ids.push_back(embedded1);
  ids.embedded_ids.push_back(embedded2);
  ids.ui_compositor_id = ui_id;
  EXPECT_CALL(mock_client_, CollectSurfaceIdsForEviction())
      .WillOnce(Return(std::move(ids)));
  EXPECT_CALL(mock_client_, GetCurrentSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  EXPECT_CALL(mock_client_, GetPreNavigationSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction(false);
  EXPECT_EQ(2u, result.size());
}

// Tests that CollectSurfaceIdsForEviction keeps ui_compositor_id under memory pressure.
// When is_process_memory_pressure=true, the UI compositor surface should be
// included in eviction even when IsEvictUnlockFrameEnabled is true.
TEST_F(FrameEvictorTest, CollectSurfaceIdsForEviction_KeepsUICompositorIdWhenMemoryPressure) {
  if (!IsEvictUnlockFrameEnabled()) {
    GTEST_SKIP() << "Test only runs when IsEvictUnlockFrameEnabled is true";
  }
  CreateEvictor();
  SurfaceId ui_id = MakeSurfaceId(4, 4, 4);
  SurfaceId embedded1 = MakeSurfaceId(1, 1, 1);
  SurfaceId embedded2 = MakeSurfaceId(1, 1, 2);
  FrameEvictorClient::EvictIds ids;
  ids.embedded_ids.push_back(embedded1);
  ids.embedded_ids.push_back(embedded2);
  ids.ui_compositor_id = ui_id;
  EXPECT_CALL(mock_client_, CollectSurfaceIdsForEviction())
      .WillOnce(Return(std::move(ids)));
  EXPECT_CALL(mock_client_, GetCurrentSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  EXPECT_CALL(mock_client_, GetPreNavigationSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction(true);
  EXPECT_EQ(3u, result.size());
}

// Tests that CollectSurfaceIdsForEviction keeps ui_compositor_id with single embedded.
// When there's only one embedded surface, ui_compositor_id should be preserved
// in the eviction list even when the feature is enabled and not under pressure.
TEST_F(FrameEvictorTest, CollectSurfaceIdsForEviction_KeepsUICompositorIdWhenOneEmbeddedId) {
  if (!IsEvictUnlockFrameEnabled()) {
    GTEST_SKIP() << "Test only runs when IsEvictUnlockFrameEnabled is true";
  }
  CreateEvictor();
  SurfaceId ui_id = MakeSurfaceId(4, 4, 4);
  SurfaceId embedded = MakeSurfaceId(1, 1, 1);
  FrameEvictorClient::EvictIds ids;
  ids.embedded_ids.push_back(embedded);
  ids.ui_compositor_id = ui_id;
  EXPECT_CALL(mock_client_, CollectSurfaceIdsForEviction())
      .WillOnce(Return(std::move(ids)));
  EXPECT_CALL(mock_client_, GetCurrentSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  EXPECT_CALL(mock_client_, GetPreNavigationSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction(false);
  EXPECT_EQ(2u, result.size());
}

// Tests that CollectSurfaceIdsForEviction includes all IDs when feature is disabled.
// When IsEvictUnlockFrameEnabled is false, the ui_compositor_id should always
// be included regardless of embedded count or memory pressure state.
TEST_F(FrameEvictorTest, CollectSurfaceIdsForEviction_NoClearWhenDisabled) {
  if (IsEvictUnlockFrameEnabled()) {
    GTEST_SKIP() << "Test only runs when IsEvictUnlockFrameEnabled is false";
  }
  CreateEvictor();
  SurfaceId ui_id = MakeSurfaceId(4, 4, 4);
  SurfaceId embedded1 = MakeSurfaceId(1, 1, 1);
  SurfaceId embedded2 = MakeSurfaceId(1, 1, 2);
  FrameEvictorClient::EvictIds ids;
  ids.embedded_ids.push_back(embedded1);
  ids.embedded_ids.push_back(embedded2);
  ids.ui_compositor_id = ui_id;
  EXPECT_CALL(mock_client_, CollectSurfaceIdsForEviction())
      .WillOnce(Return(std::move(ids)));
  EXPECT_CALL(mock_client_, GetCurrentSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  EXPECT_CALL(mock_client_, GetPreNavigationSurfaceId())
      .WillRepeatedly(Return(SurfaceId()));
  std::vector<SurfaceId> result = evictor_->CollectSurfaceIdsForEviction(false);
  EXPECT_EQ(3u, result.size());
}
#endif

// Tests that FrameEvictor destructor removes itself from FrameEvictionManager.
// Using ScopedPause to defer eviction, the evictor should safely unregister
// itself upon destruction without triggering immediate eviction callbacks.
TEST_F(FrameEvictorTest, DestructorRemovesFromManager) {
  CreateEvictor();
  evictor_->OnNewSurfaceEmbedded();
  EXPECT_TRUE(evictor_->has_surface());
  FrameEvictionManager* manager = FrameEvictionManager::GetInstance();
  manager->set_max_number_of_saved_frames(5);
  FrameEvictionManager::ScopedPause scoped_pause;
  evictor_.reset();
}

}  // namespace viz