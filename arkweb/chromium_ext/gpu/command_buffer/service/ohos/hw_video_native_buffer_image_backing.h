// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on shared_image_video_image_reader.h originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 
 
#ifndef GPU_COMMAND_BUFFER_SERVICE_OHOS_HW_VIDEO_NATIVE_BUFFER_IMAGE_BACKING_H_
#define GPU_COMMAND_BUFFER_SERVICE_OHOS_HW_VIDEO_NATIVE_BUFFER_IMAGE_BACKING_H_

#include "base/memory/scoped_refptr.h"
#include "base/task/single_thread_task_runner.h"
#include "gpu/command_buffer/service/ohos/ohos_video_image_backing.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/shared_context_state.h"
#include "gpu/command_buffer/service/shared_image/shared_image_backing.h"
#include "gpu/command_buffer/service/ohos/scoped_native_buffer_fence_sync.h"
#include "gpu/gpu_gles2_export.h"

namespace gpu {

class HwVideoNativeBufferImageBacking : public OhosVideoImageBacking,
                                        public RefCountedLockHelperDrDc {
 public:
  HwVideoNativeBufferImageBacking(
      const Mailbox& mailbox,
      const gfx::Size& size,
      const gfx::ColorSpace color_space,
      GrSurfaceOrigin surface_origin,
      SkAlphaType alpha_type,
      std::string debug_label,
      scoped_refptr<StreamTextureSharedImageInterface> stream_texture_sii,
      scoped_refptr<SharedContextState> shared_context_state,
      scoped_refptr<RefCountedLock> drdc_lock);

  ~HwVideoNativeBufferImageBacking() override;

  HwVideoNativeBufferImageBacking(const HwVideoNativeBufferImageBacking&) =
      delete;
  HwVideoNativeBufferImageBacking& operator=(
      const HwVideoNativeBufferImageBacking&) = delete;

  // SharedImageBacking implementation.
  size_t GetEstimatedSizeForMemoryDump() const override;

 protected:
  std::unique_ptr<GLTextureImageRepresentation> ProduceGLTexture(
      SharedImageManager* manager,
      MemoryTypeTracker* tracker) override;

  std::unique_ptr<GLTexturePassthroughImageRepresentation> ProduceGLTexturePassthrough(
    SharedImageManager* manager,
    MemoryTypeTracker* tracker) override;

  std::unique_ptr<SkiaGaneshImageRepresentation> ProduceSkiaGanesh(
      SharedImageManager* manager,
      MemoryTypeTracker* tracker,
      scoped_refptr<SharedContextState> context_state) override;

 private:
  // Helper class for observing SharedContext loss on gpu main thread and
  // cleaning up resources accordingly.
  class ContextLostObserverHelper
      : public SharedContextState::ContextLostObserver,
        public RefCountedLockHelperDrDc {
   public:
    ContextLostObserverHelper(
        scoped_refptr<SharedContextState> context_state,
        scoped_refptr<StreamTextureSharedImageInterface> stream_texture_sii,
        scoped_refptr<base::SingleThreadTaskRunner> gpu_main_task_runner,
        scoped_refptr<RefCountedLock> drdc_lock);
    ~ContextLostObserverHelper() override;

   private:
    // SharedContextState::ContextLostObserver implementation.
    void OnContextLost() override;

    scoped_refptr<SharedContextState> context_state_;
    scoped_refptr<StreamTextureSharedImageInterface> stream_texture_sii_;
    scoped_refptr<base::SingleThreadTaskRunner> gpu_main_task_runner_;
  };

  class GLTextureVideoImageRepresentation;
  class GLTexturePassthroughVideoImageRepresentation;
  class SkiaVkNBRepresentation;

  gpu::ScopedNativeBufferHandle GetNativeBufferHandle() const ;
  std::unique_ptr<ContextLostObserverHelper> context_lost_helper_;
  scoped_refptr<StreamTextureSharedImageInterface> stream_texture_sii_;

  // Currently this object is created only from gpu main thread.
  scoped_refptr<base::SingleThreadTaskRunner> gpu_main_task_runner_;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_OHOS_HW_VIDEO_NATIVE_BUFFER_IMAGE_BACKING_H_
