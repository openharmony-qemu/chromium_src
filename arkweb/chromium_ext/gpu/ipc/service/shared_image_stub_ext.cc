// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <inttypes.h>

#include <memory>
#include <utility>

#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/trace_event/trace_event.h"
#include "build/build_config.h"
#include "components/viz/common/resources/shared_image_format.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "gpu/command_buffer/service/scheduler.h"
#include "gpu/command_buffer/service/shared_image/shared_image_factory.h"
#include "gpu/command_buffer/service/shared_image/shared_image_representation.h"
#include "gpu/ipc/common/command_buffer_id.h"
#include "gpu/ipc/common/gpu_peak_memory.h"
#include "gpu/ipc/service/gpu_channel.h"
#include "gpu/ipc/service/gpu_channel_manager.h"
#include "gpu/ipc/service/gpu_channel_shared_image_interface.h"
#include "gpu/ipc/service/gpu_memory_buffer_factory.h"
#include "ui/gfx/buffer_format_util.h"
#include "ui/gfx/gpu_fence_handle.h"
#include "ui/gfx/gpu_memory_buffer.h"
#include "ui/gl/gl_context.h"
#include "arkweb/chromium_ext/gpu/ipc/service/shared_image_stub_ext.h"
#include "arkweb/chromium_ext/gpu/command_buffer/service/shared_image/shared_image_factory_ext.h"

namespace gpu {

SharedImageStubExt::SharedImageStubExt(GpuChannel* channel, int32_t route_id)
  : SharedImageStub(channel, route_id) {
}

#if BUILDFLAG(ARKWEB_HEIF_SUPPORT)
bool SharedImageStubExt::CreateSharedImage(const Mailbox& mailbox,
                                        gfx::GpuMemoryBufferHandle handle,
                                        gfx::BufferFormat format,
                                        gfx::BufferPlane plane,
                                        const gfx::Size& size,
                                        const gfx::ColorSpace& color_space,
                                        GrSurfaceOrigin surface_origin,
                                        SkAlphaType alpha_type,
                                        uint32_t usage,
                                        void* window_buffer) {
  TRACE_EVENT2("gpu", "SharedImageStub::CreateSharedImage", "width",
               size.width(), "height", size.height());
#if false
  if (!mailbox.IsSharedImage()) {
    LOG(ERROR) << "[HeifSupport] SharedImageStub: Trying to create a SharedImage with a "
                  "non-SharedImage mailbox.";
    OnError();
    return false;
  }
#endif
  if (!MakeContextCurrent()) {
    OnError();
    return false;
  }

  // if (!factory_->CreateSharedImage(
  //       params->mailbox, gpu::kNullSurfaceHandle,
  //       params->si_info->meta.format,

  //       params->si_info->meta.size,
  //       params->si_info->meta.color_space,params->si_info->meta.surface_origin,
  //       params->si_info->meta.alpha_type,
  //       params->si_info->meta.usage,
  //       GetLabel(params->si_info->debug_label))) {
  if (!factory_->AsSharedImageFactoryExt()->
        CreateSharedImage(mailbox, std::move(handle), format, plane,
                                   size, color_space, surface_origin,
                                   alpha_type, usage, window_buffer)) {
    LOG(ERROR)
        << "[HeifSupport] SharedImageStub: Unable to create shared image";
    OnError();
    return false;
  }
  return true;
}
#endif  // BUILDFLAG(ARKWEB_HEIF_SUPPORT)

const scoped_refptr<gpu::GpuChannelSharedImageInterfaceExt>&
SharedImageStubExt::shared_image_interface_ext() {
  gpu_channel_shared_image_interface_ext_ =
    base::MakeRefCounted<GpuChannelSharedImageInterfaceExt>(
        weak_factory_.GetWeakPtr());
  return gpu_channel_shared_image_interface_ext_;
}

}