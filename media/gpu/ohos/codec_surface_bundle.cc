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

#include "media/gpu/ohos/codec_surface_bundle.h"

#include "base/functional/bind.h"
#include "base/task/sequenced_task_runner.h"
#include "media/base/ohos/ohos_media_decoder_bridge_impl.h"

namespace media {

CodecSurfaceBundle::CodecSurfaceBundle()
    : RefCountedDeleteOnSequence<CodecSurfaceBundle>(
          base::SequencedTaskRunner::GetCurrentDefault()) {}

CodecSurfaceBundle::CodecSurfaceBundle(
    scoped_refptr<gpu::NativeImageTextureOwner> texture_owner,
    scoped_refptr<gpu::RefCountedLock> drdc_lock)
    : RefCountedDeleteOnSequence<CodecSurfaceBundle>(
          base::SequencedTaskRunner::GetCurrentDefault()),
      codec_buffer_wait_coordinator_(
          base::MakeRefCounted<CodecBufferWaitCoordinator>(
              std::move(texture_owner),
              std::move(drdc_lock))),
      ohos_native_window_(codec_buffer_wait_coordinator_->texture_owner()
                              ->AquireOhosNativeWindow()) {}

CodecSurfaceBundle::~CodecSurfaceBundle() {
  if (!codec_buffer_wait_coordinator_)
    return;

  auto task_runner =
      codec_buffer_wait_coordinator_->texture_owner()->task_runner();
  if (task_runner->RunsTasksInCurrentSequence()) {
    codec_buffer_wait_coordinator_->texture_owner()->ReleaseNativeImage();
  } else {
    task_runner->PostTask(
        FROM_HERE,
        base::BindOnce(&gpu::NativeImageTextureOwner::ReleaseNativeImage,
                       codec_buffer_wait_coordinator_->texture_owner()));
  }
}

void* CodecSurfaceBundle::GetOHOSNativeWindow() const {
  return ohos_native_window_;
}

}  // namespace media
