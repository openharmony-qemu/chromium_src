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

#include "base/run_loop.h"
#include "base/test/scoped_feature_list.h"
#include "base/time/time.h"
#include "base/unguessable_token.h"
#include "build/build_config.h"
#include "mojo/public/cpp/bindings/associated_remote.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#include "components/viz/service/frame_sinks/frame_sink_manager_impl.h"
#undef private

#include "components/viz/service/frame_sinks/root_compositor_frame_sink_impl.h"
#include "components/viz/common/display/renderer_settings.h"
#include "components/viz/common/frame_sinks/begin_frame_args.h"
#include "components/viz/common/quads/compositor_frame.h"
#include "components/viz/common/surfaces/frame_sink_id.h"
#include "components/viz/common/surfaces/local_surface_id.h"
#include "components/viz/common/surfaces/surface_id.h"
#include "components/viz/service/display/display.h"
#include "components/viz/service/display_embedder/server_shared_bitmap_manager.h"
#include "components/viz/service/frame_sinks/compositor_frame_sink_support.h"
#include "components/viz/service/frame_sinks/eviction_handler.h"
#include "components/viz/service/input/mock_input_manager.h"
#include "components/viz/service/surfaces/surface_manager.h"
#include "components/viz/test/compositor_frame_helpers.h"
#include "components/viz/test/fake_external_begin_frame_source.h"
#include "components/viz/test/fake_surface_observer.h"
#include "components/viz/test/mock_compositor_frame_sink_client.h"
#include "components/viz/test/mock_display_client.h"
#include "components/viz/test/test_output_surface_provider.h"
#include "components/viz/test/test_shared_image_interface_provider.h"

namespace viz {

namespace {

constexpr FrameSinkId kFrameSinkId(1, 1);
const base::UnguessableToken kArbitraryToken =
    base::UnguessableToken::CreateForTesting(1, 2);

struct RootCompositorFrameSinkTestData {
  mojom::RootCompositorFrameSinkParamsPtr BuildParams(
      const FrameSinkId& frame_sink_id) {
    auto params = mojom::RootCompositorFrameSinkParams::New();
    params->frame_sink_id = frame_sink_id;
    params->widget = gpu::kNullSurfaceHandle;
    params->gpu_compositing = false;
    params->compositor_frame_sink =
        compositor_frame_sink.BindNewEndpointAndPassReceiver();
    params->compositor_frame_sink_client =
        compositor_frame_sink_client.BindInterfaceRemote();
    params->display_private = display_private.BindNewEndpointAndPassReceiver();
    params->display_client = display_client.BindRemote();
    params->renderer_settings = RendererSettings();
    return params;
  }

  mojo::AssociatedRemote<mojom::CompositorFrameSink> compositor_frame_sink;
  MockCompositorFrameSinkClient compositor_frame_sink_client;
  mojo::AssociatedRemote<mojom::DisplayPrivate> display_private;
  MockDisplayClient display_client;
};

}  // namespace

class RootCompositorFrameSinkImplTest : public testing::Test {
 public:
  RootCompositorFrameSinkImplTest()
      : manager_(FrameSinkManagerImpl::InitParams(&shared_bitmap_manager_,
                                                   &output_surface_provider_)),
        begin_frame_source_(0.f, false) {}

  void SetUp() override {
    manager_.SetSharedImageInterfaceProviderForTest(
        &shared_image_interface_provider_);
    surface_observer_ =
        std::make_unique<FakeSurfaceObserver>(manager_.surface_manager());
    manager_.RegisterFrameSinkId(kFrameSinkId, true);
    manager_.SetInputManagerForTesting(
        std::make_unique<MockInputManager>(&manager_));
  }

  void TearDown() override {
    root_impl_ = nullptr;
    surface_observer_ = nullptr;
    manager_.InvalidateFrameSinkId(kFrameSinkId);
  }

  void CreateRootCompositorFrameSink() {
    RootCompositorFrameSinkTestData test_data;
    auto params = test_data.BuildParams(kFrameSinkId);
    manager_.CreateRootCompositorFrameSink(std::move(params));
    root_impl_ = GetRootCompositorFrameSinkImpl();
  }

  RootCompositorFrameSinkImpl* GetRootCompositorFrameSinkImpl() {
    auto it = manager_.root_sink_map_.find(kFrameSinkId);
    return it == manager_.root_sink_map_.end() ? nullptr : it->second.get();
  }

  bool RootCompositorFrameSinkExists() {
    return manager_.root_sink_map_.find(kFrameSinkId) !=
           manager_.root_sink_map_.end();
  }

 protected:
  DebugRendererSettings debug_settings_;
  ServerSharedBitmapManager shared_bitmap_manager_;
  TestOutputSurfaceProvider output_surface_provider_;
  TestSharedImageInterfaceProvider shared_image_interface_provider_;
  FrameSinkManagerImpl manager_;
  std::unique_ptr<FakeSurfaceObserver> surface_observer_;
  FakeExternalBeginFrameSource begin_frame_source_;
  raw_ptr<RootCompositorFrameSinkImpl> root_impl_ = nullptr;
};

TEST_F(RootCompositorFrameSinkImplTest, CreateRootCompositorFrameSink) {
  CreateRootCompositorFrameSink();
  EXPECT_TRUE(RootCompositorFrameSinkExists());
  EXPECT_NE(root_impl_, nullptr);
}

TEST_F(RootCompositorFrameSinkImplTest, DestroyRootCompositorFrameSink) {
  CreateRootCompositorFrameSink();
  EXPECT_TRUE(RootCompositorFrameSinkExists());
}

TEST_F(RootCompositorFrameSinkImplTest, SetDisplayVisibleTrue) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetDisplayVisible(true);
}

TEST_F(RootCompositorFrameSinkImplTest, SetDisplayVisibleFalse) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetDisplayVisible(false);
}

TEST_F(RootCompositorFrameSinkImplTest, SetDisplayVisibleToggle) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetDisplayVisible(true);
  root_impl_->SetDisplayVisible(false);
  root_impl_->SetDisplayVisible(true);
  root_impl_->SetDisplayVisible(false);
}

TEST_F(RootCompositorFrameSinkImplTest, Resize) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->Resize(gfx::Size(100, 200));
}

TEST_F(RootCompositorFrameSinkImplTest, ResizeMultipleSizes) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->Resize(gfx::Size(100, 100));
  root_impl_->Resize(gfx::Size(200, 200));
  root_impl_->Resize(gfx::Size(50, 50));
}

TEST_F(RootCompositorFrameSinkImplTest, ResizeZeroSize) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->Resize(gfx::Size(0, 0));
}

TEST_F(RootCompositorFrameSinkImplTest, SetDisplayColorMatrix) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  gfx::Transform color_matrix;
  root_impl_->SetDisplayColorMatrix(color_matrix);
}

TEST_F(RootCompositorFrameSinkImplTest, SetDisplayColorMatrixScaled) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  gfx::Transform color_matrix;
  color_matrix.Scale(2.0, 2.0);
  root_impl_->SetDisplayColorMatrix(color_matrix);
}

TEST_F(RootCompositorFrameSinkImplTest, SetDisplayColorSpaces) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  gfx::DisplayColorSpaces display_color_spaces;
  root_impl_->SetDisplayColorSpaces(display_color_spaces);
}

TEST_F(RootCompositorFrameSinkImplTest, SetDisplayVSyncParameters) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  base::TimeTicks timebase = base::TimeTicks::Now();
  base::TimeDelta interval = base::Milliseconds(16);
  root_impl_->SetDisplayVSyncParameters(timebase, interval);
}

TEST_F(RootCompositorFrameSinkImplTest, SetDisplayVSyncParametersMultiple) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetDisplayVSyncParameters(base::TimeTicks::Now(),
                                         base::Milliseconds(16));
  root_impl_->SetDisplayVSyncParameters(base::TimeTicks::Now(),
                                         base::Milliseconds(33));
  root_impl_->SetDisplayVSyncParameters(base::TimeTicks::Now(),
                                         base::Milliseconds(16));
}

TEST_F(RootCompositorFrameSinkImplTest, SetWantsAnimateOnlyBeginFrames) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetWantsAnimateOnlyBeginFrames();
}

TEST_F(RootCompositorFrameSinkImplTest, SetWantsBeginFrameAcks) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetWantsBeginFrameAcks();
}

TEST_F(RootCompositorFrameSinkImplTest, SetAutoNeedsBeginFrame) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetAutoNeedsBeginFrame();
}

TEST_F(RootCompositorFrameSinkImplTest, DidNotProduceFrame) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  BeginFrameAck ack;
  ack.frame_id.source_id = 1;
  ack.frame_id.sequence_number = 1;
  root_impl_->DidNotProduceFrame(ack);
}

TEST_F(RootCompositorFrameSinkImplTest, DidNotProduceFrameMultiple) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  BeginFrameAck ack1;
  ack1.frame_id.source_id = 1;
  ack1.frame_id.sequence_number = 1;
  BeginFrameAck ack2;
  ack2.frame_id.source_id = 1;
  ack2.frame_id.sequence_number = 2;
  root_impl_->DidNotProduceFrame(ack1);
  root_impl_->DidNotProduceFrame(ack2);
}

TEST_F(RootCompositorFrameSinkImplTest, InitializeCompositorFrameSinkType) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->InitializeCompositorFrameSinkType(
      mojom::CompositorFrameSinkType::kUnspecified);
}

TEST_F(RootCompositorFrameSinkImplTest, WillEvictSurface) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  LocalSurfaceId local_surface_id(1, kArbitraryToken);
  SurfaceId surface_id(kFrameSinkId, local_surface_id);
  bool result = root_impl_->WillEvictSurface(surface_id);
  EXPECT_TRUE(result);
}

TEST_F(RootCompositorFrameSinkImplTest, AddVSyncParameterObserver) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  mojo::PendingRemote<mojom::VSyncParameterObserver> observer;
  root_impl_->AddVSyncParameterObserver(std::move(observer));
}

TEST_F(RootCompositorFrameSinkImplTest, SetHwSupportForMultipleRefreshRatesTrue) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetHwSupportForMultipleRefreshRates(true);
}

TEST_F(RootCompositorFrameSinkImplTest, SetHwSupportForMultipleRefreshRatesFalse) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetHwSupportForMultipleRefreshRates(false);
}

TEST_F(RootCompositorFrameSinkImplTest, StartOverdrawTracking) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->StartOverdrawTracking(5);
}

TEST_F(RootCompositorFrameSinkImplTest, StopOverdrawTracking) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->StartOverdrawTracking(5);
  auto result = root_impl_->StopOverdrawTracking();
}

TEST_F(RootCompositorFrameSinkImplTest, StartStopOverdrawTrackingMultiple) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->StartOverdrawTracking(5);
  root_impl_->StopOverdrawTracking();
  root_impl_->StartOverdrawTracking(10);
  root_impl_->StopOverdrawTracking();
}

TEST_F(RootCompositorFrameSinkImplTest, SetMaxVSyncAndVrrEmpty) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetMaxVSyncAndVrr(std::nullopt,
                                 display::VariableRefreshRateState::kVrrDisabled);
}

TEST_F(RootCompositorFrameSinkImplTest, SetMaxVSyncAndVrrWithValue) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetMaxVSyncAndVrr(base::Milliseconds(100),
                                 display::VariableRefreshRateState::kVrrEnabled);
}

TEST_F(RootCompositorFrameSinkImplTest, SetMaxVSyncAndVrrMultiple) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetMaxVSyncAndVrr(base::Milliseconds(50),
                                 display::VariableRefreshRateState::kVrrEnabled);
  root_impl_->SetMaxVSyncAndVrr(std::nullopt,
                                 display::VariableRefreshRateState::kVrrDisabled);
  root_impl_->SetMaxVSyncAndVrr(base::Milliseconds(100),
                                 display::VariableRefreshRateState::kVrrEnabled);
}

#if BUILDFLAG(IS_ANDROID) || BUILDFLAG(IS_CHROMEOS_ASH)
TEST_F(RootCompositorFrameSinkImplTest, SetSupportedRefreshRates) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  std::vector<float> rates = {60.0f, 30.0f};
  root_impl_->SetSupportedRefreshRates(rates);
}

TEST_F(RootCompositorFrameSinkImplTest, SetSupportedRefreshRatesEmpty) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  std::vector<float> rates;
  root_impl_->SetSupportedRefreshRates(rates);
}

TEST_F(RootCompositorFrameSinkImplTest, SetSupportedRefreshRatesMultiple) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  std::vector<float> rates1 = {60.0f, 30.0f};
  std::vector<float> rates2 = {60.0f, 30.0f, 15.0f};
  root_impl_->SetSupportedRefreshRates(rates1);
  root_impl_->SetSupportedRefreshRates(rates2);
}
#endif

#if BUILDFLAG(IS_ANDROID)
TEST_F(RootCompositorFrameSinkImplTest, SetVSyncPausedTrue) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetVSyncPaused(true);
}

TEST_F(RootCompositorFrameSinkImplTest, SetVSyncPausedFalse) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetVSyncPaused(false);
}

TEST_F(RootCompositorFrameSinkImplTest, UpdateRefreshRate) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->UpdateRefreshRate(60.0f);
}

TEST_F(RootCompositorFrameSinkImplTest, PreserveChildSurfaceControls) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->PreserveChildSurfaceControls();
}

TEST_F(RootCompositorFrameSinkImplTest, SetSwapCompletionCallbackEnabledTrue) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetSwapCompletionCallbackEnabled(true);
}

TEST_F(RootCompositorFrameSinkImplTest, SetSwapCompletionCallbackEnabledFalse) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetSwapCompletionCallbackEnabled(false);
}
#endif

TEST_F(RootCompositorFrameSinkImplTest, SetDelegatedInkPointRenderer) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  mojo::PendingReceiver<gfx::mojom::DelegatedInkPointRenderer> receiver;
  root_impl_->SetDelegatedInkPointRenderer(std::move(receiver));
}

TEST_F(RootCompositorFrameSinkImplTest, DisplayOutputSurfaceLost) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->DisplayOutputSurfaceLost();
}

TEST_F(RootCompositorFrameSinkImplTest, DisplayWillDrawAndSwapTrue) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  AggregatedRenderPassList render_passes;
  root_impl_->DisplayWillDrawAndSwap(true, &render_passes);
}

TEST_F(RootCompositorFrameSinkImplTest, DisplayWillDrawAndSwapFalse) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  AggregatedRenderPassList render_passes;
  root_impl_->DisplayWillDrawAndSwap(false, &render_passes);
}

TEST_F(RootCompositorFrameSinkImplTest, DisplayDidDrawAndSwap) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->DisplayDidDrawAndSwap();
}

TEST_F(RootCompositorFrameSinkImplTest, DisplayDidCompleteSwapWithSize) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  gfx::Size size(100, 100);
  root_impl_->DisplayDidCompleteSwapWithSize(size);
}

TEST_F(RootCompositorFrameSinkImplTest, SetWideColorEnabledTrue) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetWideColorEnabled(true);
}

TEST_F(RootCompositorFrameSinkImplTest, SetWideColorEnabledFalse) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetWideColorEnabled(false);
}

TEST_F(RootCompositorFrameSinkImplTest, SetPreferredFrameInterval) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetPreferredFrameInterval(base::Milliseconds(16));
}

TEST_F(RootCompositorFrameSinkImplTest, SetPreferredFrameIntervalZero) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetPreferredFrameInterval(base::Milliseconds(0));
}

TEST_F(RootCompositorFrameSinkImplTest, SetPreferredFrameIntervalMultiple) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetPreferredFrameInterval(base::Milliseconds(16));
  root_impl_->SetPreferredFrameInterval(base::Milliseconds(33));
  root_impl_->SetPreferredFrameInterval(base::Milliseconds(16));
}

TEST_F(RootCompositorFrameSinkImplTest, GetPreferredFrameIntervalForFrameSinkId) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  mojom::CompositorFrameSinkType type;
  base::TimeDelta interval =
      root_impl_->GetPreferredFrameIntervalForFrameSinkId(kFrameSinkId, &type);
}

TEST_F(RootCompositorFrameSinkImplTest, BindLayerContext) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  mojom::PendingLayerContextPtr context = mojom::PendingLayerContext::New();
  root_impl_->BindLayerContext(std::move(context));
}

TEST_F(RootCompositorFrameSinkImplTest, ExternalBeginFrameSourceAccessor) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  ExternalBeginFrameSource* source = root_impl_->external_begin_frame_source();
}

TEST_F(RootCompositorFrameSinkImplTest, AsExtReturnsNullByDefault) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  RootCompositorFrameSinkImplExt* ext = root_impl_->AsExt();
  EXPECT_NE(ext, nullptr);
}

TEST_F(RootCompositorFrameSinkImplTest, CurrentSurfaceId) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  const SurfaceId& surface_id = root_impl_->CurrentSurfaceId();
}

TEST_F(RootCompositorFrameSinkImplTest, CombinedOperations) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetDisplayVisible(true);
  root_impl_->Resize(gfx::Size(100, 100));
  root_impl_->SetDisplayVSyncParameters(base::TimeTicks::Now(),
                                         base::Milliseconds(16));
  root_impl_->SetNeedsBeginFrame(true);
  root_impl_->SetOutputIsSecure(true);
  root_impl_->ForceImmediateDrawAndSwapIfPossible();
  root_impl_->SetDisplayVisible(false);
}

#if BUILDFLAG(ARKWEB_EVICT_UNLOCK_FRAMES)
TEST_F(RootCompositorFrameSinkImplTest, DisplayDidRealSwapBuffer) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->DisplayDidRealSwapBuffer();
}

TEST_F(RootCompositorFrameSinkImplTest, SetDisplayVisibleWithEviction) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetDisplayVisible(false);
  root_impl_->SetDisplayVisible(true);
}
#endif

TEST_F(RootCompositorFrameSinkImplTest, MultipleWillEvictSurface) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  LocalSurfaceId local_surface_id1(1, kArbitraryToken);
  LocalSurfaceId local_surface_id2(2, kArbitraryToken);
  SurfaceId surface_id1(kFrameSinkId, local_surface_id1);
  SurfaceId surface_id2(kFrameSinkId, local_surface_id2);
  bool result1 = root_impl_->WillEvictSurface(surface_id1);
  bool result2 = root_impl_->WillEvictSurface(surface_id2);
  EXPECT_TRUE(result1);
  EXPECT_TRUE(result2);
}

TEST_F(RootCompositorFrameSinkImplTest, MultipleDisplayOutputSurfaceLost) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->DisplayOutputSurfaceLost();
}

TEST_F(RootCompositorFrameSinkImplTest, MultipleDisplayDidDrawAndSwap) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->DisplayDidDrawAndSwap();
  root_impl_->DisplayDidDrawAndSwap();
  root_impl_->DisplayDidDrawAndSwap();
}

TEST_F(RootCompositorFrameSinkImplTest, MultipleDisplayDidCompleteSwapWithSize) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->DisplayDidCompleteSwapWithSize(gfx::Size(100, 100));
  root_impl_->DisplayDidCompleteSwapWithSize(gfx::Size(200, 200));
  root_impl_->DisplayDidCompleteSwapWithSize(gfx::Size(100, 100));
}

TEST_F(RootCompositorFrameSinkImplTest, MultipleSetWideColorEnabled) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetWideColorEnabled(true);
  root_impl_->SetWideColorEnabled(false);
  root_impl_->SetWideColorEnabled(true);
  root_impl_->SetWideColorEnabled(false);
}

TEST_F(RootCompositorFrameSinkImplTest, MultipleAddVSyncParameterObserver) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  mojo::PendingRemote<mojom::VSyncParameterObserver> observer1;
  mojo::PendingRemote<mojom::VSyncParameterObserver> observer2;
  root_impl_->AddVSyncParameterObserver(std::move(observer1));
  root_impl_->AddVSyncParameterObserver(std::move(observer2));
}

TEST_F(RootCompositorFrameSinkImplTest, MultipleSetDelegatedInkPointRenderer) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  mojo::PendingReceiver<gfx::mojom::DelegatedInkPointRenderer> receiver1;
  mojo::PendingReceiver<gfx::mojom::DelegatedInkPointRenderer> receiver2;
  root_impl_->SetDelegatedInkPointRenderer(std::move(receiver1));
  root_impl_->SetDelegatedInkPointRenderer(std::move(receiver2));
}

TEST_F(RootCompositorFrameSinkImplTest, MultipleBindLayerContext) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  mojom::PendingLayerContextPtr context1 = mojom::PendingLayerContext::New();
  mojom::PendingLayerContextPtr context2 = mojom::PendingLayerContext::New();
  root_impl_->BindLayerContext(std::move(context1));
  root_impl_->BindLayerContext(std::move(context2));
}

TEST_F(RootCompositorFrameSinkImplTest, MultipleGetPreferredFrameIntervalForFrameSinkId) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  mojom::CompositorFrameSinkType type;
  root_impl_->GetPreferredFrameIntervalForFrameSinkId(kFrameSinkId, &type);
  root_impl_->GetPreferredFrameIntervalForFrameSinkId(kFrameSinkId, &type);
}

TEST_F(RootCompositorFrameSinkImplTest, MultipleHwSupportChanges) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->SetHwSupportForMultipleRefreshRates(true);
  root_impl_->SetHwSupportForMultipleRefreshRates(false);
  root_impl_->SetHwSupportForMultipleRefreshRates(true);
}

TEST_F(RootCompositorFrameSinkImplTest, DisplayWillDrawAndSwapWithRenderPasses) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  AggregatedRenderPassList render_passes;
  auto render_pass = std::make_unique<AggregatedRenderPass>();
  render_pass->SetNew(AggregatedRenderPassId(1), gfx::Rect(100, 100),
                      gfx::Rect(100, 100), gfx::Transform());
  render_passes.push_back(std::move(render_pass));
  root_impl_->DisplayWillDrawAndSwap(true, &render_passes);
}

TEST_F(RootCompositorFrameSinkImplTest, MultipleInitializeCompositorFrameSinkType) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  root_impl_->InitializeCompositorFrameSinkType(
      mojom::CompositorFrameSinkType::kUnspecified);
  root_impl_->InitializeCompositorFrameSinkType(
      mojom::CompositorFrameSinkType::kLayerTree);
}

TEST_F(RootCompositorFrameSinkImplTest, MultipleSetDisplayColorMatrix) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  gfx::Transform transform1;
  transform1.Scale(2.0, 2.0);
  gfx::Transform transform2;
  transform2.Rotate(45.0);
  root_impl_->SetDisplayColorMatrix(transform1);
  root_impl_->SetDisplayColorMatrix(transform2);
  root_impl_->SetDisplayColorMatrix(gfx::Transform());
}

TEST_F(RootCompositorFrameSinkImplTest, MultipleSetDisplayColorSpaces) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  gfx::DisplayColorSpaces color_spaces1;
  gfx::DisplayColorSpaces color_spaces2;
  root_impl_->SetDisplayColorSpaces(color_spaces1);
  root_impl_->SetDisplayColorSpaces(color_spaces2);
}

TEST_F(RootCompositorFrameSinkImplTest, StressTest) {
  CreateRootCompositorFrameSink();
  ASSERT_TRUE(root_impl_);
  for (int i = 0; i < 100; ++i) {
    root_impl_->SetDisplayVisible(i % 2 == 0);
    root_impl_->Resize(gfx::Size(i * 10, i * 10));
    root_impl_->SetNeedsBeginFrame(i % 2 == 0);
    root_impl_->SetDisplayVSyncParameters(base::TimeTicks::Now(),
                                           base::Milliseconds(16));
  }
}

}  // namespace viz