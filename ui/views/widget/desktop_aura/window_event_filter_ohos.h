/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UI_VIEWS_WIDGET_DESKTOP_AURA_WINDOW_EVENT_FILTER_OHOS_H_
#define UI_VIEWS_WIDGET_DESKTOP_AURA_WINDOW_EVENT_FILTER_OHOS_H_

#include "base/memory/raw_ptr.h"
#include "ui/base/hit_test.h"
#include "ui/events/event_handler.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/views/views_export.h"

namespace ui {
class LocatedEvent;
class WmMoveResizeHandler;
}  // namespace ui

namespace views {

class DesktopWindowTreeHostPlatform;

class VIEWS_EXPORT WindowEventFilterOhos : public ui::EventHandler {
 public:
  WindowEventFilterOhos(DesktopWindowTreeHostPlatform* desktop_window_tree_host,
                        ui::WmMoveResizeHandler* handler);

  WindowEventFilterOhos(const WindowEventFilterOhos&) = delete;
  WindowEventFilterOhos& operator=(const WindowEventFilterOhos&) = delete;

  ~WindowEventFilterOhos() override;

  void HandleLocatedEventWithHitTest(int hit_test, ui::LocatedEvent* event);

 private:
  bool HandleMouseEventWithHitTest(int hit_test, ui::MouseEvent* event);

  bool HandleTouchEventWithHitTest(int hit_test, ui::TouchEvent* event);

  bool IsWithinDistance(const gfx::Point& press_location, const gfx::Point& move_location);
  // Called when the user clicked the caption area.
  void OnClickedCaption(ui::MouseEvent* event, int previous_click_component);

  void OnTouchedCaption(ui::TouchEvent* event, int previous_click_component);

  // Dispatches a message to the window manager to tell it to act as if a border
  // or titlebar drag occurred with left mouse click. In case of X11, a
  // _NET_WM_MOVERESIZE message is sent.
  void MaybeDispatchHostWindowDragMovement(int hittest,
                                           ui::LocatedEvent* event);

  const raw_ptr<DesktopWindowTreeHostPlatform> desktop_window_tree_host_;

  // A handler, which is used for interactive move/resize events if set and
  // unless MaybeDispatchHostWindowDragMovement is overridden by a derived
  // class.
  raw_ptr<ui::WmMoveResizeHandler> handler_;

  // The non-client component for the target of a MouseEvent. Mouse events can
  // be destructive to the window tree, which can cause the component of a
  // ui::EF_IS_DOUBLE_CLICK event to no longer be the same as that of the
  // initial click. Acting on a double click should only occur for matching
  // components.
  int click_component_ = HTNOWHERE;
  gfx::Point offset_;
  gfx::Rect window_bounds_;
  gfx::Point touch_press_point_;
  bool is_dragging_ = false;
  bool is_touch_move_ = false;
};

}  // namespace views

#endif  // UI_VIEWS_WIDGET_DESKTOP_AURA_WINDOW_EVENT_FILTER_OHOS_H_
