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

#include "arkweb/chromium_ext/components/viz/service/frame_sinks/frame_sink_manager_impl_utils.h"
#include "components/viz/common/display/renderer_settings.h"
#include "components/viz/service/frame_sinks/root_compositor_frame_sink_impl_ext.h"
#include "components/viz/service/display/display.h"
#include "components/viz/service/display_embedder/server_shared_bitmap_manager.h"
#include "components/viz/test/mock_compositor_frame_sink_client.h"
#include "components/viz/test/mock_display_client.h"
#include "components/viz/test/test_output_surface_provider.h"
#include "components/viz/test/test_gpu_service_holder.h"
#include "mojo/public/cpp/bindings/associated_remote.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "services/viz/privileged/mojom/compositing/frame_sink_manager.mojom.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#include "components/viz/service/frame_sinks/root_compositor_frame_sink_impl.h"
#include "components/viz/service/frame_sinks/frame_sink_manager_impl.h"
#undef private

namespace viz {
namespace {
  constexpr FrameSinkId frameSinkId1(1, 1);
  constexpr FrameSinkId frameSinkId2(2, 2);

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
}

class MockFrameSinkManagerClient : public mojom::FrameSinkManagerClient {
 public:
  MockFrameSinkManagerClient() = default;

  MockFrameSinkManagerClient(const MockFrameSinkManagerClient&) = delete;
  MockFrameSinkManagerClient& operator=(
      const MockFrameSinkManagerClient&) = delete;

  ~MockFrameSinkManagerClient() = default;
  MOCK_METHOD(void,
              OnFirstSurfaceActivation,
              (const ::viz::SurfaceInfo& surface_info),
              (override));
  MOCK_METHOD(void,
              OnAggregatedHitTestRegionListUpdated,
              (const ::viz::FrameSinkId& frame_sink_id,
                const std::vector<::viz::AggregatedHitTestRegion>& hit_test_data),
              (override));
  MOCK_METHOD(void,
              OnFrameTokenChanged,
              (const ::viz::FrameSinkId& frame_sink_id,
                uint32_t frame_token,
                ::base::TimeTicks activation_time),
              (override));
  MOCK_METHOD(void,
              OnScreenshotCaptured,
              (const ::blink::SameDocNavigationScreenshotDestinationToken& destination_token,
                ::std::unique_ptr<::viz::CopyOutputResult> copy_output_result),
              (override));
  MOCK_METHOD(void,
              RestoreRenderFit,
              (uint32_t client_id, uint32_t sink_id),
              (override));
  mojo::PendingRemote<mojom::FrameSinkManagerClient> BindInterfaceRemote() {
  return receiver_.BindNewPipeAndPassRemote();
}

private:
  mojo::Receiver<mojom::FrameSinkManagerClient> receiver_{this};
};

class FrameSinkManagerImplUtilsTest : public testing::Test {
 public:
  FrameSinkManagerImplUtilsTest() = default;
  ~FrameSinkManagerImplUtilsTest() = default;

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
  void TestEvictFrameBackBuffers() {
    ServerSharedBitmapManager shared_bitmap_manager_;
    TestOutputSurfaceProvider output_surface_provider_;
    FrameSinkManagerImpl managerImpl(FrameSinkManagerImpl::
      InitParams(&shared_bitmap_manager_, &output_surface_provider_));
    FrameSinkManagerImplUtils utils_(&managerImpl);

    managerImpl.root_sink_map_.clear();
    ASSERT_NO_FATAL_FAILURE(utils_.EvictFrameBackBuffers(frameSinkId1));

    managerImpl.root_sink_map_.try_emplace(frameSinkId1, nullptr);
    ASSERT_NO_FATAL_FAILURE(utils_.EvictFrameBackBuffers(frameSinkId1));
    managerImpl.root_sink_map_.clear();

    RootCompositorFrameSinkData root_data;
    managerImpl.CreateRootCompositorFrameSink(root_data.BuildParams(frameSinkId1));
    for (auto &item : managerImpl.root_sink_map_) {
      EXPECT_NE(item.second, nullptr);
    }
    ASSERT_NO_FATAL_FAILURE(utils_.EvictFrameBackBuffers(frameSinkId1));
  }

  void TestSetIsOfflineWebComponentInactive() {
    ServerSharedBitmapManager shared_bitmap_manager_;
    TestOutputSurfaceProvider output_surface_provider_;
    FrameSinkManagerImpl managerImpl(FrameSinkManagerImpl::
      InitParams(&shared_bitmap_manager_, &output_surface_provider_));
    FrameSinkManagerImplUtils utils_(&managerImpl);

    managerImpl.root_sink_map_.clear();
    ASSERT_NO_FATAL_FAILURE(utils_.SetIsOfflineWebComponentInactive(true, frameSinkId1));

    RootCompositorFrameSinkData root_data;
    managerImpl.CreateRootCompositorFrameSink(root_data.BuildParams(frameSinkId1));
    for (auto &item : managerImpl.root_sink_map_) {
      EXPECT_NE(item.second, nullptr);
    }
    ASSERT_NO_FATAL_FAILURE(utils_.SetIsOfflineWebComponentInactive(false, frameSinkId1));
  }

  void TestSetRootCompositorFrameSink() {
    ServerSharedBitmapManager shared_bitmap_manager_;
    TestOutputSurfaceProvider output_surface_provider_;
    FrameSinkManagerImpl managerImpl(FrameSinkManagerImpl::
      InitParams(&shared_bitmap_manager_, &output_surface_provider_));
    FrameSinkManagerImplUtils utils_(&managerImpl);

    managerImpl.root_sink_map_.clear();
    ASSERT_NO_FATAL_FAILURE(utils_.SetRootCompositorFrameSink(frameSinkId1));

    RootCompositorFrameSinkData root_data;
    managerImpl.CreateRootCompositorFrameSink(root_data.BuildParams(frameSinkId1));
    for (auto &item : managerImpl.root_sink_map_) {
      EXPECT_NE(item.second, nullptr);
    }
    ASSERT_NO_FATAL_FAILURE(utils_.SetRootCompositorFrameSink(frameSinkId1));
  }

  void TestSetEnableLowerFrameRate() {
    ServerSharedBitmapManager shared_bitmap_manager_;
    TestOutputSurfaceProvider output_surface_provider_;
    FrameSinkManagerImpl managerImpl(FrameSinkManagerImpl::
      InitParams(&shared_bitmap_manager_, &output_surface_provider_));
    FrameSinkManagerImplUtils utils_(&managerImpl);

    managerImpl.root_sink_map_.clear();
    utils_.SetEnableLowerFrameRate(true, frameSinkId1);

    RootCompositorFrameSinkData root_data;
    managerImpl.CreateRootCompositorFrameSink(root_data.BuildParams(frameSinkId1));
    for (auto &item : managerImpl.root_sink_map_) {
      EXPECT_NE(item.second, nullptr);
    }
    ASSERT_NO_FATAL_FAILURE(utils_.SetEnableLowerFrameRate(true, frameSinkId1));
  }

  void TestSetEnableHalfFrameRate() {
    ServerSharedBitmapManager shared_bitmap_manager_;
    TestOutputSurfaceProvider output_surface_provider_;
    FrameSinkManagerImpl managerImpl(FrameSinkManagerImpl::
      InitParams(&shared_bitmap_manager_, &output_surface_provider_));
    FrameSinkManagerImplUtils utils_(&managerImpl);

    managerImpl.root_sink_map_.clear();
    ASSERT_NO_FATAL_FAILURE(utils_.SetEnableHalfFrameRate(true, frameSinkId1));

    RootCompositorFrameSinkData root_data;
    managerImpl.CreateRootCompositorFrameSink(root_data.BuildParams(frameSinkId1));
    for (auto &item : managerImpl.root_sink_map_) {
      EXPECT_NE(item.second, nullptr);
    }
    ASSERT_NO_FATAL_FAILURE(utils_.SetEnableHalfFrameRate(true, frameSinkId1));
  }
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
  void TestUpdateVSyncFrequency() {
    ServerSharedBitmapManager shared_bitmap_manager_;
    TestOutputSurfaceProvider output_surface_provider_;
    FrameSinkManagerImpl managerImpl(FrameSinkManagerImpl::
      InitParams(&shared_bitmap_manager_, &output_surface_provider_));
    FrameSinkManagerImplUtils utils_(&managerImpl);

    MockCompositorFrameSinkClient compositor_frame_sink_client;
    mojo::Remote<mojom::CompositorFrameSink> compositor_frame_sink;

    managerImpl.sink_map_.clear();
    utils_.UpdateVSyncFrequency(frameSinkId1, 1);

    managerImpl.CreateCompositorFrameSink(
      frameSinkId1,
      std::nullopt,
      compositor_frame_sink.BindNewPipeAndPassReceiver(),
      compositor_frame_sink_client.BindInterfaceRemote(),
      nullptr);
    for (auto &item : managerImpl.sink_map_) {
      EXPECT_NE(item.second, nullptr);
    }
    ASSERT_NO_FATAL_FAILURE(utils_.UpdateVSyncFrequency(frameSinkId1, 1));

    RootCompositorFrameSinkData root_data;
    managerImpl.CreateRootCompositorFrameSink(root_data.BuildParams(frameSinkId2));
    for (auto &item : managerImpl.root_sink_map_) {
      EXPECT_NE(item.second, nullptr);
    }
    ASSERT_NO_FATAL_FAILURE(utils_.UpdateVSyncFrequency(frameSinkId2, 2));
  }

  void TestResetVSyncFrequency() {
    ServerSharedBitmapManager shared_bitmap_manager_;
    TestOutputSurfaceProvider output_surface_provider_;
    FrameSinkManagerImpl managerImpl(FrameSinkManagerImpl::
      InitParams(&shared_bitmap_manager_, &output_surface_provider_));
    FrameSinkManagerImplUtils utils_(&managerImpl);

    managerImpl.root_sink_map_.clear();
    utils_.ResetVSyncFrequency(frameSinkId1);

    RootCompositorFrameSinkData root_data;
    managerImpl.CreateRootCompositorFrameSink(root_data.BuildParams(frameSinkId1));
    for (auto &item : managerImpl.root_sink_map_) {
      EXPECT_NE(item.second, nullptr);
    }
    ASSERT_NO_FATAL_FAILURE(utils_.ResetVSyncFrequency(frameSinkId1));
  }
#endif

#if BUILDFLAG(ARKWEB_PIP)
  void TestSetPipActive() {
    ServerSharedBitmapManager shared_bitmap_manager_;
    TestOutputSurfaceProvider output_surface_provider_;
    FrameSinkManagerImpl managerImpl(FrameSinkManagerImpl::
      InitParams(&shared_bitmap_manager_, &output_surface_provider_));
    FrameSinkManagerImplUtils utils_(&managerImpl);

    utils_.SetPipActive(true, frameSinkId1);
    RootCompositorFrameSinkData root_data;
    managerImpl.CreateRootCompositorFrameSink(root_data.BuildParams(frameSinkId1));
    for (auto &item : managerImpl.root_sink_map_) {
      EXPECT_NE(item.second, nullptr);
    }
    ASSERT_NO_FATAL_FAILURE(utils_.SetPipActive(true, frameSinkId1));
  }
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void TestClearBlanklessSnapshotInfo() {
    ServerSharedBitmapManager shared_bitmap_manager_;
    TestOutputSurfaceProvider output_surface_provider_;
    FrameSinkManagerImpl managerImpl(FrameSinkManagerImpl::
      InitParams(&shared_bitmap_manager_, &output_surface_provider_));
    FrameSinkManagerImplUtils utils_(&managerImpl);

    ASSERT_NO_FATAL_FAILURE(utils_.ClearBlanklessSnapshotInfo(1));
    managerImpl.gpu_service_ = TestGpuServiceHolder::GetInstance()->gpu_service();
    ASSERT_NO_FATAL_FAILURE(utils_.ClearBlanklessSnapshotInfo(1));
  }
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void TestRestoreRenderFit() {
    ServerSharedBitmapManager shared_bitmap_manager_;
    TestOutputSurfaceProvider output_surface_provider_;
    FrameSinkManagerImpl managerImpl(FrameSinkManagerImpl::
      InitParams(&shared_bitmap_manager_, &output_surface_provider_));
    FrameSinkManagerImplUtils utils_(&managerImpl);
    ASSERT_NO_FATAL_FAILURE(utils_.RestoreRenderFit(frameSinkId1));

    MockFrameSinkManagerClient mock_client;
    auto client = mock_client.BindInterfaceRemote();
    managerImpl.client_remote_.Bind(std::move(client));
    managerImpl.client_ = managerImpl.client_remote_.get();
    ASSERT_NO_FATAL_FAILURE(utils_.RestoreRenderFit(frameSinkId1));
    managerImpl.client_ = nullptr;
  }
#endif
};

TEST_F(FrameSinkManagerImplUtilsTest, CreateRootCompositorFrameSink) {
  ServerSharedBitmapManager shared_bitmap_manager_;
  TestOutputSurfaceProvider output_surface_provider_;
  FrameSinkManagerImpl managerImpl(FrameSinkManagerImpl::
    InitParams(&shared_bitmap_manager_, &output_surface_provider_));
  FrameSinkManagerImplUtils utils_(&managerImpl);

  managerImpl.root_sink_map_.clear();
  RootCompositorFrameSinkData root_data;
  managerImpl.CreateRootCompositorFrameSink(root_data.BuildParams(frameSinkId1));
  for (auto &item : managerImpl.root_sink_map_) {
    EXPECT_NE(item.second, nullptr);
  }
}

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
TEST_F(FrameSinkManagerImplUtilsTest, RestoreRenderFit) {
  TestRestoreRenderFit();
}
#endif

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
TEST_F(FrameSinkManagerImplUtilsTest, EvictFrameBackBuffers) {
  TestEvictFrameBackBuffers();
}

TEST_F(FrameSinkManagerImplUtilsTest, SetIsOfflineWebComponentInactive) {
  TestSetIsOfflineWebComponentInactive();
}

TEST_F(FrameSinkManagerImplUtilsTest, SetRootCompositorFrameSink) {
  TestSetRootCompositorFrameSink();
}

TEST_F(FrameSinkManagerImplUtilsTest, SetEnableLowerFrameRate) {
  TestSetEnableLowerFrameRate();
}

TEST_F(FrameSinkManagerImplUtilsTest, SetEnableHalfFrameRate) {
  TestSetEnableHalfFrameRate();
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
TEST_F(FrameSinkManagerImplUtilsTest, UpdateVSyncFrequency) {
  TestUpdateVSyncFrequency();
}

TEST_F(FrameSinkManagerImplUtilsTest, ResetVSyncFrequency) {
  TestResetVSyncFrequency();
}
#endif

#if BUILDFLAG(ARKWEB_PIP)
TEST_F(FrameSinkManagerImplUtilsTest, SetPipActive) {
  TestSetPipActive();
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
TEST_F(FrameSinkManagerImplUtilsTest, ClearBlanklessSnapshotInfo) {
  TestClearBlanklessSnapshotInfo();
}
#endif
} // namespace viz