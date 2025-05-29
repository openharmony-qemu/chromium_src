// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/modules/media_controls/elements/media_control_entered_fullscreen_title_display_element_ext.h"

#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/dom_token_list.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/html/media/html_media_element.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/modules/media_controls/media_controls_impl.h"
#include "third_party/blink/renderer/platform/wtf/text/unicode.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/html/media/html_media_element_utils.h"

namespace blink {

MediaControlEnteredFullscreenTitleDisplayElement::
    MediaControlEnteredFullscreenTitleDisplayElement(
        MediaControlsImpl& media_controls)
    : MediaControlDivElement(media_controls) {
  SetShadowPseudoId(AtomicString(
      "-internal-media-controls-entered-fullscreen-title-display"));
  title_span_ = MakeGarbageCollected<HTMLDivElement>(GetDocument());
  title_span_->SetShadowPseudoId(
      AtomicString("-internal-media-controls-entered-fullscreen-title-item"));
  this->ParserAppendChild(title_span_);
  UpdateTitle();
}

void MediaControlEnteredFullscreenTitleDisplayElement::UpdateTitle() {
  SetTitle(MediaElement().html_media_element_utils_->GetTitle());
}

void MediaControlEnteredFullscreenTitleDisplayElement::SetTitle(
    const String& title) {
  if (title_ == title) {
    return;
  }
  title_ = title;

  bool isRTL = false;
  for (unsigned i = 0; i < title_.length(); ++i) {
    auto direction = WTF::unicode::Direction(title_[i]);
    if (direction == WTF::unicode::kRightToLeft ||
        direction == WTF::unicode::kRightToLeftArabic ||
        direction == WTF::unicode::kRightToLeftEmbedding ||
        direction == WTF::unicode::kRightToLeftOverride) {
      isRTL = true;
      break;
    } else if (direction == WTF::unicode::kLeftToRight ||
               direction == WTF::unicode::kLeftToRightEmbedding ||
               direction == WTF::unicode::kLeftToRightOverride) {
      isRTL = false;
      break;
    }
  }
#if false
  SetClass("rtl_lang", isRTL);
#endif
  title_span_->setInnerText(title_);
}

void MediaControlEnteredFullscreenTitleDisplayElement::Trace(
    blink::Visitor* visitor) const {
  MediaControlDivElement::Trace(visitor);
  visitor->Trace(title_span_);
}

}  // namespace blink
                     