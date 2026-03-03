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

#include "arkweb/chromium_ext/components/viz/host/host_frame_sink_manager_utils.h"

#include "arkweb/build/features/features.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace viz {

class MockFrameSinkManager : public mojom::FrameSinkManager {
 public:
  MockFrameSinkManager() = default;
  ~MockFrameSinkManager() override = default;

  void RegisterFrameSinkId(const ::viz::FrameSinkId& frame_sink_id,
                           bool report_activation) override {}
  void InvalidateFrameSinkId(const ::viz::FrameSinkId& frame_sink_id) override {
  }
  void SetFrameSinkDebugLabel(const ::viz::FrameSinkId& frame_sink_id,
                              const std::string& debug_label) override {}
  void CreateRootCompositorFrameSink(
      mojom::RootCompositorFrameSinkParamsPtr params) override {}
  void CreateFrameSinkBundle(
      const ::viz::FrameSinkBundleId& bundle_id,
      ::mojo::PendingReceiver<::viz::mojom::FrameSinkBundle> receiver,
      ::mojo::PendingRemote<::viz::mojom::FrameSinkBundleClient> client)
      override {}
  void CreateCompositorFrameSink(
      const ::viz::FrameSinkId& frame_sink_id,
      const std::optional<::viz::FrameSinkBundleId>& bundle_id,
      ::mojo::PendingReceiver<::viz::mojom::CompositorFrameSink>
          compositor_frame_sink,
      ::mojo::PendingRemote<::viz::mojom::CompositorFrameSinkClient>
          compositor_frame_sink_client,
      ::input::mojom::RenderInputRouterConfigPtr config) override {}
  void DestroyCompositorFrameSink(
      const ::viz::FrameSinkId& frame_sink_id,
      DestroyCompositorFrameSinkCallback callback) override {}
  void RegisterFrameSinkHierarchy(
      const ::viz::FrameSinkId& parent_frame_sink_id,
      const ::viz::FrameSinkId& child_frame_sink_id) override {}
  void UnregisterFrameSinkHierarchy(
      const ::viz::FrameSinkId& parent_frame_sink_id,
      const ::viz::FrameSinkId& child_frame_sink_id) override {}
  void AddVideoDetectorObserver(
      ::mojo::PendingRemote<::viz::mojom::VideoDetectorObserver> observer)
      override {}
  void CreateVideoCapturer(
      ::mojo::PendingReceiver<::viz::mojom::FrameSinkVideoCapturer> receiver)
      override {}
  void EvictSurfaces(
      const std::vector<::viz::SurfaceId>& surface_ids) override {}
  void Throttle(const std::vector<::viz::FrameSinkId>& frame_sink_ids,
                ::base::TimeDelta interval) override {}
  void StartThrottlingAllFrameSinks(::base::TimeDelta interval) override {}
  void StopThrottlingAllFrameSinks() override {}
  void RequestCopyOfOutput(const ::viz::SurfaceId& surface_id,
                           ::std::unique_ptr<::viz::CopyOutputRequest> request,
                           bool capture_exact_surface_id) override {}
  void UpdateDebugRendererSettings(
      const ::viz::DebugRendererSettings& debug_settings) override {}
  void ClearUnclaimedViewTransitionResources(
      const ::blink::ViewTransitionToken& transition_token) override {}
  void CreateMetricsRecorderForTest(
      ::mojo::PendingReceiver<::viz::mojom::FrameSinksMetricsRecorder> receiver)
      override {}
  void EnableFrameSinkManagerTestApi(
      ::mojo::PendingReceiver<::viz::mojom::FrameSinkManagerTestApi> receiver)
      override {}
  void SetupRenderInputRouterDelegateConnection(
      uint32_t grouping_id,
      ::mojo::PendingRemote<::input::mojom::RenderInputRouterDelegateClient>
          rir_delegate_client_remote) override {}
  MOCK_METHOD(void,
              SetEnableLowerFrameRate,
              (bool enabled, const ::viz::FrameSinkId& frame_sink_id),
              (override));
  MOCK_METHOD(void,
              SetEnableHalfFrameRate,
              (bool enabled, const ::viz::FrameSinkId& frame_sink_id),
              (override));
  MOCK_METHOD(void,
              UpdateVSyncFrequency,
              (const ::viz::FrameSinkId& frame_sink_id, uint32_t client_id),
              (override));
  MOCK_METHOD(void,
              ResetVSyncFrequency,
              (const ::viz::FrameSinkId& frame_sink_id),
              (override));
  MOCK_METHOD(void,
              SendInternalBeginFrame,
              (const ::viz::FrameSinkId& frame_sink_id),
              (override));
  MOCK_METHOD(void,
              EvictFrameBackBuffers,
              (const ::viz::FrameSinkId& frame_sink_id),
              (override));
  MOCK_METHOD(void,
              SetIsOfflineWebComponentInactive,
              (bool is_inactive, const ::viz::FrameSinkId& frame_sink_id),
              (override));
  MOCK_METHOD(void,
              SetPipActive,
              (bool active, const ::viz::FrameSinkId& frame_sink_id),
              (override));
  MOCK_METHOD(void,
              ClearBlanklessSnapshotInfo,
              (uint64_t blankless_key),
              (override));
};

class MockHostFrameSinkClient : public HostFrameSinkClient {
 public:
  MockHostFrameSinkClient() = default;
  ~MockHostFrameSinkClient() override = default;

  void OnFirstSurfaceActivation(const SurfaceInfo& surface_info) override {}
  virtual void OnFrameTokenChanged(uint32_t frame_token,
                                   base::TimeTicks activation_time) override {}

  MOCK_METHOD(void, RestoreRenderFit, (), (override));
};

class HostFrameSinkManagerUtilsTest : public testing::Test {
 public:
  HostFrameSinkManagerUtilsTest() = default;
  ~HostFrameSinkManagerUtilsTest() = default;

  void SetUp() override {
    frame_sink_manager_ = std::make_shared<MockFrameSinkManager>();
    host_frame_sink_client = std::make_shared<MockHostFrameSinkClient>();
  }

  void TearDown() override {
    if (host_frame_sink_manager_) {
      delete host_frame_sink_manager_;
      host_frame_sink_manager_ = nullptr;
    }
    if (host_frame_sink_manager_data_) {
      delete host_frame_sink_manager_data_;
      host_frame_sink_manager_data_ = nullptr;
    }
  }

  std::shared_ptr<MockFrameSinkManager> FrameSinkManager() {
    return frame_sink_manager_;
  }

  std::shared_ptr<MockHostFrameSinkClient> HostClient() {
    return host_frame_sink_client;
  }

  HostFrameSinkManager* HostManager() {
    if (!host_frame_sink_manager_) {
      host_frame_sink_manager_ = new HostFrameSinkManager();
      host_frame_sink_manager_->SetLocalManager(frame_sink_manager_.get());
    }
    return host_frame_sink_manager_;
  }

  HostFrameSinkManager* HostManagerWithData(const FrameSinkId& frame_sink_id,
                                            bool with_child = false,
                                            bool with_client = false) {
    if (!host_frame_sink_manager_data_) {
      host_frame_sink_manager_data_ = new HostFrameSinkManager();
      host_frame_sink_manager_data_->SetLocalManager(frame_sink_manager_.get());
    }
    auto& data = host_frame_sink_manager_data_->frame_sink_data_map_[frame_sink_id];
    if (with_child) {
      data.children.push_back(frame_sink_id);
    } else {
      data.children.clear();
    }
    if (with_client) {
      data.client = host_frame_sink_client.get();
    } else {
      data.client = nullptr;
    }
    return host_frame_sink_manager_data_;
  }

 private:
  std::shared_ptr<MockFrameSinkManager> frame_sink_manager_;
  std::shared_ptr<MockHostFrameSinkClient> host_frame_sink_client;
  HostFrameSinkManager* host_frame_sink_manager_ = nullptr;
  HostFrameSinkManager* host_frame_sink_manager_data_ = nullptr;
};

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
TEST_F(HostFrameSinkManagerUtilsTest, SendInternalBeginFrame) {
  HostFrameSinkManagerUtils utils(HostManager());
  const FrameSinkId sink_id(0, 0);
  EXPECT_CALL(*FrameSinkManager(), SendInternalBeginFrame);
  utils.SendInternalBeginFrame(sink_id);
}
#endif

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
TEST_F(HostFrameSinkManagerUtilsTest, SetEnableLowerFrameRate) {
  HostFrameSinkManagerUtils utils(HostManager());
  bool enabled = true;
  const FrameSinkId sink_id(0, 0);
  EXPECT_CALL(*FrameSinkManager(), SetEnableLowerFrameRate);
  utils.SetEnableLowerFrameRate(enabled, sink_id);
}

TEST_F(HostFrameSinkManagerUtilsTest, SetEnableHalfFrameRate) {
  HostFrameSinkManagerUtils utils(HostManager());
  bool enabled = true;
  const FrameSinkId sink_id(0, 0);
  EXPECT_CALL(*FrameSinkManager(), SetEnableHalfFrameRate);
  utils.SetEnableHalfFrameRate(enabled, sink_id);
}

TEST_F(HostFrameSinkManagerUtilsTest, EvictFrameBackBuffers) {
  HostFrameSinkManagerUtils utils(HostManager());
  const FrameSinkId sink_id(0, 0);
  EXPECT_CALL(*FrameSinkManager(), EvictFrameBackBuffers);
  utils.EvictFrameBackBuffers(sink_id);
}

TEST_F(HostFrameSinkManagerUtilsTest, SetIsOfflineWebComponentInactive) {
  HostFrameSinkManagerUtils utils(HostManager());
  bool is_inactive = false;
  const FrameSinkId sink_id(0, 0);
  EXPECT_CALL(*FrameSinkManager(), SetIsOfflineWebComponentInactive);
  utils.SetIsOfflineWebComponentInactive(is_inactive, sink_id);
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
TEST_F(HostFrameSinkManagerUtilsTest, UpdateVSyncFrequency_001) {
  HostFrameSinkManagerUtils utils(HostManager());
  const FrameSinkId sink_id(0, 0);
  EXPECT_CALL(*FrameSinkManager(), UpdateVSyncFrequency).Times(0);
  utils.UpdateVSyncFrequency(sink_id);
}

TEST_F(HostFrameSinkManagerUtilsTest, UpdateVSyncFrequency_002) {
  const FrameSinkId sink_id(0, 0);
  HostFrameSinkManagerUtils utils(HostManagerWithData(sink_id, false));
  EXPECT_CALL(*FrameSinkManager(), UpdateVSyncFrequency);
  utils.UpdateVSyncFrequency(sink_id);
}

TEST_F(HostFrameSinkManagerUtilsTest, UpdateVSyncFrequency_003) {
  const FrameSinkId sink_id(0, 0);
  HostFrameSinkManagerUtils utils(HostManagerWithData(sink_id, true));
  EXPECT_CALL(*FrameSinkManager(), UpdateVSyncFrequency);
  utils.UpdateVSyncFrequency(sink_id);
}

TEST_F(HostFrameSinkManagerUtilsTest, ResetVSyncFrequency) {
  HostFrameSinkManagerUtils utils(HostManager());
  const FrameSinkId sink_id(0, 0);
  EXPECT_CALL(*FrameSinkManager(), ResetVSyncFrequency);
  utils.ResetVSyncFrequency(sink_id);
}
#endif

#if BUILDFLAG(ARKWEB_PIP)
TEST_F(HostFrameSinkManagerUtilsTest, SetPipActive) {
  HostFrameSinkManagerUtils utils(HostManager());
  bool active = true;
  const FrameSinkId sink_id(0, 0);
  EXPECT_CALL(*FrameSinkManager(), SetPipActive);
  utils.SetPipActive(active, sink_id);
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
TEST_F(HostFrameSinkManagerUtilsTest, ClearBlanklessSnapshotInfo_001) {
  HostFrameSinkManager host_manager;
  HostFrameSinkManagerUtils utils(&host_manager);
  uint64_t blankless_key = 0;
  EXPECT_CALL(*FrameSinkManager(), ClearBlanklessSnapshotInfo).Times(0);
  utils.ClearBlanklessSnapshotInfo(blankless_key);
}

TEST_F(HostFrameSinkManagerUtilsTest, ClearBlanklessSnapshotInfo_002) {
  HostFrameSinkManagerUtils utils(HostManager());
  uint64_t blankless_key = 0;
  EXPECT_CALL(*FrameSinkManager(), ClearBlanklessSnapshotInfo);
  utils.ClearBlanklessSnapshotInfo(blankless_key);
}
#endif

TEST_F(HostFrameSinkManagerUtilsTest, UtilsRestoreRenderFit_001) {
  HostFrameSinkManagerUtils utils(HostManager());
  EXPECT_CALL(*HostClient(), RestoreRenderFit).Times(0);
  utils.UtilsRestoreRenderFit(0, 0);
}

TEST_F(HostFrameSinkManagerUtilsTest, UtilsRestoreRenderFit_002) {
  const FrameSinkId sink_id(0, 0);
  HostFrameSinkManagerUtils utils(HostManagerWithData(sink_id, false, false));
  EXPECT_CALL(*HostClient(), RestoreRenderFit).Times(0);
  utils.UtilsRestoreRenderFit(sink_id.client_id(), sink_id.sink_id());
}

TEST_F(HostFrameSinkManagerUtilsTest, UtilsRestoreRenderFit_003) {
  const FrameSinkId sink_id(0, 0);
  HostFrameSinkManagerUtils utils(HostManagerWithData(sink_id, false, true));
  EXPECT_CALL(*HostClient(), RestoreRenderFit).Times(1);
  utils.UtilsRestoreRenderFit(sink_id.client_id(), sink_id.sink_id());
}
}  // namespace viz
