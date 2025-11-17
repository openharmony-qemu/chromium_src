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

#include "fling_booster_utils.h"

#include <cmath>

#include "arkweb/build/features/features.h"
#include "base/strings/string_number_conversions.h"
#include "base/trace_event/trace_event.h"
#if BUILDFLAG(ARKWEB_FLING)
#include "base/logging.h"
#include "base/system/sys_info.h"
#include "base/ohos/sys_info_utils_ext.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#if BUILDFLAG(ARKWEB_PDF)
#include "base/ohos/ltpo/include/sliding_observer.h"
#endif
#endif

namespace ui {
constexpr int kStartVelocityThreshold = 1500;

#if BUILDFLAG(ARKWEB_FLING)
constexpr gfx::Vector2dF kDefaultLimit{std::numeric_limits<float>::max(),
                                       std::numeric_limits<float>::max()};
constexpr float kMaxBoostFlingSpeed = 9000;
#if BUILDFLAG(ARKWEB_PDF)
constexpr float kMaxBoostFlingSpeedPdf = 5000;
#endif
#endif // BUILDFLAG(ARKWEB_FLING)

#if BUILDFLAG(ARKWEB_FLING)
void LimitVelocity(gfx::Vector2dF &velocity, gfx::Vector2dF &max_fling_velocity)
{
  if (base::ohos::IsPcDevice()) {
    if (kDefaultLimit != max_fling_velocity) {
        LOG(INFO) << "Fling Velocity has been limited";
        return;
    }
    float vx = velocity.x();
    float vy = velocity.y();
    if (vx > kMaxBoostFlingSpeed)
      vx = kMaxBoostFlingSpeed;
    else if (vx < -kMaxBoostFlingSpeed)
      vx = -kMaxBoostFlingSpeed;

    if (vy > kMaxBoostFlingSpeed)
      vy = kMaxBoostFlingSpeed;
    else if (vy < -kMaxBoostFlingSpeed)
      vy = -kMaxBoostFlingSpeed;
    velocity.set_x(vx);
    velocity.set_y(vy);
    TRACE_EVENT2("input", "Fling Boosted", "vx", velocity.x(), "vy", velocity.y());
#if BUILDFLAG(ARKWEB_PDF)
  } else {
    bool is_pdf = base::ohos::SlidingObserver::GetInstance().IsPdf();
    if (is_pdf) {
      float vx = velocity.x();
      float vy = velocity.y();
      if (vx > kMaxBoostFlingSpeedPdf) {
        vx = kMaxBoostFlingSpeedPdf;
      } else if (vx < -kMaxBoostFlingSpeedPdf) {
        vx = -kMaxBoostFlingSpeedPdf;
      }
      if (vy > kMaxBoostFlingSpeedPdf) {
        vy = kMaxBoostFlingSpeedPdf;
      } else if (vy < -kMaxBoostFlingSpeedPdf) {
        vy = -kMaxBoostFlingSpeedPdf;
      }
      velocity.set_x(vx);
      velocity.set_y(vy);
    }
#endif
  }
}

void ScaleVelocity(
    const blink::WebGestureEvent& fling_start,
    gfx::Vector2dF& velocity)
{
  if (!OHOS::NWeb::OhosAdapterHelper::GetInstance()
           .GetSystemPropertiesInstance()
           .GetBoolParameter("web.instructionOptimize.enable", 0)) {
    if (!base::SysInfo::IsLowEndDevice() &&
        (std::abs(fling_start.data.fling_start.velocity_y) >
         std::abs(fling_start.data.fling_start.velocity_x))) {
      std::string ret =
          OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance().GetScrollVelocityScale();
      double velocityScaleTmp = 0.0;
      base::StringToDouble(ret, &velocityScaleTmp);
      double velocityScale = 1.5f;
      if (std::abs(fling_start.data.fling_start.velocity_y) < kStartVelocityThreshold) {
        velocityScale = 1.2f;
      }
      velocity.Scale(1.0f, NearZero(velocityScaleTmp) ? velocityScale : velocityScaleTmp);
    }
  }
}

void ShouldLimitFlingVelocity(gfx::Vector2dF &velocity,
                              gfx::Vector2dF &max_fling_velocity_) {
  float vx = velocity.x();
  float vy = velocity.y();
  bool is_pdf = base::ohos::SlidingObserver::GetInstance().IsPdf();

  if (is_pdf) {
    return;
  }

  if (std::abs(vx) < max_fling_velocity_.x() &&
      std::abs(vy) < max_fling_velocity_.y()) {
    LOG(DEBUG) << "No need to limit the current speed";
    return;
  }

  if (vx < -max_fling_velocity_.x()) {
    vx = -max_fling_velocity_.x();
  } else if (vx > max_fling_velocity_.x()) {
    vx = max_fling_velocity_.x();
  }

  if (vy < -max_fling_velocity_.y()) {
    vy = -max_fling_velocity_.y();
  } else if (vy > max_fling_velocity_.y()) {
    vy = max_fling_velocity_.y();
  }

  LOG(INFO) << "fling velocity:" << velocity.ToString() << " -> "
            << "[" << vx << " " << vy << "]";
  velocity.set_x(vx);
  velocity.set_y(vy);
  TRACE_EVENT2("input", "Fling Velocity", "vx", velocity.x(), "vy", velocity.y());
}
#endif // BUILDFLAG(ARKWEB_FLING)
}  // namespace ui
