// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_GPU_OHOS_CODEC_OUTPUT_BUFFER_RENDERER_H_
#define MEDIA_GPU_OHOS_CODEC_OUTPUT_BUFFER_RENDERER_H_

#include <stdint.h>

#include <memory>

#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/stream_texture_shared_image_interface.h"
#include "media/gpu/media_gpu_export.h"
#include "media/gpu/ohos/codec_buffer_wait_coordinator.h"
#include "media/gpu/ohos/codec_wrapper.h"

namespace media {

class MEDIA_GPU_EXPORT CodecOutputBufferRenderer
    : public gpu::RefCountedLockHelperDrDc {
 public:
  using BindingsMode = gpu::StreamTextureSharedImageInterface::BindingsMode;

  CodecOutputBufferRenderer(
      std::unique_ptr<CodecOutputBuffer> output_buffer,
      scoped_refptr<CodecBufferWaitCoordinator> codec_buffer_wait_coordinator,
      scoped_refptr<gpu::RefCountedLock> drdc_lock);
  ~CodecOutputBufferRenderer();

  CodecOutputBufferRenderer(const CodecOutputBufferRenderer&) = delete;
  CodecOutputBufferRenderer& operator=(const CodecOutputBufferRenderer&) =
      delete;

  bool RenderToTextureOwnerFrontBuffer(BindingsMode bindings_mode,
                                       GLuint service_id);

  bool RenderToFrontBuffer();

  bool RenderToTextureOwnerBackBuffer();

  bool was_rendered_to_front_buffer() const {
    AssertAcquiredDrDcLock();
    return phase_ == Phase::kInFrontBuffer;
  }

  gfx::Size size() const { return output_buffer_->size(); }

  const gfx::ColorSpace& color_space() const {
    return output_buffer_->color_space();
  }

  bool was_tex_image_bound() const { return was_tex_image_bound_; }

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

  enum class Phase { kInCodec, kInBackBuffer, kInFrontBuffer, kInvalidated };

  void EnsureBoundIfNeeded(BindingsMode mode, GLuint service_id);

  void set_phase_for_testing(Phase phase) { phase_ = phase; }

  Phase phase_ = Phase::kInCodec;

  std::unique_ptr<CodecOutputBuffer> output_buffer_;

  scoped_refptr<CodecBufferWaitCoordinator> codec_buffer_wait_coordinator_;

  bool was_tex_image_bound_ = false;
};

}  // namespace media
#endif  // MEDIA_GPU_OHOS_CODEC_OUTPUT_BUFFER_RENDERER_H_
