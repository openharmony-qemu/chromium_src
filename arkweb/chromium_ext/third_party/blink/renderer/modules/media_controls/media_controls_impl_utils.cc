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
#include "base/ohos/sys_info_utils_ext.h"
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

MediaControlsImplUtils::MediaControlsImplUtils(MediaControlsImpl* media_controls_impl,
    HTMLMediaElement& media_element) {
  InitMediaControlsImplUtils(media_controls_impl, media_element);
}

void MediaControlsImplUtils::InitMediaControlsImplUtils(MediaControlsImpl* media_controls_impl,
    HTMLMediaElement& media_element) {
  if (!media_controls_impl) {
    LOG(ERROR) << "InitMediaControlsImplUtils media_controls_impl is nullptr";
    return;
  }
  this->media_controls_impl_ = media_controls_impl;
#if BUILDFLAG(ARKWEB_MEDIA)
  media_controls_impl_->scrubbing_panel_ = nullptr;
  media_controls_impl_->entered_fullscreen_panel_ = nullptr;
  media_controls_impl_->entered_fullscreen_title_display_ = nullptr;
#endif
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM()) {
    media_controls_impl_->top_row_panel_ = nullptr;
    media_controls_impl_->timeline_row_panel_ = nullptr;
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
    std::pair<MediaControlElementBase*, bool>(&row_elements)[kRowElementsCount]) const {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM()) {
    row_elements[kPlaybackSpeedIndexToRowColumns].second = true;
  }
#endif
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
bool MediaControlsImplUtils::ShouldShowVideoControlsHM() const {
  if (!media_controls_impl_) {
    LOG(ERROR) << "ShouldShowVideoControlsHM media_controls_impl_ is nullptr";
    return false;
  }
  return media_controls_impl_->MediaElement().IsCustomMediaPlayerEnabled() &&
    media_controls_impl_->ShouldShowVideoControls();
}

void MediaControlsImplUtils::UpdateDeviceCSSClassExt() {
  if (media_controls_impl_) {
    media_controls_impl_->SetClass(kMediaControlsDevicePcCSSClass, base::ohos::IsPcDevice());
    media_controls_impl_->SetClass(kMediaControlsDevicePhoneCSSClass, !base::ohos::IsPcDevice());
  }
}
#endif

// LCOV_EXCL_START
void MediaControlsImplUtils::InitializeControlsExt() {
  if (!media_controls_impl_) {
    LOG(ERROR) << "InitializeControlsExt media_controls_impl_ is nullptr";
    return;
  }
#if BUILDFLAG(ARKWEB_MEDIA)
  media_controls_impl_->entered_fullscreen_panel_ =
      MakeGarbageCollected<MediaControlEnteredFullscreenPanelElement>(*media_controls_impl_);
  if (!(media_controls_impl_->entered_fullscreen_panel_)) {
    LOG(ERROR) << "InitializeControlsExt entered_fullscreen_panel_ is nullptr";
    return;
  }
  media_controls_impl_->entered_fullscreen_panel_->setInnerHTML("");
  media_controls_impl_->ParserAppendChild(media_controls_impl_->entered_fullscreen_panel_);

  media_controls_impl_->entered_fullscreen_panel_->SetIsWanted(false);

  media_controls_impl_->entered_fullscreen_title_display_ =
      MakeGarbageCollected<MediaControlEnteredFullscreenTitleDisplayElement>(
          *media_controls_impl_);
#endif  // BUILDFLAG(ARKWEB_MEDIA)

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM()) {
    media_controls_impl_->top_row_panel_ =
      MakeGarbageCollected<MediaControlTopRowPanelElement>(*media_controls_impl_);
    if (!(media_controls_impl_->top_row_panel_)) {
      LOG(ERROR) << "InitializeControlsExt top_row_panel_ is nullptr";
      return;
    }
    media_controls_impl_->top_row_panel_->setInnerHTML("");
    media_controls_impl_->timeline_row_panel_ =
      MakeGarbageCollected<MediaControlTimelineRowPanelElement>(*media_controls_impl_);
    if (!(media_controls_impl_->timeline_row_panel_)) {
      LOG(ERROR) << "InitializeControlsExt timeline_row_panel_ is nullptr";
      return;
    }
    media_controls_impl_->timeline_row_panel_->setInnerHTML("");
  }
#endif // ARKWEB_VIDEO_ASSISTANT
}

void MediaControlsImplUtils::PopulatePanelExt() {
  if (media_controls_impl_ && media_controls_impl_->scrubbing_panel_) {
    MaybeParserAppendChild(media_controls_impl_->panel_, media_controls_impl_->scrubbing_panel_);
    MaybeParserAppendChild(media_controls_impl_->scrubbing_panel_, media_controls_impl_->scrubbing_message_);
  } else {
    MaybeParserAppendChild(media_controls_impl_->panel_, media_controls_impl_->scrubbing_message_);
  }
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void MediaControlsImplUtils::PopulatePanelHM() {
  if (!media_controls_impl_ || !(media_controls_impl_->panel_)) {
    LOG(ERROR) << "PopulatePanelHM media_controls_impl_ is nullptr";
    return;
  }
  media_controls_impl_->panel_->setInnerHTML("");

  MaybeParserAppendChild(media_controls_impl_->volume_control_container_, media_controls_impl_->volume_slider_);
  media_controls_impl_->volume_control_container_->ParserAppendChild(media_controls_impl_->mute_button_);
  media_controls_impl_->AttachHoverBackground(media_controls_impl_->mute_button_);


  if (!(media_controls_impl_->download_button_)) {
    LOG(ERROR) << "PopulatePanelHM download_button_ is nullptr";
    return;
  }
  media_controls_impl_->download_button_->
    SetIsWanted(media_controls_impl_->download_button_->ShouldDisplayDownloadButton());
  media_controls_impl_->AttachHoverBackground(media_controls_impl_->download_button_);

  // top row panel
  if (media_controls_impl_->top_row_panel_) {
    media_controls_impl_->top_row_panel_->setInnerHTML("");
    MediaControlElementsHelper::CreateDiv(
        AtomicString("-internal-media-controls-button-spacer"), media_controls_impl_->top_row_panel_);
    media_controls_impl_->top_row_panel_->ParserAppendChild(media_controls_impl_->volume_control_container_);
    media_controls_impl_->top_row_panel_->ParserAppendChild(media_controls_impl_->overflow_menu_);
    media_controls_impl_->panel_->ParserAppendChild(media_controls_impl_->top_row_panel_);
  }

  if (media_controls_impl_->media_button_panel_) {
    media_controls_impl_->media_button_panel_->setInnerHTML("");
  }

  // second panel
  Element* button_panel = media_controls_impl_->panel_;
  MaybeParserAppendChild(media_controls_impl_->panel_, media_controls_impl_->scrubbing_panel_);
  MaybeParserAppendChild(media_controls_impl_->scrubbing_panel_, media_controls_impl_->scrubbing_message_);
  if (media_controls_impl_->display_cutout_fullscreen_button_) {
    media_controls_impl_->panel_->ParserAppendChild(media_controls_impl_->display_cutout_fullscreen_button_);
  }

  MaybeParserAppendChild(media_controls_impl_->panel_, media_controls_impl_->overlay_play_button_);
  media_controls_impl_->panel_->ParserAppendChild(media_controls_impl_->media_button_panel_);
  button_panel = media_controls_impl_->media_button_panel_;
  button_panel->ParserAppendChild(media_controls_impl_->play_button_);

  if (media_controls_impl_->timeline_row_panel_) {
    media_controls_impl_->timeline_row_panel_->setInnerHTML("");
    media_controls_impl_->timeline_row_panel_->ParserAppendChild(media_controls_impl_->current_time_display_);
    media_controls_impl_->timeline_row_panel_->ParserAppendChild(media_controls_impl_->timeline_);
    media_controls_impl_->timeline_row_panel_->ParserAppendChild(media_controls_impl_->duration_display_);
    button_panel->ParserAppendChild(media_controls_impl_->timeline_row_panel_);
  }

  button_panel->ParserAppendChild(media_controls_impl_->playback_speed_button_);
  if (!(media_controls_impl_->playback_speed_button_)) {
    LOG(ERROR) << "PopulatePanelHM playback_speed_button_ is nullptr";
    return;
  }
  media_controls_impl_->playback_speed_button_->
    SetIsWanted(ShouldShowPlaybackSpeedButton(media_controls_impl_->MediaElement()));
  media_controls_impl_->AttachHoverBackground(media_controls_impl_->play_button_);
  media_controls_impl_->AttachHoverBackground(media_controls_impl_->playback_speed_button_);
  button_panel->ParserAppendChild(media_controls_impl_->fullscreen_button_);
  media_controls_impl_->AttachHoverBackground(media_controls_impl_->fullscreen_button_);
  media_controls_impl_->AttachHoverBackground(media_controls_impl_->overflow_menu_);
}
#endif // ARKWEB_VIDEO_ASSISTANT

bool MediaControlsImplUtils::UpdateCSSClassFromStateEnablePlaybackSpeedButton() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM() && media_controls_impl_ && (media_controls_impl_->playback_speed_button_) &&
      !media_controls_impl_->playback_speed_button_->FastHasAttribute(html_names::kDisabledAttr)) {
    media_controls_impl_->playback_speed_button_->setAttribute(html_names::kDisabledAttr, AtomicString(""));
    return true;
  }
#endif
  return false;
}

bool MediaControlsImplUtils::UpdateCSSClassFromStateDisablePlaybackSpeedButton() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM() && media_controls_impl_ && (media_controls_impl_->playback_speed_button_) &&
      media_controls_impl_->playback_speed_button_->FastHasAttribute(html_names::kDisabledAttr)) {
    media_controls_impl_->playback_speed_button_->removeAttribute(html_names::kDisabledAttr);
    return true;
  }
#endif
  return false;
}

void MediaControlsImplUtils::UpdateCSSClassFromStateEnableCurrentTimeDisplay() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM() && media_controls_impl_ && (media_controls_impl_->current_time_display_)) {
    media_controls_impl_->current_time_display_->classList().Remove(AtomicString(kDisabledCSSClass));
  }
#endif
}

void MediaControlsImplUtils::UpdateCSSClassFromStateDisableCurrentTimeDisplay() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM() && media_controls_impl_ && (media_controls_impl_->current_time_display_)) {
    media_controls_impl_->current_time_display_->classList().Add(AtomicString(kDisabledCSSClass));
  }
#endif
}

void MediaControlsImplUtils::MaybeShowExt() {
#if BUILDFLAG(ARKWEB_MEDIA) && BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (media_controls_impl_ && media_controls_impl_->entered_fullscreen_panel_ &&
      media_controls_impl_->MediaElement().IsFullscreen()) {
    media_controls_impl_->entered_fullscreen_panel_->SetIsWanted(true);
  }
#endif // ARKWEB_MEDIA && ARKWEB_VIDEO_ASSISTANT
}

void MediaControlsImplUtils::HideExt() {
#if BUILDFLAG(ARKWEB_MEDIA)
  if (media_controls_impl_ && media_controls_impl_->entered_fullscreen_title_display_ &&
      media_controls_impl_->entered_fullscreen_panel_ && media_controls_impl_->MediaElement().IsFullscreen()) {
    media_controls_impl_->entered_fullscreen_title_display_->SetIsWanted(false);
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
    media_controls_impl_->entered_fullscreen_panel_->SetIsWanted(false);
#endif // ARKWEB_VIDEO_ASSISTANT
  }
#endif  // BUILDFLAG(ARKWEB_MEDIA)
}

void MediaControlsImplUtils::MakeOpaqueExt() {
#if BUILDFLAG(ARKWEB_MEDIA)
  if (media_controls_impl_ && media_controls_impl_->entered_fullscreen_title_display_ &&
      media_controls_impl_->MediaElement().IsFullscreen()) {
    media_controls_impl_->entered_fullscreen_title_display_->SetIsWanted(true);
  }
#endif  // UILDFLAG(ARKWEB_MEDIA)
}

void MediaControlsImplUtils::MakeTransparentExt() {
#if BUILDFLAG(ARKWEB_MEDIA)
  if (media_controls_impl_ && media_controls_impl_->entered_fullscreen_title_display_ &&
      media_controls_impl_->MediaElement().IsFullscreen()) {
    media_controls_impl_->entered_fullscreen_title_display_->SetIsWanted(false);
  }
#endif  // BUILDFLAG(ARKWEB_MEDIA)
}

void MediaControlsImplUtils::BeginScrubbingStartTimer() {
  if (media_controls_impl_ && media_controls_impl_->scrubbing_timer_.IsActive()) {
    media_controls_impl_->scrubbing_timer_.Stop();
  }
  media_controls_impl_->scrubbing_timer_.StartOneShot(kScrubbingDelay, FROM_HERE);
}

void MediaControlsImplUtils::BeginScrubbingStopTimer() {
  if (!media_controls_impl_) {
    LOG(ERROR) << "BeginScrubbingStopTimer media_controls_impl_ is nullptr";
    return;
  }
  media_controls_impl_->is_begin_scrubbing = true;
  if (media_controls_impl_->scrubbing_timer_.IsActive()) {
    media_controls_impl_->scrubbing_timer_.Stop();
  }
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
MediaControlsSizingClass MediaControlsImplUtils::GetSizingClassHM() {
  if (!media_controls_impl_) {
    return MediaControlsSizingClass::kLarge;
  }
  if (media_controls_impl_->size_.width() < kMediaControlsSizingMediumThresholdVideoAssitant) {
    return MediaControlsSizingClass::kSmall;
  }
  if (media_controls_impl_->size_.width() < kMediaControlsSizingLargeThresholdVideoAssitant) {
    return MediaControlsSizingClass::kMedium;
  }
  return MediaControlsSizingClass::kLarge;
}

void MediaControlsImplUtils::MakeTransparentImmediately() {
  if (!media_controls_impl_) {
    LOG(ERROR) << "MakeTransparentImmediately media_controls_impl_ is nullptr";
    return;
  }
  media_controls_impl_->MakeTransparent();
  media_controls_impl_->panel_->SetIsWanted(false);
}
#endif

void MediaControlsImplUtils::UpdateSizingCSSClassExt() {
  if (!media_controls_impl_) {
    LOG(ERROR) << "UpdateSizingCSSClassExt media_controls_impl_ is nullptr";
    return;
  }
  MediaControlsSizingClass sizing_class_hm = GetSizingClassHM();
  media_controls_impl_->SetClass(kMediaControlsSizingSmallCSSClass,
    media_controls_impl_->ShouldShowVideoControls() && sizing_class_hm == MediaControlsSizingClass::kSmall);
  media_controls_impl_->SetClass(kMediaControlsSizingMediumCSSClass,
    media_controls_impl_->ShouldShowVideoControls() && sizing_class_hm == MediaControlsSizingClass::kMedium);
  media_controls_impl_->SetClass(kMediaControlsSizingLargeCSSClass,
    media_controls_impl_->ShouldShowVideoControls() && sizing_class_hm == MediaControlsSizingClass::kLarge);
}

void MediaControlsImplUtils::OnDurationChangeExt() {
  if (!media_controls_impl_) {
    LOG(ERROR) << "OnDurationChangeExt media_controls_impl_ is nullptr";
    return;
  }
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (media_controls_impl_->MediaElement().IsCustomMediaPlayerEnabled()) {
    media_controls_impl_->playback_speed_button_->SetIsWanted(
        ShouldShowPlaybackSpeedButton(media_controls_impl_->MediaElement()));
  }
#endif // ARKWEB_VIDEO_ASSISTANT
}

void MediaControlsImplUtils::OnSeekingExt() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (ShouldShowVideoControlsHM()) {
    if (media_controls_impl_ && media_controls_impl_->scrubbing_message_ &&
        media_controls_impl_->is_begin_scrubbing) {
      media_controls_impl_->scrubbing_message_->updateScrubbingMsg(true);
      if (media_controls_impl_->scrubbing_message_->DoesFit() && media_controls_impl_->panel_) {
        media_controls_impl_->panel_->setAttribute(AtomicString("class"), AtomicString(kScrubbingMessageCSSClass));
      }
    }

    if (media_controls_impl_->scrubbing_panel_ && media_controls_impl_->is_begin_scrubbing) {
      media_controls_impl_->scrubbing_panel_->SetIsWanted(true);
    }
  }
#endif
}

void MediaControlsImplUtils::OnEnteredFullscreenSetIswanted() {
#if BUILDFLAG(ARKWEB_MEDIA)
  if (media_controls_impl_ && media_controls_impl_->entered_fullscreen_panel_) {
    media_controls_impl_->entered_fullscreen_panel_->ParserAppendChild(
        media_controls_impl_->entered_fullscreen_title_display_);
    media_controls_impl_->entered_fullscreen_panel_->SetIsWanted(true);    
  }
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (!media_controls_impl_->MediaElement().ShouldShowControls()) {
    media_controls_impl_->entered_fullscreen_panel_->SetIsWanted(false);
  }
#endif // ARKWEB_VIDEO_ASSISTANT
  media_controls_impl_->SetClass("fullscreen", true);

  if (!media_controls_impl_->IsVisible() && media_controls_impl_->entered_fullscreen_title_display_) {
    media_controls_impl_->entered_fullscreen_title_display_->SetIsWanted(false);
  }
#endif  // BUILDFLAG(ARKWEB_MEDIA)
}

void MediaControlsImplUtils::OnEnteredFullscreenAddStyleElement() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (media_controls_impl_ && media_controls_impl_->MediaElement().IsVideoAssistantEnabled()) {
    media_controls_impl_->style_element_ = MakeGarbageCollected<HTMLStyleElement>(
        media_controls_impl_->GetDocument(), CreateElementFlags());
    media_controls_impl_->ParserAppendChild(media_controls_impl_->style_element_);
  }
#endif  // ARKWEB_VIDEO_ASSISTANT
}

void MediaControlsImplUtils::OnExitedFullscreenSetIswanted() {
#if BUILDFLAG(ARKWEB_MEDIA)
  if (media_controls_impl_ && media_controls_impl_->entered_fullscreen_panel_) {
    media_controls_impl_->entered_fullscreen_panel_->ParserRemoveChild(
        *media_controls_impl_->entered_fullscreen_title_display_);
    media_controls_impl_->entered_fullscreen_panel_->SetIsWanted(false);

    media_controls_impl_->SetClass("fullscreen", false);
  }
#endif  // BUILDFLAG(ARKWEB_MEDIA)
}

void MediaControlsImplUtils::OnExitedFullscreenRemoveStyleElement() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (media_controls_impl_->style_element_) {
    media_controls_impl_->ParserRemoveChild(*media_controls_impl_->style_element_);
  }
#endif  // ARKWEB_VIDEO_ASSISTANT
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
const MediaControlPlaybackSpeedButtonElement& MediaControlsImplUtils::Playback_Speed_Button()
    const {
  return *media_controls_impl_->playback_speed_button_;
}

MediaControlPlaybackSpeedButtonElement& MediaControlsImplUtils::Playback_Speed_Button() {
  return *media_controls_impl_->playback_speed_button_;
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void MediaControlsImplUtils::OnPlaybackSpeedRateChanged() {
  if (media_controls_impl_ && media_controls_impl_->playback_speed_button_) {
    media_controls_impl_->playback_speed_button_->RefreshPlaybackSpeedButton();
  }
}
#endif
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void MediaControlsImplUtils::VideoAssistantTrace(Visitor* visitor) const {
  visitor->Trace(media_controls_impl_->style_element_);
  visitor->Trace(media_controls_impl_->top_row_panel_);
  visitor->Trace(media_controls_impl_->timeline_row_panel_);
}
#endif // ARKWEB_VIDEO_ASSISTANT

void MediaControlsImplUtils::TraceExt(Visitor* visitor) const {
#if BUILDFLAG(ARKWEB_MEDIA)
  visitor->Trace(media_controls_impl_->entered_fullscreen_panel_);
  visitor->Trace(media_controls_impl_->entered_fullscreen_title_display_);
  visitor->Trace(media_controls_impl_->scrubbing_panel_);
#endif  // BUILDFLAG(ARKWEB_MEDIA)
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  VideoAssistantTrace(visitor);
  visitor->Trace(media_controls_impl_->scrubbing_timer_);
#endif  // ARKWEB_VIDEO_ASSISTANT
}

void MediaControlsImplUtils::CreateExt(
    MediaControlsImpl* controls, HTMLMediaElement& media_element) {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (controls) {
    controls->SetClass("rtl", media_element.html_media_element_utils_.IsRTL());
  }
#endif
}

// LCOV_EXCL_START
bool MediaControlsImplUtils::PopulatePanelExtVideoAssistant() {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (media_controls_impl_ &&
      media_controls_impl_->mediaControlsImplUtils_.ShouldShowVideoControlsHM()) {
    media_controls_impl_->mediaControlsImplUtils_.PopulatePanelHM();
    return true;
  }
#endif
  return false;
}
// LCOV_EXCL_STOP

void MediaControlsImplUtils::BeginScrubbingExt(bool is_touch_event) {
#if BUILDFLAG(ARKWEB_MEDIA)
  if (media_controls_impl_ && media_controls_impl_->scrubbing_panel_ && is_touch_event) {
    media_controls_impl_->scrubbing_panel_->SetIsWanted(true);
  }
#endif
}

// LCOV_EXCL_START
void MediaControlsImplUtils::ScrubbingTimerFiredExt() {
  if (!media_controls_impl_ || !media_controls_impl_->MediaElement().isConnected()) {
    return;
  }

  if (media_controls_impl_->is_begin_scrubbing && media_controls_impl_->scrubbing_message_) {
    media_controls_impl_->scrubbing_message_->updateScrubbingMsg(false);
    if (media_controls_impl_->scrubbing_panel_) {
      media_controls_impl_->scrubbing_panel_->SetIsWanted(false);
    }
    media_controls_impl_->is_begin_scrubbing = false;
  }
}
// LCOV_EXCL_STOP

void MediaControlsImplUtils::Trace(Visitor* visitor) const {
  visitor->Trace(media_controls_impl_);
}

} // namespace blink