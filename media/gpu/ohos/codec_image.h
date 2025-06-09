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

#ifndef MEDIA_GPU_OHOS_CODEC_IMAGE_H_
#define MEDIA_GPU_OHOS_CODEC_IMAGE_H_

#include <cstdint>

#include <memory>
#include <vector>

#include "base/functional/callback.h"
#include "base/gtest_prod_util.h"
#include "base/memory/ref_counted_delete_on_sequence.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/stream_texture_shared_image_interface.h"
#include "media/gpu/media_gpu_export.h"
#include "media/gpu/ohos/codec_output_buffer_renderer.h"

namespace media {
class MEDIA_GPU_EXPORT CodecImage
    : public gpu::StreamTextureSharedImageInterface,
      gpu::RefCountedLockHelperDrDc {
 public:
  using UnusedCB = base::OnceCallback<void(CodecImage*)>;

  CodecImage(const gfx::Size& coded_size,
             scoped_refptr<gpu::RefCountedLock> drdc_lock);

  CodecImage(const CodecImage&) = delete;
  CodecImage& operator=(const CodecImage&) = delete;

  void Initialize(
      std::unique_ptr<CodecOutputBufferRenderer> output_buffer_renderer,
      bool is_texture_owner_backed);

  void AddUnusedCB(UnusedCB unused_cb);

  void NotifyUnused();
  void ReleaseResources() override;
  void UpdateAndBindTexImage() override;
  bool HasTextureOwner() const override;
  gpu::TextureBase* GetTextureBase() const override;
  void NotifyOverlayPromotion(bool promotion, const gfx::Rect& bounds) override;
  bool RenderToOverlay() override;
  bool TextureOwnerBindsTextureOnUpdate() override;

  bool was_rendered_to_front_buffer() const {
    return output_buffer_renderer_
               ? output_buffer_renderer_->was_rendered_to_front_buffer()
               : false;
  }

  bool is_texture_owner_backed() const { return is_texture_owner_backed_; }

  scoped_refptr<gpu::NativeImageTextureOwner> texture_owner() const {
    return output_buffer_renderer_ ? output_buffer_renderer_->texture_owner()
                                   : nullptr;
  }

  bool RenderToFrontBuffer();
  bool RenderToTextureOwnerBackBuffer();
  virtual void ReleaseCodecBuffer();

  CodecOutputBuffer* get_codec_output_buffer_for_testing() const {
    return output_buffer_renderer_
               ? output_buffer_renderer_->get_codec_output_buffer_for_testing()
               : nullptr;
  }

 protected:
  ~CodecImage() override;

 private:
  FRIEND_TEST_ALL_PREFIXES(CodecImageTest, RenderAfterUnusedDoesntCrash);

  std::unique_ptr<CodecOutputBufferRenderer> output_buffer_renderer_;

  bool RenderToTextureOwnerFrontBuffer();

  bool is_texture_owner_backed_ = false;

  gfx::Rect most_recent_bounds_;

  gfx::Size coded_size_;

  std::vector<UnusedCB> unused_cbs_;

  THREAD_CHECKER(gpu_main_thread_checker_);
};

class MEDIA_GPU_EXPORT CodecImageHolder
    : public base::RefCountedDeleteOnSequence<CodecImageHolder> {
 public:
  CodecImageHolder(scoped_refptr<base::SequencedTaskRunner> task_runner,
                   scoped_refptr<CodecImage> codec_image);

  CodecImageHolder(const CodecImageHolder&) = delete;
  CodecImageHolder& operator=(const CodecImageHolder&) = delete;

  CodecImage* codec_image_raw() const { return codec_image_.get(); }

 private:
  virtual ~CodecImageHolder();

  friend class base::RefCountedDeleteOnSequence<CodecImageHolder>;
  friend class base::DeleteHelper<CodecImageHolder>;

  scoped_refptr<CodecImage> codec_image_;
};

}  // namespace media

#endif  // MEDIA_GPU_OHOS_CODEC_IMAGE_H_
