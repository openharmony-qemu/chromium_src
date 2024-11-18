/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef GPU_COMMAND_BUFFER_SERVICE_OHOS_SAME_LAYER_NATIVE_BUFFER_IMAGE_BACKING_H_
#define GPU_COMMAND_BUFFER_SERVICE_OHOS_SAME_LAYER_NATIVE_BUFFER_IMAGE_BACKING_H_

#include "base/memory/scoped_refptr.h"
#include "base/task/single_thread_task_runner.h"
#include "gpu/command_buffer/service/ohos/ohos_video_image_backing.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/shared_context_state.h"
#include "gpu/command_buffer/service/shared_image/shared_image_backing.h"
#include "gpu/gpu_gles2_export.h"

namespace gpu {

class SameLayerNativeBufferImageBacking : public OhosVideoImageBacking,
                                          public RefCountedLockHelperDrDc {
 public:
  SameLayerNativeBufferImageBacking(
      const Mailbox& mailbox,
      const gfx::Size& size,
      const gfx::ColorSpace color_space,
      GrSurfaceOrigin surface_origin,
      SkAlphaType alpha_type,
      scoped_refptr<StreamTextureSharedImageInterface> stream_texture_sii,
      scoped_refptr<SharedContextState> shared_context_state,
      scoped_refptr<RefCountedLock> drdc_lock);

  ~SameLayerNativeBufferImageBacking() override;

  SameLayerNativeBufferImageBacking(const SameLayerNativeBufferImageBacking&) =
      delete;
  SameLayerNativeBufferImageBacking& operator=(
      const SameLayerNativeBufferImageBacking&) = delete;

 protected:
  std::unique_ptr<GLTextureImageRepresentation> ProduceGLTexture(
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

  std::unique_ptr<ContextLostObserverHelper> context_lost_helper_;
  scoped_refptr<StreamTextureSharedImageInterface> stream_texture_sii_;

  // Currently this object is created only from gpu main thread.
  scoped_refptr<base::SingleThreadTaskRunner> gpu_main_task_runner_;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_OHOS_SAME_LAYER_NATIVE_BUFFER_IMAGE_BACKING_H_
