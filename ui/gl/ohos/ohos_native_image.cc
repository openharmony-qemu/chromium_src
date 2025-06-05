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

#include "ui/gl/ohos/ohos_native_image.h"

#include <utility>

#include "base/check.h"
#include "base/debug/crash_logging.h"
#include "ui/gl/gl_bindings.h"

namespace gl {

scoped_refptr<OhosNativeImage> OhosNativeImage::Create(int texture_id) {
  auto ohos_native_image =
      OH_NativeImage_Create(texture_id, GL_TEXTURE_EXTERNAL_OES);
  if (ohos_native_image == nullptr) {
    LOG(ERROR) << "OhosNativeImage::OH_NativeImage_Create failed!";
    return nullptr;
  }
  return new OhosNativeImage(ohos_native_image);
}

OhosNativeImage::OhosNativeImage(OH_NativeImage* native_image)
    : native_image_(native_image) {}

OhosNativeImage::~OhosNativeImage() {
  if (native_image_ != nullptr) {
    OH_NativeImage_Destroy(&native_image_);
    native_image_ = nullptr;
  }
}

void OhosNativeImage::SetFrameAvailableCallback(
    base::RepeatingClosure callback) {
  DVLOG(2) << "OhosNativeImage::SetFrameAvailableCallback";
  DCHECK(!frame_available_cb_);
  frame_available_cb_ = std::move(callback);
  if (native_image_ != nullptr) {
    OH_OnFrameAvailableListener listener_callback;
    listener_callback.context = reinterpret_cast<void*>(this);
    listener_callback.onFrameAvailable =
        &OhosNativeImage::OnFrameAvailableListener;
    OH_NativeImage_SetOnFrameAvailableListener(native_image_,
                                               listener_callback);
  }
}

void OhosNativeImage::UpdateNativeImage() {
  static auto* kCrashKey = base::debug::AllocateCrashKeyString(
      "inside_surface_texture_update_tex_image",
      base::debug::CrashKeySize::Size256);
  base::debug::ScopedCrashKeyString scoped_crash_key(kCrashKey, "1");

  int32_t ret = OH_NativeImage_UpdateSurfaceImage(native_image_);
  if (ret != 0) {
    LOG(ERROR) << "OH_NativeImage_UpdateSurfaceImage is failed! ret:" << ret;
    return;
  }
}

void OhosNativeImage::GetTransformMatrix(float mtx[16]) {
  int32_t ret = OH_NativeImage_GetTransformMatrix(native_image_, mtx);
  if (ret != 0) {
    LOG(ERROR) << "OH_NativeImage_GetTransformMatrix is failed! ret:" << ret;
    return;
  }
}

void OhosNativeImage::AttachToGLContext() {
  int texture_id = 0;
  glGetIntegerv(GL_TEXTURE_BINDING_EXTERNAL_OES, &texture_id);
  DCHECK(texture_id);
  OH_NativeImage_AttachContext(native_image_, texture_id);
}

void OhosNativeImage::DetachFromGLContext() {
  OH_NativeImage_DetachContext(native_image_);
}

void OhosNativeImage::ReleaseNativeImage() {
  OH_NativeImage_Destroy(&native_image_);
  native_image_ = nullptr;
}

void* OhosNativeImage::AquireOhosNativeWindow() {
  return OH_NativeImage_AcquireNativeWindow(native_image_);
}

void OhosNativeImage::OnFrameAvailableListener(void* context) {
  OhosNativeImage* native_image = reinterpret_cast<OhosNativeImage*>(context);
  if (native_image == nullptr) {
    return;
  }
  native_image->frame_available_cb_.Run();
}
}  // namespace gl
