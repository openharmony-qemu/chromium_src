/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GPU_COMMAND_BUFFER_SERVICE_OHOS_OHOS_VIDEO_IMAGE_BACKING_H_
#define GPU_COMMAND_BUFFER_SERVICE_OHOS_OHOS_VIDEO_IMAGE_BACKING_H_

#include <memory>

#include "gpu/command_buffer/service/ohos/ohos_image_backing.h"

#include "gpu/gpu_gles2_export.h"
#include "gpu/ipc/common/vulkan_ycbcr_info.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/gl/ohos/native_buffer_utils.h"

namespace gpu {
struct Mailbox;
class AbstractTextureOHOS;
class RefCountedLock;
class StreamTextureSharedImageInterface;
class SharedContextState;
class ScopedNativeBufferFenceSync;

class GPU_GLES2_EXPORT OhosVideoImageBacking : public OhosImageBacking {
 public:
  static std::unique_ptr<OhosVideoImageBacking> Create(
      const Mailbox& mailbox,
      const gfx::Size& size,
      const gfx::ColorSpace color_space,
      GrSurfaceOrigin surface_origin,
      SkAlphaType alpha_type,
      gl::ohos::TextureOwnerMode texture_owner_mode,
      scoped_refptr<StreamTextureSharedImageInterface> stream_texture_sii,
      scoped_refptr<SharedContextState> context_state,
      scoped_refptr<RefCountedLock> drdc_lock);

  ~OhosVideoImageBacking() override;

  OhosVideoImageBacking(const OhosVideoImageBacking&) = delete;
  OhosVideoImageBacking& operator=(const OhosVideoImageBacking&) = delete;

  SharedImageBackingType GetType() const override;
  gfx::Rect ClearedRect() const override;
  void SetClearedRect(const gfx::Rect& cleared_rect) override;
  void Update(std::unique_ptr<gfx::GpuFence> in_fence) override;

 protected:
  OhosVideoImageBacking(const Mailbox& mailbox,
                       const gfx::Size& size,
                       const gfx::ColorSpace color_space,
                       GrSurfaceOrigin surface_origin,
                       SkAlphaType alpha_type,
                       bool is_thread_safe);

  std::unique_ptr<AbstractTextureOHOS> GenAbstractTexture(
      const bool passthrough);
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_OHOS_OHOS_VIDEO_IMAGE_BACKING_H_
