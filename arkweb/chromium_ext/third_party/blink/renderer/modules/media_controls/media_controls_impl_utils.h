// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_MEDIA_CONTROLS_IMPL_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_MEDIA_CONTROLS_IMPL_UTILS_H_

#include "third_party/blink/renderer/core/html/media/html_media_element.h"
#include "third_party/blink/renderer/core/geometry/dom_rect_read_only.h"
#include "third_party/blink/renderer/core/html/html_div_element.h"
#include "third_party/blink/renderer/core/html/media/media_controls.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/timer.h"

#if BUILDFLAG(ARKWEB_MEDIA)
#include "arkweb/chromium_ext/third_party/blink/renderer/modules/media_controls/elements/media_control_entered_fullscreen_panel_element_ext.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/modules/media_controls/elements/media_control_entered_fullscreen_title_display_element_ext.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/modules/media_controls/elements/media_control_scrubbing_panel_element.h"
#endif  // BUILDFLAG(ARKWEB_MEDIA)
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "third_party/blink/renderer/core/html/html_style_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_top_row_panel_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_timeline_row_panel_element.h"
#endif  // ARKWEB_VIDEO_ASSISTANT

namespace blink {

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
constexpr int kDownloadIndexToRowColumns = 7;
constexpr int kPlaybackSpeedIndexToRowColumns = 9;
constexpr int kRowElementsCount = 10;
constexpr int kMediaControlsSizingMediumThresholdVideoAssitant = 600;
constexpr int kMediaControlsSizingLargeThresholdVideoAssitant = 840;
constexpr base::TimeDelta kScrubbingDelay = base::Seconds(1.0);
#endif

const char kScrubbingMessageCSSClass[] = "scrubbing-message";

class MediaControlsImpl;
#if BUILDFLAG(ARKWEB_MEDIA)
class MediaControlEnteredFullscreenPanelElement;
class MediaControlEnteredFullscreenTitleDisplayElement;
class MediaControlScrubbingPanelElement;
#endif  // BUILDFLAG(ARKWEB_MEDIA)
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
class HTMLStyleElement;
class MediaControlTopRowPanelElement;
class MediaControlTimelineRowPanelElement;
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
// The CSS class names for the different device.
const char kMediaControlsDevicePhoneCSSClass[] = "device-phone";
const char kMediaControlsDevicePcCSSClass[] = "device-pc";
#endif  // ARKWEB_VIDEO_ASSISTANT

class MediaControlsImplUtils {
public:
  MediaControlsImpl* impl;
  MediaControlsImplUtils(MediaControlsImpl* mediaControlsImpl, HTMLMediaElement& media_element);
  static bool ShouldShowPlaybackSpeedButtonExt(HTMLMediaElement& media_element);
  void InitializeControlsExt();
  void PopulatePanelExt();
  bool UpdateCSSClassFromStateEnablePlaybackSpeedButton();
  bool UpdateCSSClassFromStateDisablePlaybackSpeedButton();
  void UpdateCSSClassFromStateEnableCurrentTimeDisplay();
  void UpdateCSSClassFromStateDisableCurrentTimeDisplay();
  void MaybeShowExt();
  void HideExt();
  void MakeOpaqueExt();
  void MakeTransparentExt();
  void BeginScrubbingStartTimer();
  void BeginScrubbingStopTimer();
  void UpdateSizingCSSClassExt();
  void OnDurationChangeExt();
  void OnSeekingExt();
  void OnEnteredFullscreenSetIswanted();
  void OnEnteredFullscreenAddStyleElement();
  void OnExitedFullscreenSetIswanted();
  void OnExitedFullscreenRemoveStyleElement();
  void TraceExt(Visitor* visitor);
  void UpdateOverflowMenuWantedExt(
      std::pair<MediaControlElementBase*, bool>(&row_elements)[kRowElementsCount]);
  static void CreateExt(MediaControlsImpl* controls, HTMLMediaElement& media_element);
  bool PopulatePanelExtVideoAssistant();
  void BeginScrubbingExt(bool is_touch_event);
  void ScrubbingTimerFiredExt();

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  bool ShouldShowVideoControlsHM() const;
  void UpdateDeviceCSSClassExt();
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  const MediaControlPlaybackSpeedButtonElement& Playback_Speed_Button() const;
  MediaControlPlaybackSpeedButtonElement& Playback_Speed_Button();
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void PopulatePanelHM();
  void VideoAssistantTrace(Visitor* visitor) const;
  MediaControlsSizingClass GetSizingClassHM();
  void MakeTransparentImmediately();
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void OnPlaybackSpeedRateChanged();
#endif
};

}
#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_MEDIA_CONTROLS_IMPL_UTILS_H_