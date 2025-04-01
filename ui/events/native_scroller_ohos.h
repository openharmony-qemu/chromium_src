#ifndef UI_EVENTS_NATIVE_SCROLLER_OHOS_H_
#define UI_EVENTS_NATIVE_SCROLLER_OHOS_H_

#include "base/time/time.h"
#include "ui/events/events_base_export.h"
#include "ui/events/gesture_curve.h"
#include "ui/gfx/geometry/vector2d.h"
#include "third_party/blink/public/common/input/web_gesture_device.h"

namespace ui {
class EVENTS_BASE_EXPORT NativeScrollerOhos : public GestureCurve {
 public:
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
               blink::WebGestureDevice device_source = blink::mojom::GestureDevice::kTouchscreen);
    
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
    float position_threshold_{0.0};
};
} // namespace ui
#endif