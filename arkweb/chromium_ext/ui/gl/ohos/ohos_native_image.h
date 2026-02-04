// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_OHOS_NATIVE_IMAGE_H_
#define UI_GL_OHOS_NATIVE_IMAGE_H_

#include <memory>

#include "base/functional/callback.h"
#include "base/memory/ref_counted.h"
#include "frame_available_listener_impl.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/graphic_adapter.h"
#include "ui/gl/gl_export.h"

namespace gl {

class GL_EXPORT OhosNativeImage
    : public base::RefCountedThreadSafe<OhosNativeImage> {
 public:
  static scoped_refptr<OhosNativeImage> Create(int texture_id);

  OhosNativeImage(const OhosNativeImage&) = delete;
  OhosNativeImage& operator=(const OhosNativeImage&) = delete;
  void SetFrameAvailableCallback(base::RepeatingClosure callback);
  void UpdateNativeImage();
  void GetSurfaceId(uint64_t* surface_id);
  void GetTransformMatrix(float mtx[16]);
  void AttachToGLContext();
  void DetachFromGLContext();
  void ReleaseNativeImage();
  void* AquireOhosNativeWindow();
  static void OnFrameAvailableListener(void* context);

  static scoped_refptr<OhosNativeImage> Create();

  int32_t AcquireNativeWindowBuffer(void** windowBuffer, int* acquireFenceFd);

  int32_t GetNativeBuffer(void* windowBuffer, void** nativeBuffer);

  int32_t ReleaseNativeWindowBuffer(void* windowBuffer, int fenceFd);

  void GetNativeWindowBufferSize(void* windowBuffer,
                                 uint32_t* width,
                                 uint32_t* height);

  void GetTransformMatrixV1(float mtx[16], size_t mtx_size);

 protected:
  explicit OhosNativeImage(
      std::unique_ptr<OHOS::NWeb::NativeImageAdapter> native_image_adapter);

 private:
  friend class base::RefCountedThreadSafe<OhosNativeImage>;
  virtual ~OhosNativeImage();
  void DestroyPassedSurfaceFromGpuProcess();

  std::unique_ptr<OHOS::NWeb::NativeImageAdapter> native_image_adapter_;
  std::shared_ptr<OHOS::NWeb::FrameAvailableListenerImpl> listener_;

  base::RepeatingClosure frame_available_cb_;
};

}  // namespace gl

#endif  // UI_GL_OHOS_NATIVE_IMAGE_H_
