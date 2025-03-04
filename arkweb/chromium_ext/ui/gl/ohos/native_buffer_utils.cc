// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on android_image_reader_utils.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "ui/gl/ohos/native_buffer_utils.h"

#include "ui/gl/gl_context.h"
#include "ui/gl/gl_fence_android_native_fence_sync.h"
#include "ui/gl/gl_gl_api_implementation.h"
#include "ui/gl/gl_surface_egl.h"
#include "ui/gl/gl_utils.h"
#include "ui/gl/gl_version_info.h"
#include "ui/gl/scoped_binders.h"
#include "ui/gl/scoped_egl_image.h"
#include "base/trace_event/trace_event.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <sys/poll.h>

namespace gl {
namespace ohos {

gl::ScopedEGLImage CreateEGLImage(EGLClientBuffer egl_client_buffer) {
  EGLint attrs[] = {
      EGL_IMAGE_PRESERVED,
      EGL_TRUE,
      EGL_NONE,
  };

  return gl::MakeScopedEGLImage(EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS,
                                egl_client_buffer, attrs);
}

gl::ScopedEGLImage CreateEGLImageForVideo(EGLClientBuffer egl_client_buffer) {
  EGLint attrs[] = {
      EGL_IMAGE_PRESERVED,
      EGL_FALSE,
      EGL_NONE,
  };

  return gl::MakeScopedEGLImage(EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS,
                                egl_client_buffer, attrs);
}

int GetEGLClientBufferFromNativeBuffer(void* ohos_native_buffer,
                                       void** egl_client_buffer) {
  return OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetOhosNativeBufferAdapter()
      .GetEGLBuffer(ohos_native_buffer, egl_client_buffer);
}

void FreeEGLClientBuffer(EGLClientBuffer egl_client_buffer) {
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetOhosNativeBufferAdapter()
      .FreeEGLBuffer(egl_client_buffer);
}

bool InsertEglFenceAndWait(base::ScopedFD acquire_fence_fd) {
  int fence_fd = acquire_fence_fd.release();
  // If fence_fd is -1, we do not need synchronization fence and image is ready
  // to be used immediately. Also we dont need to close any fd. Else we need to
  // create a sync fence which is used to signal when the buffer is ready to be
  // consumed.
  if (fence_fd == -1) {
    return true;
  }

  // Create attribute-value list with the fence_fd.
  EGLint attribs[] = {EGL_SYNC_NATIVE_FENCE_FD_ANDROID, fence_fd, EGL_NONE};

  // Create and insert the fence sync gl command using the helper class in
  // gl_fence_egl.h. This method takes the ownership of the file descriptor if
  // it succeeds.
  std::unique_ptr<gl::GLFenceEGL> egl_fence(
      gl::GLFenceEGL::Create(EGL_SYNC_NATIVE_FENCE_ANDROID, attribs));

  // If above method fails to create an egl_fence, we need to close the file
  // descriptor.
  if (egl_fence == nullptr) {
    // Create a scoped FD to close fence_fd.
    base::ScopedFD temp_fd(fence_fd);
    LOG(ERROR) << " Failed to created egl fence object ";
    return false;
  }

  // Make the server wait and not the client.
  egl_fence->ServerWait();
  return true;
}

bool SyncFenceWait(base::ScopedFD acquire_fence_fd) {
  int fence_fd = acquire_fence_fd.get();
  TRACE_EVENT1("base", "SyncFenceWait", "sync_fd", fence_fd);

  // If fence_fd is -1, we do not need synchronization fence and image is ready
  // to be used immediately. Also we dont need to close any fd. Else we need to
  // create a sync fence which is used to signal when the buffer is ready to be
  // consumed.
  if (fence_fd == -1) {
    return true;
  }

  struct pollfd poll_fds = {0};
  poll_fds.fd = fence_fd;
  poll_fds.events = POLLIN;

  int ret = -1;
  do {
    ret = poll(&poll_fds, 1, -1);
  } while (ret == -1 && (errno == EINTR || errno == EAGAIN));

  if (ret == 0) {
    ret = -1;
    errno = ETIME;
  } else if (ret > 0) {
    ret = 0;
    if (poll_fds.revents & (POLLERR | POLLNVAL)) {
      ret = -1;
      errno = EINVAL;
    }
  }

  if (ret < 0) {
    LOG(ERROR) << "Failed to do SyncFenceWait errno " << errno;
    return false;
  }

  return true;
}

}  // namespace ohos
}  // namespace gl
