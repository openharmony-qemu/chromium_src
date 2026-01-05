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
#include "third_party/blink/renderer/core/html/media/html_media_element.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/html/media/html_media_element_utils.h"

#include "arkweb/build/features/features.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_frame_client.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/core/html/media/media_error.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/html/media/html_media_element_controls_list.h"
#include "third_party/blink/renderer/core/html/time_ranges.h"
#include "base/strings/string_split.h"

namespace blink {

HTMLMediaElementUtils::HTMLMediaElementUtils(HTMLMediaElement* element)
{
    this->htmlMediaElement_ = element;
}

void HTMLMediaElementUtils::DidPlayerMutedStatusChangeExt(bool muted) {
  for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
    observer->OnMutedStatusChanged(muted);
    observer->OnInitMediaTitle();
    observer->OnInitVideoPoster();
    auto mediaTitle =  htmlMediaElement_->GetMediaTitle();
    auto videoPoster =  htmlMediaElement_->GetVideoPoster();
    if (!mediaTitle.empty()) {
      observer->OnGetMediaTitle(mediaTitle);
    }
    if (!videoPoster.empty()) {
      observer->OnGetVideoPoster(videoPoster);
    }
  }
}

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_MEDIA) || BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
WebString HTMLMediaElementUtils::GetTitle() const {

  if (htmlMediaElement_->GetDocument().GetPage() && htmlMediaElement_->GetDocument().GetPage()->MainFrame()) {
    Frame* main_frame = htmlMediaElement_->GetDocument().GetPage()->MainFrame();
    if (main_frame && main_frame->IsLocalFrame()) {
      return DynamicTo<LocalFrame>(main_frame)->GetDocument()->title();
    }
  }
  return WebString();
}
#endif // ARKWEB_MEDIA || ARKWEB_VIDEO_ASSISTANT()
// LCOV_EXCL_STOP

void HTMLMediaElementUtils::ScheduleNamedEventUtils(const AtomicString& event_name) {
#if BUILDFLAG(ARKWEB_MEDIA)
  if (event_name == event_type_names::kPlaying ||
      event_name == event_type_names::kWaiting ||
      event_name == event_type_names::kSeeking ||
      event_name == event_type_names::kStalled) {
    LOG(INFO) << "OhMedia::ScheduleEvent() " << event_name;
  }
#endif // ARKWEB_MEDIA

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (htmlMediaElement_->IsCustomMediaPlayerEnabled()) {
    if (event_name == event_type_names::kTimeupdate) {
      TimeUpdateOverlay(htmlMediaElement_->currentTime());
      BufferedEndTimeChangedOverlay(CalculateBufferedEndTime());
    } else if (event_name == event_type_names::kDurationchange) {
      DurationChangedOverlay(htmlMediaElement_->duration());
    } else if (event_name == event_type_names::kEnded) {
      EndedOverlay();
    } else if (event_name == event_type_names::kSeeking) {
      SeekingOverlay();
    } else if (event_name == event_type_names::kSeeked) {
      SeekingFinishedOverlay();
    } else if (event_name == event_type_names::kPlaying) {
      UpdatePlayStateOverlay(PlayState::kPlaying);
    } else if (event_name == event_type_names::kWaiting) {
      UpdatePlayStateOverlay(PlayState::kWaiting);
    } else if (event_name == event_type_names::kError) {
      auto error_info = htmlMediaElement_->error();
      if (error_info) {
        ErrorOverlay(error_info->code(), error_info->message());
      }
#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
    played_time_recorder_.StopRecord();
    freeze_time_recorder_.StopRecord();
#endif // ARKWEB_MEDIA_CAPABILITIES_ENHANCE
    } else if (event_name == event_type_names::kVolumechange) {
      OnVolumeChanged(htmlMediaElement_->volume());
    }
  }
#endif // ARKWEB_VIDEO_ASSISTANT
#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  if (event_name == event_type_names::kPlay) {
    played_time_recorder_.StartRecord();
    freeze_time_recorder_.StartRecord();
  } else if (event_name == event_type_names::kPlaying) {
    freeze_time_recorder_.StopRecord();
  } else if (event_name == event_type_names::kPause) {
    played_time_recorder_.PauseRecord();
    freeze_time_recorder_.PauseRecord();
  }
#endif // ARKWEB_MEDIA_CAPABILITIES_ENHANCE
}
// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
void HTMLMediaElementUtils::ResetMediaPlayerAndMediaSourceUtils() {
  // reset mediaplayer and wait for new src, so reset flags too.
  if (!htmlMediaElement_->paused_ && htmlMediaElement_->ready_state_ < HTMLMediaElement::kHaveMetadata &&
      htmlMediaElement_->load_state_ == HTMLMediaElement::kLoadingFromSourceElement) {
    played_time_recorder_.StartRecord();
    freeze_time_recorder_.StartRecord();
  }
}
#endif // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
double HTMLMediaElementUtils::playedTime() {
  if (!IsFeedsPage()) {
    LOG(INFO) << "HTMLMediaElement::playedTime is not feedsPage";
    return 0.0;
  }
  played_time_recorder_.StopRecord();
  double total_played_time = played_time_recorder_.GetDuration().InMillisecondsF();
  played_time_recorder_.Reset();
  return total_played_time;
}

double HTMLMediaElementUtils::freezeTime() {
  if (!IsFeedsPage()) {
    LOG(INFO) << "HTMLMediaElement::freezeTime is not feedsPage";
    return 0.0;
  }
  freeze_time_recorder_.StopRecord();
  base::TimeDelta total_freeze_time = freeze_time_recorder_.GetDuration();
  if (htmlMediaElement_->web_media_player_) {
    total_freeze_time += base::Milliseconds(htmlMediaElement_->web_media_player_->GetFreezeTime());
  }
  freeze_time_recorder_.Reset();
  return total_freeze_time.InMillisecondsF();
}
#endif // ARKWEB_MEDIA_CAPABILITIES_ENHANCE
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)

void HTMLMediaElementUtils::Recorder::SetThreshold(base::TimeDelta threshold) {
  threshold_ = threshold;
}

void HTMLMediaElementUtils::Recorder::StartRecord() {
  if (start_time_ > base::TimeTicks()) {
    // avoid starting record twice.
    return;
  }
  start_time_ = base::TimeTicks::Now();
}

void HTMLMediaElementUtils::Recorder::PauseRecord() {
  if (start_time_ > base::TimeTicks()) {
    base::TimeDelta elapsed = base::TimeTicks::Now() - start_time_;
    if (elapsed > threshold_) {
      accumulated_duration_ += elapsed;
    }
    start_time_ = base::TimeTicks();
  }
}

void HTMLMediaElementUtils::Recorder::StopRecord() {
  if (total_duration_ > base::TimeDelta()) {
    return;
  }
  PauseRecord();
  total_duration_ = accumulated_duration_;
  accumulated_duration_ = base::TimeDelta();
}

base::TimeDelta HTMLMediaElementUtils::Recorder::GetDuration() {
  return total_duration_;
}

void HTMLMediaElementUtils::Recorder::Reset() {
  start_time_ = base::TimeTicks();
  accumulated_duration_ = base::TimeDelta();
  total_duration_ = base::TimeDelta();
}
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE


#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
bool HTMLMediaElementUtils::IsFeedsPage() const {
  if (!htmlMediaElement_->GetDocument().GetSettings()) {
    return false;
  }
  int32_t usage_scenario = htmlMediaElement_->GetDocument().GetSettings()->GetUsageScenario();
  return usage_scenario ==
         static_cast<int32_t>(ScenarioType::SCENARIO_FEEDSPAGE_TYPE);
}
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

bool HTMLMediaElementUtils::IsMediaPlayerShown() const {
  if (!htmlMediaElement_->web_media_player_) {
    return false;
  }
  return htmlMediaElement_->web_media_player_->IsMediaPlayerShown();
}

// LCOV_EXCL_START
media::mojom::blink::VideoAttributesForVASTPtr HTMLMediaElementUtils::CollectVideoAttributesForVAST() {
  auto attributes = media::mojom::blink::VideoAttributesForVAST::New();
  attributes->show_fullscreen_button = true;
  attributes->show_download_button = !htmlMediaElement_->controls_list_->ShouldHideDownload();
  attributes->show_playback_rate_menu = true;
  attributes->current_playback_rate = htmlMediaElement_->playbackRate();
  attributes->rect = htmlMediaElement_->video_rect_;
  attributes->supports_save = htmlMediaElement_->SupportsSave();
  attributes->duration = htmlMediaElement_->duration();
  attributes->visible = htmlMediaElement_->video_visible_ && IsMediaPlayerShown() &&
      !htmlMediaElement_->video_rect_.IsEmpty();
  LOG(INFO) << "attributes->visible:" << attributes->visible;
  return attributes;
}

void HTMLMediaElementUtils::TryNotifyVideoPlaying() {
  if (!htmlMediaElement_->video_assistant_enabled_) {
    return;
  }
  if (!htmlMediaElement_->video_assistant_) {
    auto callback = WTF::BindOnce(&HTMLMediaElement::NotifyVideoPlayingInternal,
                                  WrapWeakPersistent(htmlMediaElement_.Get()));
    htmlMediaElement_->GetMediaPlayerHostRemote().RequestVideoAssistantConfig(
        WTF::BindOnce(&HTMLMediaElement::OnVideoAssistantConfigReceived,
                      WrapWeakPersistent(htmlMediaElement_.Get()), std::move(callback)));
    return;
  }
  htmlMediaElement_->NotifyVideoPlayingInternal();
}


void HTMLMediaElementUtils::UpdateVideoAssistantAttributes() {
  if (!htmlMediaElement_->IsVideoAssistantEnabled()) {
    return;
  }
  if (htmlMediaElement_->IsHTMLVideoElement()) {
    auto attributes = CollectVideoAttributesForVAST();
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      observer->OnUpdateVideoAttributes(attributes.Clone());
    }
  }
}
// LCOV_EXCL_STOP

void HTMLMediaElementUtils::NotifyVideoVisible(bool visible) {
  if (!htmlMediaElement_->IsVideoAssistantEnabled()) {
    return;
  }
  bool changed = htmlMediaElement_->video_visible_ != visible;
  LOG(INFO) << "NotifyVideoVisible(" << visible << "), changed[" << changed
            << "]";
  htmlMediaElement_->video_visible_ = visible;
  if (changed) {
    UpdateVideoAssistantAttributes();
  }
}

void HTMLMediaElementUtils::EnterFullScreenOverlay() {
  if (!htmlMediaElement_->IsCustomMediaPlayerEnabled()) {
    return;
  }
  if (htmlMediaElement_->GetWebMediaPlayer() && !htmlMediaElement_->GetWebMediaPlayer()->SupportVideoSurface()) {
    LOG(INFO) << "Decoder doesn't support video surface.";
    htmlMediaElement_->SetUserWantsControlsVisible(false);
  }
  LOG(INFO) << "EnterFullScreenOverlay";
  if (htmlMediaElement_->IsHTMLVideoElement()) {
    auto attributes = htmlMediaElement_->CollectMediaInfoAttributesForVAST();
    if (!attributes) {
      LOG(INFO) << "EnterFullScreenOverlay, failed, no player";
      return;
    }
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      observer->OnFullScreenOverlayEnter(attributes.Clone());
    }
  }
}

void HTMLMediaElementUtils::UpdatePlayStateOverlay(PlayState playState) {
  if (!htmlMediaElement_->IsCustomMediaPlayerEnabled()) {
    return;
  }
  LOG(INFO) << "UpdatePlayStateOverlay(" << static_cast<uint32_t>(playState) << ")";
  if (htmlMediaElement_->IsHTMLVideoElement()) {
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      observer->UpdatePlayStateOverlay(static_cast<uint32_t>(playState));
    }
  }
}

void HTMLMediaElementUtils::MutedChangedOverlay(bool muted) {
  if (!htmlMediaElement_->IsCustomMediaPlayerEnabled()) {
    return;
  }
  LOG(INFO) << "MutedChangedOverlay";
  if (htmlMediaElement_->IsHTMLVideoElement()) {
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      observer->MutedChangedOverlay(muted);
    }
  }
}

void HTMLMediaElementUtils::PlaybackRateChangedOverlay(double playback_rate) {
  if (!htmlMediaElement_->IsCustomMediaPlayerEnabled()) {
    return;
  }
  LOG(INFO) << "PlaybackRateChangedOverlay";
  if (htmlMediaElement_->IsHTMLVideoElement()) {
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      observer->PlaybackRateChangedOverlay(playback_rate);
    }
  }
}

void HTMLMediaElementUtils::DurationChangedOverlay(double duration) {
  if (!htmlMediaElement_->IsCustomMediaPlayerEnabled()) {
    return;
  }
  LOG(INFO) << "DurationChangedOverlay";
  if (htmlMediaElement_->IsHTMLVideoElement()) {
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      observer->DurationChangedOverlay(duration);
    }
  }
}

void HTMLMediaElementUtils::TimeUpdateOverlay(double current_time) {
  if (!htmlMediaElement_->IsCustomMediaPlayerEnabled()) {
    return;
  }
  if (htmlMediaElement_->IsHTMLVideoElement()) {
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      observer->TimeUpdateOverlay(current_time);
    }
  }
}

void HTMLMediaElementUtils::BufferedEndTimeChangedOverlay(double buffered_end_time) {
  if (!htmlMediaElement_->IsCustomMediaPlayerEnabled()) {
    return;
  }
  if (htmlMediaElement_->IsHTMLVideoElement()) {
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      observer->BufferedEndTimeChangedOverlay(buffered_end_time);
    }
  }
}

// LCOV_EXCL_START
double HTMLMediaElementUtils::CalculateBufferedEndTime()
{
  const double kCurrentTimeBufferedDelta = 1.0;
  TimeRanges* buffered_time_ranges = htmlMediaElement_->buffered();
  DCHECK(buffered_time_ranges);
  if (std::isnan(htmlMediaElement_->duration()) || std::isinf(htmlMediaElement_->duration()) || !htmlMediaElement_->duration() ||
      std::isnan(htmlMediaElement_->currentTime())) {
    return 0;
  }

  for (unsigned i = 0; i < buffered_time_ranges->length(); ++i) {
    float start = buffered_time_ranges->start(i, ASSERT_NO_EXCEPTION);
    float end = buffered_time_ranges->end(i, ASSERT_NO_EXCEPTION);
    if (std::isnan(start) || std::isnan(end) ||
        start > htmlMediaElement_->currentTime() + kCurrentTimeBufferedDelta ||
        end < htmlMediaElement_->currentTime()) {
      continue;
    }
    return end;
  }
  return 0;
}

void HTMLMediaElementUtils::EndedOverlay() {
  if (!htmlMediaElement_->IsCustomMediaPlayerEnabled()) {
    return;
  }
  LOG(INFO) << "EndedOverlay";
  if (htmlMediaElement_->IsHTMLVideoElement()) {
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      observer->EndedOverlay();
    }
  }
}
// LCOV_EXCL_STOP

void HTMLMediaElementUtils::FullscreenChangedOverlay(bool fullscreen) {
  if (!htmlMediaElement_->IsCustomMediaPlayerEnabled()) {
    return;
  }
  LOG(INFO) << "FullscreenChangedOverlay";
  if (!fullscreen) {
    htmlMediaElement_->user_wants_controls_visible_.reset();
    htmlMediaElement_->UpdateControlsVisibility();
  }
  if (htmlMediaElement_->IsHTMLVideoElement()) {
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      observer->FullscreenChangedOverlay(fullscreen);
    }
  }
}

// LCOV_EXCL_START
void HTMLMediaElementUtils::SeekingOverlay() {
  if (!htmlMediaElement_->IsCustomMediaPlayerEnabled()) {
    return;
  }
  LOG(INFO) << "SeekingOverlay";
  if (htmlMediaElement_->IsHTMLVideoElement()) {
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      observer->SeekingOverlay();
    }
  }
}

void HTMLMediaElementUtils::SeekingFinishedOverlay() {
  if (!htmlMediaElement_->IsCustomMediaPlayerEnabled()) {
    return;
  }
  LOG(INFO) << "SeekingFinishedOverlay";
  if (htmlMediaElement_->IsHTMLVideoElement()) {
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      observer->SeekingFinishedOverlay();
    }
  }
}
// LCOV_EXCL_STOP

void HTMLMediaElementUtils::ErrorOverlay(int32_t error_code, const String& error_msg) {
  if (!htmlMediaElement_->IsCustomMediaPlayerEnabled()) {
    return;
  }
  LOG(INFO) << "ErrorOverlay";
  if (htmlMediaElement_->IsHTMLVideoElement()) {
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      if (error_msg.IsNull()) {
        String error_str = "";
        observer->ErrorOverlay(error_code, error_str);
      } else {
        observer->ErrorOverlay(error_code, error_msg);
      }
    }
  }
}

void HTMLMediaElementUtils::VideoSizeChangedOverlay(int32_t width, int32_t height) {
  if (!htmlMediaElement_->IsCustomMediaPlayerEnabled()) {
    return;
  }
  LOG(DEBUG) << "VideoSizeChangedOverlay";
  if (htmlMediaElement_->IsHTMLVideoElement()) {
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      observer->VideoSizeChangedOverlay(width, height);
    }
  }
}

void HTMLMediaElementUtils::OnVolumeChanged(double volume)
{
  LOG(INFO) << "HTMLMediaElementUtils::OnVolumeChanged volume=" << volume;
  if (!htmlMediaElement_ || !htmlMediaElement_->IsCustomMediaPlayerEnabled()) {
    return;
  }

  if (htmlMediaElement_->IsHTMLVideoElement()) {
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      LOG(INFO) << "HTMLMediaElementUtils::OnVolumeChanged for observer, volume=" << volume;
      observer->OnVolumeChanged(volume);
    }
  }
}

bool HTMLMediaElementUtils::IsRTL() const {
  bool isRTL = base::i18n::IsRTL();
  const std::string locale = base::i18n::GetConfiguredLocale();
  std::vector<std::string_view> locale_split = base::SplitStringPiece(
      locale, "-_", base::KEEP_WHITESPACE, base::SPLIT_WANT_ALL);
  const std::string_view& language_code = locale_split[0];
  if (language_code.compare("ug") == 0) {
    isRTL = true;
  }
  return isRTL && locale.find("ur") == std::string::npos;
}

void HTMLMediaElementUtils::Trace(Visitor* visitor) const {
  visitor->Trace(htmlMediaElement_);
}

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
bool HTMLMediaElementUtils::IsUseVideoLoadOptimization() const {
  return htmlMediaElement_->GetDocument().IsUseVideoLoadOptimization();
}

void HTMLMediaElementUtils::SetVideoIsPlaying(bool playing) {
  std::string videoStr = htmlMediaElement_->videoId();
  LOG(INFO) << "VideoOpt SetVideoIsPlaying, videoId:" << videoStr << ", playing:" << playing;
  htmlMediaElement_->GetDocument().SetVideoIsPlaying(videoStr, playing);
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
void HTMLMediaElementUtils::OnMediaCastEnter() {
  LOG(INFO) << "HTMLMediaElementUtils::OnMediaCastEnter";
  if (!htmlMediaElement_) {
    LOG(ERROR) << "HTMLMediaElementUtils::OnMediaCastEnter, htmlMediaElement_ is nullptr";
    return;
  }
  for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
    observer->OnMediaCastEnter();
  }
}

void HTMLMediaElementUtils::OnNotifyMeidaCastUri() {
  if (!htmlMediaElement_) {
    LOG(ERROR) << "HTMLMediaElementUtils::OnNotifyMeidaCastUri, htmlMediaElement_ is nullptr";
    return;
  }

  auto mediaUri = (htmlMediaElement_->currentSrc()).GetString();
  if (mediaUri.IsNull() || mediaUri.empty()) {
    LOG(ERROR) << "OnNotifyMeidaCastUri, mediaUri IsNull";
    return;
  }
  if (htmlMediaElement_->IsHTMLVideoElement()) {
    for (auto& observer : htmlMediaElement_->media_player_observer_remote_set_->Value()) {
      observer->OnNotifyMeidaCastUri(mediaUri);
    }
  }
}
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)

}
