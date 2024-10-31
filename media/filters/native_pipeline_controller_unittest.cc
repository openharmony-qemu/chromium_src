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

#include <memory>
#define private public
#include "media/filters/native_pipeline_controller.h"
#undef private
#include "base/test/task_environment.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace media {

class MockNativePipeline : public NativePipeline {
 public:
  MOCK_METHOD(void,
              Start,
              (Client*, CreateTextureCB, DestroyTextureCB),
              (override));
  MOCK_METHOD(bool, IsRunning, (), (const, override));
  MOCK_METHOD(void, Stop, (), (override));
};

class NativePipelineControllerTest : public testing::Test {
 protected:
  NativePipelineControllerTest()
      : native_pipeline_(std::make_unique<MockNativePipeline>()),
        controller_(std::make_unique<NativePipelineController>(
            std::make_unique<MockNativePipeline>())) {}

  void SetUp() override { thread_checker_.DetachFromThread(); }

  std::unique_ptr<MockNativePipeline> native_pipeline_;
  std::unique_ptr<NativePipelineController> controller_;
  base::test::TaskEnvironment task_environment_;
  base::ThreadChecker thread_checker_;
};

TEST_F(NativePipelineControllerTest, Stop_ResetsNativePipeline) {
  EXPECT_NE(native_pipeline_, nullptr);
  ASSERT_TRUE(thread_checker_.CalledOnValidThread());
  EXPECT_CALL(*native_pipeline_, Stop()).Times(1);
  native_pipeline_->Stop();
  EXPECT_TRUE(native_pipeline_);
}
}  // namespace media
