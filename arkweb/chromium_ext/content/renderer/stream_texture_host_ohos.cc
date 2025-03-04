// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on stream_texture_host_android.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "content/renderer/stream_texture_host_ohos.h"

#include "base/unguessable_token.h"
#include "content/renderer/render_thread_impl.h"
#include "gpu/ipc/client/gpu_channel_host.h"
#include "gpu/ipc/common/command_buffer_id.h"
#include "gpu/ipc/common/gpu_channel.mojom.h"
#include "gpu/ipc/common/vulkan_ycbcr_info.h"
#include "ipc/ipc_message_macros.h"
#include "ipc/ipc_mojo_bootstrap.h"

namespace content {

StreamTextureHost::StreamTextureHost(
    scoped_refptr<gpu::GpuChannelHost> channel,
    int32_t route_id,
    mojo::PendingAssociatedRemote<gpu::mojom::StreamTexture> texture)
    : route_id_(route_id),
      listener_(nullptr),
      channel_(std::move(channel)),
      pending_texture_(std::move(texture)) {
  DCHECK(channel_);
  DCHECK(route_id_);
}

StreamTextureHost::~StreamTextureHost() {
  if (channel_) {
    OnDestroySurface();

    // We destroy the NativeTexture as a deferred message followed by a flush
    // to ensure this is ordered correctly with regards to previous deferred
    // messages, such as CreateSharedImage.
    uint32_t flush_id = channel_->EnqueueDeferredMessage(
        gpu::mojom::DeferredRequestParams::NewDestroyNativeTexture(route_id_),
        /*sync_token_fences=*/{}, /*release_count=*/0);
    channel_->EnsureFlush(flush_id);
  }
}

bool StreamTextureHost::BindToCurrentThread(Listener* listener) {
  listener_ = listener;
  if (!pending_texture_) {
    return false;
  }

  IPC::ScopedAllowOffSequenceChannelAssociatedBindings allow_off_thread_binding;

  texture_remote_.Bind(std::move(pending_texture_));
  texture_remote_->StartListening(receiver_.BindNewEndpointAndPassRemote());
  texture_remote_.set_disconnect_handler(
      base::BindOnce(&StreamTextureHost::OnDisconnectedFromGpuProcess,
                     base::Unretained(this)));
  return true;
}

void StreamTextureHost::OnDisconnectedFromGpuProcess() {
  channel_ = nullptr;
  texture_remote_.reset();
  receiver_.reset();
}

void StreamTextureHost::OnFrameWithInfoAvailable(
    const gpu::Mailbox& mailbox,
    const gfx::Size& coded_size,
    const gfx::Rect& visible_rect,
    absl::optional<gpu::VulkanYCbCrInfo> ycbcr_info) {
  TRACE_EVENT0("base", "StreamTextureHost::OnFrameWithInfoAvailable.");
  if (listener_) {
    listener_->OnFrameWithInfoAvailable(mailbox, coded_size, visible_rect,
                                        ycbcr_info);
  }
}

void StreamTextureHost::OnFrameAvailable() {
  TRACE_EVENT0("base", "StreamTextureHost::OnFrameAvailable.");
  if (listener_) {
    listener_->OnFrameAvailable();
  }
}

void StreamTextureHost::OnDestroySurface() {
  if (listener_) {
    listener_->OnDestroySurface();
  }
}

void StreamTextureHost::UpdateRotatedVisibleSize(const gfx::Size& size) {
  if (texture_remote_) {
    texture_remote_->UpdateRotatedVisibleSize(size);
  }
}

gpu::SyncToken StreamTextureHost::GenUnverifiedSyncToken() {
  // |channel_| can be set to null via OnDisconnectedFromGpuProcess() which
  // means StreamTextureHost could still be alive when |channel_| is gone.
  if (!channel_) {
    return gpu::SyncToken();
  }

  return gpu::SyncToken(gpu::CommandBufferNamespace::GPU_IO,
                        gpu::CommandBufferIdFromChannelAndRoute(
                            channel_->channel_id(), route_id_),
                        release_id_);
}

}  // namespace content
