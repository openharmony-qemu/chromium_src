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
#include "base/functional/bind.h"
#include "base/memory/scoped_refptr.h"
#include "base/task/sequenced_task_runner.h"
#include "base/test/mock_callback.h"
#include "base/test/task_environment.h"
#include "base/test/test_simple_task_runner.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "media/gpu/ohos/shared_image_video_provider.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#include "frame_info_helper.cc"
#include "frame_info_helper.h"
#undef private
namespace media {

class FrameInfoHelperTest : public ::testing::Test {
 protected:
  void SetUp() override {
    gpu_task_runner_ = base::MakeRefCounted<base::TestSimpleTaskRunner>();
    drdc_lock_ = base::MakeRefCounted<gpu::RefCountedLock>();
  }

  scoped_refptr<base::SequencedTaskRunner> gpu_task_runner_;
  scoped_refptr<gpu::RefCountedLock> drdc_lock_;
};

TEST_F(FrameInfoHelperTest, Create_HappyPath) {
  auto get_stub_cb = SharedImageVideoProvider::GetStubCB();
  auto frame_info_helper =
      FrameInfoHelper::Create(gpu_task_runner_, get_stub_cb, drdc_lock_);
  EXPECT_NE(frame_info_helper, nullptr);
}

TEST_F(FrameInfoHelperTest, GetFrameInfo_001) {
  auto get_stub_cb = SharedImageVideoProvider::GetStubCB();
  std::unique_ptr<FrameInfoHelperImpl> frame_info_helper_impl =
      std::make_unique<FrameInfoHelperImpl>(gpu_task_runner_, get_stub_cb,
                                            drdc_lock_);
  std::unique_ptr<CodecOutputBufferRenderer> buffer_renderer;
  base::MockCallback<FrameInfoHelper::FrameInfoReadyCB> callback;
  EXPECT_CALL(callback, Run(testing::_, testing::_))
      .WillOnce(testing::Invoke(
          [](std::unique_ptr<media::CodecOutputBufferRenderer> output,
             media::FrameInfoHelper::FrameInfo frameInfo) {
            ASSERT_EQ(output, nullptr);
          }));
  frame_info_helper_impl->GetFrameInfo(std::move(buffer_renderer),
                                       callback.Get());
}

TEST_F(FrameInfoHelperTest, GetFrameInfo_002) {
  auto get_stub_cb = SharedImageVideoProvider::GetStubCB();
  std::unique_ptr<FrameInfoHelperImpl> frame_info_helper_impl =
      std::make_unique<FrameInfoHelperImpl>(gpu_task_runner_, get_stub_cb,
                                            drdc_lock_);
  std::unique_ptr<CodecOutputBufferRenderer> buffer_renderer;
  base::MockCallback<FrameInfoHelper::FrameInfoReadyCB> callback;
  FrameInfoHelperImpl::Request req = {.buffer_renderer = nullptr,
                                      .callback = callback.Get()};
  frame_info_helper_impl->requests_.push(std::move(req));
  frame_info_helper_impl->GetFrameInfo(std::move(buffer_renderer),
                                       callback.Get());
  unsigned int requests_size = 2;
  EXPECT_EQ(frame_info_helper_impl->requests_.size(), requests_size);
}
}  // namespace media
