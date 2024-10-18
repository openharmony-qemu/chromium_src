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

#define private public
#define protected public

#include "base/functional/callback.h"
#include "base/gtest_prod_util.h"
#include "base/memory/ref_counted_delete_on_sequence.h"
#include <gtest/gtest.h>
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/stream_texture_shared_image_interface.h"
#include "media/gpu/media_gpu_export.h"
#include "media/gpu/ohos/codec_image.h"
#include "media/gpu/ohos/codec_output_buffer_renderer.h"
#include <memory>

namespace media {
namespace {
std::shared_ptr<CodecImage> g_codec_image = nullptr;
}

class CodecImageTest : public testing::Test {
 public:
   static void SetUpTestCase(void);
   static void TearDownTestCase(void);
   void SetUp();
   void TearDown();
};

void CodecImageTest::SetUpTestCase(void) {}

void CodecImageTest::TearDownTestCase(void) {}

void CodecImageTest::SetUp(void) { 
  g_codec_image = std::make_shared<CodecImage>(gfx::Size(100, 100), nullptr);
}

void CodecImageTest::TearDown(void) {
  g_codec_image = nullptr; 
}

TEST_F(CodecImageTest, Initialize) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->Initialize(nullptr, true);
}

TEST_F(CodecImageTest, AddUnusedCB) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->AddUnusedCB(base::BindOnce([](CodecImage* image){}));
}

TEST_F(CodecImageTest, NotifyUnused) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->NotifyUnused();
}

TEST_F(CodecImageTest, NotifyOverlayPromotion) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->NotifyOverlayPromotion(true, gfx::Rect(0, 0, 100, 100));
}

TEST_F(CodecImageTest, ReleaseResources) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->ReleaseResources();
}

TEST_F(CodecImageTest, UpdateAndBindTexImage) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->UpdateAndBindTexImage(0);
}

TEST_F(CodecImageTest, RenderToFrontBuffer) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->RenderToFrontBuffer();
}

TEST_F(CodecImageTest, RenderToTextureOwnerBackBuffer) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->RenderToTextureOwnerBackBuffer();
}

TEST_F(CodecImageTest, RenderToTextureOwnerFrontBuffer) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->RenderToTextureOwnerFrontBuffer(
      gpu::StreamTextureSharedImageInterface::BindingsMode::kBindImage, 0);
}

TEST_F(CodecImageTest, RenderToOverlay) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->RenderToOverlay();
}

TEST_F(CodecImageTest, TextureOwnerBindsTextureOnUpdate) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->TextureOwnerBindsTextureOnUpdate();
}

TEST_F(CodecImageTest, ReleaseCodecBuffer) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->ReleaseCodecBuffer();
}
}
