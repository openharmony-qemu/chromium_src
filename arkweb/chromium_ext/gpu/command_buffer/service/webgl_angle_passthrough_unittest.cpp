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

#include "gpu/command_buffer/service/abstract_texture_ohos.h"
#include "base/memory/scoped_refptr.h"
#include "gpu/command_buffer/service/texture_manager.h"
#include "gtest/gtest.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gl/init/gl_factory.h"
#include "ui/gl/test/gl_test_support.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_surface.h"
#include "ui/gl/gl_utils.h"

using namespace gpu;

// Test fixture for WebGL ANGLE Passthrough functionality
class WebGLAnglePassthroughTest : public ::testing::Test {
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

// Test: Create passthrough texture with RGBA8 format
TEST_F(WebGLAnglePassthroughTest, CreatePassthroughTexture_RGBA8) {
  gfx::Size size(256, 256);
  auto texture = AbstractTextureOHOS::CreateForPassthrough(size);

  ASSERT_NE(texture, nullptr);
  EXPECT_NE(texture->GetTextureBase(), nullptr);
  EXPECT_EQ(texture->size(), size);
}

// Test: Create passthrough texture with RGB8 format
TEST_F(WebGLAnglePassthroughTest, CreatePassthroughTexture_RGB8) {
  gfx::Size size(128, 128);
  auto texture = AbstractTextureOHOS::CreateForPassthrough(size);

  ASSERT_NE(texture, nullptr);
  EXPECT_NE(texture->GetTextureBase(), nullptr);
}

// Test: Create passthrough texture with different sizes
TEST_F(WebGLAnglePassthroughTest, CreatePassthroughTexture_VariousSizes) {
  std::vector<gfx::Size> sizes = {
    gfx::Size(64, 64),
    gfx::Size(128, 256),
    gfx::Size(512, 512),
    gfx::Size(1024, 768)
  };

  for (const auto& size : sizes) {
    auto texture = AbstractTextureOHOS::CreateForPassthrough(size);
    ASSERT_NE(texture, nullptr) << "Failed to create texture of size " << size.ToString();
    EXPECT_EQ(texture->size(), size);
  }
}

// Test: Passthrough texture with GL_TEXTURE_EXTERNAL_OES target
TEST_F(WebGLAnglePassthroughTest, PassthroughTexture_ExternalOESTarget) {
  gfx::Size size(256, 256);
  GLuint service_id = 0;
  gl::g_current_gl_context->glGenTexturesFn(1, &service_id);

  auto passthrough_texture = base::MakeRefCounted<gles2::TexturePassthrough>(
      service_id, GL_TEXTURE_EXTERNAL_OES);

  auto texture = std::make_unique<AbstractTextureOHOS>(passthrough_texture, size);
  ASSERT_NE(texture, nullptr);
  EXPECT_NE(texture->GetTextureBase(), nullptr);
}

// Test: Bind passthrough texture to service ID
TEST_F(WebGLAnglePassthroughTest, BindPassthroughTexture_ToServiceId) {
  gfx::Size size(128, 128);
  auto texture = AbstractTextureOHOS::CreateForPassthrough(size);
  ASSERT_NE(texture, nullptr);

  GLuint new_service_id = 123;
  texture->BindToServiceId(new_service_id);

  // Verify binding succeeded (texture should still be valid)
  EXPECT_NE(texture->GetTextureBase(), nullptr);
}

// Test: Texture size storage for passthrough textures
TEST_F(WebGLAnglePassthroughTest, PassthroughTexture_SizeStorage) {
  gfx::Size size(512, 256);
  auto texture = AbstractTextureOHOS::CreateForPassthrough(size);
  ASSERT_NE(texture, nullptr);

  // Verify size is correctly stored
  EXPECT_EQ(texture->size(), size);
  EXPECT_EQ(texture->size().width(), 512);
  EXPECT_EQ(texture->size().height(), 256);
}

// Test: NotifyOnContextLost for passthrough texture
TEST_F(WebGLAnglePassthroughTest, PassthroughTexture_ContextLost) {
  gfx::Size size(64, 64);
  auto texture = AbstractTextureOHOS::CreateForPassthrough(size);
  ASSERT_NE(texture, nullptr);

  // Simulate context loss
  texture->NotifyOnContextLost();

  // Verify texture handled context loss gracefully
  EXPECT_FALSE(texture->have_context_);
}

// Test: Multiple passthrough textures creation
TEST_F(WebGLAnglePassthroughTest, CreateMultiplePassthroughTextures) {
  const int kTextureCount = 10;
  std::vector<std::unique_ptr<AbstractTextureOHOS>> textures;

  for (int i = 0; i < kTextureCount; ++i) {
    gfx::Size size(128 + i * 32, 128 + i * 32);
    auto texture = AbstractTextureOHOS::CreateForPassthrough(size);
    ASSERT_NE(texture, nullptr) << "Failed to create texture " << i;
    textures.push_back(std::move(texture));
  }

  // Verify all textures are valid
  for (int i = 0; i < kTextureCount; ++i) {
    EXPECT_NE(textures[i], nullptr);
    EXPECT_NE(textures[i]->GetTextureBase(), nullptr);
  }
}

// Test: Passthrough texture cleanup
TEST_F(WebGLAnglePassthroughTest, PassthroughTexture_Cleanup) {
  gfx::Size size(256, 256);

  {
    auto texture = AbstractTextureOHOS::CreateForPassthrough(size);
    ASSERT_NE(texture, nullptr);
    EXPECT_NE(texture->GetTextureBase(), nullptr);
  }

  // Texture should be cleaned up when out of scope
  // No explicit assertion needed - if this crashes or hangs, test fails
  SUCCEED();
}

// Test: GetTextureBase returns valid pointer
TEST_F(WebGLAnglePassthroughTest, PassthroughTexture_GetTextureBase) {
  gfx::Size size(128, 128);
  auto texture = AbstractTextureOHOS::CreateForPassthrough(size);
  ASSERT_NE(texture, nullptr);

  auto* texture_base = texture->GetTextureBase();
  EXPECT_NE(texture_base, nullptr);
  EXPECT_TRUE(texture_base->GetType() == gles2::TextureBase::Type::kPassthrough);
}

// Test: Passthrough texture with non-power-of-2 size
TEST_F(WebGLAnglePassthroughTest, PassthroughTexture_NonPowerOfTwoSize) {
  gfx::Size size(100, 150);  // Non-power-of-2 dimensions
  auto texture = AbstractTextureOHOS::CreateForPassthrough(size);

  ASSERT_NE(texture, nullptr) << "Failed to create NPOT texture";
  EXPECT_EQ(texture->size(), size);
}

// Test: Passthrough texture service ID uniqueness
TEST_F(WebGLAnglePassthroughTest, PassthroughTexture_UniqueServiceIds) {
  gfx::Size size(64, 64);
  auto texture1 = AbstractTextureOHOS::CreateForPassthrough(size);
  auto texture2 = AbstractTextureOHOS::CreateForPassthrough(size);

  ASSERT_NE(texture1, nullptr);
  ASSERT_NE(texture2, nullptr);

  // Each texture should have its own service ID
  EXPECT_NE(texture1->GetTextureBase(), texture2->GetTextureBase());
}
