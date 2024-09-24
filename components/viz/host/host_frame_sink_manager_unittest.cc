// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(OHOS_UNITTESTS)
#define private public
#endif // OHOS_UNITTESTS
#include "components/viz/host/host_frame_sink_manager.h"
#if defined(OHOS_UNITTESTS)
#undef private
#endif // OHOS_UNITTESTS

#include <memory>
#include <utility>

#include "base/run_loop.h"
#include "components/viz/common/display/renderer_settings.h"
#include "components/viz/common/surfaces/frame_sink_id.h"
#include "components/viz/common/surfaces/surface_id.h"
#include "components/viz/common/surfaces/surface_info.h"
#include "components/viz/test/compositor_frame_helpers.h"
#include "components/viz/test/fake_host_frame_sink_client.h"
#include "components/viz/test/fake_surface_observer.h"
#include "components/viz/test/mock_compositor_frame_sink_client.h"
#include "components/viz/test/mock_display_client.h"
#include "components/viz/test/test_frame_sink_manager.h"
#include "mojo/public/cpp/bindings/associated_remote.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "services/viz/privileged/mojom/compositing/frame_sink_manager.mojom.h"
#include "services/viz/public/mojom/compositing/compositor_frame_sink.mojom.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using testing::_;

namespace viz {
namespace {

constexpr FrameSinkId kFrameSinkParent1(1, 1);
constexpr FrameSinkId kFrameSinkParent2(2, 1);
constexpr FrameSinkId kFrameSinkChild1(3, 1);

// Holds the four interface objects needed to create a RootCompositorFrameSink.
struct RootCompositorFrameSinkData {
  mojom::RootCompositorFrameSinkParamsPtr BuildParams(
      const FrameSinkId& frame_sink_id) {
    auto params = mojom::RootCompositorFrameSinkParams::New();
    params->gpu_compositing = false;
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

// A mock implementation of mojom::FrameSinkManager.
class MockFrameSinkManagerImpl : public TestFrameSinkManagerImpl {
 public:
  MockFrameSinkManagerImpl() = default;
  ~MockFrameSinkManagerImpl() override = default;

  // mojom::FrameSinkManager:
  MOCK_METHOD2(RegisterFrameSinkId,
               void(const FrameSinkId& frame_sink_id, bool report_activation));
  MOCK_METHOD1(InvalidateFrameSinkId, void(const FrameSinkId& frame_sink_id));
  MOCK_METHOD2(SetFrameSinkDebugLabel,
               void(const FrameSinkId& frame_sink_id,
                    const std::string& debug_label));
  MOCK_METHOD4(CreateCompositorFrameSink,
               void(const FrameSinkId&,
                    const absl::optional<FrameSinkBundleId>&,
                    mojo::PendingReceiver<mojom::CompositorFrameSink>,
                    mojo::PendingRemote<mojom::CompositorFrameSinkClient>));
  void CreateRootCompositorFrameSink(
      mojom::RootCompositorFrameSinkParamsPtr params) override {
    MockCreateRootCompositorFrameSink(params->frame_sink_id);
  }
  MOCK_METHOD1(MockCreateRootCompositorFrameSink,
               void(const FrameSinkId& frame_sink_id));
  void DestroyCompositorFrameSink(
      const FrameSinkId& frame_sink_id,
      DestroyCompositorFrameSinkCallback callback) override {
    MockDestroyCompositorFrameSink(frame_sink_id);
    std::move(callback).Run();
  }
  MOCK_METHOD1(MockDestroyCompositorFrameSink,
               void(const FrameSinkId& frame_sink_id));
  MOCK_METHOD2(RegisterFrameSinkHierarchy,
               void(const FrameSinkId& parent, const FrameSinkId& child));
  MOCK_METHOD2(UnregisterFrameSinkHierarchy,
               void(const FrameSinkId& parent, const FrameSinkId& child));
  MOCK_METHOD(void,
              Throttle,
              (const std::vector<FrameSinkId>& ids, base::TimeDelta interval),
              (override));
#if defined(OHOS_UNITTESTS)
  MOCK_METHOD2(UpdateVSyncFrequency,
              void(const FrameSinkId& frame_sink_id, uint32_t client_id));
  MOCK_METHOD1(ResetVSyncFrequency,
              void(const FrameSinkId& frame_sink_id));
#endif // OHOS_UNITTESTS
};

}  // namespace

class HostFrameSinkManagerTest : public testing::Test {
 public:
  HostFrameSinkManagerTest() { ConnectToGpu(); }

  HostFrameSinkManagerTest(const HostFrameSinkManagerTest&) = delete;
  HostFrameSinkManagerTest& operator=(const HostFrameSinkManagerTest&) = delete;

  ~HostFrameSinkManagerTest() override = default;

  HostFrameSinkManager& host() { return host_manager_; }
  MockFrameSinkManagerImpl& impl() { return *manager_impl_; }

  mojom::FrameSinkManagerClient* GetFrameSinkManagerClient() {
    return static_cast<mojom::FrameSinkManagerClient*>(&host_manager_);
  }

  bool FrameSinkDataExists(const FrameSinkId& frame_sink_id) const {
    return host_manager_.frame_sink_data_map_.count(frame_sink_id) > 0;
  }

  bool IsBoundToFrameSinkManager() {
    return host_manager_.frame_sink_manager_remote_.is_bound() ||
           host_manager_.receiver_.is_bound();
  }

  bool DisplayHitTestQueryExists(const FrameSinkId& frame_sink_id) {
    return host_manager_.display_hit_test_query_.count(frame_sink_id) > 0;
  }

  void RegisterFrameSinkIdWithFakeClient(
      const FrameSinkId& frame_sink_id,
      ReportFirstSurfaceActivation report_activation) {
    host_manager_.RegisterFrameSinkId(frame_sink_id, &host_client_,
                                      report_activation);
  }

  void FlushHostAndVerifyExpectations() {
    host_manager_.frame_sink_manager_remote_.FlushForTesting();
    testing::Mock::VerifyAndClearExpectations(&impl());
  }

  // Destroys FrameSinkManagerImpl which kills the message pipes.
  void KillGpu() { manager_impl_.reset(); }

  // Connects HostFrameSinkManager to FrameSinkManagerImpl using Mojo.
  void ConnectToGpu() {
    DCHECK(!manager_impl_);

    manager_impl_ =
        std::make_unique<testing::NiceMock<MockFrameSinkManagerImpl>>();

    mojo::PendingRemote<mojom::FrameSinkManager> frame_sink_manager;
    mojo::PendingReceiver<mojom::FrameSinkManager> frame_sink_manager_receiver =
        frame_sink_manager.InitWithNewPipeAndPassReceiver();
    mojo::PendingRemote<mojom::FrameSinkManagerClient>
        frame_sink_manager_client;
    mojo::PendingReceiver<mojom::FrameSinkManagerClient>
        frame_sink_manager_client_receiver =
            frame_sink_manager_client.InitWithNewPipeAndPassReceiver();

    host_manager_.BindAndSetManager(
        std::move(frame_sink_manager_client_receiver), nullptr,
        std::move(frame_sink_manager));
    manager_impl_->BindReceiver(std::move(frame_sink_manager_receiver),
                                std::move(frame_sink_manager_client));
  }

 protected:
  FakeHostFrameSinkClient host_client_;  // Must outlive `host_manager_`
  HostFrameSinkManager host_manager_;
  std::unique_ptr<testing::NiceMock<MockFrameSinkManagerImpl>> manager_impl_;
};

#if defined(OHOS_UNITTESTS)
TEST_F(HostFrameSinkManagerTest, UpdateVSyncFrequency001) {
  uint32_t mock_client_id = 0;
  uint32_t mock_sink_id = 0;
  FrameSinkId id(mock_client_id, mock_sink_id);
  auto iter_id = host_manager_.frame_sink_data_map_.find(id);
  auto iter_end = host_manager_.frame_sink_data_map_.end();
  
  host_manager_.UpdateVSyncFrequency(id);
  EXPECT_EQ(iter_id,iter_end);
}

TEST_F(HostFrameSinkManagerTest, UpdateVSyncFrequency002) {
  uint32_t mock_client_id = 1;
  uint32_t mock_sink_id = 1;
  uint32_t mock_client_id2 = 2;
  uint32_t mock_sink_id2 = 2;
  FrameSinkId id1(mock_client_id,mock_sink_id);
  FrameSinkId id2(mock_client_id2,mock_sink_id2);
  HostFrameSinkManager::FrameSinkData data1;
  HostFrameSinkManager::FrameSinkData data2;
  host_manager_.frame_sink_data_map_.emplace(std::move(id1), std::move(data1));
  host_manager_.frame_sink_data_map_.emplace(std::move(id2), std::move(data2));
  auto iter_id = host_manager_.frame_sink_data_map_.find(id1);
  auto iter_end = host_manager_.frame_sink_data_map_.end();

  host_manager_.UpdateVSyncFrequency(id1);
  EXPECT_NE(iter_id, iter_end);
}

TEST_F(HostFrameSinkManagerTest, UpdateVSyncFrequency003) {
  uint32_t mock_client_id = 1;
  uint32_t mock_sink_id = 1;
  uint32_t mock_client_id2 = 2;
  uint32_t mock_sink_id2 = 2;
  FrameSinkId id1(mock_client_id,mock_sink_id);
  FrameSinkId id2(mock_client_id2,mock_sink_id2);
  HostFrameSinkManager::FrameSinkData data1;
  HostFrameSinkManager::FrameSinkData data2;
  host_manager_.frame_sink_data_map_.emplace(std::move(id1), std::move(data1));
  host_manager_.frame_sink_data_map_.emplace(std::move(id2), std::move(data2));
  auto iter_id = host_manager_.frame_sink_data_map_.find(id1);
  auto mock_children = iter_id->second.children;
  uint32_t expect_size = 0;

  host_manager_.UpdateVSyncFrequency(id1);
  EXPECT_EQ(expect_size,mock_children.size());
  EXPECT_EQ(mock_children.begin(),mock_children.end());
}

TEST_F(HostFrameSinkManagerTest, UpdateVSyncFrequency004) {
  uint32_t mock_client_id = 1;
  uint32_t mock_sink_id = 1;
  uint32_t mock_client_id2 = 2;
  uint32_t mock_sink_id2 = 1;
  uint32_t mock_client_id3 = 3;
  uint32_t mock_sink_id3 = 1;
  FrameSinkId id1(mock_client_id,mock_sink_id);
  FrameSinkId id2(mock_client_id2,mock_sink_id2);
  FrameSinkId id3(mock_client_id3,mock_sink_id3);
  HostFrameSinkManager::FrameSinkData data1;
  HostFrameSinkManager::FrameSinkData data2;
  HostFrameSinkManager::FrameSinkData data3;
  host_manager_.frame_sink_data_map_.emplace(std::move(id1), std::move(data1));
  host_manager_.frame_sink_data_map_.emplace(std::move(id2), std::move(data2));
  host_manager_.frame_sink_data_map_.emplace(std::move(id3), std::move(data3));
  auto iter_id = host_manager_.frame_sink_data_map_.find(id1);
  iter_id->second.children.push_back(id2);
  iter_id->second.children.push_back(id3);
  auto mock_children = iter_id->second.children;
  uint32_t expect_size = 2;

  host_manager_.UpdateVSyncFrequency(id1);
  EXPECT_EQ(expect_size,mock_children.size());
  EXPECT_NE(mock_children.begin(),mock_children.end());
}

TEST_F(HostFrameSinkManagerTest, OnVsync001) {
  uint32_t mock_client_id = 0;
  uint32_t mock_sink_id = 0;
  FrameSinkId id(mock_client_id, mock_sink_id);
  auto iter_id = host_manager_.frame_sink_data_map_.find(id);
  auto iter_end = host_manager_.frame_sink_data_map_.end();

  host_manager_.OnVsync(mock_client_id,mock_sink_id);
  EXPECT_EQ(iter_id,iter_end);
}

TEST_F(HostFrameSinkManagerTest, OnVsync002) {
  uint32_t mock_client_id = 0;
  uint32_t mock_sink_id = 0;
  FrameSinkId id(mock_client_id, mock_sink_id);
  host_manager_.frame_sink_data_map_.emplace();
  auto iter_id = host_manager_.frame_sink_data_map_.find(id);
  auto iter_end = host_manager_.frame_sink_data_map_.end();

  host_manager_.OnVsync(mock_client_id,mock_sink_id);
  EXPECT_NE(iter_id,iter_end);
}

TEST_F(HostFrameSinkManagerTest, OnVsync003) {
    uint32_t mock_client_id = 1;
    uint32_t mock_sink_id = 1;
    uint32_t mock_client_id2 = 2;
    uint32_t mock_sink_id2 = 2;
    FrameSinkId id1(mock_client_id,mock_sink_id);
    FrameSinkId id2(mock_client_id2,mock_sink_id2);
    HostFrameSinkManager::FrameSinkData data1;
    HostFrameSinkManager::FrameSinkData data2;
    host_manager_.frame_sink_data_map_.emplace(std::move(id1), std::move(data1));
    host_manager_.frame_sink_data_map_.emplace(std::move(id2), std::move(data2));
    auto iter = host_manager_.frame_sink_data_map_.find(id1);
    iter->second.client = nullptr;
    const HostFrameSinkManager::FrameSinkData& data = iter->second;
    
    host_manager_.OnVsync(mock_client_id,mock_sink_id);
    EXPECT_EQ(nullptr,data.client);
}

TEST_F(HostFrameSinkManagerTest, OnVsync004){
    uint32_t mock_client_id = 1;
    uint32_t mock_sink_id = 1;
    uint32_t mock_client_id2 = 2;
    uint32_t mock_sink_id2 = 2;
    FrameSinkId id1(mock_client_id,mock_sink_id);
    FrameSinkId id2(mock_client_id2,mock_sink_id2);
    HostFrameSinkManager::FrameSinkData data1;
    HostFrameSinkManager::FrameSinkData data2;
    host_manager_.frame_sink_data_map_.emplace(std::move(id1), std::move(data1));
    host_manager_.frame_sink_data_map_.emplace(std::move(id2), std::move(data2));
    auto iter = host_manager_.frame_sink_data_map_.find(id1);
    FakeHostFrameSinkClient mock_client;
    iter->second.client = &mock_client;
    const HostFrameSinkManager::FrameSinkData& data = iter->second;

    host_manager_.OnVsync(mock_client_id,mock_sink_id);
    EXPECT_NE(data.client,nullptr);

}

TEST_F(HostFrameSinkManagerTest, OnVsyncReceived001) {
  uint32_t mock_client_id = 0;
  uint32_t mock_sink_id = 0;
  FrameSinkId id(mock_client_id, mock_sink_id);
  auto iter_id = host_manager_.frame_sink_data_map_.find(id);
  auto iter_end = host_manager_.frame_sink_data_map_.end();
  
  host_manager_.OnVsyncReceived(mock_client_id,mock_sink_id);
  EXPECT_EQ(iter_id,iter_end);
}

TEST_F(HostFrameSinkManagerTest, OnVsyncReceived002) {
  uint32_t mock_client_id = 0;
  uint32_t mock_sink_id = 0;
  FrameSinkId id(mock_client_id, mock_sink_id);
  host_manager_.frame_sink_data_map_.emplace();
  auto iter_id = host_manager_.frame_sink_data_map_.find(id);
  auto iter_end = host_manager_.frame_sink_data_map_.end();

  host_manager_.OnVsyncReceived(mock_client_id,mock_sink_id);
  EXPECT_NE(iter_id,iter_end);
}

TEST_F(HostFrameSinkManagerTest, OnVsyncReceived003) {
    uint32_t mock_client_id = 1;
    uint32_t mock_sink_id = 1;
    uint32_t mock_client_id2 = 2;
    uint32_t mock_sink_id2 = 2;
    FrameSinkId id1(mock_client_id,mock_sink_id);
    FrameSinkId id2(mock_client_id2,mock_sink_id2);
    HostFrameSinkManager::FrameSinkData data1;
    HostFrameSinkManager::FrameSinkData data2;
    host_manager_.frame_sink_data_map_.emplace(std::move(id1), std::move(data1));
    host_manager_.frame_sink_data_map_.emplace(std::move(id2), std::move(data2));
    auto iter = host_manager_.frame_sink_data_map_.find(id1);
    iter->second.client = nullptr;
    const HostFrameSinkManager::FrameSinkData& data = iter->second;
    
    host_manager_.OnVsyncReceived(mock_client_id,mock_sink_id);
    EXPECT_EQ(data.client,nullptr);
}

TEST_F(HostFrameSinkManagerTest, OnVsyncReceived004){
    uint32_t mock_client_id = 1;
    uint32_t mock_sink_id = 1;
    uint32_t mock_client_id2 = 2;
    uint32_t mock_sink_id2 = 2;
    FrameSinkId id1(mock_client_id,mock_sink_id);
    FrameSinkId id2(mock_client_id2,mock_sink_id2);
    HostFrameSinkManager::FrameSinkData data1;
    HostFrameSinkManager::FrameSinkData data2;
    host_manager_.frame_sink_data_map_.emplace(std::move(id1), std::move(data1));
    host_manager_.frame_sink_data_map_.emplace(std::move(id2), std::move(data2));
    auto iter = host_manager_.frame_sink_data_map_.find(id1);
    FakeHostFrameSinkClient mock_client;
    iter->second.client = &mock_client;
    const HostFrameSinkManager::FrameSinkData& data = iter->second;

    host_manager_.OnVsyncReceived(mock_client_id,mock_sink_id);
    EXPECT_NE(data.client,nullptr);
}
#endif // OHOS_UNITTESTS

// Verify that registering and destroying multiple CompositorFrameSinks works
// correctly when one of the CompositorFrameSinks hasn't been created.
TEST_F(HostFrameSinkManagerTest, CreateCompositorFrameSinks) {
  // Register then create CompositorFrameSink for child.
  EXPECT_CALL(impl(), RegisterFrameSinkId(kFrameSinkChild1,
                                          true /* report_activation */));
  RegisterFrameSinkIdWithFakeClient(kFrameSinkChild1,
                                    ReportFirstSurfaceActivation::kYes);
  EXPECT_TRUE(FrameSinkDataExists(kFrameSinkChild1));

  MockCompositorFrameSinkClient compositor_frame_sink_client;
  mojo::Remote<mojom::CompositorFrameSink> compositor_frame_sink;
  EXPECT_CALL(impl(), CreateCompositorFrameSink(kFrameSinkChild1, _, _, _));
  host().CreateCompositorFrameSink(
      kFrameSinkChild1, compositor_frame_sink.BindNewPipeAndPassReceiver(),
      compositor_frame_sink_client.BindInterfaceRemote());
  FlushHostAndVerifyExpectations();

  // Register but don't actually create CompositorFrameSink for parent.
  RegisterFrameSinkIdWithFakeClient(kFrameSinkParent1,
                                    ReportFirstSurfaceActivation::kYes);

  // Register should call through to FrameSinkManagerImpl and should work even
  // though |kFrameSinkParent1| was not created yet.
  EXPECT_CALL(impl(),
              RegisterFrameSinkHierarchy(kFrameSinkParent1, kFrameSinkChild1));
  host().RegisterFrameSinkHierarchy(kFrameSinkParent1, kFrameSinkChild1);

  // Destroy the CompositorFrameSink.
  EXPECT_CALL(impl(), InvalidateFrameSinkId(kFrameSinkChild1));
  host().InvalidateFrameSinkId(kFrameSinkChild1);
  FlushHostAndVerifyExpectations();

  // Unregister should work after the CompositorFrameSink is destroyed.
  EXPECT_CALL(impl(), UnregisterFrameSinkHierarchy(kFrameSinkParent1,
                                                   kFrameSinkChild1));
  host().UnregisterFrameSinkHierarchy(kFrameSinkParent1, kFrameSinkChild1);
  FlushHostAndVerifyExpectations();

  // Data for |kFrameSinkChild1| should be deleted now.
  EXPECT_FALSE(FrameSinkDataExists(kFrameSinkChild1));
}

// Verify that that creating two RootCompositorFrameSinks works.
TEST_F(HostFrameSinkManagerTest, CreateRootCompositorFrameSinks) {
  RegisterFrameSinkIdWithFakeClient(kFrameSinkChild1,
                                    ReportFirstSurfaceActivation::kYes);
  RootCompositorFrameSinkData root_data1;
  host().CreateRootCompositorFrameSink(
      root_data1.BuildParams(kFrameSinkChild1));
  EXPECT_TRUE(FrameSinkDataExists(kFrameSinkChild1));

  RegisterFrameSinkIdWithFakeClient(kFrameSinkParent1,
                                    ReportFirstSurfaceActivation::kYes);
  RootCompositorFrameSinkData root_data2;
  host().CreateRootCompositorFrameSink(
      root_data2.BuildParams(kFrameSinkParent1));
  EXPECT_TRUE(FrameSinkDataExists(kFrameSinkParent1));

  // Verify that registering and unregistering frame sink hierarchy works.
  EXPECT_CALL(impl(),
              RegisterFrameSinkHierarchy(kFrameSinkParent1, kFrameSinkChild1));
  host().RegisterFrameSinkHierarchy(kFrameSinkParent1, kFrameSinkChild1);

  EXPECT_CALL(impl(), UnregisterFrameSinkHierarchy(kFrameSinkParent1,
                                                   kFrameSinkChild1));
  host().UnregisterFrameSinkHierarchy(kFrameSinkParent1, kFrameSinkChild1);
  FlushHostAndVerifyExpectations();

  // We should still have the CompositorFrameSink data for |kFrameSinkChild1|.
  EXPECT_TRUE(FrameSinkDataExists(kFrameSinkChild1));

  // Data for |kFrameSinkChild1| should be deleted when everything is destroyed.
  EXPECT_CALL(impl(), InvalidateFrameSinkId(kFrameSinkChild1));
  host().InvalidateFrameSinkId(kFrameSinkChild1);
  EXPECT_FALSE(FrameSinkDataExists(kFrameSinkChild1));

  // Data for |kFrameSinkParent1| should be deleted when everything is
  // destroyed.
  EXPECT_CALL(impl(), InvalidateFrameSinkId(kFrameSinkParent1));
  host().InvalidateFrameSinkId(kFrameSinkParent1);
  EXPECT_FALSE(FrameSinkDataExists(kFrameSinkParent1));
  FlushHostAndVerifyExpectations();
}

// Verify that multiple parents in the frame sink hierarchy works.
TEST_F(HostFrameSinkManagerTest, HierarchyMultipleParents) {
  // Register two parent and child CompositorFrameSink.
  const FrameSinkId& id_parent1 = kFrameSinkParent1;
  RegisterFrameSinkIdWithFakeClient(id_parent1,
                                    ReportFirstSurfaceActivation::kYes);
  RootCompositorFrameSinkData root_data1;
  host().CreateRootCompositorFrameSink(root_data1.BuildParams(id_parent1));

  const FrameSinkId& id_parent2 = kFrameSinkChild1;
  RegisterFrameSinkIdWithFakeClient(id_parent2,
                                    ReportFirstSurfaceActivation::kYes);
  RootCompositorFrameSinkData root_data2;
  host().CreateRootCompositorFrameSink(root_data2.BuildParams(id_parent2));

  const FrameSinkId& id_child = kFrameSinkParent2;
  RegisterFrameSinkIdWithFakeClient(id_child,
                                    ReportFirstSurfaceActivation::kYes);
  MockCompositorFrameSinkClient compositor_frame_sink_client;
  mojo::Remote<mojom::CompositorFrameSink> compositor_frame_sink;
  host().CreateCompositorFrameSink(
      id_child, compositor_frame_sink.BindNewPipeAndPassReceiver(),
      compositor_frame_sink_client.BindInterfaceRemote());

  // Register |id_parent1| in hierarchy first, this is the original window
  // embedding.
  EXPECT_CALL(impl(), RegisterFrameSinkHierarchy(id_parent1, id_child));
  host().RegisterFrameSinkHierarchy(id_parent1, id_child);

  // Register |id_parent2| in hierarchy second, this is a second embedding for
  // something like alt-tab on a different monitor.
  EXPECT_CALL(impl(), RegisterFrameSinkHierarchy(id_parent2, id_child));
  host().RegisterFrameSinkHierarchy(id_parent2, id_child);
  FlushHostAndVerifyExpectations();

  // Unregistering hierarchy with multiple parents should also work.
  EXPECT_CALL(impl(), UnregisterFrameSinkHierarchy(id_parent2, id_child));
  host().UnregisterFrameSinkHierarchy(id_parent2, id_child);

  EXPECT_CALL(impl(), UnregisterFrameSinkHierarchy(id_parent1, id_child));
  host().UnregisterFrameSinkHierarchy(id_parent1, id_child);
  FlushHostAndVerifyExpectations();
}

// Verify that HostFrameSinkManager can handle restarting after a GPU crash.
TEST_F(HostFrameSinkManagerTest, RestartOnGpuCrash) {
  // Register two FrameSinkIds, hierarchy between them and create a
  // CompositorFrameSink for one.
  EXPECT_CALL(impl(), RegisterFrameSinkId(kFrameSinkParent1,
                                          true /* report_activation */));
  RegisterFrameSinkIdWithFakeClient(kFrameSinkParent1,
                                    ReportFirstSurfaceActivation::kYes);
  EXPECT_CALL(impl(), RegisterFrameSinkId(kFrameSinkChild1,
                                          true /* report_activation */));
  RegisterFrameSinkIdWithFakeClient(kFrameSinkChild1,
                                    ReportFirstSurfaceActivation::kYes);
  EXPECT_CALL(impl(),
              RegisterFrameSinkHierarchy(kFrameSinkParent1, kFrameSinkChild1));
  host().RegisterFrameSinkHierarchy(kFrameSinkParent1, kFrameSinkChild1);

  RootCompositorFrameSinkData root_data;
  EXPECT_CALL(impl(), MockCreateRootCompositorFrameSink(kFrameSinkParent1));
  host().CreateRootCompositorFrameSink(
      root_data.BuildParams(kFrameSinkParent1));

  MockCompositorFrameSinkClient compositor_frame_sink_client;
  mojo::Remote<mojom::CompositorFrameSink> compositor_frame_sink;
  EXPECT_CALL(impl(), CreateCompositorFrameSink(kFrameSinkChild1, _, _, _));
  host().CreateCompositorFrameSink(
      kFrameSinkChild1, compositor_frame_sink.BindNewPipeAndPassReceiver(),
      compositor_frame_sink_client.BindInterfaceRemote());

  EXPECT_TRUE(IsBoundToFrameSinkManager());

  // Verify registration and CompositorFrameSink creation happened.
  FlushHostAndVerifyExpectations();

  // Kill the GPU. HostFrameSinkManager will have a connection error on the
  // message pipe and should clear state appropriately.
  KillGpu();
  {
    base::RunLoop run_loop;
    host().SetConnectionLostCallback(run_loop.QuitClosure());
    run_loop.Run();
  }

  EXPECT_FALSE(IsBoundToFrameSinkManager());

  // Verify that when HostFrameSinkManager is connected to the GPU again it
  // reregisters FrameSinkIds and FrameSink hierarchy.
  ConnectToGpu();
  EXPECT_CALL(impl(), RegisterFrameSinkId(kFrameSinkParent1,
                                          true /* report_activation */));
  EXPECT_CALL(impl(), RegisterFrameSinkId(kFrameSinkChild1,
                                          true /* report_activation */));
  EXPECT_CALL(impl(),
              RegisterFrameSinkHierarchy(kFrameSinkParent1, kFrameSinkChild1));
  FlushHostAndVerifyExpectations();

  EXPECT_TRUE(IsBoundToFrameSinkManager());
}

// Verify that HostFrameSinkManager does early return when trying to send
// hit-test data after HitTestQuery was deleted.
TEST_F(HostFrameSinkManagerTest, DeletedHitTestQuery) {
  // Register a FrameSinkId, and create a RootCompositorFrameSink, which should
  // create a HitTestQuery.
  RegisterFrameSinkIdWithFakeClient(kFrameSinkParent1,
                                    ReportFirstSurfaceActivation::kYes);
  RootCompositorFrameSinkData root_data;
  EXPECT_CALL(impl(), MockCreateRootCompositorFrameSink(kFrameSinkParent1));
  host().CreateRootCompositorFrameSink(
      root_data.BuildParams(kFrameSinkParent1));

  EXPECT_TRUE(DisplayHitTestQueryExists(kFrameSinkParent1));

  // Verify RootCompositorFrameSink was created on other end of message pipe.
  FlushHostAndVerifyExpectations();

  GetFrameSinkManagerClient()->OnAggregatedHitTestRegionListUpdated(
      kFrameSinkParent1, {});
  // Continue to send hit-test data to HitTestQuery associated with
  // kFrameSinkChild1.

  host().InvalidateFrameSinkId(kFrameSinkParent1);
  // Invalidating kFrameSinkChild1 would delete the corresponding HitTestQuery,
  // so further msgs to that HitTestQuery should be dropped.
  EXPECT_FALSE(DisplayHitTestQueryExists(kFrameSinkParent1));
  GetFrameSinkManagerClient()->OnAggregatedHitTestRegionListUpdated(
      kFrameSinkParent1, {});
}

// Verify that on lost context a RootCompositorFrameSink can be recreated.
TEST_F(HostFrameSinkManagerTest, ContextLossRecreateRoot) {
  // Register a FrameSinkId, and create a RootCompositorFrameSink.
  RegisterFrameSinkIdWithFakeClient(kFrameSinkParent1,
                                    ReportFirstSurfaceActivation::kYes);
  RootCompositorFrameSinkData root_data1;
  host().CreateRootCompositorFrameSink(
      root_data1.BuildParams(kFrameSinkParent1));

  // Verify RootCompositorFrameSink was created on other end of message pipe.
  EXPECT_CALL(impl(), MockCreateRootCompositorFrameSink(kFrameSinkParent1));
  FlushHostAndVerifyExpectations();

  // Create a new RootCompositorFrameSink and try to connect it with the same
  // FrameSinkId. This will happen if the browser GL context is lost.
  RootCompositorFrameSinkData root_data2;
  host().CreateRootCompositorFrameSink(
      root_data2.BuildParams(kFrameSinkParent1));

  // Verify RootCompositorFrameSink is destroyed and then recreated.
  EXPECT_CALL(impl(), MockDestroyCompositorFrameSink(kFrameSinkParent1));
  EXPECT_CALL(impl(), MockCreateRootCompositorFrameSink(kFrameSinkParent1));
  FlushHostAndVerifyExpectations();
}

// Verify that on lost context a CompositorFrameSink can be recreated.
TEST_F(HostFrameSinkManagerTest, ContextLossRecreateNonRoot) {
  // Register a FrameSinkId and create a CompositorFrameSink.
  RegisterFrameSinkIdWithFakeClient(kFrameSinkChild1,
                                    ReportFirstSurfaceActivation::kYes);
  MockCompositorFrameSinkClient compositor_frame_sink_client1;
  mojo::Remote<mojom::CompositorFrameSink> compositor_frame_sink1;
  host().CreateCompositorFrameSink(
      kFrameSinkChild1, compositor_frame_sink1.BindNewPipeAndPassReceiver(),
      compositor_frame_sink_client1.BindInterfaceRemote());

  // Verify CompositorFrameSink was created on other end of message pipe.
  EXPECT_CALL(impl(), CreateCompositorFrameSink(kFrameSinkChild1, _, _, _));
  FlushHostAndVerifyExpectations();

  // Create a new CompositorFrameSink and try to connect it with the same
  // FrameSinkId. This will happen if the client GL context is lost.
  MockCompositorFrameSinkClient compositor_frame_sink_client2;
  mojo::Remote<mojom::CompositorFrameSink> compositor_frame_sink2;
  host().CreateCompositorFrameSink(
      kFrameSinkChild1, compositor_frame_sink2.BindNewPipeAndPassReceiver(),
      compositor_frame_sink_client2.BindInterfaceRemote());

  // Verify CompositorFrameSink is destroyed and then recreated.
  EXPECT_CALL(impl(), MockDestroyCompositorFrameSink(kFrameSinkChild1));
  EXPECT_CALL(impl(), CreateCompositorFrameSink(kFrameSinkChild1, _, _, _));
  FlushHostAndVerifyExpectations();
}

TEST_F(HostFrameSinkManagerTest, ThrottleFramePainting) {
  const std::vector<FrameSinkId> frame_sink_ids{
      FrameSinkId(1, 1), FrameSinkId(2, 2), FrameSinkId(3, 3)};
  constexpr base::TimeDelta interval = base::Hertz(10);
  EXPECT_CALL(impl(), Throttle(frame_sink_ids, interval));
  host().Throttle(frame_sink_ids, interval);

  FlushHostAndVerifyExpectations();
}
}  // namespace viz
