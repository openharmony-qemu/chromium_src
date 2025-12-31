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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAGMENT_DIRECTIVE_TEXT_FRAGMENT_ANCHOR_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAGMENT_DIRECTIVE_TEXT_FRAGMENT_ANCHOR_UTILS_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/core/fragment_directive/text_fragment_anchor.h"
#include "third_party/blink/renderer/core/fragment_directive/text_fragment_handler.h"

namespace blink {
class LocalFrame;

class CORE_EXPORT TextFragmentAnchorUtils
    : public GarbageCollected<TextFragmentAnchorUtils> {
  static base::TimeDelta HighlightFadeDelay();

 public:
  TextFragmentAnchorUtils(LocalFrame& frame,
                          bool should_scroll,
                          TextFragmentAnchor* text_fragment_anchor);
  void StartHighlightFadeTimer();
  void Trace(Visitor*) const;

 private:
 Member<LocalFrame> frame_;
 Member<TextFragmentAnchor> text_fragment_anchor_;
 base::WeakPtrFactory<TextFragmentAnchorUtils> weak_ptr_factory_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAGMENT_DIRECTIVE_TEXT_FRAGMENT_ANCHOR_UTILS_H_