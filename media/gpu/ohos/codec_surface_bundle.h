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

#ifndef MEDIA_GPU_OHOS_CODEC_SURFACE_BUNDLE_H_
#define MEDIA_GPU_OHOS_CODEC_SURFACE_BUNDLE_H_

#include "base/memory/ref_counted_delete_on_sequence.h"
#include "gpu/command_buffer/service/ohos/native_image_texture_owner.h"
#include "media/gpu/media_gpu_export.h"
#include "media/gpu/ohos/codec_buffer_wait_coordinator.h"

namespace gpu {
class RefCountedLock;
}  // namespace gpu

namespace media {
class MEDIA_GPU_EXPORT CodecSurfaceBundle
    : public base::RefCountedDeleteOnSequence<CodecSurfaceBundle> {
 public:
  CodecSurfaceBundle();
  explicit CodecSurfaceBundle(
      scoped_refptr<gpu::NativeImageTextureOwner> texture_owner,
      scoped_refptr<gpu::RefCountedLock> drdc_lock);

  CodecSurfaceBundle(const CodecSurfaceBundle&) = delete;
  CodecSurfaceBundle& operator=(const CodecSurfaceBundle&) = delete;

  void* GetOHOSNativeWindow() const;

  scoped_refptr<CodecBufferWaitCoordinator> codec_buffer_wait_coordinator()
      const {
    return codec_buffer_wait_coordinator_;
  }

 private:
  ~CodecSurfaceBundle();
  friend class base::RefCountedDeleteOnSequence<CodecSurfaceBundle>;
  friend class base::DeleteHelper<CodecSurfaceBundle>;

  scoped_refptr<CodecBufferWaitCoordinator> codec_buffer_wait_coordinator_;

  RAW_PTR_EXCLUSION void* ohos_native_window_;

  base::WeakPtrFactory<CodecSurfaceBundle> weak_factory_{this};
};

}  // namespace media

#endif  // MEDIA_GPU_OHOS_CODEC_SURFACE_BUNDLE_H_
