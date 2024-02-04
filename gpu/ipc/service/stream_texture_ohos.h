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

#ifndef GPU_IPC_SERVICE_STREAM_TEXTURE_OHOS_H_
#define GPU_IPC_SERVICE_STREAM_TEXTURE_OHOS_H_

#include <stdint.h>

#include <memory>

#include "base/memory/raw_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "base/threading/thread_checker.h"
#include "base/unguessable_token.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/command_buffer/service/shared_context_state.h"
#include "gpu/command_buffer/service/ohos/native_image_texture_owner.h"
#include "gpu/command_buffer/service/stream_texture_shared_image_interface.h"
#include "gpu/ipc/common/gpu_channel.mojom.h"
#include "gpu/ipc/service/command_buffer_stub.h"
#include "mojo/public/cpp/bindings/associated_receiver.h"
#include "mojo/public/cpp/bindings/associated_remote.h"

namespace gfx {
class Size;
}

namespace gpu {
class GpuChannel;
struct Mailbox;

// This class is thread safe to be used by multiple gpu threads as
// |texture_owner_| is thread safe and all other members are only accessed on
// gpu main thread.
class StreamTexture : public StreamTextureSharedImageInterface,
                      public mojom::StreamTexture {
 public:
  static scoped_refptr<StreamTexture> Create(
      GpuChannel* channel,
      int stream_id,
      mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver);

  StreamTexture(const StreamTexture&) = delete;
  StreamTexture& operator=(const StreamTexture&) = delete;

  // Cleans up related data and nulls |channel_|. Called when the channel
  // releases its ref on this class.
  void ReleaseChannel();
  int NativeEmbedID();

 private:
  StreamTexture(GpuChannel* channel,
                int32_t route_id,
                mojo::PendingAssociatedReceiver<mojom::StreamTexture> receiver,
                scoped_refptr<SharedContextState> context_state);
  ~StreamTexture() override;

  // Static function which is used to access |weak_stream_texture| on correct
  // thread since WeakPtr is not thread safe.
  static void RunCallback(
      scoped_refptr<base::SingleThreadTaskRunner> task_runner,
      base::WeakPtr<StreamTexture> weak_stream_texture);

  // gpu::StreamTextureSharedImageInterface implementation.
  void ReleaseResources() override {}
  bool IsUsingGpuMemory() const override;
  void UpdateAndBindTexImage(GLuint service_id) override;
  bool HasTextureOwner() const override;
  TextureBase* GetTextureBase() const override;
  void NotifyOverlayPromotion(bool promotion, const gfx::Rect& bounds) override;
  bool RenderToOverlay() override;
  bool TextureOwnerBindsTextureOnUpdate() override;

  gpu::Mailbox CreateSharedImage(const gfx::Size& coded_size);

  // Called when a new frame is available for the SurfaceOwner.
  void OnFrameAvailable();

  // mojom::StreamTexture:
  void StartListening(mojo::PendingAssociatedRemote<mojom::StreamTextureClient>
                          client) override;
  void UpdateRotatedVisibleSize(const gfx::Size& natural_size) override;

  // The NativeImageTextureOwner which receives frames.
  scoped_refptr<NativeImageTextureOwner> native_texture_owner_;

  // Current visible size from media player, includes rotation.
  gfx::Size rotated_visible_size_;

  // Whether a new frame is available that we should update to.
  bool has_pending_frame_;

  int native_embed_id_{-1};

  raw_ptr<GpuChannel> channel_;
  const int32_t route_id_;
  scoped_refptr<SharedContextState> context_state_;
  SequenceId sequence_;

  mojo::AssociatedReceiver<mojom::StreamTexture> receiver_;
  mojo::AssociatedRemote<mojom::StreamTextureClient> client_;

  gfx::Size coded_size_;
  gfx::Rect visible_rect_;

  // Bound to the thread on which StreamTexture is created. Some methods can
  // only be called on this thread. StreamTexture is created on gpu main thread.
  THREAD_CHECKER(gpu_main_thread_checker_);

  base::WeakPtrFactory<StreamTexture> weak_factory_{this};
};

}  // namespace gpu

#endif  // GPU_IPC_SERVICE_STREAM_TEXTURE_OHOS_H_
