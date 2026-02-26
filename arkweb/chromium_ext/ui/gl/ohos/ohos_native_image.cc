// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_native_image.h"

#include <mutex>
#include <utility>

#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "base/check.h"
#include "base/command_line.h"
#include "base/debug/crash_logging.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"
#include "ui/gl/gl_bindings.h"
#include "base/logging.h"

#ifndef GL_ANGLE_texture_storage_external
#define GL_ANGLE_texture_storage_external 1
#define GL_TEXTURE_NATIVE_ID_ANGLE 0x3481
#endif /* GL_ANGLE_texture_storage_external */

namespace {
constexpr char kProcessType[] = "type";
constexpr char kGpuProcess[] = "gpu-process";
}  // namespace

namespace gl {

std::mutex g_mutex_native_image;

scoped_refptr<OhosNativeImage> OhosNativeImage::Create(int texture_id) {
  int native_id = texture_id;

  if (texture_id != 0 &&
      gl::g_current_gl_driver->ext.b_GL_ANGLE_texture_external_update) {
    GLint prev_texture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_EXTERNAL_OES, &prev_texture);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture_id);
    glGetTexParameteriv(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_NATIVE_ID_ANGLE,
                        &native_id);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, prev_texture);
  }

  auto nativeImageAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateNativeImageAdapter();
  if (nativeImageAdapter == nullptr) {
    return nullptr;
  }
  nativeImageAdapter->CreateNativeImage(native_id, GL_TEXTURE_EXTERNAL_OES);
  return new OhosNativeImage(std::move(nativeImageAdapter));
}

//LCOV_EXCL_START
OhosNativeImage::OhosNativeImage(
    std::unique_ptr<OHOS::NWeb::NativeImageAdapter> native_image_adapter)
    : native_image_adapter_(std::move(native_image_adapter)) {
  task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
}

OhosNativeImage::~OhosNativeImage() {
  if (native_image_adapter_ != nullptr) {
    native_image_adapter_->DestroyNativeImage();
  }
}
//LCOV_EXCL_STOP

void OhosNativeImage::SetFrameAvailableCallback(
    base::RepeatingClosure callback) {
  DCHECK(!frame_available_cb_);
  std::lock_guard<std::mutex> lock(g_mutex_native_image);
  frame_available_cb_ = std::move(callback);
  if (native_image_adapter_ != nullptr && listener_ == nullptr) {
    listener_ = std::make_shared<OHOS::NWeb::FrameAvailableListenerImpl>(
                          weak_ptr_factory_.GetWeakPtr(), task_runner_);
    native_image_adapter_->SetOnFrameAvailableListener(listener_);
  }
}

//LCOV_EXCL_START
void OhosNativeImage::UpdateNativeImage() {
  static auto* kCrashKey = base::debug::AllocateCrashKeyString(
      "inside_surface_texture_update_tex_image",
      base::debug::CrashKeySize::Size256);
  base::debug::ScopedCrashKeyString scoped_crash_key(kCrashKey, "1");

  if (native_image_adapter_ == nullptr) {
    return;
  }
  native_image_adapter_->UpdateSurfaceImage();

  if (gl::g_current_gl_driver->ext.b_GL_ANGLE_texture_external_update) {
    glInvalidateTextureANGLE(GL_TEXTURE_EXTERNAL_OES);
  }
}
//LCOV_EXCL_STOP

void OhosNativeImage::GetSurfaceId(uint64_t* surface_id) {
  if (native_image_adapter_ == nullptr) {
    return;
  }
  native_image_adapter_->GetSurfaceId(surface_id);
  auto type =
      base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(kProcessType);
  if (type == kGpuProcess) {
    LOG(INFO) << "dxlog GetSurfaceId : " << *surface_id;
    NWebNativeWindowTracker::GetInstance()->g_browser_client_->PassSurface(
        *surface_id);
  }
}

//LCOV_EXCL_START
void OhosNativeImage::GetTransformMatrix(float mtx[16]) {
  if (native_image_adapter_ == nullptr) {
    return;
  }
  native_image_adapter_->GetTransformMatrix(mtx);
}

void OhosNativeImage::AttachToGLContext() {
  if (native_image_adapter_ == nullptr) {
    return;
  }

  int texture_id = 0;
  if (gl::g_current_gl_driver->ext.b_GL_ANGLE_texture_external_update) {
    glGetTexParameteriv(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_NATIVE_ID_ANGLE,
                        &texture_id);
  } else {
    glGetIntegerv(GL_TEXTURE_BINDING_EXTERNAL_OES, &texture_id);
  }

  DCHECK(texture_id);
  native_image_adapter_->AttachContext(texture_id);
  if (gl::g_current_gl_driver->ext.b_GL_ANGLE_texture_external_update) {
    glInvalidateTextureANGLE(GL_TEXTURE_EXTERNAL_OES);
  }
}

void OhosNativeImage::DetachFromGLContext() {
  if (native_image_adapter_ == nullptr) {
    return;
  }
  native_image_adapter_->DetachContext();
}

void OhosNativeImage::ReleaseNativeImage() {
  if (native_image_adapter_ == nullptr) {
    return;
  }
  native_image_adapter_->DestroyNativeImage();
}

void* OhosNativeImage::AquireOhosNativeWindow() {
  if (native_image_adapter_ == nullptr) {
    return nullptr;
  }
  return native_image_adapter_->AquireNativeWindowFromNativeImage();
}

void OhosNativeImage::OnFrameAvailableListener() {
  frame_available_cb_.Run();
}

scoped_refptr<OhosNativeImage> OhosNativeImage::Create() {
  auto nativeImageAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateNativeImageAdapter();
  if (nativeImageAdapter == nullptr) {
    return nullptr;
  }
  nativeImageAdapter->NewNativeImage();
  return new OhosNativeImage(std::move(nativeImageAdapter));
}

const int32_t IMAGE_READ_ERR = 1;
int32_t OhosNativeImage::AcquireNativeWindowBuffer(void** windowBuffer,
                                                   int* acquireFenceFd) {
  if (native_image_adapter_ == nullptr) {
    return IMAGE_READ_ERR;
  }
  return native_image_adapter_->AcquireNativeWindowBuffer(windowBuffer,
                                                          acquireFenceFd);
}

int32_t OhosNativeImage::GetNativeBuffer(void* windowBuffer,
                                         void** nativeBuffer) {
  if (native_image_adapter_ == nullptr) {
    return IMAGE_READ_ERR;
  }
  return native_image_adapter_->GetNativeBuffer(windowBuffer, nativeBuffer);
}

int32_t OhosNativeImage::ReleaseNativeWindowBuffer(void* windowBuffer,
                                                   int fenceFd) {
  if (native_image_adapter_ == nullptr) {
    return IMAGE_READ_ERR;
  }
  return native_image_adapter_->ReleaseNativeWindowBuffer(windowBuffer,
                                                          fenceFd);
}

void OhosNativeImage::GetNativeWindowBufferSize(void* windowBuffer,
                                                uint32_t* width,
                                                uint32_t* height) {
  if (native_image_adapter_ == nullptr) {
    return;
  }
  return native_image_adapter_->GetNativeWindowBufferSize(windowBuffer, width,
                                                          height);
}

void OhosNativeImage::GetTransformMatrixV1(float mtx[16], size_t mtx_size) {
  if (native_image_adapter_ == nullptr || mtx_size != 16u) {
    return;
  }
  native_image_adapter_->GetTransformMatrix(mtx);
}
//LCOV_EXCL_STOP
}  // namespace gl
