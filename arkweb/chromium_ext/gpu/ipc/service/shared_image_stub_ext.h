// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "build/build_config.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "gpu/command_buffer/service/memory_tracking.h"
#include "gpu/command_buffer/service/sequence_id.h"
#include "gpu/command_buffer/service/sync_point_manager.h"
#include "gpu/ipc/common/command_buffer_id.h"
#include "gpu/ipc/common/gpu_channel.mojom.h"
#include "gpu/ipc/service/gpu_ipc_service_export.h"
#include "ui/gfx/gpu_extra_info.h"
#include "gpu/ipc/service/shared_image_stub.h"
#include "arkweb/chromium_ext/gpu/ipc/service/gpu_channel_shared_image_interface_ext.h",

namespace gpu {
class SharedImageStub;

class SharedImageStubExt : public SharedImageStub {
public:
  SharedImageStubExt(GpuChannel* channel, int32_t route_id);
  gpu::SharedImageStubExt* AsSharedImageStubExt() {
    return this;
  }

#if BUILDFLAG(ARKWEB_HEIF_SUPPORT)
  bool CreateSharedImage(const Mailbox& mailbox,
                         gfx::GpuMemoryBufferHandle handle,
                         gfx::BufferFormat format,
                         gfx::BufferPlane plane,
                         const gfx::Size& size,
                         const gfx::ColorSpace& color_space,
                         GrSurfaceOrigin surface_origin,
                         SkAlphaType alpha_type,
                         uint32_t usage,
                         void* window_buffer);
#endif  // BUILDFLAG(ARKWEB_HEIF_SUPPORT)

  const scoped_refptr<gpu::GpuChannelSharedImageInterfaceExt>&
  shared_image_interface_ext();

  scoped_refptr<GpuChannelSharedImageInterfaceExt>
      gpu_channel_shared_image_interface_ext_;
};

}