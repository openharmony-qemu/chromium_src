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

#ifndef UI_OZONE_PLATFORM_OHOS_DESKTOP_WINDOW_MOVE_CLIENT_H_
#define UI_OZONE_PLATFORM_OHOS_DESKTOP_WINDOW_MOVE_CLIENT_H_

#include "base/component_export.h"
#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/gfx/geometry/size.h"
#include "ui/ozone/platform/ohos/drag/ohos_move_loop_delegate.h"
#include "ui/ozone/platform/ohos/drag/ohos_whole_screen_move_loop.h"

namespace gfx {
class Rect;
}

namespace ui {

// When we're dragging tabs, we need to manually position our window.
class OhosDesktopWindowMoveClient
    : public OhosMoveLoopDelegate {
 public:
  // Connection point that the window being moved needs to implement.
  class Delegate {
   public:
    // Returns the cursor that was used at the time the move started.
    virtual scoped_refptr<ui::PlatformCursor> GetLastCursor() = 0;
    virtual gfx::Size GetSize() = 0;
    virtual void SetBoundsOnMove(const gfx::Rect& requested_bounds) = 0;
    virtual void StartWindowMovingWithOffset(const gfx::PointF& offset_point) = 0;

   protected:
    virtual ~Delegate();
  };

  explicit OhosDesktopWindowMoveClient(Delegate* window);
  ~OhosDesktopWindowMoveClient() override;

  // Overridden from OhosMoveLoopDelegate:
  void OnMoveLoopEnded() override;
  void OnMouseMove() override;
  void OnTouchMove() override;
  void OnMoveEnd() override;

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
  bool tab_start_moving_ = false;
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_DESKTOP_WINDOW_MOVE_CLIENT_H_
