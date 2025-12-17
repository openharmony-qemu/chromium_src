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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_FRAME_SELECTION_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_FRAME_SELECTION_EXT_H_

#include "third_party/blink/renderer/core/editing/frame_selection.h"

#include "arkweb/build/features/features.h"
#include "ui/gfx/geometry/rect.h"

namespace blink {

class LocalFrame;
class FrameSelection;

class FrameSelectionExt final : public FrameSelection {
 public:
  explicit FrameSelectionExt(LocalFrame&);
  FrameSelectionExt(const FrameSelectionExt&) = delete;
  FrameSelectionExt& operator=(const FrameSelectionExt&) = delete;
  ~FrameSelectionExt();
  FrameSelectionExt* AsFrameSelectionExt() override { return this; }

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  void ScrollRectToVisualIfClosestEdge(const gfx::Point&);
  base::TimeTicks last_autoscroll_time_;
  bool selection_autoscroll_enabled_ = true;
#endif  // ARKWEB_CLIPBOARD
#if BUILDFLAG(ARKWEB_MENU)
  bool is_select_all_ = false;
  void MoveRangeSelection(const gfx::Point& point,
                          bool is_base,
                          TextGranularity);
  bool IsSelectAll() const { return is_select_all_; }
  void SetIsSelectAll(bool is_select_all) { is_select_all_ = is_select_all; }
  void NeedUpdateCursorLocation();
  gfx::Rect ClippedSelectionBoundsInRootFrame() const;
  bool IsPointInSelection(const PhysicalOffset& point);
#endif  // ARKWEB_MENU
#ifdef BUILDFLAG(ARKWEB_DRAG_DROP)
  void InvalidateSelectionForDrag();
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP）
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool is_max_length_overflow_ = false;
  void SetSelectionMarkMaxLengthOverflow() { is_max_length_overflow_ = true; }
  bool GetSelectionMarkMaxLengthOverflow();
#endif
};
}  // namespace blink
#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EDITING_FRAME_SELECTION_EXT_H_
