// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_DRAG_OHOS_MOVE_LOOP_DELEGATE_H
#define UI_OZONE_PLATFORM_OHOS_DRAG_OHOS_MOVE_LOOP_DELEGATE_H

#include "base/time/time.h"
#include "ui/gfx/geometry/point.h"

namespace ui {

class OhosMoveLoopDelegate {
 public:
  virtual ~OhosMoveLoopDelegate() = default;
  // callback event at the end of moveLoop during tab drag
  virtual void OnMoveLoopEnded() = 0;
  // callback event of the touch move event during tab drag
  virtual void OnTouchMove() = 0;
  // start drag tab using startmoving when touch event
  virtual void OnTabMoveForStartMoving() = 0;
  // callback event when tab dragging ended
  virtual void OnMoveEnd() = 0;
};
}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_DRAG_OHOS_MOVE_LOOP_DELEGATE_H
