// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/media/ohos/ohos_media_player_renderer.h"

#include <memory>

#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "content/browser/media/ohos/ohos_media_player_renderer_web_contents_observer.h"
#include "content/browser/media/session/media_session_impl.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_client.h"
#include "media/base/timestamp_constants.h"

namespace content {

namespace {
const float kDefaultVolume = 1.0;
constexpr double kPlaybackRateLevel0 = 0;
constexpr double kPlaybackRateLevel1 = 1;
constexpr double kPlaybackRateLevel2 = 1.25;
constexpr double kPlaybackRateLevel3 = 1.75;
constexpr double kPlaybackRateLevel4 = 2;
enum InterruptHint {
    INTERRUPT_HINT_NONE = 0,
    INTERRUPT_HINT_RESUME,
    INTERRUPT_HINT_PAUSE,
    INTERRUPT_HINT_STOP,
    INTERRUPT_HINT_DUCK,
    INTERRUPT_HINT_UNDUCK
};
}  // namespace

OHOSMediaPlayerRenderer::OHOSMediaPlayerRenderer(
    int process_id,
    int routing_id,
    WebContents* web_contents,
    mojo::PendingReceiver<RendererExtension> renderer_extension_receiver,
    mojo::PendingRemote<ClientExtension> client_extension_remote)
    : client_extension_(std::move(client_extension_remote)),
      has_error_(false),
      volume_(kDefaultVolume),
      web_contents_(web_contents),
      renderer_extension_receiver_(this,
                                   std::move(renderer_extension_receiver)) {
  WebContentsImpl* web_contents_impl =
      static_cast<WebContentsImpl*>(web_contents);
  web_contents_muted_ = web_contents_impl && web_contents_impl->IsAudioMuted();

  if (web_contents) {
    OHOSMediaPlayerRendererWebContentsObserver::CreateForWebContents(
        web_contents);
    web_contents_observer_ =
        OHOSMediaPlayerRendererWebContentsObserver::FromWebContents(
            web_contents);
    if (web_contents_observer_)
      web_contents_observer_->AddMediaPlayerRenderer(this);
  }
}

OHOSMediaPlayerRenderer::~OHOSMediaPlayerRenderer() {
  if (web_contents_observer_)
    web_contents_observer_->RemoveMediaPlayerRenderer(this);
}

void OHOSMediaPlayerRenderer::Initialize(
    media::MediaResource* media_resource,
    media::RendererClient* client,
    media::PipelineStatusCallback init_cb) {
  renderer_client_ = client;
  if (media_resource->GetType() != media::MediaResource::Type::URL) {
    DLOG(ERROR) << "MediaResource is not of Type URL";
    std::move(init_cb).Run(media::PIPELINE_ERROR_INITIALIZATION_FAILED);
    return;
  }

  CreateMediaPlayer(media_resource->GetMediaUrlParams(), std::move(init_cb));
}

void OHOSMediaPlayerRenderer::CreateMediaPlayer(
    const media::MediaUrlParams& url_params,
    media::PipelineStatusCallback init_cb) {
  const std::string user_agent = GetContentClient()->browser()->GetUserAgent();
  media_player_.reset(new media::OHOSMediaPlayerBridge(
      url_params.media_url, url_params.site_for_cookies,
      url_params.top_frame_origin, user_agent,
      false,  // hide_url_log
      this, url_params.allow_credentials, url_params.is_hls));
  init_cb_ = std::move(init_cb);
  int32_t ret = media_player_->Initialize();
  if (ret != 0) {
    LOG(ERROR) << "media player Initialize failed";
    std::move(init_cb_).Run(media::PIPELINE_ERROR_INITIALIZATION_FAILED);
  } else {
    initialized_ = true;
    std::move(init_cb_).Run(media::PIPELINE_OK);
    LOG(INFO) << "media player Initialize ok";
  }
}

void OHOSMediaPlayerRenderer::SetLatencyHint(
    absl::optional<base::TimeDelta> latency_hint) {}

void OHOSMediaPlayerRenderer::Flush(base::OnceClosure flush_cb) {
  std::move(flush_cb).Run();
}

void OHOSMediaPlayerRenderer::StartPlayingFrom(base::TimeDelta time) {
  if (has_error_ || !media_player_)
    return;
  media_player_->SeekTo(time);
  renderer_client_->OnBufferingStateChange(
      media::BufferingState::BUFFERING_HAVE_ENOUGH,
      media::BufferingStateChangeReason::BUFFERING_CHANGE_REASON_UNKNOWN);
}

void OHOSMediaPlayerRenderer::SetPlaybackRate(double playback_rate) {
  if (has_error_)
    return;
  if (playback_rate <= kPlaybackRateLevel0) {
    media_player_->Pause();
  } else {
    OHOS::NWeb::PlaybackRateMode mode;
    if (playback_rate < kPlaybackRateLevel1) {
      mode = OHOS::NWeb::PlaybackRateMode::SPEED_FORWARD_0_75_X;
    } else if (playback_rate < kPlaybackRateLevel2) {
      mode = OHOS::NWeb::PlaybackRateMode::SPEED_FORWARD_1_00_X;
    } else if (playback_rate < kPlaybackRateLevel3) {
      mode = OHOS::NWeb::PlaybackRateMode::SPEED_FORWARD_1_25_X;
    } else if (playback_rate < kPlaybackRateLevel4) {
      mode = OHOS::NWeb::PlaybackRateMode::SPEED_FORWARD_1_75_X;
    } else {
      mode = OHOS::NWeb::PlaybackRateMode::SPEED_FORWARD_2_00_X;
    }
    media_player_->SetPlaybackSpeed(mode);
    media_player_->Start();
  }
}

void OHOSMediaPlayerRenderer::InitiateScopedSurfaceRequest(
    InitiateScopedSurfaceRequestCallback callback) {}

void OHOSMediaPlayerRenderer::FinishPaint(int32_t fd) {
  if (media_player_) {
    media_player_->FinishPaint(fd);
  }
}

void OHOSMediaPlayerRenderer::OnFrameAvailable(int fd,
                                               uint32_t size,
                                               int32_t coded_width,
                                               int32_t coded_height,
                                               int32_t visible_width,
                                               int32_t visible_height,
                                               int32_t format) {
  if (client_extension_) {
    auto ohos_buffer = media::mojom::OhosSurfaceBufferHandle::New();
    ohos_buffer->buffer_size = size;
    base::ScopedFD buffer_fd(dup(fd));
    ohos_buffer->fd_browser = fd;
    ohos_buffer->coded_width = coded_width;
    ohos_buffer->coded_height = coded_height;
    ohos_buffer->visible_width = visible_width;
    ohos_buffer->visible_height = visible_height;
    ohos_buffer->format = format;
    ohos_buffer->buffer_fd = mojo::PlatformHandle(std::move(buffer_fd));
    client_extension_->OnFrameUpdate(std::move(ohos_buffer));
  }
}

void OHOSMediaPlayerRenderer::OnMediaDurationChanged(base::TimeDelta duration) {
  if (duration.is_zero())
    duration = media::kInfiniteDuration;

  if (duration_ != duration) {
    duration_ = duration;
    client_extension_->OnDurationChange(duration);
  }
}

void OHOSMediaPlayerRenderer::OnPlaybackComplete() {
  renderer_client_->OnEnded();
}

void OHOSMediaPlayerRenderer::OnError(int error) {
  if (error ==
      media::OHOSMediaPlayerBridge::MediaErrorType::MEDIA_ERROR_INVALID_CODE) {
    return;
  }
  LOG(ERROR) << __func__ << " Error: " << error;
  has_error_ = true;
  if (initialized_) {
    renderer_client_->OnError(media::PIPELINE_ERROR_EXTERNAL_RENDERER_FAILED);
  }
}

void OHOSMediaPlayerRenderer::OnVideoSizeChanged(int width, int height) {
  gfx::Size new_size = gfx::Size(width, height);
  if (video_size_ != new_size) {
    video_size_ = new_size;
    // Send via |client_extension_| instead of |renderer_client_|, so
    // MediaPlayerRendererClient can update its texture size.
    // MPRClient will then continue propagating changes via its RendererClient.
    client_extension_->OnVideoSizeChange(video_size_);
  }
}

void OHOSMediaPlayerRenderer::OnUpdateAudioMutingState(bool muted) {
  web_contents_muted_ = muted;
  UpdateVolume();
}

void OHOSMediaPlayerRenderer::OnWebContentsDestroyed() {
  web_contents_observer_ = nullptr;
}

void OHOSMediaPlayerRenderer::OnPlayerInterruptEvent(int32_t value) {
  if (web_contents_ == nullptr) {
    LOG(ERROR) << "web contents is nullptr";
    return;
  }
  MediaSessionImpl* mediaSession = MediaSessionImpl::Get(web_contents_);
  if (mediaSession == nullptr) {
    LOG(ERROR) << "get mediaSession is nullptr";
    return;
  }
  LOG(INFO) << "On Player InterruptEvent value:" << value;
  if (value == static_cast<int32_t>(INTERRUPT_HINT_PAUSE) ||
      value == static_cast<int32_t>(INTERRUPT_HINT_STOP)) {
    if (mediaSession->audioResumeInterval_ > 0) {
      intervalSinceLastSuspend_ = std::time(nullptr);
    }
    mediaSession->Suspend(content::MediaSession::SuspendType::kSystem);
  } else if (value == INTERRUPT_HINT_RESUME) {
    if (mediaSession->audioResumeInterval_ > 0 &&
        std::time(nullptr) - intervalSinceLastSuspend_ <=
            static_cast<double>(mediaSession->audioResumeInterval_) &&
        mediaSession->IsSuspended()) {
      mediaSession->Resume(content::MediaSession::SuspendType::kSystem);
    }
  }
}

void OHOSMediaPlayerRenderer::SetVolume(float volume) {
  volume_ = volume;
  UpdateVolume();
}

void OHOSMediaPlayerRenderer::UpdateVolume() {
  if (media_player_)
    media_player_->SetVolume(volume_, web_contents_muted_);
}

void OHOSMediaPlayerRenderer::OnAudioStateChanged(bool isAudible) {
  WebContentsImpl* web_contents_impl =
      static_cast<WebContentsImpl*>(web_contents_);
  if (isAudible) {
    web_contents_impl->AddMediaPlayerAudibleCount();
    web_contents_->OnAudioStateChanged();
  } else {
    web_contents_impl->DelMediaPlayerAudibleCount();
    web_contents_->OnAudioStateChanged();
  }
}

void OHOSMediaPlayerRenderer::OnPlayerSeekBack(base::TimeDelta back_time) {
  if (web_contents_ == nullptr) {
    LOG(ERROR) << "web contents is nullptr";
    return;
  }
  MediaSessionImpl* mediaSession = MediaSessionImpl::Get(web_contents_);
  if (mediaSession == nullptr) {
    LOG(ERROR) << "get mediaSession is nullptr";
    return;
  }
  LOG(INFO) << "SEEK_CLOSEST failure and seek back time: " << back_time;
  mediaSession->SeekTo(back_time);
}

base::TimeDelta OHOSMediaPlayerRenderer::GetMediaTime() {
  if (media_player_) {
    return media_player_->GetMediaTime();
  }
  return base::Milliseconds(0);
}

media::RendererType OHOSMediaPlayerRenderer::GetRendererType() {
  return media::RendererType::kOHOSMediaPlayer;
}

}  // namespace content
