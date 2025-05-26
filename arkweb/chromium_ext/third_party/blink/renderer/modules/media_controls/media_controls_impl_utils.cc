// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/auto_reset.h"
#include "third_party/blink/renderer/platform/timer.h"
#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/public/platform/task_type.h"
#include "third_party/blink/public/platform/user_metrics_action.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_mutation_observer_init.h"
#include "third_party/blink/renderer/core/css/css_property_value_set.h"
#include "third_party/blink/renderer/core/dom/element_traversal.h"
#include "third_party/blink/renderer/core/dom/events/event_dispatch_forbidden_scope.h"
#include "third_party/blink/renderer/core/dom/mutation_observer.h"
#include "third_party/blink/renderer/core/dom/mutation_record.h"
#include "third_party/blink/renderer/core/dom/shadow_root.h"
#include "third_party/blink/renderer/core/events/gesture_event.h"
#include "third_party/blink/renderer/core/events/keyboard_event.h"
#include "third_party/blink/renderer/core/events/pointer_event.h"
#include "third_party/blink/renderer/core/events/touch_event.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/frame/web_feature.h"
#include "third_party/blink/renderer/core/fullscreen/fullscreen.h"
#include "third_party/blink/renderer/core/geometry/dom_rect.h"
#include "third_party/blink/renderer/core/html/media/autoplay_policy.h"
#include "third_party/blink/renderer/core/html/media/html_audio_element.h"
#include "third_party/blink/renderer/core/html/media/html_media_element.h"
#include "third_party/blink/renderer/core/html/media/html_media_element_controls_list.h"
#include "third_party/blink/renderer/core/html/media/html_video_element.h"
#include "third_party/blink/renderer/core/html/time_ranges.h"
#include "third_party/blink/renderer/core/html/track/text_track.h"
#include "third_party/blink/renderer/core/html/track/text_track_container.h"
#include "third_party/blink/renderer/core/html/track/text_track_list.h"
#include "third_party/blink/renderer/core/layout/layout_object.h"
#include "third_party/blink/renderer/core/page/spatial_navigation.h"
#include "third_party/blink/renderer/core/resize_observer/resize_observer.h"
#include "third_party/blink/renderer/core/resize_observer/resize_observer_entry.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_animated_arrow_container_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_button_panel_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_cast_button_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_consts.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_current_time_display_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_display_cutout_fullscreen_button_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_download_button_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_elements_helper.h"
#include "third_party/blink/renderer/core/dom/create_element_flags.h"
#if BUILDFLAG(ARKWEB_MEDIA)
#include "arkweb/chromium_ext/third_party/blink/renderer/modules/media_controls/elements/media_control_entered_fullscreen_panel_element_ext.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/modules/media_controls/elements/media_control_entered_fullscreen_title_display_element_ext.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/modules/media_controls/elements/media_control_scrubbing_panel_element.h"
#endif  // BUILDFLAG(ARKWEB_MEDIA)
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_fullscreen_button_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_loading_panel_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_mute_button_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_overflow_menu_button_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_overflow_menu_list_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_overlay_enclosure_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_overlay_play_button_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_panel_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_panel_enclosure_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_picture_in_picture_button_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_play_button_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_playback_speed_button_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_playback_speed_list_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_remaining_time_display_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_scrubbing_message_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_text_track_list_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_timeline_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_toggle_closed_captions_button_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_volume_control_container_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_volume_slider_element.h"
#include "third_party/blink/renderer/modules/media_controls/media_controls_display_cutout_delegate.h"
#include "third_party/blink/renderer/modules/media_controls/media_controls_media_event_listener.h"
#include "third_party/blink/renderer/modules/media_controls/media_controls_orientation_lock_delegate.h"
#include "third_party/blink/renderer/modules/media_controls/media_controls_resource_loader.h"
#include "third_party/blink/renderer/modules/media_controls/media_controls_rotate_to_fullscreen_delegate.h"
#include "third_party/blink/renderer/modules/media_controls/media_controls_shared_helper.h"
#include "third_party/blink/renderer/modules/media_controls/media_controls_text_track_manager.h"
#include "third_party/blink/renderer/modules/remoteplayback/remote_playback.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/instrumentation/use_counter.h"
#include "third_party/blink/renderer/platform/runtime_enabled_features.h"
#include "third_party/blink/renderer/platform/text/platform_locale.h"
#include "third_party/blink/renderer/platform/web_test_support.h"
#include "ui/gfx/geometry/size.h"

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "third_party/blink/renderer/core/html/html_style_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_top_row_panel_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_timeline_row_panel_element.h"
#endif  // ARKWEB_VIDEO_ASSISTANT

#include "third_party/blink/renderer/modules/media_controls/media_controls_impl.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/modules/media_controls/media_controls_impl_utils.h"

namespace blink {

void MaybeParserAppendChild(Element* parent, Element* child) {
  DCHECK(parent);
  if (child)
    parent->ParserAppendChild(child);
}

bool ShouldShowPlaybackSpeedButton(HTMLMediaElement& media_element) {
  // The page disabled the button via the controlsList attribute.
  if (media_element.ControlsListInternal()->ShouldHidePlaybackRate() &&
      !media_element.UserWantsControlsVisible()) {
    UseCounter::Count(media_element.GetDocument(),
                      WebFeature::kHTMLMediaElementControlsListNoPlaybackRate);
    return false;
  }

  // A MediaStream is not seekable.
  if (media_element.GetLoadType() == WebMediaPlayer::kLoadTypeMediaStream) {
    return false;
  }

  // Don't allow for live infinite streams.
  if (media_element.duration() == std::numeric_limits<double>::infinity() &&
      media_element.getReadyState() > HTMLMediaElement::kHaveNothing) {
    return false;
  }

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (!MediaControlsImplUtils::ShouldShowPlaybackSpeedButtonExt(media_element)) {
    return false;
  }
#endif // ARKWEB_VIDEO_ASSISTANT

  return true;
}

MediaControlsImplUtils::MediaControlsImplUtils(MediaControlsImpl* mediaControlsImpl,
    HTMLMediaElement& media_element) {
  this->impl = mediaControlsImpl;
#if BUILDFLAG(ARKWEB_MEDIA)
  impl->scrubbing_panel_ = nullptr;
  impl->entered_fullscreen_panel_ = nullptr;
  impl->entered_fullscreen_title_display_ = nullptr;
#endif
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM()) {
    impl->top_row_panel_ = nullptr;
    impl->timeline_row_panel_ = nullptr;
  }
#endif
}

bool MediaControlsImplUtils::ShouldShowPlaybackSpeedButtonExt(HTMLMediaElement& media_element) {
  if (media_element.IsCustomMediaPlayerEnabled()) {
    if (media_element.duration() == 0 &&
      media_element.getReadyState() > HTMLMediaElement::kHaveNothing) {
      return false;
    }
  }
  return true;
}

void MediaControlsImplUtils::UpdateOverflowMenuWantedExt(
    std::pair<MediaControlElementBase*, bool>(&row_elements)[kRowElementsCount]) {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM()) {
    row_elements[kDownloadIndexToRowColumns].second = true;
    row_elements[kPlaybackSpeedIndexToRowColumns].second = true;
  }
#endif
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
bool MediaControlsImplUtils::ShouldShowVideoControlsHM() const {
  return impl->MediaElement().IsCustomMediaPlayerEnabled() && impl->ShouldShowVideoControls();
}
#endif

void MediaControlsImplUtils::InitializeControlsExt() {
#if BUILDFLAG(ARKWEB_MEDIA)
  impl->entered_fullscreen_panel_ =
      MakeGarbageCollected<MediaControlEnteredFullscreenPanelElement>(*impl);
  impl->entered_fullscreen_panel_->setInnerHTML("");
  impl->ParserAppendChild(impl->entered_fullscreen_panel_);

  impl->entered_fullscreen_panel_->SetIsWanted(false);

  impl->entered_fullscreen_title_display_ =
      MakeGarbageCollected<MediaControlEnteredFullscreenTitleDisplayElement>(
          *impl);
#endif  // BUILDFLAG(ARKWEB_MEDIA)

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM()) {
    impl->top_row_panel_ = MakeGarbageCollected<MediaControlTopRowPanelElement>(*impl);
    impl->top_row_panel_->setInnerHTML("");
    impl->timeline_row_panel_ = MakeGarbageCollected<MediaControlTimelineRowPanelElement>(*impl);
    impl->timeline_row_panel_->setInnerHTML("");
  }
#endif // ARKWEB_VIDEO_ASSISTANT
}

void MediaControlsImplUtils::PopulatePanelExt() {
  if (impl->scrubbing_panel_) {
    MaybeParserAppendChild(impl->panel_, impl->scrubbing_panel_);
    MaybeParserAppendChild(impl->scrubbing_panel_, impl->scrubbing_message_);
  } else {
    MaybeParserAppendChild(impl->panel_, impl->scrubbing_message_);
  }
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void MediaControlsImplUtils::PopulatePanelHM() {
  impl->panel_->setInnerHTML("");

  MaybeParserAppendChild(impl->volume_control_container_, impl->volume_slider_);
  impl->volume_control_container_->ParserAppendChild(impl->mute_button_);
  impl->AttachHoverBackground(impl->mute_button_);

  impl->download_button_->SetIsWanted(impl->download_button_->ShouldDisplayDownloadButton());
  impl->AttachHoverBackground(impl->download_button_);

  // top row panel
  if (impl->top_row_panel_) {
    impl->top_row_panel_->setInnerHTML("");
    MediaControlElementsHelper::CreateDiv(
        AtomicString("-internal-media-controls-button-spacer"), impl->top_row_panel_);
    impl->top_row_panel_->ParserAppendChild(impl->volume_control_container_);
    impl->top_row_panel_->ParserAppendChild(impl->download_button_);
    impl->panel_->ParserAppendChild(impl->top_row_panel_);
  }

  if (impl->media_button_panel_) {
    impl->media_button_panel_->setInnerHTML("");
  }
 
  // second panel
  Element* button_panel = impl->panel_;
  MaybeParserAppendChild(impl->panel_, impl->scrubbing_panel_);
  MaybeParserAppendChild(impl->scrubbing_panel_, impl->scrubbing_message_);
  if (impl->display_cutout_fullscreen_button_) {
    impl->panel_->ParserAppendChild(impl->display_cutout_fullscreen_button_);
  }
 
  MaybeParserAppendChild(impl->panel_, impl->overlay_play_button_);
  impl->panel_->ParserAppendChild(impl->media_button_panel_);
  button_panel = impl->media_button_panel_;
  button_panel->ParserAppendChild(impl->play_button_);
 
  if (impl->timeline_row_panel_) {
    impl->timeline_row_panel_->setInnerHTML("");
    impl->timeline_row_panel_->ParserAppendChild(impl->current_time_display_);
    impl->timeline_row_panel_->ParserAppendChild(impl->timeline_);
    impl->timeline_row_panel_->ParserAppendChild(impl->duration_display_);
    button_panel->ParserAppendChild(impl->timeline_row_panel_);
  }
 
  button_panel->ParserAppendChild(impl->playback_speed_button_);
  impl->playback_speed_button_->SetIsWanted(ShouldShowPlaybackSpeedButton(impl->MediaElement()));
  impl->AttachHoverBackground(impl->play_button_);
  impl->AttachHoverBackground(impl->playback_speed_button_);
  button_panel->ParserAppendChild(impl->fullscreen_button_);
  impl->AttachHoverBackground(impl->fullscreen_button_);
}
#endif // ARKWEB_VIDEO_ASSISTANT

bool MediaControlsImplUtils::UpdateCSSClassFromStateEnablePlaybackSpeedButton() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM() &&
      !impl->playback_speed_button_->FastHasAttribute(html_names::kDisabledAttr)) {
    impl->playback_speed_button_->setAttribute(html_names::kDisabledAttr, AtomicString(""));
    return true;
  }
#endif
  return false;
}

bool MediaControlsImplUtils::UpdateCSSClassFromStateDisablePlaybackSpeedButton() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM() &&
      impl->playback_speed_button_->FastHasAttribute(html_names::kDisabledAttr)) {
    impl->playback_speed_button_->removeAttribute(html_names::kDisabledAttr);
    return true;
  }
#endif
  return false;
}

void MediaControlsImplUtils::UpdateCSSClassFromStateEnableCurrentTimeDisplay() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM()) {
    impl->current_time_display_->classList().Remove(AtomicString(kDisabledCSSClass));
  }
#endif
}

void MediaControlsImplUtils::UpdateCSSClassFromStateDisableCurrentTimeDisplay() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM()) {
    impl->current_time_display_->classList().Add(AtomicString(kDisabledCSSClass));
  }
#endif
}

void MediaControlsImplUtils::MaybeShowExt() {
#if BUILDFLAG(ARKWEB_MEDIA) && BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (impl->MediaElement().IsFullscreen()) {
    impl->entered_fullscreen_panel_->SetIsWanted(true);
  }
#endif // ARKWEB_MEDIA && ARKWEB_VIDEO_ASSISTANT
}

void MediaControlsImplUtils::HideExt() {
#if BUILDFLAG(ARKWEB_MEDIA)
  if (impl->MediaElement().IsFullscreen()) {
    impl->entered_fullscreen_title_display_->SetIsWanted(false);
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
    impl->entered_fullscreen_panel_->SetIsWanted(false);
#endif // ARKWEB_VIDEO_ASSISTANT
  }
#endif  // BUILDFLAG(ARKWEB_MEDIA)
}

void MediaControlsImplUtils::MakeOpaqueExt() {
#if BUILDFLAG(ARKWEB_MEDIA)
  if (impl->MediaElement().IsFullscreen()) {
    impl->entered_fullscreen_title_display_->SetIsWanted(true);
  }
#endif  // UILDFLAG(ARKWEB_MEDIA)
}

void MediaControlsImplUtils::MakeTransparentExt() {
#if BUILDFLAG(ARKWEB_MEDIA)
  if (impl->MediaElement().IsFullscreen()) {
    impl->entered_fullscreen_title_display_->SetIsWanted(false);
  }
#endif  // BUILDFLAG(ARKWEB_MEDIA)
}

void MediaControlsImplUtils::BeginScrubbingStartTimer() {
  if (impl->scrubbing_timer_.IsActive()) {
    impl->scrubbing_timer_.Stop();
  }
  impl->scrubbing_timer_.StartOneShot(kScrubbingDelay, FROM_HERE);
}

void MediaControlsImplUtils::BeginScrubbingStopTimer() {
  impl->is_begin_scrubbing = true;
  if (impl->scrubbing_timer_.IsActive()) {
    impl->scrubbing_timer_.Stop();
  }
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
MediaControlsSizingClass MediaControlsImplUtils::GetSizingClassHM() {
  if (impl->size_.width() < kMediaControlsSizingMediumThresholdVideoAssitant) {
    return MediaControlsSizingClass::kSmall;
  }
  if (impl->size_.width() < kMediaControlsSizingLargeThresholdVideoAssitant) {
    return MediaControlsSizingClass::kMedium;
  }
  return MediaControlsSizingClass::kLarge;
}

void MediaControlsImplUtils::MakeTransparentImmediately() {
  impl->MakeTransparent();
  impl->panel_->SetIsWanted(false);
}
#endif

void MediaControlsImplUtils::UpdateSizingCSSClassExt() {
  MediaControlsSizingClass sizing_class_hm = GetSizingClassHM();
  impl->SetClass(kMediaControlsSizingSmallCSSClass,
    impl->ShouldShowVideoControls() && sizing_class_hm == MediaControlsSizingClass::kSmall);
  impl->SetClass(kMediaControlsSizingMediumCSSClass,
    impl->ShouldShowVideoControls() && sizing_class_hm == MediaControlsSizingClass::kMedium);
  impl->SetClass(kMediaControlsSizingLargeCSSClass,
    impl->ShouldShowVideoControls() && sizing_class_hm == MediaControlsSizingClass::kLarge);
}

void MediaControlsImplUtils::OnDurationChangeExt() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (impl->MediaElement().IsCustomMediaPlayerEnabled()) {
    impl->playback_speed_button_->SetIsWanted(
        ShouldShowPlaybackSpeedButton(impl->MediaElement()));
  }
#endif // ARKWEB_VIDEO_ASSISTANT
}

void MediaControlsImplUtils::OnSeekingExt() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM()) {
    if (impl->scrubbing_message_ && impl->is_begin_scrubbing) {
      impl->scrubbing_message_->updateScrubbingMsg(true);
      if (impl->scrubbing_message_->DoesFit()) {
        impl->panel_->setAttribute(AtomicString("class"), AtomicString(kScrubbingMessageCSSClass));
      }
    }

    if (impl->scrubbing_panel_ && impl->is_begin_scrubbing) {
      impl->scrubbing_panel_->SetIsWanted(true);
    }
  }
#endif
}

void MediaControlsImplUtils::OnEnteredFullscreenSetIswanted() {
#if BUILDFLAG(ARKWEB_MEDIA)
  impl->entered_fullscreen_panel_->ParserAppendChild(
      impl->entered_fullscreen_title_display_);
  impl->entered_fullscreen_panel_->SetIsWanted(true);
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (!impl->MediaElement().ShouldShowControls()) {
    impl->entered_fullscreen_panel_->SetIsWanted(false);
  }
#endif // ARKWEB_VIDEO_ASSISTANT
  impl->SetClass("fullscreen", true);

  if (!impl->IsVisible()) {
    impl->entered_fullscreen_title_display_->SetIsWanted(false);
  }
#endif  // BUILDFLAG(ARKWEB_MEDIA)
}

void MediaControlsImplUtils::OnEnteredFullscreenAddStyleElement() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (impl->MediaElement().IsVideoAssistantEnabled()) {
    impl->style_element_ = MakeGarbageCollected<HTMLStyleElement>(
        impl->GetDocument(), CreateElementFlags());
    impl->ParserAppendChild(impl->style_element_);
  }
#endif  // ARKWEB_VIDEO_ASSISTANT
}

void MediaControlsImplUtils::OnExitedFullscreenSetIswanted() {
#if BUILDFLAG(ARKWEB_MEDIA)
  impl->entered_fullscreen_panel_->ParserRemoveChild(
      *impl->entered_fullscreen_title_display_);
  impl->entered_fullscreen_panel_->SetIsWanted(false);

  impl->SetClass("fullscreen", false);
#endif  // BUILDFLAG(ARKWEB_MEDIA)
}

void MediaControlsImplUtils::OnExitedFullscreenRemoveStyleElement() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (impl->style_element_) {
    impl->ParserRemoveChild(*impl->style_element_);
  }
#endif  // ARKWEB_VIDEO_ASSISTANT
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
const MediaControlPlaybackSpeedButtonElement& MediaControlsImplUtils::Playback_Speed_Button()
    const {
  return *impl->playback_speed_button_;
}

MediaControlPlaybackSpeedButtonElement& MediaControlsImplUtils::Playback_Speed_Button() {
  return *impl->playback_speed_button_;
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void MediaControlsImplUtils::OnPlaybackSpeedRateChanged() {
  impl->playback_speed_button_->RefreshPlaybackSpeedButton();
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void MediaControlsImplUtils::VideoAssistantTrace(Visitor* visitor) const {
  visitor->Trace(impl->style_element_);
  visitor->Trace(impl->top_row_panel_);
  visitor->Trace(impl->timeline_row_panel_);
}
#endif // ARKWEB_VIDEO_ASSISTANT

void MediaControlsImplUtils::TraceExt(Visitor* visitor) {
#if BUILDFLAG(ARKWEB_MEDIA)
  visitor->Trace(impl->entered_fullscreen_panel_);
  visitor->Trace(impl->entered_fullscreen_title_display_);
  visitor->Trace(impl->scrubbing_panel_);
#endif  // BUILDFLAG(ARKWEB_MEDIA)
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  VideoAssistantTrace(visitor);
  visitor->Trace(impl->scrubbing_timer_);
#endif  // ARKWEB_VIDEO_ASSISTANT
}

void MediaControlsImplUtils::CreateExt(
    MediaControlsImpl* controls, HTMLMediaElement& media_element) {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  controls->SetClass("rtl", media_element.html_media_element_utils_->IsRTL());
#endif
}

bool MediaControlsImplUtils::PopulatePanelExtVideoAssistant() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (impl->mediaControlsImplUtils_->ShouldShowVideoControlsHM()) {
    impl->mediaControlsImplUtils_->PopulatePanelHM();
    return true;
  }
#endif
  return false;
}

void MediaControlsImplUtils::BeginScrubbingExt(bool is_touch_event) {
#if BUILDFLAG(ARKWEB_MEDIA)
  if (impl->scrubbing_panel_ && is_touch_event) {
    impl->scrubbing_panel_->SetIsWanted(true);
  }
#endif
}

void MediaControlsImplUtils::ScrubbingTimerFiredExt() {
  if (!impl->MediaElement().isConnected()) {
    return;
  }

  if (impl->is_begin_scrubbing) {
    impl->scrubbing_message_->updateScrubbingMsg(false);
    if (impl->scrubbing_panel_) {
      impl->scrubbing_panel_->SetIsWanted(false);
    }
    impl->is_begin_scrubbing = false;
  }
}

} // namespace blink