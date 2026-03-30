// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/modules/media_controls/elements/media_control_volume_control_container_element.h"

#include "third_party/blink/renderer/core/dom/dom_token_list.h"
#include "third_party/blink/renderer/core/dom/events/event.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_consts.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_elements_helper.h"
#include "third_party/blink/renderer/modules/media_controls/media_controls_impl.h"
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "base/ohos/sys_info_utils_ext.h"
#endif // ARKWEB_VIDEO_ASSISTANT

namespace blink {

MediaControlVolumeControlContainerElement::
    MediaControlVolumeControlContainerElement(MediaControlsImpl& media_controls)
    : MediaControlDivElement(media_controls) {
  SetShadowPseudoId(
      AtomicString("-webkit-media-controls-volume-control-container"));
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
    if (!GetMediaControls().ShouldShowVideoControlsHM() || base::ohos::IsPcDevice()) {
#endif
  MediaControlElementsHelper::CreateDiv(
      AtomicString("-webkit-media-controls-volume-control-hover-background"),
      this);
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
    }
#endif

  CloseContainer();
}

void MediaControlVolumeControlContainerElement::OpenContainer() {
  classList().Remove(AtomicString(kClosedCSSClass));
}

void MediaControlVolumeControlContainerElement::CloseContainer() {
  classList().Add(AtomicString(kClosedCSSClass));
}

void MediaControlVolumeControlContainerElement::DefaultEventHandler(
    Event& event) {
  if (event.type() == event_type_names::kMouseover)
    GetMediaControls().OpenVolumeSliderIfNecessary();

  if (event.type() == event_type_names::kMouseout)
    GetMediaControls().CloseVolumeSliderIfNecessary();
}

}  // namespace blink
