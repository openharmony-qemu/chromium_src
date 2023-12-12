#include "ui/events/native_scroller_ohos.h"
#include <cmath>
#include "base/logging.h"
#include "ui/events/gesture_detection/gesture_configuration.h"

namespace ui {
namespace {
constexpr float kDefaultFriction = 0.6f;
constexpr float kFrictionScale = -4.2f;
constexpr float kDefaultThreshold = 0.75f;
constexpr float kDefaultMultiplier = 60.0f;
constexpr float kThresholdForFlingEnd = 1.0f;
constexpr double Epsilon = 0.001f;
constexpr double kFriction = kDefaultFriction * kFrictionScale;

inline bool NearEqual(const double left,
                      const double right) {
    return (std::abs(left - right) <= Epsilon);
}

inline bool NearZero(const double left) {
    return NearEqual(left, 0.0);
}

inline bool GreatNotEqual(double left, double right) {
    return (left - right) > Epsilon;
}
} // namespace

void NativeScrollerOhos::Fling(float start_x,
                               float start_y,
                               float velocity_x,
                               float velocity_y,
                               float min_x,
                               float max_x,
                               float min_y,
                               float max_y,
                               base::TimeTicks start_time) {
    curr_time_ = start_time;
    start_time_ = start_time;
    friction_ = kFriction;

    // currently only vertical fling is supported
    init_velocity_y_ = std::abs(velocity_y);
    init_y_ = start_y;

    float pixel_ratio = ui::GestureConfiguration::GetInstance()->virtual_pixel_ratio();
    value_threshold_ = kDefaultThreshold;
    velocity_threshold_ = NearZero(pixel_ratio) ? (value_threshold_ * kDefaultMultiplier) : ((value_threshold_ * kDefaultMultiplier) / pixel_ratio);

    float diff_time = 0;
    if (NearZero(init_velocity_y_)) {
        signum_ = 0;
        diff_time = 0.0;
    } else {
        signum_ = GreatNotEqual(velocity_y, 0.0) ? 1.0 : -1.0;
        diff_time = std::log(velocity_threshold_ / init_velocity_y_) / friction_;
    }
    final_y_ = GetPosition(diff_time);
    finisheed_ = false;
}

bool NativeScrollerOhos::ComputeScrollOffset(base::TimeTicks time,
                                             gfx::Vector2dF* offset,
                                             gfx::Vector2dF* velocity) {
    if (finisheed_) {
        *offset = gfx::Vector2dF(0, final_y_);
        *velocity = gfx::Vector2dF();
        return false;
    }

    float diff_time = (time - start_time_).InSecondsF();
    last_y_ = curr_y_;
    curr_y_ = GetPosition(diff_time);
    curr_velocity_y_ = GetVelocity(diff_time);

    *offset = gfx::Vector2dF(0, curr_y_);
    *velocity = gfx::Vector2dF(0, curr_velocity_y_);

    if (ShouldAbortAnimation()) {
        AbortAnimation();
    }
    return !finisheed_;
}

bool NativeScrollerOhos::ShouldAbortAnimation() {
    return std::abs(curr_y_ - final_y_) < kThresholdForFlingEnd || 
           std::abs(curr_velocity_y_) <= velocity_threshold_;
}

double NativeScrollerOhos::GetPosition(float offsetTime /*second*/) {
    double position = init_y_ + signum_ * (init_velocity_y_ / friction_) * std::expm1(friction_ * offsetTime);
    return position;
}

double NativeScrollerOhos::GetVelocity(float offsetTime /*second*/) {
    auto velocity = signum_ * init_velocity_y_ * std::exp(friction_ * offsetTime);
    return velocity;
}

void NativeScrollerOhos::AbortAnimation() {
    finisheed_ = true;
}

}