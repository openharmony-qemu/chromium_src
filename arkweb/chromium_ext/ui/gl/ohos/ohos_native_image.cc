// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_native_image.h"

#include <utility>

#include "base/check.h"
#include "base/command_line.h"
#include "base/debug/crash_logging.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "ui/gl/gl_bindings.h"

namespace {
constexpr char kProcessType[] = "type";
constexpr char kGpuProcess[] = "gpu-process";
}  // namespace

namespace gl {

scoped_refptr<OhosNativeImage> OhosNativeImage::Create(int texture_id) {
  auto nativeImageAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateNativeImageAdapter();
  if (nativeImageAdapter == nullptr) {
    return nullptr;
  }
  nativeImageAdapter->CreateNativeImage(texture_id, GL_TEXTURE_EXTERNAL_OES);
  return new OhosNativeImage(std::move(nativeImageAdapter));
}

OhosNativeImage::OhosNativeImage(
    std::unique_ptr<OHOS::NWeb::NativeImageAdapter> native_image_adapter)
    : native_image_adapter_(std::move(native_image_adapter)) {}

OhosNativeImage::~OhosNativeImage() {
  if (native_image_adapter_ != nullptr) {
    native_image_adapter_->DestroyNativeImage();
  }
}

void OhosNativeImage::SetFrameAvailableCallback(
    base::RepeatingClosure callback) {
  DCHECK(!frame_available_cb_);
  frame_available_cb_ = std::move(callback);
  if (native_image_adapter_ != nullptr && listener_ == nullptr) {
    listener_ = std::make_shared<OHOS::NWeb::FrameAvailableListenerImpl>();
    listener_->SetContext(reinterpret_cast<void*>(this));
    listener_->SetOnFrameAvailableCb(
        &OhosNativeImage::OnFrameAvailableListener);
    native_image_adapter_->SetOnFrameAvailableListener(listener_);
  }
}

void OhosNativeImage::UpdateNativeImage() {
  static auto* kCrashKey = base::debug::AllocateCrashKeyString(
      "inside_surface_texture_update_tex_image",
      base::debug::CrashKeySize::Size256);
  base::debug::ScopedCrashKeyString scoped_crash_key(kCrashKey, "1");

  if (native_image_adapter_ == nullptr) {
    return;
  }
  native_image_adapter_->UpdateSurfaceImage();
}

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
  glGetIntegerv(GL_TEXTURE_BINDING_EXTERNAL_OES, &texture_id);
  DCHECK(texture_id);
  native_image_adapter_->AttachContext(texture_id);
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

void OhosNativeImage::OnFrameAvailableListener(void* context) {
  OhosNativeImage* nativeImage = reinterpret_cast<OhosNativeImage*>(context);
  if (nativeImage == nullptr) {
    return;
  }
  nativeImage->frame_available_cb_.Run();
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
}  // namespace gl
