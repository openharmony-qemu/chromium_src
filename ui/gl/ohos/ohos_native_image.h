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

#ifndef UI_GL_OHOS_NATIVE_IMAGE_H_
#define UI_GL_OHOS_NATIVE_IMAGE_H_

#include <memory>

#include "base/functional/callback.h"
#include "base/memory/ref_counted.h"
#include "ui/gl/gl_export.h"
#include "native_image/native_image.h"

namespace gl {

class GL_EXPORT OhosNativeImage
    : public base::RefCountedThreadSafe<OhosNativeImage> {
 public:
  static scoped_refptr<OhosNativeImage> Create(int texture_id);

  OhosNativeImage(const OhosNativeImage&) = delete;
  OhosNativeImage& operator=(const OhosNativeImage&) = delete;
  void SetFrameAvailableCallback(base::RepeatingClosure callback);
  void UpdateNativeImage();
  void GetTransformMatrix(float mtx[16]);
  void AttachToGLContext();
  void DetachFromGLContext();
  void ReleaseNativeImage();
  void* AquireOhosNativeWindow();
  static void OnFrameAvailableListener(void* context);

 protected:
  explicit OhosNativeImage(OH_NativeImage* native_image);

 private:
  friend class base::RefCountedThreadSafe<OhosNativeImage>;
  virtual ~OhosNativeImage();

  OH_NativeImage* native_image_;
  base::RepeatingClosure frame_available_cb_;
};

}  // namespace gl

#endif // UI_GL_OHOS_NATIVE_IMAGE_H_
