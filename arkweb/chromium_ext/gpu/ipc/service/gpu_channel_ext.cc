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
#include "arkweb/chromium_ext/gpu/ipc/service/gpu_channel_ext.h"
#if BUILDFLAG(ARKWEB_SAME_LAYER)
#include "arkweb/chromium_ext/gpu/ipc/service/stream_texture_ohos.h"
#endif
#include "gpu/ipc/service/gpu_channel_manager.h"
#include "gpu/ipc/common/gpu_channel.mojom.h"
#include "base/task/single_thread_task_runner.h"

namespace gpu {

GpuChannelExt::GpuChannelExt(GpuChannelManager* gpu_channel_manager,
                             const base::UnguessableToken& channel_token,
                             Scheduler* scheduler,
                             SyncPointManager* sync_point_manager,
                             scoped_refptr<gl::GLShareGroup> share_group,
                             scoped_refptr<base::SingleThreadTaskRunner> task_runner,
                             scoped_refptr<base::SingleThreadTaskRunner> io_task_runner,
                             int32_t client_id,
                             uint64_t client_tracing_id,
                             bool is_gpu_host,
                             ImageDecodeAcceleratorWorker* image_decode_accelerator_worker,
                             const gfx::GpuExtraInfo& gpu_extra_info,
                             GpuMemoryBufferFactory* gpu_memory_buffer_factory)
    : GpuChannel(gpu_channel_manager,
                 channel_token,
                 scheduler,
                 sync_point_manager,
                 share_group,
                 task_runner,
                 io_task_runner,
                 client_id,
                 client_tracing_id,
                 is_gpu_host,
                 image_decode_accelerator_worker,
                 gpu_extra_info,
                 gpu_memory_buffer_factory) {}

GpuChannelExt::~GpuChannelExt() {
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  // Release any references to this channel held by StreamTexture.
  for (auto& native_texture : native_textures_) {
    native_texture.second->ReleaseChannel();
  }
  native_textures_.clear();
#endif
}


#if BUILDFLAG(ARKWEB_SAME_LAYER)
int32_t GpuChannelExt::CreateNativeTexture(
    int32_t native_id,
    gl::ohos::TextureOwnerMode texture_owner_mode,
    mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver) {
  auto found = native_textures_.find(native_id);
  if (found != native_textures_.end()) {
    LOG(ERROR) << "[NativeEmbed] Trying to create a StreamTexture with an "
                  "existing native_id.";
    return -1;
  }
  scoped_refptr<StreamTexture> native_texture = StreamTexture::Create(
      this, native_id, texture_owner_mode, std::move(receiver));

  if (!native_texture) {
    return -1;
  }
  native_textures_.emplace(native_id, std::move(native_texture));

  return current_native_embed_id(native_id);
}

void GpuChannelExt::DestroyNativeTexture(int32_t native_id) {
  auto found = native_textures_.find(native_id);
  if (found == native_textures_.end()) {
    LOG(ERROR)
        << "[NativeEmbed] Trying to destroy a non-existent native texture.";
    return;
  }
  found->second->ReleaseChannel();
  native_textures_.erase(native_id);
}

int32_t GpuChannelExt::current_native_embed_id(int32_t native_id) {
  return native_textures_[native_id]->NativeEmbedID();
}
#endif

}
