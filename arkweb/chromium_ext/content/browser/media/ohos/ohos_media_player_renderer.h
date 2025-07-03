// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_MEDIA_OHOS_MEDIA_PLAYER_RENDERER_H_
#define CONTENT_BROWSER_MEDIA_OHOS_MEDIA_PLAYER_RENDERER_H_

#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"
#include "base/timer/timer.h"
#include "base/unguessable_token.h"
#include "content/common/content_export.h"
#include "content/public/browser/render_frame_host.h"
#include "media/base/media_log.h"
#include "media/base/media_resource.h"
#include "media/base/ohos/ohos_media_player_bridge.h"
#include "media/base/renderer.h"
#include "media/base/renderer_client.h"
#include "media/mojo/mojom/renderer_extensions.mojom.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "url/gurl.h"

namespace content {
class WebContents;
class OHOSMediaPlayerRendererWebContentsObserver;

class CONTENT_EXPORT OHOSMediaPlayerRenderer
    : public media::Renderer,
      public media::mojom::MediaPlayerRendererExtension,
      public media::OHOSMediaPlayerBridge::Client {
 public:
  using RendererExtension = media::mojom::MediaPlayerRendererExtension;
  using ClientExtension = media::mojom::MediaPlayerRendererClientExtension;

  OHOSMediaPlayerRenderer(const OHOSMediaPlayerRenderer&) = delete;
  OHOSMediaPlayerRenderer& operator=(const OHOSMediaPlayerRenderer&) = delete;

  OHOSMediaPlayerRenderer(
      int process_id,
      int routing_id,
      WebContents* web_contents,
      mojo::PendingReceiver<RendererExtension> renderer_extension_receiver,
      mojo::PendingRemote<ClientExtension> client_extension_remote);

  ~OHOSMediaPlayerRenderer() override;

  // media::Renderer implementation
  void Initialize(media::MediaResource* media_resource,
                  media::RendererClient* client,
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
                  media::RequestSurfaceCB request_surface_cb,
                  media::VideoDecoderChangedCB decoder_changed_cb,
#endif // ARKWEB_VIDEO_ASSISTANT
                  media::PipelineStatusCallback init_cb) override;
  void SetLatencyHint(std::optional<base::TimeDelta> latency_hint) override;
  void Flush(base::OnceClosure flush_cb) override;
  void StartPlayingFrom(base::TimeDelta time) override;

  void SetPlaybackRate(double playback_rate) override;
  void SetVolume(float volume) override;
  base::TimeDelta GetMediaTime() override;
  void SetNativeWindowSurface(int native_window_id) override;

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void SetVideoSurface(int32_t surface_id) override;
#endif  // ARKWEB_VIDEO_ASSISTANT

  // media::OHOSMediaPlayerBridge::Client implementation
  void OnFrameAvailable(int fd,
                        uint32_t size,
                        int32_t coded_width,
                        int32_t coded_height,
                        int32_t visible_width,
                        int32_t visible_height,
                        int32_t format) override;
  media::OHOSMediaResourceGetter* GetMediaResourceGetter() override;
  void OnMediaDurationChanged(base::TimeDelta duration) override;
  void OnPlaybackComplete() override;
  void OnError(int error) override;
  void OnVideoSizeChanged(int width, int height) override;
  void OnPlayerInterruptEvent(int32_t value) override;
  media::RendererType GetRendererType() override;

  void OnUpdateAudioMutingState(bool muted);
  void OnWebContentsDestroyed();
  void OnAudioStateChanged(bool isAudible) override;
  void OnPlayerSeekBack(base::TimeDelta back_time) override;
  bool isNeedResume(int32_t resumeInterval);
  // media::mojom::MediaPlayerRendererExtension implementation.
  //
  // Registers a request in the content::ScopedSurfaceRequestManager, and
  // returns the token associated to the request. The token can then be used to
  // complete the request via the gpu::ScopedSurfaceRequestConduit.
  // A completed request will call back to OnScopedSurfaceRequestCompleted().
  //
  // NOTE: If a request is already pending, calling this method again will
  // safely cancel the pending request before registering a new one.
  void InitiateScopedSurfaceRequest(
      InitiateScopedSurfaceRequestCallback callback) override;
  void FinishPaint(int32_t fd) override;
#if BUILDFLAG(ARKWEB_PIP)
  void PipEnable(bool enable) override;
#endif

 private:
  void CreateMediaPlayer();
  void TryOrCreateMediaPlayer();

  void UpdateVolume();

  void GetGrantMediaFileAccessDirs(std::vector<std::string>& grantMediaFileAccessDirs);

  mojo::Remote<ClientExtension> client_extension_;

  raw_ptr<media::RendererClient> renderer_client_ = nullptr;

  std::unique_ptr<media::OHOSMediaPlayerBridge> media_player_;

  // Current duration of the media.
  base::TimeDelta duration_;

  // Indicates if a serious error has been encountered by the |media_player_|.
  bool has_error_;

  gfx::Size video_size_;

  // Identifiers to find the RenderFrameHost that created |this|.
  // NOTE: We store these IDs rather than a RenderFrameHost* because we do not
  // know when the RenderFrameHost is destroyed.
  int render_process_id_;
  int routing_id_;

  std::unique_ptr<media::OHOSMediaResourceGetter> media_resource_getter_;

  bool web_contents_muted_;
  raw_ptr<OHOSMediaPlayerRendererWebContentsObserver> web_contents_observer_ = nullptr;
  float volume_;

  bool initialized_ = false;

  int native_window_id_ = -1;

  std::unique_ptr<media::MediaUrlParams> url_params_;

  base::WeakPtr<WebContents> web_contents_ = nullptr;

  time_t intervalSinceLastSuspend_;

  mojo::Receiver<MediaPlayerRendererExtension> renderer_extension_receiver_;

  media::PipelineStatusCallback init_cb_;

  // NOTE: Weak pointers must be invalidated before all other member variables.
  base::WeakPtrFactory<OHOSMediaPlayerRenderer> weak_factory_{this};
};

}  // namespace content

#endif  // CONTENT_BROWSER_MEDIA_OHOS_MEDIA_PLAYER_RENDERER_H_
