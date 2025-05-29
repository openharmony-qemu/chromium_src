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
  virtual void OnMouseMovement(const gfx::Point& screen_point,
                               int flags,
                               base::TimeTicks event_time) = 0;
  virtual void OnMouseReleased() = 0;
  virtual void OnMoveLoopEnded() = 0;
};
}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_DRAG_OHOS_MOVE_LOOP_DELEGATE_H
