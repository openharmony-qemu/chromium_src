/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 */

#ifndef CONTENT_RENDERER_MEDIA_OHOS_NATIVE_RENDERER_CLIENT_H_
#define CONTENT_RENDERER_MEDIA_OHOS_NATIVE_RENDERER_CLIENT_H_

#include <memory>

#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "media/base/native_texture_wrapper.h"
#include "media/base/video_renderer_sink.h"
#include "media/mojo/clients/mojo_renderer_wrapper.h"
#include "ui/gfx/geometry/size.h"

namespace content {

class NativeRenderClient : public media::MojoRendererWrapper {
 public:
  NativeRenderClient();
  NativeRenderClient(
      scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner,
      media::ScopedNativeTextureWrapper native_texture_wrapper,
      media::VideoRendererSink* sink);
  ~NativeRenderClient() override;

  // media::Renderer implementation (inherited from media::MojoRendererWrapper).
  // We override normal initialization to set up |stream_texture_wrapper_|.
  void Initialize(media::CreateTextureCB create_texture_cb,
                  media::DestroyTextureCB destroy_texture_cb) override;

  void OnSurfaceCreated(int native_embed_id);
  void OnSurfaceDestroy();

  void OnVideoSizeChange(const gfx::Size& size);

  // Called on |compositor_task_runner_| whenever |stream_texture_wrapper_| has
  // a new frame.
  void OnFrameAvailable();

  media::RendererType GetRendererType() override {
    return media::RendererType::kNative;
  }

 private:
  // Owns the StreamTexture whose surface is used by NativeRenderer.
  // Provides the VideoFrames to |sink_|.
  media::ScopedNativeTextureWrapper native_texture_wrapper_;

  media::VideoRendererSink* sink_;

  // Used by |native_texture_wrapper_| to signal OnFrameAvailable() and to send
  // VideoFrames to |sink_| on the right thread.
  scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner_;

  media::CreateTextureCB create_texture_cb_;
  media::DestroyTextureCB destroy_texture_cb_;

  // NOTE: Weak pointers must be invalidated before all other member variables.
  base::WeakPtrFactory<NativeRenderClient> weak_factory_{this};
};

}  // namespace content

#endif  // CONTENT_RENDERER_MEDIA_OHOS_NATIVE_RENDERER_CLIENT_H_
