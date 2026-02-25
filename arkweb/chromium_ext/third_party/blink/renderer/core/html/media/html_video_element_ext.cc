/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "third_party/blink/renderer/platform/weborigin/security_policy.h"
#endif  // ARKWEB_VIDEO_ASSISTANT
#if BUILDFLAG(ARKWEB_PIP)
#include "third_party/blink/renderer/core/dom/events/event.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

namespace blink {

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
const uint16_t kVideoPreloadTimeDefault = 4;   // preloadTime default 4s
const uint16_t kVideoMinCacheTimeDefault = 2;  // minCacheTime default 2s
const uint16_t kVideoMaxCacheTimeDefault = 6;  // maxCacheTime default 6s
const uint16_t kVideoMoovSizeDefault = 512;    // moovSize default 512kB
const uint16_t kVideoBitrateDefault = 2000;    // bitRate default 2000kb/s
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void HTMLVideoElement::RequestEnterFullscreen() {
  if (!IsFullscreen()) {
    FullscreenOptions* options = FullscreenOptions::Create();
    options->setNavigationUI("hide");
    Fullscreen::RequestFullscreen(*this, options,
                                  FullscreenRequestType::kForCustomMediaPlayer);
  }
}

void HTMLVideoElement::RequestExitFullscreen() {
  if (IsFullscreen()) {
    Fullscreen::ExitFullscreen(GetDocument());
  }
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void HTMLVideoElement::SetPlaybackRate(double playback_rate) {
  setPlaybackRate(playback_rate);
}

// LCOV_EXCL_START
void HTMLVideoElement::RequestDownloadUrl() {
  const KURL& url = downloadURL();
  if (url.IsNull() || url.IsEmpty()) {
    return;
  }

  ResourceRequest request(url);
  request.SetSuggestedFilename(title());
  request.SetRequestContext(mojom::blink::RequestContextType::DOWNLOAD);
  request.SetRequestorOrigin(GetExecutionContext()->GetSecurityOrigin());

  ExecutionContext* context = GetExecutionContext();
  if (context) {
    Referrer referrer = SecurityPolicy::GenerateReferrer(
        context->GetReferrerPolicy(), url, context->OutgoingReferrer());
    request.SetReferrerString(referrer.referrer);
    request.SetReferrerPolicy(referrer.referrer_policy);
  }

  GetDocument().GetFrame()->DownloadURL(
      request, network::mojom::blink::RedirectMode::kError);
}
#endif  // ARKWEB_VIDEO_ASSISTANT
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_PIP)
void HTMLVideoElement::OnPictureInPictureStateChanged(
    uint32_t state, int32_t width, int32_t height) {
  if (!web_media_player_ || !pip_down_) {
    pip_down_ = true;
    return;
  }

  for (auto& observer : GetMediaPlayerObserverRemoteSet()) {
    observer->OnPictureInPictureStateChanged(state, width, height);
  }
}

void HTMLVideoElement::PipDown(bool state) {
  if (!web_media_player_)
    return;
  pip_down_ = state;
  LOG(INFO) << __func__ << " state:" << pip_down_;
}

void HTMLVideoElement::RequestExitPictureInPicture() {
  PictureInPictureController::From(GetDocument())
      .ExitPictureInPicture(this, nullptr);
}

void HTMLVideoElement::NotifyPipResize() {
  DispatchEvent(*Event::Create(event_type_names::kResize));
}

void HTMLVideoElement::PipRequestPlay() {
  if (!paused()) {
    RequestPlay();
  }
}
#endif

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
uint16_t HTMLVideoElement::GetVideoPreloadTimeDefault() {
  int result = INT_MAX;
  if (WebLocalFrameImpl* web_frame =
          WebLocalFrameImpl::FromFrame(GetDocument().GetFrame())) {
    result = web_frame->Client()->AsWebLocalFrameClientExt()->GetVideoPreloadTimeDefault();
  }
  if (result == INT_MAX) {
    return kVideoPreloadTimeDefault;
  }
  return result;
}

uint16_t HTMLVideoElement::GetVideoMinCacheTimeDefault() {
  int result = INT_MAX;
  if (WebLocalFrameImpl* web_frame =
          WebLocalFrameImpl::FromFrame(GetDocument().GetFrame())) {
    result = web_frame->Client()->AsWebLocalFrameClientExt()->GetVideoMinCacheTimeDefault();
  }
  if (result == INT_MAX) {
    return kVideoMinCacheTimeDefault;
  }
  return result;
}

uint16_t HTMLVideoElement::GetVideoMaxCacheTimeDefault() {
  int result = INT_MAX;
  if (WebLocalFrameImpl* web_frame =
          WebLocalFrameImpl::FromFrame(GetDocument().GetFrame())) {
    result = web_frame->Client()->AsWebLocalFrameClientExt()->GetVideoMaxCacheTimeDefault();
  }
  if (result == INT_MAX) {
    return kVideoMaxCacheTimeDefault;
  }
  return result;
}

uint16_t HTMLVideoElement::GetVideoMoovSizeDefault() {
  int result = INT_MAX;
  if (WebLocalFrameImpl* web_frame =
          WebLocalFrameImpl::FromFrame(GetDocument().GetFrame())) {
    result = web_frame->Client()->AsWebLocalFrameClientExt()->GetVideoMoovSizeDefault();
  }
  if (result == INT_MAX) {
    return kVideoMoovSizeDefault;
  }
  return result;
}

uint16_t HTMLVideoElement::GetVideoBitrateDefault() {
  int result = INT_MAX;
  if (WebLocalFrameImpl* web_frame =
          WebLocalFrameImpl::FromFrame(GetDocument().GetFrame())) {
    result = web_frame->Client()->AsWebLocalFrameClientExt()->GetVideoBitrateDefault();
  }
  if (result == INT_MAX) {
    return kVideoBitrateDefault;
  }
  return result;
}

void HTMLVideoElement::CheckAndSetValue(const QualifiedName& name,
                                        uint16_t* out) {
  const AtomicString& value = FastGetAttribute(name);
  bool ok = true;
  int result = value.ToInt(&ok);
  if (!ok) {
    return;
  }

  if (result <= 0 || result > UINT16_MAX) {
    return;
  }

  *out = result;
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

// ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION
uint16_t HTMLVideoElement::hbsPreloadTime() {
  return hbs_preload_time_;
}

uint16_t HTMLVideoElement::hbsMaxCacheTime() {
  return hbs_max_cache_time_;
}

uint16_t HTMLVideoElement::hbsMinCacheTime() {
  return hbs_min_cache_time_;
}

uint16_t HTMLVideoElement::hbsBitrate() {
  return hbs_bitrate_;
}

uint16_t HTMLVideoElement::hbsMoovSize() {
  return hbs_moov_size_;
}

void HTMLVideoElement::setHbsPreloadTime(uint16_t time) {
  setAttribute(html_names::kHbspreloadtimeAttr,
               AtomicString::Number<short>(time));
}

void HTMLVideoElement::setHbsMaxCacheTime(uint16_t time) {
  setAttribute(html_names::kHbsmaxcachetimeAttr,
               AtomicString::Number<short>(time));
}

void HTMLVideoElement::setHbsMinCacheTime(uint16_t time) {
  setAttribute(html_names::kHbsmincachetimeAttr,
               AtomicString::Number<short>(time));
}

void HTMLVideoElement::setHbsBitrate(uint16_t bt) {
  setAttribute(html_names::kHbsbitrateAttr, AtomicString::Number<short>(bt));
}

void HTMLVideoElement::setHbsMoovSize(uint16_t ms) {
  setAttribute(html_names::kHbsmoovsizeAttr, AtomicString::Number<short>(ms));
}
// ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
void HTMLVideoElement::UpdateUiPlayState(bool is_playing) {
  if (remoting_interstitial_) {
    remoting_interstitial_->UpdateUiPlayState(is_playing);
  }
}

void HTMLVideoElement::UpdateUiPlayPosition(int64_t position) {
  if (remoting_interstitial_) {
    remoting_interstitial_->UpdateUiPlayPosition(position);
  }
}

void HTMLVideoElement::UpdateRemotePlayState(bool is_playing) {
  for (auto& observer : GetMediaPlayerObserverRemoteSet()) {
    observer->UpdateRemotePlayState(is_playing);
  }
    
}

void HTMLVideoElement::UpdateRemotePlayPosition(int64_t position) {
  for (auto& observer : GetMediaPlayerObserverRemoteSet()) {
    observer->UpdateRemotePlayPosition(position);
  }
}

void HTMLVideoElement::NotifyRemoteInterstitial(MediaControlsSizingClass sizing_class) {
  LOG(INFO) << "HTMLVideoElement::NotifyRemoteExitFullScreen";
  sizing_class_ = sizing_class;
  if (remoting_interstitial_) {
    remoting_interstitial_->NotifyRemoteInterstitial(sizing_class);
  }
}

void HTMLVideoElement::UpdateRemoteFullScreenCss() {
  LOG(INFO) << "HTMLVideoElement::UpdateRemoteFullScreenCss";
#if !defined(COMPONENT_BUILD)
  if (remoting_interstitial_) {
    remoting_interstitial_->UpdateRemoteFullScreenCss(false);
  }
#endif // COMPONENT_BUILD
}
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)

}  // namespace blink