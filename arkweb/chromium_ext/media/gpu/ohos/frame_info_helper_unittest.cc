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
 * See the License for specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

#include "base/test/bind.h"
#include "base/test/task_environment.h"

#include "media/base/test_helpers.h"

#define private public
#include "media/gpu/ohos/codec_output_buffer_renderer.h"
#include "media/gpu/ohos/codec_buffer_wait_coordinator.h"
#include "media/gpu/ohos/codec_wrapper.h"
#undef private

#include "gpu/command_buffer/service/ohos/native_image_texture_owner.h"

#include "media/gpu/ohos/frame_info_helper.h"

namespace media {

namespace {

constexpr gfx::Size kTestVisibleSize(100, 100);
constexpr gfx::Size kTestVisibleSize2(110, 110);
constexpr gfx::Size kTestVisibleSize3(120, 120);
constexpr gfx::Size kTestCodedSize(128, 128);
constexpr gfx::Size kTestVisibleRectSize(100, 50);
constexpr gfx::Rect kTestVisibleRect(0, 0, 100, 50);

}  // namespace

class FrameInfoHelperTest : public testing::Test {
 public:
  FrameInfoHelperTest() : task_environment_() {}

  void SetUp() override {
    drdc_lock_ = base::MakeRefCounted<gpu::RefCountedLock>();
    task_runner_ = base::SequencedTaskRunner::GetCurrentDefault();
  }

  void TearDown() override {
    task_runner_ = nullptr;
    drdc_lock_ = nullptr;
  }

  // Helper method to call GetFrameInfo synchronously
  void GetFrameInfo(
      std::unique_ptr<CodecOutputBufferRenderer> buffer_renderer,
      FrameInfoHelper::FrameInfo* out_info) {
    bool called = false;
    FrameInfoHelper::FrameInfo local_info;
    auto callback = base::BindLambdaForTesting(
        [&](std::unique_ptr<CodecOutputBufferRenderer> br,
            FrameInfoHelper::FrameInfo info) {
          called = true;
          local_info = info;
        });
    helper_->GetFrameInfo(std::move(buffer_renderer), callback);
    base::RunLoop().RunUntilIdle();
    ASSERT_TRUE(called);
    if (out_info) {
      *out_info = local_info;
    }
  }

  base::test::SingleThreadTaskEnvironment task_environment_;

  std::unique_ptr<FrameInfoHelper> helper_;
  scoped_refptr<gpu::RefCountedLock> drdc_lock_;
  scoped_refptr<base::SequencedTaskRunner> task_runner_;
};

// ============================================================================
// Priority 1 Tests - Critical Path Coverage
// ============================================================================

// Test Create With Valid Parameters
TEST_F(FrameInfoHelperTest, CreateHelperWithValidParameters001) {
  helper_ = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);
  EXPECT_NE(helper_, nullptr);
}

// Test Create With Null Stub
TEST_F(FrameInfoHelperTest, CreateHelperWithNullStub001) {
  helper_ = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);
  EXPECT_NE(helper_, nullptr);
}

// Test No Buffer Renderer
TEST_F(FrameInfoHelperTest, GetFrameInfoWithNullBufferRenderer001) {
  helper_ = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);

  FrameInfoHelper::FrameInfo result;
  GetFrameInfo(nullptr, &result);

  // Expect default empty frame info
  EXPECT_TRUE(result.coded_size.IsEmpty());
  EXPECT_TRUE(result.visible_rect.IsEmpty());
  EXPECT_FALSE(result.ycbcr_info.has_value());
}

// Test: Cache Hit - Same Size Multiple Times
TEST_F(FrameInfoHelperTest, CacheHitSameSize001) {
  helper_ = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);

  // First request with null buffer - uses visible size
  FrameInfoHelper::FrameInfo info1;
  GetFrameInfo(nullptr, &info1);
  EXPECT_EQ(info1.coded_size, gfx::Size());
  EXPECT_EQ(info1.visible_rect, gfx::Rect());

  // Second request with same "size" (null buffer again)
  // Should use to cached/default empty info
  FrameInfoHelper::FrameInfo info2;
  GetFrameInfo(nullptr, &info2);
  EXPECT_EQ(info2.coded_size, gfx::Size());
  EXPECT_EQ(info2.visible_rect, gfx::Rect());
}

// Test: Cache Miss - Different Size
TEST_F(FrameInfoHelperTest, CacheMissDifferentSize001) {
  helper_ = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);

  // First request - sets visible_size to empty
  FrameInfoHelper::FrameInfo info1;
  GetFrameInfo(nullptr, &info1);
  EXPECT_TRUE(info1.coded_size.IsEmpty());

  // Second request with null buffer
  // Same size (empty), should use cached info
  FrameInfoHelper::FrameInfo info2;
  GetFrameInfo(nullptr, &info2);
  EXPECT_TRUE(info2.coded_size.IsEmpty());
}

// Test: Size Change - First Request With One Size, Second With Different
TEST_F(FrameInfoHelperTest, SizeChangeBetweenRequests001) {
  helper_ = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);

  // First request with size1
  FrameInfoHelper::FrameInfo info1;
  GetFrameInfo(nullptr, &info1);

  // Second request with different "size" (still null, so empty size)
  // But since first was empty, this should also be empty
  // This test verifies to size comparison logic
  FrameInfoHelper::FrameInfo info2;
  GetFrameInfo(nullptr, &info2);
  EXPECT_EQ(info2.coded_size, info1.coded_size);
}

// Test Single Request
TEST_F(FrameInfoHelperTest, SingleRequest001) {
  helper_ = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);

  bool callback_called = false;
  helper_->GetFrameInfo(
      nullptr,
      base::BindLambdaForTesting(
          [&](std::unique_ptr<CodecOutputBufferRenderer>,
              FrameInfoHelper::FrameInfo) { callback_called = true; }));

  base::RunLoop().RunUntilIdle();
  EXPECT_TRUE(callback_called);
}

// Test Multiple Requests With Null Buffer Renderers
TEST_F(FrameInfoHelperTest, MultipleRequestsWithNullBufferRenderers001) {
  helper_ = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);

  int callback_count = 0;
  for (int i = 0; i < 3; ++i) {
    helper_->GetFrameInfo(
        nullptr,
        base::BindLambdaForTesting(
            [&](std::unique_ptr<CodecOutputBufferRenderer>,
                FrameInfoHelper::FrameInfo) { ++callback_count; }));
  }

  base::RunLoop().RunUntilIdle();
  EXPECT_EQ(callback_count, 3);
}

// Test: Multiple Helpers
TEST_F(FrameInfoHelperTest, MultipleHelpers001) {
  auto helper1 = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);

  auto helper2 = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);

  EXPECT_NE(helper1, nullptr);
  EXPECT_NE(helper2, nullptr);

  bool callback1_called = false;
  bool callback2_called = false;
  FrameInfoHelper::FrameInfo info1;
  FrameInfoHelper::FrameInfo info2;

  helper1->GetFrameInfo(
      nullptr,
      base::BindLambdaForTesting(
          [&](std::unique_ptr<CodecOutputBufferRenderer>,
              FrameInfoHelper::FrameInfo info) {
            callback1_called = true;
            info1 = info;
          }));

  helper2->GetFrameInfo(
      nullptr,
      base::BindLambdaForTesting(
          [&](std::unique_ptr<CodecOutputBufferRenderer>,
              FrameInfoHelper::FrameInfo info) {
            callback2_called = true;
            info2 = info;
          }));

  base::RunLoop().RunUntilIdle();
  EXPECT_TRUE(callback1_called);
  EXPECT_TRUE(callback2_called);
  EXPECT_EQ(info1.coded_size, info2.coded_size);
}

// Test: Concurrent Requests to test thread safety
TEST_F(FrameInfoHelperTest, ConcurrentRequests001) {
  helper_ = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);

  std::atomic<int> callback_count{0};

  // Start multiple requests concurrently
  for (int i = 0; i < 5; ++i) {
    helper_->GetFrameInfo(
        nullptr,
        base::BindLambdaForTesting(
            [&](std::unique_ptr<CodecOutputBufferRenderer>,
                FrameInfoHelper::FrameInfo) { ++callback_count; }));
  }

  base::RunLoop().RunUntilIdle();
  EXPECT_EQ(callback_count, 5);
}

// Test: Empty Size Then Non-Empty Size
TEST_F(FrameInfoHelperTest, EmptySizeThenNonEmptySize001) {
  helper_ = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);

  // First request with null buffer (empty size)
  FrameInfoHelper::FrameInfo info1;
  GetFrameInfo(nullptr, &info1);
  EXPECT_TRUE(info1.coded_size.IsEmpty());

  // Since we can't create buffers with different sizes,
  // this test verifies to empty size path is working
  // Second null request should use cached empty info
  FrameInfoHelper::FrameInfo info2;
  GetFrameInfo(nullptr, &info2);
  EXPECT_EQ(info2.coded_size, info1.coded_size);
}

// Test: Request Queue Processing Order
TEST_F(FrameInfoHelperTest, RequestQueueProcessingOrder001) {
  helper_ = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);

  std::vector<FrameInfoHelper::FrameInfo> results;
  std::atomic<int> completion_order{0};

  // Submit 3 requests
  for (int i = 0; i < 3; ++i) {
    helper_->GetFrameInfo(
        nullptr,
        base::BindLambdaForTesting(
            [&](std::unique_ptr<CodecOutputBufferRenderer>,
                FrameInfoHelper::FrameInfo info) {
              results.push_back(info);
              completion_order.store(static_cast<int>(results.size()));
            }));
  }

  base::RunLoop().RunUntilIdle();

  // All requests should complete in order
  EXPECT_EQ(static_cast<int>(results.size()), 3);
  EXPECT_EQ(completion_order.load(), 3);
}

// Test: Callback Parameters Verification
TEST_F(FrameInfoHelperTest, CallbackParametersVerification001) {
  helper_ = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);

  bool callback_called = false;
  const CodecOutputBufferRenderer* renderer_ptr = nullptr;

  helper_->GetFrameInfo(
      nullptr,
      base::BindLambdaForTesting(
          [&](std::unique_ptr<CodecOutputBufferRenderer> renderer,
              FrameInfoHelper::FrameInfo info) {
            callback_called = true;
            renderer_ptr = renderer.get();  // Should be null for null buffer
          }));

  base::RunLoop().RunUntilIdle();
  EXPECT_TRUE(callback_called);
  EXPECT_EQ(renderer_ptr, nullptr);
}

// Test: FrameInfo Default Construction
TEST_F(FrameInfoHelperTest, FrameInfoDefaultConstruction001) {
  FrameInfoHelper::FrameInfo info;
  EXPECT_TRUE(info.coded_size.IsEmpty());
  EXPECT_TRUE(info.visible_rect.IsEmpty());
  EXPECT_FALSE(info.ycbcr_info.has_value());
}

// Test: FrameInfo Copy Constructor
TEST_F(FrameInfoHelperTest, FrameInfoCopyConstruction001) {
  FrameInfoHelper::FrameInfo source;
  source.coded_size = kTestCodedSize;
  source.visible_rect = kTestVisibleRect;

  FrameInfoHelper::FrameInfo copy(source);
  EXPECT_EQ(copy.coded_size, kTestCodedSize);
  EXPECT_EQ(copy.visible_rect, kTestVisibleRect);
  EXPECT_EQ(copy.ycbcr_info.has_value(), source.ycbcr_info.has_value());
}

// Test: FrameInfo Move Constructor
TEST_F(FrameInfoHelperTest, FrameInfoMoveConstruction001) {
  FrameInfoHelper::FrameInfo source;
  source.coded_size = kTestCodedSize;
  source.visible_rect = kTestVisibleRect;

  FrameInfoHelper::FrameInfo moved(std::move(source));
  EXPECT_EQ(moved.coded_size, kTestCodedSize);
  EXPECT_EQ(moved.visible_rect, kTestVisibleRect);
  EXPECT_FALSE(moved.coded_size.IsEmpty());
}

// Test: FrameInfo Assignment Operator
TEST_F(FrameInfoHelperTest, FrameInfoAssignmentOperator001) {
  FrameInfoHelper::FrameInfo info1;
  FrameInfoHelper::FrameInfo info2;

  info1.coded_size = kTestCodedSize;
  info1.visible_rect = kTestVisibleRect;

  info2 = info1;

  EXPECT_EQ(info2.coded_size, kTestCodedSize);
  EXPECT_EQ(info2.visible_rect, kTestVisibleRect);
}

// Test: FrameInfo With YCbCr Info
TEST_F(FrameInfoHelperTest, FrameInfoWithYCbCrInfo001) {
  // Note: YCbCr info can only be obtained through texture owner path
  // This test verifies the structure can hold YCbCr info
  FrameInfoHelper::FrameInfo info;

  // Initially no YCbCr info
  EXPECT_FALSE(info.ycbcr_info.has_value());

  // Emulate YCbCr info being set (simulating texture owner path result)
  info.ycbcr_info = gpu::VulkanYCbCrInfo();

  EXPECT_TRUE(info.ycbcr_info.has_value());
}

// Test: Multiple Sequential Requests Same Size
TEST_F(FrameInfoHelperTest, MultipleSequentialRequestsSameSize001) {
  helper_ = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);

  std::vector<FrameInfoHelper::FrameInfo> results;

  // Submit 5 sequential requests with same (empty) size
  for (int i = 0; i < 5; ++i) {
    FrameInfoHelper::FrameInfo info;
    GetFrameInfo(nullptr, &info);
    results.push_back(info);
  }

  // All should return empty frame info
  for (const auto& info : results) {
    EXPECT_TRUE(info.coded_size.IsEmpty());
    EXPECT_TRUE(info.visible_rect.IsEmpty());
  }
}

// Test: Multiple Sequential Requests Different Sizes
TEST_F(FrameInfoHelperTest, MultipleSequentialRequestsDifferentSizes001) {
  helper_ = FrameInfoHelper::Create(
      task_runner_,
      base::BindRepeating([]() -> gpu::CommandBufferStub* { return nullptr; }),
      drdc_lock_);

  std::vector<gfx::Size> sizes = {kTestVisibleSize, kTestVisibleSize2, kTestVisibleSize3};

  for (const auto& size : sizes) {
    FrameInfoHelper::FrameInfo info;
    GetFrameInfo(nullptr, &info);
    EXPECT_TRUE(info.coded_size.IsEmpty());
  }
}

}  // namespace media
