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

#include "arkweb/chromium_ext/components/viz/service/frame_sinks/root_compositor_frame_sink_impl_ext.h"
#include "components/viz/service/display_embedder/server_shared_bitmap_manager.h"
#include "components/viz/test/test_output_surface_provider.h"
#include "services/viz/privileged/mojom/compositing/frame_sink_manager.mojom.h"
#include "components/viz/test/mock_display_client.h"
#include "components/viz/test/mock_compositor_frame_sink_client.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#include "components/viz/service/frame_sinks/frame_sink_manager_impl.h"
#undef private


namespace {
  constexpr viz::FrameSinkId frameSinkId(1, 1);
  bool dealed = false;

  void DealFunc () {
    dealed = true;
  }
}

namespace viz {

class RootCompositorFrameSinkImplExtTest : public testing::Test {
 public:
  RootCompositorFrameSinkImplExtTest() = default;
  ~RootCompositorFrameSinkImplExtTest() = default;

};

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

TEST_F(RootCompositorFrameSinkImplExtTest, RootCompositorFrameSinkImplExtTest01) {
  ServerSharedBitmapManager shared_bitmap_manager_;
  TestOutputSurfaceProvider output_surface_provider_;
  FrameSinkManagerImpl managerImpl(FrameSinkManagerImpl::
    InitParams(&shared_bitmap_manager_, &output_surface_provider_));

  RootCompositorFrameSinkData root_data;
  managerImpl.CreateRootCompositorFrameSink(root_data.BuildParams(frameSinkId));
  for (auto &item : managerImpl.root_sink_map_) {
    EXPECT_NE(item.second, nullptr);
    auto sinkImpl = std::move(item.second);
    RootCompositorFrameSinkImplExt* sinkImplExt = static_cast<RootCompositorFrameSinkImplExt*>(sinkImpl.get());
    ASSERT_NO_FATAL_FAILURE(sinkImplExt->SetDrawMode(1));

    ASSERT_NO_FATAL_FAILURE(sinkImplExt->SetShouldFrameSubmissionBeforeDraw(false, base::DoNothing()));
    sinkImplExt->SetShouldFrameSubmissionBeforeDraw(true, base::BindOnce(&DealFunc));
    EXPECT_EQ(dealed, true);

    ASSERT_NO_FATAL_FAILURE(sinkImplExt->EvictFrameBackBuffers());
    ASSERT_NO_FATAL_FAILURE(sinkImplExt->SetIfNeedCleanBuffers(false));
    ASSERT_NO_FATAL_FAILURE(sinkImplExt->SetIsOfflineWebComponentInactive(false));
    ASSERT_NO_FATAL_FAILURE(sinkImplExt->SetIsOfflineWebComponentInactive(true));
    ASSERT_NO_FATAL_FAILURE(sinkImplExt->DisableSwapUntilMaximized());
  }
}

}