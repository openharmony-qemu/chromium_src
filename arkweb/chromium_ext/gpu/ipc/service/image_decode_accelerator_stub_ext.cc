// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stddef.h>

#include <algorithm>
#include <new>
#include <optional>
#include <utility>
#include <vector>

#include "base/containers/span.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/numerics/checked_math.h"
#include "base/numerics/safe_conversions.h"
#include "base/task/single_thread_task_runner.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "components/viz/common/resources/shared_image_format_utils.h"
#include "gpu/command_buffer/common/constants.h"
#include "gpu/command_buffer/common/context_result.h"
#include "gpu/command_buffer/common/discardable_handle.h"
#include "gpu/command_buffer/common/scheduling_priority.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "gpu/command_buffer/common/sync_token.h"
#include "gpu/command_buffer/service/context_group.h"
#include "gpu/command_buffer/service/decoder_context.h"
#include "gpu/command_buffer/service/gr_shader_cache.h"
#include "gpu/command_buffer/service/scheduler.h"
#include "gpu/command_buffer/service/service_transfer_cache.h"
#include "gpu/command_buffer/service/shared_context_state.h"
#include "gpu/command_buffer/service/shared_image/shared_image_factory.h"
#include "gpu/command_buffer/service/shared_image/shared_image_representation.h"
#include "gpu/command_buffer/service/task_graph.h"
#include "gpu/config/gpu_finch_features.h"
#include "gpu/ipc/common/surface_handle.h"
#include "gpu/ipc/service/command_buffer_stub.h"
#include "gpu/ipc/service/gpu_channel.h"
#include "gpu/ipc/service/gpu_channel_manager.h"
#include "gpu/ipc/service/shared_image_stub.h"
#include "third_party/abseil-cpp/absl/cleanup/cleanup.h"
#include "third_party/skia/include/core/SkColorSpace.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkImageInfo.h"
#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/include/gpu/ganesh/GrBackendSemaphore.h"
#include "third_party/skia/include/gpu/ganesh/GrBackendSurface.h"
#include "third_party/skia/include/gpu/ganesh/GrTypes.h"
#include "ui/gfx/buffer_format_util.h"
#include "ui/gfx/buffer_types.h"
#include "ui/gfx/color_space.h"
#include "ui/gfx/gpu_memory_buffer.h"
#include "arkweb/chromium_ext/gpu/ipc/service/shared_image_stub_ext.h"
#include "arkweb/chromium_ext/gpu/ipc/service/image_decode_accelerator_stub_ext.h"

namespace gpu {

struct CleanUpContext {
  scoped_refptr<base::SingleThreadTaskRunner> main_task_runner_;
  raw_ptr<SharedContextState> shared_context_state_ = nullptr;
  std::unique_ptr<SkiaImageRepresentation> skia_representation_;
  std::unique_ptr<SkiaImageRepresentation::ScopedReadAccess>
      skia_scoped_access_;
  size_t num_callbacks_pending_;
  CleanUpContext(scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
                 raw_ptr<SharedContextState> shared_context_state,
                 std::unique_ptr<SkiaImageRepresentation> skia_representation,
                 std::unique_ptr<SkiaImageRepresentation::ScopedReadAccess>
                     skia_scoped_access)
      : main_task_runner_(main_task_runner),
        shared_context_state_(shared_context_state),
        skia_representation_(std::move(skia_representation)),
        skia_scoped_access_(std::move(skia_scoped_access)),
        num_callbacks_pending_(skia_representation_->NumPlanesExpected()) {}
};

#if BUILDFLAG(ARKWEB_HEIF_SUPPORT)
void CleanUpResource(SkImages::ReleaseContext context) {
  auto* clean_up_context = static_cast<CleanUpContext*>(context);
  DCHECK(clean_up_context->main_task_runner_->BelongsToCurrentThread());

  // The context should be current as we set it to be current earlier, and this
  // call is coming from Skia itself.
  DCHECK(
      clean_up_context->shared_context_state_->IsCurrent(/*surface=*/nullptr));

  clean_up_context->skia_scoped_access_->ApplyBackendSurfaceEndState();
  delete clean_up_context;
}
#endif

ImageDecodeAcceleratorStubExt::ImageDecodeAcceleratorStubExt(
    ImageDecodeAcceleratorWorker* worker,
    GpuChannel* channel, int32_t route_id)
    : ImageDecodeAcceleratorStub(worker, channel, route_id) {

}

ImageDecodeAcceleratorStubExt::~ImageDecodeAcceleratorStubExt() {

}

#if BUILDFLAG(ARKWEB_HEIF_SUPPORT)
bool ImageDecodeAcceleratorStubExt::ProcessCompletedDecodeExt(mojom::ScheduleImageDecodeParams& params,
                                scoped_refptr<SharedContextState> shared_context_state,
                                std::vector<sk_sp<SkImage>> plane_sk_images,
                                std::optional<base::ScopedClosureRunner>& notify_gl_state_changed,
                                std::unique_ptr<ImageDecodeAcceleratorWorker::DecodeResult>& completed_decode) {
  // We should notify the SharedContextState that we or Skia may have modified
  // the driver's GL state. We put this in a ScopedClosureRunner so that if we
  // return early, the SharedContextState ends up in a consistent state.
  // TODO(blundell): Determine whether this is still necessary after the
  // transition to SharedImage.
  notify_gl_state_changed.emplace(base::BindOnce(
      [](scoped_refptr<SharedContextState> scs) {
        scs->set_need_context_state_reset(true);
      },
      shared_context_state));

  // Create an SkImage for each plane.
  const size_t num_planes =
      completed_decode->handle.native_pixmap_handle.planes.size();
  plane_sk_images.resize(num_planes);
  for (size_t plane = 0u; plane < num_planes; plane++) {
    gfx::Size plane_size = params.output_size;
    // Extract the plane out of |completed_decode->handle| and put it in its own
    // gfx::GpuMemoryBufferHandle so that we can create a SharedImage for the
    // plane.
    gfx::GpuMemoryBufferHandle plane_handle;
    plane_handle.type = completed_decode->handle.type;
    plane_handle.native_pixmap_handle.planes.push_back(
        std::move(completed_decode->handle.native_pixmap_handle.planes[plane]));

    // TODO: Right now, we only support RGBA8888 for the output of the decoder,
    // We need to support NV12 next.
    const auto plane_format = gfx::BufferFormat::RGBA_8888;

    // NOTE: The SurfaceHandle would typically be used to know what gpu adapter
    // the buffer belongs to, but here we already have the buffer handle, so it
    // should be OK to pass a null SurfaceHandle (it's not clear what
    // SurfaceHandle was used to create the original buffers).
    gpu::Mailbox mailbox = gpu::Mailbox::Generate();
    if (!channel_->shared_image_stub()->AsSharedImageStubExt()->CreateSharedImage(
            mailbox, std::move(plane_handle), plane_format,
            gfx::BufferPlane::DEFAULT, plane_size, gfx::ColorSpace(),
            kTopLeft_GrSurfaceOrigin, kOpaque_SkAlphaType,
            SHARED_IMAGE_USAGE_OOP_RASTERIZATION,  // todo:
                                                   // SHARED_IMAGE_USAGE_RASTER
                                                   // |
            completed_decode->window_buffer)) {
      LOG(ERROR) << "[HeifSupport] Could not create SharedImage";
      return true;
    }

    // Create the SkiaRepresentation::ScopedReadAccess from the SharedImage.
    // There is a need to be careful here as the SkiaRepresentation can outlive
    // the channel: the representation is effectively owned by the transfer
    // cache, which is owned by SharedContextState, which is destroyed by
    // GpuChannelManager *after* GpuChannelManager destroys the channels. Hence,
    // we cannot supply the channel's SharedImageStub as a MemoryTracker to
    // create a SharedImageRepresentationFactory here (the factory creates a
    // MemoryTypeTracker instance backed by that MemoryTracker that needs to
    // outlive the representation). Instead, we create the Skia representation
    // directly using the SharedContextState's MemoryTypeTracker instance.
    std::unique_ptr<SkiaImageRepresentation> skia_representation =
        channel_->gpu_channel_manager()->shared_image_manager()->ProduceSkia(
            mailbox, shared_context_state->memory_type_tracker(),
            shared_context_state);

    // Note that per the above reasoning, we have to make sure that the factory
    // representation doesn't outlive the channel (since it *was* created via
    // the channel). We can destroy it now that the skia representation is
    // alive.
    channel_->shared_image_stub()->factory()->DestroySharedImage(mailbox);
    if (!skia_representation) {
      DLOG(ERROR) << "Could not create a SkiaImageRepresentation";
      return true;
    }

    std::vector<GrBackendSemaphore> begin_semaphores;
    std::vector<GrBackendSemaphore> end_semaphores;
    auto skia_scoped_access = skia_representation->BeginScopedReadAccess(
        &begin_semaphores, &end_semaphores);
    if (!skia_scoped_access) {
      LOG(ERROR) << "[HeifSupport] Could not get scoped access to "
                    "SkiaImageRepresentation";
      return true;
    }

    // As this SharedImage has just been created, there should not be any
    // semaphores.
    DCHECK(begin_semaphores.empty());
    DCHECK(end_semaphores.empty());

    // Create the SkImage, handing over lifetime management of the
    // skia image representation and scoped access.
    CleanUpContext* resource = new CleanUpContext(
        channel_->task_runner(), shared_context_state.get(),
        std::move(skia_representation), std::move(skia_scoped_access));
    plane_sk_images[plane] = resource->skia_scoped_access_->CreateSkImage(
        shared_context_state.get(), CleanUpResource, resource);
    if (!plane_sk_images[plane]) {
      LOG(ERROR) << "[HeifSupport] Could not create planar SkImage";
      return true;
    }
  }

  // Insert the cache entry in the transfer cache. Note that this section
  // validates several of the IPC parameters: |params.raster_decoder_route_id|,
  // |params.transfer_cache_entry_id|, |params.discardable_handle_shm_id|, and
  // |params.discardable_handle_shm_offset|.
  CommandBufferStub* command_buffer =
      channel_->LookupCommandBuffer(params.raster_decoder_route_id);
  if (!command_buffer) {
    LOG(ERROR) << "[HeifSupport] Could not find the command buffer";
    return true;
  }
  scoped_refptr<Buffer> handle_buffer =
      command_buffer->GetTransferBuffer(params.discardable_handle_shm_id);
  if (!DiscardableHandleBase::ValidateParameters(
          handle_buffer.get(), params.discardable_handle_shm_offset)) {
    LOG(ERROR)
        << "[HeifSupport] Could not validate the discardable handle parameters";
    return true;
  }
  DCHECK(command_buffer->decoder_context());
  if (command_buffer->decoder_context()->GetRasterDecoderId() < 0) {
    LOG(ERROR) << "[HeifSupport] Could not get the raster decoder ID";
    return true;
  }

  {
    auto* gr_shader_cache = channel_->gpu_channel_manager()->gr_shader_cache();
    // absl::optional<raster::GrShaderCache::ScopedCacheUse> cache_use;
    std::optional<raster::GrShaderCache::ScopedCacheUse> cache_use;
    if (gr_shader_cache) {
      cache_use.emplace(gr_shader_cache,
                        base::strict_cast<int32_t>(channel_->client_id()));
    }
    DCHECK(shared_context_state->transfer_cache());

    if (!shared_context_state->transfer_cache()->AsServiceTransferCacheExt()
             ->CreateLockedRGBAHardwareDecodedImageEntry(
                 command_buffer->decoder_context()->GetRasterDecoderId(),
                 params.transfer_cache_entry_id,
                 ServiceDiscardableHandle(std::move(handle_buffer),
                                          params.discardable_handle_shm_offset,
                                          params.discardable_handle_shm_id),
                 shared_context_state->gr_context(), std::move(plane_sk_images),
                 completed_decode->buffer_byte_size)) {
      LOG(ERROR) << "[HeifSupport] Could not create and insert the transfer "
                    "cache entry";
      return true;
    }
  }
  DCHECK(notify_gl_state_changed);
  notify_gl_state_changed->RunAndReset();
  return false;
}

void ImageDecodeAcceleratorStubExt::ReleasePixmapData(
    base::WaitableEvent* finish_event) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  lock_.AssertAcquired();
  worker_->ReleaseDecodedPixelMap();
  if (finish_event) {
    finish_event->Signal();
  }
}
#endif
}