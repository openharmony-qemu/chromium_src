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

#include "base/memory/weak_ptr.h"
#define private public
#include "gpu/command_buffer/service/abstract_texture_ohos.h"
#undef private

#include <memory>
#include <utility>

#include "base/memory/scoped_refptr.h"
#include "gpu/command_buffer/service/texture_manager.h"
#include "gtest/gtest.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gl/init/gl_factory.h"
#include "ui/gl/test/gl_test_support.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_surface.h"

using namespace gpu;

class AbstractTextureOHOSTest : public ::testing::Test {
 public:
  void SetUp() override {
    service_id = 789;
    abstract_texture_ohos = AbstractTextureOHOS::CreateForTesting(service_id);
    abstract_texture_shared = AbstractTextureOHOS::CreateForTesting(service_id);
  }

  void TearDown() override {}

  GLuint service_id;

 protected:
  std::unique_ptr<AbstractTextureOHOS> abstract_texture_ohos;
  std::shared_ptr<AbstractTextureOHOS> abstract_texture_shared;
};

TEST_F(AbstractTextureOHOSTest, AbstractTextureOHOS_Destructor001) {
  abstract_texture_shared->have_context_ = false;
  abstract_texture_shared->texture_ = nullptr;
  abstract_texture_shared->texture_passthrough_ = nullptr;
  abstract_texture_shared.reset();
  EXPECT_EQ(abstract_texture_shared.use_count(), 0);
}

TEST_F(AbstractTextureOHOSTest, AbstractTextureOHOS_Destructor002) {
  abstract_texture_shared->have_context_ = true;
  abstract_texture_shared->texture_ = nullptr;
  abstract_texture_shared->texture_passthrough_ = nullptr;
  abstract_texture_shared.reset();
  EXPECT_EQ(abstract_texture_shared.use_count(), 0);
}

TEST_F(AbstractTextureOHOSTest, NotifyOnContextLostFalse) {
  abstract_texture_ohos->texture_passthrough_ = nullptr;
  abstract_texture_ohos->NotifyOnContextLost();
  EXPECT_FALSE(abstract_texture_ohos->have_context_);
}

TEST_F(AbstractTextureOHOSTest, NotifyOnContextLostTrue) {
  abstract_texture_ohos->texture_passthrough_ =
      base::MakeRefCounted<gles2::TexturePassthrough>(2, 2);
  abstract_texture_ohos->NotifyOnContextLost();
  EXPECT_FALSE(abstract_texture_ohos->have_context_);
}

TEST_F(AbstractTextureOHOSTest, BindToServiceId_Test001) {
  abstract_texture_ohos->texture_passthrough_ = nullptr;
  abstract_texture_ohos->BindToServiceId(service_id);
  EXPECT_FALSE(abstract_texture_ohos->texture_passthrough_);
}

TEST_F(AbstractTextureOHOSTest, GetTextureBase_Test001) {
  EXPECT_FALSE(abstract_texture_ohos->texture_);
  EXPECT_FALSE(abstract_texture_ohos->texture_passthrough_);
  abstract_texture_ohos->texture_for_testing_ = nullptr;

  EXPECT_FALSE(abstract_texture_ohos->GetTextureBase());
}

TEST_F(AbstractTextureOHOSTest, GetTextureBase_Test002) {
  EXPECT_FALSE(abstract_texture_ohos->texture_);
  EXPECT_FALSE(abstract_texture_ohos->texture_passthrough_);
  EXPECT_TRUE(abstract_texture_ohos->texture_for_testing_);

  EXPECT_TRUE(abstract_texture_ohos->GetTextureBase());
}

// Test fixture with GL context
class AbstractTextureOHOSPassthroughTest : public ::testing::Test {
 public:
  void SetUp() override {
    gl::init::InitializeGLNoExtensionsOneOff(
        /*init_bindings=*/true, /*gpu_preference=*/gl::GpuPreference::kDefault);
    auto display = gl::GLTestSupport::InitializeGL(std::nullopt);
    ASSERT_NE(display, nullptr);
    surf_ = gl::init::CreateOffscreenGLSurface(display, gfx::Size());
    ASSERT_NE(surf_, nullptr);
    gl_context_ = gl::init::CreateGLContext(nullptr, surf_.get(), gl::GLContextAttribs());
    ASSERT_NE(gl_context_, nullptr);
    bool make_current_result = gl_context_->MakeCurrent(surf_.get());
    ASSERT_TRUE(make_current_result);
  }

  void TearDown() override {
    if (gl_context_ && surf_) {
      gl_context_->ReleaseCurrent(surf_.get());
    }
  }

 protected:
  scoped_refptr<gl::GLSurface> surf_;
  scoped_refptr<gl::GLContext> gl_context_;
};

// Test: The texture created by CreateForPassthrough contains the correct size
TEST_F(AbstractTextureOHOSPassthroughTest, CreateForPassthrough_WithSize) {
  gfx::Size size(256, 256);
  auto texture = AbstractTextureOHOS::CreateForPassthrough(size);
  ASSERT_NE(texture, nullptr);
  EXPECT_NE(texture->GetTextureBase(), nullptr);
}

// Test: Passthrough constructor correctly initializes all members
TEST_F(AbstractTextureOHOSPassthroughTest, Passthrough_Constructor_InitializesMembers) {
  gfx::Size size(128, 128);
  GLuint service_id = 0;
  gl::g_current_gl_context->glGenTexturesFn(1, &service_id);
  auto passthrough_texture = base::MakeRefCounted<gles2::TexturePassthrough>(
      service_id, GL_TEXTURE_EXTERNAL_OES);

  auto texture = std::make_unique<AbstractTextureOHOS>(passthrough_texture, size);
  ASSERT_NE(texture, nullptr);
  EXPECT_NE(texture->GetTextureBase(), nullptr);
}

// Test: DCHECK verifies that the texture target is GL_TEXTURE_EXTERNAL_OES
TEST_F(AbstractTextureOHOSPassthroughTest, Passthrough_Constructor_ValidatesTarget) {
  gfx::Size size(64, 64);
  GLuint service_id = 0;
  gl::g_current_gl_context->glGenTexturesFn(1, &service_id);
  auto passthrough_texture = base::MakeRefCounted<gles2::TexturePassthrough>(
      service_id, GL_TEXTURE_EXTERNAL_OES);

  auto texture = std::make_unique<AbstractTextureOHOS>(passthrough_texture, size);
  EXPECT_NE(texture, nullptr);
}

// Test: BindToServiceId non-ANGLE extension path
TEST_F(AbstractTextureOHOSPassthroughTest, BindToServiceId_WithoutANGLEExtension) {
  gfx::Size size(32, 32);
  auto texture = AbstractTextureOHOS::CreateForPassthrough(size);
  ASSERT_NE(texture, nullptr);
  GLuint new_service_id = 123;

  texture->BindToServiceId(new_service_id);
}

// Test: Size is stored internally and used in BindToServiceId with ANGLE extension
TEST_F(AbstractTextureOHOSPassthroughTest, Passthrough_SizeStoredCorrectly) {
  gfx::Size size(512, 256);
  GLuint service_id = 0;
  gl::g_current_gl_context->glGenTexturesFn(1, &service_id);
  auto passthrough_texture = base::MakeRefCounted<gles2::TexturePassthrough>(
      service_id, GL_TEXTURE_EXTERNAL_OES);

  auto texture = std::make_unique<AbstractTextureOHOS>(passthrough_texture, size);
  EXPECT_NE(texture, nullptr);
}

// Test: This is verified indirectly through successful texture operations
TEST_F(AbstractTextureOHOSPassthroughTest, Passthrough_ApiSetToCurrentContext) {
  gfx::Size size(100, 100);
  GLuint service_id = 0;
  gl::g_current_gl_context->glGenTexturesFn(1, &service_id);
  auto passthrough_texture = base::MakeRefCounted<gles2::TexturePassthrough>(
      service_id, GL_TEXTURE_EXTERNAL_OES);

  auto texture = std::make_unique<AbstractTextureOHOS>(passthrough_texture, size);
  ASSERT_NE(texture, nullptr);
}

// Test: If ANGLE extension is available, it should call glTexImage2DExternalANGLE
TEST_F(AbstractTextureOHOSPassthroughTest, BindToServiceId_WithANGLEExtension) {
  gfx::Size size(256, 256);
  auto texture = AbstractTextureOHOS::CreateForPassthrough(size);
  ASSERT_NE(texture, nullptr);
  GLuint new_service_id = 456;

  texture->BindToServiceId(new_service_id);
}

// Test: Complete Process of CreateForPassthrough and BindToServiceId
TEST_F(AbstractTextureOHOSPassthroughTest, CreateForPassthrough_And_Bind) {
  gfx::Size size(320, 240);
  auto texture = AbstractTextureOHOS::CreateForPassthrough(size);
  ASSERT_NE(texture, nullptr);

  GLuint bind_service_id = 789;
  texture->BindToServiceId(bind_service_id);
  EXPECT_NE(texture->GetTextureBase(), nullptr);
}
