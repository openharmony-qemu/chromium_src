// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>

#include <memory>

#include "gmock/gmock.h"
#include "media/base/test_helpers.h"


#define private public
#include "media/gpu/ohos/codec_output_buffer_renderer.h"
#undef private

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
    codec_output_buffer_renderer_ = std::make_unique<CodecOutputBufferRenderer>(
        std::move(output_buffer), std::move(codec_buffer_wait_coordinator),
        std::move(drdc_lock));
  }

  BindingsMode bindings_mode_;
  GLuint service_id_;
  std::unique_ptr<CodecOutputBufferRenderer> codec_output_buffer_renderer_;
};

TEST_F(CodecOutputBufferRendererTest, TestRenderToTextureOwnerBackBuffer001) {
  codec_output_buffer_renderer_->phase_ =
      CodecOutputBufferRenderer::Phase::kInBackBuffer;
  bool actual_result =
      codec_output_buffer_renderer_->RenderToTextureOwnerBackBuffer();
  EXPECT_TRUE(actual_result);
}

TEST_F(CodecOutputBufferRendererTest, TestRenderToTextureOwnerBackBuffer002) {
  codec_output_buffer_renderer_->phase_ =
      CodecOutputBufferRenderer::Phase::kInvalidated;
  bool actual_result =
      codec_output_buffer_renderer_->RenderToTextureOwnerBackBuffer();
  EXPECT_FALSE(actual_result);
}

TEST_F(CodecOutputBufferRendererTest, TestRenderToTextureOwnerBackBuffer003) {
  codec_output_buffer_renderer_->codec_buffer_wait_coordinator_ = nullptr;
  bool actual_result =
      codec_output_buffer_renderer_->RenderToTextureOwnerBackBuffer();
  EXPECT_FALSE(actual_result);
}

TEST_F(CodecOutputBufferRendererTest, TestRenderToTextureOwnerFrontBuffer001) {
  codec_output_buffer_renderer_->codec_buffer_wait_coordinator_ = nullptr;
  bool actual_result =
      codec_output_buffer_renderer_->RenderToTextureOwnerFrontBuffer(
          BindingsMode::kBindImage, 1);
  EXPECT_FALSE(actual_result);
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
TEST_F(CodecOutputBufferRendererTest, TestRenderVideoView001) {
  bool expected_result = false;
  bool actual_result = codec_output_buffer_renderer_->RenderVideoView();
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(CodecOutputBufferRendererTest, TestRenderVideoView002) {
  bool expected_result = false;
  codec_output_buffer_renderer_->output_buffer_ = nullptr;
  bool actual_result = codec_output_buffer_renderer_->RenderVideoView();
  EXPECT_EQ(expected_result, actual_result);
}
#endif
}  // namespace media
