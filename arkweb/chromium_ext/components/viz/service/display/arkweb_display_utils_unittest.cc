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

#include "arkweb/build/features/features.h"
#include "components/viz/service/display/display_client.h"
#include "components/viz/service/display/display_damage_tracker.h"
#include "components/viz/service/display/output_surface_frame.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#define protected public
#include "components/viz/service/display/display.h"
#include "arkweb/chromium_ext/components/viz/service/display/arkweb_display_utils.h"
#include "arkweb/chromium_ext/components/viz/service/display/arkweb_display_utils.cc"
#undef private
#undef protected

namespace viz {
class DisplaySchedulerMock : public viz::DisplaySchedulerBase {
public:
  DisplaySchedulerMock() = default;
  ~DisplaySchedulerMock() = default;
  void OnDisplayDamaged(SurfaceId surface_id) {}
  void OnRootFrameMissing(bool missing) {}
  void OnPendingSurfacesChanged() {}
  void SetVisible(bool visible) {}
  void ForceImmediateSwapIfPossible() {}
  void SetNeedsOneBeginFrame(bool needs_draw) {}
  void DidSwapBuffers() {}
  void DidReceiveSwapBuffersAck() {}
  void OutputSurfaceLost() {}
  void ReportFrameTime(
      base::TimeDelta frame_time,
      base::flat_set<base::PlatformThreadId> animation_thread_ids,
      base::flat_set<base::PlatformThreadId> renderer_main_thread_ids,
      base::TimeTicks draw_start,
      HintSession::BoostType boost_type) {}
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void SetShouldFrameSubmissionBeforeDraw(bool should)  {}
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void ReenableSwapCheck(const SurfaceId& surface_id, int width, int height) {}
#endif // ARKWEB_MAXIMIZE_RESIZE
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void SetClientId(const uint32_t client_id) {}
#endif
};

class OutputSurfaceMock : public viz::OutputSurface {
public:
  OutputSurfaceMock() = default;
  ~OutputSurfaceMock() = default;
  void BindToClient(OutputSurfaceClient* client) {}
  void EnsureBackbuffer() {}
  void DiscardBackbuffer() {}
  void Reshape(const ReshapeParams& params) {}
  void SwapBuffers(OutputSurfaceFrame frame) {}
  void SetUpdateVSyncParametersCallback(
      UpdateVSyncParametersCallback callback) {}
  void SetDisplayTransformHint(gfx::OverlayTransform transform){}
  void SetBypassVsyncCondition(int32_t condition) {}
  void SetNativeInnerWeb(bool isInnerWeb) {}
  gfx::OverlayTransform GetDisplayTransform() {}
};

class DisplayClientMock : public viz::DisplayClient {
 public:
  DisplayClientMock() = default;
  ~DisplayClientMock() = default;
  void DisplayOutputSurfaceLost() {}
  void DisplayWillDrawAndSwap(
      bool will_draw_and_swap,
      AggregatedRenderPassList* render_passes) {}
  void DisplayDidDrawAndSwap() {}
  void DisplayDidReceiveCALayerParams(
      const gfx::CALayerParams& ca_layer_params) {}
  void DisplayDidCompleteSwapWithSize(const gfx::Size& pixel_size) {}
  void DisplayAddChildWindowToBrowser(
      gpu::SurfaceHandle child_window) {}
  void SetWideColorEnabled(bool enabled) {}
  void SetPreferredFrameInterval(base::TimeDelta interval) {}
  base::TimeDelta GetPreferredFrameIntervalForFrameSinkId(
      const FrameSinkId& id,
      mojom::CompositorFrameSinkType* type) {}
#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void RestoreRenderFit(const FrameSinkId& frame_sink_id) {}
#endif // ARKWEB_MAXIMIZE_RESIZE
};

class DumpFrameObserverTest : public testing::Test {
 public:
  DumpFrameObserverTest() = default;
  ~DumpFrameObserverTest() = default;

  void TestPropertiesUpdateAndShouldDump() {
    DumpFrameObserver observer;
    std::string test_string = "true";
    observer.PropertiesUpdate(test_string.c_str());
    EXPECT_TRUE(observer.ShouldDump());

    test_string = "false";
    observer.PropertiesUpdate(test_string.c_str());
    EXPECT_FALSE(observer.ShouldDump());

    observer.dump_param_list_.push_back("true");
    observer.PropertiesUpdate("true");
    EXPECT_TRUE(observer.ShouldDump());

    observer.PropertiesUpdate("a#b#c");
    EXPECT_EQ(observer.dump_param_list_.size(), 3);
    EXPECT_EQ(observer.dump_param_list_[0], "a");
    EXPECT_EQ(observer.dump_param_list_[1], "b");
    EXPECT_EQ(observer.dump_param_list_[2], "c");
  }

  void TestShouldDumpInFreqAndDumpPath(){
    DumpFrameObserver observer;
    observer.dump_param_list_.clear();
    EXPECT_EQ(observer.DumpPath(), "");
    observer.dump_param_list_.push_back("true");
    EXPECT_FALSE(observer.ShouldDumpInFreq());

    observer.dump_param_list_.push_back("1");
    EXPECT_TRUE(observer.ShouldDumpInFreq());

    observer.dump_param_list_.pop_back();
    observer.dump_param_list_.push_back("2");
    EXPECT_FALSE(observer.ShouldDumpInFreq());

    observer.dump_param_list_.push_back("3");
    EXPECT_EQ(observer.DumpPath(), "3");
  }
};

TEST_F(DumpFrameObserverTest, PropertiesUpdateAndShouldDump) {
  TestPropertiesUpdateAndShouldDump();
}

TEST_F(DumpFrameObserverTest, ShouldDumpInFreqAndDumpPath){
  TestShouldDumpInFreqAndDumpPath();
}
class ArkwebDisplayUtilsTest : public testing::Test {
 public:
  ArkwebDisplayUtilsTest() = default;
  ~ArkwebDisplayUtilsTest() = default;
 protected:
  // Helper function to create a Display object for testing
  std::unique_ptr<Display> CreateTestDisplay() {
    RendererSettings settings;
    FrameSinkId frame_sink_id;
    auto output_surface = std::make_unique<OutputSurfaceMock>();
    auto scheduler = std::make_unique<DisplaySchedulerMock>();
    return std::make_unique<Display>(
        nullptr, nullptr, nullptr, nullptr, settings,
        nullptr, frame_sink_id, nullptr, 
        std::move(output_surface), nullptr, std::move(scheduler), nullptr);
  }

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  void TestSetDrawRect() {
    ArkwebDisplayUtils utils(nullptr);
    const gfx::Rect test_rect(10, 10);
    utils.SetDrawRect(test_rect);
    EXPECT_NE(utils.draw_rect_, test_rect);

    auto display = CreateTestDisplay();
    ArkwebDisplayUtils utils2(display.get());
    utils2.SetDrawMode(1);
    utils2.SetDrawRect(test_rect);
    EXPECT_EQ(utils2.draw_rect_, test_rect);
  }

  void TestSetDrawMode() {
    ArkwebDisplayUtils utils(nullptr);
    const int32_t mode = 1;
    utils.SetDrawMode(mode);
    EXPECT_EQ(utils.draw_mode_, mode);
  }
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void TestDisableSwapUntilMaximized() {
    ArkwebDisplayUtils utils(nullptr);
    utils.reset_init_timer_ = nullptr;
    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::DISABLE_SWAP;
    utils.DisableSwapUntilMaximized();
    EXPECT_EQ(utils.temp_idle_state_, ArkwebDisplayUtils::TempIdleState::INIT);

    utils.reset_init_timer_ = std::make_unique<base::RetainingOneShotTimer>();
    ASSERT_NE(utils.reset_init_timer_, nullptr);
    utils.DisableSwapUntilMaximized();
  }

  void TestRestoreRenderFitTimeElapsed() {
    auto display = CreateTestDisplay();
    ArkwebDisplayUtils utils(display.get());

    utils.RestoreRenderFitTimeElapsed();
    EXPECT_EQ(utils.temp_idle_state_, ArkwebDisplayUtils::TempIdleState::RESTORE_RENDERFIT);

    auto client = std::make_unique<DisplayClientMock>();
    utils.display_->client_  = client.get();
    utils.RestoreRenderFitTimeElapsed();
    EXPECT_EQ(utils.temp_idle_state_, ArkwebDisplayUtils::TempIdleState::RESTORE_RENDERFIT);
  }

  void TestReenableSwapCheck() {
    auto display = CreateTestDisplay();
    display->current_surface_id_ = SurfaceId(display->frame_sink_id_, LocalSurfaceId());
    gfx::Size size(10,10);
    display->current_surface_size_ = size;
    ArkwebDisplayUtils utils(display.get());

    FrameSinkId frame_sink_id_1;
    LocalSurfaceId local_surface_id_1;
    SurfaceId surface_id_1(frame_sink_id_1, local_surface_id_1);
    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::INIT;
    utils.ReenableSwapCheck(surface_id_1, 0, 0);

    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::DISABLE_SWAP;
    utils.ReenableSwapCheck(surface_id_1, 0, 0);

    FrameSinkId frame_sink_id_2(1,1);
    base::UnguessableToken embed_token1;
    LocalSurfaceId local_surface_id_2(1,embed_token1);
    SurfaceId surface_id_2(frame_sink_id_2, local_surface_id_2);
    utils.ReenableSwapCheck(surface_id_2, 0, 0);

    FrameSinkId frame_sink_id_3(1,1);
    base::UnguessableToken embed_token2 = base::UnguessableToken::Create();
    LocalSurfaceId local_surface_id_3(1,embed_token2);
    SurfaceId surface_id_3(frame_sink_id_3, local_surface_id_3);
    utils.ReenableSwapCheck(surface_id_3, 0, 0);

    utils.ReenableSwapCheck(surface_id_2, 11, 11);
    EXPECT_EQ(utils.temp_idle_state_, ArkwebDisplayUtils::TempIdleState::REENABLE_SWAP);
  }

  void TestRestoreRenderFit() {
    auto display = CreateTestDisplay();
    ArkwebDisplayUtils utils(display.get());
    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::INIT;
    utils.RestoreRenderFit();
    EXPECT_NE(utils.temp_idle_state_, ArkwebDisplayUtils::TempIdleState::RESTORE_RENDERFIT);

    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::REENABLE_SWAP;
    utils.RestoreRenderFit();
    EXPECT_NE(utils.temp_idle_state_, ArkwebDisplayUtils::TempIdleState::RESTORE_RENDERFIT);

    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::INIT;
    auto client = std::make_unique<DisplayClientMock>();
    utils.display_->client_  = client.get();
    utils.RestoreRenderFit();
    EXPECT_NE(utils.temp_idle_state_, ArkwebDisplayUtils::TempIdleState::RESTORE_RENDERFIT);

    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::REENABLE_SWAP;
    utils.RestoreRenderFit();
    EXPECT_EQ(utils.temp_idle_state_, ArkwebDisplayUtils::TempIdleState::RESTORE_RENDERFIT);
  }

  void TestShouldDisableSwap() {
    ArkwebDisplayUtils utils(nullptr);
    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::INIT;
    EXPECT_TRUE(utils.ShouldDisableSwap(true));
    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::DISABLE_SWAP;
    EXPECT_FALSE(utils.ShouldDisableSwap(true));
    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::INIT;
    EXPECT_FALSE(utils.ShouldDisableSwap(false));
    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::DISABLE_SWAP;
    EXPECT_FALSE(utils.ShouldDisableSwap(false));
  }
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void TestSetClientId() {
    ArkwebDisplayUtils utils(nullptr);
    const uint32_t client_id = 1;
    utils.SetClientId(client_id);
    EXPECT_EQ(utils.client_id_, client_id);
  }

  void TestSetGpuChannelManager() {
    ArkwebDisplayUtils utils(nullptr);
    GpuServiceImpl* test_impl = nullptr;
    utils.SetGpuServiceImpl(test_impl);
    EXPECT_EQ(utils.gpu_service_impl_, test_impl);
  }
#endif

  void TestResize() {
    gfx::Size size1(10, 10);
    gfx::Size size2(9000, 9000);
    auto display = CreateTestDisplay();
    display->current_surface_size_.set_height(7000);
    display->current_surface_size_.set_width(0);
    ArkwebDisplayUtils utils(display.get());

    #if BUILDFLAG(ARKWEB_SYNC_RENDER)
    utils.Resize(size1);
    utils.Resize(size2);
    gfx::Size test(8000, 8000);
    EXPECT_EQ(display->current_surface_size_, test);

    utils.SetDrawMode(1);
    utils.Resize(size2);
    EXPECT_EQ(display->current_surface_size_.width(), test.width());
    #endif

    #if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::RESTORE_RENDERFIT;
    utils.Resize(size2);
    EXPECT_NE(utils.temp_idle_state_, ArkwebDisplayUtils::TempIdleState::DISABLE_SWAP);

    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::INIT;
    utils.Resize(size2);

    utils.Resize(size2);
    EXPECT_EQ(utils.temp_idle_state_, ArkwebDisplayUtils::TempIdleState::DISABLE_SWAP);
    #endif
  }

  void TestArkwebDisplayUtils() {
    auto display = CreateTestDisplay();

    #if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
    ArkwebDisplayUtils utils1(display.get());
    EXPECT_NE(utils1.reset_init_timer_, nullptr);
    EXPECT_NE(utils1.reenable_swap_timer_, nullptr);
    #endif

    ArkwebDisplayUtils utils2(display.get());
    EXPECT_NE(utils2.display_, nullptr);

    #if BUILDFLAG(ARKWEB_DFX_DUMP)
    EXPECT_NE(utils2.dump_frame_observer_, nullptr);
    #endif

    #if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
    EXPECT_NE(utils2.reset_init_timer_, nullptr);
    EXPECT_NE(utils2.reenable_swap_timer_, nullptr);
    #endif
  }

  void TestDrawAndSwap() {
    viz::AggregatedRenderPass last_render_pass;
    gfx::Size current_surface_size = gfx::Size(10, 10);
    AggregatedFrame frame;

    auto display = CreateTestDisplay();
    ArkwebDisplayUtils utils(display.get());
    utils.SetDrawMode(0);
    last_render_pass.output_rect = gfx::Rect(20, 20);
    last_render_pass.damage_rect = gfx::Rect(20, 20);
    utils.DrawAndSwap(last_render_pass, current_surface_size, frame);
    EXPECT_EQ(frame.surface_damage_rect_list_.size(), 1);

    utils.SetDrawMode(1);
    last_render_pass.output_rect = gfx::Rect(10, 20);
    last_render_pass.damage_rect = gfx::Rect(20, 20);
    utils.DrawAndSwap(last_render_pass, current_surface_size, frame);
    EXPECT_EQ(frame.surface_damage_rect_list_.size(), 2);

    utils.SetDrawMode(0);
    last_render_pass.output_rect = gfx::Rect(21, 22);
    last_render_pass.damage_rect = gfx::Rect(10, 20);
    utils.DrawAndSwap(last_render_pass, current_surface_size, frame);
    EXPECT_EQ(frame.surface_damage_rect_list_.size(), 2);

    utils.SetDrawMode(1);
    last_render_pass.output_rect = gfx::Rect(20, 20);
    last_render_pass.damage_rect = gfx::Rect(20, 20);
    utils.DrawAndSwap(last_render_pass, current_surface_size, frame);
    EXPECT_EQ(frame.surface_damage_rect_list_.size(), 3);
  }

#if BUILDFLAG(ARKWEB_DFX_DUMP)
  void TestDrawAndSwapDump() {
    auto display = CreateTestDisplay();
    ArkwebDisplayUtils utils(display.get());
    utils.dump_frame_observer_.reset();
    AggregatedFrame frame;
    utils.DrawAndSwapDump(frame);

    utils.dump_frame_observer_ = std::make_unique<DumpFrameObserver>();
    ASSERT_NE(utils.dump_frame_observer_, nullptr);
    utils.dump_frame_observer_->PropertiesUpdate("true");
    EXPECT_EQ(utils.dump_frame_observer_->should_dump_, true);
    utils.DrawAndSwapDump(frame);

    utils.dump_frame_observer_->dump_param_list_.push_back("true");
    utils.dump_frame_observer_->dump_param_list_.push_back("1");
    EXPECT_TRUE(utils.dump_frame_observer_->ShouldDumpInFreq());
    utils.DrawAndSwapDump(frame);

    auto pass = std::make_unique<viz::AggregatedRenderPass>();
    frame.render_pass_list.emplace_back(std::move(pass));
    utils.DrawAndSwapDump(frame);
    EXPECT_EQ(frame.render_pass_list.size(), 1);
  }
#endif
};

TEST_F(ArkwebDisplayUtilsTest, ArkwebDisplayUtilsTest_001){
  TestDrawAndSwapDump();
  TestDrawAndSwap();
}

TEST_F(ArkwebDisplayUtilsTest, ArkwebDisplayUtilsTest_002){
  TestArkwebDisplayUtils();
}

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
TEST_F(ArkwebDisplayUtilsTest, ArkwebDisplayUtilsTest_003) {
  TestSetDrawRect();
  TestSetDrawMode();
}
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
TEST_F(ArkwebDisplayUtilsTest, ArkwebDisplayUtilsTest_004) {
  TestDisableSwapUntilMaximized();
  TestRestoreRenderFitTimeElapsed();
}

TEST_F(ArkwebDisplayUtilsTest, ArkwebDisplayUtilsTest_005) {
  TestRestoreRenderFit();
  TestReenableSwapCheck();
}

TEST_F(ArkwebDisplayUtilsTest, ArkwebDisplayUtilsTest_006) {
  TestShouldDisableSwap();
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
TEST_F(ArkwebDisplayUtilsTest, ArkwebDisplayUtilsTest_008) {
  TestSetClientId();
  TestSetGpuChannelManager();
}
#endif
TEST_F(ArkwebDisplayUtilsTest, ArkwebDisplayUtilsTest_009) {
  TestResize();
}
}  // namespace viz
