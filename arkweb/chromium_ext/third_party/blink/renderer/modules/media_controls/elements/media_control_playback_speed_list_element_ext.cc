// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/metrics/histogram_functions.h"
#include "third_party/blink/public/strings/grit/blink_strings.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_scroll_into_view_options.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_union_boolean_scrollintoviewoptions.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/events/event.h"
#include "third_party/blink/renderer/core/dom/events/event_dispatch_forbidden_scope.h"
#include "third_party/blink/renderer/core/dom/focus_params.h"
#include "third_party/blink/renderer/core/dom/frame_request_callback_collection.h"
#include "third_party/blink/renderer/core/dom/text.h"
#include "third_party/blink/renderer/core/html/forms/html_input_element.h"
#include "third_party/blink/renderer/core/html/forms/html_label_element.h"
#include "third_party/blink/renderer/core/html/html_span_element.h"
#include "third_party/blink/renderer/core/html/media/html_media_element.h"
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "third_party/blink/renderer/core/html/html_hr_element.h"
#include "ui/base/l10n/l10n_util.h"
#include "ohos_nweb_ex/overrides/ui/strings/grit/ohos_ex_ui_strings.h"
#include "third_party/blink/renderer/core/html/media/html_media_element.h"
#endif
#include "third_party/blink/renderer/core/input_type_names.h"
#include "third_party/blink/renderer/core/keywords.h"
#include "third_party/blink/renderer/modules/media_controls/media_controls_impl.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/text/platform_locale.h"
#include "ui/strings/grit/ax_strings.h"

#include "arkweb/chromium_ext/third_party/blink/renderer/modules/media_controls/elements/media_control_playback_speed_list_element_ext.h"

namespace blink {

MediaControlPlaybackSpeedListElementExt::MediaControlPlaybackSpeedListElementExt(
    MediaControlsImpl& media_controls)
    : MediaControlPlaybackSpeedListElement(media_controls) {
  
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void MediaControlPlaybackSpeedListElementExt::SetHrHidden(Event& event) {
  Node* target = event.target()->ToNode();
  if (!target || !target->IsElementNode() ||
      !To<Element>(target)->FastHasAttribute(html_names::kAriaLabelAttr)) {
    return;
  }
  if (target->HasPreviousSibling() && target->previousSibling()->IsElementNode()) {
    To<Element>(target->previousSibling())->SetShadowPseudoId(
      AtomicString("-internal-media-controls-playback-speed-list-hr-hidden"));
  }
  if (target->HasNextSibling() && target->nextSibling()->IsElementNode()) {
    To<Element>(target->nextSibling())->SetShadowPseudoId(
      AtomicString("-internal-media-controls-playback-speed-list-hr-hidden"));
  }
}

void MediaControlPlaybackSpeedListElementExt::RemoveHrHidden(Event& event) {
  Node* target = event.target()->ToNode();
  if (!target || !target->IsElementNode() ||
      !To<Element>(target)->FastHasAttribute(html_names::kAriaLabelAttr)) {
    return;
  }
  if (target->HasPreviousSibling() && target->previousSibling()->IsElementNode() &&
      To<Element>(target->previousSibling())->ShadowPseudoId().GetString() ==
        "-internal-media-controls-playback-speed-list-hr-hidden") {
    To<Element>(target->previousSibling())->removeAttribute(html_names::kPseudoAttr);
  }
  if (target->HasNextSibling() && target->nextSibling()->IsElementNode() &&
      To<Element>(target->nextSibling())->ShadowPseudoId().GetString() ==
        "-internal-media-controls-playback-speed-list-hr-hidden") {
    To<Element>(target->nextSibling())->removeAttribute(html_names::kPseudoAttr);
  }
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
String MediaControlPlaybackSpeedListElementExt::CreatePlaybackSpeedLabelContentHM(const double playback_rate) {
  static constexpr int playback_speed_rate_integerize = 100;
  static constexpr int keep_one_decimal = 10;
  std::string format = l10n_util::GetStringUTF8(
      IDS_HW_MEDIA_VIDEO_PLAYBACK_SPEED_VALUE);
  std::string rate_item_content;
  std::string playback_rate_precised;
  if (0 == int(playback_rate * playback_speed_rate_integerize) % keep_one_decimal) {
    playback_rate_precised = base::StringPrintf("%.1f", playback_rate);
  } else {
    playback_rate_precised = base::StringPrintf("%.2f", playback_rate);
  }
  base::StringAppendVHelper(&rate_item_content, format.c_str(), playback_rate_precised.c_str());
  return String(rate_item_content);
}
#endif

void MediaControlPlaybackSpeedListElementExt::DefaultEventHandlerExt(Event& event) {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (GetMediaControls().ShouldShowVideoControlsHM()) {
    if (event.type() == event_type_names::kGestureshowpress) {
      SetHrHidden(event);
    } else if (event.type() == event_type_names::kLostpointercapture) {
      RemoveHrHidden(event);
    }
  }
#endif
}

}