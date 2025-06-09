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
