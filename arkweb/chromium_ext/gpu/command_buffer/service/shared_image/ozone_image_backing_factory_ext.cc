// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <dawn/dawn_proc_table.h>
#include <dawn/native/DawnNative.h>

#include "base/logging.h"
#include "base/memory/scoped_refptr.h"
#include "build/build_config.h"
#include "build/chromecast_buildflags.h"
#include "build/chromeos_buildflags.h"
#include "components/viz/common/resources/shared_image_format_utils.h"
#include "gpu/command_buffer/common/gpu_memory_buffer_support.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "gpu/command_buffer/service/dawn_context_provider.h"
#include "gpu/command_buffer/service/service_utils.h"
#include "gpu/command_buffer/service/shared_image/ozone_image_backing.h"
#include "gpu/command_buffer/service/shared_image/shared_image_format_service_utils.h"
#include "gpu/command_buffer/service/shared_memory_region_wrapper.h"
#include "gpu/config/gpu_finch_features.h"
#include "ui/gfx/buffer_types.h"
#include "ui/gfx/gpu_memory_buffer.h"
#include "ui/gfx/native_pixmap.h"
#include "ui/gl/buildflags.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_fence.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_surface_egl.h"
#include "ui/ozone/public/ozone_platform.h"
#include "ui/ozone/public/surface_factory_ozone.h"
#include "arkweb/chromium_ext/gpu/command_buffer/service/shared_image/ozone_image_backing_factory_ext.h"
#include "arkweb/chromium_ext/gpu/command_buffer/common/gpu_memory_buffer_support_utils.h"

namespace gpu {
    
OzoneImageBackingFactoryExt::OzoneImageBackingFactoryExt(
    scoped_refptr<SharedContextState> shared_context_state,
    const GpuDriverBugWorkarounds& workarounds,
    const GpuPreferences& gpu_preferences)
    : OzoneImageBackingFactory(shared_context_state, workarounds),
    use_passthrough_(gpu_preferences.use_passthrough_cmd_decoder &&
                       gles2::PassthroughCommandDecoderSupported()) {
#if BUILDFLAG(USE_DAWN)
  dawn_procs_ = base::MakeRefCounted<base::RefCountedData<DawnProcTable>>(
      dawn::native::GetProcs());
#endif  // BUILDFLAG(USE_DAWN)
}

#if BUILDFLAG(ARKWEB_HEIF_SUPPORT)
std::unique_ptr<SharedImageBacking> OzoneImageBackingFactoryExt::CreateSharedImage(
    const Mailbox& mailbox,
    gfx::GpuMemoryBufferHandle handle,
    gfx::BufferFormat buffer_format,
    gfx::BufferPlane plane,
    const gfx::Size& size,
    const gfx::ColorSpace& color_space,
    GrSurfaceOrigin surface_origin,
    SkAlphaType alpha_type,
    uint32_t usage,
    void* window_buffer) {
  DCHECK_EQ(handle.type, gfx::NATIVE_PIXMAP);
  ui::SurfaceFactoryOzone* surface_factory =
      ui::OzonePlatform::GetInstance()->GetSurfaceFactoryOzone();
  scoped_refptr<gfx::NativePixmap> pixmap =
      surface_factory->CreateNativePixmapFromHandle(
          kNullSurfaceHandle, size, buffer_format,
          std::move(handle.native_pixmap_handle), window_buffer);
  if (!pixmap) {
    LOG(ERROR) << "[HeifSupport] OzoneImageBackingFactory::CreateSharedImage "
                  "pixmap is null.";
    return nullptr;
  }
  const gfx::Size plane_size = gpu::GetPlaneSize(plane, size);
  auto si_format = viz::GetSharedImageFormat(GetPlaneBufferFormat(plane, buffer_format));
#if false
  const viz::ResourceFormat plane_format =
      viz::GetResourceFormat(GetPlaneBufferFormat(plane, buffer_format));
#endif
  std::string debug_label = "ZGLEE";
  auto backing = std::make_unique<OzoneImageBacking>(
      mailbox, si_format, plane_size, color_space, surface_origin, alpha_type,
      SharedImageUsageSet(usage), std::move(debug_label), shared_context_state_,
      std::move(pixmap), workarounds_);
  backing->SetCleared();

  return backing;
}
#endif  // BUILDFLAG(ARKWEB_HEIF_SUPPORT)

}