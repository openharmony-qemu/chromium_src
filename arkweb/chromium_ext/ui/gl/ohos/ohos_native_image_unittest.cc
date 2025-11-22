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

#include "base/containers/span.h"
#define private public
#define protected public
#include "arkweb/chromium_ext/ui/gl/ohos/ohos_native_image.h"
#undef private
#undef protected

#include <cstdint>

#include <array>
#include <memory>
#include <string_view>

#include "base/command_line.h"
#include "base/command_line.h"
#include "base/strings/string_split.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_display.h"
#include "ui/gl/gl_display_egl_util.h"
#include "ui/gl/init/gl_factory.h"
#include "ui/gl/test/gl_surface_test_support.h"
#include "ui/gl/test/gl_test_support.h"


namespace gl {
namespace {

constexpr int32_t kMockAcceleratedWidget = 12345;

class OhosNativeImageTest : public testing::Test {
 public:
  void SetUp() override {
    gl::init::InitializeGLNoExtensionsOneOff(
        /*init_bindings=*/true, /*gpu_preference=*/gl::GpuPreference::kDefault);
    display_ = gl::GLTestSupport::InitializeGL(std::nullopt);
    ASSERT_NE(display_, nullptr);
    surf_ = gl::init::CreateOffscreenGLSurface(display_, gfx::Size());
    ASSERT_NE(surf_, nullptr);
    gl_context_ = gl::init::CreateGLContext(nullptr, surf_.get(), gl::GLContextAttribs());
    ASSERT_NE(gl_context_, nullptr);
    bool make_current_result = gl_context_->MakeCurrent(surf_.get());
    ASSERT_TRUE(make_current_result);
  }

 protected:
  scoped_refptr<gl::GLSurface> surf_;
  scoped_refptr<gl::GLContext> gl_context_;
  raw_ptr<GLDisplay> display_ = nullptr;
};

TEST_F(OhosNativeImageTest, SetFrameAvailableCallback) {
  auto ohos_native_image = OhosNativeImage::Create(12345);
  EXPECT_NE(ohos_native_image, nullptr);
  ohos_native_image->SetFrameAvailableCallback(base::DoNothing());
}

TEST_F(OhosNativeImageTest, UpdateNativeImage) {
  auto ohos_native_image = OhosNativeImage::Create(12345);
  EXPECT_NE(ohos_native_image, nullptr);
  ohos_native_image->UpdateNativeImage();
  ohos_native_image->native_image_adapter_ = nullptr;
  ohos_native_image->UpdateNativeImage();
}

TEST_F(OhosNativeImageTest, GetSurfaceId) {
  auto ohos_native_image = OhosNativeImage::Create(12345);
  EXPECT_NE(ohos_native_image, nullptr);
  uint64_t surface_id = 0;
  ohos_native_image->GetSurfaceId(&surface_id);
  ohos_native_image->native_image_adapter_ = nullptr;
  ohos_native_image->GetSurfaceId(&surface_id);
}

TEST_F(OhosNativeImageTest, GetTransformMatrix) {
  auto ohos_native_image = OhosNativeImage::Create(12345);
  EXPECT_NE(ohos_native_image, nullptr);
  float mtx[16];
  ohos_native_image->GetTransformMatrix(mtx);
  ohos_native_image->native_image_adapter_ = nullptr;
  ohos_native_image->GetTransformMatrix(mtx);
}

TEST_F(OhosNativeImageTest, AttachToGLContext) {
  auto ohos_native_image = OhosNativeImage::Create(12345);
  EXPECT_NE(ohos_native_image, nullptr);
  ohos_native_image->AttachToGLContext();
  ohos_native_image->native_image_adapter_ = nullptr;
  ohos_native_image->AttachToGLContext();
}

TEST_F(OhosNativeImageTest, DetachFromGLContext) {
  auto ohos_native_image = OhosNativeImage::Create(12345);
  EXPECT_NE(ohos_native_image, nullptr);
  ohos_native_image->DetachFromGLContext();
  ohos_native_image->native_image_adapter_ = nullptr;
  ohos_native_image->DetachFromGLContext();
}

TEST_F(OhosNativeImageTest, ReleaseNativeImage) {
  auto ohos_native_image = OhosNativeImage::Create(12345);
  EXPECT_NE(ohos_native_image, nullptr);
  ohos_native_image->ReleaseNativeImage();
  ohos_native_image->native_image_adapter_ = nullptr;
  ohos_native_image->ReleaseNativeImage();
}

TEST_F(OhosNativeImageTest, AquireOhosNativeWindow) {
  auto ohos_native_image = OhosNativeImage::Create(12345);
  EXPECT_NE(ohos_native_image, nullptr);
  ohos_native_image->AquireOhosNativeWindow();
  ohos_native_image->native_image_adapter_ = nullptr;
  ohos_native_image->AquireOhosNativeWindow();
}

TEST_F(OhosNativeImageTest, OnFrameAvailableListener) {
  auto ohos_native_image = OhosNativeImage::Create(12345);
  EXPECT_NE(ohos_native_image, nullptr);
  ohos_native_image->SetFrameAvailableCallback(base::DoNothing());
  ohos_native_image->OnFrameAvailableListener();
}

TEST_F(OhosNativeImageTest, create) {
  auto ohos_native_image = OhosNativeImage::Create();
  EXPECT_NE(ohos_native_image, nullptr);
}

TEST_F(OhosNativeImageTest, WindowBuffer) {
  auto ohos_native_image = OhosNativeImage::Create(12345);
  EXPECT_NE(ohos_native_image, nullptr);
  void* buffer = nullptr;
  int fence_fd = -1;
  int32_t ret = ohos_native_image->AcquireNativeWindowBuffer(&buffer, &fence_fd);

  void* nativeBuffer = nullptr;
  ohos_native_image->GetNativeBuffer(buffer, &nativeBuffer);

  uint32_t width = 800;
  uint32_t height = 800;
  ohos_native_image->GetNativeWindowBufferSize(buffer, &width, &height);

  ohos_native_image->ReleaseNativeWindowBuffer(buffer, fence_fd);
}

TEST_F(OhosNativeImageTest, WindowBufferNullptr) {
  auto ohos_native_image = OhosNativeImage::Create(12345);
  EXPECT_NE(ohos_native_image, nullptr);
  ohos_native_image->native_image_adapter_ = nullptr;

  void* buffer = nullptr;
  int fence_fd = -1;
  int32_t ret = ohos_native_image->AcquireNativeWindowBuffer(&buffer, &fence_fd);

  void* nativeBuffer = nullptr;
  ohos_native_image->GetNativeBuffer(buffer, &nativeBuffer);

  uint32_t width = 800;
  uint32_t height = 800;
  ohos_native_image->GetNativeWindowBufferSize(buffer, &width, &height);

  ohos_native_image->ReleaseNativeWindowBuffer(buffer, fence_fd);
}

TEST_F(OhosNativeImageTest, GetTransformMatrixV1) {
  auto ohos_native_image = OhosNativeImage::Create(12345);
  EXPECT_NE(ohos_native_image, nullptr);
  float mtx[16];
  ohos_native_image->GetTransformMatrixV1(mtx, 16u);
  ohos_native_image->native_image_adapter_ = nullptr;
  ohos_native_image->GetTransformMatrixV1(mtx, 16u);
}

}  // namespace
}  // namespace gl