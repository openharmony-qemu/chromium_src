// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/ohos/ohos_video_image_backing.h"

#include "base/ohos/scoped_native_buffer_handle.h"
#include "components/viz/common/gpu/vulkan_context_provider.h"
#include "components/viz/common/resources/resource_sizes.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "gpu/command_buffer/service/abstract_texture_ohos.h"
#include "gpu/command_buffer/service/ohos/hw_video_native_buffer_image_backing.h"
#include "gpu/command_buffer/service/ohos/native_image_image_backing.h"
#include "gpu/command_buffer/service/ohos/native_image_texture_owner.h"
#include "gpu/command_buffer/service/ohos/same_layer_native_buffer_image_backing.h"
#include "gpu/command_buffer/service/ohos/scoped_native_buffer_fence_sync.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/shared_context_state.h"
#include "gpu/config/gpu_finch_features.h"
#include "gpu/vulkan/vulkan_device_queue.h"
#include "gpu/vulkan/vulkan_implementation.h"

namespace gpu {

OhosVideoImageBacking::OhosVideoImageBacking(const Mailbox& mailbox,
                                             const gfx::Size& size,
                                             const gfx::ColorSpace color_space,
                                             GrSurfaceOrigin surface_origin,
                                             SkAlphaType alpha_type,
                                             std::string debug_label,
                                             bool is_thread_safe)
    : OhosImageBacking(
          mailbox,
          viz::SinglePlaneFormat::kRGBA_8888,
          size,
          color_space,
          surface_origin,
          alpha_type,
          (SHARED_IMAGE_USAGE_DISPLAY_READ | SHARED_IMAGE_USAGE_GLES2_READ),
          std::move(debug_label),
          viz::SinglePlaneFormat::kRGBA_8888.EstimatedSizeInBytes(size),
          is_thread_safe,
          base::ScopedFD()) {}

OhosVideoImageBacking::~OhosVideoImageBacking() {}

// Static.
std::unique_ptr<OhosVideoImageBacking> OhosVideoImageBacking::Create(
    const Mailbox& mailbox,
    const gfx::Size& size,
    const gfx::ColorSpace color_space,
    GrSurfaceOrigin surface_origin,
    SkAlphaType alpha_type,
    std::string debug_label,
    gl::ohos::TextureOwnerMode texture_owner_mode,
    scoped_refptr<StreamTextureSharedImageInterface> stream_texture_sii,
    scoped_refptr<SharedContextState> context_state,
    scoped_refptr<RefCountedLock> drdc_lock) {
  if (texture_owner_mode ==
      gl::ohos::TextureOwnerMode::kSameLayerNativeBuffer) {
    return std::make_unique<SameLayerNativeBufferImageBacking>(
        mailbox, size, color_space, surface_origin, alpha_type,
        std::move(debug_label), std::move(stream_texture_sii),
        std::move(context_state), std::move(drdc_lock));
  }
  if (texture_owner_mode ==
      gl::ohos::TextureOwnerMode::kHwVideoZeroCopyNativeBuffer) {
    return std::make_unique<HwVideoNativeBufferImageBacking>(
        mailbox, size, color_space, surface_origin, alpha_type,
        std::move(debug_label), std::move(stream_texture_sii),
        std::move(context_state), std::move(drdc_lock));
  }

  return std::make_unique<NativeImageImageBacking>(
      mailbox, size, color_space, surface_origin, alpha_type,
      std::move(debug_label), std::move(stream_texture_sii),
      std::move(context_state));
}

std::unique_ptr<AbstractTextureOHOS> OhosVideoImageBacking::GenAbstractTexture(
    const bool passthrough) {
  if (passthrough) {
    return AbstractTextureOHOS::CreateForPassthrough(size());
  } else {
    return AbstractTextureOHOS::CreateForValidating(size());
  }
}

SharedImageBackingType OhosVideoImageBacking::GetType() const {
  return SharedImageBackingType::kVideo;
}

gfx::Rect OhosVideoImageBacking::ClearedRect() const {
  return gfx::Rect(size());
}

void OhosVideoImageBacking::SetClearedRect(const gfx::Rect& cleared_rect) {}

void OhosVideoImageBacking::Update(std::unique_ptr<gfx::GpuFence> in_fence) {
  DCHECK(!in_fence);
}

}  // namespace gpu
