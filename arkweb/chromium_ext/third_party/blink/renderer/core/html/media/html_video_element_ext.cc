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
namespace blink {

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

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void HTMLVideoElement::SetPlaybackRate(double playback_rate) {
  setPlaybackRate(playback_rate);
}

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

#if BUILDFLAG(ARKWEB_PIP)
void HTMLVideoElement::OnPictureInPictureStateChanged(uint32_t state) {
  if (!web_media_player_ || !pip_down_) {
    pip_down_ = true;
    return;
  }

  for (auto& observer : GetMediaPlayerObserverRemoteSet()) {

    observer->OnPictureInPictureStateChanged(state);
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
#endif
}  // namespace blink