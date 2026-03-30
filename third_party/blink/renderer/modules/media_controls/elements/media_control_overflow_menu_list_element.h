// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_OVERFLOW_MENU_LIST_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_OVERFLOW_MENU_LIST_ELEMENT_H_

#include "third_party/blink/renderer/modules/media_controls/elements/media_control_popup_menu_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_overflow_menu_list_element_utils.h"
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "third_party/blink/renderer/core/html/html_hr_element.h"
#endif

namespace blink {

class Event;
class MediaControlsImpl;

// Holds a list of elements within the overflow menu.
class MediaControlOverflowMenuListElement final
    : public MediaControlPopupMenuElement {
 public:
  explicit MediaControlOverflowMenuListElement(MediaControlsImpl&);

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  friend class MediaControlOverflowMenuListElementUtils;
#endif

  void OpenOverflowMenu();
  void CloseOverflowMenu();

  // Override MediaControlPopupMenuElement
  void SetIsWanted(bool) final;

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void Trace(Visitor*) const override;
  HTMLHRElement* CreateSplitLineItem();
#endif
 private:
  void DefaultEventHandler(Event&) override;
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  Member<HTMLHRElement> split_line_item_;
  MediaControlOverflowMenuListElementUtils elementUtils_;
#endif
};

}  // namespace blink
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "arkweb/chromium_ext/third_party/blink/renderer/modules/media_controls/elements/media_control_overflow_menu_list_element_utils.h"
#endif
#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_OVERFLOW_MENU_LIST_ELEMENT_H_
