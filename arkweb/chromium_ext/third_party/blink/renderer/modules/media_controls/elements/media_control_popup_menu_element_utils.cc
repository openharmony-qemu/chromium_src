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

#include "arkweb/chromium_ext/third_party/blink/renderer/modules/media_controls/elements/media_control_popup_menu_element_utils.h"

#include "third_party/blink/renderer/modules/media_controls/elements/media_control_playback_speed_button_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_popup_menu_element.h"
#include "third_party/blink/renderer/modules/media_controls/media_controls_impl.h"

#include "third_party/blink/renderer/core/css/css_property_value_set.h"
#include "third_party/blink/renderer/core/css/css_style_declaration.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/events/event.h"
#include "third_party/blink/renderer/core/dom/events/native_event_listener.h"
#include "third_party/blink/renderer/core/dom/focus_params.h"
#include "third_party/blink/renderer/core/event_type_names.h"
#include "third_party/blink/renderer/core/events/keyboard_event.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/geometry/dom_rect.h"
#include "third_party/blink/renderer/core/html/html_element.h"
#include "third_party/blink/renderer/core/html/media/html_media_element.h"

namespace blink {

MediaControlPopupMenuElementUtils::MediaControlPopupMenuElementUtils(
    MediaControlPopupMenuElement* element) {
  this->element = element;
}

void MediaControlPopupMenuElementUtils::ShouldSetPopupAnchorHM(
    DOMRect* bounding_client_rect, LocalDOMWindow* dom_window) {
  if (element->GetMediaControls().ShouldShowVideoControlsHM()) {
    SetPopupAnchorHM(bounding_client_rect, dom_window);
  }
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
static constexpr int kPopupMenuMarginPxOhos = 8;
static constexpr int kPopupMenuPaddingPx = 4;
// 48*7+6+4+4+8=358
static constexpr int kPopupMenuBottomSpaceLeft = 358;
// 216+4+4+8=232
static constexpr int kPopupMenuLeftSpaceLeft = 232;
static const char kImportant[] = "important";
static const char kPx[] = "px";

void MediaControlPopupMenuElementUtils::SetPopupAnchorHM(
    DOMRect* bounding_client_rect, LocalDOMWindow* dom_window) {
  element->style()->removeProperty("max-height", ASSERT_NO_EXCEPTION);
  if (kPopupMenuBottomSpaceLeft <= dom_window->innerHeight() -
      bounding_client_rect->bottom() + kPopupMenuMarginPxOhos) {
    WTF::String top_str_value = WTF::String::Number(bounding_client_rect->bottom() - kPopupMenuPaddingPx) + kPx;
    element->style()->setProperty(dom_window, "top", top_str_value, kImportant,
                        ASSERT_NO_EXCEPTION);
  } else {
    if (kPopupMenuBottomSpaceLeft > bounding_client_rect->top() + kPopupMenuPaddingPx) {
      float height = bounding_client_rect->top() - kPopupMenuMarginPxOhos - kPopupMenuPaddingPx;
      height = height > 0 ? height : 0;
      WTF::String height_str_value = WTF::String::Number(height) + kPx;
      element->style()->setProperty(dom_window, "max-height", height_str_value, kImportant,
                          ASSERT_NO_EXCEPTION);
      WTF::String top_str_value = WTF::String::Number(kPopupMenuMarginPxOhos) + kPx;
      element->style()->setProperty(dom_window, "top", top_str_value, kImportant,
                          ASSERT_NO_EXCEPTION);
    } else if (kPopupMenuBottomSpaceLeft <= bounding_client_rect->top() + kPopupMenuPaddingPx) {
      WTF::String top_str_value = WTF::String::Number(bounding_client_rect->top() + kPopupMenuPaddingPx +
                          kPopupMenuMarginPxOhos - kPopupMenuBottomSpaceLeft) + kPx;
      element->style()->setProperty(dom_window, "top", top_str_value, kImportant,
                          ASSERT_NO_EXCEPTION);
    }
  }

  if (kPopupMenuLeftSpaceLeft <= bounding_client_rect->right() &&
      kPopupMenuLeftSpaceLeft > dom_window->innerWidth() - bounding_client_rect->left()) {
      WTF::String left_str_value = WTF::String::Number(bounding_client_rect->right() - kPopupMenuLeftSpaceLeft) + kPx;
      element->style()->setProperty(dom_window, "left", left_str_value, kImportant, ASSERT_NO_EXCEPTION);
  } else if (kPopupMenuLeftSpaceLeft > bounding_client_rect->right() &&
      kPopupMenuLeftSpaceLeft <= dom_window->innerWidth() - bounding_client_rect->left()) {
    WTF::String left_str_value = WTF::String::Number(bounding_client_rect->left()) + kPx;
    element->style()->setProperty(dom_window, "left", left_str_value, kImportant, ASSERT_NO_EXCEPTION);
  } else {
    if (!element->MediaElement().html_media_element_utils_->IsRTL()) {
      WTF::String left_str_value = WTF::String::Number(bounding_client_rect->right() - kPopupMenuLeftSpaceLeft) + kPx;
      element->style()->setProperty(dom_window, "left", left_str_value, kImportant, ASSERT_NO_EXCEPTION);
    } else {
      WTF::String left_str_value = WTF::String::Number(bounding_client_rect->left()) + kPx;
      element->style()->setProperty(dom_window, "left", left_str_value, kImportant, ASSERT_NO_EXCEPTION);
    }
  }
}
#endif

Element* MediaControlPopupMenuElementUtils::ShouldPlaybackSpeedButton() {
  if (element->GetMediaControls().ShouldShowVideoControlsHM()) {
    return &element->GetMediaControls().mediaControlsImplUtils_->Playback_Speed_Button();
  }
}

}  // namespace blink
