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

#include "gtest/gtest.h"
#define private public
#include "components/viz/service/display/display.h"
#include "components/viz/service/frame_sinks/frame_sink_manager_impl.h"
#undef private

#include "base/command_line.h"
#include "components/viz/common/surfaces/frame_sink_id.h"
#include "components/viz/service/display_embedder/server_shared_bitmap_manager.h"
#include "components/viz/test/compositor_frame_helpers.h"
#include "components/viz/test/mock_compositor_frame_sink_client.h"
#include "components/viz/test/mock_display_client.h"
#include "components/viz/test/test_output_surface_provider.h"
#include "content/public/common/content_switches.h"
#include "third_party/khronos/GLES2/gl2.h"

namespace viz {
namespace {

struct RootCompositorFrameSinkData {
  mojom::RootCompositorFrameSinkParamsPtr BuildParams(
      const FrameSinkId& frame_sink_id) {
    auto params = mojom::RootCompositorFrameSinkParams::New();
    params->frame_sink_id = frame_sink_id;
    params->widget = gpu::kNullSurfaceHandle;
    params->compositor_frame_sink =
        compositor_frame_sink.BindNewEndpointAndPassReceiver();
    params->compositor_frame_sink_client =
        compositor_frame_sink_client.BindInterfaceRemote();
    params->display_private = display_private.BindNewEndpointAndPassReceiver();
    params->display_client = display_client.BindRemote();
    return params;
  }

  mojo::AssociatedRemote<mojom::CompositorFrameSink> compositor_frame_sink;
  MockCompositorFrameSinkClient compositor_frame_sink_client;
  mojo::AssociatedRemote<mojom::DisplayPrivate> display_private;
  MockDisplayClient display_client;
};

}  // namespace

class MockFrameSinkManagerClient : public mojom::FrameSinkManagerClient {
 public:
  MOCK_METHOD(void,
              OnFirstSurfaceActivation,
              (const ::viz::SurfaceInfo& surface_info),
              ());
  MOCK_METHOD(
      void,
      OnAggregatedHitTestRegionListUpdated,
      (const ::viz::FrameSinkId& frame_sink_id,
       const std::vector<::viz::AggregatedHitTestRegion>& hit_test_data),
      ());
  MOCK_METHOD(void,
              OnFrameTokenChanged,
              (const ::viz::FrameSinkId& frame_sink_id,
               uint32_t frame_token,
               ::base::TimeTicks activation_time),
              ());
  MOCK_METHOD(void,
              OnVsyncReceived,
              (uint32_t client_id, uint32_t sink_id),
              ());
  MOCK_METHOD(void, OnVsync, (uint32_t client_id, uint32_t sink_id), ());
};

class CompositorFrameSinkImplTest : public testing::Test {
 public:
  CompositorFrameSinkImplTest()
      : manager_(FrameSinkManagerImpl::InitParams(&shared_bitmap_manager_,
                                                  &output_surface_provider_)) {}
  ~CompositorFrameSinkImplTest() override = default;

  std::unique_ptr<CompositorFrameSinkSupport> CreateCompositorFrameSinkSupport(
      const FrameSinkId& frame_sink_id) {
    return std::make_unique<CompositorFrameSinkSupport>(nullptr, &manager_,
                                                        frame_sink_id, false);
  }

  const BeginFrameSource* GetBeginFrameSource(
      const std::unique_ptr<CompositorFrameSinkSupport>& support) {
    return support->begin_frame_source_;
  }

  void ExpireAllTemporaryReferencesAndGarbageCollect() {
    manager_.surface_manager()->ExpireOldTemporaryReferences();
    manager_.surface_manager()->ExpireOldTemporaryReferences();
    manager_.surface_manager()->GarbageCollectSurfaces();
  }

  bool CompositorFrameSinkExists(const FrameSinkId& frame_sink_id) {
    return base::Contains(manager_.sink_map_, frame_sink_id) ||
           base::Contains(manager_.root_sink_map_, frame_sink_id);
  }

  CapturableFrameSink* FindCapturableFrameSink(const FrameSinkId& id) {
    return manager_.FindCapturableFrameSink(VideoCaptureTarget(id));
  }

  void VerifyThrottling(base::TimeDelta interval,
                        const std::vector<FrameSinkId>& ids) {
    for (auto& id : ids) {
      EXPECT_EQ(interval, manager_.support_map_[id]->begin_frame_interval_);
    }
  }

  void CreateTestCompositorFrameSink(const FrameSinkId& frame_sink_id) {
    manager_.RegisterFrameSinkId(frame_sink_id, true);
    MockCompositorFrameSinkClient compositor_frame_sink_client;
    mojo::Remote<mojom::CompositorFrameSink> compositor_frame_sink;
    manager_.CreateCompositorFrameSink(
        frame_sink_id, absl::nullopt,
        compositor_frame_sink.BindNewPipeAndPassReceiver(),
        compositor_frame_sink_client.BindInterfaceRemote());
    EXPECT_TRUE(CompositorFrameSinkExists(frame_sink_id));
  }

  void AddResourcesToFrame(CompositorFrame* frame,
                           ResourceId* resource_ids,
                           size_t num_resource_ids) {
    for (size_t i = 0u; i < num_resource_ids; ++i) {
      TransferableResource resource;
      resource.id = resource_ids[i];
      resource.mailbox_holder.texture_target = GL_TEXTURE_2D;
      resource.mailbox_holder.sync_token = frame_sync_token_;
      frame->resource_list.push_back(resource);
    }
  }

 protected:
  DebugRendererSettings debug_settings_;
  ServerSharedBitmapManager shared_bitmap_manager_;
  TestOutputSurfaceProvider output_surface_provider_;
  FrameSinkManagerImpl manager_;
  const gpu::SyncToken frame_sync_token_;
  LocalSurfaceId local_surface_id_;
};

TEST_F(CompositorFrameSinkImplTest, SubmitCompositorFrame_001) {
  constexpr FrameSinkId kFrameSinkIdTmp(1, 1);
  const uint64_t kBeginFrameSourceId = 1337;
  CreateTestCompositorFrameSink(kFrameSinkIdTmp);
  ResourceId first_frame_ids[] = {ResourceId(1), ResourceId(2), ResourceId(3)};
  auto frame = MakeDefaultCompositorFrame(kBeginFrameSourceId);
  AddResourcesToFrame(&frame, first_frame_ids, std::size(first_frame_ids));
  frame.metadata.dropped_frame_count = 1;
  frame.metadata.dropped_frame_duration = 1;
  absl::optional<HitTestRegionList> hit_test_region_list;
  uint64_t submit_time = 1;
  manager_.sink_map_[kFrameSinkIdTmp]->SubmitCompositorFrame(
      local_surface_id_, std::move(frame), hit_test_region_list, submit_time);
  EXPECT_TRUE(!!frame.metadata.dropped_frame_count &&
              !!frame.metadata.dropped_frame_duration);
}

TEST_F(CompositorFrameSinkImplTest, SubmitCompositorFrame_002) {
  constexpr FrameSinkId kFrameSinkIdTmp(1, 1);
  const uint64_t kBeginFrameSourceId = 1337;
  CreateTestCompositorFrameSink(kFrameSinkIdTmp);
  ResourceId first_frame_ids[] = {ResourceId(1), ResourceId(2), ResourceId(3)};
  auto frame = MakeDefaultCompositorFrame(kBeginFrameSourceId);
  AddResourcesToFrame(&frame, first_frame_ids, std::size(first_frame_ids));
  absl::optional<HitTestRegionList> hit_test_region_list;
  uint64_t submit_time = 1;
  manager_.sink_map_[kFrameSinkIdTmp]->SubmitCompositorFrame(
      local_surface_id_, std::move(frame), hit_test_region_list, submit_time);
  EXPECT_FALSE(!!frame.metadata.dropped_frame_count &&
               !!frame.metadata.dropped_frame_duration);
}

TEST_F(CompositorFrameSinkImplTest, SubmitCompositorFrame_003) {
  constexpr FrameSinkId kFrameSinkIdTmp(1, 1);
  const uint64_t kBeginFrameSourceId = 1337;
  CreateTestCompositorFrameSink(kFrameSinkIdTmp);
  ResourceId first_frame_ids[] = {ResourceId(1), ResourceId(2), ResourceId(3)};
  auto frame = MakeDefaultCompositorFrame(kBeginFrameSourceId);
  AddResourcesToFrame(&frame, first_frame_ids, std::size(first_frame_ids));
  frame.metadata.dropped_frame_count = 1;
  absl::optional<HitTestRegionList> hit_test_region_list;
  uint64_t submit_time = 1;
  manager_.sink_map_[kFrameSinkIdTmp]->SubmitCompositorFrame(
      local_surface_id_, std::move(frame), hit_test_region_list, submit_time);
  EXPECT_FALSE(!!frame.metadata.dropped_frame_count &&
               !!frame.metadata.dropped_frame_duration);
}

TEST_F(CompositorFrameSinkImplTest, SubmitCompositorFrame_004) {
  constexpr FrameSinkId kFrameSinkIdTmp(1, 1);
  const uint64_t kBeginFrameSourceId = 1337;
  CreateTestCompositorFrameSink(kFrameSinkIdTmp);
  ResourceId first_frame_ids[] = {ResourceId(1), ResourceId(2), ResourceId(3)};
  auto frame = MakeDefaultCompositorFrame(kBeginFrameSourceId);
  AddResourcesToFrame(&frame, first_frame_ids, std::size(first_frame_ids));
  frame.metadata.dropped_frame_duration = 1;
  absl::optional<HitTestRegionList> hit_test_region_list;
  uint64_t submit_time = 1;
  manager_.sink_map_[kFrameSinkIdTmp]->SubmitCompositorFrame(
      local_surface_id_, std::move(frame), hit_test_region_list, submit_time);
  EXPECT_FALSE(!!frame.metadata.dropped_frame_count &&
               !!frame.metadata.dropped_frame_duration);
}

TEST_F(CompositorFrameSinkImplTest, GetFrameRate_001) {
  constexpr FrameSinkId kFrameSinkIdTmp(1, 1);
  const uint64_t kBeginFrameSourceId = 1337;
  CreateTestCompositorFrameSink(kFrameSinkIdTmp);
  ResourceId first_frame_ids[] = {ResourceId(1), ResourceId(2), ResourceId(3)};
  auto frame = MakeDefaultCompositorFrame(kBeginFrameSourceId);
  AddResourcesToFrame(&frame, first_frame_ids, std::size(first_frame_ids));
  manager_.sink_map_[kFrameSinkIdTmp]->GetFrameRate();
  EXPECT_TRUE(manager_.sink_map_[kFrameSinkIdTmp]->support_);
}

TEST_F(CompositorFrameSinkImplTest, ReportKeyThreadIds_001) {
  constexpr FrameSinkId kFrameSinkIdTmp(1, 1);
  const uint64_t kBeginFrameSourceId = 1337;
  CreateTestCompositorFrameSink(kFrameSinkIdTmp);
  ResourceId first_frame_ids[] = {ResourceId(1), ResourceId(2), ResourceId(3)};
  auto frame = MakeDefaultCompositorFrame(kBeginFrameSourceId);
  AddResourcesToFrame(&frame, first_frame_ids, std::size(first_frame_ids));
  const std::vector<int32_t> thread_ids;
  bool is_created = true;
  manager_.sink_map_[kFrameSinkIdTmp]->ReportKeyThreadIds(thread_ids, 1, true);
  EXPECT_EQ(is_created ? OHOS::NWeb::ResSchedStatusAdapter::THREAD_CREATED
                       : OHOS::NWeb::ResSchedStatusAdapter::THREAD_DESTROYED,
            OHOS::NWeb::ResSchedStatusAdapter::THREAD_CREATED);
}

TEST_F(CompositorFrameSinkImplTest, ReportKeyThreadIds_002) {
  constexpr FrameSinkId kFrameSinkIdTmp(1, 1);
  const uint64_t kBeginFrameSourceId = 1337;
  CreateTestCompositorFrameSink(kFrameSinkIdTmp);
  ResourceId first_frame_ids[] = {ResourceId(1), ResourceId(2), ResourceId(3)};
  auto frame = MakeDefaultCompositorFrame(kBeginFrameSourceId);
  AddResourcesToFrame(&frame, first_frame_ids, std::size(first_frame_ids));
  const std::vector<int32_t> thread_ids;
  bool is_created = false;
  manager_.sink_map_[kFrameSinkIdTmp]->ReportKeyThreadIds(thread_ids, 1, false);
  EXPECT_EQ(is_created ? OHOS::NWeb::ResSchedStatusAdapter::THREAD_CREATED
                       : OHOS::NWeb::ResSchedStatusAdapter::THREAD_DESTROYED,
            OHOS::NWeb::ResSchedStatusAdapter::THREAD_DESTROYED);
}

TEST_F(CompositorFrameSinkImplTest, ReportKeyThreadIds_003) {
  constexpr FrameSinkId kFrameSinkIdTmp(1, 1);
  const uint64_t kBeginFrameSourceId = 1337;
  CreateTestCompositorFrameSink(kFrameSinkIdTmp);
  ResourceId first_frame_ids[] = {ResourceId(1), ResourceId(2), ResourceId(3)};
  auto frame = MakeDefaultCompositorFrame(kBeginFrameSourceId);
  AddResourcesToFrame(&frame, first_frame_ids, std::size(first_frame_ids));
  const std::vector<int32_t> thread_ids;
  manager_.sink_map_[kFrameSinkIdTmp]->ReportKeyThreadIds(thread_ids, 1, false);
  EXPECT_NE(base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
                switches::kProcessType),
            switches::kGpuProcess);
}

TEST_F(CompositorFrameSinkImplTest, TriggerVsyncImplTask_001) {
  constexpr FrameSinkId kFrameSinkIdTmp(1, 1);
  CreateTestCompositorFrameSink(kFrameSinkIdTmp);
  std::unique_ptr<BeginFrameSource> begin_frame_source =
      std::unique_ptr<BeginFrameSource>();
  manager_.sink_map_[kFrameSinkIdTmp]->support_->begin_frame_source_ =
      begin_frame_source.get();
  manager_.sink_map_[kFrameSinkIdTmp]->TriggerVsyncImplTask();

  EXPECT_TRUE(manager_.sink_map_[kFrameSinkIdTmp]->support_);

  EXPECT_TRUE(
      !manager_.sink_map_[kFrameSinkIdTmp]->support_ ||
      !manager_.sink_map_[kFrameSinkIdTmp]->support_->begin_frame_source());
}

TEST_F(CompositorFrameSinkImplTest, TriggerVsyncImplTask_002) {
  constexpr FrameSinkId kFrameSinkIdTmp(1, 1);
  CreateTestCompositorFrameSink(kFrameSinkIdTmp);
  manager_.sink_map_[kFrameSinkIdTmp]->TriggerVsyncImplTask();
  EXPECT_TRUE(
      !manager_.sink_map_[kFrameSinkIdTmp]->support_ ||
      !manager_.sink_map_[kFrameSinkIdTmp]->support_->begin_frame_source());
}

TEST_F(CompositorFrameSinkImplTest, TriggerVsyncImplTask_003) {
  constexpr FrameSinkId kFrameSinkIdTmp(1, 1);
  CreateTestCompositorFrameSink(kFrameSinkIdTmp);
  std::unique_ptr<BeginFrameSource> begin_frame_source =
      std::unique_ptr<BeginFrameSource>();
  manager_.sink_map_[kFrameSinkIdTmp]->support_->begin_frame_source_ =
      begin_frame_source.get();
  manager_.sink_map_[kFrameSinkIdTmp]->TriggerVsyncImplTask();
  EXPECT_TRUE(
      !manager_.sink_map_[kFrameSinkIdTmp]->support_ ||
      !manager_.sink_map_[kFrameSinkIdTmp]->support_->begin_frame_source());
}
}  // namespace viz
