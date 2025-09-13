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

#include <iostream>
#include <memory>

#include "base/functional/bind.h"
#include "base/location.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/raw_ptr_exclusion.h"
#include "base/memory/weak_ptr.h"
#include "base/run_loop.h"
#include "base/task/single_thread_task_runner.h"
#include "base/test/gmock_callback_support.h"
#include "base/test/simple_test_tick_clock.h"
#include "base/test/task_environment.h"
#include "base/threading/simple_thread.h"
#include "base/time/clock.h"
#include "base/time/time.h"
#include "media/base/media_util.h"
#include "media/base/mock_filters.h"
#include "media/base/test_helpers.h"
#include "media/base/serial_runner.h"

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_refptr.h"

#include "testing/gtest/include/gtest/gtest.h"

#define private public
#include "media/base/pipeline_impl.h"
#define ARKWEB_TEST_INCLUDE
#include "media/base/pipeline_impl.cc"
#undef private

using ::testing::Mock;
using ::testing::StrictMock;

namespace media {
using namespace OHOS::NWeb;
class PipelineImplUnitTest : public ::testing::Test {
 public:
  PipelineImplUnitTest() = default;
  ~PipelineImplUnitTest() override = default;

  void SetUp() override {
    scoped_renderer_ = std::make_unique<StrictMock<MockRenderer>>();
    weak_this_ = weak_factory_.GetWeakPtr();
    pipeline_ = std::make_unique<PipelineImpl>(
        task_environment_.GetMainThreadTaskRunner(),
        task_environment_.GetMainThreadTaskRunner(),
        base::BindRepeating(&PipelineImplUnitTest::TakeRenderer, base::Unretained(this)),
        &media_log_);
  }

  void TearDown() override {
    pipeline_.reset();
    scoped_renderer_.reset();
  }

  std::unique_ptr<Renderer> TakeRenderer(std::optional<RendererType> /* renderer_type */) {
    return std::move(scoped_renderer_);
  }

  base::test::SingleThreadTaskEnvironment task_environment_;
  std::unique_ptr<StrictMock<MockRenderer>> scoped_renderer_;
  NullMediaLog media_log_;
  base::WeakPtr<PipelineImplUnitTest> weak_this_;
  base::WeakPtrFactory<PipelineImplUnitTest> weak_factory_{this};
  std::unique_ptr<PipelineImpl> pipeline_;
};

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
TEST_F(PipelineImplUnitTest, TestItem1) {
  auto demuxer = std::make_unique<StrictMock<MockDemuxer>>();
  pipeline_->renderer_wrapper_->demuxer_ = demuxer.get();

  pipeline_->SetPlaybackRateWithReason(-1.0, ActionReason::kNormal);
  pipeline_->SetPlaybackRateWithReason(1.0, ActionReason::kNormal);

  pipeline_->renderer_wrapper_->state_ = PipelineImpl::RendererWrapper::State::kCreated;
  pipeline_->renderer_wrapper_->SetPlaybackRateWithReason(1.0, ActionReason::kNormal);

  EXPECT_CALL(*demuxer, SetPlaybackRate(1.0)).Times(1);
  pipeline_->renderer_wrapper_->state_ = PipelineImpl::RendererWrapper::State::kStarting;
  pipeline_->renderer_wrapper_->SetPlaybackRateWithReason(1.0, ActionReason::kNormal);

  auto mock_renderer = std::make_unique<testing::StrictMock<media::MockRenderer>>();
  EXPECT_CALL(*mock_renderer, SetPlaybackRateWithReason(1.0, ActionReason::kNormal)).Times(1);
  EXPECT_CALL(*demuxer, SetPlaybackRate(1.0)).Times(1);
  pipeline_->renderer_wrapper_->shared_state_.renderer = std::move(mock_renderer);
  pipeline_->renderer_wrapper_->state_ = PipelineImpl::RendererWrapper::State::kPlaying;
  pipeline_->renderer_wrapper_->SetPlaybackRateWithReason(1.0, ActionReason::kNormal);
  pipeline_->renderer_wrapper_->shared_state_.renderer = nullptr;
}

TEST_F(PipelineImplUnitTest, TestItem2) {
  pipeline_->renderer_wrapper_->shared_state_.renderer = nullptr;
  pipeline_->renderer_wrapper_->SetMediaPlayerState(false, 0);

  auto mock_renderer = std::make_unique<testing::StrictMock<media::MockRenderer>>();
  EXPECT_CALL(*mock_renderer, SetMediaPlayerState(true, 0)).Times(1);
  pipeline_->renderer_wrapper_->shared_state_.renderer = std::move(mock_renderer);
  pipeline_->renderer_wrapper_->SetMediaPlayerState(true, 0);
  pipeline_->renderer_wrapper_->shared_state_.renderer = nullptr;
}
#endif

#if BUILDFLAG(ARKWEB_PIP)
TEST_F(PipelineImplUnitTest, TestItem3) {
  pipeline_->renderer_wrapper_->shared_state_.renderer = nullptr;
  pipeline_->renderer_wrapper_->PipEnable(true);

  auto mock_renderer = std::make_unique<testing::StrictMock<media::MockRenderer>>();
  EXPECT_CALL(*mock_renderer, PipEnable(true)).Times(1);
  pipeline_->renderer_wrapper_->shared_state_.renderer = std::move(mock_renderer);
  pipeline_->renderer_wrapper_->PipEnable(true);
  pipeline_->renderer_wrapper_->shared_state_.renderer = nullptr;
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
TEST_F(PipelineImplUnitTest, TestItem4) {
  pipeline_->renderer_wrapper_->shared_state_.renderer = nullptr;
  pipeline_->renderer_wrapper_->RecycleDmaBuffer();

  auto mock_renderer = std::make_unique<testing::StrictMock<media::MockRenderer>>();
  EXPECT_CALL(*mock_renderer, RecycleDmaBuffer()).Times(1);
  pipeline_->renderer_wrapper_->shared_state_.renderer = std::move(mock_renderer);
  pipeline_->renderer_wrapper_->RecycleDmaBuffer();
  pipeline_->renderer_wrapper_->shared_state_.renderer = nullptr;
}

TEST_F(PipelineImplUnitTest, TestItem5) {
  pipeline_->renderer_wrapper_->shared_state_.renderer = nullptr;
  pipeline_->renderer_wrapper_->ResumeDmaBuffer();

  auto mock_renderer = std::make_unique<testing::StrictMock<media::MockRenderer>>();
  EXPECT_CALL(*mock_renderer, ResumeDmaBuffer()).Times(1);
  pipeline_->renderer_wrapper_->shared_state_.renderer = std::move(mock_renderer);
  pipeline_->renderer_wrapper_->ResumeDmaBuffer();
  pipeline_->renderer_wrapper_->shared_state_.renderer = nullptr;
}
#endif

}  // namespace media
