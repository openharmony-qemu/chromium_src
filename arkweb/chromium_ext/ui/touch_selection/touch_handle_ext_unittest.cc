// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "base/time/time.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/test/motion_event_test_utils.h"
#include "ui/events/velocity_tracker/motion_event.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/vector2d_f.h"
#include "ui/touch_selection/touch_handle_orientation.h"
#include "ui/touch_selection/touch_selection_draggable.h"
#include "ui/touch_selection/ui_touch_selection_export.h"
#define private public
#include "ui/touch_selection/touch_handle.h"
#include "arkweb/chromium_ext/ui/touch_selection/touch_handle_ext.h"

using ui::test::MockMotionEvent;

namespace ui {
namespace {

const int kDefaultTapDurationMs = 200;
const double kDefaultTapSlop = 10.;
const float kDefaultDrawableSize = 10.f;
const gfx::RectF kDefaultViewportRect(0, 0, 560, 1200);

struct MockDrawableData {
  TouchHandleOrientation orientation = TouchHandleOrientation::UNDEFINED;
  float alpha = 0.f;
  bool mirror_horizontal = false;
  bool mirror_vertical = false;
  bool enabled = false;
  bool visible = false;
  gfx::RectF rect{0, 0, kDefaultDrawableSize, kDefaultDrawableSize};
};

class MockTouchHandleDrawable : public TouchHandleDrawable {
 public:
  explicit MockTouchHandleDrawable(MockDrawableData* data) : data_(data) {}
  ~MockTouchHandleDrawable() override {}

  void SetEnabled(bool enabled) override { data_->enabled = enabled; }

  void SetOrientation(TouchHandleOrientation orientation,
                      bool mirror_vertical,
                      bool mirror_horizontal) override {
    data_->orientation = orientation;
    data_->mirror_horizontal = mirror_horizontal;
    data_->mirror_vertical = mirror_vertical;
  }

  void SetOrigin(const gfx::PointF& origin) override {
    data_->rect.set_origin(origin);
  }

  void SetAlpha(float alpha) override {
    data_->alpha = alpha;
    data_->visible = alpha > 0;
  }

  // TODO(AviD): Add unittests for non-zero values of padding ratio once the
  // code refactoring is completed.
  float GetDrawableHorizontalPaddingRatio() const override { return 0; }

  gfx::RectF GetVisibleBounds() const override { return data_->rect; }

  void SetEdge(const gfx::PointF& top, const gfx::PointF& bottom) override {
  }

 private:
  raw_ptr<MockDrawableData> data_;
};

class TouchHandleExtTest : public testing::Test, public TouchHandleClient {
 public:
  TouchHandleExtTest()
      : dragging_(false),
        dragged_(false),
        tapped_(false),
        needs_animate_(false) {}

  ~TouchHandleExtTest() override {}

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
    return std::make_unique<MockTouchHandleDrawable>(&drawable_data_);
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

TEST_F(TouchHandleExtTest, ResetPositionAfterDragEnd) {
  TouchHandleExt handle(this, TouchHandleOrientation::CENTER,
                     kDefaultViewportRect);
  handle.ResetPositionAfterDragEnd();
  handle.SetVisible(true, TouchHandle::AnimationStyle::ANIMATION_NONE);
  handle.ResetPositionAfterDragEnd();
  SetEnalbeDrawable(true);
  handle.ResetPositionAfterDragEnd();
  TouchHandleExt handle2(this, TouchHandleOrientation::CENTER, kDefaultViewportRect);
  handle2.SetVisible(false, TouchHandle::AnimationStyle::ANIMATION_NONE);
  handle2.ResetPositionAfterDragEnd();
  SetEnalbeDrawable(false);
}

TEST_F(TouchHandleExtTest, SetFocus) {
  std::shared_ptr<TouchHandle> handle = std::make_shared<TouchHandleExt>(this, TouchHandleOrientation::CENTER, kDefaultViewportRect);
  gfx::PointF focus_top(10.0f, 200.0f);
  gfx::PointF focus_bottom(10.9f, 300.0f);
  gfx::PointF focus_top2(20.0f, 200.0f);
  gfx::PointF focus_bottom2(20.9f, 300.0f);
  EXPECT_FALSE(handle->AsTouchHandleExt()->SetFocus(focus_top, focus_bottom));
  handle->SetFocus(focus_top, focus_bottom);
  handle->focus_top_= focus_bottom;
  handle->focus_bottom_= focus_top;
  EXPECT_TRUE(handle->AsTouchHandleExt()->SetFocus(focus_top, focus_bottom));
  handle->focus_top_= focus_bottom;
  handle->focus_bottom_= focus_top;
  EXPECT_TRUE(handle->AsTouchHandleExt()->SetFocus(focus_top2, focus_bottom));
  handle->focus_top_= focus_bottom;
  handle->focus_bottom_= focus_top;
  EXPECT_TRUE(handle->AsTouchHandleExt()->SetFocus(focus_top, focus_bottom2));
}
}  // namespace
}  // namespace ui