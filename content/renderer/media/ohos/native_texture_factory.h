/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 */

#ifndef CONTENT_RENDERER_MEDIA_OHOS_NATIVE_TEXTURE_FACTORY_H_
#define CONTENT_RENDERER_MEDIA_OHOS_NATIVE_TEXTURE_FACTORY_H_

#include <stdint.h>

#include <memory>

#include "base/memory/ref_counted.h"
#include "base/task/single_thread_task_runner.h"
#include "base/unguessable_token.h"
#include "cc/layers/video_frame_provider.h"
#include "content/common/content_export.h"
#include "content/renderer/stream_texture_host_ohos.h"
#include "gpu/command_buffer/common/mailbox.h"
#include "gpu/ipc/common/gpu_channel.mojom.h"
#include "mojo/public/cpp/bindings/pending_associated_receiver.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace gpu {
class ClientSharedImageInterface;
class GpuChannelHost;
class SharedImageInterface;
struct VulkanYCbCrInfo;
}  // namespace gpu

namespace content {

class NativeTextureFactory;

using CreateSurfaceTextureCB = base::OnceCallback<void(int)>;
using DestroyTextureCB = base::OnceCallback<void()>;

// The proxy class for the gpu thread to notify the compositor thread
// when a new video frame is available.
class CONTENT_EXPORT NativeTextureProxy : public StreamTextureHost::Listener {
 public:
  using CreateVideoFrameCB = base::RepeatingCallback<void(
      const gpu::Mailbox& mailbox,
      const gfx::Size& coded_size,
      const gfx::Rect& visible_rect,
      const absl::optional<gpu::VulkanYCbCrInfo>&)>;

  NativeTextureProxy() = delete;
  NativeTextureProxy(const NativeTextureProxy&) = delete;
  NativeTextureProxy& operator=(const NativeTextureProxy&) = delete;

  ~NativeTextureProxy() override;

  // Initialize and bind to |task_runner|, which becomes the thread that the
  // provided callback will be run on. This can be called on any thread, but
  // must be called with the same |task_runner| every time.
  void BindToTaskRunner(
      const base::RepeatingClosure& received_frame_cb,
      const CreateVideoFrameCB& create_video_frame_cb,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner);

  // StreamTextureHost::DestroyListener implementation:
  void OnFrameAvailable() override;
  void OnFrameWithInfoAvailable(
      const gpu::Mailbox& mailbox,
      const gfx::Size& coded_size,
      const gfx::Rect& visible_rect,
      const absl::optional<gpu::VulkanYCbCrInfo>& ycbcr_info) override;
  void OnDestroySurface() override;

  // Notifies StreamTexture that video size has been changed and so it can
  // recreate shared image.
  void UpdateRotatedVisibleSize(const gfx::Size& size);

  // Clears |received_frame_cb_| in a thread safe way.
  void ClearReceivedFrameCB();

  // Clears |create_video_frame_cb_| in a thread safe way.
  void ClearCreateVideoFrameCB();

  struct Deleter {
    inline void operator()(NativeTextureProxy* ptr) const { ptr->Release(); }
  };

 private:
  friend class NativeTextureFactory;

  explicit NativeTextureProxy(std::unique_ptr<StreamTextureHost> host,
                              CreateSurfaceTextureCB create_texture_cb,
                              DestroyTextureCB destroy_texture_cb);
  void BindOnThread();
  void Release();

  const std::unique_ptr<StreamTextureHost> host_;

  // Protects access to |received_frame_cb_| and |task_runner_|.
  base::Lock lock_;
  base::RepeatingClosure received_frame_cb_;
  CreateVideoFrameCB create_video_frame_cb_;
  CreateSurfaceTextureCB create_texture_cb_;
  DestroyTextureCB destroy_texture_cb_;

  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
};

typedef std::unique_ptr<NativeTextureProxy, NativeTextureProxy::Deleter>
    ScopedNativeTextureProxy;

// Factory class for managing stream textures.
class CONTENT_EXPORT NativeTextureFactory
    : public base::RefCountedThreadSafe<NativeTextureFactory> {
 public:
  static scoped_refptr<NativeTextureFactory> Create(
      scoped_refptr<gpu::GpuChannelHost> channel);

  NativeTextureFactory() = delete;
  NativeTextureFactory(const NativeTextureFactory&) = delete;
  NativeTextureFactory& operator=(const NativeTextureFactory&) = delete;

  // Create the NativeTextureProxy object. This internally creates a
  // gpu::StreamTexture and returns its route_id. If this route_id is invalid
  // nullptr is returned. If the route_id is valid it returns
  // NativeTextureProxy object.
  ScopedNativeTextureProxy CreateProxy(CreateSurfaceTextureCB create_texture_cb,
                                       DestroyTextureCB destroy_texture_cb);

  // Returns true if the NativeTextureFactory's channel is lost.
  bool IsLost() const;

  gpu::SharedImageInterface* SharedImageInterface();

 private:
  friend class base::RefCountedThreadSafe<NativeTextureFactory>;
  NativeTextureFactory(scoped_refptr<gpu::GpuChannelHost> channel);
  ~NativeTextureFactory();

  scoped_refptr<gpu::GpuChannelHost> channel_;
  std::unique_ptr<gpu::ClientSharedImageInterface> shared_image_interface_;
};

}  // namespace content

#endif  // CONTENT_RENDERER_MEDIA_OHOS_NATIVE_TEXTURE_FACTORY_H_
