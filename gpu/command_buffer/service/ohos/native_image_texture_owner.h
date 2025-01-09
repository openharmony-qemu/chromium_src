// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_IMAGE_TEXTURE_OWNER_H_
#define GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_IMAGE_TEXTURE_OWNER_H_

#include "base/memory/ref_counted.h"
#include "base/memory/ref_counted_delete_on_sequence.h"
#include "base/task/single_thread_task_runner.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/ohos/ohos_video_image_backing.h"
#include "gpu/command_buffer/service/shared_context_state.h"
#include "gpu/gpu_gles2_export.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_image.h"
#include "ui/gl/gl_surface.h"

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

class GPU_GLES2_EXPORT NativeImageTextureOwner
    : public base::RefCountedDeleteOnSequence<NativeImageTextureOwner>,
      public SharedContextState::ContextLostObserver {
 public:
  static scoped_refptr<NativeImageTextureOwner> Create(
      scoped_refptr<SharedContextState> context_state,
      gl::ohos::TextureOwnerMode mode,
      scoped_refptr<RefCountedLock> drdc_lock);

  NativeImageTextureOwner(const NativeImageTextureOwner&) = delete;
  NativeImageTextureOwner& operator=(const NativeImageTextureOwner&) = delete;

  scoped_refptr<base::SingleThreadTaskRunner> task_runner() {
    return task_runner_;
  }

  GLuint GetTextureId() const;
  TextureBase* GetTextureBase() const;
  virtual gl::GLContext* GetContext() const = 0;
  virtual gl::GLSurface* GetSurface() const = 0;
  virtual void* AquireOhosNativeWindow() const = 0;

  virtual void UpdateNativeImage() = 0;

  virtual void GetSurfaceId(uint64_t* surface_id) = 0;

  virtual void EnsureNativeImageBound(GLuint service_id) = 0;

  virtual void ReleaseNativeImage() = 0;

  virtual bool GetCodedSizeAndVisibleRect(gfx::Size rotated_visible_size,
                                          gfx::Size* coded_size,
                                          gfx::Rect* visible_rect) = 0;

  virtual void SetFrameAvailableCallback(
      const base::RepeatingClosure& frame_available_cb) = 0;

  virtual void RunWhenBufferIsAvailable(base::OnceClosure callback) = 0;

  virtual std::unique_ptr<ScopedNativeBufferFenceSync> GetNativeBuffer() = 0;

  bool binds_texture_on_update() const { return binds_texture_on_update_; }

  void OnContextLost() override;

 protected:
  friend class base::RefCountedDeleteOnSequence<NativeImageTextureOwner>;
  friend class base::DeleteHelper<NativeImageTextureOwner>;

  class ScopedRestoreTextureBinding {
   public:
    ScopedRestoreTextureBinding() {
      glGetIntegerv(GL_TEXTURE_BINDING_EXTERNAL_OES, &bound_service_id_);
    }
    ~ScopedRestoreTextureBinding() {
      glBindTexture(GL_TEXTURE_EXTERNAL_OES, bound_service_id_);
    }

   private:
    GLint bound_service_id_;
  };

  NativeImageTextureOwner(bool binds_texture_on_update,
                          std::unique_ptr<AbstractTextureOHOS> texture,
                          scoped_refptr<SharedContextState> context_state);
  ~NativeImageTextureOwner() override;

  virtual void ReleaseResources() = 0;

  AbstractTextureOHOS* texture() const { return texture_.get(); }

 private:
  NativeImageTextureOwner(bool binds_texture_on_update,
                          std::unique_ptr<AbstractTextureOHOS> texture);

  const bool binds_texture_on_update_;

  scoped_refptr<SharedContextState> context_state_;
  std::unique_ptr<AbstractTextureOHOS> texture_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_IMAGE_TEXTURE_OWNER_H_
