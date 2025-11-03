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

#include <queue>
#include <vector>

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/test/motion_event_test_utils.h"
#include "ui/touch_selection/ui_touch_selection_export.h"
#define private public
#include "arkweb/chromium_ext/ui/touch_selection/touch_selection_controller_ext.h"
#include "ui/touch_selection/touch_selection_controller.h"

using testing::ElementsAre;
using testing::IsEmpty;
using ui::test::MockMotionEvent;

namespace ui {
namespace {
constexpr int kDefaultTapTimeoutMs = 200;
constexpr float kDefaultTapSlop = 10.f;
constexpr gfx::PointF kIgnoredPoint(0, 0);
const gfx::RectF kDefaultViewportRect(0, 0, 560, 1200);
const int kDefaultTapDurationMs = 200;
const float kDefaultDrawableSize = 10.f;
constexpr TouchSelectionController::Config kDefaultConfig = {
    .max_tap_duration = base::Milliseconds(kDefaultTapTimeoutMs),
    .tap_slop = kDefaultTapSlop,
};

struct MockDrawableData {
  TouchHandleOrientation orientation = TouchHandleOrientation::UNDEFINED;
  float alpha = 0.f;
  bool mirror_horizontal = false;
  bool mirror_vertical = false;
  bool enabled = false;
  bool visible = false;
  gfx::RectF rect{0, 0, kDefaultDrawableSize, kDefaultDrawableSize};
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

class MockTouchHandleClient : public TouchHandleClient {
 public:
  MockTouchHandleClient()
      : dragging_(false),
        dragged_(false),
        tapped_(false),
        needs_animate_(false) {}

  ~MockTouchHandleClient() override {}

  // TouchHandleClient implementation.
  void OnDragBegin(const TouchSelectionDraggable& handler,
                   const gfx::PointF& drag_position) override {
    dragging_ = true;
  }

  void OnDragUpdate(const TouchSelectionDraggable& handler,
                    const gfx::PointF& drag_position) override {
    dragged_ = true;
    drag_position_ = drag_position;
  }

  void OnDragEnd(const TouchSelectionDraggable& handler) override {
    dragging_ = false;
  }

  void UpdateSelectionChanged(
      const TouchSelectionDraggable& draggable) override {
  }

  bool IsWithinTapSlop(const gfx::Vector2dF& delta) const override {
    return delta.LengthSquared() < (kDefaultTapSlop * kDefaultTapSlop);
  }

  void OnHandleTapped(const TouchHandle& handle) override { tapped_ = true; }

  void SetNeedsAnimate() override { needs_animate_ = true; }

  std::unique_ptr<TouchHandleDrawable> CreateDrawable() override {
    return std::make_unique<MockTouchHandleDrawableExt>(true);
  }

  base::TimeDelta GetMaxTapDuration() const override {
    return base::Milliseconds(kDefaultTapDurationMs);
  }

  bool IsAdaptiveHandleOrientationEnabled() const override {
    // Enable adaptive handle orientation by default for unittests
    return true;
  }

  void Animate(TouchHandle& handle) {
    needs_animate_ = false;
    base::TimeTicks now = base::TimeTicks::Now();
    while (handle.Animate(now))
      now += base::Milliseconds(16);
  }

  bool GetAndResetHandleDragged() {
    bool dragged = dragged_;
    dragged_ = false;
    return dragged;
  }

  bool GetAndResetHandleTapped() {
    bool tapped = tapped_;
    tapped_ = false;
    return tapped;
  }

  bool GetAndResetNeedsAnimate() {
    bool needs_animate = needs_animate_;
    needs_animate_ = false;
    return needs_animate;
  }

  void UpdateHandleFocus(TouchHandle& handle,
                         gfx::PointF& top,
                         gfx::PointF& bottom) {
    handle.SetFocus(top, bottom);
    handle.UpdateHandleLayout();
  }

  void UpdateHandleOrientation(TouchHandle& handle,
                               TouchHandleOrientation orientation) {
    handle.SetOrientation(orientation);
    handle.UpdateHandleLayout();
  }

  void UpdateHandleVisibility(TouchHandle& handle,
                              bool visible,
                              TouchHandle::AnimationStyle animation_style) {
    handle.SetVisible(visible, animation_style);
    handle.UpdateHandleLayout();
  }

  void UpdateViewportRect(TouchHandle& handle, gfx::RectF viewport_rect) {
    handle.SetViewportRect(viewport_rect);
    handle.UpdateHandleLayout();
  }

  bool IsDragging() const { return dragging_; }
  const gfx::PointF& DragPosition() const { return drag_position_; }
  bool NeedsAnimate() const { return needs_animate_; }

  const MockDrawableData& drawable() { return drawable_data_; }

  void SetEnalbeDrawable(bool enabled) { enable_drawable_ = enabled; }

 private:
  gfx::PointF drag_position_;
  bool dragging_;
  bool dragged_;
  bool tapped_;
  bool needs_animate_;
  bool enable_drawable_ = false;
  MockDrawableData drawable_data_;
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

  void SelectBetweenCoordinatesV2(const gfx::PointF& position, bool is_base) override {
    is_base_ = is_base;
  }

  bool IsBase() { return is_base_; }

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
  bool is_base_ = false;
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
  controller().SetTouchNumsForHandle(event);

  ChangeSelection(start_rect, visible, end_rect, visible);
  EXPECT_EQ(start_rect.bottom_left(), GetLastEventStart());

  ChangeInsertion(end_rect, visible);
  EXPECT_EQ(end_rect.bottom_left(), GetLastEventStart());
  controller().WillHandleTouchEvent(event);
  ClearInsertion();

  OnTapEvent();
  ChangeInsertion(end_rect, visible);
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
  // Enable dragging so that the following Action::DOWN starts handle dragging.
  SetDraggingEnabled(true);

  // Touch down to start dragging.
  MockMotionEvent event(MockMotionEvent::Action::DOWN, event_time, 0, 0);
  EXPECT_FALSE(controller().WillHandleTouchEvent(event));
  EXPECT_FALSE(GetAndResetCaretMoved());

  // Move the handle.
  gfx::PointF start_offset = start_rect.CenterPoint();
  event = MockMotionEvent(MockMotionEvent::Action::MOVE, event_time, 0, 5);
  EXPECT_FALSE(controller().WillHandleTouchEvent(event));
  EXPECT_FALSE(GetAndResetCaretMoved());

  // Deactivate touch selection to end dragging.
  controller().HideAndDisallowShowingAutomatically();

  // Move the finger. There is no handle to move, so the cursor is not moved;
  // but, the event is still consumed because the touch down that started the
  // touch sequence was consumed.
  event = MockMotionEvent(MockMotionEvent::Action::MOVE, event_time, 5, 5);
  EXPECT_FALSE(controller().WillHandleTouchEvent(event));
  EXPECT_FALSE(GetAndResetCaretMoved());

  // Lift the finger to end the touch sequence.
  event = MockMotionEvent(MockMotionEvent::Action::UP, event_time, 5, 5);
  EXPECT_FALSE(controller().WillHandleTouchEvent(event));
  EXPECT_FALSE(GetAndResetCaretMoved());

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

  // The Action::DOWN should lock to the closest handle.
  gfx::PointF end_offset = end_rect.CenterPoint();
  gfx::PointF fixed_offset = start_rect.CenterPoint();
  float touch_down_x = (end_offset.x() + fixed_offset.x()) / 2 + 1.f;
  MockMotionEvent event(MockMotionEvent::Action::DOWN, event_time, touch_down_x,
                        0);
  SetDraggingEnabled(true);
  EXPECT_FALSE(controller().WillHandleTouchEvent(event));
  controller().SelectOverImg();
  EXPECT_FALSE(GetAndResetSelectionMoved());
  controller().SelectOverImg();

  // Even though the Action::MOVE is over the start handle, it should continue
  // targetting the end handle that consumed the Action::DOWN.
  event = MockMotionEvent(MockMotionEvent::Action::MOVE, event_time, 0, 0);
  EXPECT_FALSE(controller().WillHandleTouchEvent(event));
  controller().SelectOverImg();
  EXPECT_FALSE(GetAndResetSelectionMoved());

  gfx::SelectionBound bound;
  gfx::RectF rect(50, 5, 0, 10);
  bound.set_type(gfx::SelectionBound::CENTER);
  bound.SetEdge(rect.origin(), rect.bottom_left());
  bound.set_visible(visible);
  controller().OnHandleSwap(true, bound, bound);
  controller().OnHandleSwap(false, bound, bound);

  event = MockMotionEvent(MockMotionEvent::Action::UP, event_time, 0, 0);
  EXPECT_FALSE(controller().WillHandleTouchEvent(event));
  ChangeSelection(start_rect, true, end_rect, false);
  controller().IsEndHandleNotVisible(event);
  ChangeSelection(start_rect, false, end_rect, false);
  controller().IsEndHandleNotVisible(event);
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
  start_rect.Offset(1, 0);
  ChangeSelection(start_rect, visible, end_rect, visible);
  controller().SetTouchNumsForHandle(event);
  // Selection movement does not currently trigger a separate event.

  PreTouchInfo first_down = {1, 1, base::TimeTicks::Now()};
  PreTouchInfo second_down = {1, 1, base::TimeTicks::Now()};
  EXPECT_TRUE(controller().IsContinuousEvent(first_down, second_down, true));
  first_down = {1, 1, base::TimeTicks::Min()};
  second_down = {1, 1, base::TimeTicks::Max()};
  EXPECT_FALSE(controller().IsContinuousEvent(first_down, second_down, true));

  ClearSelection();
}

TEST_F(TouchSelectionControllerExtTest, ArkSelectBetweenCoordinates) {
  ClearSelection();

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

  // The Action::DOWN should lock to the closest handle.
  gfx::PointF end_offset = end_rect.CenterPoint();
  gfx::PointF fixed_offset = start_rect.CenterPoint();
  float touch_down_x = (end_offset.x() + fixed_offset.x()) / 2 + 1.f;
  MockMotionEvent event(MockMotionEvent::Action::DOWN, event_time, touch_down_x,
                        0);
  SetDraggingEnabled(true);
  EXPECT_FALSE(controller().WillHandleTouchEvent(event));
  controller().SelectOverImg();
  EXPECT_FALSE(GetAndResetSelectionMoved());
  controller().SelectOverImg();

  // Even though the Action::MOVE is over the start handle, it should continue
  // targetting the end handle that consumed the Action::DOWN.
  event = MockMotionEvent(MockMotionEvent::Action::MOVE, event_time, 0, 0);
  EXPECT_FALSE(controller().WillHandleTouchEvent(event));
  controller().SelectOverImg();
  EXPECT_FALSE(GetAndResetSelectionMoved());

  gfx::SelectionBound bound;
  gfx::RectF rect(50, 5, 0, 10);
  bound.set_type(gfx::SelectionBound::CENTER);
  bound.SetEdge(rect.origin(), rect.bottom_left());
  bound.set_visible(visible);
  controller().OnHandleSwap(true, bound, bound);
  controller().OnHandleSwap(false, bound, bound);

  event = MockMotionEvent(MockMotionEvent::Action::UP, event_time, 0, 0);
  EXPECT_FALSE(controller().WillHandleTouchEvent(event));
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
  EXPECT_FALSE(GetAndResetSelectionMoved());
  ClearSelection();
}

TEST_F(TouchSelectionControllerExtTest, UpdateSelectionChanged) {
  std::shared_ptr<TouchHandleClient> touchHandleClient = std::make_shared<MockTouchHandleClient>();
  MockMotionEvent event(MockMotionEvent::Action::DOWN, base::TimeTicks::Now(), 1, 0);
  gfx::SelectionBound start_bound;
  gfx::SelectionBound end_bound;
  controller().active_status_ = TouchSelectionController::ActiveStatus::SELECTION_ACTIVE;
  EXPECT_EQ(controller().SelectOverImg(), false);
  controller().insertion_handle_ =
    std::make_unique<TouchHandleExt>(touchHandleClient.get(), TouchHandleOrientation::CENTER, kDefaultViewportRect);
  controller().start_selection_handle_ =
    std::make_unique<TouchHandleExt>(touchHandleClient.get(), TouchHandleOrientation::LEFT, kDefaultViewportRect);
  EXPECT_EQ(controller().SelectOverImg(), false);
  controller().end_selection_handle_ =
    std::make_unique<TouchHandleExt>(touchHandleClient.get(), TouchHandleOrientation::RIGHT, kDefaultViewportRect);
  controller().SetTouchNumsForHandle(event);
  EXPECT_EQ(controller().gestureTouchQueue_.size(), 1);
  controller().UpdateSelectionChanged(*controller().insertion_handle_);
  controller().UpdateSelectionChanged(*controller().start_selection_handle_);
  controller().ResetPositionAfterDragEnd(*controller().start_selection_handle_);
  controller().ResetPositionAfterDragEnd(*controller().end_selection_handle_);
  controller().OnInsertionChangedExt(start_bound, end_bound);
  controller().active_status_ = TouchSelectionController::ActiveStatus::INACTIVE;
  EXPECT_EQ(controller().SelectOverImg(), false);
  controller().active_status_ = TouchSelectionController::ActiveStatus::SELECTION_ACTIVE;
  controller().show_touch_handles_ = false;
  EXPECT_EQ(controller().SelectOverImg(), false);
  controller().show_touch_handles_ = true;
  EXPECT_EQ(controller().SelectOverImg(), false);
  controller().end_selection_handle_->BeginDrag();
  EXPECT_EQ(controller().SelectOverImg(), true);
  controller().start_selection_handle_->BeginDrag();
  EXPECT_EQ(controller().SelectOverImg(), true);
  controller().start_selection_handle_->EndDrag();
  controller().end_selection_handle_->EndDrag();

  controller().longpress_drag_selector_.SetState(LongPressDragSelector::SelectionState::DRAGGING);
  EXPECT_EQ(controller().SelectOverImg(), true);

  controller().end_selection_handle_->SetVisible(true, TouchHandle::AnimationStyle::ANIMATION_NONE);
  controller().IsEndHandleNotVisible(event);
  EXPECT_EQ(controller().end_selection_handle_->AsTouchHandleExt()->GetVisible(), true);
  controller().end_selection_handle_->SetVisible(false, TouchHandle::AnimationStyle::ANIMATION_NONE);
  controller().IsEndHandleNotVisible(event);
  EXPECT_EQ(controller().end_selection_handle_->AsTouchHandleExt()->GetVisible(), false);
  controller().start_selection_handle_->SetVisible(false, TouchHandle::AnimationStyle::ANIMATION_NONE);
  controller().IsEndHandleNotVisible(event);
  EXPECT_EQ(controller().start_selection_handle_->AsTouchHandleExt()->GetVisible(), false);
  controller().start_selection_handle_->SetVisible(true, TouchHandle::AnimationStyle::ANIMATION_NONE);
  controller().IsEndHandleNotVisible(event);
  EXPECT_EQ(controller().start_selection_handle_->AsTouchHandleExt()->GetVisible(), true);
  controller().insertion_handle_ = nullptr;
  controller().start_selection_handle_ = nullptr;
  controller().end_selection_handle_ = nullptr;
  controller().SetTouchNumsForHandle(event);
  EXPECT_EQ(controller().gestureTouchQueue_.size(), 2);
}

TEST_F(TouchSelectionControllerExtTest, ArkSelectBetweenCoordinates002) {
  gfx::RectF rect(5, 5, 0, 10);
  gfx::RectF line_rect(50, 5, 10, 0);
  gfx::SelectionBound start_bound;
  gfx::SelectionBound end_bound;
  gfx::PointF base(7.5f, 5.0f);
  gfx::PointF extent(50.0f, 5.0f);
  start_bound.set_type(gfx::SelectionBound::LEFT);
  end_bound.set_type(gfx::SelectionBound::RIGHT);
  std::shared_ptr<TouchHandleClient> touchHandleClient = std::make_shared<MockTouchHandleClient>();
  controller().insertion_handle_ =
    std::make_unique<TouchHandle>(touchHandleClient.get(), TouchHandleOrientation::CENTER, kDefaultViewportRect);
  controller().start_selection_handle_ =
    std::make_unique<TouchHandleExt>(touchHandleClient.get(), TouchHandleOrientation::LEFT, kDefaultViewportRect);
  controller().end_selection_handle_ =
    std::make_unique<TouchHandleExt>(touchHandleClient.get(), TouchHandleOrientation::RIGHT, kDefaultViewportRect);
  controller().end_selection_handle_->SetVisible(true, TouchHandle::AnimationStyle::ANIMATION_NONE);
  controller().start_selection_handle_->SetVisible(true, TouchHandle::AnimationStyle::ANIMATION_NONE);
  SelectBetweenCoordinatesV2(extent, true);
  controller().ArkSelectBetweenCoordinates(base, extent);
  controller().start_selection_handle_->SetVisible(false, TouchHandle::AnimationStyle::ANIMATION_NONE);
  controller().ArkSelectBetweenCoordinates(base, extent);
  start_bound.SetEdge(rect.origin(), rect.bottom_left());
  controller().start_ = start_bound;
  controller().ArkSelectBetweenCoordinates(base, extent);
  start_bound.SetEdge(line_rect.origin(), line_rect.bottom_left());
  controller().start_ = start_bound;
  controller().ArkSelectBetweenCoordinates(base, extent);
  end_bound.SetEdge(line_rect.origin(), line_rect.bottom_left());
  controller().end_ = end_bound;
  controller().ArkSelectBetweenCoordinates(base, extent);
  EXPECT_TRUE(IsBase());
  end_bound.SetEdge(rect.origin(), rect.bottom_left());
  controller().end_ = end_bound;
  controller().ArkSelectBetweenCoordinates(base, extent);
  EXPECT_FALSE(IsBase());
  controller().anchor_drag_to_selection_start_ = true;
  controller().ArkSelectBetweenCoordinates(base, extent);
  controller().anchor_drag_to_selection_start_ = false;

  controller().end_selection_handle_->SetVisible(false, TouchHandle::AnimationStyle::ANIMATION_NONE);
  controller().ArkSelectBetweenCoordinates(base, extent);
  end_bound.SetEdge(line_rect.origin(), line_rect.bottom_left());
  controller().end_ = end_bound;
  controller().ArkSelectBetweenCoordinates(base, extent);
  EXPECT_FALSE(IsBase());
  start_bound.SetEdge(rect.origin(), rect.bottom_left());
  controller().start_ = start_bound;
  controller().ArkSelectBetweenCoordinates(base, extent);
  EXPECT_TRUE(IsBase());
  controller().anchor_drag_to_selection_start_ = true;
  controller().ArkSelectBetweenCoordinates(base, extent);
  controller().anchor_drag_to_selection_start_ = false;
  controller().end_selection_handle_->BeginDrag();
  EXPECT_EQ(controller().OnHandleSwap(false, start_bound, end_bound), true);
  controller().end_selection_handle_->EndDrag();
  controller().start_selection_handle_->BeginDrag();
  EXPECT_EQ(controller().OnHandleSwap(false, start_bound, end_bound), true);
  controller().start_selection_handle_->EndDrag();

  controller().insertion_handle_ = nullptr;
  controller().start_selection_handle_ = nullptr;
  controller().end_selection_handle_ = nullptr;
  controller().OnInsertionChangedExt(start_bound, end_bound);
}
}
}  // namespace ui
