// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on image_reader_gl_owner.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 
 
#include "gpu/command_buffer/service/ohos/hw_video_native_buffer_gl_owner.h"

#include <stdint.h>

#include "base/debug/alias.h"
#include "base/debug/dump_without_crashing.h"
#include "base/feature_list.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/memory/raw_ptr.h"
#include "base/posix/eintr_wrapper.h"
#include "base/synchronization/waitable_event.h"
#include "base/task/bind_post_task.h"
#include "base/trace_event/trace_event.h"
#include "gpu/command_buffer/service/abstract_texture_ohos.h"
#include "gpu/command_buffer/service/ohos/native_image_texture_gl_owner.h"
#include "gpu/command_buffer/service/ohos/scoped_native_buffer_fence_sync.h"
#include "gpu/config/gpu_finch_features.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "ui/gl/gl_utils.h"
#include "ui/gl/scoped_binders.h"
#include "ui/gl/scoped_make_current.h"
#include "arkweb/ohos_adapter_ndk/graphic_adapter/window_adapter_impl.h"

namespace gpu {

// This class is safe to be created/destroyed on different threads. This is made
// sure by destruction happening on correct thread. This class is not thread
// safe to be used concurrently on multiple thraeads.
//LCOV_EXCL_START
class HwVideoNativeBufferGLOwner::ScopedNativeBufferImpl
    : public ScopedNativeBufferFenceSync {
 public:
  ScopedNativeBufferImpl(
      scoped_refptr<HwVideoNativeBufferGLOwner> texture_owner,
      OhosWindowBuffer* image,
      ScopedNativeBufferHandle handle,
      base::ScopedFD fence_fd)
      : ScopedNativeBufferFenceSync(std::move(handle), std::move(fence_fd)),
        texture_owner_(std::move(texture_owner)),
        image_(image) {
    DCHECK(image_);
    texture_owner_->RegisterRefOnImageLocked(image_);
  }

  ~ScopedNativeBufferImpl() override {
    texture_owner_->ReleaseRefOnImage(image_, std::move(read_fence_));
  }

  void SetReadFence(base::ScopedFD fence_fd) final {
    read_fence_ = gl::MergeFDs(std::move(read_fence_), std::move(fence_fd));
  }

 private:
  base::ScopedFD read_fence_;
  scoped_refptr<HwVideoNativeBufferGLOwner> texture_owner_;
  raw_ptr<OhosWindowBuffer> image_;
};

#define IMAGE_BUFFER_SIZE 3
HwVideoNativeBufferGLOwner::HwVideoNativeBufferGLOwner(
    std::unique_ptr<AbstractTextureOHOS> texture,
    scoped_refptr<SharedContextState> context_state,
    scoped_refptr<RefCountedLock> drdc_lock)
    : NativeImageTextureOwner(false /* binds_texture_on_image_update */,
                              std::move(texture),
                              std::move(context_state)),
      RefCountedLockHelperDrDc(std::move(drdc_lock)),
      loader_(gl::OhosNativeImage::Create()),
      context_(gl::GLContext::GetCurrent()),
      surface_(gl::GLSurface::GetCurrent()) {
  DCHECK(context_);
  DCHECK(surface_);
  max_images_ = IMAGE_BUFFER_SIZE;
}

HwVideoNativeBufferGLOwner::~HwVideoNativeBufferGLOwner() {
  DCHECK_CALLED_ON_VALID_THREAD(gpu_main_thread_checker_);

  // Call ReleaseResources() if it hasn't already. This will do nothing if the
  // texture and other resources has already been destroyed due to context loss.
  ReleaseResources();

  DCHECK_EQ(image_refs_.size(), 0u);
}

void HwVideoNativeBufferGLOwner::ReleaseResources() {
  DCHECK_CALLED_ON_VALID_THREAD(gpu_main_thread_checker_);
  base::AutoLock auto_lock(lock_);
  // Either TextureOwner is being destroyed or the TextureOwner's shared context
  // is lost. Cleanup is it hasn't already.
  if (!loader_) {
    return;
  }

  TRACE_EVENT0("gpu", "HwVideoNativeBufferGLOwner::ReleaseResources");
  // Delete all images before closing the associated image reader.
  for (auto& image_ref : image_refs_) {
    int32_t return_code =
      loader_->ReleaseNativeWindowBuffer(image_ref.first->rawbuffer, -1);
    if (return_code != 0) {
      OHOS::NWeb::WindowAdapterNdkImpl::GetInstance()
        .NativeWindowUnRef(image_ref.first->rawbuffer);
    }
    image_ref.first = nullptr;
  }

  // Delete the image reader.
  loader_->ReleaseNativeImage();
  loader_ = nullptr;

  // Clean up the ImageRefs which should now be a no-op since there is no
  // valid |image_reader_|.
  image_refs_.clear();
  current_image_ref_.reset();
}

void HwVideoNativeBufferGLOwner::SetFrameAvailableCallback(
    const base::RepeatingClosure& frame_available_cb) {
  DCHECK(!is_frame_available_callback_set_);
  is_frame_available_callback_set_ = true;
  loader_->SetFrameAvailableCallback(std::move(frame_available_cb));
}

void HwVideoNativeBufferGLOwner::UpdateNativeImage() {
  TRACE_EVENT0("gpu", "HwVideoNativeBufferGLOwner::UpdateNativeImage");
  base::AutoLock auto_lock(lock_);
  // If we've lost the texture, then do nothing.
  if (!texture()) {
    return;
  }

  DCHECK(loader_);
  int acquire_fence_fd = -1;
  void *buffer = nullptr;

  int32_t return_code = 0;
  return_code =
      loader_->AcquireNativeWindowBuffer(&buffer, &acquire_fence_fd);
  // If there is no new image simply return. At this point previous image will
  // still be bound to the texture.
  if (return_code != 0 || !buffer) {
    LOG(ERROR) << "NativeImage: image is nullptr or acquire buffer fail :"
               << return_code;
    return;
  }

  base::ScopedFD scoped_acquire_fence_fd(acquire_fence_fd);
  OhosWindowBuffer* image = new OhosWindowBuffer();
  if (image) {
    image->rawbuffer = buffer;
    // Make the newly acquired image as current image.
    current_image_ref_.emplace(this, image, std::move(scoped_acquire_fence_fd));
  }
}

std::unique_ptr<ScopedNativeBufferFenceSync>
HwVideoNativeBufferGLOwner::GetNativeBuffer() {
  TRACE_EVENT0("gpu", "HwVideoNativeBufferGLOwner::GetNativeBuffer");

  base::AutoLock auto_lock(lock_);
  if (!current_image_ref_ || !loader_) {
    return nullptr;
  }
  OHOSNativeBuffer buffer;
  int32_t return_code = 0;
  return_code = loader_->GetNativeBuffer(current_image_ref_->image()->rawbuffer, &buffer);
  if (return_code != 0 || !buffer) {
    LOG(ERROR) << "GetNativeBuffer returned nullptr: ";
    return nullptr;
  }

  uint32_t seq_num = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                         .GetOhosNativeBufferAdapter()
                         .GetSeqNum(buffer);
  TRACE_EVENT1("gpu", "HwVideoNativeBufferGLOwner::GetNativeBuffer", "seqnum",
               seq_num);

  return std::make_unique<ScopedNativeBufferImpl>(
      this, current_image_ref_->image(),
      ScopedNativeBufferHandle::Create(buffer),
      current_image_ref_->GetReadyFence());
}

void HwVideoNativeBufferGLOwner::RegisterRefOnImageLocked(
    OhosWindowBuffer* image) {
  lock_.AssertAcquired();
  DCHECK(loader_);

  // Add a ref that the caller will release.
  image_refs_[image].count++;
}

void HwVideoNativeBufferGLOwner::ReleaseRefOnImage(OhosWindowBuffer* image,
                                                   base::ScopedFD fence_fd) {
  base::AutoLock auto_lock(lock_);
  ReleaseRefOnImageLocked(image, std::move(fence_fd));
}
//LCOV_EXCL_STOP

void HwVideoNativeBufferGLOwner::ReleaseRefOnImageLocked(
    OhosWindowBuffer* image,
    base::ScopedFD fence_fd) {
  TRACE_EVENT0("gpu", "HwVideoNativeBufferGLOwner::ReleaseRefOnImageLocked");
  lock_.AssertAcquired();
  // During cleanup on losing the texture, all images are synchronously released
  // and the |image_reader_| is destroyed.
  if (!loader_) {
    return;
  }

  // Ensure that DrDc lock is held when |buffer_available_cb| can be triggered
  // because we do not want any other thread to steal the free buffer slot which
  // is meant to be used by |buffer_available_cb| and hence resulting in wrong
  // FrameInfo for all future frames.
  AssertAcquiredDrDcLock();

  auto it = image_refs_.find(image);
  CHECK(it != image_refs_.end());

  auto& image_ref = it->second;
  DCHECK_GT(image_ref.count, 0u);
  image_ref.count--;
  image_ref.release_fence_fd =
      gl::MergeFDs(std::move(image_ref.release_fence_fd), std::move(fence_fd));

  if (image_ref.count > 0) {
    return;
  }

  TRACE_EVENT1("base", "HwVideoNativeBufferGLOwner::ReleaseRefOnImageLocked", "fd", image_ref.release_fence_fd.get());
  int return_code = 0;
  if (image_ref.release_fence_fd.is_valid()) {
    return_code = loader_->ReleaseNativeWindowBuffer(
        image->rawbuffer, std::move(image_ref.release_fence_fd.release()));
  } else {
    return_code = loader_->ReleaseNativeWindowBuffer(
        image->rawbuffer, -1);
  }

  if (return_code != 0) {
    LOG(WARNING) << "An error occured while releasing the native buffer";
    OHOS::NWeb::WindowAdapterNdkImpl::GetInstance()
        .NativeWindowUnRef(image->rawbuffer);
  }

  image_refs_.erase(it);
  delete image;
  image = nullptr;
  DCHECK_GT(max_images_, static_cast<int32_t>(image_refs_.size()));
  auto buffer_available_cb = std::move(buffer_available_cb_);
  // |buffer_available_cb| will try to acquire lock again via
  // UpdateNativeImage(), hence we need to unlock here. Note that when
  // |max_images_| is 1, this callback will always be empty here since it will
  // be run immediately in RunWhenBufferIsAvailable(). Hence resetting
  // |current_image_ref_| in UpdateNativeImage() can not trigger this callback.
  // Otherwise triggering this callback from UpdateNativeImage() on
  // |current_image_ref_| reset would cause callback and hence FrameInfoHelper
  // to run and eventually call UpdateNativeImage() from there which could have
  // been filmsy.
  if (buffer_available_cb) {
    base::AutoUnlock auto_unlock(lock_);
    DCHECK_GT(max_images_, 1);
    std::move(buffer_available_cb).Run();
  }
}

//LCOV_EXCL_START
gl::GLContext* HwVideoNativeBufferGLOwner::GetContext() const {
  DCHECK_CALLED_ON_VALID_THREAD(gpu_main_thread_checker_);
  return context_.get();
}

gl::GLSurface* HwVideoNativeBufferGLOwner::GetSurface() const {
  DCHECK_CALLED_ON_VALID_THREAD(gpu_main_thread_checker_);
  return surface_.get();
}
//LCOV_EXCL_STOP

void HwVideoNativeBufferGLOwner::RunWhenBufferIsAvailable(
    base::OnceClosure callback) {
  TRACE_EVENT0("base", "HwVideoNativeBufferGLOwner::RunWhenBufferIsAvailable");
  DCHECK_CALLED_ON_VALID_THREAD(gpu_main_thread_checker_);
  int image_refs_size = 0;
  {
    base::AutoLock auto_lock(lock_);
    // Note that we handle only one simultaneous request, this is not issue
    // because FrameInfoHelper maintain request queue and has only single
    // outstanding request on GPU thread.
    DCHECK(!buffer_available_cb_);
    image_refs_size = static_cast<int>(image_refs_.size());
  }
  // If `max_images` == 1 we will drop it before acquiring new buffer. Note
  // that this must never happen with SurfaceControl and the
  // HwVideoNativeBufferGLOwner is the sole owner of the images.
  if (max_images_ == 1 || image_refs_size < max_images_) {
    // This callback is run from here as well as from ReleaseRefOnImage() where
    // we remove one image from image reader queue before callback is run.
    // Once the |lock_| is dropped in this method here, another thread can
    // UpdateNativeImage() before callback is run and hence cause the image
    // reader queue to become full. In that case callback will not be able to
    // render and acquire updated image and hence will use FrameInfo of the
    // previous image which will result in wrong coded size for all future
    // frames. To avoid this, no other thread should try to UpdateNativeImage()
    // when this callback is run. Hence drdc_lock should be held from all the
    // places from where the callback could be run which is either
    // OnGpu::GetFrameInfo() or
    // HwVideoNativeBufferGLOwner::ReleaseRefOnImageLocked() and
    // OnGpu::GetFrameInfoImpl() should assume that the drdc_lock is always
    // held.
    std::move(callback).Run();
  } else {
    base::AutoLock auto_lock(lock_);
    buffer_available_cb_ = std::move(callback);
  }
}

bool HwVideoNativeBufferGLOwner::GetCodedSizeAndVisibleRect(
    gfx::Size rotated_visible_size,
    gfx::Size* coded_size,
    gfx::Rect* visible_rect) {
  base::AutoLock auto_lock(lock_);
  DCHECK(coded_size);
  DCHECK(visible_rect);

  if (!loader_ || !current_image_ref_) {
    *visible_rect = gfx::Rect();
    *coded_size = gfx::Size();
    return false;
  }

  uint32_t width = 0;
  uint32_t height = 0;
  loader_->GetNativeWindowBufferSize(current_image_ref_->image()->rawbuffer,
                                     &width, &height);

  *coded_size = gfx::Size(width, height);
  *visible_rect = gfx::Rect(*coded_size);

  LOG(DEBUG) << "GetCodedSizeAndVisibleRect rotated_visible_size width = "
             << rotated_visible_size.width()
             << ", height = " << rotated_visible_size.height()
             << "; coded_size width = " << width << ", height = " << height;
  return true;
}

//LCOV_EXCL_START
HwVideoNativeBufferGLOwner::ImageRef::ImageRef() = default;
HwVideoNativeBufferGLOwner::ImageRef::~ImageRef() = default;
HwVideoNativeBufferGLOwner::ImageRef::ImageRef(ImageRef&& other) = default;
HwVideoNativeBufferGLOwner::ImageRef&
HwVideoNativeBufferGLOwner::ImageRef::operator=(ImageRef&& other) = default;

HwVideoNativeBufferGLOwner::ScopedCurrentImageRef::ScopedCurrentImageRef(
    HwVideoNativeBufferGLOwner* texture_owner,
    OhosWindowBuffer* image,
    base::ScopedFD ready_fence)
    : texture_owner_(texture_owner),
      image_(image),
      ready_fence_(std::move(ready_fence)) {
  DCHECK(texture_owner_);
  texture_owner_->lock_.AssertAcquired();
  DCHECK(image_);
  texture_owner_->RegisterRefOnImageLocked(image_);
}

HwVideoNativeBufferGLOwner::ScopedCurrentImageRef::~ScopedCurrentImageRef() {
  texture_owner_->lock_.AssertAcquired();
  texture_owner_->ReleaseRefOnImageLocked(image_, std::move(ready_fence_));
}

base::ScopedFD
HwVideoNativeBufferGLOwner::ScopedCurrentImageRef::GetReadyFence() const {
  return base::ScopedFD(HANDLE_EINTR(dup(ready_fence_.get())));
}

void* HwVideoNativeBufferGLOwner::AquireOhosNativeWindow() const {
  TRACE_EVENT0("base", "HwVideoNativeBufferGLOwner::AquireOhosNativeWindow");
  DCHECK_CALLED_ON_VALID_THREAD(gpu_main_thread_checker_);

  if (loader_ != nullptr) {
    return loader_->AquireOhosNativeWindow();
  } else {
    return nullptr;
  }
}

void HwVideoNativeBufferGLOwner::GetSurfaceId(uint64_t* surface_id) {
  DCHECK_CALLED_ON_VALID_THREAD(gpu_main_thread_checker_);
  if (loader_) {
    loader_->GetSurfaceId(surface_id);
  }
}

void HwVideoNativeBufferGLOwner::EnsureNativeImageBound(GLuint service_id) {
  DCHECK_EQ(service_id, GetTextureId());
}

void HwVideoNativeBufferGLOwner::ReleaseNativeImage() {
  DCHECK_CALLED_ON_VALID_THREAD(gpu_main_thread_checker_);
  // ReleaseNativeImage() call is not required with HwVideoNativeBuffer.
}
//LCOV_EXCL_STOP
}  // namespace gpu
                   