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

#ifndef GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_BUFFER_GL_OWNER_H_
#define GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_BUFFER_GL_OWNER_H_

#include <memory>

#include "base/containers/circular_deque.h"
#include "base/containers/flat_map.h"
#include "base/files/scoped_file.h"
#include "base/memory/raw_ptr.h"
#include "base/threading/thread_checker.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/ohos/native_image_texture_owner.h"
#include "gpu/gpu_gles2_export.h"
#include "ui/gl/gl_fence_egl.h"
#include "ui/gl/ohos/ohos_native_image.h"

namespace gpu {
class AbstractTextureOHOS;
class ScopedNativeBufferFenceSync;
class TextureBase;

// OhosWindowBuffer --- AImage
class OhosWindowBuffer {
public:
    void* rawbuffer = nullptr;

    OhosWindowBuffer() = default;
    OhosWindowBuffer(const OhosWindowBuffer* buffer) {
        rawbuffer = buffer->rawbuffer;
    }
};

class GPU_GLES2_EXPORT SameLayerNativeBufferGLOwner
    : public NativeImageTextureOwner,
      public RefCountedLockHelperDrDc {
 public:
  SameLayerNativeBufferGLOwner(const SameLayerNativeBufferGLOwner&) = delete;
  SameLayerNativeBufferGLOwner& operator=(const SameLayerNativeBufferGLOwner&) = delete;

  gl::GLContext* GetContext() const override;
  gl::GLSurface* GetSurface() const override;
  void SetFrameAvailableCallback(
      const base::RepeatingClosure& frame_available_cb) override;
  void UpdateNativeImage() override;
  std::unique_ptr<ScopedNativeBufferFenceSync> GetNativeBuffer() override;
  bool GetCodedSizeAndVisibleRect(gfx::Size rotated_visible_size,
                                  gfx::Size* coded_size,
                                  gfx::Rect* visible_rect) override;

  void RunWhenBufferIsAvailable(base::OnceClosure callback) override;

  void* AquireOhosNativeWindow() const override;

  void GetSurfaceId(uint64_t* surface_id) override;

  void EnsureNativeImageBound(GLuint service_id) override;

  void ReleaseNativeImage() override;

 protected:
  void ReleaseResources() override;

 private:
  friend class NativeImageTextureOwner;
  class ScopedNativeBufferImpl;

  // Manages ownership of the latest image retrieved from gl::OhosNativeImage and
  // ensuring synchronization of its use in GL using fences.
  class ScopedCurrentImageRef {
   public:
    ScopedCurrentImageRef(SameLayerNativeBufferGLOwner* texture_owner,
                          OhosWindowBuffer* image,
                          base::ScopedFD ready_fence);

    ScopedCurrentImageRef(const ScopedCurrentImageRef&) = delete;
    ScopedCurrentImageRef& operator=(const ScopedCurrentImageRef&) = delete;

    ~ScopedCurrentImageRef();
    OhosWindowBuffer* image() const { return image_; }
    base::ScopedFD GetReadyFence() const;

   private:
    raw_ptr<SameLayerNativeBufferGLOwner> texture_owner_;
    raw_ptr<OhosWindowBuffer> image_;
    base::ScopedFD ready_fence_;
  };

  SameLayerNativeBufferGLOwner(std::unique_ptr<AbstractTextureOHOS> texture,
                     scoped_refptr<SharedContextState> context_state,
                     scoped_refptr<RefCountedLock> drdc_lock);
  ~SameLayerNativeBufferGLOwner() override;

  // Registers and releases a ref on the image. Once the ref-count for an image
  // goes to 0, it is released back to the gl::OhosNativeImage with an optional release
  // fence if needed.
  void RegisterRefOnImageLocked(OhosWindowBuffer* image);
  void ReleaseRefOnImageLocked(OhosWindowBuffer* image, base::ScopedFD fence_fd);

  // This method acquires |lock_| and calls ReleaseRefOnImageLocked().
  void ReleaseRefOnImage(OhosWindowBuffer* image, base::ScopedFD fence_fd);

  // All members which can be concurrently accessed from multiple threads will
  // be guarded by |lock_|.
  mutable base::Lock lock_;

  // This works like a cached image until next new image is acquired which overwrites this.
  std::optional<ScopedCurrentImageRef> current_image_ref_ GUARDED_BY(lock_);

  // A map consisting of pending refs on an AImage. If an image has any refs, it
  // is automatically released once the ref-count is 0.
  struct ImageRef {
    ImageRef();

    ImageRef(const ImageRef&) = delete;
    ImageRef& operator=(const ImageRef&) = delete;

    ~ImageRef();

    ImageRef(ImageRef&& other);
    ImageRef& operator=(ImageRef&& other);

    size_t count = 0u;
    base::ScopedFD release_fence_fd;
  };
  using AImageRefMap = base::flat_map<OhosWindowBuffer*, ImageRef>;
  AImageRefMap image_refs_ GUARDED_BY(lock_);

  // reference to the class instance which is used to dynamically
  // load the functions in android libraries at runtime.
  scoped_refptr<gl::OhosNativeImage> loader_;

  // The context and surface that were used to create |texture_id_|.
  scoped_refptr<gl::GLContext> context_;
  scoped_refptr<gl::GLSurface> surface_;
  int32_t max_images_ = 0;

  // Runs when free buffer is available.
  base::OnceClosure buffer_available_cb_ GUARDED_BY(lock_);

  bool is_frame_available_callback_set_ = false;

  // This class is created on gpu main thread.
  THREAD_CHECKER(gpu_main_thread_checker_);
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_BUFFER_GL_OWNER_H_
