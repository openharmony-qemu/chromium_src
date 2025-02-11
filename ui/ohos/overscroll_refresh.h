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


#ifndef UI_OHOS_OVERSCROLL_REFRESH_H_
#define UI_OHOS_OVERSCROLL_REFRESH_H_

#include <memory>

#include "base/memory/raw_ptr.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "deceleration_animator.h"
#include "ui/gfx/geometry/size_f.h"
#include "ui/gfx/geometry/vector2d_f.h"
#include "ui/ohos/ui_ohos_export.h"

// A Java counterpart will be generated for this enum.
// GENERATED_JAVA_ENUM_PACKAGE: org.chromium.ui
enum class OverscrollAction {
  NONE = 0,
  PULL_TO_REFRESH = 1,
  HISTORY_NAVIGATION = 2
};

namespace cc {
struct OverscrollBehavior;
}

namespace gfx {
class PointF;
}

namespace ui {

class OverscrollRefreshHandler;

// Simple pull-to-refresh styled effect. Listens to scroll events, conditionally
// activating when:
//   1) The scroll begins when the page's root layer 1) has no vertical scroll
//      offset and 2) lacks the overflow-y:hidden property.
//   2) The page doesn't consume the initial scroll events.
//   3) The initial scroll direction is upward.
// The actuall pull response, animation and action are delegated to the
// provided refresh handler.
class UI_OHOS_EXPORT OverscrollRefresh {
 public:
  // The default distance in dp from a side of the device to start a navigation
  // from.
  static constexpr int kDefaultNavigationEdgeWidth = 24;

  OverscrollRefresh(ui::OverscrollRefreshHandler* handler, float edge_width);

  OverscrollRefresh(const OverscrollRefresh&) = delete;
  OverscrollRefresh& operator=(const OverscrollRefresh&) = delete;

  virtual ~OverscrollRefresh();

  // Scroll event stream listening methods.
  void OnScrollBegin(const gfx::PointF& pos);
  // Returns whether the refresh was activated.
  void OnScrollEnd(const gfx::Vector2dF& velocity);

  // Scroll ack listener. The effect will only be activated if |can_navigate|
  // is true which happens when the scroll update is not consumed and the
  // overscroll_behavior on y axis is 'auto'.
  // This method is made virtual for mocking.
  virtual void OnOverscrolled(
      const cc::OverscrollBehavior& overscroll_behavior);

  // Returns true if the effect has consumed the |scroll_delta|.
  bool WillHandleScrollUpdate(const gfx::Vector2dF& scroll_delta);

  // Release the effect (if active), preventing any associated refresh action.
  void ReleaseWithoutActivation();

  // Notify the effect of the latest scroll offset and overflow properties.
  // The effect will be disabled when the offset is non-zero or overflow is
  // hidden. Note: All dimensions are in device pixels.
  void OnFrameUpdated(const gfx::SizeF& viewport_size,
                      const gfx::PointF& content_scroll_offset,
                      bool root_overflow_y_hidden);

  // Reset the effect to its inactive state, immediately detaching and
  // disabling any active effects.
  // This method is made virtual for mocking.
  virtual void Reset();

  // Returns true if the refresh effect is either being manipulated or animated.
  // This method is made virtual for mocking.
  virtual bool IsActive() const;

  // Returns true if the effect is waiting for an unconsumed scroll to start.
  // This method is made virtual for mocking.
  virtual bool IsAwaitingScrollUpdateAck() const;

  void DidStopRefresh();

  base::WeakPtr<OverscrollRefresh> GetWeakPtr();

  class RefreshListener : public DecelerationAnimatorListener {
   public:
    RefreshListener(base::WeakPtr<ui::OverscrollRefresh> overscroll_refresh);
    RefreshListener(const RefreshListener& other);
    ~RefreshListener() override;

    void onAnimationEnd() override;
    void onAnimationRepeat(float delta) override;

   private:
    base::WeakPtr<OverscrollRefresh> overscroll_refresh_ = nullptr;
  };

 protected:
  // This constructor is for mocking only.
  OverscrollRefresh();

 private:
  void Release(bool allow_refresh);

  void StartResetAnimate();
  void AnimateHover(float x_delta, float y_delta);
  void AnimateReset(float x_delta, float y_delta);

  bool scrolled_to_top_;
  // True if the content y offset was zero before scroll began. Overscroll
  // should not be triggered for the scroll that started from non-zero offset.
  bool top_at_scroll_start_;
  bool overflow_y_hidden_;

  enum ScrollConsumptionState {
    DISABLED,
    AWAITING_SCROLL_UPDATE_ACK,
    ENABLED,
  } scroll_consumption_state_;

  float viewport_width_;
  float scroll_begin_x_;
  float scroll_begin_y_;
  const float edge_width_;  // in px
  gfx::Vector2dF cumulative_scroll_;
  const raw_ptr<OverscrollRefreshHandler> handler_;

  bool did_stop_refresh_{false};
  gfx::Vector2dF pulltorefresh_scroll_;
  base::RetainingOneShotTimer reset_timer_;
scoped_refptr<DecelerationAnimator> deceleration_animator_;
  const scoped_refptr<base::SequencedTaskRunner> task_runner_;

  static const base::TimeDelta kResetDurationMs;
  static const base::TimeDelta kAnimateDurationMs;

  base::WeakPtrFactory<OverscrollRefresh> weak_factory_{this};
};

}  // namespace ui

#endif  // UI_OHOS_OVERSCROLL_REFRESH_H_
