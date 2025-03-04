// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_IMAGE_SURFACE_TEXTURE_GL_OWNER_H_
#define GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_IMAGE_SURFACE_TEXTURE_GL_OWNER_H_

#include <memory>

#include "base/threading/thread_checker.h"
#include "gpu/command_buffer/service/ohos/native_image_texture_owner.h"
#include "gpu/gpu_export.h"
#include "ohos_video_image_backing.h"
#include "ui/gl/ohos/ohos_native_image.h"

#if BUILDFLAG(ARKWEB_UNITTESTS)
#define MYPRIVATE public
#else
#define MYPRIVATE private
#endif

namespace gpu {
class ScopedNativeBufferFenceSync;

class GPU_GLES2_EXPORT NativeImageTextureGlOwner
    : public NativeImageTextureOwner {
 public:
  NativeImageTextureGlOwner(const NativeImageTextureGlOwner&) = delete;
  NativeImageTextureGlOwner& operator=(const NativeImageTextureGlOwner&) =
      delete;

  gl::GLContext* GetContext() const override;
  gl::GLSurface* GetSurface() const override;
  void SetFrameAvailableCallback(
      const base::RepeatingClosure& frame_available_cb) override;
  void* AquireOhosNativeWindow() const override;
  void UpdateNativeImage() override;
  void GetSurfaceId(uint64_t* surface_id) override;
  void EnsureNativeImageBound(GLuint service_id) override;
  void ReleaseNativeImage() override;
  bool GetCodedSizeAndVisibleRect(gfx::Size rotated_visible_size,
                                  gfx::Size* coded_size,
                                  gfx::Rect* visible_rect) override;

  void RunWhenBufferIsAvailable(base::OnceClosure callback) override;
  std::unique_ptr<ScopedNativeBufferFenceSync> GetNativeBuffer() override;

  static bool DecomposeTransform(float matrix[16],
                                 size_t matrix_size,
                                 gfx::Size rotated_visible_size,
                                 gfx::Size* coded_size,
                                 gfx::Rect* visible_rect);

 protected:
  void ReleaseResources() override;

  MYPRIVATE : friend class NativeImageTextureOwner;

  NativeImageTextureGlOwner(std::unique_ptr<AbstractTextureOHOS> texture,
                            scoped_refptr<SharedContextState> context_state);
  ~NativeImageTextureGlOwner() override;

  scoped_refptr<gl::OhosNativeImage> native_image_;

  scoped_refptr<gl::GLContext> context_;
  scoped_refptr<gl::GLSurface> surface_;

  bool is_frame_available_callback_set_ = false;

  THREAD_CHECKER(thread_checker_);
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_IMAGE_SURFACE_TEXTURE_GL_OWNER_H_
