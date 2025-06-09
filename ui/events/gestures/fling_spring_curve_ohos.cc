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

#include "ui/events/gestures/fling_spring_curve_ohos.h"

#include <algorithm>
#include <cmath>

#include "base/check_op.h"
#include "base/logging.h"

namespace ui {

constexpr float kSpringAmplitudeRatio = 0.01f;
constexpr float kSpringMinAmplitude = 1e-3f;
constexpr float floatPi = 3.14159265f;

constexpr float kSpringCurveKTouch = 1.5f;
constexpr float kSpringCurveKTouchpad = 1.5f * 1.5f;
constexpr float kSpringCurveVelocityCoefBase = 1.0f;
constexpr float kSpringCurveVelocityCoefTouch =
    kSpringCurveVelocityCoefBase * kSpringCurveKTouch;
constexpr float kSpringCurveVelocityCoefTouchpad =
    kSpringCurveVelocityCoefBase * kSpringCurveKTouchpad;
constexpr float kSpringCurveF = 1.2f;
constexpr float kSpringCurveCoef = -4.2f;
constexpr float kSpringCurveResponse =
    2.0f * floatPi / kSpringCurveCoef / kSpringCurveF;

// final position is zero in spring curve
// start position at time zero is init_offset
// so the element move from init_offset to zero
inline double CalcCurvePosition(float time, float velocity, float init_offset) {
  float natural_angular_velocity = 2 * floatPi / std::fabs(kSpringCurveResponse);
  float coeff_decay = -natural_angular_velocity;
  double coeffScale = velocity + init_offset * natural_angular_velocity;
  // critical-damped
  double decay = exp(coeff_decay * time);
  return (init_offset + coeffScale * time) * decay;
}

// v = ds/dt
// in the begining, v is velocity (v0)
// at the end, v is near zero
// so v is slow down from velocity to zero
inline double CalcCurveVelocity(float time, float velocity, float init_offset) {
  float natural_angular_velocity = 2 * floatPi / std::fabs(kSpringCurveResponse);
  float coeff_decay = -natural_angular_velocity;
  double coeff_scale = velocity + init_offset * natural_angular_velocity;
  // critical-damped
  double decay = exp(coeff_decay * time);
  return decay * coeff_decay * (init_offset + coeff_scale * time) +
         decay * coeff_scale;
}

inline constexpr bool FloatEqual(float x, float y) {
  return std::abs(x - y) <= std::numeric_limits<float>::epsilon();
}
inline constexpr bool DoubleEqual(double x, double y) {
  return std::abs(x - y) <= std::numeric_limits<double>::epsilon();
}

inline float CalcCurveDuration(float velocity, float init_offset) {
  float natural_angular_velocity = 2 * floatPi / std::fabs(kSpringCurveResponse);
  // minimumAmplitudeRatio_ is used for interpointSpring to determine the ending
  // accuracy of spring animation. the smaller the minimumAmplitudeRatio_, the
  // closer it is to the endpoint at the end of the animation, and the longer
  // the animation duration.
  float minimum_amplitude = std::max(
      std::fabs(init_offset) * kSpringAmplitudeRatio, kSpringMinAmplitude);
  // critical damping spring use dampingRatio = 0.999 to esimate duration
  // approximately
  constexpr float damping_ratio = 0.999f;
  double damped_angular_velocity =
      natural_angular_velocity * std::sqrt(1.0f - damping_ratio * damping_ratio);
  if (DoubleEqual(damped_angular_velocity, 0.0)) {
    return 0.0f;
  }
  double temp_coeff_a = 1.0 / (damping_ratio * natural_angular_velocity);
  double temp_coeff_b =
      (velocity + init_offset * damping_ratio * natural_angular_velocity) *
      (1 / damped_angular_velocity);
  double tempCoeffC =
      std::sqrt(init_offset * init_offset + temp_coeff_b * temp_coeff_b);
  if (DoubleEqual(tempCoeffC, 0.0)) {
    return 0.0f;
  }
  return static_cast<float>(std::log(tempCoeffC / minimum_amplitude) *
                            temp_coeff_a);
}

FlingSpringCurveOhos::FlingSpringCurveOhos(
    const gfx::Vector2dF& velocity,
    base::TimeTicks start_timestamp,
    blink::WebGestureDevice device_source) {
  DCHECK(!velocity.IsZero());
  start_timestamp_ = start_timestamp;
  device_source_ = device_source;
  start_velocity_ = velocity;
  if (device_source_ == blink::WebGestureDevice::kTouchpad) {
    start_velocity_.Scale(kSpringCurveVelocityCoefTouchpad);
  } else {
    start_velocity_.Scale(kSpringCurveVelocityCoefTouch);
  }
  GetMaxDistance(start_velocity_, max_distance_);
  EstimateDuration(curve_duration_);
}

FlingSpringCurveOhos::~FlingSpringCurveOhos() {}

void FlingSpringCurveOhos::GetMaxDistance(const gfx::Vector2dF& velocity,
                                          gfx::Vector2dF& distance) {
  // larger velocity make longer distance
  distance.set_x(velocity.x() / (kSpringCurveCoef * kSpringCurveF));
  distance.set_y(velocity.y() / (kSpringCurveCoef * kSpringCurveF));
}

void FlingSpringCurveOhos::GetPositionAtTime(float offset_time,
                                             gfx::Vector2dF& position) {
  if (offset_time < curve_duration_.x()) {
    position.set_x(
        CalcCurvePosition(offset_time, start_velocity_.x(), max_distance_.x()) -
        max_distance_.x());
  } else {
    position.set_x(-1.0f * max_distance_.x());
  }

  if (offset_time < curve_duration_.y()) {
    position.set_y(
        CalcCurvePosition(offset_time, start_velocity_.y(), max_distance_.y()) -
        max_distance_.y());
  } else {
    position.set_y(-1.0f * max_distance_.y());
  }
}

void FlingSpringCurveOhos::GetVelocityAtTime(float offset_time,
                                             gfx::Vector2dF& velocity) {
  if (offset_time < curve_duration_.x()) {
    velocity.set_x(
        CalcCurveVelocity(offset_time, start_velocity_.x(), max_distance_.x()));
  } else {
    velocity.set_x(0.0f);
  }

  if (offset_time < curve_duration_.y()) {
    velocity.set_y(
        CalcCurveVelocity(offset_time, start_velocity_.y(), max_distance_.y()));
  } else {
    velocity.set_y(0.0f);
  }
}

void FlingSpringCurveOhos::EstimateDuration(gfx::Vector2dF& duration) {
  duration.set_x(CalcCurveDuration(start_velocity_.x(), max_distance_.x()));
  duration.set_y(CalcCurveDuration(start_velocity_.y(), max_distance_.y()));
}

bool FlingSpringCurveOhos::ComputeScrollOffset(base::TimeTicks time,
                                               gfx::Vector2dF* offset,
                                               gfx::Vector2dF* velocity) {
  DCHECK(offset);
  DCHECK(velocity);
  base::TimeDelta elapsed_time = time - start_timestamp_;
  if (elapsed_time.is_negative()) {
    *offset = gfx::Vector2dF();
    *velocity = gfx::Vector2dF();
    return true;
  }

  bool still_active = true;
  // calc distance and velocity at offset_time
  double offset_time = elapsed_time.InSecondsF();
  GetPositionAtTime(offset_time, *offset);
  GetVelocityAtTime(offset_time, *velocity);
  if (offset_time >= curve_duration_.x() &&
      offset_time >= curve_duration_.y()) {
    still_active = false;
  }
  return still_active;
}

}  // namespace ui
