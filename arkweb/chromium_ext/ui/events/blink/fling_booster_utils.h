/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FLING_BOOSTER_UTILS_H
#define FLING_BOOSTER_UTILS_H

#include <cmath>
#include "arkweb/build/features/features.h"
#include "ui/events/blink/fling_booster.h"

namespace ui {

constexpr double Epsilon = 0.001f;

inline bool NearEqual(const double left, const double right)
{
    return (std::abs(left - right) <= Epsilon);
}

inline bool NearZero(const double num)
{
    return NearEqual(num, 0.0);
}

#if BUILDFLAG(ARKWEB_FLING)
void LimitVelocity(gfx::Vector2dF &velocity,
                   gfx::Vector2dF &max_fling_velocity);
void ScaleVelocity(
    const blink::WebGestureEvent& fling_start,
    gfx::Vector2dF& velocity);
void ShouldLimitFlingVelocity(gfx::Vector2dF &velocity,
                              gfx::Vector2dF &max_fling_velocity_);
#endif // BUILDFLAG(ARKWEB_FLING)

}  // namespace ui

#endif  // FLING_BOOSTER_UTILS_H
