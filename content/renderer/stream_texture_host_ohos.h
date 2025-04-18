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

#ifndef CONTENT_RENDERER_STREAM_TEXTURE_HOST_OHOS_H_
#define CONTENT_RENDERER_STREAM_TEXTURE_HOST_OHOS_H_

#include <stdint.h>

#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "content/common/content_export.h"
#include "gpu/ipc/common/gpu_channel.mojom.h"
#include "mojo/public/cpp/bindings/associated_receiver.h"
#include "mojo/public/cpp/bindings/associated_remote.h"

namespace base {
class UnguessableToken;
}

namespace gfx {
class Rect;
class Size;
}  // namespace gfx

namespace gpu {
class GpuChannelHost;
struct Mailbox;
struct SyncToken;
struct VulkanYCbCrInfo;
}  // namespace gpu

namespace content {

// Class for handling all the IPC messages between the GPU process and
// StreamTextureProxy.
class CONTENT_EXPORT StreamTextureHost
    : public gpu::mojom::StreamTextureClient {
 public:
  StreamTextureHost() = delete;

  explicit StreamTextureHost(
      scoped_refptr<gpu::GpuChannelHost> channel,
      int32_t route_id,
      mojo::PendingAssociatedRemote<gpu::mojom::StreamTexture> texture);

  StreamTextureHost(const StreamTextureHost&) = delete;
  StreamTextureHost& operator=(const StreamTextureHost&) = delete;

  ~StreamTextureHost() override;

  // Listener class that is listening to the stream texture updates. It is
  // implemented by StreamTextureProxyImpl.
  class Listener {
   public:
    virtual void OnFrameAvailable() = 0;
    virtual void OnFrameWithInfoAvailable(
        const gpu::Mailbox& mailbox,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const absl::optional<gpu::VulkanYCbCrInfo>& ycbcr_info) = 0;
    virtual void OnDestroySurface() = 0;
    virtual ~Listener() {}
  };

  bool BindToCurrentThread(Listener* listener);
  void OnDisconnectedFromGpuProcess();

  void UpdateRotatedVisibleSize(const gfx::Size& size);
  gpu::SyncToken GenUnverifiedSyncToken();

 private:
  // gpu::mojom::StreamTextureClient:
  void OnFrameAvailable() override;
  void OnFrameWithInfoAvailable(
      const gpu::Mailbox& mailbox,
      const gfx::Size& coded_size,
      const gfx::Rect& visible_rect,
      absl::optional<gpu::VulkanYCbCrInfo> ycbcr_info) override;
  void OnDestroySurface();

  int32_t route_id_;
  raw_ptr<Listener> listener_;
  scoped_refptr<gpu::GpuChannelHost> channel_;
  uint32_t release_id_ = 0;

  // The StreamTextureHost may be created on another thread, but the Mojo
  // endpoints below are to be bound on the compositor thread. This holds the
  // pending renderer-side StreamTexture endpoint until it can be bound on the
  // compositor thread.
  mojo::PendingAssociatedRemote<gpu::mojom::StreamTexture> pending_texture_;

  // Receives client messages from a corresponding StreamTexture instance in
  // the GPU process.
  mojo::AssociatedReceiver<gpu::mojom::StreamTextureClient> receiver_{this};

  // Sends messages to a corresponding StreamTexture instance in the GPU
  // process.
  mojo::AssociatedRemote<gpu::mojom::StreamTexture> texture_remote_;

  base::WeakPtrFactory<StreamTextureHost> weak_ptr_factory_{this};
};

}  // namespace content

#endif  // CONTENT_RENDERER_STREAM_TEXTURE_HOST_OHOS_H_
