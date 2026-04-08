/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#ifndef ARKWEB_VISUAL_VIEWPORT_UTILS_H_
#define ARKWEB_VISUAL_VIEWPORT_UTILS_H_
 
#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/frame/frame.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/visual_viewport.h"
namespace blink {
 
class VisualViewport;
 
class ArkWebVisualViewportUtils final
    : public GarbageCollected<ArkWebVisualViewportUtils> {
 public:
  explicit ArkWebVisualViewportUtils(VisualViewport* viewport = nullptr);
  ~ArkWebVisualViewportUtils();
 
  ArkWebVisualViewportUtils(const ArkWebVisualViewportUtils&) = delete;
  ArkWebVisualViewportUtils& operator=(const ArkWebVisualViewportUtils&) =
      delete;
 
  void TriggerUpdateAfterLayoutForNonOverlayScrollbar();
 
  void Trace(Visitor* visitor) const;
 
 private:
  Member<VisualViewport> viewport_;
};
 
}  // namespace blink
 
#endif  // ARKWEB_VISUAL_VIEWPORT_UTILS_H_