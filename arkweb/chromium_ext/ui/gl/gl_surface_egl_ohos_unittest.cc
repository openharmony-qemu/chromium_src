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

#include "arkweb/chromium_ext/ui/gl/gl_surface_egl_ohos.cc"
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

class GlSurfaceEglOhosTest : public testing::Test {
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

TEST_F(GlSurfaceEglOhosTest, CreateNativeViewGLSurfaceEGLOhosSuccess) {
  base::CommandLine::ForCurrentProcess()->RemoveSwitch(::switches::kOhosHanceSurface);

  auto surface = NativeViewGLSurfaceEGLOhos::CreateNativeViewGLSurfaceEGLOhos(
      display_.get(), 1);
  EXPECT_NE(surface, nullptr);
}

TEST_F(GlSurfaceEglOhosTest, CreateNativeViewGLSurfaceEGLOhosFailed) {
  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kOhosHanceSurface);

  auto surface = NativeViewGLSurfaceEGLOhos::CreateNativeViewGLSurfaceEGLOhos(
      display_.get(), 4);
  EXPECT_EQ(surface, nullptr);
}

TEST_F(GlSurfaceEglOhosTest, SwapBuffers) {
  auto surface = NativeViewGLSurfaceEGLOhos::CreateNativeViewGLSurfaceEGLOhos(
    display_, kMockAcceleratedWidget);
  ASSERT_TRUE(surface);

  gfx::FrameData frame_data;
  auto result = surface->SwapBuffers(base::DoNothing(), frame_data);

  EXPECT_TRUE(result == gfx::SwapResult::SWAP_ACK ||
              result == gfx::SwapResult::SWAP_FAILED);
}

TEST_F(GlSurfaceEglOhosTest, Resize) {
  auto surface = NativeViewGLSurfaceEGLOhos::CreateNativeViewGLSurfaceEGLOhos(
    display_, kMockAcceleratedWidget);
  ASSERT_TRUE(surface);

  gfx::Size size(0, 0);
  bool ok = surface->Resize(size, 1.0f, gfx::ColorSpace(), false);

  EXPECT_FALSE(ok);
}

TEST_F(GlSurfaceEglOhosTest, SetBackbufferAllocation) {
  auto surface = NativeViewGLSurfaceEGLOhos::CreateNativeViewGLSurfaceEGLOhos(
      display_, kMockAcceleratedWidget);
  ASSERT_TRUE(surface);

  surface->SetNativeInnerWeb(true);
  surface->SetBackbufferAllocation(false);

  surface->SetNativeInnerWeb(false);
  surface->SetBackbufferAllocation(false);


  auto* info = new WindowsSurfaceInfo{
    .display = eglGetCurrentDisplay(),
    .surface = eglGetCurrentSurface(EGL_DRAW),
    .context = eglGetCurrentContext(),
    .window = reinterpret_cast<void*>(0x2222)};

  auto res = NWebNativeWindowTracker::GetInstance()->AddNativeWindow(static_cast<void*>(info));
  void* window = NWebNativeWindowTracker::GetInstance()->GetNativeWindow(res);
  NativeViewGLSurfaceEGLOhos* new_surface = new NativeViewGLSurfaceEGLOhos(
    display_->GetAs<gl::GLDisplayEGL>(),
    reinterpret_cast<EGLNativeWindowType>(window));
  new_surface->SetBackbufferAllocation(false);
}

TEST_F(GlSurfaceEglOhosTest, SetBypassVsyncConditionNoWindow) {
  auto surface = NativeViewGLSurfaceEGLOhos::CreateNativeViewGLSurfaceEGLOhos(
    display_, kMockAcceleratedWidget);
  ASSERT_TRUE(surface);
  surface->SetBypassVsyncCondition(0);
  surface->SetBypassVsyncCondition(5);
}

TEST_F(GlSurfaceEglOhosTest, SetBypassVsyncConditionHasWindow) {
  auto* info = new WindowsSurfaceInfo{
    .display = eglGetCurrentDisplay(),
    .surface = eglGetCurrentSurface(EGL_DRAW),
    .context = eglGetCurrentContext(),
    .window = reinterpret_cast<void*>(0x2222)};

  auto res = NWebNativeWindowTracker::GetInstance()->AddNativeWindow(static_cast<void*>(info));
  void* window = NWebNativeWindowTracker::GetInstance()->GetNativeWindow(res);
  NativeViewGLSurfaceEGLOhos* surface = new NativeViewGLSurfaceEGLOhos(
    display_->GetAs<gl::GLDisplayEGL>(),
    reinterpret_cast<EGLNativeWindowType>(window));
  surface->SetBypassVsyncCondition(5);
}


}  // namespace
}  // namespace gl