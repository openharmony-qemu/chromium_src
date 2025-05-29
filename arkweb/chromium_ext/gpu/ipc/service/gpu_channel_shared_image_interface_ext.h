// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/ipc/service/gpu_channel_shared_image_interface.h"

#include "base/memory/raw_ptr.h"
#include "build/build_config.h"
#include "gpu/command_buffer/client/client_shared_image.h"
#include "gpu/command_buffer/client/shared_image_interface.h"
#include "gpu/command_buffer/common/command_buffer_id.h"
#include "gpu/command_buffer/common/shared_image_capabilities.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "gpu/command_buffer/service/sequence_id.h"
#include "gpu/gpu_gles2_export.h"
#include "gpu/ipc/common/gpu_memory_buffer_handle_info.h"
#include "gpu/ipc/service/shared_image_stub.h"
#include "ui/gfx/gpu_memory_buffer.h"
#include "gpu/ipc/service/gpu_ipc_service_export.h"

#if BUILDFLAG(ARKWEB_MEDIA_CODEC)
#include "arkweb/chromium_ext/ui/gl/ohos/native_buffer_utils.h"
#endif

namespace gpu {
class GpuChannelSharedImageInterface;

class GpuChannelSharedImageInterfaceExt : public GpuChannelSharedImageInterface {
public:

  explicit GpuChannelSharedImageInterfaceExt(base::WeakPtr<SharedImageStub> shared_image_stub);

  gpu::GpuChannelSharedImageInterfaceExt* AsGpuChannelSharedImageInterfaceExt() {
    return this;
  }

#if BUILDFLAG(ARKWEB_MEDIA_CODEC)
  scoped_refptr<ClientSharedImage> CreateSharedImageForOhosVideo(
      const gfx::Size& size,
      const gfx::ColorSpace& color_space,
      scoped_refptr<StreamTextureSharedImageInterface> image,
      scoped_refptr<RefCountedLock> drdc_lock,
      gl::ohos::TextureOwnerMode texture_owner_mode);
#endif
};

}