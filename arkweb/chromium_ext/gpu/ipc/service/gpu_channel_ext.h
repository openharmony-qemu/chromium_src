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
#ifndef GPU_IPC_SERVICE_GPU_CHANNEL_EXT_H_
#define GPU_IPC_SERVICE_GPU_CHANNEL_EXT_H_

#include "gpu/ipc/service/gpu_channel.h"
#include "base/functional/callback.h"
#include "gpu/ipc/common/gpu_channel.mojom.h"
#if BUILDFLAG(ARKWEB_SAME_LAYER)
#include "ui/gl/ohos/native_buffer_utils.h"
#endif

namespace gpu {
class GpuChannel;

class GpuChannelExt : public GpuChannel {
 public:
  GpuChannelExt(GpuChannelManager* gpu_channel_manager,
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
                 GpuMemoryBufferFactory* gpu_memory_buffer_factory);
  ~GpuChannelExt() override;

  gpu::GpuChannelExt* AsGpuChannelExt() override {
    return this;
  }
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  int32_t CreateNativeTexture(
      int32_t native_id,
      gl::ohos::TextureOwnerMode texture_owner_mode,
      mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver);

  // Called by StreamTexture to remove the GpuChannel's reference to the
  // StreamTexture.
  void DestroyNativeTexture(int32_t stream_id);
  int32_t current_native_embed_id(int32_t native_id);
#endif
 private:
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  // Set of active NativeTextures.
  base::flat_map<int32_t, scoped_refptr<StreamTexture>> native_textures_;
#endif
};
}// namespace gpu

#endif  // GPU_IPC_SERVICE_GPU_CHANNEL_EXT_H_