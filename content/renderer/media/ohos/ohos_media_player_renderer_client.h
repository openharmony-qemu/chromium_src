// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_MEDIA_OHOS_MEDIA_PLAYER_RENDERER_CLIENT_H_
#define CONTENT_RENDERER_MEDIA_OHOS_MEDIA_PLAYER_RENDERER_CLIENT_H_

#include <memory>

#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "content/common/content_export.h"
#include "media/base/media_resource.h"
#include "media/base/renderer.h"
#include "media/base/renderer_client.h"
#include "media/base/video_renderer_sink.h"
#include "media/base/video_util.h"
#include "media/mojo/clients/mojo_renderer.h"
#include "media/mojo/clients/mojo_renderer_wrapper.h"
#include "media/mojo/mojom/renderer_extensions.mojom.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"

namespace content {

struct CachedBuffer {
  int fd_browser_;
  int fd_;
  uint8_t* mapped_;
  uint32_t buffer_size_;
};

class OHOSMediaPlayerRendererClient
    : public media::mojom::MediaPlayerRendererClientExtension,
      public media::MojoRendererWrapper {
 public:
  using RendererExtention = media::mojom::MediaPlayerRendererExtension;
  using ClientExtention = media::mojom::MediaPlayerRendererClientExtension;
  OHOSMediaPlayerRendererClient(
      mojo::PendingRemote<RendererExtention> renderer_extension_remote,
      mojo::PendingReceiver<ClientExtention> client_extension_receiver,
      scoped_refptr<base::SequencedTaskRunner> media_task_runner,
      std::unique_ptr<media::MojoRenderer> mojo_renderer,
      media::VideoRendererSink* sink);
  ~OHOSMediaPlayerRendererClient() override;

  OHOSMediaPlayerRendererClient(const OHOSMediaPlayerRendererClient&) = delete;
  OHOSMediaPlayerRendererClient& operator=(
      const OHOSMediaPlayerRendererClient&) = delete;

  // Renderer implementation.
  void Initialize(media::MediaResource* media_resource,
                  media::RendererClient* client,
#ifdef OHOS_VIDEO_ASSISTANT
                  media::RequestSurfaceCB request_surface_cb,
                  media::VideoDecoderChangedCB decoder_changed_cb,
#endif // OHOS_VIDEO_ASSISTANT
                  media::PipelineStatusCallback init_cb) override;
  media::RendererType GetRendererType() override;

  // media::mojom::MediaPlayerRendererClientExtension implementation
  void OnDurationChange(base::TimeDelta duration) override;
  void OnVideoSizeChange(const gfx::Size& size) override;
  void OnFrameUpdate(media::mojom::OhosSurfaceBufferHandlePtr
                         ohos_surface_buffer_handle) override;

 private:
  void OnRemoteRendererInitialized(media::PipelineStatus status);
  void OnFinishPaintCallback();

  void PaintNV12VideoFrame(const gfx::Size& coded_size,
                           const gfx::Rect& visible_rect,
                           const gfx::Size& natural_size,
                           uint8_t* mapped,
                           const uint32_t& buffer_size,
                           const int& fd,
                           const int& fd_browser);

  media::MediaResource* media_resource_;

  std::deque<CachedBuffer> cached_buffers_;

  media::RendererClient* client_;

  media::VideoRendererSink* sink_;

  scoped_refptr<base::SequencedTaskRunner> media_task_runner_;

  media::PipelineStatusCallback init_cb_;

  std::vector<uint8_t> resize_buf_;

  // This class is constructed on the main task runner, and used on
  // |media_task_runner_|. These member are used to delay calls to Bind() for
  // |renderer_extension_ptr_| and |client_extension_binding_|, until we are on
  // |media_task_runner_|.
  // Both are set in the constructor, and consumed in Initialize().
  mojo::PendingReceiver<ClientExtention>
      delayed_bind_client_extension_receiver_;
  mojo::PendingRemote<RendererExtention>
      delayed_bind_renderer_extention_remote_;

  // Used to call methods on the MediaPlayerRenderer in the browser process.
  mojo::Remote<RendererExtention> renderer_extension_remote_;

  // Used to receive events from MediaPlayerRenderer in the browser process.
  mojo::Receiver<MediaPlayerRendererClientExtension> client_extension_receiver_{
      this};

  // NOTE: Weak pointers must be invalidated before all other member variables.
  base::WeakPtrFactory<OHOSMediaPlayerRendererClient> weak_factory_{this};
};

}  // namespace content

#endif  // CONTENT_RENDERER_MEDIA_OHOS_MEDIA_PLAYER_RENDERER_CLIENT_H_
