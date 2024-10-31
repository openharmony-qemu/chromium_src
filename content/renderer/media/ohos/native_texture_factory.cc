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

#include "content/renderer/media/ohos/native_texture_factory.h"

#include "base/functional/bind.h"
#include "base/logging.h"
#include "gpu/ipc/client/client_shared_image_interface.h"
#include "gpu/ipc/client/gpu_channel_host.h"
#include "gpu/ipc/common/gpu_channel.mojom.h"
#include "mojo/public/cpp/bindings/pending_associated_remote.h"
#include "mojo/public/cpp/bindings/sync_call_restrictions.h"
#include "ui/gfx/geometry/size.h"

namespace content {

NativeTextureProxy::NativeTextureProxy(std::unique_ptr<StreamTextureHost> host,
                                       int32_t native_embed_id)
    : host_(std::move(host)), native_embed_id_(native_embed_id) {}

NativeTextureProxy::~NativeTextureProxy() {}

void NativeTextureProxy::Release() {
  // Cannot call |received_frame_cb_| after returning from here.
  ClearReceivedFrameCB();

  // |this| can be deleted by the |task_runner_| on the compositor thread by
  // posting task to that thread. So we need to clear the
  // |create_video_frame_cb_| here first so that its not called on the
  // compositor thread before |this| is deleted. The problem is that
  // |create_video_frame_cb_| is provided by the owner of NativeTextureProxy,
  // which is being destroyed and is releasing NativeTextureProxy.
  ClearCreateVideoFrameCB();

  ClearDestroyTextureCB();

  // Release is analogous to the destructor, so there should be no more external
  // calls to this object in Release. Therefore there is no need to acquire the
  // lock to access |task_runner_|.
  if (!task_runner_.get() || task_runner_->BelongsToCurrentThread() ||
      !task_runner_->DeleteSoon(FROM_HERE, this)) {
    delete this;
  }
}

void NativeTextureProxy::ClearReceivedFrameCB() {
  base::AutoLock lock(lock_);
  received_frame_cb_.Reset();
}

void NativeTextureProxy::ClearCreateVideoFrameCB() {
  base::AutoLock lock(lock_);
  create_video_frame_cb_.Reset();
}

void NativeTextureProxy::ClearDestroyTextureCB() {
  OnDestroySurface();
}

void NativeTextureProxy::BindToTaskRunner(
    const base::RepeatingClosure& received_frame_cb,
    const CreateVideoFrameCB& create_video_frame_cb,
    DestroyTextureCB destroy_texture_cb,
    scoped_refptr<base::SingleThreadTaskRunner> task_runner) {
  DCHECK(task_runner.get());

  {
    base::AutoLock lock(lock_);
    DCHECK(!task_runner_.get() || (task_runner.get() == task_runner_.get()));
    task_runner_ = task_runner;
    received_frame_cb_ = received_frame_cb;
    create_video_frame_cb_ = create_video_frame_cb;
    if (native_embed_id_ != -1) {
      destroy_texture_cb_ = std::move(destroy_texture_cb);
    }
  }

  if (task_runner->BelongsToCurrentThread()) {
    BindOnThread();
    return;
  }
  // Unretained is safe here only because the object is deleted on |loop_|
  // thread.
  task_runner->PostTask(FROM_HERE,
                        base::BindOnce(&NativeTextureProxy::BindOnThread,
                                       base::Unretained(this)));
}

void NativeTextureProxy::BindOnThread() {
  host_->BindToCurrentThread(this);
}

void NativeTextureProxy::OnFrameAvailable() {
  base::AutoLock lock(lock_);
  if (!received_frame_cb_.is_null())
    received_frame_cb_.Run();
}

void NativeTextureProxy::OnFrameWithInfoAvailable(
    const gpu::Mailbox& mailbox,
    const gfx::Size& coded_size,
    const gfx::Rect& visible_rect,
    const absl::optional<gpu::VulkanYCbCrInfo>& ycbcr_info) {
  base::AutoLock lock(lock_);
  // Set the ycbcr info before running the received frame callback so that the
  // first frame has it.
  if (!create_video_frame_cb_.is_null())
    create_video_frame_cb_.Run(mailbox, coded_size, visible_rect, ycbcr_info);
  if (!received_frame_cb_.is_null())
    received_frame_cb_.Run();
}

void NativeTextureProxy::UpdateRotatedVisibleSize(const gfx::Size& size) {
  base::AutoLock lock(lock_);
  if (!task_runner_)
    return;

  if (!task_runner_->BelongsToCurrentThread()) {
    // Note that Unretained is safe here because this object is deleted
    // exclusively by posting a task to the same task runner, after its owner
    // has dropped the only reference to it.
    task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&NativeTextureProxy::UpdateRotatedVisibleSize,
                                  base::Unretained(this), size));
    return;
  }
  host_->UpdateRotatedVisibleSize(size);
}

void NativeTextureProxy::OnDestroySurface() {
  base::AutoLock lock(lock_);
  if (!destroy_texture_cb_.is_null()) {
    std::move(destroy_texture_cb_).Run();
  }
}

// static
scoped_refptr<NativeTextureFactory> NativeTextureFactory::Create(
    scoped_refptr<gpu::GpuChannelHost> channel) {
  return new NativeTextureFactory(std::move(channel));
}

NativeTextureFactory::NativeTextureFactory(
    scoped_refptr<gpu::GpuChannelHost> channel)
    : channel_(std::move(channel)) {
  DCHECK(channel_);
}

NativeTextureFactory::~NativeTextureFactory() = default;

ScopedNativeTextureProxy NativeTextureFactory::CreateProxy(
    gl::ohos::TextureOwnerMode texture_owner_mode) {
  // Send a StreamTexture receiver down to the GPU process. This will be bound
  // to a concrete StreamTexture impl there.
  int32_t native_id = channel_->GenerateRouteID();
  mojo::SyncCallRestrictions::ScopedAllowSyncCall allow_sync;
  mojo::PendingAssociatedRemote<gpu::mojom::StreamTexture> remote;
  int32_t native_embed_id = -1;
  channel_->GetGpuChannel().CreateNativeTexture(
      native_id, (int32_t)texture_owner_mode,
      remote.InitWithNewEndpointAndPassReceiver(), &native_embed_id);
  if (native_embed_id == -1) {
    DLOG(ERROR) << "[NativeEmbed] Fail to create NativeTexture.";
    return ScopedNativeTextureProxy();
  }

  return ScopedNativeTextureProxy(
      new NativeTextureProxy(std::make_unique<StreamTextureHost>(
                                 channel_, native_id, std::move(remote)),
                             native_embed_id));
}

bool NativeTextureFactory::IsLost() const {
  return channel_->IsLost();
}

gpu::SharedImageInterface* NativeTextureFactory::SharedImageInterface() {
  if (shared_image_interface_)
    return shared_image_interface_.get();

  shared_image_interface_ = channel_->CreateClientSharedImageInterface();
  DCHECK(shared_image_interface_);
  return shared_image_interface_.get();
}

}  // namespace content
