// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/ohos/same_layer_native_buffer_image_backing.h"

#include <sys/poll.h>

#include "base/logging.h"
#include "base/task/bind_post_task.h"
#include "components/viz/common/resources/resource_format_utils.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "gpu/command_buffer/service/abstract_texture_ohos.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder.h"
#include "gpu/command_buffer/service/memory_tracking.h"
#include "gpu/command_buffer/service/ohos/scoped_native_buffer_fence_sync.h"
#include "gpu/command_buffer/service/shared_context_state.h"
#include "gpu/command_buffer/service/shared_image/shared_image_backing.h"
#include "gpu/command_buffer/service/shared_image/shared_image_format_utils.h"
#include "gpu/command_buffer/service/shared_image/shared_image_representation.h"
#include "gpu/command_buffer/service/shared_image/skia_gl_image_representation.h"
#include "gpu/command_buffer/service/stream_texture_shared_image_interface.h"
#include "gpu/command_buffer/service/texture_manager.h"
#include "ui/gfx/color_space.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/gpu_fence_handle.h"
#include "ui/gl/android/egl_fence_utils.h"
#include "ui/gl/ohos/native_buffer_utils.h"


namespace gpu {

namespace {

void CreateAndBindEglImageFromNativeBuffer(OHOSNativeBuffer buffer,
                                           GLuint service_id) {
  DCHECK(buffer);

  EGLClientBuffer egl_client_buffer;
  if (gl::ohos::GetEGLClientBufferFromNativeBuffer(buffer,
                                                   &egl_client_buffer) != 0) {
    LOG(ERROR) << "Failed to get EGLClientBuffer!";
    return;
  }

  auto egl_image = gl::ohos::CreateEGLImage(egl_client_buffer);
  gl::ohos::FreeEGLClientBuffer(egl_client_buffer);
  if (egl_image == EGL_NO_IMAGE_KHR) {
    LOG(ERROR) << "Failed to create EGLImage! ";
    return;
  }

  glBindTexture(GL_TEXTURE_EXTERNAL_OES, service_id);
  glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, egl_image.get());
}

bool SyncFenceWait(base::ScopedFD acquire_fence_fd) {
  int fence_fd = acquire_fence_fd.get();
  TRACE_EVENT2("base", __FILE__, "func", __func__, "sync_fd", fence_fd);

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

}  // namespace

SameLayerNativeBufferImageBacking::SameLayerNativeBufferImageBacking(
    const Mailbox& mailbox,
    const gfx::Size& size,
    const gfx::ColorSpace color_space,
    GrSurfaceOrigin surface_origin,
    SkAlphaType alpha_type,
    scoped_refptr<StreamTextureSharedImageInterface> stream_texture_sii,
    scoped_refptr<SharedContextState> context_state,
    scoped_refptr<RefCountedLock> drdc_lock)
    : OhosVideoImageBacking(mailbox,
                            size,
                            color_space,
                            surface_origin,
                            alpha_type,
                            !!drdc_lock),
      RefCountedLockHelperDrDc(std::move(drdc_lock)),
      stream_texture_sii_(std::move(stream_texture_sii)),
      gpu_main_task_runner_(base::SingleThreadTaskRunner::GetCurrentDefault()) {
  DCHECK(stream_texture_sii_);

  context_lost_helper_ = std::make_unique<ContextLostObserverHelper>(
      std::move(context_state), stream_texture_sii_, gpu_main_task_runner_,
      GetDrDcLock());
}

SameLayerNativeBufferImageBacking::~SameLayerNativeBufferImageBacking() {
  // This backing is created on gpu main thread but can be destroyed on DrDc
  // thread if the last representation was on DrDc thread.
  // |context_lost_helper_| is destroyed here by posting task to the
  // |gpu_main_thread_| to ensure that resources are cleaned up correvtly on
  // the gpu main thread.
  if (!gpu_main_task_runner_->RunsTasksInCurrentSequence()) {
    auto helper_destruction_cb = base::BindPostTask(
        gpu_main_task_runner_,
        base::BindOnce(
            [](std::unique_ptr<ContextLostObserverHelper> context_lost_helper,
               scoped_refptr<StreamTextureSharedImageInterface>
                   stream_texture_sii) {
              // Reset the |stream_texture_sii| first so that its ref in the
              // |context_lost_helper| gets reset under the DrDc lock.
              stream_texture_sii.reset();
              context_lost_helper.reset();
            }));
    std::move(helper_destruction_cb)
        .Run(std::move(context_lost_helper_), std::move(stream_texture_sii_));
  }
}

// Representation of SameLayerNativeBufferImageBacking as a GL Texture.
class SameLayerNativeBufferImageBacking::GLTextureVideoImageRepresentation
    : public GLTextureImageRepresentation,
      public RefCountedLockHelperDrDc {
 public:
  GLTextureVideoImageRepresentation(
      SharedImageManager* manager,
      SameLayerNativeBufferImageBacking* backing,
      MemoryTypeTracker* tracker,
      std::unique_ptr<AbstractTextureOHOS> texture,
      scoped_refptr<RefCountedLock> drdc_lock)
      : GLTextureImageRepresentation(manager, backing, tracker),
        RefCountedLockHelperDrDc(std::move(drdc_lock)),
        texture_(std::move(texture)) {}

  ~GLTextureVideoImageRepresentation() override {
    if (!has_context()) {
      texture_->NotifyOnContextLost();
    }
  }

  // Disallow copy and assign.
  GLTextureVideoImageRepresentation(const GLTextureVideoImageRepresentation&) =
      delete;
  GLTextureVideoImageRepresentation& operator=(
      const GLTextureVideoImageRepresentation&) = delete;

  gles2::Texture* GetTexture(int plane_index) override {
    DCHECK_EQ(plane_index, 0);

    auto* texture = gles2::Texture::CheckedCast(texture_->GetTextureBase());
    DCHECK(texture);

    return texture;
  }

  bool BeginAccess(GLenum mode) override {
    TRACE_EVENT0("gpu",
                 "SameLayerNativeBufferImageBacking::"
                 "GLTextureVideoImageRepresentation::BeginAccess");
    // This representation should only be called for read.
    DCHECK(mode == GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);

    auto* native_buffer_backing =
        static_cast<SameLayerNativeBufferImageBacking*>(backing());
    {
      base::AutoLockMaybe auto_lock(GetDrDcLockPtr());
      scoped_native_buffer_ =
          native_buffer_backing->stream_texture_sii_->GetNativeBuffer();
    }
    if (!scoped_native_buffer_) {
      LOG(ERROR) << "Failed to get the hardware buffer.";
      return false;
    }

    SyncFenceWait(scoped_native_buffer_->TakeFence());
    CreateAndBindEglImageFromNativeBuffer(scoped_native_buffer_->buffer(),
                                          texture_->service_id());
    return true;
  }

  void EndAccess() override {
    DCHECK(scoped_native_buffer_);
    TRACE_EVENT0("gpu",
                 "SameLayerNativeBufferImageBacking::"
                 "GLTextureVideoImageRepresentation::EndAccess");
    base::ScopedFD sync_fd = gl::CreateEglFenceAndExportFd();

    scoped_native_buffer_->SetReadFence(std::move(sync_fd));
    base::AutoLockMaybe auto_lock(GetDrDcLockPtr());
    scoped_native_buffer_ = nullptr;
  }

 private:
  std::unique_ptr<AbstractTextureOHOS> texture_;
  std::unique_ptr<ScopedNativeBufferFenceSync> scoped_native_buffer_;
};

std::unique_ptr<GLTextureImageRepresentation>
SameLayerNativeBufferImageBacking::ProduceGLTexture(
    SharedImageManager* manager,
    MemoryTypeTracker* tracker) {
  base::AutoLockMaybe auto_lock(GetDrDcLockPtr());

  // For (old) overlays, we don't have a texture owner, but overlay promotion
  // might not happen for some reasons. In that case, it will try to draw
  // which should result in no image.
  if (!stream_texture_sii_->HasTextureOwner()) {
    return nullptr;
  }

  // Generate an abstract texture.
  auto texture = GenAbstractTexture(/*passthrough=*/false);
  if (!texture) {
    return nullptr;
  }

  return std::make_unique<GLTextureVideoImageRepresentation>(
      manager, this, tracker, std::move(texture), GetDrDcLock());
}

std::unique_ptr<SkiaGaneshImageRepresentation>
SameLayerNativeBufferImageBacking::ProduceSkiaGanesh(
    SharedImageManager* manager,
    MemoryTypeTracker* tracker,
    scoped_refptr<SharedContextState> context_state) {
  TRACE_EVENT0("gpu",
               "SameLayerNativeBufferImageBacking::ProduceSkiaGanesh");
  base::AutoLockMaybe auto_lock(GetDrDcLockPtr());

  DCHECK(context_state);

  // For (old) overlays, we don't have a texture owner, but overlay promotion
  // might not happen for some reasons. In that case, it will try to draw
  // which should result in no image.
  if (!stream_texture_sii_->HasTextureOwner()) {
    return nullptr;
  }

  DCHECK(context_state->GrContextIsGL());

  auto texture = GenAbstractTexture(false);
  if (!texture) {
    return nullptr;
  }

  std::unique_ptr<GLTextureImageRepresentationBase> gl_representation;
  gl_representation = std::make_unique<GLTextureVideoImageRepresentation>(
      manager, this, tracker, std::move(texture), GetDrDcLock());

  return SkiaGLImageRepresentation::Create(std::move(gl_representation),
                                           std::move(context_state), manager,
                                           this, tracker);
}

SameLayerNativeBufferImageBacking::ContextLostObserverHelper::
    ContextLostObserverHelper(
        scoped_refptr<SharedContextState> context_state,
        scoped_refptr<StreamTextureSharedImageInterface> stream_texture_sii,
        scoped_refptr<base::SingleThreadTaskRunner> gpu_main_task_runner,
        scoped_refptr<RefCountedLock> drdc_lock)
    : RefCountedLockHelperDrDc(std::move(drdc_lock)),
      context_state_(std::move(context_state)),
      stream_texture_sii_(std::move(stream_texture_sii)),
      gpu_main_task_runner_(std::move(gpu_main_task_runner)) {
  DCHECK(context_state_);
  DCHECK(stream_texture_sii_);

  context_state_->AddContextLostObserver(this);
}

SameLayerNativeBufferImageBacking::ContextLostObserverHelper::
    ~ContextLostObserverHelper() {
  DCHECK(gpu_main_task_runner_->RunsTasksInCurrentSequence());

  if (context_state_) {
    context_state_->RemoveContextLostObserver(this);
  }
  {
    base::AutoLockMaybe auto_lock(GetDrDcLockPtr());
    stream_texture_sii_->ReleaseResources();
    stream_texture_sii_.reset();
  }
}

// SharedContextState::ContextLostObserver implementation.
void SameLayerNativeBufferImageBacking::ContextLostObserverHelper::
    OnContextLost() {
  DCHECK(gpu_main_task_runner_->RunsTasksInCurrentSequence());
  base::AutoLockMaybe auto_lock(GetDrDcLockPtr());

  // We release codec buffers when shared image context is lost. This is
  // because texture owner's texture was created on shared context. Once
  // shared context is lost, no one should try to use that texture.
  stream_texture_sii_->ReleaseResources();
  context_state_->RemoveContextLostObserver(this);
  context_state_ = nullptr;
}
}  // namespace gpu
