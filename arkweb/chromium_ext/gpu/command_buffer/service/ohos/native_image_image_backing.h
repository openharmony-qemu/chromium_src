// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on shared_image_video_surface_texture.h originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 
 
#ifndef GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_IMAGE_IMAGE_BACKING_H_
#define GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_IMAGE_IMAGE_BACKING_H_

#include <memory>

#include "base/memory/scoped_refptr.h"
#include "base/synchronization/waitable_event.h"
#include "base/task/single_thread_task_runner.h"
#include "gpu/command_buffer/service/ohos/ohos_image_backing.h"
#include "gpu/command_buffer/service/ohos/ohos_video_image_backing.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/shared_context_state.h"
#include "gpu/command_buffer/service/stream_texture_shared_image_interface.h"
#include "gpu/gpu_gles2_export.h"

namespace gpu {
struct Mailbox;

class GPU_GLES2_EXPORT NativeImageImageBacking
    : public OhosVideoImageBacking,
      public SharedContextState::ContextLostObserver {
 public:
  NativeImageImageBacking(
      const Mailbox& mailbox,
      const gfx::Size& size,
      const gfx::ColorSpace color_space,
      GrSurfaceOrigin surface_origin,
      SkAlphaType alpha_type,
      std::string debug_label,
      scoped_refptr<StreamTextureSharedImageInterface> stream_texture_sii,
      scoped_refptr<SharedContextState> shared_context_state);

  ~NativeImageImageBacking() override;

  NativeImageImageBacking(const NativeImageImageBacking&) = delete;
  NativeImageImageBacking& operator=(const NativeImageImageBacking&) = delete;

  size_t GetEstimatedSizeForMemoryDump() const override;

  void OnContextLost() override;

 protected:
  std::unique_ptr<GLTextureImageRepresentation> ProduceGLTexture(
      SharedImageManager* manager,
      MemoryTypeTracker* tracker) override;

  std::unique_ptr<GLTexturePassthroughImageRepresentation>
  ProduceGLTexturePassthrough(SharedImageManager* manager,
                              MemoryTypeTracker* tracker) override;

  std::unique_ptr<SkiaGaneshImageRepresentation> ProduceSkiaGanesh(
      SharedImageManager* manager,
      MemoryTypeTracker* tracker,
      scoped_refptr<SharedContextState> context_state) override;

  //   std::unique_ptr<gpu::LegacyOverlayImageRepresentation>
  //   ProduceLegacyOverlay(
  //       gpu::SharedImageManager* manager,
  //       gpu::MemoryTypeTracker* tracker);

 private:
  class SharedImageRepresentationGLTextureVideo;
  class SharedImageRepresentationGLTexturePassthroughVideo;
  class SharedImageRepresentationOverlayVideo;

  void BeginGLReadAccess(const GLuint service_id);

  scoped_refptr<StreamTextureSharedImageInterface> stream_texture_sii_;
  scoped_refptr<SharedContextState> context_state_;
  scoped_refptr<base::SingleThreadTaskRunner> gpu_main_task_runner_;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_IMAGE_IMAGE_BACKING_H_
