// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_DESKTOP_WINDOW_MOVE_CLIENT_H_
#define UI_OZONE_PLATFORM_OHOS_DESKTOP_WINDOW_MOVE_CLIENT_H_

#include "base/component_export.h"
#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/size.h"
#include "ui/ozone/platform/ohos/drag/ohos_move_loop_delegate.h"
#include "ui/ozone/platform/ohos/drag/ohos_whole_screen_move_loop.h"

namespace gfx {
class Rect;
}

namespace ui {

// When we're dragging tabs, we need to manually position our window.
class OhosDesktopWindowMoveClient : public OhosMoveLoopDelegate {
 public:
  // Connection point that the window being moved needs to implement.
  class Delegate {
   public:
    // Returns the cursor that was used at the time the move started.
    virtual scoped_refptr<ui::PlatformCursor> GetLastCursor() = 0;
    virtual gfx::Size GetSize() = 0;
    virtual void SetBoundsOnMove(const gfx::Rect& requested_bounds) = 0;

   protected:
    virtual ~Delegate();
  };

  explicit OhosDesktopWindowMoveClient(Delegate* window);
  ~OhosDesktopWindowMoveClient() override;

  // Overridden from OhosMoveLoopDelegate:
  void OnMouseMovement(const gfx::Point& point,
                       int flags,
                       base::TimeTicks event_time) override;
  void OnMouseReleased() override;
  void OnMoveLoopEnded() override;

  bool RunMoveLoop(bool can_grab_pointer, const gfx::Vector2d& drag_offset);
  void EndMoveLoop();

 private:
  OhosWholeScreenMoveLoop move_loop_{this};

  // We need to keep track of this so we can actually move it when reacting to
  // mouse events.
  const raw_ptr<Delegate> window_delegate_;

  // Our cursor offset from the top left window origin when the drag
  // started. Used to calculate the window's new bounds relative to the current
  // location of the cursor.
  gfx::Vector2d window_offset_;
  gfx::Size window_size_;
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_DESKTOP_WINDOW_MOVE_CLIENT_H_
