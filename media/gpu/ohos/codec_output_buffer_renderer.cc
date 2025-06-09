/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "media/gpu/ohos/codec_output_buffer_renderer.h"
#include <string>

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

CodecOutputBufferRenderer::~CodecOutputBufferRenderer() {
  Invalidate();
}

bool CodecOutputBufferRenderer::RenderToTextureOwnerBackBuffer() {
  AssertAcquiredDrDcLock();
  DCHECK_NE(phase_, Phase::kInFrontBuffer);
  if (phase_ == Phase::kInBackBuffer)
    return true;
  if (phase_ == Phase::kInvalidated)
    return false;

  // Normally, we should have a wait coordinator if we're called.  However, if
  // the renderer is torn down (either VideoFrameSubmitter or the whole process)
  // before we get returns back from viz, then we can be notified that we're
  // no longer in use (erroneously) when the VideoFrame is destroyed.  So, if
  // we don't have a wait coordinator, then just fail.
  if (!codec_buffer_wait_coordinator_)
    return false;

  // Don't render frame if one is already pending.
  // RenderToTextureOwnerFrontBuffer will wait before calling this.
  if (codec_buffer_wait_coordinator_->IsExpectingFrameAvailable()) {
    return false;
  }
  if (!output_buffer_->ReleaseToSurface()) {
    Invalidate();
    return false;
  }
  phase_ = Phase::kInBackBuffer;
  codec_buffer_wait_coordinator_->SetReleaseTimeToNow();
  return true;
}

bool CodecOutputBufferRenderer::RenderToTextureOwnerFrontBuffer() {
  AssertAcquiredDrDcLock();
  if (!codec_buffer_wait_coordinator_)
    return false;

  if (phase_ == Phase::kInFrontBuffer) {
    return true;
  }
  if (phase_ == Phase::kInvalidated)
    return false;

  if (phase_ != Phase::kInBackBuffer) {
    if (codec_buffer_wait_coordinator_->IsExpectingFrameAvailable()) {
      codec_buffer_wait_coordinator_->WaitForFrameAvailable();

      codec_buffer_wait_coordinator_->texture_owner()->UpdateNativeImage();
    }
    if (!RenderToTextureOwnerBackBuffer()) {
      DCHECK_EQ(phase_, Phase::kInvalidated);
      return false;
    }
  }

  phase_ = Phase::kInFrontBuffer;
  if (codec_buffer_wait_coordinator_->IsExpectingFrameAvailable())
    codec_buffer_wait_coordinator_->WaitForFrameAvailable();

  codec_buffer_wait_coordinator_->texture_owner()->UpdateNativeImage();

  if (frame_info_callback_) {
    gfx::Size coded_size;
    gfx::Rect visible_rect;
    if (texture_owner() && texture_owner()->GetCodedSizeAndVisibleRect(
                               size(), &coded_size, &visible_rect)) {
      std::move(frame_info_callback_).Run(coded_size, visible_rect);
    } else {
      std::move(frame_info_callback_).Run(std::nullopt, std::nullopt);
    }
  }

  return true;
}

bool CodecOutputBufferRenderer::RenderToOverlay() {
  AssertAcquiredDrDcLock();
  if (phase_ == Phase::kInFrontBuffer)
    return true;
  if (phase_ == Phase::kInvalidated)
    return false;

  if (!output_buffer_->ReleaseToSurface()) {
    Invalidate();
    return false;
  }
  phase_ = Phase::kInFrontBuffer;
  return true;
}

bool CodecOutputBufferRenderer::RenderToFrontBuffer() {
  AssertAcquiredDrDcLock();

  // Trigger early rendering of the image before it is used for compositing.
  return codec_buffer_wait_coordinator_ ? RenderToTextureOwnerFrontBuffer()
                                        : RenderToOverlay();
}

void CodecOutputBufferRenderer::Invalidate() {
  phase_ = Phase::kInvalidated;
  if (frame_info_callback_) {
    std::move(frame_info_callback_).Run(std::nullopt, std::nullopt);
  }
}

}  // namespace media
