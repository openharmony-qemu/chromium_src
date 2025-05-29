/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef GPU_IPC_SERVICE_GPU_CHANNEL_H_
#error "must be in include form GPU_IPC_SERVICE_GPU_CHANNEL_H_"
#endif

#include "arkweb/chromium_ext/gpu/ipc/service/gpu_channel_ext.h"
#include "gpu/ipc/service/gpu_channel.h"

namespace gpu {

namespace {
#if BUILDFLAG(ARKWEB_SAME_LAYER)
int32_t TryCreateNativeTexture(
    base::WeakPtr<GpuChannel> channel,
    int32_t native_id,
    gl::ohos::TextureOwnerMode texture_owner_mode,
    mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver) {
  if (!channel) {
    return -1;
  }
  channel->AsGpuChannelExt()->CreateNativeTexture(native_id, texture_owner_mode,
                               std::move(receiver));
  return channel->AsGpuChannelExt()->current_native_embed_id(native_id);
}
#endif
} // namespace

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void GpuChannelMessageFilter::CreateNativeTexture(int32_t native_id,
                                          int32_t texture_owner_mode,
                                          mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver,
                                          mojom::GpuChannel::CreateNativeTextureCallback callback) {
  base::AutoLock auto_lock(gpu_channel_lock_);
  if (!gpu_channel_) {
    receiver_.reset();
    return;
  }

  main_task_runner_->PostTaskAndReplyWithResult(
      FROM_HERE,
      base::BindOnce(&TryCreateNativeTexture, gpu_channel_->AsWeakPtr(),
                     native_id, (gl::ohos::TextureOwnerMode)texture_owner_mode,
                     std::move(receiver)),
      std::move(callback));
}
#endif

} // namespace gpu