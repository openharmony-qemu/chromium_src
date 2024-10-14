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

#include "media/mojo/services/gpu_mojo_media_client.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace media {

class AbstractTextureOHOSTest : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST(AbstractTextureOHOSTest, GetPlatformSupportedVideoDecoderConfigs) {
  gpu::GpuDriverBugWorkarounds gpu_workarounds;
  gpu::GpuPreferences gpu_preferences;
  const gpu::GPUInfo gpu_info;
  base::OnceCallback<SupportedVideoDecoderConfigs()> get_vda_configs;
  testing::internal::CaptureStderr();
  std::string log_output1 = testing::internal::GetCapturedStderr();
  GetPlatformSupportedVideoDecoderConfigs(gpu_workarounds, gpu_preferences,
                                          gpu_info, std::move(get_vda_configs));
  EXPECT_EQ(log_output1.find("GetPlatformSupportedVideoDecoderConfigs"),
            std::string::npos);
}

}  // namespace media
