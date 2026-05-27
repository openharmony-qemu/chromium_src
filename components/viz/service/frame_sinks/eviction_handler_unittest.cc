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

#include <memory>
#include <utility>
#include <vector>

#include "base/test/scoped_feature_list.h"
#include "base/unguessable_token.h"
#include "components/viz/common/features.h"
#include "components/viz/common/frame_sinks/copy_output_result.h"
#include "components/viz/common/quads/compositor_frame.h"
#include "components/viz/common/resources/resource_id.h"
#include "components/viz/common/resources/returned_resource.h"
#include "components/viz/common/resources/transferable_resource.h"
#include "components/viz/common/surfaces/frame_sink_id.h"
#include "components/viz/common/surfaces/local_surface_id.h"
#include "components/viz/common/surfaces/surface_id.h"
#include "components/viz/service/display/display.h"
#include "components/viz/common/display/renderer_settings.h"
#define private public
#include "components/viz/service/frame_sinks/compositor_frame_sink_support.h"
#include "components/viz/service/frame_sinks/eviction_handler.h"
#undef private
#include "components/viz/service/frame_sinks/frame_sink_manager_impl.h"
#include "components/viz/service/frame_sinks/surface_resource_holder.h"
#include "components/viz/service/surfaces/surface.h"
#include "components/viz/service/surfaces/surface_manager.h"
#include "components/viz/test/compositor_frame_helpers.h"
#include "components/viz/test/fake_external_begin_frame_source.h"
#include "components/viz/test/fake_surface_observer.h"
#include "components/viz/test/mock_compositor_frame_sink_client.h"
#include "components/viz/test/test_context_provider.h"
#include "components/viz/test/test_shared_image_interface_provider.h"
#include "gpu/command_buffer/common/mailbox.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "gpu/command_buffer/common/sync_token.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "components/viz/service/display_embedder/server_shared_bitmap_manager.h"
#include "third_party/khronos/GLES2/gl2.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace viz {

constexpr FrameSinkId kFrameSinkId(1, 1);
const base::UnguessableToken kArbitraryToken =
    base::UnguessableToken::CreateForTesting(1, 2);

class EvictionHandlerTest : public testing::Test {
 public:
  EvictionHandlerTest() : begin_frame_source_(0.f, false) {}

  void SetUp() override {
    manager_ = std::make_unique<FrameSinkManagerImpl>(
        FrameSinkManagerImpl::InitParams(&shared_bitmap_manager_));
    surface_observer_ =
        std::make_unique<FakeSurfaceObserver>(manager_->surface_manager());
    manager_->SetSharedImageInterfaceProviderForTest(
        &shared_image_interface_provider_);
    manager_->RegisterFrameSinkId(kFrameSinkId, true);
    support_ = std::make_unique<CompositorFrameSinkSupport>(
        &mock_client_, manager_.get(), kFrameSinkId, true);
    support_->SetBeginFrameSource(&begin_frame_source_);
  }

  void TearDown() override {
    handler_.reset();
    support_.reset();
    manager_->InvalidateFrameSinkId(kFrameSinkId);
    manager_.reset();
  }

  void CreateEvictionHandler() {
    handler_ = std::make_unique<EvictionHandler>(
        display_.get(), support_.get(), manager_->reserved_resource_id_tracker());
  }

  void CreateSurfaceWithFrame(const LocalSurfaceId& local_surface_id) {
    auto frame = MakeDefaultCompositorFrame();
    support_->SubmitCompositorFrame(local_surface_id, std::move(frame));
    manager_->surface_manager()->GarbageCollectSurfaces();
  }

  SurfaceId GetSurfaceId(const LocalSurfaceId& local_surface_id) {
    return SurfaceId(kFrameSinkId, local_surface_id);
  }

  Surface* GetSurfaceForId(const SurfaceId& id) {
    return manager_->surface_manager()->GetSurfaceForId(id);
  }

  bool InProgress() const { return handler_->in_progress_; }

  bool ToEvictIsValid() const {
    return handler_->to_evict_on_next_draw_and_swap_.is_valid();
  }

  int64_t SnapshotSeqId() const { return handler_->snapshot_seq_id_; }

  void SetInProgress(bool value) { handler_->in_progress_ = value; }

  void SetToEvict(const LocalSurfaceId& id) {
    handler_->to_evict_on_next_draw_and_swap_ = id;
  }

  void SetSnapshotSeqId(int64_t id) { handler_->snapshot_seq_id_ = id; }

  bool CopyOutputResultsContains(ResourceId id) {
    return handler_->copy_output_results_.contains(id);
  }

  void AddToCopyOutputResults(ResourceId id,
                               std::unique_ptr<CopyOutputResult> result) {
    handler_->copy_output_results_[id] = std::move(result);
  }

  void ClearCopyOutputResults(ResourceId id) {
    handler_->copy_output_results_.erase(id);
  }

  void SubmitPlaceholderContentForEvictionTest(
      const SurfaceId& to_evict,
      int64_t snapshot_seq_id,
      std::unique_ptr<CopyOutputResult> copy_result) {
    handler_->SubmitPlaceholderContentForEviction(to_evict, snapshot_seq_id,
                                                   std::move(copy_result));
  }

  std::optional<TransferableResource>
  CreateTransferableResourceFromCopyOutputResultTest(
      CopyOutputResult* copy_result,
      scoped_refptr<gpu::ClientSharedImage>& output_software_shared_image) {
    return handler_->CreateTransferableResourceFromCopyOutputResult(
        copy_result, output_software_shared_image);
  }

  ReservedResourceDelegate* GetExternalReservedResourceDelegate() {
    return support_->external_reserved_resource_delegate_;
  }

 protected:
  TestSharedImageInterfaceProvider shared_image_interface_provider_;
  ServerSharedBitmapManager shared_bitmap_manager_;
  std::unique_ptr<FrameSinkManagerImpl> manager_;
  std::unique_ptr<FakeSurfaceObserver> surface_observer_;
  FakeExternalBeginFrameSource begin_frame_source_;
  MockCompositorFrameSinkClient mock_client_;
  std::unique_ptr<CompositorFrameSinkSupport> support_;
  std::unique_ptr<Display> display_;
  std::unique_ptr<EvictionHandler> handler_;
  RendererSettings renderer_settings_;
};

// Tests that EvictionHandler constructor sets itself as the external reserved
// resource delegate on the CompositorFrameSinkSupport. The handler should be
// registered to receive resource-related callbacks from the support.
TEST_F(EvictionHandlerTest, ConstructorSetsExternalReservedResourceDelegate) {
  EXPECT_EQ(GetExternalReservedResourceDelegate(), nullptr);
  CreateEvictionHandler();
  EXPECT_EQ(GetExternalReservedResourceDelegate(), handler_.get());
}

// Tests that EvictionHandler destructor clears the external reserved resource
// delegate on the CompositorFrameSinkSupport. After destruction, the support
// should have no delegate to prevent callbacks to destroyed handler.
TEST_F(EvictionHandlerTest, DestructorClearsExternalReservedResourceDelegate) {
  CreateEvictionHandler();
  EXPECT_EQ(GetExternalReservedResourceDelegate(), handler_.get());
  handler_.reset();
  EXPECT_EQ(GetExternalReservedResourceDelegate(), nullptr);
}

// Tests that MaybeFinishEvictionProcess clears eviction state when called.
// The in_progress_ flag and to_evict_on_next_draw_and_swap_ should be reset
// to indicate the eviction process has completed.
TEST_F(EvictionHandlerTest, MaybeFinishEvictionProcessClearsState) {
  CreateEvictionHandler();

  SetInProgress(true);
  SetToEvict(LocalSurfaceId(1, base::UnguessableToken::Create()));

  handler_->MaybeFinishEvictionProcess();

  EXPECT_FALSE(ToEvictIsValid());
  EXPECT_FALSE(InProgress());
}

// Tests that DisplayDidDrawAndSwap does nothing when eviction is not in progress.
// When in_progress_ is false, calling DisplayDidDrawAndSwap should not change
// any state or trigger eviction actions.
TEST_F(EvictionHandlerTest, DisplayDidDrawAndSwapDoesNothingWhenNotInProgress) {
  CreateEvictionHandler();

  SetInProgress(false);
  SetToEvict(LocalSurfaceId());

  handler_->DisplayDidDrawAndSwap();

  EXPECT_FALSE(InProgress());
}

// Tests that DisplayDidDrawAndSwap does not evict when in_progress but ID is invalid.
// When to_evict_on_next_draw_and_swap_ is not valid, the handler should not
// proceed with eviction even if in_progress_ is true.
TEST_F(EvictionHandlerTest,
       DisplayDidDrawAndSwapNoEvictionWhenInProgressButInvalidId) {
  CreateEvictionHandler();

  SetInProgress(true);
  SetToEvict(LocalSurfaceId());

  handler_->DisplayDidDrawAndSwap();

  EXPECT_TRUE(InProgress());
}

// Tests that ReceiveFromChild only processes resources in copy_output_results_.
// Resources matching IDs in copy_output_results_ should be retained; external
// resources not in the map should be ignored.
TEST_F(EvictionHandlerTest, ReceiveFromChildRefsResourcesInCopyOutputResults) {
  CreateEvictionHandler();

  ResourceId resource_id =
      manager_->reserved_resource_id_tracker()->AllocId(0);
  auto result = std::make_unique<CopyOutputResult>(
      CopyOutputResult::Format::RGBA,
      CopyOutputResult::Destination::kSystemMemory, gfx::Rect(0, 0, 10, 10),
      false);
  AddToCopyOutputResults(resource_id, std::move(result));

  std::vector<TransferableResource> resources;
  TransferableResource resource;
  resource.id = resource_id;
  resources.push_back(resource);

  handler_->ReceiveFromChild(resources);

  EXPECT_TRUE(CopyOutputResultsContains(resource_id));
}

// Tests that ReceiveFromChild ignores resources not tracked in copy_output_results_.
// External resources with IDs not present in the handler's tracking should not
// be added to copy_output_results_ map.
TEST_F(EvictionHandlerTest,
       ReceiveFromChildIgnoresResourcesNotInCopyOutputResults) {
  CreateEvictionHandler();

  ResourceId external_resource_id(100);

  std::vector<TransferableResource> resources;
  TransferableResource resource;
  resource.id = external_resource_id;
  resources.push_back(resource);

  handler_->ReceiveFromChild(resources);

  EXPECT_FALSE(CopyOutputResultsContains(external_resource_id));
}

// Tests that RefResources increments ref count for resources in copy_output_results_.
// Resources tracked in copy_output_results_ should be successfully referenced
// without causing errors or crashes.
TEST_F(EvictionHandlerTest, RefResourcesRefsResourcesInCopyOutputResults) {
  CreateEvictionHandler();

  ResourceId resource_id =
      manager_->reserved_resource_id_tracker()->AllocId(0);
  auto result = std::make_unique<CopyOutputResult>(
      CopyOutputResult::Format::RGBA,
      CopyOutputResult::Destination::kSystemMemory, gfx::Rect(0, 0, 10, 10),
      false);
  AddToCopyOutputResults(resource_id, std::move(result));

  std::vector<TransferableResource> resources;
  TransferableResource resource;
  resource.id = resource_id;
  resources.push_back(resource);

  handler_->RefResources(resources);
}

// Tests that UnrefResources keeps entry when ref count remains above zero.
// When the allocated resource has ref count > 1, unreffing once should not
// remove the resource from copy_output_results_ map.
TEST_F(EvictionHandlerTest, UnrefResourcesKeepsWhenRefCountNotZero) {
  CreateEvictionHandler();

  ResourceId resource_id =
      manager_->reserved_resource_id_tracker()->AllocId(2);
  auto result = std::make_unique<CopyOutputResult>(
      CopyOutputResult::Format::RGBA,
      CopyOutputResult::Destination::kSystemMemory, gfx::Rect(0, 0, 10, 10),
      false);
  AddToCopyOutputResults(resource_id, std::move(result));

  std::vector<ReturnedResource> resources;
  resources.emplace_back(resource_id, gpu::SyncToken(), gfx::GpuFenceHandle(), 1,
                          false);

  handler_->UnrefResources(resources);

  EXPECT_TRUE(CopyOutputResultsContains(resource_id));
}

// Tests that UnrefResources ignores resources not tracked in copy_output_results_.
// External resources not in the handler's tracking should not affect state
// when unreffed; no entries should be added or modified.
TEST_F(EvictionHandlerTest,
       UnrefResourcesIgnoresResourcesNotInCopyOutputResults) {
  CreateEvictionHandler();

  ResourceId external_resource_id(100);

  std::vector<ReturnedResource> resources;
  resources.emplace_back(external_resource_id, gpu::SyncToken(),
                          gfx::GpuFenceHandle(), 1, false);

  handler_->UnrefResources(resources);

  EXPECT_FALSE(CopyOutputResultsContains(external_resource_id));
}

// Tests that snapshot sequence ID starts at zero on construction.
// The handler's snapshot_seq_id_ should be initialized to 0 to track
// the sequence of eviction snapshot operations.
TEST_F(EvictionHandlerTest, SnapshotSequenceIdStartsAtZero) {
  CreateEvictionHandler();

  EXPECT_EQ(SnapshotSeqId(), 0);
}

// Tests that SubmitPlaceholderContent ignores requests with mismatched snapshot seq ID.
// When the provided snapshot_seq_id doesn't match the handler's current ID,
// the placeholder submission should be ignored and to_evict not set.
TEST_F(EvictionHandlerTest,
       SubmitPlaceholderContentIgnoresMismatchedSnapshotSeqId) {
  CreateEvictionHandler();

  SetInProgress(true);
  SetSnapshotSeqId(5);

  LocalSurfaceId to_evict_id(1, base::UnguessableToken::Create());
  SurfaceId to_evict(kFrameSinkId, to_evict_id);

  SubmitPlaceholderContentForEvictionTest(
      to_evict, 3, std::unique_ptr<CopyOutputResult>());

  EXPECT_FALSE(ToEvictIsValid());
}

// Tests that SubmitPlaceholderContent ignores requests when not in progress.
// When in_progress_ is false, placeholder content submission should be ignored
// regardless of snapshot sequence ID matching.
TEST_F(EvictionHandlerTest, SubmitPlaceholderContentIgnoresWhenNotInProgress) {
  CreateEvictionHandler();

  SetInProgress(false);
  SetSnapshotSeqId(5);

  LocalSurfaceId to_evict_id(1, base::UnguessableToken::Create());
  SurfaceId to_evict(kFrameSinkId, to_evict_id);

  SubmitPlaceholderContentForEvictionTest(
      to_evict, 5, std::unique_ptr<CopyOutputResult>());

  EXPECT_FALSE(ToEvictIsValid());
}

// Tests that CreateTransferableResourceFromCopyOutputResult returns empty for null.
// When the CopyOutputResult pointer is null, no transferable resource should
// be created; the result should be empty and shared image unchanged.
TEST_F(EvictionHandlerTest, CreateTransferableResourceWithNullCopyResult) {
  CreateEvictionHandler();

  scoped_refptr<gpu::ClientSharedImage> software_shared_image;
  auto result = CreateTransferableResourceFromCopyOutputResultTest(
      nullptr, software_shared_image);

  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(software_shared_image, nullptr);
}

// Tests that CreateTransferableResourceFromCopyOutputResult returns empty for empty.
// When CopyOutputResult has empty size (no content), no transferable resource
// should be created; the result should be empty.
TEST_F(EvictionHandlerTest, CreateTransferableResourceWithEmptyCopyResult) {
  CreateEvictionHandler();

  auto empty_result = std::make_unique<CopyOutputResult>(
      CopyOutputResult::Format::RGBA,
      CopyOutputResult::Destination::kSystemMemory, gfx::Rect(), false);

  scoped_refptr<gpu::ClientSharedImage> software_shared_image;
  auto result = CreateTransferableResourceFromCopyOutputResultTest(
      empty_result.get(), software_shared_image);

  EXPECT_FALSE(result.has_value());
}

// Tests that ReceiveFromChild handles multiple resources correctly.
// Multiple resources matching IDs in copy_output_results_ should all be
// processed and retained in the tracking map.
TEST_F(EvictionHandlerTest, ReceiveFromChildMultipleResources) {
  CreateEvictionHandler();

  ResourceId resource_id1 =
      manager_->reserved_resource_id_tracker()->AllocId(0);
  ResourceId resource_id2 =
      manager_->reserved_resource_id_tracker()->AllocId(0);

  auto result1 = std::make_unique<CopyOutputResult>(
      CopyOutputResult::Format::RGBA,
      CopyOutputResult::Destination::kSystemMemory, gfx::Rect(0, 0, 10, 10),
      false);
  auto result2 = std::make_unique<CopyOutputResult>(
      CopyOutputResult::Format::RGBA,
      CopyOutputResult::Destination::kSystemMemory, gfx::Rect(0, 0, 20, 20),
      false);
  AddToCopyOutputResults(resource_id1, std::move(result1));
  AddToCopyOutputResults(resource_id2, std::move(result2));

  std::vector<TransferableResource> resources;
  TransferableResource resource1;
  resource1.id = resource_id1;
  TransferableResource resource2;
  resource2.id = resource_id2;
  resources.push_back(resource1);
  resources.push_back(resource2);

  handler_->ReceiveFromChild(resources);

  EXPECT_TRUE(CopyOutputResultsContains(resource_id1));
  EXPECT_TRUE(CopyOutputResultsContains(resource_id2));
}

// Tests that UnrefResources handles multiple resources with ref count zero.
// When multiple resources have ref count reaching zero simultaneously, all
// should be removed from copy_output_results_ map.
TEST_F(EvictionHandlerTest, UnrefResourcesMultipleResources) {
  CreateEvictionHandler();

  ResourceId resource_id1 =
      manager_->reserved_resource_id_tracker()->AllocId(1);
  ResourceId resource_id2 =
      manager_->reserved_resource_id_tracker()->AllocId(1);

  auto result1 = std::make_unique<CopyOutputResult>(
      CopyOutputResult::Format::RGBA,
      CopyOutputResult::Destination::kSystemMemory, gfx::Rect(0, 0, 10, 10),
      false);
  auto result2 = std::make_unique<CopyOutputResult>(
      CopyOutputResult::Format::RGBA,
      CopyOutputResult::Destination::kSystemMemory, gfx::Rect(0, 0, 20, 20),
      false);
  AddToCopyOutputResults(resource_id1, std::move(result1));
  AddToCopyOutputResults(resource_id2, std::move(result2));

  std::vector<ReturnedResource> resources;
  resources.emplace_back(resource_id1, gpu::SyncToken(), gfx::GpuFenceHandle(), 1,
                          false);
  resources.emplace_back(resource_id2, gpu::SyncToken(), gfx::GpuFenceHandle(), 1,
                          false);

  handler_->UnrefResources(resources);

  EXPECT_FALSE(CopyOutputResultsContains(resource_id1));
  EXPECT_FALSE(CopyOutputResultsContains(resource_id2));
}

// Tests that in_progress_ flag is initially false after construction.
// The handler should not be in an eviction process state initially.
TEST_F(EvictionHandlerTest, InProgressFlagInitiallyFalse) {
  CreateEvictionHandler();

  EXPECT_FALSE(InProgress());
}

// Tests that in_progress_ flag is set and reset correctly through the eviction flow.
// Setting in_progress_ true should persist; MaybeFinishEvictionProcess should
// reset it to false to indicate completion.
TEST_F(EvictionHandlerTest, InProgressFlagSetAndResetCorrectly) {
  CreateEvictionHandler();

  SetInProgress(true);
  EXPECT_TRUE(InProgress());

  handler_->MaybeFinishEvictionProcess();
  EXPECT_FALSE(InProgress());
}

#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
// Tests that GetNextEvictLocalSurfaceId returns the stored eviction target ID.
// The handler should provide access to the LocalSurfaceId scheduled for eviction
// on the next draw and swap.
TEST_F(EvictionHandlerTest, GetNextEvictLocalSurfaceId) {
  CreateEvictionHandler();

  LocalSurfaceId test_id(5, base::UnguessableToken::Create());
  SetToEvict(test_id);

  EXPECT_EQ(handler_->GetNextEvictLocalSurfaceId(), test_id);
}

// Tests that SetNextEvictLocalSurfaceId stores the provided ID for deferred eviction.
// The handler should accept and store a LocalSurfaceId to be evicted after
// the next draw and swap operation.
TEST_F(EvictionHandlerTest, SetNextEvictLocalSurfaceId) {
  CreateEvictionHandler();

  LocalSurfaceId test_id(10, base::UnguessableToken::Create());
  handler_->SetNextEvictLocalSurfaceId(test_id);

  EXPECT_EQ(handler_->GetNextEvictLocalSurfaceId(), test_id);
}
#endif  // BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)

}  // namespace viz