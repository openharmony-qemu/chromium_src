/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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


#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/test/motion_event_test_utils.h"
#include "ui/touch_selection/touch_selection_controller_test_api.h"
#include "ui/touch_selection/touch_selection_controller.h"
#include "ui/touch_selection/ui_touch_selection_export.h"
#include <queue>
#define private public
#include "arkweb/chromium_ext/ui/touch_selection/touch_selection_controller_ext.h"

using testing::ElementsAre;
using testing::IsEmpty;
using ui::test::MockMotionEvent;

namespace ui {
namespace {
constexpr int kDefaultTapTimeoutMs = 200;
constexpr float kDefaultTapSlop = 10.f;
constexpr gfx::PointF kIgnoredPoint(0, 0);
constexpr TouchSelectionController::Config kDefaultConfig = {
    .max_tap_duration = base::Milliseconds(kDefaultTapTimeoutMs),
    .tap_slop = kDefaultTapSlop,
};

class MockTouchHandleDrawableExt : public TouchHandleDrawable {
 public:
  explicit MockTouchHandleDrawableExt(bool contains_point)
      : intersects_rect_(contains_point) {}

  MockTouchHandleDrawableExt(const MockTouchHandleDrawableExt&) = delete;
  MockTouchHandleDrawableExt& operator=(const MockTouchHandleDrawableExt&) = delete;

  ~MockTouchHandleDrawableExt() override {}
  void SetEnabled(bool enabled) override {}
  void SetOrientation(TouchHandleOrientation orientation,
                      bool mirror_vertical,
                      bool mirror_horizontal) override {}
  void SetOrigin(const gfx::PointF& origin) override {}
  void SetAlpha(float alpha) override {}
  gfx::RectF GetVisibleBounds() const override {
    return intersects_rect_ ? gfx::RectF(5, 5, 0, 10)
                             : gfx::RectF(-1000, -1000, 0, 0);
  }
  float GetDrawableHorizontalPaddingRatio() const override { return 0; }

  void SetEdge(const gfx::PointF& top, const gfx::PointF& bottom) override {}

 private:
  bool intersects_rect_;
};

class TouchSelectionControllerExtTest : public testing::Test,
                                     public TouchSelectionControllerClient {
 public:
  TouchSelectionControllerExtTest() = default;

  TouchSelectionControllerExtTest(const TouchSelectionControllerExtTest&) = delete;
  TouchSelectionControllerExtTest& operator=(const TouchSelectionControllerExtTest&) =
      delete;

  ~TouchSelectionControllerExtTest() override {}

  void SetUp() override {
    InitializeControllerWithConfig(kDefaultConfig);
  }

  void TearDown() override { controller_.reset(); }

  bool SupportsAnimation() const override { return animation_enabled_; }

  void SetNeedsAnimate() override { needs_animate_ = true; }

  void MoveCaret(const gfx::PointF& position) override {
    caret_moved_ = true;
    caret_position_ = position;
  }

  void SelectBetweenCoordinates(const gfx::PointF& base,
                                const gfx::PointF& extent) override {
    if (base == selection_end_ && extent == selection_start_)
      selection_points_swapped_ = true;

    selection_start_ = base;
    selection_end_ = extent;
  }

  void MoveRangeSelectionExtent(const gfx::PointF& extent) override {
    selection_moved_ = true;
    selection_end_ = extent;
  }

  void OnSelectionEvent(SelectionEventType event) override {
    events_.push_back(event);
    last_event_start_ = controller_->GetStartPosition();
    last_event_end_ = controller_->GetEndPosition();
    last_event_bounds_rect_ = controller_->GetRectBetweenBounds();
  }

  void OnDragUpdate(const TouchSelectionDraggable::Type type,
                    const gfx::PointF& position) override {
    last_drag_update_position_ = position;
  }

  std::unique_ptr<TouchHandleDrawable> CreateDrawable() override {
    return std::make_unique<MockTouchHandleDrawableExt>(&dragging_enabled_);
  }

  void DidScroll() override {}

  void InitializeControllerWithConfig(TouchSelectionController::Config config) {
    controller_ = std::make_unique<TouchSelectionControllerExt>(this, config);
  }

  void StartTouchEventSequence() {
    controller_->WillHandleTouchEvent(
        MockMotionEvent(MotionEvent::Action::DOWN));
  }

  void SetAnimationEnabled(bool enabled) { animation_enabled_ = enabled; }
  void SetDraggingEnabled(bool enabled) { dragging_enabled_ = enabled; }

  void ClearSelection() {
    controller_->OnSelectionBoundsChanged(gfx::SelectionBound(),
                                          gfx::SelectionBound());
  }

  void ClearInsertion() { ClearSelection(); }

  void ChangeInsertion(const gfx::RectF& rect, bool visible) {
    gfx::SelectionBound bound;
    bound.set_type(gfx::SelectionBound::CENTER);
    bound.SetEdge(rect.origin(), rect.bottom_left());
    bound.set_visible(visible);
    controller_->OnSelectionBoundsChanged(bound, bound);
  }

  void ChangeSelection(const gfx::RectF& start_rect,
                       bool start_visible,
                       const gfx::RectF& end_rect,
                       bool end_visible) {
    gfx::SelectionBound start_bound, end_bound;
    start_bound.set_type(gfx::SelectionBound::LEFT);
    end_bound.set_type(gfx::SelectionBound::RIGHT);
    start_bound.SetEdge(start_rect.origin(), start_rect.bottom_left());
    end_bound.SetEdge(end_rect.origin(), end_rect.bottom_left());
    start_bound.set_visible(start_visible);
    end_bound.set_visible(end_visible);
    controller_->OnSelectionBoundsChanged(start_bound, end_bound);
  }

  void ChangeVerticalSelection(const gfx::RectF& start_rect,
                               bool start_visible,
                               const gfx::RectF& end_rect,
                               bool end_visible) {
    gfx::SelectionBound start_bound, end_bound;
    start_bound.set_type(gfx::SelectionBound::RIGHT);
    end_bound.set_type(gfx::SelectionBound::LEFT);
    start_bound.SetEdge(start_rect.origin(), start_rect.bottom_right());
    end_bound.SetEdge(end_rect.bottom_right(), end_rect.origin());
    start_bound.set_visible(start_visible);
    end_bound.set_visible(end_visible);
    controller_->OnSelectionBoundsChanged(start_bound, end_bound);
  }

  void OnLongPressEvent() {
    controller().HandleLongPressEvent(base::TimeTicks(),
                                          kIgnoredPoint);
  }

  void OnDoublePressEvent() {
    controller().HandleDoublePressEvent(base::TimeTicks(), kIgnoredPoint);
  }

  void OnTapEvent() {
    controller().HandleTapEvent(kIgnoredPoint, 1);
  }

  void OnDoubleTapEvent() {
    controller().HandleTapEvent(kIgnoredPoint, 2);
  }

  void OnTripleTapEvent() { controller().HandleTapEvent(kIgnoredPoint, 3); }

  void Animate() {
    base::TimeTicks now = base::TimeTicks::Now();
    while (needs_animate_) {
      needs_animate_ = controller_->Animate(now);
      now += base::Milliseconds(16);
    }
  }

  bool GetAndResetNeedsAnimate() {
    bool needs_animate = needs_animate_;
    Animate();
    return needs_animate;
  }

  bool GetAndResetCaretMoved() {
    bool moved = caret_moved_;
    caret_moved_ = false;
    return moved;
  }

  bool GetAndResetSelectionMoved() {
    bool moved = selection_moved_;
    selection_moved_ = false;
    return moved;
  }

  bool GetAndResetSelectionPointsSwapped() {
    bool swapped = selection_points_swapped_;
    selection_points_swapped_ = false;
    return swapped;
  }

  const gfx::PointF& GetLastCaretPosition() const { return caret_position_; }
  const gfx::PointF& GetLastSelectionStart() const { return selection_start_; }
  const gfx::PointF& GetLastSelectionEnd() const { return selection_end_; }
  const gfx::PointF& GetLastEventStart() const { return last_event_start_; }
  const gfx::PointF& GetLastEventEnd() const { return last_event_end_; }
  const gfx::RectF& GetLastEventBoundsRect() const {
    return last_event_bounds_rect_;
  }
  const gfx::PointF& GetLastDragUpdatePosition() const {
    return last_drag_update_position_;
  }

  std::vector<SelectionEventType> GetAndResetEvents() {
    std::vector<SelectionEventType> events;
    events.swap(events_);
    return events;
  }

  TouchSelectionControllerExt& controller() { return *controller_; }

 private:
  gfx::PointF last_event_start_;
  gfx::PointF last_event_end_;
  gfx::PointF caret_position_;
  gfx::PointF selection_start_;
  gfx::PointF selection_end_;
  gfx::RectF last_event_bounds_rect_;
  gfx::PointF last_drag_update_position_;
  std::vector<SelectionEventType> events_;
  bool caret_moved_ = false;
  bool selection_moved_ = false;
  bool selection_points_swapped_ = false;
  bool needs_animate_ = false;
  bool animation_enabled_ = true;
  bool dragging_enabled_ = false;
  std::unique_ptr<TouchSelectionControllerExt> controller_;
};

TEST_F(TouchSelectionControllerExtTest, InsertionDragged) {
  base::TimeTicks event_time = base::TimeTicks::Now();
  TearDown();
  TouchSelectionController::Config kLongDragConfig = {
    .max_tap_duration = base::Milliseconds(kDefaultTapTimeoutMs),
    .tap_slop = kDefaultTapSlop,
    .enable_longpress_drag_selection = true,
  };
  InitializeControllerWithConfig(kLongDragConfig);
  MockMotionEvent event(MockMotionEvent::Action::CANCEL, event_time, 0, 0);
  controller().WillHandleTouchEvent(event);
  controller().SetTouchNumsForHandle(event);
  OnLongPressEvent();

  gfx::RectF start_rect(5, 5, 0, 10);
  gfx::RectF end_rect(50, 5, 0, 10);
  bool visible = true;

  ChangeInsertion(start_rect, visible);
  EXPECT_THAT(GetAndResetEvents(),
              ElementsAre(INSERTION_HANDLE_SHOWN));
  EXPECT_EQ(start_rect.bottom_left(), GetLastEventStart());
  controller().SetTouchNumsForHandle(event);

  ChangeSelection(start_rect, visible, end_rect, visible);
  EXPECT_THAT(GetAndResetEvents(),
              ElementsAre(INSERTION_HANDLE_CLEARED, SELECTION_HANDLES_SHOWN));
  EXPECT_EQ(start_rect.bottom_left(), GetLastEventStart());

  ChangeInsertion(end_rect, visible);
  EXPECT_THAT(GetAndResetEvents(),
              ElementsAre(SELECTION_HANDLES_CLEARED, INSERTION_HANDLE_SHOWN));
  EXPECT_EQ(end_rect.bottom_left(), GetLastEventStart());
  controller().WillHandleTouchEvent(event);
  ClearInsertion();
  EXPECT_THAT(GetAndResetEvents(), ElementsAre(INSERTION_HANDLE_CLEARED));

  OnTapEvent();
  ChangeInsertion(end_rect, visible);
  EXPECT_THAT(GetAndResetEvents(), ElementsAre(INSERTION_HANDLE_SHOWN));
  EXPECT_EQ(end_rect.bottom_left(), GetLastEventStart());
  TearDown();
  InitializeControllerWithConfig(kDefaultConfig);
}

TEST_F(TouchSelectionControllerExtTest, InsertionDeactivatedWhileDragging) {
  base::TimeTicks event_time = base::TimeTicks::Now();
  OnTapEvent();

  float line_height = 10.f;
  gfx::RectF start_rect(10, 0, 0, line_height);
  bool visible = true;
  ChangeInsertion(start_rect, visible);
  EXPECT_THAT(GetAndResetEvents(),
              ElementsAre(INSERTION_HANDLE_SHOWN));
  EXPECT_EQ(start_rect.bottom_left(), GetLastEventStart());

  // Enable dragging so that the following Action::DOWN starts handle dragging.
  SetDraggingEnabled(true);

  // Touch down to start dragging.
  MockMotionEvent event(MockMotionEvent::Action::DOWN, event_time, 0, 0);
  EXPECT_TRUE(controller().WillHandleTouchEvent(event));
  EXPECT_FALSE(GetAndResetCaretMoved());
  EXPECT_THAT(GetAndResetEvents(), ElementsAre(INSERTION_HANDLE_DRAG_STARTED));

  // Move the handle.
  gfx::PointF start_offset = start_rect.CenterPoint();
  event = MockMotionEvent(MockMotionEvent::Action::MOVE, event_time, 0, 5);
  EXPECT_TRUE(controller().WillHandleTouchEvent(event));
  EXPECT_TRUE(GetAndResetCaretMoved());
  EXPECT_EQ(start_offset + gfx::Vector2dF(0, 5), GetLastCaretPosition());

  // Deactivate touch selection to end dragging.
  controller().HideAndDisallowShowingAutomatically();
  EXPECT_THAT(GetAndResetEvents(), ElementsAre(INSERTION_HANDLE_DRAG_STOPPED,
                                               INSERTION_HANDLE_CLEARED));

  // Move the finger. There is no handle to move, so the cursor is not moved;
  // but, the event is still consumed because the touch down that started the
  // touch sequence was consumed.
  event = MockMotionEvent(MockMotionEvent::Action::MOVE, event_time, 5, 5);
  EXPECT_TRUE(controller().WillHandleTouchEvent(event));
  EXPECT_FALSE(GetAndResetCaretMoved());
  EXPECT_EQ(start_offset + gfx::Vector2dF(0, 5), GetLastCaretPosition());

  // Lift the finger to end the touch sequence.
  event = MockMotionEvent(MockMotionEvent::Action::UP, event_time, 5, 5);
  EXPECT_TRUE(controller().WillHandleTouchEvent(event));
  EXPECT_FALSE(GetAndResetCaretMoved());
  EXPECT_THAT(GetAndResetEvents(), IsEmpty());

  gfx::SelectionBound start_bound, end_bound;
  gfx::RectF start_rect2(5, 5, 0, 10);
  gfx::RectF end_rect2(50, 5, 0, 10);
  start_bound.set_type(gfx::SelectionBound::LEFT);
  end_bound.set_type(gfx::SelectionBound::RIGHT);
  start_bound.SetEdge(start_rect2.origin(), start_rect2.bottom_left());
  end_bound.SetEdge(end_rect2.origin(), end_rect2.bottom_left());
  start_bound.set_visible(true);
  end_bound.set_visible(true);
  controller().OnInsertionChangedExt(start_bound, end_bound);

  // Following Action::DOWN should not be consumed if it does not start handle
  // dragging.
  SetDraggingEnabled(false);
  event = MockMotionEvent(MotionEvent::Action::DOWN, event_time, 0, 0);
  EXPECT_FALSE(controller().WillHandleTouchEvent(event));
}

TEST_F(TouchSelectionControllerExtTest, GetTouchNums) {
  MockMotionEvent event(MockMotionEvent::Action::DOWN, base::TimeTicks::Now(),
                        1, 1);
  EXPECT_EQ(controller().GetTouchNums(event), 1);
  MockMotionEvent event2(MockMotionEvent::Action::DOWN, base::TimeTicks::Now(),
                         1000, 1000);
  EXPECT_EQ(controller().GetTouchNums(event2), 1);
  MockMotionEvent event3(MockMotionEvent::Action::DOWN, base::TimeTicks::Now(),
                         1000, 1000);
  EXPECT_EQ(controller().GetTouchNums(event3), 2);
  MockMotionEvent event4(MockMotionEvent::Action::DOWN, base::TimeTicks::Now(),
                         1000, 1000);
  EXPECT_EQ(controller().GetTouchNums(event4), 3);
}

TEST_F(TouchSelectionControllerExtTest, SelectionBasicDrag_001) {
  PreTouchInfo curTouchInfo;
  curTouchInfo.x = 1;
  curTouchInfo.y = 1;
  curTouchInfo.start = base::TimeTicks::Now();
  std::queue<PreTouchInfo> touchQueue;
  touchQueue.push(curTouchInfo);
  controller().gestureTouchQueue_ = std::move(touchQueue);
  controller().SelectOverImg();
  base::TimeTicks event_time = base::TimeTicks::Now();
  OnLongPressEvent();

  float line_height = 10.f;
  gfx::RectF start_rect(0, 0, 0, line_height);
  gfx::RectF end_rect(50, 0, 0, line_height);
  bool visible = true;
  ChangeSelection(start_rect, visible, end_rect, visible);
  controller().SelectOverImg();
  EXPECT_THAT(GetAndResetEvents(),
              ElementsAre(SELECTION_HANDLES_SHOWN));
  EXPECT_EQ(start_rect.bottom_left(), GetLastEventStart());

  // The Action::DOWN should lock to the closest handle.
  gfx::PointF end_offset = end_rect.CenterPoint();
  gfx::PointF fixed_offset = start_rect.CenterPoint();
  float touch_down_x = (end_offset.x() + fixed_offset.x()) / 2 + 1.f;
  MockMotionEvent event(MockMotionEvent::Action::DOWN, event_time, touch_down_x,
                        0);
  SetDraggingEnabled(true);
  EXPECT_TRUE(controller().WillHandleTouchEvent(event));
  controller().SelectOverImg();
  EXPECT_THAT(GetAndResetEvents(), ElementsAre(SELECTION_HANDLE_DRAG_STARTED));
  EXPECT_FALSE(GetAndResetSelectionMoved());
  controller().SelectOverImg();

  // Even though the Action::MOVE is over the start handle, it should continue
  // targetting the end handle that consumed the Action::DOWN.
  event = MockMotionEvent(MockMotionEvent::Action::MOVE, event_time, 0, 0);
  EXPECT_TRUE(controller().WillHandleTouchEvent(event));
  controller().SelectOverImg();
  EXPECT_TRUE(GetAndResetSelectionMoved());
  EXPECT_EQ(fixed_offset, GetLastSelectionStart());
  EXPECT_EQ(end_offset - gfx::Vector2dF(touch_down_x, 0),
            GetLastSelectionEnd());

  gfx::SelectionBound bound;
  gfx::RectF rect(50, 5, 0, 10);
  bound.set_type(gfx::SelectionBound::CENTER);
  bound.SetEdge(rect.origin(), rect.bottom_left());
  bound.set_visible(visible);
  controller().OnHandleSwap(true, bound, bound);
  controller().OnHandleSwap(false, bound, bound);

  event = MockMotionEvent(MockMotionEvent::Action::UP, event_time, 0, 0);
  EXPECT_TRUE(controller().WillHandleTouchEvent(event));
  ChangeSelection(start_rect, true, end_rect, false);
  controller().HandleIfEndNotVisible(event);
  ChangeSelection(start_rect, false, end_rect, false);
  controller().HandleIfEndNotVisible(event);
  EXPECT_THAT(GetAndResetEvents(), ElementsAre(SELECTION_HANDLE_DRAG_STOPPED));
  EXPECT_FALSE(GetAndResetSelectionMoved());
  ClearSelection();
}

TEST_F(TouchSelectionControllerExtTest, SelectionBasic) {
  PreTouchInfo curTouchInfo;
  curTouchInfo.x = 1;
  curTouchInfo.y = 1;
  curTouchInfo.start = base::TimeTicks::Now();
  std::queue<PreTouchInfo> touchQueue;
  touchQueue.push(curTouchInfo);
  controller().gestureTouchQueue_ = std::move(touchQueue);
  gfx::RectF start_rect(5, 5, 0, 10);
  gfx::RectF end_rect(50, 5, 0, 10);
  base::TimeTicks event_time = base::TimeTicks::Now();
  MockMotionEvent event(MockMotionEvent::Action::CANCEL, event_time, 0, 0);
  ClearSelection();
  bool visible = true;

  OnLongPressEvent();
  ChangeSelection(start_rect, visible, end_rect, visible);
  EXPECT_THAT(GetAndResetEvents(),
              ElementsAre(SELECTION_HANDLES_SHOWN));
  EXPECT_EQ(start_rect.bottom_left(), GetLastEventStart());

  start_rect.Offset(1, 0);
  ChangeSelection(start_rect, visible, end_rect, visible);
  controller().SetTouchNumsForHandle(event);
  // Selection movement does not currently trigger a separate event.
  EXPECT_THAT(GetAndResetEvents(), ElementsAre(SELECTION_HANDLES_MOVED));
  EXPECT_EQ(start_rect.bottom_left(), GetLastEventStart());
  EXPECT_EQ(end_rect.bottom_left(), GetLastEventEnd());

  PreTouchInfo first_down = {1, 1, base::TimeTicks::Now()};
  PreTouchInfo second_down = {1, 1, base::TimeTicks::Now()};
  controller().IsContinuousEvent(first_down, second_down, true);
  first_down = {1, 1, base::TimeTicks::Min()};
  second_down = {1, 1, base::TimeTicks::Max()};
  controller().IsContinuousEvent(first_down, second_down, true);

  ClearSelection();
  EXPECT_THAT(GetAndResetEvents(),
              ElementsAre(SELECTION_HANDLES_CLEARED));
}

TEST_F(TouchSelectionControllerExtTest, ArkSelectBetweenCoordinates) {
  ClearSelection();
  EXPECT_THAT(GetAndResetEvents(),
              ElementsAre(SELECTION_HANDLES_CLEARED));

  PreTouchInfo curTouchInfo;
  curTouchInfo.x = 1;
  curTouchInfo.y = 1;
  curTouchInfo.start = base::TimeTicks::Now();
  std::queue<PreTouchInfo> touchQueue;
  touchQueue.push(curTouchInfo);
  controller().gestureTouchQueue_ = std::move(touchQueue);
  controller().SelectOverImg();
  base::TimeTicks event_time = base::TimeTicks::Now();
  OnLongPressEvent();

  float line_height = 10.f;
  gfx::RectF start_rect(0, 0, 0, line_height);
  gfx::RectF end_rect(50, 0, 0, line_height);
  gfx::PointF base(7.5f, 5.0f);
  gfx::PointF extent(50.0f, 5.0f);
  bool visible = true;
  ChangeSelection(start_rect, visible, end_rect, visible);
  controller().SelectOverImg();
  EXPECT_THAT(GetAndResetEvents(),
              ElementsAre(SELECTION_HANDLES_SHOWN));
  EXPECT_EQ(start_rect.bottom_left(), GetLastEventStart());

  // The Action::DOWN should lock to the closest handle.
  gfx::PointF end_offset = end_rect.CenterPoint();
  gfx::PointF fixed_offset = start_rect.CenterPoint();
  float touch_down_x = (end_offset.x() + fixed_offset.x()) / 2 + 1.f;
  MockMotionEvent event(MockMotionEvent::Action::DOWN, event_time, touch_down_x,
                        0);
  SetDraggingEnabled(true);
  EXPECT_TRUE(controller().WillHandleTouchEvent(event));
  controller().SelectOverImg();
  EXPECT_THAT(GetAndResetEvents(), ElementsAre(SELECTION_HANDLE_DRAG_STARTED));
  EXPECT_FALSE(GetAndResetSelectionMoved());
  controller().SelectOverImg();

  // Even though the Action::MOVE is over the start handle, it should continue
  // targetting the end handle that consumed the Action::DOWN.
  event = MockMotionEvent(MockMotionEvent::Action::MOVE, event_time, 0, 0);
  EXPECT_TRUE(controller().WillHandleTouchEvent(event));
  controller().SelectOverImg();
  EXPECT_TRUE(GetAndResetSelectionMoved());
  EXPECT_EQ(fixed_offset, GetLastSelectionStart());
  EXPECT_EQ(end_offset - gfx::Vector2dF(touch_down_x, 0),
            GetLastSelectionEnd());

  gfx::SelectionBound bound;
  gfx::RectF rect(50, 5, 0, 10);
  bound.set_type(gfx::SelectionBound::CENTER);
  bound.SetEdge(rect.origin(), rect.bottom_left());
  bound.set_visible(visible);
  controller().OnHandleSwap(true, bound, bound);
  controller().OnHandleSwap(false, bound, bound);

  event = MockMotionEvent(MockMotionEvent::Action::UP, event_time, 0, 0);
  EXPECT_TRUE(controller().WillHandleTouchEvent(event));
  start_rect.Offset(1, 0);
  start_rect.set_x(6);
  ChangeSelection(start_rect, visible, end_rect, visible);
  controller().ArkSelectBetweenCoordinates(base, extent);
  start_rect.set_x(7);
  ChangeSelection(start_rect, true, end_rect, false);
  controller().ArkSelectBetweenCoordinates(base, extent);
  start_rect.set_x(8);
  ChangeSelection(start_rect, false, end_rect, false);
  controller().ArkSelectBetweenCoordinates(base, extent);
  start_rect.set_x(9);
  ChangeSelection(start_rect, false, end_rect, false);
  controller().ArkSelectBetweenCoordinates(base, extent);
  start_rect.set_x(10);
  ChangeSelection(start_rect, false, end_rect, false);
  controller().ArkSelectBetweenCoordinates(base, extent);
  EXPECT_THAT(GetAndResetEvents(), ElementsAre(SELECTION_HANDLE_DRAG_STOPPED));
  EXPECT_FALSE(GetAndResetSelectionMoved());
  ClearSelection();
}
}
}  // namespace ui