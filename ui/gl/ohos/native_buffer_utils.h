// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_OHOS_NATIVE_BUFFER_UTILS_H_
#define UI_GL_OHOS_NATIVE_BUFFER_UTILS_H_

#include "base/files/scoped_file.h"
#include "gpu/gpu_gles2_export.h"
#include "ohos_adapter_helper.h"
#include "ui/gl/scoped_egl_image.h"

namespace gl {
namespace ohos {

#define EGL_NATIVE_BUFFER_OHOS 0x34E1

enum class TextureOwnerMode {
  kNone = 0,
  kNativeImageTexture = 1,
  kSameLayerNativeBuffer = 2,
};

GL_EXPORT ScopedEGLImage CreateEGLImage(EGLClientBuffer egl_client_buffer);

GL_EXPORT int GetEGLClientBufferFromNativeBuffer(void* ohos_native_buffer,
                                                 void** egl_client_buffer);

GL_EXPORT void FreeEGLClientBuffer(EGLClientBuffer egl_client_buffer);

GL_EXPORT bool InsertEglFenceAndWait(base::ScopedFD acquire_fence_fd);

}  // namespace ohos
}  // namespace gl

#endif  // UI_GL_OHOS_NATIVE_IMAGE_H_
