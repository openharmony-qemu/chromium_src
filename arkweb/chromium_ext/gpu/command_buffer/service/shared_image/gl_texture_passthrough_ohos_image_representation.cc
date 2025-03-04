// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on gl_texture_passthrough_ohos_image_representation.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "gpu/command_buffer/service/shared_image/gl_texture_passthrough_ohos_image_representation.h"

#include "gpu/command_buffer/service/texture_manager.h"
#include "ui/gl/android/egl_fence_utils.h"

namespace gpu {

GLTexturePassthroughOhosImageRepresentation::
    GLTexturePassthroughOhosImageRepresentation(
        SharedImageManager* manager,
        OhosNativeBufferImageBacking* backing,
        MemoryTypeTracker* tracker,
        gl::ScopedEGLImage egl_image,
        scoped_refptr<gles2::TexturePassthrough> texture)
    : GLTexturePassthroughImageRepresentation(manager, backing, tracker),
      egl_image_(std::move(egl_image)),
      texture_(std::move(texture)) {
  // TODO(https://crbug.com/1172769): Remove this CHECK.
  CHECK(texture_);
}

GLTexturePassthroughOhosImageRepresentation::
    ~GLTexturePassthroughOhosImageRepresentation() {
  EndAccess();
  if (!has_context()) {
    texture_->MarkContextLost();
  }
}

const scoped_refptr<gles2::TexturePassthrough>&
GLTexturePassthroughOhosImageRepresentation::GetTexturePassthrough(
    int plane_index) {
  DCHECK_EQ(plane_index, 0);
  return texture_;
}

bool GLTexturePassthroughOhosImageRepresentation::BeginAccess(GLenum mode) {
  bool read_only_mode = (mode == GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);
  bool read_write_mode =
      (mode == GL_SHARED_IMAGE_ACCESS_MODE_READWRITE_CHROMIUM);
  DCHECK(read_only_mode || read_write_mode);

  if (read_only_mode) {
    base::ScopedFD write_sync_fd;
    if (!ohos_backing()->BeginRead(this, &write_sync_fd)) {
      return false;
    }
    if (!gl::InsertEglFenceAndWait(std::move(write_sync_fd))) {
      return false;
    }
  } else {
    base::ScopedFD sync_fd;
    if (!ohos_backing()->BeginWrite(&sync_fd)) {
      return false;
    }
    if (!gl::InsertEglFenceAndWait(std::move(sync_fd))) {
      return false;
    }
  }

  if (read_only_mode) {
    mode_ = RepresentationAccessMode::kRead;
  } else {
    mode_ = RepresentationAccessMode::kWrite;
  }

  return true;
}

void GLTexturePassthroughOhosImageRepresentation::EndAccess() {
  if (mode_ == RepresentationAccessMode::kNone) {
    return;
  }

  base::ScopedFD sync_fd = gl::CreateEglFenceAndExportFd();

  // Pass this fd to its backing.
  if (mode_ == RepresentationAccessMode::kRead) {
    ohos_backing()->EndRead(this, std::move(sync_fd));
  } else if (mode_ == RepresentationAccessMode::kWrite) {
    ohos_backing()->EndWrite(std::move(sync_fd));
  }

  mode_ = RepresentationAccessMode::kNone;
}

}  // namespace gpu
