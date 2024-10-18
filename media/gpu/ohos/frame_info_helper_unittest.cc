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

#include "base/task/sequenced_task_runner.h"
#include "base/test/task_environment.h"
#include "base/test/test_simple_task_runner.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "media/gpu/ohos/shared_image_video_provider.h"

#define private public
#include "frame_info_helper.h"

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
}  // namespace media
