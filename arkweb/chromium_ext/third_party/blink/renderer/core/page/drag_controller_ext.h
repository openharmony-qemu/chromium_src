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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_DRAG_CONTROLLER_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_DRAG_CONTROLLER_EXT_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/execution_context/execution_context_lifecycle_observer.h"
#include "third_party/blink/renderer/core/page/drag_controller.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/heap/member.h"
#include "third_party/blink/renderer/platform/wtf/text/string_builder.h"
#include "ui/gfx/geometry/rect.h"

namespace gfx {
class RectF;
}

namespace blink {

class Page;
class DragState;
class LocalFrame;
class FrameSelection;
class DragController;

class DragControllerExt final : public DragController {
 public:
  explicit DragControllerExt(Page*);
  DragControllerExt(const DragControllerExt&) = delete;
  DragControllerExt& operator=(const DragControllerExt&) = delete;
  ~DragControllerExt();
  DragControllerExt* AsDragControllerExt() override { return this; }

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  bool IsHyperLinkDragging();
  bool DragLinkCheckSrcAndType();
  void UpdateLinkStyle(Node* node);
  void RestoreLinkStyle(Node* node);
  void FindAndRemoveGrayStyle(Element* tempEle);
  void StartDragLinkEffects();
  void RestoreDragLinkEffects();
  void StartDragImageEffects();
  void RestoreDragImageEffects();
  void StartDragTextEffects();
  void RestoreDragTextEffects();
  void InvalidateSelectionForDrag(Document* document);
  bool IsInImageDraging();
  bool IsInTextDraging();
  void SetDragInitState(bool did_initiate_drag) {
    did_initiate_drag_ = did_initiate_drag;
  }
  bool IsDraging() { return is_dragging_; }
  gfx::RectF GetVisibleRectToUIInRootFrame(LocalFrame* frame);
  void SetIsDragging(bool is_dragging) { is_dragging_ = is_dragging; }
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
 private:
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  bool is_dragging_;
  WTF::StringBuilder origin_style_;
  bool did_start_drag_image_effects_;
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
};
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAGE_DRAG_CONTROLLER_EXT_H_
