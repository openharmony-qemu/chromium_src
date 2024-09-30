
#ifndef CONTENT_BROWSER_MEDIA_OHOS_OHOS_CUSTOM_MEDIA_PLAYER_RENDERER_H_
#define CONTENT_BROWSER_MEDIA_OHOS_OHOS_CUSTOM_MEDIA_PLAYER_RENDERER_H_

#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
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
#include "url/gurl.h"

#include "content/public/browser/custom_media_player.h"
#include "content/public/browser/media_player_id.h"
#include "content/public/browser/web_contents_observer.h"

namespace content {

class WebContents;

class CONTENT_EXPORT OHOSCustomMediaPlayerRenderer
    : public media::Renderer,
      public media::mojom::MediaPlayerRendererExtension,
      public WebContentsObserver,
      public media::OHOSMediaPlayerBridge::Client {
 public:
  using RendererExtension = media::mojom::MediaPlayerRendererExtension;
  using ClientExtension = media::mojom::CustomMediaPlayerRendererClientExtension;

  OHOSCustomMediaPlayerRenderer(const OHOSCustomMediaPlayerRenderer&) = delete;
  OHOSCustomMediaPlayerRenderer& operator=(const OHOSCustomMediaPlayerRenderer&) = delete;

  OHOSCustomMediaPlayerRenderer(
      int process_id,
      int routing_id,
      int player_id,
      WebContents* web_contents,
      mojo::PendingReceiver<RendererExtension> renderer_extension_receiver,
      mojo::PendingRemote<ClientExtension> client_extension_remote);

  ~OHOSCustomMediaPlayerRenderer() override;

  // media::Renderer implementation
  void Initialize(media::MediaResource* media_resource,
                  media::RendererClient* client,
                  media::PipelineStatusCallback init_cb) override;
  void SetLatencyHint(absl::optional<base::TimeDelta> latency_hint) override;
  void Flush(base::OnceClosure flush_cb) override;
  void StartPlayingFrom(base::TimeDelta time) override;

  void SetPlaybackRate(double playback_rate) override;
  void SetVolume(float volume) override;
  base::TimeDelta GetMediaTime() override;
  media::RendererType GetRendererType() override;
  void SetMuted(bool muted) override;
  void SetSurfaceId(int surface_id, const gfx::Rect& rect) override;
  void SetMediaPlayerState(bool is_suspend, int suspend_type) override;

  void SetMediaSourceList(const std::vector<MediaSourceInfo>& source_infos) override;
  void SetMediaControls(bool show_media_controls,
      const std::vector<std::string>& controls_list) override;
  void SetPoster(const std::string& poster_url) override;
  void SetAttributes(base::flat_map<std::string, std::string> attributes) override;
  void SetReferrer(const std::string& referrer) override;
  void SetIsAudio(bool is_audio) override;
  void SetPlaybackRateWithReason(double playback_rate,
      media::ActionReason reason) override;

  // media::OHOSMediaPlayerBridge::Client implementation
  void OnFrameAvailable(int fd, uint32_t size, int32_t coded_width, int32_t coded_height,
      int32_t visible_width, int32_t visible_height, int32_t format) override {}
  void OnMediaDurationChanged(base::TimeDelta duration) override;
  void OnPlaybackComplete() override;
  void OnError(int error) override;
  void OnVideoSizeChanged(int width, int height) override;
  void OnPlayerInterruptEvent(int32_t value) override {}
  void OnAudioStateChanged(bool isAudible) override {}

  // media::mojom::MediaPlayerRendererExtension implementation.
  void InitiateScopedSurfaceRequest(
      InitiateScopedSurfaceRequestCallback callback) override {}
  void FinishPaint(int32_t fd) override {}

  void OnTimeUpdate(base::TimeDelta media_time);
  void OnBufferingStateChange(media::BufferingState state);

  void UpdatePlaybackStatus(uint32_t status);
  void UpdateVolume(double volume);
  void UpdateMuted(bool muted);
  void UpdatePlaybackRate(double playback_rate);
  void UpdateBufferedEndTime(double buffered_time);
  void OnFullscreenChanged(bool fullscreen);

 private:
  void GetCookies();
  void OnCookiesRetrieved(const std::string& cookies);

  void TryCreateMediaPlayer();
  void CreateMediaPlayer();

  void UpdateVolume();

  mojo::Remote<ClientExtension> client_extension_;

  raw_ptr<media::RendererClient> renderer_client_;

  std::unique_ptr<CustomMediaPlayer> media_player_;

  // Current duration of the media.
  base::TimeDelta duration_;

  // Indicates if a serious error has been encountered by the |media_player_|.
  bool has_error_;

  gfx::Size video_size_;

  bool web_contents_muted_;
  float volume_;

  mojo::Receiver<MediaPlayerRendererExtension> renderer_extension_receiver_;

  media::PipelineStatusCallback init_cb_;

  bool initialized_ = false;

  // TODO : delete
  GlobalRenderFrameHostId global_render_frame_host_id_;

  MediaPlayerId media_player_id_;

  bool muted_ = false;

  int surface_id_ = -1;

  gfx::Rect video_rect_;

  media::MediaResource* media_resource_;

  base::TimeDelta media_time_;

  std::vector<MediaSourceInfo> source_infos_;

  bool show_media_controls_ = false;

  bool is_media_player_suspend_ = false;

  std::vector<std::string> controls_list_;

  std::string poster_url_;

  std::map<std::string, std::string> attributes_;

  std::string referrer_;

  bool is_audio_ = false;

  bool is_playing_ = false;

  absl::optional<std::string> cookies_;

  // NOTE: Weak pointers must be invalidated before all other member variables.
  base::WeakPtrFactory<OHOSCustomMediaPlayerRenderer> weak_factory_{this};
};

}  // namespace content

#endif  // CONTENT_BROWSER_MEDIA_OHOS_OHOS_CUSTOM_MEDIA_PLAYER_RENDERER_H_
