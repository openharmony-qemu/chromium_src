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
