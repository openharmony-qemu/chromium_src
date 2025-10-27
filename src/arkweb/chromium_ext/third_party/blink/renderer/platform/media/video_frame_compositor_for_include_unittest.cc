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

#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "base/test/task_environment.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/platform/web_video_frame_submitter.h"

#define private public
#include "third_party/blink/public/platform/media/video_frame_compositor.h"
#undef private

namespace blink {

class MockWebVideoFrameSubmitter : public WebVideoFrameSubmitter {
 public:
  MOCK_METHOD(void, Initialize, (cc::VideoFrameProvider*, bool), (override));
  MOCK_METHOD(void, SetTransform, (media::VideoTransformation), (override));
  MOCK_METHOD(void, EnableSubmission, (viz::SurfaceId), (override));
  MOCK_METHOD(void, SetIsSurfaceVisible, (bool), (override));
  MOCK_METHOD(void, SetIsPageVisible, (bool), (override));
  MOCK_METHOD(void, SetForceBeginFrames, (bool), (override));
  MOCK_METHOD(void, SetForceSubmit, (bool), (override));
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  MOCK_METHOD(void, SetHasNativeLayer, (bool has_native_layer), (override));
#endif
  MOCK_METHOD(void, StopUsingProvider, (), (override));
  MOCK_METHOD(void, StartRendering, (), (override));
  MOCK_METHOD(void, StopRendering, (), (override));
  MOCK_METHOD(void, DidReceiveFrame, (), (override));
  MOCK_METHOD(bool, IsDrivingFrameUpdates, (), (const, override));
};

class VideoFrameCompositorForIncludeTest : public testing::Test {
 public:
  void SetUp() override {
    task_environment_ = std::make_unique<base::test::TaskEnvironment>();
    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();

    auto mock_submitter = std::make_unique<MockWebVideoFrameSubmitter>();
    mock_submitter_ = mock_submitter.get();

    video_frame_compositor_ = std::make_unique<VideoFrameCompositor>(
          task_runner_, std::move(mock_submitter));
  }

  void TearDown() override {
    if (video_frame_compositor_) {
      video_frame_compositor_->Stop();
      video_frame_compositor_.reset();
    }
    task_environment_.reset();
  }

  std::unique_ptr<base::test::TaskEnvironment> task_environment_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  std::unique_ptr<VideoFrameCompositor> video_frame_compositor_;
  raw_ptr<MockWebVideoFrameSubmitter> mock_submitter_;
};

#if BUILDFLAG(ARKWEB_MEDIA)
TEST_F(VideoFrameCompositorForIncludeTest, SetFinishPaintCallback) {
  bool callback_called = false;
  VideoFrameCompositor::FinishPaintCallback callback = base::BindRepeating(
      [](bool* called) { *called = true; },
      &callback_called);

  video_frame_compositor_->SetFinishPaintCallback(std::move(callback));
  EXPECT_FALSE(callback_called);
}
#endif  // BUILDFLAG(ARKWEB_MEDIA)

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
TEST_F(VideoFrameCompositorForIncludeTest, GetFreezeTimeConditionNotMet) {
  int64_t positive_time = (base::Time::Now() - base::Time::UnixEpoch()).InMilliseconds() - 200;
  video_frame_compositor_->SetStartTime(positive_time);
  video_frame_compositor_->is_playing_ = false;
  int64_t freeze_time = video_frame_compositor_->GetFreezeTime();
  EXPECT_EQ(freeze_time, 0);

  video_frame_compositor_->SetStartTime(0);
  freeze_time = video_frame_compositor_->GetFreezeTime();
  EXPECT_EQ(freeze_time, 0);

  video_frame_compositor_->is_playing_ = true;
  video_frame_compositor_->last_frame_time_ = 0;
  freeze_time = video_frame_compositor_->GetFreezeTime();
  EXPECT_EQ(freeze_time, 0);

  video_frame_compositor_->is_playing_ = true;
  video_frame_compositor_->last_frame_time_ = -1;
  freeze_time = video_frame_compositor_->GetFreezeTime();
  EXPECT_EQ(freeze_time, 0);
}

TEST_F(VideoFrameCompositorForIncludeTest, GetFreezeTimePlayingValidTime) {
  int64_t recent_time = (base::Time::Now() - base::Time::UnixEpoch()).InMilliseconds() - 50;
  video_frame_compositor_->SetStartTime(recent_time);
  int64_t freeze_time = video_frame_compositor_->GetFreezeTime();
  EXPECT_EQ(freeze_time, 0);

  int64_t old_time = (base::Time::Now() - base::Time::UnixEpoch()).InMilliseconds() - 200;
  video_frame_compositor_->SetStartTime(old_time);
  freeze_time = video_frame_compositor_->GetFreezeTime();
  EXPECT_GT(freeze_time, 100);

  freeze_time = video_frame_compositor_->GetFreezeTime();
  EXPECT_EQ(freeze_time, 0);
}

TEST_F(VideoFrameCompositorForIncludeTest, ProcessNewFrame1) {
  scoped_refptr<media::VideoFrame> frame = media::VideoFrame::CreateBlackFrame(gfx::Size(320, 240));
  base::TimeTicks presentation_time = base::TimeTicks::Now();
  int64_t positive_time = (base::Time::Now() - base::Time::UnixEpoch()).InMilliseconds() - 200;
  video_frame_compositor_->is_playing_ = false;
  video_frame_compositor_->last_frame_time_ = positive_time;
  bool result = video_frame_compositor_->ProcessNewFrame(frame, presentation_time, true);
  EXPECT_TRUE(result);

  video_frame_compositor_->is_playing_ = false;
  video_frame_compositor_->last_frame_time_ = 0;
  result = video_frame_compositor_->ProcessNewFrame(frame, presentation_time, true);
  EXPECT_TRUE(result);

  video_frame_compositor_->is_playing_ = true;
  video_frame_compositor_->last_frame_time_ = 0;
  result = video_frame_compositor_->ProcessNewFrame(frame, presentation_time, true);
  EXPECT_TRUE(result);

  video_frame_compositor_->is_playing_ = true;
  video_frame_compositor_->last_frame_time_ = -1;
  result = video_frame_compositor_->ProcessNewFrame(frame, presentation_time, true);
  EXPECT_TRUE(result);

  int64_t recent_time = (base::Time::Now() - base::Time::UnixEpoch()).InMilliseconds() - 50;
  video_frame_compositor_->is_playing_ = true;
  video_frame_compositor_->last_frame_time_ = recent_time;
  result = video_frame_compositor_->ProcessNewFrame(frame, presentation_time, true);
  EXPECT_TRUE(result);

  int64_t old_time = (base::Time::Now() - base::Time::UnixEpoch()).InMilliseconds() - 200;
  video_frame_compositor_->is_playing_ = true;
  video_frame_compositor_->last_frame_time_ = old_time;
  result = video_frame_compositor_->ProcessNewFrame(frame, presentation_time, true);
  EXPECT_TRUE(result);
}

TEST_F(VideoFrameCompositorForIncludeTest, ProcessNewFrame2) {
  base::TimeTicks presentation_time = base::TimeTicks::Now();
  bool result = video_frame_compositor_->ProcessNewFrame(nullptr, presentation_time, false);
  EXPECT_FALSE(result);

  scoped_refptr<media::VideoFrame> frame = media::VideoFrame::CreateBlackFrame(gfx::Size(320, 240));
  result = video_frame_compositor_->ProcessNewFrame(frame, presentation_time, false);
  EXPECT_TRUE(result);

  result = video_frame_compositor_->ProcessNewFrame(frame, presentation_time, false);
  EXPECT_FALSE(result);

  result = video_frame_compositor_->ProcessNewFrame(frame, presentation_time, true);
  EXPECT_TRUE(result);
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
}  // namespace blink