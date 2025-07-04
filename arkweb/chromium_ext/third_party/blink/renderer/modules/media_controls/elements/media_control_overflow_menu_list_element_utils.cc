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

#include "arkweb/chromium_ext/third_party/blink/renderer/modules/media_controls/elements/media_control_overflow_menu_list_element_utils.h"

#include "third_party/blink/renderer/modules/media_controls/elements/media_control_overflow_menu_list_element.h"
#include "third_party/blink/renderer/modules/media_controls/media_controls_impl.h"

namespace blink {

MediaControlOverflowMenuListElementUtils::MediaControlOverflowMenuListElementUtils(
    MediaControlOverflowMenuListElement* element) {
  this->element = element;
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void MediaControlOverflowMenuListElementUtils::SetHrHidden(Event& event) {
  Node* target = event.target()->ToNode();
  if (!target || !target->IsElementNode() ||
      !To<Element>(target)->FastHasAttribute(html_names::kAriaLabelAttr)) {
    return;
  }
  if (target->HasPreviousSibling() && target->previousSibling()->IsElementNode()) {
    To<Element>(target->previousSibling())->SetShadowPseudoId(
      AtomicString("-internal-media-controls-overflow-menu-list-hr-hidden"));
  }
}

void MediaControlOverflowMenuListElementUtils::RemoveHrHidden(Event& event) {
  Node* target = event.target()->ToNode();
  if (!target || !target->IsElementNode() ||
      !To<Element>(target)->FastHasAttribute(html_names::kAriaLabelAttr)) {
    return;
  }
  if (target->HasPreviousSibling() && target->previousSibling()->IsElementNode() &&
      To<Element>(target->previousSibling())->ShadowPseudoId().GetString() ==
        "-internal-media-controls-overflow-menu-list-hr-hidden") {
    To<Element>(target->previousSibling())->removeAttribute(html_names::kPseudoAttr);
  }
}

void MediaControlOverflowMenuListElementUtils::DefaultEventHandlerExt(Event& event) {
  if (element && element->GetMediaControls().ShouldShowVideoControlsHM()) {
    if (event.type() == event_type_names::kGestureshowpress) {
      SetHrHidden(event);
    } else if (event.type() == event_type_names::kLostpointercapture) {
      RemoveHrHidden(event);
    }
  }
}
#endif

}  // namespace blink
