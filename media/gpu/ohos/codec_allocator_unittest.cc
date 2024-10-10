/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/memory/raw_ptr.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/thread_pool.h"
#include "base/test/simple_test_tick_clock.h"
#include "base/test/task_environment.h"
#include "base/threading/thread.h"
#include "base/time/tick_clock.h"
#include "base/time/time.h"
#include "media/base/ohos/ohos_media_decoder_bridge_impl.h"

#define private public
#include "media/gpu/ohos/codec_allocator.h"

using namespace testing;
using namespace OHOS::NWeb;
namespace media {

class MockVideoBridgeCodecConfig : public VideoBridgeCodecConfig {
 public:
  MockVideoBridgeCodecConfig() {}
  ~MockVideoBridgeCodecConfig() = default;
};

class CodecAllocatorTest : public ::testing::Test {
 public:
  void SetUp() override {
    task_runner_ = base::ThreadPool::CreateSequencedTaskRunner(
        {base::TaskPriority::USER_VISIBLE, base::MayBlock(),
         base::TaskShutdownBehavior::SKIP_ON_SHUTDOWN});
    allocator_ = CodecAllocator::GetInstance(task_runner_);
  }

  scoped_refptr<base::SequencedTaskRunner> task_runner_;
  CodecAllocator* allocator_;
};

TEST_F(CodecAllocatorTest, CodecAllocatorTest_GetInstance_001) {
  auto instance1 = CodecAllocator::GetInstance(task_runner_);
  auto instance2 = CodecAllocator::GetInstance(task_runner_);
  EXPECT_EQ(instance1, instance2);
}

TEST_F(CodecAllocatorTest, CodecAllocatorTest_CreateMediaCodecAsync) {}

TEST_F(CodecAllocatorTest, CodecAllocatorTest_ReleaseMediaCodec) {}

TEST_F(CodecAllocatorTest, CodecAllocatorTest_OnCodecCreated) {
  auto codec_created_cb =
      base::BindOnce([](std::unique_ptr<MediaCodecDecoderBridgeImpl> codec) {});
  std::unique_ptr<VideoBridgeCodecConfig> config =
      std::make_unique<MockVideoBridgeCodecConfig>();
  auto codec = MediaCodecDecoderBridgeImpl::CreateVideoDecoder(*config);
  allocator_->OnCodecCreated(base::TimeTicks::Now(),
                             std::move(codec_created_cb), std::move(codec));
  EXPECT_NE(allocator_, nullptr);
}

TEST_F(CodecAllocatorTest, CodecAllocatorTest_OnCodecReleased) {
  auto codec_created_cb = base::BindOnce([]() {});
  allocator_->OnCodecReleased(base::TimeTicks::Now(),
                              std::move(codec_created_cb));
  EXPECT_NE(allocator_, nullptr);
}

TEST_F(CodecAllocatorTest, CodecAllocatorTest_SelectCodecTaskRunner) {
  allocator_->SelectCodecTaskRunner();
  EXPECT_NE(allocator_, nullptr);
}
}  // namespace media
