/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ui/ohos/overscroll_refresh.h"

#include "base/check.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/notreached.h"
#include "cc/input/overscroll_behavior.h"
#include "content/public/browser/browser_thread.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/ohos/overscroll_refresh_handler.h"

namespace content {
class RenderWidgetHostView;
}
namespace ui {
namespace {

// Experimentally determined constant used to allow activation even if touch
// release results in a small upward fling (quite common during a slow scroll).
const float kMinFlingVelocityForActivation = -500.f;

// Weighted value used to determine whether a scroll should trigger vertical
// scroll or horizontal navigation.
const float kWeightAngle30 = 1.73f;
const float kPullFrictionCoff = 0.6f;
const float kMinTriggerAnimateTwice = 1;
const int kTriggerRefreshDistance = 64;

const base::TimeDelta kDidStopRefreshingDelayMs = base::Milliseconds(500);
}  // namespace

const base::TimeDelta OverscrollRefresh::kResetDurationMs =
    base::Milliseconds(1250);
const base::TimeDelta OverscrollRefresh::kAnimateDurationMs =
    base::Milliseconds(300);

OverscrollRefresh::OverscrollRefresh(ui::OverscrollRefreshHandler* handler,
                                     float edge_width)
    : scrolled_to_top_(true),
      top_at_scroll_start_(true),
      overflow_y_hidden_(false),
      scroll_consumption_state_(DISABLED),
      edge_width_(edge_width),
      handler_(handler),
      deceleration_animator_(new DecelerationAnimator(2.f, nullptr)) {
  DCHECK(handler);
  deceleration_animator_->setRefreshListener(
      std::make_unique<RefreshListener>(GetWeakPtr()));
}

OverscrollRefresh::OverscrollRefresh()
    : scrolled_to_top_(true),
      overflow_y_hidden_(false),
      scroll_consumption_state_(DISABLED),
      edge_width_(kDefaultNavigationEdgeWidth * 1.f),
      handler_(nullptr),
      deceleration_animator_(nullptr) {}

OverscrollRefresh::~OverscrollRefresh() {
  if (deceleration_animator_) {
    deceleration_animator_->resetAnimate();
  }
}

void OverscrollRefresh::Reset() {
  pulltorefresh_scroll_.set_y(0);
  pulltorefresh_scroll_.set_x(0);
  scroll_consumption_state_ = DISABLED;
  cumulative_scroll_.set_x(0);
  cumulative_scroll_.set_y(0);
  handler_->PullToRefreshAction(ui::PullToRefreshAction::PULL_RESET);
}

void OverscrollRefresh::OnScrollBegin(const gfx::PointF& pos) {
  scroll_begin_x_ = pos.x();
  scroll_begin_y_ = pos.y();
  top_at_scroll_start_ = scrolled_to_top_;
  ReleaseWithoutActivation();
  scroll_consumption_state_ = AWAITING_SCROLL_UPDATE_ACK;
}

void OverscrollRefresh::OnScrollEnd(const gfx::Vector2dF& scroll_velocity) {
  bool allow_activation = scroll_velocity.y() > kMinFlingVelocityForActivation;
  Release(allow_activation);
}

void OverscrollRefresh::OnOverscrolled(const cc::OverscrollBehavior& behavior) {
  if (scroll_consumption_state_ != AWAITING_SCROLL_UPDATE_ACK) {
    return;
  }

  float ydelta = cumulative_scroll_.y();
  float xdelta = cumulative_scroll_.x();
  bool in_y_direction = std::abs(ydelta) > std::abs(xdelta);
  bool in_x_direction = std::abs(ydelta) * kWeightAngle30 < std::abs(xdelta);
  ::OverscrollAction type = ::OverscrollAction::NONE;
  bool navigate_forward = false;
  if (ydelta > 0 && in_y_direction) {
    // Pull-to-refresh. Check overscroll-behavior-y
    if (behavior.y != cc::OverscrollBehavior::Type::kAuto) {
      Reset();
      return;
    }
    type = ::OverscrollAction::PULL_TO_REFRESH;
  } else if (in_x_direction &&
             (scroll_begin_x_ < edge_width_ ||
              viewport_width_ - scroll_begin_x_ < edge_width_)) {
    // Swipe-to-navigate. Check overscroll-behavior-x
    if (behavior.x != cc::OverscrollBehavior::Type::kAuto) {
      Reset();
      return;
    }
    type = ::OverscrollAction::HISTORY_NAVIGATION;
    navigate_forward = xdelta < 0;
  }

  if (type != ::OverscrollAction::NONE) {
    scroll_consumption_state_ =
        handler_->PullToRefreshAction(ui::PullToRefreshAction::PULL_START)
            ? ENABLED
            : DISABLED;
  }
}

bool OverscrollRefresh::WillHandleScrollUpdate(
    const gfx::Vector2dF& scroll_delta) {
  switch (scroll_consumption_state_) {
    case DISABLED:
      return false;

    case AWAITING_SCROLL_UPDATE_ACK:
      // Check applies for the pull-to-refresh condition only.
      if (std::abs(scroll_delta.y()) > std::abs(scroll_delta.x())) {
        // If the initial scroll motion is downward, or we're in other cases
        // where activation shouldn't have happened, stop here.
        if (scroll_delta.y() <= 0 || !top_at_scroll_start_ ||
            overflow_y_hidden_) {
          scroll_consumption_state_ = DISABLED;
          return false;
        }
      }
      cumulative_scroll_.Add(scroll_delta);
      return false;

    case ENABLED:
      gfx::Vector2dF scroll_delta_scale = scroll_delta;
      scroll_delta_scale.Scale(kPullFrictionCoff);
      pulltorefresh_scroll_.Add(scroll_delta_scale);
      handler_->PullToRefreshUpdate(scroll_delta_scale.x(),
                                    scroll_delta_scale.y());
      return true;
  }

  NOTREACHED() << "Invalid overscroll state: " << scroll_consumption_state_;
}

void OverscrollRefresh::ReleaseWithoutActivation() {
  bool allow_activation = false;
  Release(allow_activation);
}

bool OverscrollRefresh::IsActive() const {
  return scroll_consumption_state_ == ENABLED;
}

bool OverscrollRefresh::IsAwaitingScrollUpdateAck() const {
  return scroll_consumption_state_ == AWAITING_SCROLL_UPDATE_ACK;
}

void OverscrollRefresh::OnFrameUpdated(const gfx::SizeF& viewport_size,
                                       const gfx::PointF& content_scroll_offset,
                                       bool root_overflow_y_hidden) {
  viewport_width_ = viewport_size.width();
  scrolled_to_top_ = content_scroll_offset.y() == 0;
  overflow_y_hidden_ = root_overflow_y_hidden;
}

void OverscrollRefresh::Release(bool allow_refresh) {
  if (scroll_consumption_state_ == ENABLED) {
    handler_->PullToRefreshAction(ui::PullToRefreshAction::PULL_RELEASE);
    float scroll_y = pulltorefresh_scroll_.y();
    if (scroll_y > kTriggerRefreshDistance) {
      scroll_y -= kTriggerRefreshDistance;
    } else {
      handler_->PullToRefreshAction(ui::PullToRefreshAction::PULL_CANCEL);
    }
    if (deceleration_animator_) {
      deceleration_animator_->startAnimate(scroll_y, kAnimateDurationMs);
    }
  }
  scroll_consumption_state_ = DISABLED;
  cumulative_scroll_.set_x(0);
  cumulative_scroll_.set_y(0);
}

void OverscrollRefresh::StartResetAnimate() {
  if (pulltorefresh_scroll_.y() > kMinTriggerAnimateTwice) {
    if (deceleration_animator_) {
      deceleration_animator_->startAnimate(pulltorefresh_scroll_.y(),
                                           kAnimateDurationMs);
    }
  }
}

void OverscrollRefresh::AnimateHover(float x_delta, float y_delta) {
  pulltorefresh_scroll_ -= gfx::Vector2dF(x_delta, y_delta);
  handler_->PullToRefreshUpdate(x_delta, -1 * y_delta);
}

void OverscrollRefresh::AnimateReset(float x_delta, float y_delta) {
  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE, base::BindOnce(&OverscrollRefresh::AnimateReset,
                                  base::Unretained(this), x_delta, y_delta));
    return;
  }

  pulltorefresh_scroll_ -= gfx::Vector2dF(x_delta, y_delta);
  if (pulltorefresh_scroll_.y() > kMinTriggerAnimateTwice) {
    did_stop_refresh_ = false;
    handler_->PullToRefreshAction(ui::PullToRefreshAction::PULL_REFRESH);
    reset_timer_.Start(
        FROM_HERE, kResetDurationMs,
        base::BindRepeating(&OverscrollRefresh::StartResetAnimate,
                            base::Unretained(this)));
  } else {
    Reset();
  }
}

void OverscrollRefresh::DidStopRefresh() {
  if (!did_stop_refresh_) {
    reset_timer_.Start(
        FROM_HERE, kDidStopRefreshingDelayMs,
        base::BindRepeating(&OverscrollRefresh::StartResetAnimate,
                            base::Unretained(this)));
  }
  did_stop_refresh_ = true;
}

base::WeakPtr<OverscrollRefresh> OverscrollRefresh::GetWeakPtr() {
  return weak_factory_.GetWeakPtr();
}

OverscrollRefresh::RefreshListener::RefreshListener(
    base::WeakPtr<ui::OverscrollRefresh> overscroll_refresh)
    : overscroll_refresh_(overscroll_refresh) {}

OverscrollRefresh::RefreshListener::RefreshListener(
    const OverscrollRefresh::RefreshListener& other) = default;

OverscrollRefresh::RefreshListener::~RefreshListener() = default;

void OverscrollRefresh::RefreshListener::onAnimationEnd() {
  if (overscroll_refresh_.get()) {
    overscroll_refresh_->AnimateReset(0, 0);
  }
}
void OverscrollRefresh::RefreshListener::onAnimationRepeat(float delta) {
  if (overscroll_refresh_.get()) {
    overscroll_refresh_->AnimateHover(0, delta);
  }
}
}  // namespace ui
