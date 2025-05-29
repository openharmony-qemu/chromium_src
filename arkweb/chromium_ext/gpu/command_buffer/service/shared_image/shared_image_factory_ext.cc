// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <inttypes.h>

#include <memory>

#include "arkweb/build/features/features.h"
#include "base/containers/contains.h"
#include "base/metrics/histogram_functions.h"
#include "base/trace_event/memory_dump_manager.h"
#include "build/build_config.h"
#include "build/chromecast_buildflags.h"
#include "build/chromeos_buildflags.h"
#include "components/viz/common/resources/shared_image_format_utils.h"
#include "gpu/command_buffer/common/shared_image_trace_utils.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder.h"
#include "gpu/command_buffer/service/service_utils.h"
#include "gpu/command_buffer/service/shared_context_state.h"
#include "gpu/command_buffer/service/shared_image/compound_image_backing.h"
#include "gpu/command_buffer/service/shared_image/egl_image_backing_factory.h"
#include "gpu/command_buffer/service/shared_image/gl_texture_image_backing_factory.h"
#include "gpu/command_buffer/service/shared_image/raw_draw_image_backing_factory.h"
#include "gpu/command_buffer/service/shared_image/shared_image_backing.h"
#include "gpu/command_buffer/service/shared_image/shared_image_format_service_utils.h"
#include "gpu/command_buffer/service/shared_image/shared_image_manager.h"
#include "gpu/command_buffer/service/shared_image/shared_image_representation.h"
#include "gpu/command_buffer/service/shared_image/shared_memory_image_backing_factory.h"
#include "gpu/command_buffer/service/shared_image/wrapped_sk_image_backing_factory.h"
#include "gpu/config/gpu_preferences.h"
#include "gpu/ipc/common/gpu_memory_buffer_impl_shared_memory.h"
#include "ui/base/ozone_buildflags.h"
#include "ui/base/ui_base_features.h"
#include "ui/gfx/buffer_format_util.h"
#include "ui/gfx/gpu_memory_buffer.h"
#include "ui/gl/gl_display.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_switches.h"
#include "ui/gl/gl_utils.h"
#include "ui/gl/gl_version_info.h"
#include "ui/gl/trace_util.h"

#include "arkweb/chromium_ext/gpu/command_buffer/service/shared_image/shared_image_factory_ext.h"

namespace gpu {

const char* GmbTypeToString(gfx::GpuMemoryBufferType type) {
  switch (type) {
    case gfx::EMPTY_BUFFER:
      return "empty";
    case gfx::SHARED_MEMORY_BUFFER:
      return "shared_memory";
    case gfx::IO_SURFACE_BUFFER:
    case gfx::NATIVE_PIXMAP:
    case gfx::DXGI_SHARED_HANDLE:
    case gfx::ANDROID_HARDWARE_BUFFER:
    case gfx::OHOS_NATIVE_BUFFER:
      return "platform";
  }
  NOTREACHED();
}

SharedImageFactoryExt::SharedImageFactoryExt(
    const GpuPreferences& gpu_preferences,
    const GpuDriverBugWorkarounds& workarounds,
    const GpuFeatureInfo& gpu_feature_info,
    SharedContextState* context_state,
    SharedImageManager* manager,
    MemoryTracker* tracker,
    bool is_for_display_compositor)
    : SharedImageFactory(gpu_preferences, workarounds,
    gpu_feature_info, context_state, manager, tracker,
    is_for_display_compositor) {
}

#if BUILDFLAG(ARKWEB_HEIF_SUPPORT)
bool SharedImageFactoryExt::CreateSharedImage(const Mailbox& mailbox,
    gfx::GpuMemoryBufferHandle handle,
    gfx::BufferFormat format,
    gfx::BufferPlane plane,
    const gfx::Size& size,
    const gfx::ColorSpace& color_space,
    GrSurfaceOrigin surface_origin,
    SkAlphaType alpha_type,
    uint32_t usage,
    void* window_buffer) {
#if false
  auto si_format =
      viz::SharedImageFormat::SinglePlane(viz::GetResourceFormat(format));

#endif
  auto si_format = viz::GetSharedImageFormat(format);
  gfx::GpuMemoryBufferType gmb_type = handle.type;

  SharedImageUsageSet usage_set(usage);

  auto* factory = GetFactoryByUsage(usage_set, si_format, size,
                                    /*pixel_data=*/{}, gmb_type);
  std::string debug_label = "ZGLEE";
  if (!factory) {
    LogGetFactoryFailed(usage_set, si_format, gmb_type, debug_label);
    return false;
  }

  std::unique_ptr<SharedImageBacking> backing;
  backing = factory->CreateSharedImage(mailbox, std::move(handle), format,
                                       plane, size, color_space, surface_origin,
                                       alpha_type, usage, window_buffer);
  if (backing) {
    LOG(DEBUG) << "[HeifSupport] CreateSharedImage[" << backing->GetName()
               << "] from handle size=" << size.ToString()
               << " usage=" << CreateLabelForSharedImageUsage(usage_set)
               << " buffer_format=" << gfx::BufferFormatToString(format)
               << " gmb_type=" << GmbTypeToString(gmb_type);
    backing->OnWriteSucceeded();
  }

  return RegisterBacking(std::move(backing));
}
#endif  // BUILDFLAG(ARKWEB_HEIF_SUPPORT)

}
