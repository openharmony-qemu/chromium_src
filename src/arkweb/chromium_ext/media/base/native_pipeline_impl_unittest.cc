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
#include "base/logging.h"
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
#include "arkweb/chromium_ext/media/base/native_pipeline_impl.h"
#define ARKWEB_TEST_INCLUDE
#include "arkweb/chromium_ext/media/base/native_pipeline_impl.cc"
#undef private

using ::testing::Mock;
using ::testing::StrictMock;

namespace media {
using namespace OHOS::NWeb;
class MockClient : public media::NativePipeline::Client {
 public:
  void OnSetLayer() {}
  virtual ~MockClient() = default;
};

class MockSequencedTaskRunner : public base::SequencedTaskRunner {
public:
    MOCK_METHOD(bool, PostNonNestableTask, (const base::Location &location, base::OnceClosure onceClosure), ());
    MOCK_METHOD(bool, PostNonNestableDelayedTask,
        (const base::Location &location, base::OnceClosure onceClosure, base::TimeDelta time), (override));
    MOCK_METHOD(base::DelayedTaskHandle, PostCancelableDelayedTask,
        (base::subtle::PostDelayedTaskPassKey key, const base::Location &location, base::OnceClosure onceClosure,
            base::TimeDelta time),
        (override));
    MOCK_METHOD(base::DelayedTaskHandle, PostCancelableDelayedTaskAt,
        (base::subtle::PostDelayedTaskPassKey key, const base::Location &location, base::OnceClosure onceClosure,
            base::TimeTicks ticks, base::subtle::DelayPolicy policy),
        (override));
    MOCK_METHOD(bool, PostDelayedTaskAt,
        (base::subtle::PostDelayedTaskPassKey key, const base::Location &location, base::OnceClosure onceClosure,
            base::TimeTicks ticks, base::subtle::DelayPolicy policy),
        (override));
    MOCK_METHOD(bool, RunsTasksInCurrentSequence, (), (const, override));
    MOCK_METHOD(
        bool, PostDelayedTask, (const base::Location &location, base::OnceClosure, base::TimeDelta), (override));
    MOCK_METHOD(bool, DeleteOrReleaseSoonInternal,
        (const base::Location &location, void (*)(const void *), const void *), (override));
};

class NativePipelineImplTest : public ::testing::Test {
 public:
  NativePipelineImplTest() = default;
  ~NativePipelineImplTest() override = default;

  void SetUp() override {
    EXPECT_CALL(mock_task_runner_, RunsTasksInCurrentSequence())
        .WillRepeatedly(testing::Return(true));
    EXPECT_CALL(mock_task_runner_, PostDelayedTask(testing::_, testing::_, testing::_))
        .WillRepeatedly(testing::Return(false));
    EXPECT_CALL(mock_task_runner_, DeleteOrReleaseSoonInternal(testing::_, testing::_, testing::_))
        .WillRepeatedly(testing::Return(false));

    scoped_renderer_ = std::make_unique<StrictMock<MockRenderer>>();

    weak_this_ = weak_factory_.GetWeakPtr();

    pipeline_ = std::make_unique<NativePipelineImpl>(
      &mock_task_runner_,
      task_environment_.GetMainThreadTaskRunner(),
      base::BindRepeating(&NativePipelineImplTest::TakeRenderer, base::Unretained(this)));
  }

  void TearDown() override {
    pipeline_.reset();
    scoped_renderer_.reset();
  }

  std::unique_ptr<Renderer> TakeRenderer(std::optional<RendererType> renderer_type) {
    (void)renderer_type;
    return std::move(scoped_renderer_);
  }

  void OnSurfaceCreated(media::RectChangedCB rect_changed_cb, int native_embed_id) {}

  void OnSurfaceDestroyed() {}

  void OnPipelineStatus(media::PipelineStatus status) {}

  base::test::SingleThreadTaskEnvironment task_environment_;
  std::unique_ptr<StrictMock<MockRenderer>> scoped_renderer_;
  base::WeakPtr<NativePipelineImplTest> weak_this_;
  base::WeakPtrFactory<NativePipelineImplTest> weak_factory_{this};
  MockSequencedTaskRunner mock_task_runner_;
  std::unique_ptr<NativePipelineImpl> pipeline_;
};

TEST_F(NativePipelineImplTest, TestItem1) {
  auto client = std::make_unique<MockClient>();
  media::CreateTextureCB create_texture_cb = base::BindPostTaskToCurrentDefault(
      base::BindOnce(&NativePipelineImplTest::OnSurfaceCreated, weak_this_));
  media::DestroyTextureCB destroy_texture_cb = base::BindPostTaskToCurrentDefault(
      base::BindOnce(&NativePipelineImplTest::OnSurfaceDestroyed, weak_this_));
  pipeline_->Start(client.get(), std::move(create_texture_cb), std::move(destroy_texture_cb));
  EXPECT_TRUE(pipeline_->IsRunning());

  pipeline_->Stop();
  EXPECT_FALSE(pipeline_->IsRunning());
}

TEST_F(NativePipelineImplTest, TestItem2) {
  EXPECT_CALL(mock_task_runner_, RunsTasksInCurrentSequence()).WillRepeatedly(testing::Return(false));

  pipeline_->Stop();
  EXPECT_FALSE(pipeline_->IsRunning());
}

TEST_F(NativePipelineImplTest, TestItem3) {
  PipelineStatusCallback done_cb = base::BindOnce([](PipelineStatus status) {});
  pipeline_->renderer_wrapper_->CreateRendererInternal(std::move(done_cb));

  pipeline_->renderer_wrapper_->default_renderer_ = std::make_unique<testing::StrictMock<media::MockRenderer>>();
  PipelineStatusCallback done_cb1 = base::BindOnce([](PipelineStatus status) {
      EXPECT_EQ(status, PIPELINE_OK);
  });
  pipeline_->renderer_wrapper_->CreateRendererInternal(std::move(done_cb1));
}

TEST_F(NativePipelineImplTest, TestItem4) {
  PipelineStatus pipeline_status;
  PipelineStatusCallback done_cb = base::BindOnce([](PipelineStatus status) {
      EXPECT_EQ(status, PIPELINE_ERROR_INITIALIZATION_FAILED);
  });
  pipeline_->renderer_wrapper_->OnRendererCreated(std::move(done_cb), nullptr);
}

}  // namespace media
