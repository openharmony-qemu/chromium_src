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

#include <array>
#include <memory>
#include <cstdint>
#include <string_view>
#include <sys/eventfd.h>
#include <poll.h>

#include "arkweb/chromium_ext/gpu/command_buffer/service/ohos/native_buffer_config_impl.h"
#include "arkweb/chromium_ext/ui/gl/ohos/native_buffer_utils.h"
#include "base/ohos/scoped_native_buffer_handle.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_surface.h"
#include "ui/gl/gl_utils.h"
#include "ui/gl/init/gl_factory.h"
#include "ui/gl/test/gl_surface_test_support.h"
#include "ui/gl/test/gl_test_support.h"


namespace gl {
namespace ohos {
namespace {

class NativeBufferUtilsTest : public testing::Test {
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

    std::shared_ptr<OHOS::NWeb::NativeBufferConfigAdapterImpl> configAdapter =
    std::make_shared<OHOS::NWeb::NativeBufferConfigAdapterImpl>();
    configAdapter->SetBufferWidth(size_.width());
    configAdapter->SetBufferHeight(size_.height());
    configAdapter->SetBufferUsage(
      gpu::OH_NativeBuffer_Usage::NATIVEBUFFER_USAGE_HW_RENDER |
      gpu::OH_NativeBuffer_Usage::NATIVEBUFFER_USAGE_HW_TEXTURE |
      gpu::OH_NativeBuffer_Usage::NATIVEBUFFER_USAGE_MEM_DMA |
      gpu::OH_NativeBuffer_Usage::NATIVEBUFFER_USAGE_CPU_READ |
      gpu::OH_NativeBuffer_Usage::NATIVEBUFFER_USAGE_CPU_WRITE);
    OHOS::NWeb::OhosNativeBufferAdapter& adapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().GetOhosNativeBufferAdapter();
    adapter.Allocate(configAdapter, &buffer_);
  }
 protected:
    scoped_refptr<gl::GLSurface> surf_;
    scoped_refptr<gl::GLContext> gl_context_;
    raw_ptr<GLDisplay> display_ = nullptr;
    void* buffer_ = nullptr;
    gfx::Size size_{1920, 1080};
};

TEST_F(NativeBufferUtilsTest, CreateEGLImage) {
  EGLClientBuffer egl_client_buffer;
  GetEGLClientBufferFromNativeBuffer(buffer_, &egl_client_buffer);
  auto scoped_image = CreateEGLImage(egl_client_buffer);
  EXPECT_NE(scoped_image.get(), EGL_NO_IMAGE_KHR);
}

TEST_F(NativeBufferUtilsTest, CreateEGLImageForVideo) {
  EGLClientBuffer egl_client_buffer;
  GetEGLClientBufferFromNativeBuffer(buffer_, &egl_client_buffer);
  auto scoped_image = CreateEGLImageForVideo(egl_client_buffer);
  EXPECT_NE(scoped_image.get(), EGL_NO_IMAGE_KHR);
}

TEST_F(NativeBufferUtilsTest, GetEGLClientBufferFromNativeBuffer) {
  EGLClientBuffer egl_client_buffer;
  auto result = GetEGLClientBufferFromNativeBuffer(buffer_, &egl_client_buffer);
  ASSERT_EQ(result, 0);
}

TEST_F(NativeBufferUtilsTest, FreeEGLClientBuffer) {
  EGLClientBuffer egl_client_buffer;
  GetEGLClientBufferFromNativeBuffer(buffer_, &egl_client_buffer);
  FreeEGLClientBuffer(&egl_client_buffer);
}

TEST_F(NativeBufferUtilsTest, InsertEglFenceAndWait_NoFence) {
  base::ScopedFD no_fence;
  EXPECT_TRUE(InsertEglFenceAndWait(std::move(no_fence)));
}

TEST_F(NativeBufferUtilsTest, InsertEglFenceAndWait_ValidFence) {
  int fds[2];
  ASSERT_EQ(pipe(fds), 0);
  base::ScopedFD read_end(fds[0]);
  base::ScopedFD write_end(fds[1]);

  ASSERT_EQ(write(write_end.get(), "x", 1), 1);

  EXPECT_TRUE(InsertEglFenceAndWait(std::move(read_end)));
}

TEST_F(NativeBufferUtilsTest, NegativeFd) {
  base::ScopedFD no_fence;
  EXPECT_TRUE(SyncFenceWait(std::move(no_fence)));
}

TEST_F(NativeBufferUtilsTest, ReadableFence) {
  int fds[2];
  ASSERT_EQ(pipe(fds), 0);
  base::ScopedFD read_end(fds[0]);
  base::ScopedFD write_end(fds[1]);

  ASSERT_EQ(write(write_end.get(), "x", 1), 1);

  EXPECT_TRUE(SyncFenceWait(std::move(read_end)));
}

TEST_F(NativeBufferUtilsTest, Timeout) {
  int fd = eventfd(0, EFD_CLOEXEC);
  ASSERT_NE(fd, -1);
  base::ScopedFD event_fd(fd);

  struct pollfd pfd = {.fd = fd, .events = POLLIN};
  int ret = poll(&pfd, 1, 10);
  ASSERT_EQ(ret, 0);

  EXPECT_FALSE(SyncFenceWait(std::move(event_fd)));
}

TEST_F(NativeBufferUtilsTest, EINTR_RetryThenSuccess) {
  int fds[2];
  ASSERT_EQ(pipe(fds), 0);
  base::ScopedFD read_end(fds[0]);
  base::ScopedFD write_end(fds[1]);

  ASSERT_EQ(write(write_end.get(), "x", 1), 1);

  EXPECT_TRUE(SyncFenceWait(std::move(read_end)));
}

}  // namespace
}  // namespace ohos
}  // namespace gl