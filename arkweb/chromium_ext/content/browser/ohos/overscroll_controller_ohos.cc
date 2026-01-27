// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on overscroll_controller_android.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "content/browser/ohos/overscroll_controller_ohos.h"

#include "base/command_line.h"
#include "cc/layers/layer.h"
#include "components/viz/common/quads/compositor_frame_metadata.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/common/content_features.h"
#include "content/public/common/content_switches.h"
#include "third_party/blink/public/common/input/web_gesture_event.h"
#include "third_party/blink/public/common/input/web_input_event.h"
#include "ui/events/blink/did_overscroll_params.h"

using ui::DidOverscrollParams;
using ui::OverscrollRefresh;

namespace content {
namespace {

std::unique_ptr<OverscrollRefresh> CreateRefreshEffect(
    ui::OverscrollRefreshHandler* handler) {
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebExPullToRefresh)) {
    return nullptr;
  }

  return std::make_unique<OverscrollRefresh>(
      handler, OverscrollRefresh::kDefaultNavigationEdgeWidth);
}

}  // namespace

OverscrollControllerOHOS::OverscrollControllerOHOS(
    ui::OverscrollRefreshHandler* handler)
    : enabled_(true), refresh_effect_(CreateRefreshEffect(handler)) {}

OverscrollControllerOHOS::~OverscrollControllerOHOS() = default;

bool OverscrollControllerOHOS::WillHandleGestureEvent(
    const blink::WebGestureEvent& event) {
  if (!enabled_) {
    return false;
  }

  if (event.SourceDevice() != blink::WebGestureDevice::kTouchscreen) {
    return false;
  }

  if (!refresh_effect_) {
    return false;
  }

  bool handled = false;
  switch (event.GetType()) {
    case blink::WebInputEvent::Type::kGestureScrollBegin:
      refresh_effect_->OnScrollBegin(event.PositionInWidget());
      break;

    case blink::WebInputEvent::Type::kGestureScrollUpdate: {
      gfx::Vector2dF scroll_delta(event.data.scroll_update.delta_x,
                                  event.data.scroll_update.delta_y);
      handled = refresh_effect_->WillHandleScrollUpdate(scroll_delta);
    } break;

    case blink::WebInputEvent::Type::kGestureScrollEnd:
      refresh_effect_->OnScrollEnd(gfx::Vector2dF());
      break;

    case blink::WebInputEvent::Type::kGestureFlingStart: {
      if (refresh_effect_->IsActive()) {
        gfx::Vector2dF scroll_velocity(event.data.fling_start.velocity_x,
                                       event.data.fling_start.velocity_y);
        refresh_effect_->OnScrollEnd(scroll_velocity);
        // TODO(jdduke): Figure out a cleaner way of suppressing a fling.
        // It's important that the any downstream code sees a scroll-ending
        // event (in this case GestureFlingStart) if it has seen a scroll begin.
        // Thus, we cannot simply consume the fling. Changing the event type to
        // a GestureScrollEnd might work in practice, but could lead to
        // unexpected results. For now, simply truncate the fling velocity, but
        // not to zero as downstream code may not expect a zero-velocity fling.
        blink::WebGestureEvent& modified_event =
            const_cast<blink::WebGestureEvent&>(event);
        modified_event.data.fling_start.velocity_x = .01f;
        modified_event.data.fling_start.velocity_y = .01f;
      }
    } break;

    case blink::WebInputEvent::Type::kGesturePinchBegin:
      refresh_effect_->ReleaseWithoutActivation();
      break;

    default:
      break;
  }

  return handled;
}

void OverscrollControllerOHOS::OnGestureEventAck(
    const blink::WebGestureEvent& event,
    blink::mojom::InputEventResultState ack_result) {
  if (!enabled_) {
    return;
  }

  // The overscroll effect requires an explicit release signal that may not be
  // sent from the renderer compositor.
  if (event.GetType() == blink::WebInputEvent::Type::kGestureScrollEnd ||
      event.GetType() == blink::WebInputEvent::Type::kGestureFlingStart) {
    OnOverscrolled(DidOverscrollParams());
  }

  if (event.GetType() == blink::WebInputEvent::Type::kGestureScrollUpdate &&
      refresh_effect_) {
    // The effect should only be allowed if the scroll events go unconsumed.
    if (refresh_effect_->IsAwaitingScrollUpdateAck() &&
        ack_result == blink::mojom::InputEventResultState::kConsumed) {
      refresh_effect_->Reset();
    }
  }
}

void OverscrollControllerOHOS::OnOverscrolled(
    const DidOverscrollParams& params) {
  if (!enabled_) {
    return;
  }

  if (refresh_effect_) {
    refresh_effect_->OnOverscrolled(params.overscroll_behavior);

    if (refresh_effect_->IsActive() ||
        refresh_effect_->IsAwaitingScrollUpdateAck()) {
      // An active (or potentially active) refresh effect should always pre-empt
      // the passive glow effect.
      return;
    }
  }

  // When use-zoom-for-dsf is enabled, each value of params was already scaled
  // by the device scale factor.
  gfx::Vector2dF accumulated_overscroll = params.accumulated_overscroll;
  gfx::Vector2dF latest_overscroll_delta = params.latest_overscroll_delta;
  gfx::Vector2dF current_fling_velocity = params.current_fling_velocity;

  if (params.overscroll_behavior.x == cc::OverscrollBehavior::Type::kNone) {
    accumulated_overscroll.set_x(0);
    latest_overscroll_delta.set_x(0);
    current_fling_velocity.set_x(0);
  }

  if (params.overscroll_behavior.y == cc::OverscrollBehavior::Type::kNone) {
    accumulated_overscroll.set_y(0);
    latest_overscroll_delta.set_y(0);
    current_fling_velocity.set_y(0);
  }
}

void OverscrollControllerOHOS::OnFrameMetadataUpdated(
    float page_scale_factor,
    float device_scale_factor,
    const gfx::SizeF& scrollable_viewport_size,
    const gfx::SizeF& root_layer_size,
    const gfx::PointF& root_scroll_offset,
    bool root_overflow_y_hidden) {
  if (!refresh_effect_) {
    return;
  }

  // When use-zoom-for-dsf is enabled, frame_metadata.page_scale_factor was
  // already scaled by the device scale factor.
  float scale_factor = page_scale_factor;
  gfx::SizeF viewport_size =
      gfx::ScaleSize(scrollable_viewport_size, scale_factor);
  gfx::PointF content_scroll_offset =
      gfx::ScalePoint(root_scroll_offset, scale_factor);

  if (refresh_effect_) {
    refresh_effect_->OnFrameUpdated(viewport_size, content_scroll_offset,
                                    root_overflow_y_hidden);
  }
}

void OverscrollControllerOHOS::Enable() {
  enabled_ = true;
}

void OverscrollControllerOHOS::Disable() {
  if (!enabled_) {
    return;
  }
  enabled_ = false;
  if (!enabled_) {
    if (refresh_effect_) {
      refresh_effect_->Reset();
    }
  }
}

void OverscrollControllerOHOS::DidStopRefresh() {
  if (enabled_ && refresh_effect_) {
    refresh_effect_->DidStopRefresh();
  }
}

}  // namespace content
