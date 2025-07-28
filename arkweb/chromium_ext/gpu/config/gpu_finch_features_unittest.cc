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

#include "gpu/config/gpu_finch_features.h"

#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "arkweb/chromium_ext/gpu/config/gpu_finch_features_ext.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace features {

class gpufinchfeaturesTest : public ::testing::Test {
 public:
  void SetUp() {
    base::CommandLine::Reset();
    const char* argv[] = {""};
    base::CommandLine::Init(1, argv);
  }
};

TEST_F(gpufinchfeaturesTest, IsDrDcEnabledTest) {
  auto result = features::IsDrDcEnabled();
  EXPECT_FALSE(result);
}

#if BUILDFLAG(ARKWEB_VULKAN)
TEST_F(gpufinchfeaturesTest, VulkanSwitchTest) {
  auto* cmd = base::CommandLine::ForCurrentProcess();
  EXPECT_FALSE(cmd->HasSwitch(switches::kOhosEnableVulkan));
  cmd->AppendSwitch(switches::kOhosEnableVulkan);
  EXPECT_TRUE(cmd->HasSwitch(switches::kOhosEnableVulkan));
  cmd->RemoveSwitch(switches::kOhosEnableVulkan);
  EXPECT_FALSE(cmd->HasSwitch(switches::kOhosEnableVulkan));
}
#endif

#if BUILDFLAG(ARKWEB_DRDC)
TEST_F(gpufinchfeaturesTest, DrDcSwitchTest) {
  auto* cmd = base::CommandLine::ForCurrentProcess();
  EXPECT_FALSE(cmd->HasSwitch(::switches::kOhosEnableDrDc));
  cmd->AppendSwitch(::switches::kOhosEnableDrDc);
  EXPECT_TRUE(cmd->HasSwitch(::switches::kOhosEnableDrDc));
  cmd->RemoveSwitch(::switches::kOhosEnableDrDc);
  EXPECT_FALSE(cmd->HasSwitch(::switches::kOhosEnableDrDc));
}
#endif

}  // namespace features
