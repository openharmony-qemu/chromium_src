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

#include "arkweb/chromium_ext/components/viz/service/display_embedder/skia_output_surface_dependency_impl_utils.h"
#include "components/viz/service/display_embedder/skia_output_surface_dependency_impl.h"
#include "components/viz/service/gl/gpu_service_impl.h"
#include "gpu/config/gpu_preferences.h"
#include "components/viz/test/test_gpu_service_holder.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

namespace viz {
class SkiaOutputSurfaceDependencyImplUtilsTests : public testing::Test {
 public:
   SkiaOutputSurfaceDependencyImplUtilsTests() = default;
   ~SkiaOutputSurfaceDependencyImplUtilsTests() = default;
};

#if BUILDFLAG(ARKWEB_D_VSYNC)
TEST_F(SkiaOutputSurfaceDependencyImplUtilsTests, GetIsScroll1) {
  SkiaOutputSurfaceDependencyImplUtils implUtils(nullptr);
  EXPECT_EQ(implUtils.skiaOutputSurfaceDependencyImpl, nullptr);
  EXPECT_FALSE(implUtils.GetIsScroll());
}

TEST_F(SkiaOutputSurfaceDependencyImplUtilsTests, GetIsScroll2) {
  SkiaOutputSurfaceDependencyImpl impl(nullptr, 1);
  SkiaOutputSurfaceDependencyImplUtils implUtils(&impl);
  EXPECT_NE(implUtils.skiaOutputSurfaceDependencyImpl, nullptr);
  EXPECT_FALSE(implUtils.GetIsScroll());
}

TEST_F(SkiaOutputSurfaceDependencyImplUtilsTests, GetIsScroll3) {
  auto gpu_service_impl = TestGpuServiceHolder::GetInstance()->gpu_service();
  gpu_service_impl->SetIsScroll(true);
  SkiaOutputSurfaceDependencyImpl impl(gpu_service_impl, 1);
  SkiaOutputSurfaceDependencyImplUtils implUtils(&impl);
  EXPECT_TRUE(implUtils.GetIsScroll());
}
#endif
} // namespace viz