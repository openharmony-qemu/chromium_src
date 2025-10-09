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

#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/widget/compositing/layer_tree_settings_utils.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/platform/widget/compositing/layer_tree_settings.h"
#include "third_party/blink/public/platform/platform.h"

using ::testing::Return;

namespace blink {
namespace {

TEST(LayerTreeSettingsUtilsTest, AdjustMemoryForSmallScreen) {
  cc::ManagedMemoryPolicy policy(0);
  AdjustMemoryLimitBasedOnScreenWidth(policy, gfx::Size(1000, 800), 1.0f);

  EXPECT_EQ(policy.bytes_limit_when_visible, 512u * 1024 * 1024);
  EXPECT_EQ(policy.priority_cutoff_when_visible, 
            gpu::MemoryAllocation::CUTOFF_ALLOW_NICE_TO_HAVE);
}

TEST(LayerTreeSettingsUtilsTest, AdjustMemoryForLargeScreen) {
  cc::ManagedMemoryPolicy policy(0);

  AdjustMemoryLimitBasedOnScreenWidth(policy, gfx::Size(4000, 2000), 1.0f);

  EXPECT_EQ(policy.bytes_limit_when_visible, 2 * 512u * 1024 * 1024);
}

class MockPlatform : public Platform {
 public:
  MOCK_METHOD(int32_t, GetDrawMode, ());
};

TEST(LayerTreeSettingsUtilsTest, SetMaxVisibleBytesWhenDrawModeEnabled) {
  cc::ManagedMemoryPolicy policy(3000u * 1024 * 1024);

  MockPlatform platform;
  EXPECT_CALL(platform, GetDrawMode()).WillOnce(Return(true));
  Platform::SetCurrentPlatformForTesting(&platform);

  SetMaxVisibleBytes(policy);

  EXPECT_EQ(policy.bytes_limit_when_visible, 3000u * 1024 * 1024);
}

TEST(LayerTreeSettingsUtilsTest, ConfigureScrollbarWhenOverlayEnabled) {
  cc::LayerTreeSettings settings;
  ConfigureOverlayScrollbarSettings(settings);
}

TEST(LayerTreeSettingsUtilsTest, AdjustSettingsForLowMemory) {
  cc::LayerTreeSettings settings;
  AdjustGraphicsSettings(gfx::Size(), settings, true);

  EXPECT_EQ(settings.max_memory_for_prepaint_percentage, 67);
}

TEST(GraphicsUtilitiesTest, AdjustSettingsForNormalMemory) {
  cc::LayerTreeSettings settings;
  AdjustGraphicsSettings(gfx::Size(), settings, false);
  
  EXPECT_EQ(settings.max_memory_for_prepaint_percentage, 50);
  EXPECT_TRUE(settings.create_low_res_tiling);
}

class TestGraphicsUtilities : public ::testing::Test {
 protected:
  void SetUp() override {
    command_line_ = base::CommandLine::ForCurrentProcess();
  }

  base::CommandLine* command_line_;
};

#if BUILDFLAG(IS_ARKWEB_EXT)
TEST_F(TestGraphicsUtilities, SetDeleteDelayWhenConditionsMet) {
  command_line_->AppendSwitch(::switches::kEnableNwebEx);
  cc::LayerTreeSettings settings;
  settings.enable_delete_unused_resources_delay = true;
  SetEnableDeleteUnusedResourcesDelay(settings);
}
#endif

}  // namespace
}  // namespace blink