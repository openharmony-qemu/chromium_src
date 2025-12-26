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
#define private public
#include "arkweb/chromium_ext/components/viz/service/frame_sinks/compositor_frame_sink_support_utils.h"
#undef private

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "components/viz/test/mock_compositor_frame_sink_client.h"
#include "components/viz/service/frame_sinks/frame_sink_manager_impl.h"
#include "components/viz/common/frame_sinks/begin_frame_source.h"
#include "ash/frame_sink/test/test_begin_frame_source.h"

namespace viz {
constexpr bool kIsRoot = false;
constexpr FrameSinkId kArbitraryFrameSinkId(1, 1);

class CompositorFrameSinkSupportUtilsTest : public testing::Test {
public:
    CompositorFrameSinkSupportUtilsTest() = default;
    ~CompositorFrameSinkSupportUtilsTest() = default;

  void SetUp() override {
    std::unique_ptr<FrameSinkManagerImpl> manager = std::make_unique<FrameSinkManagerImpl>(
        FrameSinkManagerImpl::InitParams(&shared_bitmap_manager_));
    surface_observer_ =
        std::make_unique<FakeSurfaceObserver>(manager->surface_manager());
    manager->SetLocalClient(&frame_sink_manager_client_);
    std::unique_ptr<base::SimpleTestTickClock> now_src = std::make_unique<base::SimpleTestTickClock>();
    manager->surface_manager()->SetTickClockForTesting(now_src.get());
    manager->RegisterFrameSinkId(kArbitraryFrameSinkId,
                                  true /* report_activation */);
    manager->SetSharedImageInterfaceProviderForTest(
        &shared_image_interface_provider_);
    support_ = std::make_unique<CompositorFrameSinkSupport>(
        &fake_support_client_, manager.get(), kArbitraryFrameSinkId, kIsRoot);
    support_->SetBeginFrameSource(&begin_frame_source_);
  }

  void TearDown() override {
    manager->InvalidateFrameSinkId(kArbitraryFrameSinkId);
  }

 protected:
  TestSharedImageInterfaceProvider shared_image_interface_provider_;
  ServerSharedBitmapManager shared_bitmap_manager_;
  testing::NiceMock<MockFrameSinkManagerClient> frame_sink_manager_client_;
  FakeCompositorFrameSinkClient fake_support_client_;
  FakeExternalBeginFrameSource begin_frame_source_;
  std::unique_ptr<CompositorFrameSinkSupport> support_;
  std::unique_ptr<FakeSurfaceObserver> surface_observer_;
};

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
// Observer that records ReenableSwapCheck invocations for tests.
class TestReenableObserver : public SurfaceObserver {
 public:
  TestReenableObserver() = default;
  void ReenableSwapCheck(const SurfaceId& surface_id, int width, int height) override {
    called = true;
    last_surface_id = surface_id;
    last_width = width;
    last_height = height;
  }
  bool called = false;
  SurfaceId last_surface_id;
  int last_width = -1;
  int last_height = -1;
};
#endif // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_THROTTLE_FRAME)
TEST_F(CompositorFrameSinkSupportUtilsTest, UpdateThrottleMode) {
  support_->is_root_ = true;
  CompositorFrameSinkSupportUtils utils(support_.get());
  ASSERT_NO_FATAL_FAILURE(utils.UpdateThrottleMode(true));
}

TEST_F(CompositorFrameSinkSupportUtilsTest, UpdateThrottleModeNotEnable) {
  CompositorFrameSinkSupportUtils utils(support_.get());
  ASSERT_NO_FATAL_FAILURE(utils.UpdateThrottleMode(true));
}

TEST_F(CompositorFrameSinkSupportUtilsTest, UpdateThrottleModeEnable) {
  CompositorFrameSinkSupportUtils utils(support_.get());
  ASSERT_NO_FATAL_FAILURE(utils.UpdateThrottleMode(false));
  auto result = base::TimeDelta::FromIntervalValue(0);
  ASSERT_EQ(utils.compositorFrameSinkSupport->preferred_frame_interval_, result);
}
#endif // ARKWEB_THROTTLE_FRAME

#if BUILDFLAG(ARKWEB_VIDEO_LTPO)
TEST_F(CompositorFrameSinkSupportUtilsTest, VideoLtpo_PushPopKeepsMaxCount) {
  CompositorFrameSinkSupportUtils utils(support_.get());

  // Fill the internal queue up to kMaxFrameCount using artificial timestamps.
  for (int i = 0; i < utils.kMaxFrameCount; ++i) {
    utils.frames_time_stamps_.push(static_cast<int64_t>(i));
  }

  // Sanity check: queue is full
  ASSERT_EQ(utils.frames_time_stamps_.size(), static_cast<size_t>(utils.kMaxFrameCount));

  // Record the front value; after calling videoLtpoMaybeSubmitCompositorFrame
  // the queue should push one and pop one, keeping size equal to kMaxFrameCount
  int64_t prev_front = utils.frames_time_stamps_.front();

  // Call the method under test. It will push current timestamp and pop oldest.
  ASSERT_NO_FATAL_FAILURE(utils.videoLtpoMaybeSubmitCompositorFrame());

  // Size must remain at most kMaxFrameCount
  EXPECT_LE(utils.frames_time_stamps_.size(), static_cast<size_t>(utils.kMaxFrameCount));

  // Front should have advanced (popped)
  EXPECT_NE(utils.frames_time_stamps_.front(), prev_front);

  // Back should be >= front
  EXPECT_GE(utils.frames_time_stamps_.back(), utils.frames_time_stamps_.front());

  // GetFrameRate should be non-negative
  int fr = utils.GetFrameRate();
  EXPECT_GE(fr, 0);
}

TEST_F(CompositorFrameSinkSupportUtilsTest, GetCurrentTimeStampMS_Increases) {
  CompositorFrameSinkSupportUtils utils(support_.get());
  int64_t t1 = utils.GetCurrentTimeStampMS();
  int64_t t2 = utils.GetCurrentTimeStampMS();
  // t2 should be greater or equal to t1 (system clock monotonicity not strictly guaranteed,
  // but milliseconds resolution makes this expectation reasonable)
  EXPECT_GE(t2, t1);
}
#endif // ARKWEB_VIDEO_LTPO

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
TEST_F(CompositorFrameSinkSupportUtilsTest, MaximizeResize_ReenableSwapCheckCalled) {
  TestReenableObserver observer;

  ASSERT_NE(support_->surface_manager_, nullptr);
  support_->surface_manager_->AddObserver(&observer);

  FrameSinkId fsid = kArbitraryFrameSinkId;
  base::UnguessableToken token = base::UnguessableToken::Create();
  LocalSurfaceId local_id(1, token);
  SurfaceId surface_id(fsid, local_id);
  SurfaceInfo info(surface_id, 1.0f, gfx::Size(10, 10));
  Surface* surface = support_->surface_manager_->CreateSurface(base::WeakPtr<SurfaceClient>(), info, SurfaceId());
  ASSERT_NE(surface, nullptr);

  CompositorFrameSinkSupportUtils utils(support_.get());
  gfx::Size size(123, 456);
  utils.maximizeResizeCompositorFrame(surface, size);

  EXPECT_TRUE(observer.called);
  EXPECT_EQ(observer.last_width, size.width());
  EXPECT_EQ(observer.last_height, size.height());

  support_->surface_manager_->RemoveObserver(&observer);
}
#endif // ARKWEB_MAXIMIZE_RESIZE
} // viz