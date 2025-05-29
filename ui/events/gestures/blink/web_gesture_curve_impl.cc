// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/gestures/blink/web_gesture_curve_impl.h"

#include <limits.h>

#include <list>
#include <sstream>
#include <tuple>
#include <utility>

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/ui/gl/events/native_scroller_ohos.h"
#include "base/metrics/histogram_macros.h"
#include "build/build_config.h"
#include "ui/base/ui_base_features.h"
#include "ui/events/gestures/fixed_velocity_curve.h"
#include "ui/events/gestures/fling_curve.h"
#include "ui/events/gestures/physics_based_fling_curve.h"
#include "ui/events/mobile_scroller.h"

#if BUILDFLAG(ARKWEB_FLING)
#include "base/system/sys_info.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif  // BUILDFLAG(ARKWEB_FLING)

using blink::WebGestureCurve;

namespace ui {
namespace {

std::unique_ptr<GestureCurve> CreateDefaultPlatformCurve(
    blink::WebGestureDevice device_source,
    const gfx::Vector2dF& initial_velocity,
    bool use_mobile_fling_curve,
    const gfx::Vector2dF& pixels_per_inch,
    const float boost_multiplier,
    const gfx::Size& bounding_size) {
  if (device_source == blink::WebGestureDevice::kSyntheticAutoscroll) {
    return std::make_unique<FixedVelocityCurve>(initial_velocity,
                                                base::TimeTicks());
  }
#if BUILDFLAG(ARKWEB_FLING)
  LOG(DEBUG) << "CreateDefaultPlatformCurve initial_velocity: x="
             << initial_velocity.x() << " y=" << initial_velocity.y();
  bool use_native_fling_curve = false;
#ifdef USE_NATIVE_FLING_CURVE
  if (!OHOS::NWeb::OhosAdapterHelper::GetInstance()
           .GetSystemPropertiesInstance()
           .GetBoolParameter("web.instructionOptimize.enable", 0)) {
    use_native_fling_curve = true;
  }
#endif
  if (use_native_fling_curve && !base::SysInfo::IsLowEndDevice() &&
      (std::abs(initial_velocity.y()) > std::abs(initial_velocity.x()))) {
    LOG(DEBUG) << "WebGestureCurveImpl DUMP_FLING_CURVE initial_velocity: "
               << initial_velocity.y();
    auto scroller = std::make_unique<NativeScrollerOhos>();
    scroller->Fling(0, 0, initial_velocity.x(), initial_velocity.y(), INT_MIN,
                    static_cast<float>(INT_MAX), INT_MIN,
                    static_cast<float>(INT_MAX), base::TimeTicks());
    return std::move(scroller);
  }
#endif  // ARKWEB_FLING

#ifdef USE_MOBILE_FLING_CURVE
  use_mobile_fling_curve = true;
#endif

  if (use_mobile_fling_curve) {
    MobileScroller::Config config;
#ifdef USE_MOBILE_FLING_CURVE
    config.chromecast_optimized = true;
#endif
    auto scroller = std::make_unique<MobileScroller>(config);
    scroller->Fling(0, 0, initial_velocity.x(), initial_velocity.y(), INT_MIN,
                    static_cast<float>(INT_MAX), INT_MIN,
                    static_cast<float>(INT_MAX), base::TimeTicks());
    return std::move(scroller);
  }

  if (base::FeatureList::IsEnabled(features::kExperimentalFlingAnimation)) {
    return std::make_unique<PhysicsBasedFlingCurve>(
        initial_velocity, base::TimeTicks(), pixels_per_inch, boost_multiplier,
        bounding_size);
  }

  return std::make_unique<FlingCurve>(initial_velocity, base::TimeTicks());
}

}  // namespace

// static
std::unique_ptr<WebGestureCurve>
WebGestureCurveImpl::CreateFromDefaultPlatformCurve(
    blink::WebGestureDevice device_source,
    const gfx::Vector2dF& initial_velocity,
    const gfx::Vector2dF& initial_offset,
    bool on_main_thread,
    bool use_mobile_fling_curve,
    const gfx::Vector2dF& pixels_per_inch,
    const float boost_multiplier,
    const gfx::Size& viewport_size) {
  return std::unique_ptr<WebGestureCurve>(new WebGestureCurveImpl(
      CreateDefaultPlatformCurve(device_source, initial_velocity,
                                 use_mobile_fling_curve, pixels_per_inch,
                                 boost_multiplier, viewport_size),
      initial_offset, on_main_thread ? ThreadType::MAIN : ThreadType::IMPL));
}

// static
std::unique_ptr<WebGestureCurve>
WebGestureCurveImpl::CreateFromUICurveForTesting(
    std::unique_ptr<GestureCurve> curve,
    const gfx::Vector2dF& initial_offset) {
  return std::unique_ptr<WebGestureCurve>(new WebGestureCurveImpl(
      std::move(curve), initial_offset, ThreadType::TEST));
}

WebGestureCurveImpl::WebGestureCurveImpl(std::unique_ptr<GestureCurve> curve,
                                         const gfx::Vector2dF& initial_offset,
                                         ThreadType animating_thread_type)
    : curve_(std::move(curve)),
      last_offset_(initial_offset),
      ticks_since_first_animate_(0),
      first_animate_time_(0),
      last_animate_time_(0) {}

WebGestureCurveImpl::~WebGestureCurveImpl() {}

bool WebGestureCurveImpl::Advance(double time,
                                  gfx::Vector2dF& out_current_velocity,
                                  gfx::Vector2dF& out_delta_to_scroll) {
  // If the fling has yet to start, simply return and report true to prevent
  // fling termination.
  if (time <= 0)
    return true;

  if (!first_animate_time_) {
    first_animate_time_ = last_animate_time_ = time;
  } else if (time != last_animate_time_) {
    // Animation can occur multiple times a frame, but with the same timestamp.
    // Suppress recording of such redundant animate calls, avoiding artificially
    // inflated FPS computation.
    last_animate_time_ = time;
    ++ticks_since_first_animate_;
  }

  const base::TimeTicks time_ticks = base::TimeTicks() + base::Seconds(time);
  gfx::Vector2dF offset;
  bool still_active =
      curve_->ComputeScrollOffset(time_ticks, &offset, &out_current_velocity);

#if BUILDFLAG(ARKWEB_FLING)
  // dump curve
  LOG(DEBUG) << "WebGestureCurveImpl::Advance DUMP_FLING_CURVE time = " << time
             << " offset = " << offset.y()
             << " velocity = " << out_current_velocity.y();
#endif  // ARKWEB_FLING

  out_delta_to_scroll = offset - last_offset_;
  last_offset_ = offset;

  return still_active;
}

}  // namespace ui
