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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "base/test/bind.h"
#include "content/browser/ohos/software_compositor_host_ohos.h"
#include "content/browser/renderer_host/render_widget_host_impl.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/test/test_browser_context.h"
#include "content/public/test/test_renderer_host.h"

namespace content {
struct CallbackTestResult {
  bool callback_called = false;
  std::string received_id;
  bool received_state = false;
  void* received_data = nullptr;
  int received_width = -1;
  int received_height = -1;
};
class SoftwareCompositorHostOhosTest : public RenderViewHostTestHarness {
 public:
  SoftwareCompositorHostOhosTest() : RenderViewHostTestHarness(
      base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}

 protected:
  void SetUp() override {
    RenderViewHostTestHarness::SetUp();
    RenderWidgetHostImpl* widget = RenderWidgetHostImpl::From(
        web_contents()->GetRenderWidgetHostView()->GetRenderWidgetHost());
    host_ = std::make_unique<SoftwareCompositorHostOhos>(widget);
  }

  void TearDown() override {
    host_.reset();
    RenderViewHostTestHarness::TearDown();
  }

  SoftwareCompositorHostOhos::WebSnapchatCallback CreateTestCallback(
      CallbackTestResult* result) {
    return base::BindLambdaForTesting([result](const char* id, bool state,void* data, int width, int height) {
      result->callback_called = true;
      if (id != nullptr) {
        result->received_id = std::string(id);
      }
      result->received_state = state;
      result->received_data = data;
      result->received_width = width;
      result->received_height = height;
    });
  }
  gfx::SizeF GetCurrentSize() const { return host_->current_; }
  std::unique_ptr<SoftwareCompositorHostOhos> host_;
};

TEST_F(SoftwareCompositorHostOhosTest, OnDrawSwCallbackWithFalseResult) {
  testing::internal::CaptureStderr();
  CallbackTestResult test_result;
  host_->OnDrawSwCallback(CreateTestCallback(&test_result), "test_false", false);
  EXPECT_TRUE(test_result.callback_called);
  EXPECT_EQ(test_result.received_width, 0);
}

TEST_F(SoftwareCompositorHostOhosTest, OnDrawSwCallbackWithTrueResult) {
  CallbackTestResult test_result;
  host_->OnDrawSwCallback(CreateTestCallback(&test_result), "test_true", true);
  EXPECT_TRUE(test_result.callback_called);
  std::string info_log = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(info_log.find("OnDrawSwCallback: shared memory is invalid!") != std::string::npos);
}

TEST_F(SoftwareCompositorHostOhosTest, DemandDrawSwAsync_clipWidthZero) {
  CallbackTestResult test_result;
  auto callback = CreateTestCallback(&test_result);
  host_->DemandDrawSwAsync("test_clipW0", 0, 800, gfx::SizeF(1024, 768), gfx::PointF(), std::move(callback));
  base::RunLoop().RunUntilIdle();
  gfx::SizeF current_size = GetCurrentSize();
  EXPECT_FLOAT_EQ(current_size.width(), 1024.0f);
  EXPECT_FLOAT_EQ(current_size.height(), 768.0f);
}

TEST_F(SoftwareCompositorHostOhosTest, DemandDrawSwAsync_clipHeightZero) {
  CallbackTestResult test_result;
  auto callback = CreateTestCallback(&test_result);
  host_->DemandDrawSwAsync("test_clipH0", 800, 0, gfx::SizeF(1024, 768), gfx::PointF(), std::move(callback));
  base::RunLoop().RunUntilIdle();
  gfx::SizeF current_size = GetCurrentSize();
  EXPECT_FLOAT_EQ(current_size.width(), 800.0f);
  EXPECT_FLOAT_EQ(current_size.height(), 768.0f);
}

TEST_F(SoftwareCompositorHostOhosTest, DemandDrawSwAsync_byteSizeOverflow) {
  const char* test_id = "test_overflow";
  float clip_width = 100000.0f;
  float clip_height = 100000.0f;
  gfx::SizeF size(100000.0f, 100000.0f);
  gfx::PointF offset(0.0f, 0.0f);

  CallbackTestResult test_result;
  auto callback = CreateTestCallback(&test_result);

  host_->DemandDrawSwAsync(test_id, clip_width, clip_height, size, offset,
      std::move(callback));

  base::RunLoop().RunUntilIdle();

  EXPECT_TRUE(test_result.callback_called);
  EXPECT_FALSE(test_result.received_state);
  EXPECT_EQ(test_result.received_data, nullptr);
  EXPECT_EQ(test_result.received_width, 0);
  EXPECT_EQ(test_result.received_height, 0);
}

TEST_F(SoftwareCompositorHostOhosTest, DemandDrawSwAsync_clipBothZero) {
  testing::internal::CaptureStderr();
  const char* test_id = "test_both_zero";
  float clip_width = 0;
  float clip_height = 0;
  gfx::SizeF size(1024.0f, 768.0f);
  gfx::PointF offset(0.0f, 0.0f);

  CallbackTestResult test_result;
  auto callback = CreateTestCallback(&test_result);

  host_->DemandDrawSwAsync(test_id, clip_width, clip_height, size, offset,
      std::move(callback));

  std::string info_log = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(info_log.find("start DemandDrawSwAsync width:0") != std::string::npos);

  base::RunLoop().RunUntilIdle();
  gfx::SizeF current_size = GetCurrentSize();
  EXPECT_FLOAT_EQ(current_size.width(), 1024.0f);
  EXPECT_FLOAT_EQ(current_size.height(), 768.0f);
}

TEST_F(SoftwareCompositorHostOhosTest, DemandDrawSwAsync_clipBothNonZero) {
  const char* test_id = "test_both_non_zero";
  float clip_width = 800.0f;
  float clip_height = 600.0f;
  gfx::SizeF size(1024.0f, 768.0f);
  gfx::PointF offset(0.0f, 0.0f);
  CallbackTestResult test_result;
  auto callback = CreateTestCallback(&test_result);

  host_->DemandDrawSwAsync(test_id, clip_width, clip_height, size, offset,
      std::move(callback));

  base::RunLoop().RunUntilIdle();
  gfx::SizeF current_size = GetCurrentSize();
  EXPECT_FLOAT_EQ(current_size.width(), 800.0f);
  EXPECT_FLOAT_EQ(current_size.height(), 600.0f);
}

TEST_F(SoftwareCompositorHostOhosTest, SetSharedMemory_creatShmRegionError) {
  testing::internal::CaptureStderr();
  EXPECT_NO_FATAL_FAILURE(host_->SetSharedMemory(0, 0));
  std::string info_log = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(info_log.find("create shm region error") != std::string::npos);
}
}  // namespace content