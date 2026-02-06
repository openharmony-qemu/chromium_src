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
#include "arkweb/chromium_ext/third_party/blink/renderer/core/html/media/html_media_element_utils.h"

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
#include "third_party/blink/renderer/core/frame/page_scale_constraints_set.h"
#include "third_party/blink/renderer/platform/weborigin/security_policy.h"
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
#include "arkweb/chromium_ext/url/ohos/log_utils.h"
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "base/base_switches.h"
#include "base/strings/string_util.h"
#include "third_party/blink/renderer/core/frame/visual_viewport.h"
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
#include "arkweb/ohos_nweb/src/sysevent/event_reporter.h"
#endif // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION) || BUILDFLAG(ARKWEB_MEDIA_CAST)
#include "third_party/blink/renderer/core/html/media/html_video_element.h"
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION || ARKWEB_MEDIA_CAST

namespace blink {
namespace {
extern std::string GetFormatFromType(std::string type);
#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
extern String BuildElementErrorMessage(const String& error);
#endif // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
float PageConstraintInitalScale(const Document& document) {
  float scale = 1.0;
  if (auto* page = document.GetPage()) {
    scale = page->GetPageScaleConstraintsSet().FinalConstraints().initial_scale;
  } else {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO) << "using default scale 1.0";
#endif  // ARKWEB_LOGGER_REPORT
  }
  return scale;
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
}  // anonymous namespace

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
bool HTMLMediaElement::IsMuted() {
  return muted_;
}
uint32_t HTMLMediaElement::IsMediaMuted() {
  static constexpr uint32_t notMuted = 1;
  if (!HasAudio()) {
    static constexpr uint32_t noAudioTracks = 2;
    return noAudioTracks;
  }
  if (muted_) {
    static constexpr uint32_t muted = 0;
    return muted;
  }
  return notMuted;
}


bool HTMLMediaElement::IsUsedCustomVideoPlayer() {
  return IsCustomVideoPlayerEnabled();
}
bool HTMLMediaElement::IsCustomVideoPlayerEnabled() {
  return should_create_custom_renderer_ && GetDocument().GetSettings() &&
         GetDocument().GetSettings()->IsCustomVideoPlayerEnabled();
}
bool HTMLMediaElement::ShouldCustomVideoPlayerOverlay() {
  return GetDocument().GetSettings() &&
         GetDocument().GetSettings()->IsCustomVideoPlayerOverlay();
}
bool HTMLMediaElement::ShouldShowMediaControls() {
  return FastHasAttribute(html_names::kControlsAttr);
}

std::string HTMLMediaElement::GetMediaFormat() {
  return media_format_;
}

void HTMLMediaElement::RestartForPrimitive() {
  should_create_custom_renderer_ = false;
  next_child_node_to_consider_ = next_retry_child_node_;
}

Vector<media::Renderer::MediaSourceInfo>
HTMLMediaElement::GetRemainSourceInfos() {
  Vector<media::Renderer::MediaSourceInfo> source_infos;
  if (load_state_ == kLoadingFromSourceElement) {
    NodeVector potential_source_nodes;
    GetChildNodes(*this, potential_source_nodes);

    next_retry_child_node_ = next_child_node_to_consider_;

    bool looking_for_start_node = false;
    for (unsigned i = 0; i < potential_source_nodes.size(); ++i) {
      Node* node = potential_source_nodes[i].Get();
      if (!looking_for_start_node && (next_child_node_to_consider_ != node)) {
        continue;
      }

      looking_for_start_node = true;
      HTMLSourceElement* source = DynamicTo<HTMLSourceElement>(node);
      if (!source || (node->parentNode() != this)) {
        continue;
      }

      current_source_node_ = source;
      next_child_node_to_consider_ = source->nextSibling();

      const AtomicString& src_value =
          source->FastGetAttribute(html_names::kSrcAttr);
      if (src_value.empty()) {
        continue;
      }

      media::Renderer::MediaSourceInfo source_info;
      source_info.media_format = GetFormatFromType(source->type().Latin1());
      source_info.media_source =
          source->GetDocument().CompleteURL(src_value).GetString().Latin1();
      source_infos.emplace_back(source_info);
    }
  }
  return source_infos;
}
Vector<WebString> HTMLMediaElement::GetMediaControlsList() {
  Vector<WebString> list;
  list.reserve(controls_list_->length());
  for (unsigned i = 0; i < controls_list_->length(); i++) {
    list.emplace_back(controls_list_->item(i));
  }
  return list;
}

base::flat_map<std::string, std::string>
HTMLMediaElement::GetElementAttributes() {
  base::flat_map<std::string, std::string> attributes_map;
  AttributeCollection attribute_collection = Element::AttributesWithoutUpdate();
  for (const auto& attribute : attribute_collection) {
    attributes_map.insert(
        {attribute.LocalName().Utf8(), attribute.Value().Utf8()});
  }
  return attributes_map;
}
std::string HTMLMediaElement::GetOutgoingReferrerString() {
  ExecutionContext* context = GetExecutionContext();
  if (!context) {
    return std::string();
  }
  return SecurityPolicy::GenerateReferrer(context->GetReferrerPolicy(),
                                          currentSrc(),
                                          context->OutgoingReferrer())
      .referrer.Utf8();
}

void HTMLMediaElement::UpdatePlaybackStatus(uint32_t status) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO) << "UpdatePlaybackStatus(" << status << "), paused_["
                     << paused_ << "]";
#endif

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO) << "UpdatePlaybackStatus(" << status << "), paused_["
                     << paused_ << "]";
#endif  // ARKWEB_LOGGER_REPORT

  base::AutoReset<bool> resetter(&played_by_custom_mp_, true);
  if (paused_ == !status) {
    return;
  }
  if (status) {
    PlayInternal();
  } else {
    PauseInternal(PlayPromiseError::kPaused_PauseCalled);
  }
}
void HTMLMediaElement::UpdateVolume(double volume) {
  if (volume_ == volume) {
    return;
  }
  volume_ = volume;
  ScheduleNamedEvent(event_type_names::kVolumechange);
}
void HTMLMediaElement::UpdateMuted(bool muted) {
  setMuted(muted);
}
void HTMLMediaElement::UpdatePlaybackRate(double playback_rate) {
  if (playback_rate_ == playback_rate) {
    return;
  }
  playback_rate_ = playback_rate;
  ScheduleNamedEvent(event_type_names::kRatechange);
}
gfx::Rect HTMLMediaElement::GetVideoRect() {
  if (!layer_rect_.IsEmpty()) {
    return layer_rect_;
  }
  if (GetLayoutObject() && GetLayoutObject()->IsBox()) {
    auto* layout_box = To<LayoutBox>(GetLayoutObject());
    return gfx::Rect(ToFlooredPoint(layout_box->Location()),
                     ToFlooredSize(layout_box->Size()));
  }

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO) << "using default vidoe size";
#endif  // ARKWEB_LOGGER_REPORT

  return gfx::Rect(LayoutReplaced::kDefaultWidth,
                   LayoutReplaced::kDefaultHeight);
}
void HTMLMediaElement::OnLayerRectChange(const gfx::Rect& rect) {
  if (layer_rect_ == rect) {
    return;
  }
  layer_rect_ = rect;
  float scale = PageConstraintInitalScale(GetDocument());
  for (auto& observer : media_player_observer_remote_set_->Value()) {
    observer->UpdateLayerRect(gfx::ScaleToEnclosingRect(layer_rect_, scale));
  }
}

void HTMLMediaElement::FullscreenChanged(bool is_fullscreen) {
  DVLOG(1) << "FullscreenChanged(" << is_fullscreen << ")";
  for (auto& observer : media_player_observer_remote_set_->Value()) {
    observer->FullscreenChanged(is_fullscreen);
  }
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
void HTMLMediaElement::SetMediaTitle() {
  media_title_ = FastGetAttribute(html_names::kTitleAttr).GetString();
}

String HTMLMediaElement::GetMediaTitle() const {
  if (media_title_.empty()) {
    return String();
  }
  return media_title_;
}

String HTMLMediaElement::GetVideoPoster() const {
  if (video_poster_.empty()) {
    return String();
  }
  return video_poster_;
}

void HTMLMediaElement::DidEndAVSession(bool is_hidden) {
  LOG(INFO) << "DidEndAVSession";
  for (auto& observer : media_player_observer_remote_set_->Value())
    observer->OnEndAVSession(is_hidden);
}
#endif  // ARKWEB_MEDIA_AVSESSION
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
bool HTMLMediaElement::IsVideoAssistantEnabled() {
  if (!video_assistant_enabled_) {
    return false;
  }
  return video_assistant_.value_or(true);
}

void HTMLMediaElement::OnLayerBoundsChange(const gfx::Rect& bounds) {
  if (has_been_seen_playing_once_) {
    return;
  }
  video_rect_ = gfx::RectF(bounds);
  if (GetDocument().GetPage()) {
    const auto& visual_viewport = GetDocument().GetPage()->GetVisualViewport();
    gfx::Rect visible_rect = visual_viewport.VisibleContentRect();
    visible_rect = gfx::Rect(visible_rect.size());
    if (!bounds.Intersects(visible_rect)) {
      video_rect_ = gfx::RectF();
    }
  }
  if (has_notified_playing_) {
    has_been_seen_playing_once_ = !video_rect_.IsEmpty();
  }
  html_media_element_utils_.UpdateVideoAssistantAttributes();
}

void HTMLMediaElement::OnPageVisibilityChanged() {
  html_media_element_utils_.UpdateVideoAssistantAttributes();
}

void HTMLMediaElement::NotifyVideoPlayingInternal() {
  if (!IsVideoAssistantEnabled()) {
    return;
  }
  const int delay_in_ms = 100;
  if (!notify_video_playing_timer_.IsActive()) {
    notify_video_playing_timer_.StartOneShot(base::Milliseconds(delay_in_ms),
                                             FROM_HERE);
  }
}

void HTMLMediaElement::NotifyVideoDestroyed() {
  if (!IsVideoAssistantEnabled()) {
    return;
  }
  LOG(INFO) << "NotifyVideoDestroyed";
  if (IsHTMLVideoElement()) {
    for (auto& observer : media_player_observer_remote_set_->Value()) {
      observer->OnVideoDestroyed();
    }
  }
}

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
std::string HTMLMediaElement::AddErrorCodeToMessage(
    WebMediaPlayer::NetworkState state,
    const String& error) {
  StringBuilder builder;
  int net_error_code = 0;
  media::PipelineStatus pipeline_status = media::PIPELINE_OK;
  if (GetWebMediaPlayer()) {
    pipeline_status = GetWebMediaPlayer()->GetPipelineStatus();
    net_error_code = GetWebMediaPlayer()->GetWebURLErrorReason();
  }

  builder.AppendFormat(
      "[internal_error:pipeline:%d,blink:%d,net_error_code:%d]",
      pipeline_status.code(), state, net_error_code);
  builder.Append(error);
  return builder.ToString().Utf8();
}

void HTMLMediaElement::ReportMediaLoadingErrorMessage(
    const std::string& error_type,
    WebMediaPlayer::NetworkState error,
    const String& message,
    bool is_message_not_clear) {
  StringBuilder builder;
  if (!is_message_not_clear) {
    builder.Append(message);
  } else {
    builder.Append((error == WebMediaPlayer::kNetworkStateFormatError
                       ? "Format error"
                       : "Network error"));
  }

  ReportWebMediaPlayErrorInfo(
      error_type, error, AddErrorCodeToMessage(error, builder.ToString()));
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(ERROR) << "Media loadding failed message: "
                      << AddErrorCodeToMessage(error, builder.ToString());
#endif // ARKWEB_LOGGER_REPORT
}

void HTMLMediaElement::SetVideoExperienceMojo() {
  if (is_logger_export_) {
    LocalFrame* frame = GetDocument().GetFrame();
    if (frame) {
      if (video_experience_reporter_) {
        video_experience_reporter_.reset();
      }
      frame->GetBrowserInterfaceBroker().GetInterface(
        video_experience_reporter_.BindNewPipeAndPassReceiver(nullptr));
    }
  }
}
#endif // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

media::mojom::blink::MediaInfoForVASTPtr
HTMLMediaElement::CollectMediaInfoAttributesForVAST() {
  auto media_player = GetWebMediaPlayer();
  if (!media_player) {
    return nullptr;
  }
  auto mediaInfoAttr = media::mojom::blink::MediaInfoForVAST::New();
  mediaInfoAttr->id = WTF::String::FromUTF8(GetIdAttribute().Utf8());
  mediaInfoAttr->title = WTF::String::FromUTF8(html_media_element_utils_.GetTitle().Utf8());
  mediaInfoAttr->duration = duration();
  mediaInfoAttr->volume = volume();
  mediaInfoAttr->current_time = media_player->CurrentTime();
  mediaInfoAttr->playback_rate = playbackRate();
  mediaInfoAttr->video_width = media_player->NaturalSize().width();
  mediaInfoAttr->video_height = media_player->NaturalSize().height();
  mediaInfoAttr->isMuted = IsMediaMuted();
  mediaInfoAttr->isPlaying = !media_player->Paused();
  mediaInfoAttr->isShowPlaybackSpeed = true;
  mediaInfoAttr->show_download_button = !controls_list_->ShouldHideDownload();
  mediaInfoAttr->supports_save = SupportsSave();
  mediaInfoAttr->fullscreen_overlay = true;

  if (GetWebMediaPlayer() && !GetWebMediaPlayer()->SupportVideoSurface()) {
    mediaInfoAttr->fullscreen_overlay = false;
  }
  return mediaInfoAttr;
}

void HTMLMediaElement::OnVideoAssistantConfigReceived(
    base::OnceCallback<void()> callback,
    media::mojom::blink::VideoAssistantConfigPtr config) {
  LOG(INFO) << "OnVideoAssistantConfigReceived(" << config->video_assistant
            << ")";
  video_assistant_ = config->video_assistant;
  if (!video_assistant_.value()) {
    return;
  }
  std::move(callback).Run();
}
void HTMLMediaElement::OnNotifyVideoPlayingTimerFired(TimerBase*) {
  LOG(INFO) << "OnNotifyVideoPlayingTimerFired";
  has_notified_playing_ = true;
  if (IsHTMLVideoElement()) {
    auto attributes = html_media_element_utils_.CollectVideoAttributesForVAST();
    if (!has_been_seen_playing_once_) {
      has_been_seen_playing_once_ = !video_rect_.IsEmpty();
    }
    for (auto& observer : media_player_observer_remote_set_->Value()) {
      observer->OnVideoPlaying(attributes.Clone());
    }
  }
}

bool HTMLMediaElement::IsCustomMediaPlayerEnabled() const {
  return GetDocument().GetSettings() &&
        GetDocument().GetSettings()->GetCustomMediaPlayerEnabled() &&
        IsHTMLVideoElement();
}

void HTMLMediaElement::OnSupportVideoSurfaceChanged(
    bool support, std::string decoder_name) {
  LOG(INFO) << "OnSupportVideoSurfaceChanged("
            << support << ", " << decoder_name << ")";
  if (!IsCustomMediaPlayerEnabled()) {
    return;
  }
  if (!support && IsFullscreen()) {
    SetUserWantsControlsVisible(false);
  } else {
    user_wants_controls_visible_.reset();
    UpdateControlsVisibility();
  }
  for (auto& observer : media_player_observer_remote_set_->Value()) {
    observer->FullscreenOverlayChanged(support, WTF::String(decoder_name));
  }
}

void HTMLMediaElement::SetVideoSurface(int32_t widget_id) {
  LOG(INFO) << "SetVideoSurface(" << widget_id << ")";
  if (GetWebMediaPlayer()) {
    GetWebMediaPlayer()->SetVideoSurface(widget_id);
  }
}

void HTMLMediaElement::SetVolume(double volume)
{
  LOG(INFO) << "HTMLMediaElement::SetVolume volume=" << volume;
  setVolume(volume);
}

void HTMLMediaElement::GetVolume(GetVolumeCallback callback)
{
  LOG(INFO) << "HTMLMediaElement::GetVolume volume=" << EffectiveMediaVolume();
  std::move(callback).Run(EffectiveMediaVolume());
}

void HTMLMediaElement::RequestExitFullscreenIfNeeded() {
  Element* element = Fullscreen::FullscreenElementFrom(GetDocument());
  if (!element || element == this) {
    return;
  }
  HTMLMediaElement* fullscreen_video_element =
      DynamicTo<HTMLMediaElement>(element);
  if (!fullscreen_video_element) {
    return;
  }
  if (!IsCustomMediaPlayerEnabled()) {
    return;
  }
  if (!fullscreen_video_element->GetWebMediaPlayer() ||
      fullscreen_video_element->ended()) {
    Fullscreen::ExitFullscreen(GetDocument());
  }
}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_PIP)
void HTMLMediaElement::PipEnable(bool enable) {
  LOG(INFO) << "HTMLMediaElement::PipEnable(" << enable << ")";
  if (GetWebMediaPlayer()) {
    GetWebMediaPlayer()->PipEnable(enable);
  }
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
void HTMLMediaElement::OnDmaBufferSeekTo(base::TimeDelta dmabuf_pause_time) {
  double time = dmabuf_pause_time.InSecondsF();
  LOG(INFO) << "DMABUF::" << __func__ << "(" << *this << "),  time=" << time;
  Seek(time);
}

void HTMLMediaElement::RecycleDmaBuffer() {
  if (GetWebMediaPlayer()) {
    LOG(INFO) << "DMABUF::" << __func__ << "(" << *this << ")";
    GetWebMediaPlayer()->RecycleDmaBuffer();
  }
}

void HTMLMediaElement::ResumeDmaBuffer() {
  if (GetWebMediaPlayer()) {
    LOG(INFO) << "DMABUF::" << __func__ << "(" << *this << ")";
    GetWebMediaPlayer()->ResumeDmaBuffer();
  }
}
#endif  // ARKWEB_MEDIA_DMABUF

#if BUILDFLAG(ARKWEB_BFCACHE)
bool HTMLMediaElement::IsMediaResumeFromBFCachePage() const {
  if (GetDocument().GetSettings()) {
    return GetDocument().GetSettings()->GetMediaResumeFromBFCachePage();
  }
  return true;
}
#endif  // BUILDFLAG(ARKWEB_BFCACHE)

#if BUILDFLAG(ARKWEB_MEDIA_MEMORY_PRESSURE)
void HTMLMediaElement::NotifyMemoryLevel(int32_t level) {
  LOG(INFO) << "DMABUF::" << __func__ << "(" << *this << "),  level=" << level;
  switch (level) {
    case 0:
      memory_pressure_level_ = base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_NONE;
      break;
    case 1:
      memory_pressure_level_ = base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_MODERATE;
      break;
    case 2:
      memory_pressure_level_ = base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_CRITICAL;
      break;
    default:
      LOG(WARNING) << "DMABUF::" << __func__ << ": Unknown memory level " << level;
      memory_pressure_level_ = base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_NONE;
      break;
  }
  if (GetWebMediaPlayer()) {
    GetWebMediaPlayer()->NotifyMemoryLevel(memory_pressure_level_);
  }
  if (memory_pressure_level_ >= base::MemoryPressureListener::MEMORY_PRESSURE_LEVEL_MODERATE) {
    RecycleDmaBuffer();
  }
}
#endif  // ARKWEB_MEDIA_MEMORY_PRESSURE

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
std::string HTMLMediaElement::videoId() {
  return GetIdAttribute().Utf8();
}

uint16_t HTMLMediaElement::hbsMediaPreloadTime() {
  if (auto* video_element = DynamicTo<HTMLVideoElement>(this)) {
    return video_element->hbsPreloadTime();
  }
  return UINT16_MAX;
}

uint16_t HTMLMediaElement::hbsMediaMaxCacheTime() {
  if (auto* video_element = DynamicTo<HTMLVideoElement>(this)) {
    return video_element->hbsMaxCacheTime();
  }
  return UINT16_MAX;
}

uint16_t HTMLMediaElement::hbsMediaMinCacheTime() {
  if (auto* video_element = DynamicTo<HTMLVideoElement>(this)) {
    return video_element->hbsMinCacheTime();
  }
  return UINT16_MAX;
}

uint16_t HTMLMediaElement::hbsMediaBitrate() {
  if (auto* video_element = DynamicTo<HTMLVideoElement>(this)) {
    return video_element->hbsBitrate();
  }
  return UINT16_MAX;
}

uint16_t HTMLMediaElement::hbsMediaMoovSize() {
  if (auto* video_element = DynamicTo<HTMLVideoElement>(this)) {
    return video_element->hbsMoovSize();
  }
  return UINT16_MAX;
}

bool HTMLMediaElement::IsUseVideoLoadOptimization() const {
  return GetDocument().IsUseVideoLoadOptimization();
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
// Please make incremental modifications based on the native implementation during each upgrade.
void HTMLMediaElement::MediaLoadingFailed(WebMediaPlayer::NetworkState error,
                                          const String& input_message) {
  DVLOG(3) << "MediaLoadingFailed(" << *this << ", " << int{error}
           << ", message='" << input_message << "')";
 
#if BUILDFLAG(ARKWEB_MEDIA)
  LOG(INFO) << "OhMedia::MediaLoadingFailed error = " << (int)error
            << ", input_message = " << input_message << ")";
#endif // ARKWEB_MEDIA
  bool should_be_opaque = MediaShouldBeOpaque();
  if (should_be_opaque)
    error = WebMediaPlayer::kNetworkStateNetworkError;
  String empty_string;
  const String& message = should_be_opaque ? empty_string : input_message;
 
  StopPeriodicTimers();
 
  // If we failed while trying to load a <source> element, the movie was never
  // parsed, and there are more <source> children, schedule the next one
  if (ready_state_ < kHaveMetadata &&
      load_state_ == kLoadingFromSourceElement) {
    // resource selection algorithm
    // Step 9.Otherwise.9 - Failed with elements: Queue a task, using the DOM
    // manipulation task source, to fire a simple event named error at the
    // candidate element.
    if (current_source_node_) {
      current_source_node_->ScheduleErrorEvent();
    } else {
      DVLOG(3) << "mediaLoadingFailed(" << *this
               << ") - error event not sent, <source> was removed";
    }
 
    // 9.Otherwise.10 - Asynchronously await a stable state. The synchronous
    // section consists of all the remaining steps of this algorithm until the
    // algorithm says the synchronous section has ended.
 
    // 9.Otherwise.11 - Forget the media element's media-resource-specific
    // tracks.
    ForgetResourceSpecificTracks();
 
    if (HavePotentialSourceChild()) {
      DVLOG(3) << "mediaLoadingFailed(" << *this
               << ") - scheduling next <source>";
      ScheduleNextSourceChild();
    } else {
      DVLOG(3) << "mediaLoadingFailed(" << *this
               << ") - no more <source> elements, waiting";
      WaitForSourceChange();
    }
 
    return;
  }
 
  std::string error_type;
  bool is_message_not_clear = false;
  if (error == WebMediaPlayer::kNetworkStateNetworkError &&
      ready_state_ >= kHaveMetadata) {
    if (html_media_element_utils_.IsFeedsPage()) {
      MediaEngineError(MakeGarbageCollected<MediaError>(
          MediaError::kMediaErrNetwork, AddErrorCodeToMessage(error, message).c_str()));
    } else {
      MediaEngineError(MakeGarbageCollected<MediaError>(
          MediaError::kMediaErrNetwork, message));
    }
    error_type = "Network Error";
  } else if (error == WebMediaPlayer::kNetworkStateDecodeError) {
    if (html_media_element_utils_.IsFeedsPage()) {
      MediaEngineError(MakeGarbageCollected<MediaError>(
          MediaError::kMediaErrDecode, AddErrorCodeToMessage(error, message).c_str()));
    } else {
      MediaEngineError(MakeGarbageCollected<MediaError>(
          MediaError::kMediaErrDecode, message));
    }
    error_type = "Decode Error";
  } else if ((error == WebMediaPlayer::kNetworkStateFormatError ||
              error == WebMediaPlayer::kNetworkStateNetworkError) &&
             (load_state_ == kLoadingFromSrcAttr ||
              (load_state_ == kLoadingFromSrcObject &&
               src_object_media_source_handle_))) {
    if (message.empty()) {
      // Generate a more meaningful error message to differentiate the two types
      // of MEDIA_SRC_ERR_NOT_SUPPORTED.
      if (html_media_element_utils_.IsFeedsPage()) {
        NoneSupported(BuildElementErrorMessage(AddErrorCodeToMessage(error,
            (error == WebMediaPlayer::kNetworkStateFormatError ? "Format error"
                                                               : "Network error")).c_str()));
      } else {
        NoneSupported(BuildElementErrorMessage(
            error == WebMediaPlayer::kNetworkStateFormatError ? "Format error"
                                                              : "Network error"));
      }
      is_message_not_clear = true;
      error_type =
          (error == WebMediaPlayer::kNetworkStateFormatError ? "Format Error"
                                                             : "Network Error");
    } else {
      if (html_media_element_utils_.IsFeedsPage()) {
        NoneSupported(BuildElementErrorMessage(AddErrorCodeToMessage(error,message).c_str()));
      } else {
        NoneSupported(message);
      }
      error_type = "Others";
    }
  }
 
  ReportMediaLoadingErrorMessage(error_type, error, message, is_message_not_clear);

  if (is_logger_export_ && !html_media_element_utils_.IsFeedsPage()) {
    LOG(INFO) << "OhMedia, MediaLoadingFailed report video experience";
    ReportVideoExperienceToBI();
  }

  UpdateLayoutObject();
}

void HTMLMediaElement::ReportVideoExperienceToBI() {
  if (!is_logger_export_) {
    return;
  }
  if (!video_experience_reporter_) {
    return;
  }
  auto params = html_media_element_utils_.ReportVideoExperienceToBI();
  if (!params) {
    return;
  }
  video_experience_reporter_->ReportVideoExperienceToBI(std::move(params));
}

String HTMLMediaElement::GetMediaPlayerType() const {
  switch (GetLoadType()) {
    case WebMediaPlayer::kLoadTypeMediaSource:
      return String("MediaSource");
    case WebMediaPlayer::kLoadTypeMediaStream:
      return String("MediaStream");
    case WebMediaPlayer::kLoadTypeURL:
      if (GetWebMediaPlayer()) {
        if (GetWebMediaPlayer()->UsingMediaPlayer()) {
          return String("MediaPlayer");
        } else {
          return String("MediaCodec");
        }
      }
      break;
    default:
      break;
  }
  return g_empty_string;
}

void HTMLMediaElement::ScheduleVideoFreezeEvent() {
  if (html_media_element_utils_.IsFeedsPage()) {
    ScheduleNamedEvent(event_type_names::kVideofreeze);
  }
}

double HTMLMediaElement::freezeTime() {
  if (!html_media_element_utils_.IsFeedsPage()) {
    LOG(INFO) << "HTMLMediaElement::freezeTime is not feedsPage";
    return 0.0;
  }
  html_media_element_utils_.freeze_time_recorder_.StopRecord();
  base::TimeDelta total_freeze_time = html_media_element_utils_.freeze_time_recorder_.GetDuration();
  if (web_media_player_) {
    total_freeze_time += base::Milliseconds(web_media_player_->GetFreezeTime());
  }
  html_media_element_utils_.freeze_time_recorder_.Reset();
  return total_freeze_time.InMillisecondsF();
}

double HTMLMediaElement::playedTime() {
  if (!html_media_element_utils_.IsFeedsPage()) {
    LOG(INFO) << "HTMLMediaElement::playedTime is not feedsPage";
    return 0.0;
  }
  html_media_element_utils_.played_time_recorder_.StopRecord();
  double total_played_time = html_media_element_utils_.played_time_recorder_.GetDuration().InMillisecondsF();
  html_media_element_utils_.played_time_recorder_.Reset();
  return total_played_time;
}

#endif // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
void HTMLMediaElement::OnMediaCastEnter() {
  LOG(INFO) << "HTMLMediaElement::OnMediaCastEnter";
  html_media_element_utils_.OnMediaCastEnter();
}

void HTMLMediaElement::PullUpCastBackGround(const String& device_name) {
  LOG(INFO) << "HTMLMediaElement::PullUpCastBackGround";
  if (auto* video_element = DynamicTo<HTMLVideoElement>(this)) {
    video_element->MediaRemotingStarted(WebString(device_name));
    for (auto& observer : media_player_observer_remote_set_->Value()) {
      observer->SetPauseByAvcast(true);
    }
  }
}

void HTMLMediaElement::NotifyRemoteExitFullScreen() {
  LOG(INFO) << "HTMLMediaElement::NotifyRemoteExitFullScreen";
  if (auto* video_element = DynamicTo<HTMLVideoElement>(this)) {
    video_element->UpdateRemoteFullScreenCss();
  }
}

void HTMLMediaElement::MediaCastStopByNavigation() {
  MediaCastStopped();
}

void HTMLMediaElement::MediaCastStopped() {
  LOG(INFO) << "HTMLMediaElement::MediaRemotingStopped";
  if (auto* video_element = DynamicTo<HTMLVideoElement>(this)) {
    video_element->MediaRemotingStopped(MediaPlayerClient::kMediaRemotingStopNoText);
    for (auto& observer : media_player_observer_remote_set_->Value()) {
      observer->SetPauseByAvcast(false);
    }
  }
}

void HTMLMediaElement::HandleStopMediaCast() {
  LOG(INFO) << "HTMLMediaElement::HandleStopMediaCast";
  for (auto& observer : media_player_observer_remote_set_->Value()) {
    observer->HandleStopMediaCast();
  }
}

void HTMLMediaElement::UpdateUiPlayState(bool is_playing) {
  LOG(INFO) << "HTMLMediaElement::UpdateUiPlayState";
  if (auto* video_element = DynamicTo<HTMLVideoElement>(this)) {
    video_element->UpdateUiPlayState(is_playing);
  }
}

void HTMLMediaElement::UpdateUiPlayPosition(int64_t position) {
  LOG(DEBUG) << "HTMLMediaElement::UpdateUiPlayPosition";
  if (auto* video_element = DynamicTo<HTMLVideoElement>(this)) {
    video_element->UpdateUiPlayPosition(position);
  }
}

bool HTMLMediaElement::EnableMediaCastByUrlProtocol() {
  LOG(INFO) << "HTMLMediaElement::EnableMediaCastByUrlProtocol";
  if (currentSrc().ProtocolIsInHTTPFamily()) {
    LOG(INFO) << "HTMLMediaElement::EnableMediaCastByUrlProtocol, url is http or https";
    return true;
  }
  return false;
}

void HTMLMediaElement::GetMediaCastCurrentTime(GetMediaCastCurrentTimeCallback callback) {
  LOG(INFO) << "HTMLMediaElement::GetMediaCastCurrentTime: " << currentTime();
  std::move(callback).Run(currentTime());
}

void HTMLMediaElement::NotifyCastControlShow(bool is_show) {
  LOG(INFO) << "HTMLMediaElement::NotifyCastControlShow, is_show: " << is_show;
  cast_botton_show_ = is_show;
#if !defined(COMPONENT_BUILD)
  if (GetMediaControls()) {
    LOG(INFO) << "HTMLMediaElement::NotifyCastControlShow go on";
    GetMediaControls()->NotifyCastControlShow();
  }
#endif // COMPONENT_BUILD
}
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)

}  // namespace blink
