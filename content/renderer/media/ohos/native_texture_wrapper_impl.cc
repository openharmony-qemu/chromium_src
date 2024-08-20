/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "content/renderer/media/ohos/native_texture_wrapper_impl.h"

#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "base/task/bind_post_task.h"
#include "cc/layers/video_frame_provider.h"
#include "gpu/GLES2/gl2extchromium.h"
#include "gpu/command_buffer/client/shared_image_interface.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "base/task/bind_post_task.h"

namespace {
// Non-member function to allow it to run even after this class is deleted.
void OnReleaseVideoFrame(scoped_refptr<content::NativeTextureFactory> factories,
                         gpu::Mailbox mailbox,
                         const gpu::SyncToken& sync_token) {
  gpu::SharedImageInterface* sii = factories->SharedImageInterface();
  sii->DestroySharedImage(sync_token, mailbox);
  sii->Flush();
}
}  // namespace

namespace content {

NativeTextureWrapperImpl::NativeTextureWrapperImpl(
    bool enable_texture_copy,
    scoped_refptr<NativeTextureFactory> factory,
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner)
    : enable_texture_copy_(enable_texture_copy),
      factory_(factory),
      main_task_runner_(main_task_runner) {}

NativeTextureWrapperImpl::~NativeTextureWrapperImpl() {
  DCHECK(main_task_runner_->BelongsToCurrentThread());

  // Clears create video frame callback so it couldn't be called from compositor
  // thread after |this| is being destroyed.
  if (native_texture_proxy_)
    native_texture_proxy_->ClearCreateVideoFrameCB();

  SetCurrentFrameInternal(nullptr);
}

media::ScopedNativeTextureWrapper NativeTextureWrapperImpl::Create(
    bool enable_texture_copy,
    scoped_refptr<NativeTextureFactory> factory,
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner) {
  return media::ScopedNativeTextureWrapper(new NativeTextureWrapperImpl(
      enable_texture_copy, factory, main_task_runner));
}

scoped_refptr<media::VideoFrame> NativeTextureWrapperImpl::GetCurrentFrame() {
  base::AutoLock auto_lock(current_frame_lock_);
  return current_frame_;
}

void NativeTextureWrapperImpl::CreateVideoFrame(
    const gpu::Mailbox& mailbox,
    const gfx::Size& coded_size,
    const gfx::Rect& visible_rect,
    const absl::optional<gpu::VulkanYCbCrInfo>& ycbcr_info) {
  // This message comes from GPU process when the SharedImage is already
  // created, so we don't need to wait on any synctoken, mailbox is ready to
  // use.
  gpu::MailboxHolder holders[media::VideoFrame::kMaxPlanes] = {
      gpu::MailboxHolder(mailbox, gpu::SyncToken(), GL_TEXTURE_EXTERNAL_OES)};

  gpu::SharedImageInterface* sii = factory_->SharedImageInterface();
  sii->NotifyMailboxAdded(mailbox, gpu::SHARED_IMAGE_USAGE_DISPLAY_READ |
                                       gpu::SHARED_IMAGE_USAGE_GLES2 |
                                       gpu::SHARED_IMAGE_USAGE_RASTER);

  // The pixel format doesn't matter here as long as it's valid for texture
  // frames. But SkiaRenderer wants to ensure that the format of the resource
  // used here which will eventually create a promise image must match the
  // format of the resource(SharedImageVideo) used to create fulfill image.
  // crbug.com/1028746. Since we create all the textures/abstract textures as
  // well as shared images for video to be of format RGBA, we need to use the
  // pixel format as ABGR here(which corresponds to 32bpp RGBA).
  scoped_refptr<media::VideoFrame> new_frame =
      media::VideoFrame::WrapNativeTextures(
          media::PIXEL_FORMAT_ABGR, holders,
          base::BindPostTask(
              main_task_runner_,
              base::BindOnce(&OnReleaseVideoFrame, factory_, mailbox)),
          coded_size, visible_rect, visible_rect.size(), base::TimeDelta());
  new_frame->set_ycbcr_info(ycbcr_info);

  if (enable_texture_copy_) {
    new_frame->metadata().copy_required = true;
  }

  SetCurrentFrameInternal(new_frame);
}

void NativeTextureWrapperImpl::ClearCBOnAnyThread() {
  // Safely stop StreamTextureProxy from signaling the arrival of new frames.
  if (native_texture_proxy_) {
    native_texture_proxy_->ClearReceivedFrameCB();
    native_texture_proxy_->ClearDestroyTextureCB();
  }
}

void NativeTextureWrapperImpl::SetCurrentFrameInternal(
    scoped_refptr<media::VideoFrame> video_frame) {
  base::AutoLock auto_lock(current_frame_lock_);
  current_frame_ = std::move(video_frame);
}

void NativeTextureWrapperImpl::UpdateTextureSize(const gfx::Size& new_size) {
  DVLOG(2) << __func__;

  if (!main_task_runner_->BelongsToCurrentThread()) {
    main_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&NativeTextureWrapperImpl::UpdateTextureSize,
                                  weak_factory_.GetWeakPtr(), new_size));
    return;
  }

  // InitializeOnMainThread() hasn't run, or failed.
  if (!native_texture_proxy_)
    return;

  if (rotated_visible_size_ == new_size)
    return;

  rotated_visible_size_ = new_size;
  native_texture_proxy_->UpdateRotatedVisibleSize(rotated_visible_size_);
}

void NativeTextureWrapperImpl::Initialize(
    const base::RepeatingClosure& received_frame_cb,
    scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner,
    StreamTextureWrapperInitCB init_cb,
    CreateSurfaceTextureCB create_texture_cb,
    DestroyTextureCB destroy_texture_cb) {
  LOG(INFO) << "[NativeEmbed] NativeTextureWrapperImpl::Initialize.";
  compositor_task_runner_ = compositor_task_runner;

  main_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(
          &NativeTextureWrapperImpl::InitializeOnMainThread,
          weak_factory_.GetWeakPtr(), received_frame_cb,
          base::BindPostTaskToCurrentDefault(std::move(init_cb)),
          base::BindPostTaskToCurrentDefault(std::move(create_texture_cb)),
          base::BindPostTaskToCurrentDefault(std::move(destroy_texture_cb))));
}

void NativeTextureWrapperImpl::InitializeOnMainThread(
    const base::RepeatingClosure& received_frame_cb,
    StreamTextureWrapperInitCB init_cb,
    CreateSurfaceTextureCB create_texture_cb,
    DestroyTextureCB destroy_texture_cb) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());

  if (!factory_) {
    std::move(init_cb).Run(false);
    return;
  }

  native_texture_proxy_ = factory_->CreateProxy();
  if (!native_texture_proxy_) {
    std::move(init_cb).Run(false);
    return;
  }

  // Unretained is safe here since |native_texture_proxy_| is a scoped member of
  // the this NativeTextureWrapperImpl class which clears/resets this callback
  // before |this| is destroyed.
  native_texture_proxy_->BindToTaskRunner(
      received_frame_cb,
      base::BindRepeating(&NativeTextureWrapperImpl::CreateVideoFrame,
                          base::Unretained(this)),
      std::move(destroy_texture_cb), compositor_task_runner_);

  std::move(init_cb).Run(true);

  auto native_embed_id = native_texture_proxy_->current_native_embed_id();
  if (native_embed_id != -1) {
    LOG(INFO)
        << "[NativeEmbed] Running create_texture_cb(OnCreateSurface) callback.";
    std::move(create_texture_cb).Run(native_embed_id);
  }
}

void NativeTextureWrapperImpl::Destroy() {
  if (!main_task_runner_->BelongsToCurrentThread()) {
    // base::Unretained is safe here because this function is the only one that
    // can call delete.
    main_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&NativeTextureWrapperImpl::Destroy,
                                  base::Unretained(this)));
    return;
  }

  delete this;
}

}  // namespace content
