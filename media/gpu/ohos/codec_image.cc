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

#include "media/gpu/ohos/codec_image.h"

#include <string>

#include <memory>

#include "base/functional/callback_helpers.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder.h"
#include "gpu/command_buffer/service/texture_manager.h"
#include "gpu/config/gpu_finch_features.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/scoped_make_current.h"

namespace media {

CodecImage::CodecImage(const gfx::Size& coded_size,
                       scoped_refptr<gpu::RefCountedLock> drdc_lock)
    : RefCountedLockHelperDrDc(std::move(drdc_lock)), coded_size_(coded_size) {}

CodecImage::~CodecImage() {
  DCHECK_CALLED_ON_VALID_THREAD(gpu_main_thread_checker_);
  AssertAcquiredDrDcLock();
  NotifyUnused();
}

void CodecImage::Initialize(
    std::unique_ptr<CodecOutputBufferRenderer> output_buffer_renderer,
    bool is_texture_owner_backed) {
  DCHECK(output_buffer_renderer);
  output_buffer_renderer_ = std::move(output_buffer_renderer);
  is_texture_owner_backed_ = is_texture_owner_backed;
}

void CodecImage::AddUnusedCB(UnusedCB unused_cb) {
  DCHECK_CALLED_ON_VALID_THREAD(gpu_main_thread_checker_);
  unused_cbs_.push_back(std::move(unused_cb));
}

void CodecImage::NotifyUnused() {
  DCHECK_CALLED_ON_VALID_THREAD(gpu_main_thread_checker_);
  AssertAcquiredDrDcLock();

  ReleaseCodecBuffer();

  for (auto& cb : unused_cbs_)
    std::move(cb).Run(this);
  unused_cbs_.clear();
}

void CodecImage::NotifyOverlayPromotion(bool promotion,
                                        const gfx::Rect& bounds) {}

void CodecImage::ReleaseResources() {
  DCHECK_CALLED_ON_VALID_THREAD(gpu_main_thread_checker_);
  ReleaseCodecBuffer();
}

void CodecImage::UpdateAndBindTexImage() {
  AssertAcquiredDrDcLock();
  RenderToTextureOwnerFrontBuffer();
}

bool CodecImage::HasTextureOwner() const {
  return !!texture_owner();
}

gpu::TextureBase* CodecImage::GetTextureBase() const {
  return texture_owner()->GetTextureBase();
}

bool CodecImage::RenderToFrontBuffer() {
  DCHECK_CALLED_ON_VALID_THREAD(gpu_main_thread_checker_);
  AssertAcquiredDrDcLock();
  if (!output_buffer_renderer_)
    return false;
  return output_buffer_renderer_->RenderToFrontBuffer();
}

bool CodecImage::RenderToTextureOwnerBackBuffer() {
  DCHECK_CALLED_ON_VALID_THREAD(gpu_main_thread_checker_);
  AssertAcquiredDrDcLock();
  if (!output_buffer_renderer_)
    return false;

  return output_buffer_renderer_->RenderToTextureOwnerBackBuffer();
}

bool CodecImage::RenderToTextureOwnerFrontBuffer() {
  AssertAcquiredDrDcLock();
  if (!output_buffer_renderer_)
    return false;
  return output_buffer_renderer_->RenderToTextureOwnerFrontBuffer();
}

bool CodecImage::RenderToOverlay() {
  return false;
}

bool CodecImage::TextureOwnerBindsTextureOnUpdate() {
  AssertAcquiredDrDcLock();
  if (!output_buffer_renderer_)
    return false;
  return output_buffer_renderer_->texture_owner()->binds_texture_on_update();
}

void CodecImage::ReleaseCodecBuffer() {
  DCHECK_CALLED_ON_VALID_THREAD(gpu_main_thread_checker_);
  AssertAcquiredDrDcLock();
  output_buffer_renderer_.reset();
}

CodecImageHolder::CodecImageHolder(
    scoped_refptr<base::SequencedTaskRunner> task_runner,
    scoped_refptr<CodecImage> codec_image)
    : base::RefCountedDeleteOnSequence<CodecImageHolder>(
          std::move(task_runner)),
      codec_image_(std::move(codec_image)) {}

CodecImageHolder::~CodecImageHolder() = default;

}  // namespace media
