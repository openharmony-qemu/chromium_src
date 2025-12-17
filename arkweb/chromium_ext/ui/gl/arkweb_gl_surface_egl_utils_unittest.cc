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

#include <cstdint>

#include <array>
#include <memory>
#include <string_view>

#include "arkweb/chromium_ext/ui/gl/arkweb_gl_surface_egl_utils.h"
#include "base/command_line.h"
#include "base/strings/string_split.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace gl {

class ArkwebGlSurfaceEglUtilsTest : public testing::Test {
 public:

  void SetUp() override {
    arkweb_surface_utils_ = std::make_unique<ArkwebGlSurfaceEglUtils>();
  }

  void TearDown() override {
  }

 protected:
  std::unique_ptr<ArkwebGlSurfaceEglUtils> arkweb_surface_utils_;
};

TEST_F(ArkwebGlSurfaceEglUtilsTest, CheckSwapBufferOutputFlagTest) {
  auto result = arkweb_surface_utils_->CheckSwapBufferOutputFlag();
  EXPECT_FALSE(result);
}

TEST_F(ArkwebGlSurfaceEglUtilsTest, SwapBuffersSolutionTest) {
  gfx::Size size{1920, 1080};
  auto result = arkweb_surface_utils_->SwapBuffersSolution(false, false, size);
  EXPECT_FALSE(result);
  arkweb_surface_utils_->SwapBuffersSolution(false, false, size);
  arkweb_surface_utils_->SwapBuffersSolution(false, true, size);
}

TEST_F(ArkwebGlSurfaceEglUtilsTest, SwapBuffersWithDamageSolutionTest) {
  gfx::Size size{1920, 1080};
  auto swapResult = gfx::SwapResult::SWAP_FAILED;
  auto start = std::chrono::high_resolution_clock::now();
  arkweb_surface_utils_->SwapBuffersWithDamageSolution(swapResult, start, size);
  start = std::chrono::steady_clock::now() - std::chrono::milliseconds(17);
  arkweb_surface_utils_->SwapBuffersWithDamageSolution(swapResult, start, size);
  start = std::chrono::steady_clock::now() - std::chrono::milliseconds(3001);
  arkweb_surface_utils_->SwapBuffersWithDamageSolution(swapResult, start, size);
}

}  // namespace gl
