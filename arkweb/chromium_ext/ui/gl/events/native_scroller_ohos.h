// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on mobile_scroller.h originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#ifndef UI_EVENTS_NATIVE_SCROLLER_OHOS_H_
#define UI_EVENTS_NATIVE_SCROLLER_OHOS_H_

#include "base/time/time.h"
#include "ui/events/events_base_export.h"
#include "ui/events/gesture_curve.h"
#include "ui/gfx/geometry/vector2d.h"

namespace ui {
class EVENTS_BASE_EXPORT NativeScrollerOhos : public GestureCurve {
 public:
  // same as third_party/blink/public/mojom/input/gesture_event.mojom GestureDevice
  enum class GestureDevice {
    kUninitialized,
    kTouchpad,
    kTouchscreen,
    kSyntheticAutoscroll,
    kScrollbar,
  };
  NativeScrollerOhos() = default;
  ~NativeScrollerOhos() override {}

  // Initialize fling state according to velocity and time at end of scorlling
  void Fling(float start_x,
             float start_y,
             float velocity_x,
             float velocity_y,
             float min_x,
             float max_x,
             float min_y,
             float max_y,
             base::TimeTicks start_time,
             GestureDevice device_source = GestureDevice::kTouchscreen);

  // GestureCurve implementation
  bool ComputeScrollOffset(base::TimeTicks time,
                           gfx::Vector2dF* offset,
                           gfx::Vector2dF* velocity) override;

 private:
  double GetPosition(float offsetTime /*second*/);
  double GetVelocity(float offsetTime /*second*/);

  bool ShouldAbortAnimation();
  void AbortAnimation();

 private:
  bool finisheed_{true};
  base::TimeTicks start_time_;
  base::TimeTicks curr_time_;
  base::TimeTicks final_time_;

  float friction_{0.0};
  float init_velocity_y_{0.0};
  float curr_velocity_y_{0.0};

  float init_y_{0.0};
  float curr_y_{0.0};
  float last_y_{0.0};
  float final_y_{0.0};

  float value_threshold_{0.0};
  float velocity_threshold_{0.0};
  float signum_{0.0};
};
}  // namespace ui
#endif
