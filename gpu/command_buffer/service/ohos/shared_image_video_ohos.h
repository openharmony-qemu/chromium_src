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

#ifndef GPU_COMMAND_BUFFER_SERVICE_OHOS_SHARED_IMAGE_VIDEO_OHOS_H_
#define GPU_COMMAND_BUFFER_SERVICE_OHOS_SHARED_IMAGE_VIDEO_OHOS_H_

#include <memory>

#include "gpu/command_buffer/service/ohos/shared_image_backing_ohos.h"
#include "gpu/gpu_gles2_export.h"
#include "gpu/ipc/common/vulkan_ycbcr_info.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace gpu {
struct Mailbox;
class AbstractTextureOHOS;
class RefCountedLock;
class StreamTextureSharedImageInterface;
class SharedContextState;
class NativeImageTextureOwner;

class GPU_GLES2_EXPORT SharedImageVideoOhos : public SharedImageBackingOhos {
 public:
  static std::unique_ptr<SharedImageVideoOhos> Create(
      const Mailbox& mailbox,
      const gfx::Size& size,
      const gfx::ColorSpace color_space,
      GrSurfaceOrigin surface_origin,
      SkAlphaType alpha_type,
      scoped_refptr<StreamTextureSharedImageInterface> stream_texture_sii,
      scoped_refptr<SharedContextState> context_state);

  ~SharedImageVideoOhos() override;

  SharedImageVideoOhos(const SharedImageVideoOhos&) = delete;
  SharedImageVideoOhos& operator=(const SharedImageVideoOhos&) = delete;

  SharedImageBackingType GetType() const override;
  gfx::Rect ClearedRect() const override;
  void SetClearedRect(const gfx::Rect& cleared_rect) override;
  void Update(std::unique_ptr<gfx::GpuFence> in_fence) override;

 protected:
  SharedImageVideoOhos(const Mailbox& mailbox,
                       const gfx::Size& size,
                       const gfx::ColorSpace color_space,
                       GrSurfaceOrigin surface_origin,
                       SkAlphaType alpha_type,
                       bool is_thread_safe);

  std::unique_ptr<AbstractTextureOHOS> GenAbstractTexture(
      const bool passthrough);
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_OHOS_SHARED_IMAGE_VIDEO_OHOS_H_
