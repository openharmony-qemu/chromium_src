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

#include "base/functional/bind.h"
#define private public
#define protected public

#include <gtest/gtest.h>
#include <memory>
#include "base/functional/callback.h"
#include "base/gtest_prod_util.h"
#include "base/memory/ref_counted_delete_on_sequence.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/stream_texture_shared_image_interface.h"
#include "gpu/config/gpu_preferences.h"
#include "media/gpu/media_gpu_export.h"
#include "media/gpu/ohos/codec_image.h"
#include "media/gpu/ohos/codec_output_buffer_renderer.h"
#include "media/gpu/ohos/video_frame_factory_impl.h"

namespace media {
namespace {
std::shared_ptr<CodecImage> g_codec_image = nullptr;
}

class CodecImageTest : public testing::Test {
 public:
  static void SetUpTestCase();
  static void TearDownTestCase();
  void SetUp();
  void TearDown();
};

void CodecImageTest::SetUpTestCase() {}

void CodecImageTest::TearDownTestCase() {}

void CodecImageTest::SetUp() {
  g_codec_image = std::make_shared<CodecImage>(gfx::Size(100, 100), nullptr);
}

void CodecImageTest::TearDown() {
  g_codec_image = nullptr;
}

TEST_F(CodecImageTest, Initialize) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->Initialize(nullptr, true);
  ASSERT_EQ(g_codec_image->output_buffer_renderer_, nullptr);
  ASSERT_EQ(g_codec_image->is_texture_owner_backed_, true);
}

TEST_F(CodecImageTest, AddUnusedCB) {
  ASSERT_NE(g_codec_image, nullptr);
  auto unusedCB = base::BindOnce([](CodecImage* image) {});
  g_codec_image->AddUnusedCB(std::move(unusedCB));
  ASSERT_FALSE(g_codec_image->unused_cbs_.empty());
}

TEST_F(CodecImageTest, NotifyUnused) {
  ASSERT_NE(g_codec_image, nullptr);
  auto unusedCB = base::BindOnce([](CodecImage* image) {});
  g_codec_image->AddUnusedCB(std::move(unusedCB));
  g_codec_image->NotifyUnused();

  g_codec_image->NotifyUnused();
  ASSERT_TRUE(g_codec_image->unused_cbs_.empty());
}

TEST_F(CodecImageTest, NotifyOverlayPromotion) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->NotifyOverlayPromotion(true, gfx::Rect(0, 0, 100, 100));
}

TEST_F(CodecImageTest, IsUsingGpuMemory) {
  ASSERT_NE(g_codec_image, nullptr);
  std::unique_ptr<CodecOutputBuffer> output_buffer = nullptr;
  scoped_refptr<CodecBufferWaitCoordinator> codec_buffer_wait_coordinator =
      nullptr;
  scoped_refptr<gpu::RefCountedLock> drdc_lock = nullptr;
  std::unique_ptr<CodecOutputBufferRenderer> output_buffer_renderer = nullptr;
  output_buffer_renderer = std::make_unique<CodecOutputBufferRenderer>(
      std::move(output_buffer), std::move(codec_buffer_wait_coordinator),
      std::move(drdc_lock));
  g_codec_image->output_buffer_renderer_ = std::move(output_buffer_renderer);
  g_codec_image->IsUsingGpuMemory();

  g_codec_image->output_buffer_renderer_ = nullptr;
  auto ret = g_codec_image->IsUsingGpuMemory();
  EXPECT_EQ(ret, false);
}

TEST_F(CodecImageTest, UpdateAndBindTexImage) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->UpdateAndBindTexImage(0);
}

TEST_F(CodecImageTest, RenderToFrontBuffer) {
  ASSERT_NE(g_codec_image, nullptr);
  std::unique_ptr<CodecOutputBuffer> output_buffer = nullptr;
  scoped_refptr<CodecBufferWaitCoordinator> codec_buffer_wait_coordinator =
      nullptr;
  scoped_refptr<gpu::RefCountedLock> drdc_lock = nullptr;
  std::unique_ptr<CodecOutputBufferRenderer> output_buffer_renderer = nullptr;
  output_buffer_renderer = std::make_unique<CodecOutputBufferRenderer>(
      std::move(output_buffer), std::move(codec_buffer_wait_coordinator),
      std::move(drdc_lock));
  g_codec_image->output_buffer_renderer_ = std::move(output_buffer_renderer);
  g_codec_image->RenderToFrontBuffer();

  g_codec_image->output_buffer_renderer_ = nullptr;
  auto ret = g_codec_image->RenderToFrontBuffer();
  EXPECT_EQ(ret, false);
}

TEST_F(CodecImageTest, RenderToTextureOwnerBackBuffer) {
  ASSERT_NE(g_codec_image, nullptr);
  std::unique_ptr<CodecOutputBuffer> output_buffer = nullptr;
  scoped_refptr<CodecBufferWaitCoordinator> codec_buffer_wait_coordinator =
      nullptr;
  scoped_refptr<gpu::RefCountedLock> drdc_lock = nullptr;
  std::unique_ptr<CodecOutputBufferRenderer> output_buffer_renderer = nullptr;
  output_buffer_renderer = std::make_unique<CodecOutputBufferRenderer>(
      std::move(output_buffer), std::move(codec_buffer_wait_coordinator),
      std::move(drdc_lock));
  g_codec_image->output_buffer_renderer_ = std::move(output_buffer_renderer);
  g_codec_image->RenderToTextureOwnerBackBuffer();

  g_codec_image->output_buffer_renderer_ = nullptr;
  auto ret = g_codec_image->RenderToTextureOwnerBackBuffer();
  EXPECT_EQ(ret, false);
}

TEST_F(CodecImageTest, RenderToTextureOwnerFrontBuffer) {
  ASSERT_NE(g_codec_image, nullptr);
  std::unique_ptr<CodecOutputBuffer> output_buffer = nullptr;
  scoped_refptr<CodecBufferWaitCoordinator> codec_buffer_wait_coordinator =
      nullptr;
  scoped_refptr<gpu::RefCountedLock> drdc_lock = nullptr;
  std::unique_ptr<CodecOutputBufferRenderer> output_buffer_renderer = nullptr;
  output_buffer_renderer = std::make_unique<CodecOutputBufferRenderer>(
      std::move(output_buffer), std::move(codec_buffer_wait_coordinator),
      std::move(drdc_lock));
  g_codec_image->output_buffer_renderer_ = std::move(output_buffer_renderer);
  g_codec_image->RenderToTextureOwnerFrontBuffer(
      gpu::StreamTextureSharedImageInterface::BindingsMode::kBindImage, 0);

  g_codec_image->output_buffer_renderer_ = nullptr;
  auto ret = g_codec_image->RenderToTextureOwnerFrontBuffer(
      gpu::StreamTextureSharedImageInterface::BindingsMode::kBindImage, 0);
  EXPECT_EQ(ret, false);
}

TEST_F(CodecImageTest, RenderToOverlay) {
  ASSERT_NE(g_codec_image, nullptr);
  auto ret = g_codec_image->RenderToOverlay();
  EXPECT_EQ(ret, false);
}

TEST_F(CodecImageTest, TextureOwnerBindsTextureOnUpdate) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->TextureOwnerBindsTextureOnUpdate();
}

TEST_F(CodecImageTest, ReleaseCodecBuffer) {
  ASSERT_NE(g_codec_image, nullptr);
  g_codec_image->ReleaseCodecBuffer();
}
}  // namespace media
