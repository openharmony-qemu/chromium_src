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
constexpr ColorRGBA COLOR_BLACK(0, 0, 0, 255);
constexpr ColorRGBA COLOR_YELLOW(255, 255, 0, 255);
constexpr ColorRGBA COLOR_CYAN(0, 255, 255, 255);

}  // namespace

class AngleBackendSyncTest : public ::testing::Test {
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
    textures_.clear(); framebuffers_.clear(); buffers_.clear(); programs_.clear(); shaders_.clear();
  }

 protected:
  GLuint CreateTexture() {
    GLuint tex; gl::g_current_gl_context->glGenTexturesFn(1, &tex);
    textures_.push_back(tex); return tex;
  }

  GLuint CreateFramebuffer() {
    GLuint fbo; gl::g_current_gl_context->glGenFramebuffersFn(1, &fbo);
    framebuffers_.push_back(fbo); return fbo;
  }

  GLuint CreateBuffer() {
    GLuint buf; gl::g_current_gl_context->glGenBuffersFn(1, &buf);
    buffers_.push_back(buf); return buf;
  }

  void CreateTexture2D(GLuint tex, GLsizei w, GLsizei h) {
    gl::g_current_gl_context->glBindTextureFn(GL_TEXTURE_2D, tex);
    gl::g_current_gl_context->glTexImage2DFn(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    gl::g_current_gl_context->glTexParameteriFn(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl::g_current_gl_context->glTexParameteriFn(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

  void CreateTexture2DWithData(GLuint tex, GLsizei w, GLsizei h, const ColorRGBA& c) {
    gl::g_current_gl_context->glBindTextureFn(GL_TEXTURE_2D, tex);
    std::vector<ColorRGBA> data(w * h, c);
    gl::g_current_gl_context->glTexImage2DFn(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    gl::g_current_gl_context->glTexParameteriFn(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl::g_current_gl_context->glTexParameteriFn(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

  ColorRGBA ReadPixel(int x, int y) {
    ColorRGBA pixel;
    gl::g_current_gl_context->glReadPixelsFn(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
    return pixel;
  }

  void SetClearColor(const ColorRGBA& c) {
    gl::g_current_gl_context->glClearColorFn(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, 1.0f);
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

// Test: Texture upload -> FBO render -> ReadPixels
TEST_F(AngleBackendSyncTest, Scene1_TextureToFBOToReadPixels) {
  const GLsizei size = 64;
  GLuint tex = CreateTexture();
  CreateTexture2D(tex, size, size);

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
  ASSERT_EQ(gl::g_current_gl_context->glCheckFramebufferStatusFn(GL_FRAMEBUFFER), GL_FRAMEBUFFER_COMPLETE);

  gl::g_current_gl_context->glViewportFn(0, 0, size, size);
  SetClearColor(COLOR_BLUE);
  gl::g_current_gl_context->glClearFn(GL_COLOR_BUFFER_BIT);

  ColorRGBA pixel = ReadPixel(0, 0);
  EXPECT_TRUE(pixel.Equals(COLOR_BLUE, 10)) << "Texture->FBO->ReadPixels sync failed";
}

// Test: FBO switching with dual texture binding
TEST_F(AngleBackendSyncTest, Scene1_FBOSwitchingDualTexture) {
  const GLsizei size = 64;
  GLuint texA = CreateTexture(), texB = CreateTexture();
  CreateTexture2D(texA, size, size);
  CreateTexture2D(texB, size, size);

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glViewportFn(0, 0, size, size);

  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texA, 0);
  SetClearColor(COLOR_RED);
  gl::g_current_gl_context->glClearFn(GL_COLOR_BUFFER_BIT);

  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texB, 0);
  SetClearColor(COLOR_GREEN);
  gl::g_current_gl_context->glClearFn(GL_COLOR_BUFFER_BIT);

  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texA, 0);
  EXPECT_TRUE(ReadPixel(0, 0).Equals(COLOR_RED, 10)) << "texA should retain red after FBO switch";

  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texB, 0);
  EXPECT_TRUE(ReadPixel(0, 0).Equals(COLOR_GREEN, 10)) << "texB should be green";
}

// Test: TexSubImage2D incremental update sync
TEST_F(AngleBackendSyncTest, Scene1_TexSubImage2DIncrementalUpdate) {
  const GLsizei size = 64;
  GLuint tex = CreateTexture();
  CreateTexture2D(tex, size, size);

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

  ColorRGBA colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE};
  for (const auto& color : colors) {
    std::vector<ColorRGBA> data(32 * 32, color);
    gl::g_current_gl_context->glTexSubImage2DFn(GL_TEXTURE_2D, 0, 16, 16, 32, 32, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    EXPECT_TRUE(ReadPixel(32, 32).Equals(color, 10)) << "TexSubImage2D sync failed for color";
  }
}

// Test: Clear -> ReadPixels immediate sync
TEST_F(AngleBackendSyncTest, Scene2_ClearImmediateReadPixels) {
  const GLsizei size = 32;
  GLuint tex = CreateTexture();
  CreateTexture2D(tex, size, size);

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

  ColorRGBA colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE};
  for (const auto& color : colors) {
    SetClearColor(color);
    gl::g_current_gl_context->glClearFn(GL_COLOR_BUFFER_BIT);
    EXPECT_TRUE(ReadPixel(0, 0).Equals(color, 5)) << "Clear->ReadPixels immediate sync failed";
  }
}

// Test: DrawArrays -> ReadPixels immediate sync
TEST_F(AngleBackendSyncTest, Scene2_DrawArraysImmediateReadPixels) {
  const GLsizei size = 32;
  GLuint tex = CreateTexture();
  CreateTexture2D(tex, size, size);

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

  GLuint prog = CreateProgram(
      "attribute vec2 aPos; void main() { gl_Position = vec4(aPos, 0, 1); }",
      "precision mediump float; void main() { gl_FragColor = vec4(1, 0, 0, 1); }");
  gl::g_current_gl_context->glUseProgramFn(prog);

  GLuint buf = CreateBuffer();
  SetupQuad(prog, buf);

  gl::g_current_gl_context->glViewportFn(0, 0, size, size);
  SetClearColor(COLOR_BLACK);
  gl::g_current_gl_context->glClearFn(GL_COLOR_BUFFER_BIT);
  gl::g_current_gl_context->glDrawArraysFn(GL_TRIANGLES, 0, 6);

  EXPECT_TRUE(ReadPixel(0, 0).Equals(COLOR_RED, 10)) << "DrawArrays->ReadPixels sync failed";
}

// Test: Alternating read/write stress test
TEST_F(AngleBackendSyncTest, Scene2_AlternatingReadWriteStressTest) {
  const GLsizei size = 32;
  GLuint tex = CreateTexture();
  CreateTexture2D(tex, size, size);

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
  gl::g_current_gl_context->glViewportFn(0, 0, size, size);

  ColorRGBA colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE};
  for (int i = 0; i < 20; i++) {
    SetClearColor(colors[i % 3]);
    gl::g_current_gl_context->glClearFn(GL_COLOR_BUFFER_BIT);
    EXPECT_TRUE(ReadPixel(0, 0).Equals(colors[i % 3], 50)) << "Alternating stress test failed at iteration " << i;
  }
}

// Test: TexImage2D -> immediate sampling
TEST_F(AngleBackendSyncTest, Scene3_TexImage2DImmediateSampling) {
  const GLsizei size = 32;

  for (const auto& baseColor : {COLOR_RED, COLOR_GREEN, COLOR_BLUE}) {
    GLuint srcTex = CreateTexture();
    CreateTexture2DWithData(srcTex, size, size, baseColor);

    GLuint dstTex = CreateTexture();
    CreateTexture2D(dstTex, size, size);

    GLuint fbo = CreateFramebuffer();
    gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
    gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTex, 0);

    GLuint prog = CreateProgram(
        "attribute vec2 aPos; varying vec2 vTC; void main() { gl_Position = vec4(aPos, 0, 1); vTC = aPos * 0.5 + 0.5; }",
        "precision mediump float; varying vec2 vTC; uniform sampler2D uTex; void main() { gl_FragColor = texture2D(uTex, vTC); }");
    gl::g_current_gl_context->glUseProgramFn(prog);
    gl::g_current_gl_context->glActiveTextureFn(GL_TEXTURE0);
    gl::g_current_gl_context->glBindTextureFn(GL_TEXTURE_2D, srcTex);
    gl::g_current_gl_context->glUniform1iFn(gl::g_current_gl_context->glGetUniformLocationFn(prog, "uTex"), 0);

    GLuint buf = CreateBuffer();
    SetupQuad(prog, buf);

    gl::g_current_gl_context->glViewportFn(0, 0, size, size);
    SetClearColor(COLOR_BLACK);
    gl::g_current_gl_context->glClearFn(GL_COLOR_BUFFER_BIT);
    gl::g_current_gl_context->glDrawArraysFn(GL_TRIANGLES, 0, 6);

    EXPECT_TRUE(ReadPixel(0, 0).Equals(baseColor, 60)) << "TexImage2D->sampling sync failed";
  }
}

// Test: BufferData update -> immediate use
TEST_F(AngleBackendSyncTest, Scene3_BufferDataImmediateUse) {
  const GLsizei size = 32;
  GLuint tex = CreateTexture();
  CreateTexture2D(tex, size, size);

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
  gl::g_current_gl_context->glViewportFn(0, 0, size, size);

  for (int i = 0; i < 10; i++) {
    uint8_t r = (i * 25) % 256;
    char fs[128];
    snprintf(fs, sizeof(fs), "precision mediump float; uniform vec3 uColor; void main() { gl_FragColor = vec4(%f,%f,%f,1); }",
            r / 255.0f, 128 / 255.0f, 64 / 255.0f);

    GLuint prog = CreateProgram("attribute vec2 aPos; void main() { gl_Position = vec4(aPos, 0, 1); }", fs);
    gl::g_current_gl_context->glUseProgramFn(prog);

    GLuint buf = CreateBuffer();
    SetupQuad(prog, buf);

    SetClearColor(COLOR_BLACK);
    gl::g_current_gl_context->glClearFn(GL_COLOR_BUFFER_BIT);
    gl::g_current_gl_context->glDrawArraysFn(GL_TRIANGLES, 0, 6);

    ColorRGBA pixel = ReadPixel(0, 0);
    EXPECT_NEAR(pixel.r, r, 40) << "BufferData->use sync failed at iteration " << i;
  }
}

// Test: Data consistency after massive command insertion
TEST_F(AngleBackendSyncTest, Scene4_MassiveCommandInsertionConsistency) {
  const GLsizei size = 32;
  GLuint tex = CreateTexture();
  CreateTexture2D(tex, size, size);

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

  GLuint prog = CreateProgram(
      "attribute vec2 aPos; void main() { gl_Position = vec4(aPos, 0, 1); }",
      "precision mediump float; uniform vec3 uColor; void main() { gl_FragColor = vec4(uColor, 1); }");
  gl::g_current_gl_context->glUseProgramFn(prog);
  GLint colorLoc = gl::g_current_gl_context->glGetUniformLocationFn(prog, "uColor");
  GLint posLoc = gl::g_current_gl_context->glGetAttribLocationFn(prog, "aPos");
  gl::g_current_gl_context->glEnableVertexAttribArrayFn(posLoc);

  GLuint buf = CreateBuffer();
  gl::g_current_gl_context->glBindBufferFn(GL_ARRAY_BUFFER, buf);
  float vertices[] = {-1, -1, 1, -1, -1, 1, -1, 1, 1, -1, 1, 1};
  gl::g_current_gl_context->glBufferDataFn(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  gl::g_current_gl_context->glVertexAttribPointerFn(posLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

  gl::g_current_gl_context->glViewportFn(0, 0, size, size);

  for (int i = 0; i < 5; i++) {
    uint8_t r = (i * 50) % 256;
    gl::g_current_gl_context->glUniform3fFn(colorLoc, r / 255.0f, 100 / 255.0f, 50 / 255.0f);
    SetClearColor(COLOR_BLACK);
    gl::g_current_gl_context->glClearFn(GL_COLOR_BUFFER_BIT);
    gl::g_current_gl_context->glDrawArraysFn(GL_TRIANGLES, 0, 6);

    for (int j = 0; j < 20; j++) {
      GLuint noiseBuf = CreateBuffer();
      gl::g_current_gl_context->glBindBufferFn(GL_ARRAY_BUFFER, noiseBuf);
      float noiseData = j * 0.1f;
      gl::g_current_gl_context->glBufferDataFn(GL_ARRAY_BUFFER, sizeof(noiseData), &noiseData, GL_DYNAMIC_DRAW);
      gl::g_current_gl_context->glVertexAttribPointerFn(posLoc, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    gl::g_current_gl_context->glBindBufferFn(GL_ARRAY_BUFFER, buf);
    gl::g_current_gl_context->glVertexAttribPointerFn(posLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    ColorRGBA pixel = ReadPixel(0, 0);
    EXPECT_NEAR(pixel.r, r, 40) << "Command insertion consistency failed at iteration " << i;
  }
}

// Test: Cross texture binding switch
TEST_F(AngleBackendSyncTest, Scene4_CrossTextureBindingSwitch) {
  const GLsizei size = 32;
  std::vector<GLuint> texs;
  ColorRGBA colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW, COLOR_CYAN};

  for (int i = 0; i < 5; i++) {
    GLuint tex = CreateTexture();
    texs.push_back(tex);
    CreateTexture2DWithData(tex, size, size, colors[i]);
  }

  for (int i = 0; i < 5; i++) {
    GLuint fbo = CreateFramebuffer();
    gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
    gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texs[i], 0);
    EXPECT_TRUE(ReadPixel(0, 0).Equals(colors[i], 50)) << "Cross binding switch failed at texture " << i;
  }
}

// Test: WebGL2 Fence sync object
TEST_F(AngleBackendSyncTest, Scene5_FenceSyncSupportCheck) {
  if (!gl::g_current_gl_context->glFenceSyncFn) {
    GTEST_SKIP() << "glFenceSync not supported";
    return;
  }

  GLsync sync = gl::g_current_gl_context->glFenceSyncFn(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
  ASSERT_NE(sync, nullptr);
  gl::g_current_gl_context->glFlushFn();

  GLenum result = gl::g_current_gl_context->glClientWaitSyncFn(sync, 0, 0);
  gl::g_current_gl_context->glDeleteSyncFn(sync);

  EXPECT_TRUE(result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED || result == GL_TIMEOUT_EXPIRED)
      << "Fence sync returned unexpected result: " << result;
}

// Test: WebGL2 CopyBufferSubData sync
TEST_F(AngleBackendSyncTest, Scene5_CopyBufferSubDataSync) {
  if (!gl::g_current_gl_context->glCopyBufferSubDataFn) {
    GTEST_SKIP() << "glCopyBufferSubData not supported";
    return;
  }

  const int dataSize = 256;
  std::vector<float> srcData(dataSize);
  for (int i = 0; i < dataSize; i++) srcData[i] = i * 0.1f;

  GLuint srcBuf = CreateBuffer();
  gl::g_current_gl_context->glBindBufferFn(GL_COPY_READ_BUFFER, srcBuf);
  gl::g_current_gl_context->glBufferDataFn(GL_COPY_READ_BUFFER, srcData.size() * sizeof(float), srcData.data(), GL_STATIC_DRAW);

  GLuint dstBuf = CreateBuffer();
  gl::g_current_gl_context->glBindBufferFn(GL_COPY_WRITE_BUFFER, dstBuf);
  gl::g_current_gl_context->glBufferDataFn(GL_COPY_WRITE_BUFFER, srcData.size() * sizeof(float), nullptr, GL_STATIC_DRAW);

  gl::g_current_gl_context->glCopyBufferSubDataFn(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, srcData.size() * sizeof(float));

  if (gl::g_current_gl_context->glGetBufferSubDataFn) {
    std::vector<float> dstData(dataSize);
    gl::g_current_gl_context->glBindBufferFn(GL_ARRAY_BUFFER, dstBuf);
    gl::g_current_gl_context->glGetBufferSubDataFn(GL_ARRAY_BUFFER, 0, dstData.size() * sizeof(float), dstData.data());

    for (int i = 0; i < dataSize; i++) {
      EXPECT_FLOAT_EQ(dstData[i], srcData[i]) << "CopyBufferSubData data mismatch at index " << i;
    }
  } else {
    GTEST_SKIP() << "glGetBufferSubData not supported";
  }
}

// Test: WebGL2 Texture storage sync
TEST_F(AngleBackendSyncTest, Scene5_TextureStorageSync) {
  if (!gl::g_current_gl_context->glTexStorage2DFn) {
    GTEST_SKIP() << "glTexStorage2D not supported";
    return;
  }

  const GLsizei size = 32;
  GLuint tex = CreateTexture();
  gl::g_current_gl_context->glBindTextureFn(GL_TEXTURE_2D, tex);
  gl::g_current_gl_context->glTexStorage2DFn(GL_TEXTURE_2D, 1, GL_RGBA8, size, size);

  std::vector<ColorRGBA> data(size * size, COLOR_RED);
  gl::g_current_gl_context->glTexSubImage2DFn(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

  GLuint fbo = CreateFramebuffer();
  gl::g_current_gl_context->glBindFramebufferFn(GL_FRAMEBUFFER, fbo);
  gl::g_current_gl_context->glFramebufferTexture2DFn(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

  EXPECT_TRUE(ReadPixel(0, 0).Equals(COLOR_RED, 10)) << "Texture storage sync failed";
}
