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

#ifndef UI_TOUCH_SELECTION_TOUCH_HANDLE_EXT_H_
#define UI_TOUCH_SELECTION_TOUCH_HANDLE_EXT_H_

#include "arkweb/build/features/features.h"
#include "ui/touch_selection/touch_handle.h"

namespace ui {
class TouchHandle;

class TouchHandleExt : public TouchHandle {
 public:
  // The drawable will be enabled but invisible until otherwise specified.
  TouchHandleExt(TouchHandleClient* client,
                 TouchHandleOrientation orientation,
                 const gfx::RectF& viewport_rect);

  TouchHandleExt(const TouchHandleExt&) = delete;
  TouchHandleExt& operator=(const TouchHandleExt&) = delete;

  ~TouchHandleExt() override;
  TouchHandleExt* AsTouchHandleExt() override { return this; }

#if BUILDFLAG(ARKWEB_MENU)
  void SetEdge(const gfx::PointF& top, const gfx::PointF& bottom);
  const gfx::PointF& focus_top() const { return focus_top_; }
  bool GetEnabled() const { return enabled_; }
  void ResetPositionAfterDragEnd();
  bool GetVisible() const { return is_visible_; }
  bool SetFocus(const gfx::PointF& top, const gfx::PointF& bottom);
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  const gfx::RectF& viewport() const { return viewport_rect_; }
#endif
};
}  // namespace ui

#endif  // UI_TOUCH_SELECTION_TOUCH_HANDLE_EXT_H_
