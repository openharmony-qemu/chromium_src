// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_GESTURES_FLING_SPRING_CURVE_OHOS_H_
#define UI_EVENTS_GESTURES_FLING_SPRING_CURVE_OHOS_H_

#include "base/time/time.h"
#include "third_party/blink/public/common/input/web_gesture_device.h"
#include "ui/events/events_base_export.h"
#include "ui/events/gesture_curve.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/gfx/geometry/vector2d_f.h"

namespace ui {

// FlingSpringCurveOhos can be used to scroll a UI element suitable
// for touch screen-based flings.
class EVENTS_BASE_EXPORT FlingSpringCurveOhos : public GestureCurve {
 public:
  FlingSpringCurveOhos(const gfx::Vector2dF& velocity,
                       base::TimeTicks start_timestamp,
                       blink::WebGestureDevice device_source_);

  FlingSpringCurveOhos(const FlingSpringCurveOhos&) = delete;
  FlingSpringCurveOhos& operator=(const FlingSpringCurveOhos&) = delete;

  ~FlingSpringCurveOhos() override;

  bool ComputeScrollOffset(base::TimeTicks time,
                           gfx::Vector2dF* offset,
                           gfx::Vector2dF* velocity) override;

  void GetMaxDistance(const gfx::Vector2dF& velocity, gfx::Vector2dF& distance);
  void GetPositionAtTime(float offset_time, gfx::Vector2dF& position);
  void GetVelocityAtTime(float offset_time, gfx::Vector2dF& velocity);
  void EstimateDuration(gfx::Vector2dF& duration);

 private:
  gfx::Vector2dF curve_duration_;
  base::TimeTicks start_timestamp_;
  gfx::Vector2dF start_velocity_;
  gfx::Vector2dF max_distance_;
  blink::WebGestureDevice device_source_;
};

}  // namespace ui

#endif  // UI_EVENTS_GESTURES_FLING_SPRING_CURVE_OHOS_H_