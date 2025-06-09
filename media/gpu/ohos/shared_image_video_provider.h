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

#ifndef MEDIA_GPU_OHOS_SHARED_IMAGE_VIDEO_PROVIDER_H_
#define MEDIA_GPU_OHOS_SHARED_IMAGE_VIDEO_PROVIDER_H_

#include "base/functional/callback.h"
#include "gpu/command_buffer/client/client_shared_image.h"
#include "gpu/ipc/common/vulkan_ycbcr_info.h"
#include "media/gpu/media_gpu_export.h"
#include "media/gpu/ohos/codec_image.h"
#include "ui/gfx/color_space.h"
#include "ui/gfx/geometry/size.h"

namespace gpu {
class CommandBufferStub;
class SharedContextState;
struct SyncToken;
}  // namespace gpu

namespace media {

class MEDIA_GPU_EXPORT SharedImageVideoProvider {
 public:
  using GetStubCB = base::RepeatingCallback<gpu::CommandBufferStub*()>;
  using GpuInitCB =
      base::OnceCallback<void(scoped_refptr<gpu::SharedContextState>)>;

  struct ImageSpec {
    ImageSpec();
    ImageSpec(const gfx::Size& coded_size, uint64_t generation_id);
    ImageSpec(const ImageSpec&);
    ~ImageSpec();

    gfx::Size coded_size;

    gfx::ColorSpace color_space;

    uint64_t generation_id = 0;

    bool operator==(const ImageSpec&) const;
    bool operator!=(const ImageSpec&) const;
  };

  using ReleaseCB = base::OnceCallback<void(const gpu::SyncToken&)>;

  struct ImageRecord {
    ImageRecord();
    ImageRecord(ImageRecord&&);

    ImageRecord(const ImageRecord&) = delete;
    ImageRecord& operator=(const ImageRecord&) = delete;

    ~ImageRecord();

    gpu::Mailbox mailbox;

    // ClientSharedImage for the current shared image.
    scoped_refptr<gpu::ClientSharedImage> shared_image;

    ReleaseCB release_cb;

    scoped_refptr<CodecImageHolder> codec_image_holder;

    bool is_vulkan = false;
  };

  SharedImageVideoProvider() = default;

  SharedImageVideoProvider(const SharedImageVideoProvider&) = delete;
  SharedImageVideoProvider& operator=(const SharedImageVideoProvider&) = delete;

  virtual ~SharedImageVideoProvider() = default;

  using ImageReadyCB = base::OnceCallback<void(ImageRecord)>;

  virtual void Initialize(GpuInitCB gpu_init_cb) = 0;

  virtual void RequestImage(ImageReadyCB cb, const ImageSpec& spec) = 0;
};

}  // namespace media

#endif  // MEDIA_GPU_OHOS_SHARED_IMAGE_VIDEO_PROVIDER_H_
