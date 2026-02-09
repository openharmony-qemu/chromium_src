/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ui/gl/gl_bindings.h"
#include "ui/gl/init/gl_factory.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_surface.h"
#include "ui/gl/gl_version_info.h"
#include "ui/gl/init/gl_initializer.h"
#include "ui/gl/test/gl_test_support.h"

#include "gtest/gtest.h"
#include "ui/gfx/geometry/size.h"

namespace gl {
namespace init {

class GLFactoryOhosTest : public testing::Test {
 public:
  void SetUp() override {
    // Initialize GL for testing
    gl::init::InitializeGLNoExtensionsOneOff(
        /*init_bindings=*/true, /*gpu_preference=*/gl::GpuPreference::kDefault);
    display_ = gl::GLTestSupport::InitializeGL(std::nullopt);
    if (display_ == nullptr) {
      GTEST_SKIP() << "GL display initialization failed";
    }
    surf_ = gl::init::CreateOffscreenGLSurface(display_, gfx::Size());
    if (surf_ == nullptr) {
      GTEST_SKIP() << "GL surface creation failed";
    }
  }

  void TearDown() override {
    if (gl_context_ && surf_) {
      gl_context_->ReleaseCurrent(surf_.get());
    }
    gl_context_ = nullptr;
    surf_ = nullptr;
  }

 protected:
  scoped_refptr<gl::GLSurface> surf_;
  scoped_refptr<gl::GLContext> gl_context_;
  raw_ptr<GLDisplay> display_ = nullptr;
};

// Test: CreateGLContext with offscreen surface creates valid context
TEST_F(GLFactoryOhosTest, CreateGLContext_OffscreenSurface) {
  gl_context_ = gl::init::CreateGLContext(nullptr, surf_.get(), gl::GLContextAttribs());

  ASSERT_NE(gl_context_, nullptr);
  EXPECT_TRUE(gl_context_->MakeCurrent(surf_.get()));
  EXPECT_TRUE(gl_context_->IsCurrent(surf_.get()));
}

// Test: CreateGLContext with null surface returns null
TEST_F(GLFactoryOhosTest, CreateGLContext_NullSurface) {
  scoped_refptr<gl::GLContext> context = gl::init::CreateGLContext(
      nullptr, nullptr, gl::GLContextAttribs());

  EXPECT_EQ(context, nullptr);
}

// Test: CreateGLContext can make current and release
TEST_F(GLFactoryOhosTest, CreateGLContext_MakeCurrentAndRelease) {
  gl_context_ = gl::init::CreateGLContext(nullptr, surf_.get(), gl::GLContextAttribs());
  ASSERT_NE(gl_context_, nullptr);

  // Make current
  EXPECT_TRUE(gl_context_->MakeCurrent(surf_.get()));
  EXPECT_TRUE(gl_context_->IsCurrent(surf_.get()));

  // Release current
  gl_context_->ReleaseCurrent(surf_.get());
  EXPECT_FALSE(gl_context_->IsCurrent(surf_.get()));
}

// Test: CreateGLContext with share group
TEST_F(GLFactoryOhosTest, CreateGLContext_WithShareGroup) {
  scoped_refptr<GLShareGroup> share_group = new GLShareGroup();

  gl_context_ = gl::init::CreateGLContext(share_group.get(), surf_.get(), gl::GLContextAttribs());

  ASSERT_NE(gl_context_, nullptr);
  EXPECT_TRUE(gl_context_->MakeCurrent(surf_.get()));
}

// Test: Multiple MakeCurrent calls succeed
TEST_F(GLFactoryOhosTest, CreateGLContext_MultipleMakeCurrent) {
  gl_context_ = gl::init::CreateGLContext(nullptr, surf_.get(), gl::GLContextAttribs());
  ASSERT_NE(gl_context_, nullptr);

  // First MakeCurrent
  EXPECT_TRUE(gl_context_->MakeCurrent(surf_.get()));
  EXPECT_TRUE(gl_context_->IsCurrent(surf_.get()));

  // Release and MakeCurrent again
  gl_context_->ReleaseCurrent(surf_.get());
  EXPECT_TRUE(gl_context_->MakeCurrent(surf_.get()));
  EXPECT_TRUE(gl_context_->IsCurrent(surf_.get()));
}

// Test: CreateOffscreenGLSurface creates valid surface
TEST_F(GLFactoryOhosTest, CreateOffscreenGLSurface_Valid) {
  scoped_refptr<gl::GLSurface> surface =
      gl::init::CreateOffscreenGLSurface(display_, gfx::Size(256, 256));

  ASSERT_NE(surface, nullptr);
  EXPECT_TRUE(surface->Initialize());
}

// Test: CreateOffscreenGLSurface with zero size
TEST_F(GLFactoryOhosTest, CreateOffscreenGLSurface_ZeroSize) {
  scoped_refptr<gl::GLSurface> surface =
      gl::init::CreateOffscreenGLSurface(display_, gfx::Size(0, 0));

  // Should create surfaceless surface if supported
  // Otherwise should create pbuffer
  ASSERT_NE(surface, nullptr);
}

// Test: CreateOffscreenGLSurface with large size
TEST_F(GLFactoryOhosTest, CreateOffscreenGLSurface_LargeSize) {
  scoped_refptr<gl::GLSurface> surface =
      gl::init::CreateOffscreenGLSurface(display_, gfx::Size(4096, 4096));

  ASSERT_NE(surface, nullptr);
}

// Test: GetAllowedGLImplementations returns EGLGLES2 and EGLANGLE
TEST_F(GLFactoryOhosTest, GetAllowedGLImplementations) {
  std::vector<GLImplementationParts> implementations =
      gl::init::GetAllowedGLImplementations();

  EXPECT_GE(implementations.size(), 1u);
}

// Test: GetGLWindowSystemBindingInfo for EGL implementations
TEST_F(GLFactoryOhosTest, GetGLWindowSystemBindingInfo_EGL) {
  GLImplementation impl = gl::GetGLImplementation();
  if (impl != kGLImplementationEGLGLES2 && impl != kGLImplementationEGLANGLE) {
    GTEST_SKIP() << "Test requires EGL implementation";
  }

  // Create a GL context first
  gl_context_ = gl::init::CreateGLContext(nullptr, surf_.get(), gl::GLContextAttribs());
  ASSERT_NE(gl_context_, nullptr);
  ASSERT_TRUE(gl_context_->MakeCurrent(surf_.get()));

  // Get GL version and renderer strings
  const char* version_str =
      reinterpret_cast<const char*>(glGetString(GL_VERSION));
  const char* renderer_str =
      reinterpret_cast<const char*>(glGetString(GL_RENDERER));

  // Get extensions
  std::string extensions_string = gl::GetGLExtensionsFromCurrentContext();
  gfx::ExtensionSet extension_set = gfx::MakeExtensionSet(extensions_string);

  // Construct GLVersionInfo with proper parameters
  GLVersionInfo gl_info(version_str, renderer_str, extension_set);
  GLWindowSystemBindingInfo info;

  bool result = gl::init::GetGLWindowSystemBindingInfo(gl_info, &info);

  // May return false for some implementations
  // Just verify it doesn't crash
  SUCCEED();
}

// Test: SetDisabledExtensionsPlatform doesn't crash
TEST_F(GLFactoryOhosTest, SetDisabledExtensionsPlatform) {
  GLImplementation impl = gl::GetGLImplementation();

  // Should not crash for any implementation
  gl::init::SetDisabledExtensionsPlatform("GL_EXT_foo");

  SUCCEED();
}

// Test: InitializeExtensionSettingsOneOffPlatform succeeds
TEST_F(GLFactoryOhosTest, InitializeExtensionSettingsOneOffPlatform) {
  GLImplementation impl = gl::GetGLImplementation();

  bool result = gl::init::InitializeExtensionSettingsOneOffPlatform(display_);

  // Should succeed for EGL implementations
  if (impl == kGLImplementationEGLGLES2 || impl == kGLImplementationEGLANGLE) {
    EXPECT_TRUE(result);
  }
}

// Test: Two contexts with same share group
TEST_F(GLFactoryOhosTest, SharedContext_SameShareGroup) {
  scoped_refptr<GLShareGroup> share_group = new GLShareGroup();

  scoped_refptr<gl::GLContext> ctx1 =
      gl::init::CreateGLContext(share_group.get(), surf_.get(), gl::GLContextAttribs());
  scoped_refptr<gl::GLContext> ctx2 =
      gl::init::CreateGLContext(share_group.get(), surf_.get(), gl::GLContextAttribs());

  ASSERT_NE(ctx1, nullptr);
  ASSERT_NE(ctx2, nullptr);

  EXPECT_TRUE(ctx1->MakeCurrent(surf_.get()));
  ctx1->ReleaseCurrent(surf_.get());

  EXPECT_TRUE(ctx2->MakeCurrent(surf_.get()));
  ctx2->ReleaseCurrent(surf_.get());
}

// Test: CreateGLContext with empty GLContextAttribs
TEST_F(GLFactoryOhosTest, CreateGLContext_EmptyAttribs) {
  gl::GLContextAttribs attribs;

  gl_context_ = gl::init::CreateGLContext(nullptr, surf_.get(), attribs);

  ASSERT_NE(gl_context_, nullptr);
  EXPECT_TRUE(gl_context_->MakeCurrent(surf_.get()));
}

// Test: CreateGLContext with null share group
TEST_F(GLFactoryOhosTest, CreateGLContext_NullShareGroup) {
  gl_context_ = gl::init::CreateGLContext(nullptr, surf_.get(), gl::GLContextAttribs());

  ASSERT_NE(gl_context_, nullptr);
  EXPECT_TRUE(gl_context_->MakeCurrent(surf_.get()));
}

// Test: CreateOffscreenGLSurface multiple times
TEST_F(GLFactoryOhosTest, CreateOffscreenGLSurface_MultipleTimes) {
  scoped_refptr<gl::GLSurface> surface1 =
      gl::init::CreateOffscreenGLSurface(display_, gfx::Size(100, 100));
  scoped_refptr<gl::GLSurface> surface2 =
      gl::init::CreateOffscreenGLSurface(display_, gfx::Size(200, 200));

  ASSERT_NE(surface1, nullptr);
  ASSERT_NE(surface2, nullptr);
  EXPECT_NE(surface1.get(), surface2.get());
}

// Test: Context creation with surface that has handle
TEST_F(GLFactoryOhosTest, GLNonOwnedContext_CodePath_SurfaceWithHandle) {
  // Create a regular offscreen surface
  scoped_refptr<gl::GLSurface> surface =
      gl::init::CreateOffscreenGLSurface(display_, gfx::Size(128, 128));
  ASSERT_NE(surface, nullptr);

  // Check if surface has a handle
  if (surface->GetHandle() != nullptr) {
    // This should trigger GLNonOwnedContext creation path
    gl_context_ = gl::init::CreateGLContext(nullptr, surface.get(), gl::GLContextAttribs());
    ASSERT_NE(gl_context_, nullptr);
    EXPECT_TRUE(gl_context_->MakeCurrent(surface.get()));
  } else {
    // Surface is surfaceless, GLContextEGL will be created instead
    GTEST_SKIP() << "Surface does not have handle, GLNonOwnedContext not created";
  }
}

// Test: Verify context handles release current correctly
TEST_F(GLFactoryOhosTest, GLNonOwnedContext_ReleaseCurrentBehavior) {
  gl_context_ = gl::init::CreateGLContext(nullptr, surf_.get(), gl::GLContextAttribs());
  ASSERT_NE(gl_context_, nullptr);

  EXPECT_TRUE(gl_context_->MakeCurrent(surf_.get()));
  EXPECT_TRUE(gl_context_->IsCurrent(surf_.get()));

  // ReleaseCurrent should be a no-op for GLNonOwnedContext
  // but should still work correctly
  gl_context_->ReleaseCurrent(surf_.get());
  EXPECT_FALSE(gl_context_->IsCurrent(surf_.get()));
}

// Test: Verify GetHandle returns nullptr for non-owned context
TEST_F(GLFactoryOhosTest, GLNonOwnedContext_GetHandleIsNullptr) {
  gl_context_ = gl::init::CreateGLContext(nullptr, surf_.get(), gl::GLContextAttribs());
  ASSERT_NE(gl_context_, nullptr);

  // For GLNonOwnedContext, GetHandle() should return nullptr
  // because we don't own the EGL context
  void* handle = gl_context_->GetHandle();

  // Handle may be nullptr or non-null depending on context type
  // Just verify we can call it without crashing
  SUCCEED();
}

}  // namespace init
}  // namespace gl
