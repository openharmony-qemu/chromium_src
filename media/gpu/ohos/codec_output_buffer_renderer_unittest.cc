// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>
#include <memory>

#define private public
#include "media/gpu/ohos/codec_output_buffer_renderer.h"

namespace media {
class CodecOutputBufferRendererTest : public testing::Test {
 public:
  using BindingsMode = gpu::StreamTextureSharedImageInterface::BindingsMode;
  void SetUp() {
    bindings_mode_ = BindingsMode::kBindImage;
    service_id_ = 1u;
    std::unique_ptr<CodecOutputBuffer> output_buffer;
    scoped_refptr<CodecBufferWaitCoordinator> codec_buffer_wait_coordinator;
    scoped_refptr<gpu::RefCountedLock> drdc_lock;
    codec_output_buffer_ = std::make_unique<CodecOutputBufferRenderer>(
        std::move(output_buffer), std::move(codec_buffer_wait_coordinator),
        std::move(drdc_lock));
  }

  BindingsMode bindings_mode_;
  GLuint service_id_;
  std::unique_ptr<CodecOutputBufferRenderer> codec_output_buffer_;
};

TEST_F(CodecOutputBufferRendererTest, RenderToTextureOwnerBackBuffer) {
  bool back_buffer;
  back_buffer = codec_output_buffer_->RenderToTextureOwnerBackBuffer();
  EXPECT_FALSE(back_buffer);

  codec_output_buffer_->phase_ =
      CodecOutputBufferRenderer::Phase::kInBackBuffer;
  back_buffer = codec_output_buffer_->RenderToTextureOwnerBackBuffer();
  EXPECT_TRUE(back_buffer);

  codec_output_buffer_->phase_ = CodecOutputBufferRenderer::Phase::kInvalidated;
  back_buffer = codec_output_buffer_->RenderToTextureOwnerBackBuffer();
  EXPECT_FALSE(back_buffer);
}

TEST_F(CodecOutputBufferRendererTest, RenderToTextureOwnerFrontBuffer) {
  bool front_buffer = codec_output_buffer_->RenderToTextureOwnerFrontBuffer(
      bindings_mode_, service_id_);
  EXPECT_FALSE(front_buffer);
}

TEST_F(CodecOutputBufferRendererTest, RenderToFrontBuffer) {
  bool back_buffer = codec_output_buffer_->RenderToTextureOwnerBackBuffer();
  EXPECT_FALSE(back_buffer);
  bool front_buffer = codec_output_buffer_->RenderToFrontBuffer();
  EXPECT_FALSE(front_buffer);
  EXPECT_EQ(back_buffer, front_buffer);
}

TEST_F(CodecOutputBufferRendererTest, EnsureBoundIfNeeded) {}

}  // namespace media
