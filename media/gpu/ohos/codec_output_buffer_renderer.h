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

#ifndef MEDIA_GPU_OHOS_CODEC_OUTPUT_BUFFER_RENDERER_H_
#define MEDIA_GPU_OHOS_CODEC_OUTPUT_BUFFER_RENDERER_H_

#include <cstdint>

#include <memory>

#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/stream_texture_shared_image_interface.h"
#include "media/gpu/media_gpu_export.h"
#include "media/gpu/ohos/codec_buffer_wait_coordinator.h"
#include "media/gpu/ohos/codec_wrapper.h"

namespace media {

// A class that holds CodecOutputBuffer and renders it to TextureOwner or
// overlay as necessary. Unit tests for this class are part of CodecImage unit
// tests. Note that when DrDc is enabled(kEnableDrDc),
// a per codec dr-dc lock is expected to be held while calling methods of this
// class. This is ensured by adding AssertAcquiredDrDcLock() to those methods.
class MEDIA_GPU_EXPORT CodecOutputBufferRenderer
    : public gpu::RefCountedLockHelperDrDc {
 public:
  CodecOutputBufferRenderer(
      std::unique_ptr<CodecOutputBuffer> output_buffer,
      scoped_refptr<CodecBufferWaitCoordinator> codec_buffer_wait_coordinator,
      scoped_refptr<gpu::RefCountedLock> drdc_lock);
  ~CodecOutputBufferRenderer();

  CodecOutputBufferRenderer(const CodecOutputBufferRenderer&) = delete;
  CodecOutputBufferRenderer& operator=(const CodecOutputBufferRenderer&) =
      delete;

  // Renders this image to the overlay. Returns true if the buffer is in the
  // overlay front buffer. Returns false if the buffer was invalidated.
  bool RenderToOverlay();

  // Renders this image to the texture owner front buffer by first rendering
  // it to the back buffer if it's not already there, and then waiting for the
  // frame available event before calling UpdateTexImage().
  bool RenderToTextureOwnerFrontBuffer();

  // Renders this image to the front buffer of its backing surface.
  // Returns true if the buffer is in the front buffer. Returns false if the
  // buffer was invalidated. After an image is invalidated it's no longer
  // possible to render it.
  bool RenderToFrontBuffer();

  // Renders this image to the back buffer of its texture owner. Only valid if
  // is_texture_owner_backed(). Returns true if the buffer is in the back
  // buffer. Returns false if the buffer was invalidated.
  // RenderToTextureOwnerBackBuffer() will not block if there is any previously
  // pending frame and will return false in this case.
  bool RenderToTextureOwnerBackBuffer();

  void InvalidateForTesting() { Invalidate(); }

  // Runs the frame info callback when UpdateTexImage() is called. If the buffer
  // is dropped without being rendered to the front buffer, std::nullopt will
  // be sent for the coded size and visible rect.
  using FrameInfoCallback =
      base::OnceCallback<void(std::optional<gfx::Size> coded_size,
                              std::optional<gfx::Rect> visible_rect)>;
  void set_frame_info_callback(FrameInfoCallback callback) {
    frame_info_callback_ = std::move(callback);
  }

  // Whether the codec buffer has been rendered to the front buffer.
  bool was_rendered_to_front_buffer() const {
    AssertAcquiredDrDcLock();
    return phase_ == Phase::kInFrontBuffer;
  }

  gfx::Size size() const { return output_buffer_->size(); }

  // Color space of the image.
  const gfx::ColorSpace& color_space() const {
    return output_buffer_->color_space();
  }

  scoped_refptr<gpu::NativeImageTextureOwner> texture_owner() const {
    return codec_buffer_wait_coordinator_
               ? codec_buffer_wait_coordinator_->texture_owner()
               : nullptr;
  }

  CodecOutputBuffer* get_codec_output_buffer_for_testing() const {
    return output_buffer_.get();
  }

 private:
  friend class FrameInfoHelperTest;
  // The lifecycle phases of an buffer.
  // The only possible transitions are from left to right. Both
  // kInFrontBuffer and kInvalidated are terminal.
  enum class Phase { kInCodec, kInBackBuffer, kInFrontBuffer, kInvalidated };

  // Sets `phase_` to Phase::kInvalidated and clears `frame_info_callback_` if
  // needed.
  void Invalidate();

  // The phase of the image buffer's lifecycle.
  Phase phase_ = Phase::kInCodec;

  // The buffer backing this image.
  std::unique_ptr<CodecOutputBuffer> output_buffer_;

  // The CodecBufferWaitCoordinator that |output_buffer_| will be rendered to.
  // Or null, if this image is backed by an overlay.
  scoped_refptr<CodecBufferWaitCoordinator> codec_buffer_wait_coordinator_;

  FrameInfoCallback frame_info_callback_;
};

}  // namespace media
#endif  // MEDIA_GPU_OHOS_CODEC_OUTPUT_BUFFER_RENDERER_H_
