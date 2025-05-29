// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_PLAYBACK_SPEED_LIST_ELEMENT_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_PLAYBACK_SPEED_LIST_ELEMENT_EXT_H_

#include "third_party/blink/renderer/modules/media_controls/elements/media_control_playback_speed_list_element.h"

namespace blink {
class MediaControlPlaybackSpeedListElement;

class MediaControlPlaybackSpeedListElementExt : public MediaControlPlaybackSpeedListElement {
public:
  MediaControlPlaybackSpeedListElementExt(MediaControlsImpl& media_controls);

  blink::MediaControlPlaybackSpeedListElementExt* AsMediaControlPlaybackSpeedListElementExt() override {
      return this;
  }

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void SetHrHidden(Event& event);
  void RemoveHrHidden(Event& event);
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  String CreatePlaybackSpeedLabelContentHM(const double playback_rate);
#endif

void DefaultEventHandlerExt(Event& event) override;

};

}

#endif // THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_PLAYBACK_SPEED_LIST_ELEMENT_EXT_H_