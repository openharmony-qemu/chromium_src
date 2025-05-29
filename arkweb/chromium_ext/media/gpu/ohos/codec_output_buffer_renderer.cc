// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on codec_output_buffer_renderer.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "media/gpu/ohos/codec_output_buffer_renderer.h"

#include <string.h>

#include "base/functional/callback_helpers.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder.h"
#include "gpu/command_buffer/service/texture_manager.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/scoped_make_current.h"

namespace media {

CodecOutputBufferRenderer::CodecOutputBufferRenderer(
    std::unique_ptr<CodecOutputBuffer> output_buffer,
    scoped_refptr<CodecBufferWaitCoordinator> codec_buffer_wait_coordinator,
    scoped_refptr<gpu::RefCountedLock> drdc_lock)
    : RefCountedLockHelperDrDc(std::move(drdc_lock)),
      output_buffer_(std::move(output_buffer)),
      codec_buffer_wait_coordinator_(std::move(codec_buffer_wait_coordinator)) {
}

CodecOutputBufferRenderer::~CodecOutputBufferRenderer() = default;

bool CodecOutputBufferRenderer::RenderToTextureOwnerBackBuffer() {
  AssertAcquiredDrDcLock();
  DCHECK_NE(phase_, Phase::kInFrontBuffer);
  if (phase_ == Phase::kInBackBuffer) {
    return true;
  }
  if (phase_ == Phase::kInvalidated) {
    return false;
  }

  if (!codec_buffer_wait_coordinator_) {
    return false;
  }

  if (codec_buffer_wait_coordinator_->IsExpectingFrameAvailable()) {
    return false;
  }
  if (!output_buffer_->ReleaseToSurface()) {
    phase_ = Phase::kInvalidated;
    return false;
  }
  phase_ = Phase::kInBackBuffer;
  codec_buffer_wait_coordinator_->SetReleaseTimeToNow();
  return true;
}

bool CodecOutputBufferRenderer::RenderToTextureOwnerFrontBuffer(
    BindingsMode bindings_mode,
    GLuint service_id) {
  LOG(DEBUG) << "CodecOutputBufferRenderer::RenderToTextureOwnerFrontBuffer";
  AssertAcquiredDrDcLock();
  if (!codec_buffer_wait_coordinator_) {
    return false;
  }

  if (phase_ == Phase::kInFrontBuffer) {
    EnsureBoundIfNeeded(bindings_mode, service_id);
    return true;
  }
  if (phase_ == Phase::kInvalidated) {
    return false;
  }

  if (phase_ != Phase::kInBackBuffer) {
    if (codec_buffer_wait_coordinator_->IsExpectingFrameAvailable()) {
      codec_buffer_wait_coordinator_->WaitForFrameAvailable();
      codec_buffer_wait_coordinator_->texture_owner()->UpdateNativeImage();
    }
    if (!RenderToTextureOwnerBackBuffer()) {
      DCHECK(phase_ == Phase::kInvalidated);
      return false;
    }
  }

  phase_ = Phase::kInFrontBuffer;
  if (codec_buffer_wait_coordinator_->IsExpectingFrameAvailable()) {
    codec_buffer_wait_coordinator_->WaitForFrameAvailable();
  }

  codec_buffer_wait_coordinator_->texture_owner()->UpdateNativeImage();
  if (codec_buffer_wait_coordinator_->texture_owner()
          ->binds_texture_on_update()) {
    was_tex_image_bound_ = true;
  }
  EnsureBoundIfNeeded(bindings_mode, service_id);
  return true;
}

void CodecOutputBufferRenderer::EnsureBoundIfNeeded(BindingsMode mode,
                                                    GLuint service_id) {
  AssertAcquiredDrDcLock();
  DCHECK(codec_buffer_wait_coordinator_);

  if (mode == BindingsMode::kBindImage) {
    DCHECK_GT(service_id, 0u);
    codec_buffer_wait_coordinator_->texture_owner()->EnsureNativeImageBound(
        service_id);
    was_tex_image_bound_ = true;
  }
}

bool CodecOutputBufferRenderer::RenderToFrontBuffer() {
  LOG(DEBUG) << "CodecOutputBufferRenderer::RenderToFrontBuffer";
  AssertAcquiredDrDcLock();

  // This code is used to trigger early rendering of the image before it is used
  // for compositing, there is no need to bind the image. Hence pass texture
  // service_id as 0.
  return RenderToTextureOwnerFrontBuffer(BindingsMode::kDontBindImage,
                                         0 /* service_id */);
}

}  // namespace media
                     