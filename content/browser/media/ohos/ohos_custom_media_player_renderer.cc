
#include "content/browser/media/ohos/ohos_custom_media_player_renderer.h"

#include <memory>

#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/browser/gpu/gpu_process_host.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_client.h"
#include "media/base/timestamp_constants.h"
#include "net/http/http_request_headers.h"

#include "gpu/ipc/common/gpu_surface_id_tracker.h"
#include "content/browser/child_process_security_policy_impl.h"
#include "content/public/browser/custom_media_player_listener.h"
#include "content/public/browser/storage_partition.h"
#include "services/network/public/mojom/network_context.mojom.h"
#include "services/network/public/mojom/restricted_cookie_manager.mojom.h"

namespace content {

namespace {

const float kDefaultVolume = 1.0;

class CustomMediaPlayerListenerImpl : public CustomMediaPlayerListener {
 public:
  explicit CustomMediaPlayerListenerImpl(
      base::WeakPtr<OHOSCustomMediaPlayerRenderer> renderer);
  virtual ~CustomMediaPlayerListenerImpl();

  void OnStatusChanged(uint32_t status) override;
  void OnVolumeChanged(double volume) override;
  void OnMutedChanged(bool muted) override;
  void OnPlaybackRateChanged(double playback_rate) override;
  void OnDurationChanged(double duration) override;
  void OnTimeUpdate(double current_play_time) override;
  void OnBufferedEndTimeChanged(double buffered_time) override;
  void OnEnded() override;
  void OnNetworkStateChanged(uint32_t state) override;
  void OnReadyStateChanged(uint32_t state) override;
  void OnFullscreenChanged(bool fullscreen) override;
  void OnSeeking() override;
  void OnSeekFinished() override;
  void OnError(uint32_t error_code, const std::string& error_msg) override;
  void OnVideoSizeChanged(int width, int height) override;

 private:
  base::WeakPtr<OHOSCustomMediaPlayerRenderer> renderer_;
};

CustomMediaPlayerListenerImpl::CustomMediaPlayerListenerImpl(
    base::WeakPtr<OHOSCustomMediaPlayerRenderer> renderer)
    : renderer_(std::move(renderer)) {}
CustomMediaPlayerListenerImpl::~CustomMediaPlayerListenerImpl() = default;

void CustomMediaPlayerListenerImpl::OnStatusChanged(uint32_t status) {
  DVLOG(1) << __func__;
  if (renderer_) {
    renderer_->UpdatePlaybackStatus(status);
  }
}
void CustomMediaPlayerListenerImpl::OnVolumeChanged(double volume) {
  DVLOG(1) << __func__;
  if (renderer_) {
    renderer_->UpdateVolume(volume);
  }
}
void CustomMediaPlayerListenerImpl::OnMutedChanged(bool muted) {
  DVLOG(1) << __func__;
  if (renderer_) {
    renderer_->UpdateMuted(muted);
  }
}
void CustomMediaPlayerListenerImpl::OnPlaybackRateChanged(double playback_rate) {
  DVLOG(1) << __func__;
  if (renderer_) {
    renderer_->UpdatePlaybackRate(playback_rate);
  }
}
void CustomMediaPlayerListenerImpl::OnDurationChanged(double duration) {
  DVLOG(1) << __func__;
  if (renderer_) {
    renderer_->OnMediaDurationChanged(base::Seconds(duration));
  }
}
void CustomMediaPlayerListenerImpl::OnTimeUpdate(double current_play_time) {
  if (renderer_) {
    renderer_->OnTimeUpdate(base::Seconds(current_play_time));
  }
}
void CustomMediaPlayerListenerImpl::OnBufferedEndTimeChanged(double buffered_time) {
  if (renderer_) {
    renderer_->UpdateBufferedEndTime(buffered_time);
  }
}
void CustomMediaPlayerListenerImpl::OnEnded() {
  DVLOG(1) << __func__;
  if (renderer_) {
    renderer_->OnPlaybackComplete();
  }
}
void CustomMediaPlayerListenerImpl::OnNetworkStateChanged(uint32_t state) {}
void CustomMediaPlayerListenerImpl::OnReadyStateChanged(uint32_t state) {
  if (renderer_) {
    media::BufferingState buffering_state = media::BUFFERING_HAVE_ENOUGH;
    if (state == static_cast<uint32_t>(ReadyState::HAVE_NOTHING)) {
      buffering_state = media::BUFFERING_HAVE_NOTHING;
    }
    renderer_->OnBufferingStateChange(buffering_state);
  }
}

void CustomMediaPlayerListenerImpl::OnFullscreenChanged(bool fullscreen) {
  if (renderer_) {
    renderer_->OnFullscreenChanged(fullscreen);
  }
}
void CustomMediaPlayerListenerImpl::OnSeeking() {}
void CustomMediaPlayerListenerImpl::OnSeekFinished() {}
void CustomMediaPlayerListenerImpl::OnError(uint32_t error_code, const std::string& error_msg) {
  if (renderer_) {
    renderer_->OnError(media::PIPELINE_ERROR_EXTERNAL_RENDERER_FAILED);
  }
}
void CustomMediaPlayerListenerImpl::OnVideoSizeChanged(int width, int height) {
  if (renderer_) {
    renderer_->OnVideoSizeChanged(width, height);
  }
}

// Returns the cookie manager for the `browser_context` at the client end of the
// mojo pipe. This will be restricted to the origin of `url`, and will apply
// policies from user and ContentBrowserClient to cookie operations.
mojo::PendingRemote<network::mojom::RestrictedCookieManager>
GetRestrictedCookieManagerForContext(
    BrowserContext* browser_context,
    const GURL& url,
    const net::SiteForCookies& site_for_cookies,
    const url::Origin& top_frame_origin,
    RenderFrameHostImpl* render_frame_host) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);

  url::Origin request_origin = url::Origin::Create(url);
  StoragePartition* storage_partition =
      browser_context->GetDefaultStoragePartition();

  // `request_origin` cannot be used to create `isolation_info` since it
  // represents the media resource, not the frame origin. Here we use the
  // `top_frame_origin` as the frame origin to ensure the consistency check
  // passes when creating `isolation_info`. This is ok because
  // `isolation_info.frame_origin` is unused in RestrictedCookieManager.
  DCHECK(site_for_cookies.IsNull() ||
         site_for_cookies.IsFirstParty(top_frame_origin.GetURL()));
  net::IsolationInfo isolation_info = net::IsolationInfo::Create(
      net::IsolationInfo::RequestType::kOther, top_frame_origin,
      top_frame_origin, site_for_cookies, absl::nullopt);

  mojo::PendingRemote<network::mojom::RestrictedCookieManager> pipe;
  static_cast<StoragePartitionImpl*>(storage_partition)
      ->CreateRestrictedCookieManager(
          network::mojom::RestrictedCookieManagerRole::NETWORK, request_origin,
          std::move(isolation_info),
          /* is_service_worker = */ false,
          render_frame_host ? render_frame_host->GetProcess()->GetID() : -1,
          render_frame_host ? render_frame_host->GetRoutingID()
                            : MSG_ROUTING_NONE,
          render_frame_host ? render_frame_host->GetCookieSettingOverrides()
                            : net::CookieSettingOverrides(),
          pipe.InitWithNewPipeAndPassReceiver(),
          render_frame_host ? render_frame_host->CreateCookieAccessObserver()
                            : mojo::NullRemote());
  return pipe;
}

void ReturnResultOnUIThread(
    base::OnceCallback<void(const std::string&)> callback,
    const std::string& result) {
  GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE, base::BindOnce(std::move(callback), result));
}

void ReturnResultOnUIThreadAndClosePipe(
    mojo::Remote<network::mojom::RestrictedCookieManager> pipe,
    base::OnceCallback<void(const std::string&)> callback,
    const std::string& result) {
  GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE, base::BindOnce(std::move(callback), result));
}

MediaInfo::Preload ConvertTo(uint32_t preload_type) {
  if (preload_type == 0) {
    return MediaInfo::Preload::NONE;
  }
  if (preload_type == 1) {
    return MediaInfo::Preload::METADATA;
  }
  if (preload_type == 2) {
    return MediaInfo::Preload::AUTO;
  }
  return MediaInfo::Preload::AUTO;
}

}  // namespace

OHOSCustomMediaPlayerRenderer::OHOSCustomMediaPlayerRenderer(
    int process_id,
    int routing_id,
    int player_id,
    WebContents* web_contents,
    mojo::PendingReceiver<RendererExtension> renderer_extension_receiver,
    mojo::PendingRemote<ClientExtension> client_extension_remote)
    : WebContentsObserver(web_contents),
      client_extension_(std::move(client_extension_remote)),
      has_error_(false),
      volume_(kDefaultVolume),
      renderer_extension_receiver_(this,
                                   std::move(renderer_extension_receiver)),
      global_render_frame_host_id_(process_id, routing_id),
      media_player_id_(GlobalRenderFrameHostId(process_id, routing_id), player_id) {
  DCHECK_EQ(WebContents::FromRenderFrameHost(
                RenderFrameHost::FromID(process_id, routing_id)),
            web_contents);

  WebContentsImpl* web_contents_impl =
      static_cast<WebContentsImpl*>(WebContentsObserver::web_contents());
  web_contents_muted_ = web_contents_impl && web_contents_impl->IsAudioMuted();
}

OHOSCustomMediaPlayerRenderer::~OHOSCustomMediaPlayerRenderer() {
  WebContentsImpl* web_contents_impl =
      static_cast<WebContentsImpl*>(web_contents());
  if (media_player_) {
    if (web_contents_impl) {
      web_contents_impl->RemoveCustomMediaPlayer(
          media_player_id_, media_player_.get());
    }
    media_player_->Release();
  }
}

void OHOSCustomMediaPlayerRenderer::Initialize(media::MediaResource* media_resource,
                                     media::RendererClient* client,
#ifdef OHOS_VIDEO_ASSISTANT
                                     media::RequestSurfaceCB request_surface_cb,
                                     media::VideoDecoderChangedCB decoder_changed_cb,
#endif // OHOS_VIDEO_ASSISTANT
                                     media::PipelineStatusCallback init_cb) {
  DVLOG(1) << __func__;

  DCHECK_CURRENTLY_ON(BrowserThread::UI);

  renderer_client_ = client;

  if (media_resource->GetType() != media::MediaResource::Type::URL) {
    DLOG(ERROR) << "MediaResource is not of Type URL";
    std::move(init_cb).Run(media::PIPELINE_ERROR_INITIALIZATION_FAILED);
    return;
  }

  init_cb_ = std::move(init_cb);
  media_resource_ = media_resource;

  GetCookies();
}

void OHOSCustomMediaPlayerRenderer::GetCookies() {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);

  const GURL& url = media_resource_->GetMediaUrlParams().media_url;
  const net::SiteForCookies& site_for_cookies =
      media_resource_->GetMediaUrlParams().site_for_cookies;
  const url::Origin& top_frame_origin =
      media_resource_->GetMediaUrlParams().top_frame_origin;
  bool has_storage_access =
      media_resource_->GetMediaUrlParams().has_storage_access;

  base::OnceCallback<void(const std::string&)> callback =
        base::BindOnce(&OHOSCustomMediaPlayerRenderer::OnCookiesRetrieved,
                       weak_factory_.GetWeakPtr());

  ChildProcessSecurityPolicyImpl* policy =
      ChildProcessSecurityPolicyImpl::GetInstance();
  if (!policy->CanAccessDataForOrigin(media_player_id_.frame_routing_id.child_id,
                                      url::Origin::Create(url))) {
    // Running the callback asynchronously on the caller thread to avoid
    // reentrancy issues.
    ReturnResultOnUIThread(std::move(callback), std::string());
    return;
  }

  RenderProcessHost* host = RenderProcessHost::FromID(
      media_player_id_.frame_routing_id.child_id);
  if (!host) {
    LOG(ERROR) << "GetCookies failed";
    return;
  }

  BrowserContext* context = host->GetBrowserContext();

  mojo::Remote<network::mojom::RestrictedCookieManager> cookie_manager(
      GetRestrictedCookieManagerForContext(
          context, url, site_for_cookies, top_frame_origin,
          RenderFrameHostImpl::FromID(media_player_id_.frame_routing_id)));
  network::mojom::RestrictedCookieManager* cookie_manager_ptr =
      cookie_manager.get();

  cookie_manager_ptr->GetCookiesString(
      url, site_for_cookies, top_frame_origin, has_storage_access,
      base::BindOnce(&ReturnResultOnUIThreadAndClosePipe,
                     std::move(cookie_manager), std::move(callback)));
}

void OHOSCustomMediaPlayerRenderer::OnCookiesRetrieved(
    const std::string& cookies) {
  cookies_= cookies;
  TryCreateMediaPlayer();
}

void OHOSCustomMediaPlayerRenderer::TryCreateMediaPlayer() {
  DVLOG(1) << __func__;

  bool wait_surface_created = surface_id_ == -1;
  if (wait_surface_created) {
    return;
  }

  if (!cookies_) {
    return;
  }

  CreateMediaPlayer();
}

void OHOSCustomMediaPlayerRenderer::CreateMediaPlayer() {
  DVLOG(1) << __func__;

  DCHECK_CURRENTLY_ON(BrowserThread::UI);

  WebContentsImpl* web_contents_impl =
      static_cast<WebContentsImpl*>(web_contents());

  if (!web_contents_impl) {
    LOG(ERROR) << "CreateMediaPlayer failed, no web_contents";
    std::move(init_cb_).Run(media::PIPELINE_ERROR_INITIALIZATION_FAILED);
    return;
  }

  content::GpuProcessHost* gpu_process_host = content::GpuProcessHost::Get();
  if (!gpu_process_host || !gpu_process_host->gpu_host()) {
    LOG(ERROR) << "CreateMediaPlayer failed, no gpu host";
    std::move(init_cb_).Run(media::PIPELINE_ERROR_INITIALIZATION_FAILED);
    return;
  }
  std::string surface_id_string =
      gpu_process_host->gpu_host()->GetSurfaceId(surface_id_);

  MediaInfo media_info;
  media_info.embed_id = std::to_string(surface_id_);
  media_info.media_type = is_audio_ ? MediaInfo::MediaType::AUDIO
                                    : MediaInfo::MediaType::VIDEO;
  media_info.media_src_list.reserve(source_infos_.size());
  for (const auto& info : source_infos_) {
    media_info.media_src_list.push_back({
        static_cast<MediaInfo::SourceType>(
            media_resource_->GetMediaUrlParams().custom_media_url_params.media_source_type),
        info.media_source, info.media_format});
  }
  media_info.surface_info.id = surface_id_string;
  media_info.surface_info.x = video_rect_.x();
  media_info.surface_info.y = video_rect_.y();
  media_info.surface_info.width = video_rect_.width();
  media_info.surface_info.height = video_rect_.height();
  media_info.controls = show_media_controls_;
  media_info.controlslist = std::move(controls_list_);
  media_info.muted = muted_;
  media_info.poster_url = poster_url_;
  media_info.preload = ConvertTo(
      media_resource_->GetMediaUrlParams().custom_media_url_params.preload_type);
  if (!cookies_->empty()) {
    media_info.https_headers.insert(std::make_pair(
        net::HttpRequestHeaders::kCookie,
        std::move(cookies_.value())));
  }
  if (!referrer_.empty()) {
    media_info.https_headers.insert(std::make_pair(
        net::HttpRequestHeaders::kReferer,
        std::move(referrer_)));
  }
  std::string user_agent = GetContentClient()->browser()->GetUserAgent();
  if (!user_agent.empty()) {
    media_info.https_headers.insert(std::make_pair(
        net::HttpRequestHeaders::kUserAgent,
        std::move(user_agent)));
  }
  media_info.attributes = std::move(attributes_);

  media_player_ = web_contents_impl->CreateCustomMediaPlayer(
      std::make_unique<CustomMediaPlayerListenerImpl>(
          weak_factory_.GetWeakPtr()), media_info);
  if (!media_player_) {
    LOG(INFO) << "CreateCustomMediaPlayer, no media player";
    std::move(init_cb_).Run(media::PIPELINE_ERROR_INITIALIZATION_FAILED);
    return;
  }

  web_contents_impl->AddCustomMediaPlayer(
      media_player_id_, media_player_.get());

  initialized_ = true;
  std::move(init_cb_).Run(media::PIPELINE_OK);
  LOG(INFO) << "media player initialize ok.";
}

void OHOSCustomMediaPlayerRenderer::SetLatencyHint(
    absl::optional<base::TimeDelta> latency_hint) {}

void OHOSCustomMediaPlayerRenderer::Flush(base::OnceClosure flush_cb) {
  DVLOG(3) << __func__;
  std::move(flush_cb).Run();
}

void OHOSCustomMediaPlayerRenderer::StartPlayingFrom(base::TimeDelta time) {
  DVLOG(1) << __func__;
  if (media_player_) {
    media_player_->Seek(time.InSecondsF());
    if (!is_playing_) {
      media_player_->Play();
    }
  }
  is_playing_ = true;

  renderer_client_->OnBufferingStateChange(
      media::BUFFERING_HAVE_ENOUGH, media::BUFFERING_CHANGE_REASON_UNKNOWN);
}

void OHOSCustomMediaPlayerRenderer::SetPlaybackRate(double playback_rate) {
  DVLOG(1) << __func__;
  if (media_player_) {
    if (playback_rate == 0) {
      if (is_playing_) {
        media_player_->Pause();
      }
      is_playing_ = false;
      return;
    }
    media_player_->SetPlaybackRate(playback_rate);
  }
  if (!is_playing_) {
    media_player_->Play();
  }
  is_playing_ = true;
}

void OHOSCustomMediaPlayerRenderer::SetVolume(float volume) {
  DVLOG(1) << __func__;
  volume_ = volume;
  UpdateVolume();
}

void OHOSCustomMediaPlayerRenderer::UpdateVolume() {
  DVLOG(1) << __func__;
  float volume = web_contents_muted_ ? 0 : volume_;
  if (media_player_)
    media_player_->SetVolume(volume);
}

base::TimeDelta OHOSCustomMediaPlayerRenderer::GetMediaTime() {
  return media_time_;
}

media::RendererType OHOSCustomMediaPlayerRenderer::GetRendererType() {
  return media::RendererType::kOHOSCustomMediaPlayer;
}

media::OHOSMediaResourceGetter* OHOSCustomMediaPlayerRenderer::GetMediaResourceGetter() {
  return nullptr;
}

void OHOSCustomMediaPlayerRenderer::OnMediaDurationChanged(base::TimeDelta duration) {
  // For HLS streams, the reported duration may be zero for infinite streams.
  // See http://crbug.com/501213.
  if (duration.is_zero())
    duration = media::kInfiniteDuration;

  if (duration_ != duration) {
    duration_ = duration;
    client_extension_->OnDurationChange(duration);
  }
}

void OHOSCustomMediaPlayerRenderer::OnPlaybackComplete() {
  renderer_client_->OnEnded();
}

void OHOSCustomMediaPlayerRenderer::OnError(int error) {
  // Some errors are forwarded to the MediaPlayerListener, but are of no
  // importance to us. Ignore these errors, which are reported as
  // MEDIA_ERROR_INVALID_CODE by MediaPlayerListener.
  if (error ==
      media::OHOSMediaPlayerBridge::MediaErrorType::MEDIA_ERROR_INVALID_CODE) {
    return;
  }

  LOG(ERROR) << __func__ << " Error: " << error;
  has_error_ = true;
  renderer_client_->OnError(media::PIPELINE_ERROR_EXTERNAL_RENDERER_FAILED);
}

void OHOSCustomMediaPlayerRenderer::OnVideoSizeChanged(int width, int height) {
  // This method is called when we find a video size from metadata or when
  // |media_player|'s size actually changes.
  // We therefore may already have the latest video size.
  gfx::Size new_size = gfx::Size(width, height);
  if (video_size_ != new_size) {
    video_size_ = new_size;
    // Send via |client_extension_| instead of |renderer_client_|, so
    // MediaPlayerRendererClient can update its texture size.
    // MPRClient will then continue propagating changes via its RendererClient.
    client_extension_->OnVideoSizeChange(video_size_);
  }
}

void OHOSCustomMediaPlayerRenderer::SetMuted(bool muted) {
  muted_ = muted;
}

void OHOSCustomMediaPlayerRenderer::SetSurfaceId(int surface_id,
    const gfx::Rect& rect) {
  surface_id_ = surface_id;
  video_rect_ = rect;
  TryCreateMediaPlayer();
}

void OHOSCustomMediaPlayerRenderer::SetMediaPlayerState(bool is_suspend,
                                                        int suspend_type) {
  DVLOG(1) << __func__;
  if (!media_player_) {
    return;
  }

  if (is_suspend) {
    if (is_media_player_suspend_) {
      return;
    }

    is_media_player_suspend_ = true;
    media_player_->SuspendMediaPlayer(suspend_type);
    return;
  }

  if (is_media_player_suspend_) {
    is_media_player_suspend_ = false;
    media_player_->ResumeMediaPlayer();
  }
}

void OHOSCustomMediaPlayerRenderer::SetMediaSourceList(
    const std::vector<MediaSourceInfo>& source_infos) {
  source_infos_ = source_infos;
}

void OHOSCustomMediaPlayerRenderer::SetMediaControls(
    bool show_media_controls,
    const std::vector<std::string>& controls_list) {
  show_media_controls_ = show_media_controls;
  controls_list_ = controls_list;
}

void OHOSCustomMediaPlayerRenderer::SetPoster(const std::string& poster_url) {
  poster_url_ = poster_url;
}

void OHOSCustomMediaPlayerRenderer::SetAttributes(
    base::flat_map<std::string, std::string> attributes) {
  attributes_.clear();
  for (const auto& item : attributes) {
    attributes_.insert({item.first, item.second});
  }
}

void OHOSCustomMediaPlayerRenderer::SetReferrer(
    const std::string& referrer) {
  referrer_ = referrer;
}

void OHOSCustomMediaPlayerRenderer::SetIsAudio(bool is_audio) {
  is_audio_ = is_audio;
}

void OHOSCustomMediaPlayerRenderer::SetPlaybackRateWithReason(
    double playback_rate, media::ActionReason reason) {
  DVLOG(1) << __func__ << "(" << playback_rate << ", " << static_cast<int>(reason) << ")";
  if (reason != media::ActionReason::kNormal) {
    return;
  }
  SetPlaybackRate(playback_rate);
}

void OHOSCustomMediaPlayerRenderer::OnTimeUpdate(base::TimeDelta media_time) {
  media_time_ = media_time;
}

void OHOSCustomMediaPlayerRenderer::OnBufferingStateChange(media::BufferingState state) {
  renderer_client_->OnBufferingStateChange(
      state, media::BUFFERING_CHANGE_REASON_UNKNOWN);
}

void OHOSCustomMediaPlayerRenderer::UpdatePlaybackStatus(uint32_t status) {
  is_playing_ = !!status;
  client_extension_->UpdatePlaybackStatus(status);
}

void OHOSCustomMediaPlayerRenderer::UpdateVolume(double volume) {
  client_extension_->UpdateVolume(volume);
}

void OHOSCustomMediaPlayerRenderer::UpdateMuted(bool muted) {
  client_extension_->UpdateMuted(muted);
}

void OHOSCustomMediaPlayerRenderer::UpdatePlaybackRate(double playback_rate) {
  client_extension_->UpdatePlaybackRate(playback_rate);
}

void OHOSCustomMediaPlayerRenderer::UpdateBufferedEndTime(double buffered_time) {
  client_extension_->UpdateBufferedEndTime(buffered_time);
}

void OHOSCustomMediaPlayerRenderer::OnFullscreenChanged(bool fullscreen) {
  WebContentsImpl* web_contents_impl =
      static_cast<WebContentsImpl*>(web_contents());
  if (!web_contents_impl) {
    return;
  }
  if (fullscreen) {
    web_contents_impl->RequestEnterFullscreen(media_player_id_);
  } else {
    web_contents_impl->RequestExitFullscreen(media_player_id_);
  }
}
}  // namespace content
