// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

  // Called when the user clicked the caption area.
  void OnClickedCaption(ui::MouseEvent* event, int previous_click_component);

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
  bool is_dragging_ = false;
};

}  // namespace views

#endif  // UI_VIEWS_WIDGET_DESKTOP_AURA_WINDOW_EVENT_FILTER_OHOS_H_