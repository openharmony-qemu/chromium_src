// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/shared_image/shared_image_backing_factory.h"
#include "gpu/command_buffer/service/shared_image/shared_image_backing.h"

#if BUILDFLAG(IS_OHOS)
#include "base/logging.h"
#endif

namespace gpu {
#if BUILDFLAG(IS_OHOS)
const char* SharedImageUsageToString(SharedImageUsage usage) {
    switch (usage) {
        case SHARED_IMAGE_USAGE_GLES2: return "SHARED_IMAGE_USAGE_GLES2";
        case SHARED_IMAGE_USAGE_GLES2_FRAMEBUFFER_HINT: return "SHARED_IMAGE_USAGE_GLES2_FRAMEBUFFER_HINT";
        case SHARED_IMAGE_USAGE_RASTER: return "SHARED_IMAGE_USAGE_RASTER";
        case SHARED_IMAGE_USAGE_DISPLAY_READ: return "SHARED_IMAGE_USAGE_DISPLAY_READ";
        case SHARED_IMAGE_USAGE_DISPLAY_WRITE: return "SHARED_IMAGE_USAGE_DISPLAY_WRITE";
        case SHARED_IMAGE_USAGE_SCANOUT: return "SHARED_IMAGE_USAGE_SCANOUT";
        case SHARED_IMAGE_USAGE_OOP_RASTERIZATION: return "SHARED_IMAGE_USAGE_OOP_RASTERIZATION";
        case SHARED_IMAGE_USAGE_WEBGPU: return "SHARED_IMAGE_USAGE_WEBGPU";
        case SHARED_IMAGE_USAGE_CONCURRENT_READ_WRITE: return "SHARED_IMAGE_USAGE_CONCURRENT_READ_WRITE";
        case SHARED_IMAGE_USAGE_VIDEO_DECODE: return "SHARED_IMAGE_USAGE_VIDEO_DECODE";
        case SHARED_IMAGE_USAGE_WEBGPU_SWAP_CHAIN_TEXTURE: return "SHARED_IMAGE_USAGE_WEBGPU_SWAP_CHAIN_TEXTURE";
        case SHARED_IMAGE_USAGE_MACOS_VIDEO_TOOLBOX: return "SHARED_IMAGE_USAGE_MACOS_VIDEO_TOOLBOX";
        case SHARED_IMAGE_USAGE_MIPMAP: return "SHARED_IMAGE_USAGE_MIPMAP";
        case SHARED_IMAGE_USAGE_CPU_WRITE: return "SHARED_IMAGE_USAGE_CPU_WRITE";
        case SHARED_IMAGE_USAGE_RAW_DRAW: return "SHARED_IMAGE_USAGE_RAW_DRAW";
        case SHARED_IMAGE_USAGE_RASTER_DELEGATED_COMPOSITING: return "SHARED_IMAGE_USAGE_RASTER_DELEGATED_COMPOSITING";
        case SHARED_IMAGE_USAGE_HIGH_PERFORMANCE_GPU: return "SHARED_IMAGE_USAGE_HIGH_PERFORMANCE_GPU";
        case SHARED_IMAGE_USAGE_SCANOUT_DCOMP_SURFACE: return "SHARED_IMAGE_USAGE_SCANOUT_DCOMP_SURFACE";
        case SHARED_IMAGE_USAGE_WEBGPU_STORAGE_TEXTURE: return "SHARED_IMAGE_USAGE_WEBGPU_STORAGE_TEXTURE";
        case SHARED_IMAGE_USAGE_CPU_UPLOAD: return "SHARED_IMAGE_USAGE_CPU_UPLOAD";
        default: return "Unknown Usage";
    }
}
#endif

SharedImageBackingFactory::SharedImageBackingFactory(uint32_t valid_usages)
  : invalid_usages_(~valid_usages) {
#if BUILDFLAG(IS_OHOS)
  uint32_t usage = 1;
  while (usage <= LAST_SHARED_IMAGE_USAGE) {
      if (valid_usages & usage) {
          LOG(DEBUG) << __FUNCTION__ << " HWIMGDEC - Supported SharedImageBackingFactory with usage: "
                      << SharedImageUsageToString(static_cast<SharedImageUsage>(usage))
                      << " (" << usage << ")";
      }
      usage <<= 1; // Shift to the next enum value
  }
#endif
}

SharedImageBackingFactory::~SharedImageBackingFactory() = default;

std::unique_ptr<SharedImageBacking>
SharedImageBackingFactory::CreateSharedImage(
    const Mailbox& mailbox,
    viz::SharedImageFormat format,
    const gfx::Size& size,
    const gfx::ColorSpace& color_space,
    GrSurfaceOrigin surface_origin,
    SkAlphaType alpha_type,
    uint32_t usage,
    std::string debug_label,
    gfx::GpuMemoryBufferHandle handle) {
  return nullptr;
}

base::WeakPtr<SharedImageBackingFactory>
SharedImageBackingFactory::GetWeakPtr() {
  return weak_ptr_factory_.GetWeakPtr();
}

bool SharedImageBackingFactory::CanCreateSharedImage(
    uint32_t usage,
    viz::SharedImageFormat format,
    const gfx::Size& size,
    bool thread_safe,
    gfx::GpuMemoryBufferType gmb_type,
    GrContextType gr_context_type,
    base::span<const uint8_t> pixel_data) {
  if (invalid_usages_ & usage) {
    // This factory doesn't support all the usages.
    return false;
  }

  return IsSupported(usage, format, size, thread_safe, gmb_type,
                     gr_context_type, pixel_data);
}

void SharedImageBackingFactory::InvalidateWeakPtrsForTesting() {
  weak_ptr_factory_.InvalidateWeakPtrs();
}

}  // namespace gpu
