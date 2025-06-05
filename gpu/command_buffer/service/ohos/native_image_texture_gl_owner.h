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

#ifndef GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_IMAGE_SURFACE_TEXTURE_GL_OWNER_H_
#define GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_IMAGE_SURFACE_TEXTURE_GL_OWNER_H_

#include <memory>

#include "base/threading/thread_checker.h"
#include "gpu/command_buffer/service/ohos/native_image_texture_owner.h"
#include "gpu/gpu_export.h"
#include "shared_image_video_ohos.h"
#include "ui/gl/ohos/ohos_native_image.h"

namespace gpu {

class GPU_GLES2_EXPORT NativeImageTextureGlOwner
    : public NativeImageTextureOwner {
 public:
  NativeImageTextureGlOwner(const NativeImageTextureGlOwner&) = delete;
  NativeImageTextureGlOwner& operator=(const NativeImageTextureGlOwner&) = delete;

  gl::GLContext* GetContext() const override;
  gl::GLSurface* GetSurface() const override;
  void SetFrameAvailableCallback(
      const base::RepeatingClosure& frame_available_cb) override;
  void* AquireOhosNativeWindow() const override;
  void UpdateNativeImage() override;
  void EnsureNativeImageBound(GLuint service_id) override;
  void ReleaseNativeImage() override;
  bool GetCodedSizeAndVisibleRect(gfx::Size rotated_visible_size,
                                  gfx::Size* coded_size,
                                  gfx::Rect* visible_rect) override;

  void RunWhenBufferIsAvailable(base::OnceClosure callback) override;

 protected:
  void ReleaseResources() override;

 private:
  friend class NativeImageTextureOwner;

  NativeImageTextureGlOwner(std::unique_ptr<AbstractTextureOHOS> texture,
                            scoped_refptr<SharedContextState> context_state);
  ~NativeImageTextureGlOwner() override;

  static bool DecomposeTransform(float matrix[16],
                                 gfx::Size rotated_visible_size,
                                 gfx::Size* coded_size,
                                 gfx::Rect* visible_rect);

  scoped_refptr<gl::OhosNativeImage> native_image_;

  scoped_refptr<gl::GLContext> context_;
  scoped_refptr<gl::GLSurface> surface_;

  bool is_frame_available_callback_set_ = false;

  THREAD_CHECKER(thread_checker_);
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_OHOS_NATIVE_IMAGE_SURFACE_TEXTURE_GL_OWNER_H_
