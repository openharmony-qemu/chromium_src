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
#include "components/viz/service/display/display.h"
#include "components/viz/service/frame_sinks/frame_sink_manager_impl.h"
#undef private

#define protected public
#include "components/viz/service/display/direct_renderer.h"
#undef protected

#include <memory>

#include "components/viz/service/display_embedder/server_shared_bitmap_manager.h"
#include "components/viz/test/mock_compositor_frame_sink_client.h"
#include "components/viz/test/mock_display_client.h"
#include "components/viz/test/test_output_surface_provider.h"
#include "testing/gmock/include/gmock/gmock.h"

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

class MockFrameSinkManagerClient
    : public testing::NiceMock<mojom::FrameSinkManagerClient> {
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
  MOCK_METHOD(void, OnVsync, (uint32_t client_id, uint32_t sink_id), ());
  MOCK_METHOD(void,
              OnVsyncReceived,
              (uint32_t client_id, uint32_t sink_id),
              ());
};

class MockExternalBeginFrameSourceClient
    : public ExternalBeginFrameSourceClient {
 public:
  MockExternalBeginFrameSourceClient() = default;
  virtual ~MockExternalBeginFrameSourceClient() = default;

  MOCK_METHOD(void, OnNeedsBeginFrames, (bool needs_begin_frames), ());
};

class RootCompositorFrameSinkImplTest : public testing::Test {
 public:
  RootCompositorFrameSinkImplTest()
      : manager_(FrameSinkManagerImpl::InitParams(&shared_bitmap_manager_,
                                                  &output_surface_provider_)) {}
  ~RootCompositorFrameSinkImplTest() override = default;

  bool CompositorFrameSinkExists(const FrameSinkId& frame_sink_id) {
    return base::Contains(manager_.sink_map_, frame_sink_id) ||
           base::Contains(manager_.root_sink_map_, frame_sink_id);
  }

  void CreateTestCompositorFrameSink(const FrameSinkId& frame_sink_id,
                                     bool is_root) {
    manager_.RegisterFrameSinkId(frame_sink_id, true);
    RootCompositorFrameSinkData root_data;
    manager_.CreateRootCompositorFrameSink(
        root_data.BuildParams(frame_sink_id));
    EXPECT_TRUE(CompositorFrameSinkExists(frame_sink_id));
  }

  void TearDown() override {}

 protected:
  DebugRendererSettings debug_settings_;
  ServerSharedBitmapManager shared_bitmap_manager_;
  TestOutputSurfaceProvider output_surface_provider_;
  FrameSinkManagerImpl manager_;
};

class MockExternalBeginFrameSource : public ExternalBeginFrameSource {
 public:
  explicit MockExternalBeginFrameSource(
      MockExternalBeginFrameSourceClient* mock_client,
      uint32_t restart_id = ExternalBeginFrameSource::kNotRestartableId)
      : ExternalBeginFrameSource(mock_client, restart_id) {}
  ~MockExternalBeginFrameSource() override = default;
  MOCK_METHOD(void, SendInternalBeginFrame, (), (override));
};

TEST_F(RootCompositorFrameSinkImplTest, SendInternalBeginFrame_001) {
  constexpr FrameSinkId kFrameSinkIdA(1, 1);
  CreateTestCompositorFrameSink(kFrameSinkIdA, true);
  std::unique_ptr<MockExternalBeginFrameSourceClient> client =
      std::make_unique<MockExternalBeginFrameSourceClient>();
    manager_.root_sink_map_[kFrameSinkIdA]->external_begin_frame_source_=std::make_unique<MockExternalBeginFrameSource>(client.get(), 1);
  MockExternalBeginFrameSource* external_begin_frame_source;
  external_begin_frame_source = static_cast<MockExternalBeginFrameSource*>(manager_.root_sink_map_[kFrameSinkIdA]->external_begin_frame_source_.get());
  EXPECT_CALL(*external_begin_frame_source, SendInternalBeginFrame()).Times(1);
  manager_.root_sink_map_[kFrameSinkIdA]->SendInternalBeginFrame();
}

TEST_F(RootCompositorFrameSinkImplTest, SendInternalBeginFrame_002) {
  constexpr FrameSinkId kFrameSinkIdB(2, 1);
  CreateTestCompositorFrameSink(kFrameSinkIdB, true);
  std::unique_ptr<MockExternalBeginFrameSourceClient> client =
      std::make_unique<MockExternalBeginFrameSourceClient>();
  std::unique_ptr<MockExternalBeginFrameSource> external_begin_frame_source =
      std::make_unique<MockExternalBeginFrameSource>(client.get(), 1);
  manager_.root_sink_map_[kFrameSinkIdB]->external_begin_frame_source_.reset();
  EXPECT_CALL(*external_begin_frame_source, SendInternalBeginFrame()).Times(0);
  manager_.root_sink_map_[kFrameSinkIdB]->SendInternalBeginFrame();
}

TEST_F(RootCompositorFrameSinkImplTest, EvictFrameBackBuffers_001) {
  constexpr FrameSinkId kFrameSinkIdC(3, 1);
  CreateTestCompositorFrameSink(kFrameSinkIdC, true);
  manager_.root_sink_map_[kFrameSinkIdC]->EvictFrameBackBuffers(true);
  EXPECT_FALSE(manager_.root_sink_map_[kFrameSinkIdC]
                   ->display_->renderer_for_testing()
                   ->visible_);
}

TEST_F(RootCompositorFrameSinkImplTest, EvictFrameBackBuffers_002) {
  constexpr FrameSinkId kFrameSinkIdD(4, 1);
  CreateTestCompositorFrameSink(kFrameSinkIdD, true);
  manager_.root_sink_map_[kFrameSinkIdD]->EvictFrameBackBuffers(false);
  EXPECT_TRUE(manager_.root_sink_map_[kFrameSinkIdD]
                  ->display_->renderer_for_testing()
                  ->visible_);
}

}  // namespace viz
