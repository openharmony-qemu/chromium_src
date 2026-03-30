// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_MEDIA_MEDIA_REMOTING_INTERSTITIAL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_MEDIA_MEDIA_REMOTING_INTERSTITIAL_H_

#include "third_party/blink/renderer/core/html/html_div_element.h"
#include "third_party/blink/renderer/platform/timer.h"

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
#include "third_party/blink/renderer/core/dom/events/native_event_listener.h"
#include "third_party/blink/renderer/core/dom/events/event.h"
#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/modules/media_controls/media_controls_impl.h"
#include "third_party/blink/renderer/core/svg/svg_svg_element.h"
#include "third_party/blink/renderer/core/events/mouse_event.h"
#include "third_party/blink/renderer/core/css/css_style_declaration.h"
#include "third_party/blink/renderer/core/css/css_property_names.h"
#include "third_party/blink/renderer/core/css/css_value.h"
#include "third_party/blink/renderer/core/css/css_primitive_value.h"
#include "third_party/blink/renderer/core/geometry/dom_rect.h"
#include "third_party/blink/renderer/core/input/touch.h"
#include "third_party/blink/renderer/core/events/touch_event.h"
#include "third_party/blink/renderer/core/fullscreen/fullscreen.h"
#include "third_party/blink/renderer/core/html/forms/html_button_element.h"
#include "third_party/blink/renderer/core/html/html_span_element.h"
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)

namespace blink {

class HTMLImageElement;
class HTMLVideoElement;
class WebString;
#if BUILDFLAG(ARKWEB_MEDIA_CAST)
class Event;
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)

// Media Remoting UI. DOM structure looks like:
//
// MediaRemotingInterstitial
//     (-internal-media-remoting-interstitial)
// +-HTMLImageElement
// |    (-internal-media-interstitial-background-image)
// \-HTMLDivElement
// |    (-internal-media-remoting-cast-icon)
// \-HTMLDivElement
// |    (-internal-media-interstitial-message)
// |-HTMLDivElement
//      (-internal-media-remoting-toast-message)
class MediaRemotingInterstitial final : public HTMLDivElement {
 public:
  explicit MediaRemotingInterstitial(HTMLVideoElement&);

  // Show Media Remoting interstitial. |remote_device_friendly_name| will be
  // shown in the UI to indicate which device the content is rendered on. An
  // empty name indicates an unknown remote device. A default message will be
  // shown in this case.
  void Show(const WebString& remote_device_friendly_name);

  // Hide Media Remoting interstitial. A text message may be displayed for five
  // seconds according to the IDS string associated with the given |error_code|.
  void Hide(int error_code);

  void OnPosterImageChanged();

  // Query for whether the remoting interstitial is visible.
  bool IsVisible() const { return state_ == kVisible; }

  HTMLVideoElement& GetVideoElement() const { return *video_element_; }

  void Trace(Visitor*) const override;

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
  void OnStopCastingClicked();
  void OnSwitchDeviceClicked();
  void OnPlayPauseClicked();
  void AddMediaCastBackGround();
  void ProgressBarAddEvent();
  void DurationAndFullScreenAddEvent();

  // Progress Bar Related Methods
  void OnProgressBarClicked(double percentage);
  void OnProgressDragStart();
  void OnProgressDragging(double percentage);
  void OnProgressDragEnd(double percentage);
  void OnProgressDragCancel();
  // about full screen
  void OnFullscreenClicked();

  // Remote status and playback position synchronized to the UI
  void UpdateUiPlayState(bool is_playing);
  void UpdateUiPlayPosition(int64_t position);
  
  // Ui synchronizes remote playback status and position
  void UpdateRemotePlayState(bool is_playing);
  void UpdateRemotePlayPosition(int64_t position);
  bool GetPlayingState() {return is_playing_;}
  void NotifyRemoteInterstitial(MediaControlsSizingClass sizing_class);
#if !defined(COMPONENT_BUILD)
  void UpdateRemoteFullScreenCss(bool is_fullscreen);
#endif // COMPONENT_BUILD
  HTMLDivElement& GetMediaRemotingProgressBar() { return *progress_bar_; }
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)

 private:
  // Node override.
  bool IsMediaRemotingInterstitial() const override { return true; }
  void DidMoveToNewDocument(Document&) override;

  void ToggleInterstitialTimerFired(TimerBase*);

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
  void EnterFullscreen();
  void ExitFullscreen();
  String FormatTime(double time);
  void UpdateProgressUI();
  void ParseMediaCastControl();
  void UpdatePlayButtonUI();
  void InitializeMediaRemotingInterstitial();
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)

  // Indicates whether the interstitial should be visible. It is set/changed
  // when Show()/Hide() is called.
  enum State {
    kHidden,   // The interstitial is currently not showing.
    kVisible,  // The interstitial is currently visible except the toast.
    kToast,    // Only the toast is visible.
  };
  State state_ = kHidden;

  HeapTaskRunnerTimer<MediaRemotingInterstitial> toggle_interstitial_timer_;
  Member<HTMLVideoElement> video_element_;
  Member<HTMLImageElement> background_image_;
  Member<HTMLDivElement> cast_icon_;
  Member<HTMLDivElement> cast_text_message_;
  Member<HTMLDivElement> toast_message_;

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
  Member<HTMLDivElement> play_pause_button_;
  Member<HTMLSpanElement> current_time_display_;
  Member<HTMLSpanElement> duration_display_;
  Member<HTMLDivElement> progress_fill_;
  Member<HTMLDivElement> progress_bar_;
  Member<HTMLDivElement> fullscreen_button_;
  Member<HTMLDivElement> button_container_;
  Member<HTMLDivElement> left_button_;
  Member<HTMLDivElement> right_button_;
  Member<HTMLDivElement> controls_container_;
  Member<HTMLDivElement> left_group_;
  Member<HTMLDivElement> right_group_;
  Member<HTMLDivElement> progress_group_;
  Member<HTMLDivElement> progress_thumb_;
  Member<HTMLDivElement> video_casting_;

  bool is_progress_dragging_ = false;
  double drag_start_time_ = 0.0;
  bool should_resume_after_drag_ = false;
  bool was_playing_before_drag_ = false;
  bool is_playing_ = false;
  double current_time_ = 0.0;
  double duration_ = 0.0;
  double buffer_percentage_ = 0.0;
  bool is_fullscreen_ = false;
  WeakMember<MediaRemotingInterstitial> weak_this_;
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_MEDIA_MEDIA_REMOTING_INTERSTITIAL_H_
