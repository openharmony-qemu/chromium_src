// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/gpu/ipc/service/gpu_channel_shared_image_interface_ext.h"

#include "base/process/memory.h"
#include "base/synchronization/waitable_event.h"
#include "build/build_config.h"
#include "components/viz/common/resources/shared_image_format_utils.h"
#include "gpu/command_buffer/client/client_shared_image.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "gpu/command_buffer/common/sync_token.h"
#include "gpu/command_buffer/service/scheduler.h"
#include "gpu/command_buffer/service/shared_image/shared_image_factory.h"
#include "gpu/ipc/service/gpu_channel.h"
#include "gpu/ipc/service/gpu_channel_manager.h"
#include "ui/gfx/buffer_format_util.h"

#if BUILDFLAG(ARKWEB_MEDIA_CODEC)
#include "arkweb/chromium_ext/gpu/command_buffer/service/ohos/ohos_video_image_backing.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/stream_texture_shared_image_interface.h"
#endif
 
namespace gpu {

GpuChannelSharedImageInterfaceExt::GpuChannelSharedImageInterfaceExt(
      base::WeakPtr<SharedImageStub> shared_image_stub)
        : GpuChannelSharedImageInterface(shared_image_stub) {
}

#if BUILDFLAG(ARKWEB_MEDIA_CODEC)
scoped_refptr<ClientSharedImage>
GpuChannelSharedImageInterfaceExt::CreateSharedImageForOhosVideo(
    const gfx::Size& size,
    const gfx::ColorSpace& color_space,
    scoped_refptr<StreamTextureSharedImageInterface> image,
    scoped_refptr<RefCountedLock> drdc_lock,
    gl::ohos::TextureOwnerMode texture_owner_mode) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(gpu_sequence_checker_);
  if (!shared_image_stub_) {
    return nullptr;
  }

  auto mailbox = Mailbox::Generate();

  scoped_refptr<SharedContextState> shared_context =
      shared_image_stub_->shared_context_state();

  if (shared_context->context_lost()) {
    LOG(DEBUG) << "GpuChannelSharedImageInterface::"
                  "CreateSharedImageForOhosVideo context_lost";
    return nullptr;
  }

  auto shared_image_backing = OhosVideoImageBacking::Create(
      mailbox, size, color_space, kTopLeft_GrSurfaceOrigin, kPremul_SkAlphaType,
      /*debug_label=*/"DirectSIVideo", texture_owner_mode, std::move(image),
      std::move(shared_context), std::move(drdc_lock));
  SharedImageMetadata metadata{shared_image_backing->format(),
                               shared_image_backing->size(),
                               shared_image_backing->color_space(),
                               shared_image_backing->surface_origin(),
                               shared_image_backing->alpha_type(),
                               shared_image_backing->usage()};

  // Register it with shared image mailbox. This keeps |shared_image_backing|
  // around until its destruction cb is called.
  DCHECK(shared_image_stub_->channel()
             ->gpu_channel_manager()
             ->shared_image_manager());

  shared_image_stub_->factory()->RegisterBacking(
      std::move(shared_image_backing));
  return base::WrapRefCounted<ClientSharedImage>(
      new ClientSharedImage(mailbox, metadata, GenVerifiedSyncToken(), holder_,
                            GL_TEXTURE_EXTERNAL_OES));
}
#endif
}