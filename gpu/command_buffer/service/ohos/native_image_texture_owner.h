/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_IMAGE_TEXTURE_OWNER_H_
#define GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_IMAGE_TEXTURE_OWNER_H_

#include "base/memory/ref_counted.h"
#include "base/memory/ref_counted_delete_on_sequence.h"
#include "base/task/single_thread_task_runner.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/shared_context_state.h"
#include "gpu/gpu_gles2_export.h"
#include "shared_image_video_ohos.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_surface.h"

namespace gpu {
class AbstractTextureOHOS;
class TextureBase;

class GPU_GLES2_EXPORT NativeImageTextureOwner
    : public base::RefCountedDeleteOnSequence<NativeImageTextureOwner>,
      public SharedContextState::ContextLostObserver {
 public:
  enum class Mode {
    kAImageReaderInsecure,
    kAImageReaderInsecureMultithreaded,
    kAImageReaderInsecureSurfaceControl,
    kAImageReaderSecureSurfaceControl,
    kSurfaceTextureInsecure,
    kOhosSurfaceTexture
  };

  static scoped_refptr<NativeImageTextureOwner> Create(
      scoped_refptr<SharedContextState> context_state,
      Mode mode = Mode::kOhosSurfaceTexture);

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

  virtual void EnsureNativeImageBound(GLuint service_id) = 0;

  virtual void ReleaseNativeImage() = 0;

  virtual bool GetCodedSizeAndVisibleRect(gfx::Size rotated_visible_size,
                                          gfx::Size* coded_size,
                                          gfx::Rect* visible_rect) = 0;

  virtual void SetFrameAvailableCallback(
      const base::RepeatingClosure& frame_available_cb) = 0;

  virtual void RunWhenBufferIsAvailable(base::OnceClosure callback) = 0;

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
