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

#include "arkweb/chromium_ext/components/viz/service/display/arkweb_display_utils.h"

#include "arkweb/build/features/features.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace viz {

class ArkwebDisplayUtilsTest : public testing::Test {
 public:
  ArkwebDisplayUtilsTest() = default;
  ~ArkwebDisplayUtilsTest() = default;

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  void TestSetDrawRect() {
    ArkwebDisplayUtils utils(nullptr);
    const gfx::Rect test_rect(10, 10);
    utils.SetDrawRect(test_rect);
    EXPECT_NE(utils.draw_rect_, test_rect);
  }

  void TestSetDrawMode() {
    ArkwebDisplayUtils utils(nullptr);
    const int32_t mode = 1;
    utils.SetDrawMode(mode);
    EXPECT_EQ(utils.draw_mode_, mode);
  }
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
  void TestDisableSwapUntilMaximized() {
    ArkwebDisplayUtils utils(nullptr);
    utils.reset_init_timer_ = nullptr;
    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::DISABLE_SWAP;
    utils.DisableSwapUntilMaximized();
    EXPECT_EQ(utils.temp_idle_state_, ArkwebDisplayUtils::TempIdleState::INIT);
  }

  void TestShouldDisableSwap() {
    ArkwebDisplayUtils utils(nullptr);
    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::INIT;
    EXPECT_TRUE(utils.ShouldDisableSwap(true));
    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::DISABLE_SWAP;
    EXPECT_FALSE(utils.ShouldDisableSwap(true));
    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::INIT;
    EXPECT_FALSE(utils.ShouldDisableSwap(false));
    utils.temp_idle_state_ = ArkwebDisplayUtils::TempIdleState::DISABLE_SWAP;
    EXPECT_FALSE(utils.ShouldDisableSwap(false));
  }
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void TestSetClientId() {
    ArkwebDisplayUtils utils(nullptr);
    const uint32_t client_id = 1;
    utils.SetClientId(client_id);
    EXPECT_EQ(utils.client_id_, client_id);
  }

  void TestSetGpuChannelManager() {
    ArkwebDisplayUtils utils(nullptr);
    raw_ptr<gpu::GpuChannelManager> test_manager;
    utils.SetGpuChannelManager(test_manager);
    EXPECT_EQ(utils.gpu_channel_manager_, test_manager);
  }
#endif
};

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
TEST_F(ArkwebDisplayUtilsTest, SetDrawRect) {
  TestSetDrawRect();
}

TEST_F(ArkwebDisplayUtilsTest, SetDrawMode) {
  TestSetDrawMode();
}
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
TEST_F(ArkwebDisplayUtilsTest, DisableSwapUntilMaximized) {
  TestDisableSwapUntilMaximized();
}

TEST_F(ArkwebDisplayUtilsTest, ShouldDisableSwap) {
  TestShouldDisableSwap();
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
TEST_F(ArkwebDisplayUtilsTest, RemoveDuplicatesRect) {
  std::vector<gfx::Rect> quad_list_empty;
  ArkwebDisplayUtils utils(nullptr);
  utils.removeDuplicatesRect(quad_list_empty);

  std::vector<gfx::Rect> quad_list_in{gfx::Rect(1, 1), gfx::Rect(2, 2),
                                      gfx::Rect(1, 1), gfx::Rect(3, 3),
                                      gfx::Rect(1, 1), gfx::Rect(4, 4)};
  std::vector<gfx::Rect> quad_list_unique{gfx::Rect(1, 1), gfx::Rect(2, 2),
                                          gfx::Rect(3, 3), gfx::Rect(4, 4)};
  utils.removeDuplicatesRect(quad_list_in);
  std::sort(quad_list_in.begin(), quad_list_in.end());
  std::sort(quad_list_unique.begin(), quad_list_unique.end());
  EXPECT_TRUE(quad_list_in == quad_list_unique);
}

TEST_F(ArkwebDisplayUtilsTest, SetClientId) {
  TestSetClientId();
}

TEST_F(ArkwebDisplayUtilsTest, SetGpuChannelManager) {
  TestSetGpuChannelManager();
}
#endif
}  // namespace viz
