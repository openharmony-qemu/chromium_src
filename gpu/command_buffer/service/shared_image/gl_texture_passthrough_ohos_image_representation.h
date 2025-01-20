// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_COMMAND_BUFFER_SERVICE_SHARED_IMAGE_GL_TEXTURE_PASSTHROUGH_OHOS_IMAGE_REPRESENTATION_H_
#define GPU_COMMAND_BUFFER_SERVICE_SHARED_IMAGE_GL_TEXTURE_PASSTHROUGH_OHOS_IMAGE_REPRESENTATION_H_

#include "gpu/command_buffer/service/shared_image/ohos_native_buffer_image_backing.h"
#include "gpu/command_buffer/service/shared_image/shared_image_representation.h"
#include "ui/gl/scoped_egl_image.h"

namespace gpu {
class OhosNativeBufferImageBacking;

class GLTexturePassthroughOhosImageRepresentation
    : public GLTexturePassthroughImageRepresentation {
 public:
  GLTexturePassthroughOhosImageRepresentation(
      SharedImageManager* manager,
      OhosNativeBufferImageBacking* backing,
      MemoryTypeTracker* tracker,
      gl::ScopedEGLImage egl_image,
      scoped_refptr<gles2::TexturePassthrough> texture);
  ~GLTexturePassthroughOhosImageRepresentation() override;

  GLTexturePassthroughOhosImageRepresentation(
      const GLTexturePassthroughOhosImageRepresentation&) = delete;
  GLTexturePassthroughOhosImageRepresentation& operator=(
      const GLTexturePassthroughOhosImageRepresentation&) = delete;

  const scoped_refptr<gles2::TexturePassthrough>& GetTexturePassthrough(
      int plane_index) override;

  bool BeginAccess(GLenum mode) override;
  void EndAccess() override;

 private:
  OhosNativeBufferImageBacking* ohos_backing() {
    return static_cast<OhosNativeBufferImageBacking*>(backing());
  }

  gl::ScopedEGLImage egl_image_;
  scoped_refptr<gles2::TexturePassthrough> texture_;
  RepresentationAccessMode mode_ = RepresentationAccessMode::kNone;
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_SERVICE_SHARED_IMAGE_GL_TEXTURE_PASSTHROUGH_OHOS_IMAGE_REPRESENTATION_H_
