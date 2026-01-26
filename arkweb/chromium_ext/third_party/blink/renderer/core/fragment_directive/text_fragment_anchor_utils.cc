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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/fragment_directive/text_fragment_anchor_utils.h"

#include "base/timer/timer.h"
#include "third_party/blink/renderer/core/fragment_directive/text_fragment_handler.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/settings.h"

namespace blink {
// static
base::TimeDelta TextFragmentAnchorUtils::HighlightFadeDelay() {
  // Fade delay start from scroll start.
  return base::Milliseconds(1000);
}

TextFragmentAnchorUtils::TextFragmentAnchorUtils(
    LocalFrame& frame,
    bool should_scroll,
    TextFragmentAnchor* text_fragment_anchor)
    : frame_(frame),
      text_fragment_anchor_(text_fragment_anchor),
      weak_ptr_factory_(this) {}

void TextFragmentAnchorUtils::StartHighlightFadeTimer() {
  if (!(frame_ &&
        frame_->GetSettings() &&
        frame_->GetSettings()->GetArkwebAgentEnabled())) {
    LOG(ERROR) << "Start highlight fade timer failed, agent not enabled.";
    return;
  }

  if (frame_->GetSettings()->GetAgentNeedHighlight()) {
    frame_->StartHighlightFadeTimer(HighlightFadeDelay());
  } else {
    frame_->StartHighlightFadeTimer(base::Milliseconds(0));
  }
}

void TextFragmentAnchorUtils::Trace(Visitor* visitor) const {
  visitor->Trace(text_fragment_anchor_);
  visitor->Trace(frame_);
}
}  // namespace blink