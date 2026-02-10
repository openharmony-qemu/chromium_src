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
#include "gpu/command_buffer/service/texture_manager.h"
#include "base/memory/scoped_refptr.h"
#include "gtest/gtest.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gl/init/gl_factory.h"
#include "ui/gl/test/gl_test_support.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_surface.h"
#include "ui/gl/gl_utils.h"
#include "ui/gl/gl_bindings.h"

using namespace gpu;

namespace {

struct ColorRGBA {
  uint8_t r, g, b, a;
  constexpr ColorRGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
      : r(red), g(green), b(blue), a(alpha) {}

  bool Equals(const ColorRGBA& other, int tolerance = 5) const {
    return std::abs(r - other.r) <= tolerance &&
           std::abs(g - other.g) <= tolerance &&
           std::abs(b - other.b) <= tolerance;
  }
};

constexpr ColorRGBA COLOR_RED(255, 0, 0, 255);
constexpr ColorRGBA COLOR_GREEN(0, 255, 0, 255);
constexpr ColorRGBA COLOR_BLUE(0, 0, 255, 255);
constexpr ColorRGBA COLOR_WHITE(255, 255, 255, 255);
constexpr ColorRGBA COLOR_BLACK(0, 0, 0, 255);
constexpr ColorRGBA COLOR_GRAY(128, 128, 128, 255);

}  // namespace

class TexturePixelReadWriteTest : public ::testing::Test {
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
    ASSERT_TRUE(gl_context_->MakeCurrent(surf_.get()));
  }

  void TearDown() override {
    if (gl_context_ && surf_) {
      gl_context_->ReleaseCurrent(surf_.get());
      CleanupResources();
    }
  }

  void CleanupResources() {
    gl_context_->MakeCurrent(surf_.get());
    for (GLuint tex : textures_) gl::g_current_gl_context->glDeleteTexturesFn(1, &tex);
    for (GLuint fbo : framebuffers_) gl::g_current_gl_context->glDeleteFramebuffersFn(1, &fbo);
    for (GLuint buf : buffers_) gl::g_current_gl_context->glDeleteBuffersFn(1, &buf);
    for (GLuint prog : programs_) gl::g_current_gl_context->glDeleteProgramFn(prog);
    for (GLuint shader : shaders_) gl::g_current_gl_context->glDeleteShaderFn(shader);
    textures_.clear();
    framebuffers_.clear();
    buffers_.clear();
    programs_.clear();
    shaders_.clear();
  }

 protected:
  GLuint CreateTexture() {
    GLuint tex;
    gl::g_current_gl_context->glGenTexturesFn(1, &tex);
    textures_.push_back(tex);
    return tex;
  }

  GLuint CreateFramebuffer() {
    GLuint fbo;
    gl::g_current_gl_context->glGenFramebuffersFn(1, &fbo);
    framebuffers_.push_back(fbo);
    return fbo;
  }

  GLuint CreateBuffer() {
    GLuint buf;
    gl::g_current_gl_context->glGenBuffersFn(1, &buf);
    buffers_.push_back(buf);
    return buf;
  }

  void CreateTexture2D(GLuint tex, GLsizei w, GLsizei h, GLenum format = GL_RGBA) {
    gl::g_current_gl_context->glBindTextureFn(GL_TEXTURE_2D, tex);
    gl::g_current_gl_context->glTexImage2DFn(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, nullptr);
    gl::g_current_gl_context->glTexParameteriFn(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl::g_current_gl_context->glTexParameteriFn(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

  void CreateTexture2DWithData(GLuint tex, GLsizei w, GLsizei h, const ColorRGBA& c, GLenum format = GL_RGBA) {
    gl::g_current_gl_context->glBindTextureFn(GL_TEXTURE_2D, tex);
    std::vector<ColorRGBA> data(w * h, c);
    gl::g_current_gl_context->glTexImage2DFn(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data.data());
    gl::g_current_gl_context->glTexParameteriFn(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl::g_current_gl_context->glTexParameteriFn(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

  ColorRGBA ReadPixel(int x, int y) {
    ColorRGBA pixel;
    gl::g_current_gl_context->glReadPixelsFn(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
    return pixel;
  }

  void SetClearColor(const ColorRGBA& c) {
    gl::g_current_gl_context->glClearColorFn(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f);
  }

  GLuint CreateProgram(const char* vs, const char* fs) {
    GLuint vsh = gl::g_current_gl_context->glCreateShaderFn(GL_VERTEX_SHADER);
    gl::g_current_gl_context->glShaderSourceFn(vsh, 1, &vs, nullptr);
    gl::g_current_gl_context->glCompileShaderFn(vsh);
    shaders_.push_back(vsh);

    GLuint fsh = gl::g_current_gl_context->glCreateShaderFn(GL_FRAGMENT_SHADER);
    gl::g_current_gl_context->glShaderSourceFn(fsh, 1, &fs, nullptr);
    gl::g_current_gl_context->glCompileShaderFn(fsh);
    shaders_.push_back(fsh);

    GLuint prog = gl::g_current_gl_context->glCreateProgramFn();
    gl::g_current_gl_context->glAttachShaderFn(prog, vsh);
    gl::g_current_gl_context->glAttachShaderFn(prog, fsh);
    gl::g_current_gl_context->glLinkProgramFn(prog);
    programs_.push_back(prog);
    return prog;
  }

  void SetupQuad(GLuint prog, GLuint buf) {
    gl::g_current_gl_context->glBindBufferFn(GL_ARRAY_BUFFER, buf);
    float v[] = {-1,-1, 1,-1, -1,1, -1,1, 1,-1, 1,1};
    gl::g_current_gl_context->glBufferDataFn(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    GLint loc = gl::g_current_gl_context->glGetAttribLocationFn(prog, "aPos");
    gl::g_current_gl_context->glEnableVertexAttribArrayFn(loc);
    gl::g_current_gl_context->glVertexAttribPointerFn(loc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  }

  scoped_refptr<gl::GLSurface> surf_;
  scoped_refptr<gl::GLContext> gl_context_;
  std::vector<GLuint> textures_, framebuffers_, buffers_, programs_, shaders_;
};

// Test: RGBA8 texture upload and read
TEST_F(TexturePixelReadWriteTest, Category1_RGBA8_TextureUploadAndRead) {
  const GLsizei size = 64;
  GLuint tex = CreateTexture();
  CreateTexture2DWithData(tex, size, size, COLOR_RED);

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
  ASSERT_EQ(gl::g_current_gl_context->glCheckFramebufferStatusFn(GL_FRAMEBUFFER), GL_FRAMEBUFFER_COMPLETE);

  gl::g_current_gl_context->glViewportFn(0, 0, size, size);
  ColorRGBA pixel = ReadPixel(32, 32);
  EXPECT_TRUE(pixel.Equals(COLOR_RED, 10)) << "RGBA8 texture upload and read failed";
}

// Test: RGB8 texture format compatibility
TEST_F(TexturePixelReadWriteTest, Category1_RGB8_TextureFormatCompatibility) {
  const GLsizei size = 32;
  GLuint tex = CreateTexture();

  gl::g_current_gl_context->glBindTextureFn(GL_TEXTURE_2D, tex);
  std::vector<ColorRGB> data(size * size, {COLOR_RED.r, COLOR_RED.g, COLOR_RED.b});
  gl::g_current_gl_context->glTexImage2DFn(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0,
                                           GL_RGB, GL_UNSIGNED_BYTE, data.data());
  gl::g_current_gl_context->glTexParameteriFn(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

  GLenum status = gl::g_current_gl_context->glCheckFramebufferStatusFn(GL_FRAMEBUFFER);
  if (status == GL_FRAMEBUFFER_COMPLETE) {
    ColorRGBA pixel = ReadPixel(16, 16);
    EXPECT_TRUE(pixel.Equals(COLOR_RED, 10)) << "RGB8 format compatibility failed";
  } else {
    GTEST_SKIP() << "RGB8 not supported as FBO attachment";
  }
}

// Test: Texture parameter setup validation
TEST_F(TexturePixelReadWriteTest, Category1_TextureParameterValidation) {
  GLuint tex = CreateTexture();
  gl::g_current_gl_context->glBindTextureFn(GL_TEXTURE_2D, tex);
  gl::g_current_gl_context->glTexImage2DFn(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

  gl::g_current_gl_context->glTexParameteriFn(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  gl::g_current_gl_context->glTexParameteriFn(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gl::g_current_gl_context->glTexParameteriFn(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  gl::g_current_gl_context->glTexParameteriFn(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  GLint min_filter = 0;
  GLint mag_filter = 0;
  GLint wrap_s = 0;
  GLint wrap_t = 0;
  gl::g_current_gl_context->glGetTexParameterivFn(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &min_filter);
  gl::g_current_gl_context->glGetTexParameterivFn(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &mag_filter);
  gl::g_current_gl_context->glGetTexParameterivFn(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrap_s);
  gl::g_current_gl_context->glGetTexParameterivFn(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrap_t);

  EXPECT_EQ(min_filter, GL_LINEAR);
  EXPECT_EQ(mag_filter, GL_LINEAR);
  EXPECT_EQ(wrap_s, GL_CLAMP_TO_EDGE);
  EXPECT_EQ(wrap_t, GL_CLAMP_TO_EDGE);
}

// Test: Large scale texture upload performance
TEST_F(TexturePixelReadWriteTest, Category2_LargeScaleTextureUpload) {
  const GLsizei size = 512;
  GLuint tex = CreateTexture();

  auto start = std::chrono::high_resolution_clock::now();
  CreateTexture2D(tex, size, size);
  auto end = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  EXPECT_LT(duration, 100) << "Large texture upload took too long: " << duration << "ms";

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
  EXPECT_EQ(gl::g_current_gl_context->glCheckFramebufferStatusFn(GL_FRAMEBUFFER), GL_FRAMEBUFFER_COMPLETE);
}

// Test: Frequent ReadPixels performance
TEST_F(TexturePixelReadWriteTest, Category2_FrequentReadPixelsPerformance) {
  const GLsizei size = 64;
  GLuint tex = CreateTexture();
  CreateTexture2DWithData(tex, size, size, COLOR_BLUE);

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
  gl::g_current_gl_context->glViewportFn(0, 0, size, size);

  const int iterations = 100;
  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < iterations; i++) {
    ColorRGBA pixel = ReadPixel(32, 32);
    EXPECT_TRUE(pixel.Equals(COLOR_BLUE, 5));
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  EXPECT_LT(duration, 50) << "Frequent ReadPixels took too long: " << duration << "ms for " << iterations << " reads";
}

// Test: TexSubImage2D incremental update
TEST_F(TexturePixelReadWriteTest, Category2_TexSubImage2D_IncrementalUpdate) {
  const GLsizei size = 64;
  GLuint tex = CreateTexture();
  CreateTexture2D(tex, size, size);

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
  gl::g_current_gl_context->glViewportFn(0, 0, size, size);

  ColorRGBA colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE};
  for (const auto& color : colors) {
    std::vector<ColorRGBA> data(32 * 32, color);
    gl::g_current_gl_context->glTexSubImage2DFn(GL_TEXTURE_2D, 0, 16, 16, 32, 32,
                                                GL_RGBA, GL_UNSIGNED_BYTE, data.data());

    ColorRGBA pixel = ReadPixel(32, 32);
    EXPECT_TRUE(pixel.Equals(color, 10)) << "TexSubImage2D update failed";
  }
}

// Test: Mipmap generation and sampling
TEST_F(TexturePixelReadWriteTest, Category3_MipmapGenerationAndSampling) {
  const GLsizei size = 64;
  GLuint tex = CreateTexture();

  gl::g_current_gl_context->glBindTextureFn(GL_TEXTURE_2D, tex);
  std::vector<ColorRGBA> data(size * size, COLOR_RED);
  gl::g_current_gl_context->glTexImage2DFn(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0,
                                           GL_RGBA, GL_UNSIGNED_BYTE, data.data());

  gl::g_current_gl_context->glGenerateMipmapFn(GL_TEXTURE_2D);
  gl::g_current_gl_context->glTexParameteriFn(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  gl::g_current_gl_context->glTexParameteriFn(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
  gl::g_current_gl_context->glViewportFn(0, 0, size, size);

  ColorRGBA pixel = ReadPixel(32, 32);
  EXPECT_TRUE(pixel.Equals(COLOR_RED, 20)) << "Mipmap texture sampling failed";
}

// Test: Render to texture (RTT)
TEST_F(TexturePixelReadWriteTest, Category3_RenderToTexture) {
  const GLsizei size = 64;

  GLuint tex = CreateTexture();
  CreateTexture2D(tex, size, size);

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
  ASSERT_EQ(gl::g_current_gl_context->glCheckFramebufferStatusFn(GL_FRAMEBUFFER), GL_FRAMEBUFFER_COMPLETE);

  gl::g_current_gl_context->glViewportFn(0, 0, size, size);
  SetClearColor(COLOR_GREEN);
  gl::g_current_gl_context->glClearFn(GL_COLOR_BUFFER_BIT);

  GLuint prog = CreateProgram(
      "attribute vec2 aPos; void main() { gl_Position = vec4(aPos, 0, 1); }",
      "precision mediump float; void main() { gl_FragColor = vec4(0, 1, 0, 1); }");
  gl::g_current_gl_context->glUseProgramFn(prog);

  GLuint buf = CreateBuffer();
  SetupQuad(prog, buf);
  gl::g_current_gl_context->glDrawArraysFn(GL_TRIANGLES, 0, 6);

  ColorRGBA pixel = ReadPixel(32, 32);
  EXPECT_TRUE(pixel.Equals(COLOR_GREEN, 10)) << "Render to texture failed";
}

// Test: WebGL2 texture storage
TEST_F(TexturePixelReadWriteTest, Category3_WebGL2_TextureStorage) {
  if (!gl::g_current_gl_context->glTexStorage2DFn) {
    GTEST_SKIP() << "glTexStorage2D not supported";
    return;
  }

  const GLsizei size = 32;
  GLuint tex = CreateTexture();

  gl::g_current_gl_context->glBindTextureFn(GL_TEXTURE_2D, tex);
  gl::g_current_gl_context->glTexStorage2DFn(GL_TEXTURE_2D, 1, GL_RGBA8, size, size);

  std::vector<ColorRGBA> data(size * size, COLOR_BLUE);
  gl::g_current_gl_context->glTexSubImage2DFn(GL_TEXTURE_2D, 0, 0, 0, size, size,
                                              GL_RGBA, GL_UNSIGNED_BYTE, data.data());

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

  ColorRGBA pixel = ReadPixel(16, 16);
  EXPECT_TRUE(pixel.Equals(COLOR_BLUE, 10)) << "WebGL2 texture storage failed";
}

// Test: Fast path passthrough texture creation
TEST_F(TexturePixelReadWriteTest, Category4_PassthroughTextureCreation) {
  gfx::Size size(256, 256);
  auto texture = AbstractTextureOHOS::CreateForPassthrough(size);

  ASSERT_NE(texture, nullptr);
  EXPECT_NE(texture->GetTextureBase(), nullptr);
  EXPECT_EQ(texture->size(), size);
}

// Test: Multiple passthrough texture stress test
TEST_F(TexturePixelReadWriteTest, Category4_MultiplePassthroughTextures) {
  const int count = 20;
  std::vector<std::unique_ptr<AbstractTextureOHOS>> textures;

  for (int i = 0; i < count; i++) {
    gfx::Size size(64 + i * 8, 64 + i * 8);
    auto texture = AbstractTextureOHOS::CreateForPassthrough(size);
    ASSERT_NE(texture, nullptr) << "Failed to create texture " << i;
    textures.push_back(std::move(texture));
  }

  for (int i = 0; i < count; i++) {
    EXPECT_NE(textures[i], nullptr);
    EXPECT_NE(textures[i]->GetTextureBase(), nullptr);
    EXPECT_EQ(textures[i]->size().width(), 64 + i * 8);
  }
}

// Test: Passthrough texture with context loss
TEST_F(TexturePixelReadWriteTest, Category4_PassthroughContextLoss) {
  gfx::Size size(128, 128);
  auto texture = AbstractTextureOHOS::CreateForPassthrough(size);

  ASSERT_NE(texture, nullptr);
  EXPECT_TRUE(texture->have_context_);

  texture->NotifyOnContextLost();
  EXPECT_FALSE(texture->have_context_);
}

// Test: Texture read/write consistency check
TEST_F(TexturePixelReadWriteTest, Category4_ReadWriteConsistency) {
  const GLsizei size = 128;
  GLuint tex = CreateTexture();
  CreateTexture2D(tex, size, size);

  std::vector<ColorRGBA> original_data(size * size);
  for (int i = 0; i < size * size; i++) {
    original_data[i] = ColorRGBA(i % 256, (i * 2) % 256, (i * 3) % 256, 255);
  }

  gl::g_current_gl_context->glBindTextureFn(GL_TEXTURE_2D, tex);
  gl::g_current_gl_context->glTexSubImage2DFn(GL_TEXTURE_2D, 0, 0, 0, size, size,
                                              GL_RGBA, GL_UNSIGNED_BYTE, original_data.data());

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
  gl::g_current_gl_context->glViewportFn(0, 0, size, size);

  std::vector<ColorRGBA> read_data(size * size);
  gl::g_current_gl_context->glReadPixelsFn(0, 0, size, size, GL_RGBA, GL_UNSIGNED_BYTE, read_data.data());

  int match_count = 0;
  for (int i = 0; i < size * size; i++) {
    if (original_data[i].Equals(read_data[i], 5)) {
      match_count++;
    }
  }

  float match_ratio = static_cast<float>(match_count) / (size * size);
  EXPECT_GT(match_ratio, 0.99f) << "Read/write consistency check failed: only " 
                                << (match_ratio * 100) << "% pixels match";
}
