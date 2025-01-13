// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_SHARED_IMAGE_GL_TEXTURE_ANDROID_IMAGE_REPRESENTATION_H_
#define GPU_COMMAND_BUFFER_SERVICE_SHARED_IMAGE_GL_TEXTURE_ANDROID_IMAGE_REPRESENTATION_H_

#include "base/memory/raw_ptr.h"
#include "gpu/command_buffer/service/shared_image/ohos_native_buffer_image_backing.h"
#include "gpu/command_buffer/service/shared_image/shared_image_representation.h"
#include "ui/gl/scoped_egl_image.h"

namespace gpu {
class OhosNativeBufferImageBacking;

// A generic GL Texture representation which can be used by any backing on
// OHOS.
class GLTextureOhosImageRepresentation
    : public GLTextureImageRepresentation {
 public:
  GLTextureOhosImageRepresentation(SharedImageManager* manager,
                                      OhosNativeBufferImageBacking* backing,
                                      MemoryTypeTracker* tracker,
                                      gl::ScopedEGLImage egl_image,
                                      gles2::Texture* texture);
  ~GLTextureOhosImageRepresentation() override;

  GLTextureOhosImageRepresentation(
      const GLTextureOhosImageRepresentation&) = delete;
  GLTextureOhosImageRepresentation& operator=(
      const GLTextureOhosImageRepresentation&) = delete;

  gles2::Texture* GetTexture(int plane_index) override;

  bool BeginAccess(GLenum mode) override;
  void EndAccess() override;

 private:
  OhosNativeBufferImageBacking* ohos_backing() {
    return static_cast<OhosNativeBufferImageBacking*>(backing());
  }

  gl::ScopedEGLImage egl_image_;
  raw_ptr<gles2::Texture> texture_;
  RepresentationAccessMode mode_ = RepresentationAccessMode::kNone;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_SHARED_IMAGE_GL_TEXTURE_ANDROID_IMAGE_REPRESENTATION_H_
