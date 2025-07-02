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

#ifndef ARKWEB_EXT_THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_POPUP_MENU_ELEMENT_H_
#define ARKWEB_EXT_THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_POPUP_MENU_ELEMENT_H_

#include "third_party/blink/renderer/modules/media_controls/elements/media_control_div_element.h"
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_playback_speed_button_element.h"
#endif
namespace blink {
class MediaControlPopupMenuElement;
class MediaControlsImpl;
class MediaControlPlaybackSpeedButtonElement;

class MediaControlPopupMenuElementUtils {
public:
    MediaControlPopupMenuElement* element;
    MediaControlPopupMenuElementUtils(MediaControlPopupMenuElement* element);

    void ShouldSetPopupAnchorHM(DOMRect* bounding_client_rect, LocalDOMWindow* dom_window);

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
    void SetPopupAnchorHM(DOMRect* bounding_client_rect, LocalDOMWindow* dom_window);
    bool IsOverflowMenuPopup();
#endif

    Element* ShouldPlaybackSpeedButton();
};

}  // namespace blink

#endif  // ARKWEB_EXT_THIRD_PARTY_BLINK_RENDERER_MODULES_MEDIA_CONTROLS_ELEMENTS_MEDIA_CONTROL_POPUP_MENU_ELEMENT_H_
