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
#include "content/browser/ohos/overscroll_controller_ohos.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "arkweb/chromium_ext/ui/ohos/overscroll_refresh.h"
#include "arkweb/chromium_ext/ui/ohos/overscroll_refresh_handler.h"
#include "base/command_line.h"
#include "base/time/time.h"
#include "cc/input/overscroll_behavior.h"
#include "content/public/common/content_switches.h"
#include "third_party/blink/public/common/input/web_gesture_event.h"
#include "ui/events/blink/did_overscroll_params.h"

namespace ui {
class MockOverscrollRefresh : public OverscrollRefresh {
 public:
  MockOverscrollRefresh(ui::OverscrollRefreshHandler* handler, float edge_width)
      : OverscrollRefresh(handler, edge_width) {}
  MOCK_METHOD(void,
      OnOverscrolled,
      (const cc::OverscrollBehavior&),
      (override));
  MOCK_METHOD(bool, IsAwaitingScrollUpdateAck, (), (const, override));
  MOCK_METHOD(void, Reset, (), (override));
  MOCK_METHOD(bool, IsActive, (), (const, override));
  MOCK_METHOD(void, OnScrollBegin, (const gfx::PointF&));
  MOCK_METHOD(bool, WillHandleScrollUpdate, (const gfx::Vector2dF&));
  MOCK_METHOD(void, OnScrollEnd, (const gfx::Vector2dF&));
  MOCK_METHOD(void, ReleaseWithoutActivation, ());
  MOCK_METHOD(void, DidStopRefresh, ());
  MOCK_METHOD(void,
      OnFrameUpdated,
      (const gfx::SizeF&, const gfx::PointF&, bool));
};

class MockOverscrollRefreshHandler : public OverscrollRefreshHandler {
 public:
  ~MockOverscrollRefreshHandler() override = default;
  MOCK_METHOD(bool, PullToRefreshAction, (ui::PullToRefreshAction), (override));
  MOCK_METHOD(void, PullToRefreshUpdate, (float, float), (override));
};
}  // namespace ui

namespace content {
class TestableOverscrollControllerOHOS : public OverscrollControllerOHOS {
 public:
  TestableOverscrollControllerOHOS(ui::OverscrollRefreshHandler* handler)
      : OverscrollControllerOHOS(handler) {}

  void SetRefreshEffectForTesting(
      std::unique_ptr<ui::MockOverscrollRefresh> effect) {
    mock_refresh_effect_ = effect.get();
    refresh_effect_ = std::move(effect);
  }

  bool WillHandleGestureEvent(const blink::WebGestureEvent& event) {
    if (!enabled_ || !refresh_effect_) {
      return false;
    }

    if (event.GetType() == blink::WebInputEvent::Type::kGestureScrollBegin) {
      gfx::PointF pos = event.PositionInWidget();
      mock_refresh_effect_->OnScrollBegin(pos);
      return false;
    }

    if (event.GetType() == blink::WebInputEvent::Type::kGestureScrollUpdate) {
      gfx::Vector2dF delta(event.data.scroll_update.delta_x,
          event.data.scroll_update.delta_y);
      return mock_refresh_effect_->WillHandleScrollUpdate(delta);
    }

    if (event.GetType() == blink::WebInputEvent::Type::kGestureScrollEnd) {
      mock_refresh_effect_->OnScrollEnd(gfx::Vector2dF());
      return false;
    }

    if (event.GetType() == blink::WebInputEvent::Type::kGesturePinchBegin) {
      mock_refresh_effect_->ReleaseWithoutActivation();
      return false;
    }
    if (event.GetType() == blink::WebInputEvent::Type::kGestureFlingStart) {
      if (mock_refresh_effect_->IsActive()) {
        gfx::Vector2dF scroll_velocity(event.data.fling_start.velocity_x,
            event.data.fling_start.velocity_y);
        mock_refresh_effect_->OnScrollEnd(scroll_velocity);
        const_cast<blink::WebGestureEvent&>(event).data.fling_start.velocity_x =
            0.01f;
        const_cast<blink::WebGestureEvent&>(event).data.fling_start.velocity_y =
            0.01f;
      }
      return false;
    }
    return OverscrollControllerOHOS::WillHandleGestureEvent(event);
  }

  void DidStopRefresh() {
    if (enabled_ && mock_refresh_effect_) {
      mock_refresh_effect_->DidStopRefresh();
    } else {
      OverscrollControllerOHOS::DidStopRefresh();
    }
  }

  void OnFrameMetadataUpdated(float page_scale_factor,
      float device_scale_factor,
      const gfx::SizeF& scrollable_viewport_size,
      const gfx::SizeF& root_layer_size,
      const gfx::PointF& root_scroll_offset,
      bool root_overflow_y_hidden) {
    if (!mock_refresh_effect_) {
      return;
    }
    float scale_factor = page_scale_factor;
    gfx::SizeF viewport_size =
        gfx::ScaleSize(scrollable_viewport_size, scale_factor);
    gfx::PointF content_scroll_offset =
        gfx::ScalePoint(root_scroll_offset, scale_factor);
    mock_refresh_effect_->OnFrameUpdated(viewport_size, content_scroll_offset,
        root_overflow_y_hidden);
  }

  bool IsEnabled() const { return enabled_; }
  bool HasRefreshEffect() const { return refresh_effect_ != nullptr; }
  ui::MockOverscrollRefresh* GetMockRefreshPtr() const {
    return mock_refresh_effect_;
  }
  ui::OverscrollRefresh* GetRefreshEffect() const {
    return refresh_effect_.get();
  }

  using OverscrollControllerOHOS::enabled_;
  using OverscrollControllerOHOS::OnOverscrolled;
  using OverscrollControllerOHOS::refresh_effect_;

 private:
  ui::MockOverscrollRefresh* mock_refresh_effect_ = nullptr;
};

class OverscrollControllerOHOSTest : public ::testing::Test {
 protected:
  void SetUp() override {
    mock_handler_ = new ui::MockOverscrollRefreshHandler();
    handler_ = std::unique_ptr<ui::MockOverscrollRefreshHandler>(mock_handler_);
    controller_ =
        std::make_unique<TestableOverscrollControllerOHOS>(mock_handler_);
  }

  void TearDown() override {
    controller_.reset();
    handler_.reset();
  }
  ui::MockOverscrollRefresh* SetupMockRefreshEffect() {
    const float edge_width =
        static_cast<float>(ui::OverscrollRefresh::kDefaultNavigationEdgeWidth);
    auto mock_refresh =
        std::make_unique<ui::MockOverscrollRefresh>(mock_handler_, edge_width);
    ui::MockOverscrollRefresh* mock_refresh_ptr = mock_refresh.get();
    controller_->SetRefreshEffectForTesting(std::move(mock_refresh));
    controller_->Enable();
    return mock_refresh_ptr;
  }

  ui::MockOverscrollRefreshHandler* mock_handler_;
  std::unique_ptr<ui::MockOverscrollRefreshHandler> handler_;
  std::unique_ptr<TestableOverscrollControllerOHOS> controller_;
};

TEST_F(OverscrollControllerOHOSTest, WillHandleGestureEvent_Disable) {
  controller_->Disable();
  blink::WebGestureEvent event(blink::WebInputEvent::Type::kGestureScrollUpdate,
      0, base::TimeTicks());
  EXPECT_FALSE(controller_->WillHandleGestureEvent(event));
}

TEST_F(OverscrollControllerOHOSTest, WillHandleGestureEvent_NoRefreshEffect) {
  controller_->Enable();
  blink::WebGestureEvent event(blink::WebInputEvent::Type::kGestureScrollUpdate,
      0, base::TimeTicks());
  EXPECT_FALSE(controller_->WillHandleGestureEvent(event));
}

TEST_F(OverscrollControllerOHOSTest, WillHandleGestureEvent_ScrollBegin) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, OnScrollBegin(testing::_));
  blink::WebGestureEvent event(blink::WebInputEvent::Type::kGestureScrollBegin,
      0, base::TimeTicks());
  event.SetPositionInWidget(gfx::PointF(100, 200));
  EXPECT_FALSE(controller_->WillHandleGestureEvent(event));
}

TEST_F(OverscrollControllerOHOSTest,
    WillHandleGestureEvent_Enabled_ScrollUpdate_Handled) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  blink::WebGestureEvent begin_event(
      blink::WebInputEvent::Type::kGestureScrollBegin, 0, base::TimeTicks());
  gfx::PointF begin_pos(100, 200);
  begin_event.SetPositionInWidget(begin_pos);
  EXPECT_CALL(*mock_refresh_ptr, OnScrollBegin(begin_pos))
      .WillOnce(testing::Return());
  EXPECT_FALSE(controller_->WillHandleGestureEvent(begin_event));
  testing::Mock::VerifyAndClearExpectations(mock_refresh_ptr);
  blink::WebGestureEvent update_event(
      blink::WebInputEvent::Type::kGestureScrollUpdate, 0, base::TimeTicks());
  gfx::Vector2dF scroll_delta(10.0f, 5.0f);
  update_event.data.scroll_update.delta_x = scroll_delta.x();
  update_event.data.scroll_update.delta_y = scroll_delta.y();
  EXPECT_CALL(*mock_refresh_ptr, WillHandleScrollUpdate(scroll_delta))
      .WillOnce(testing::Return(true));
  EXPECT_TRUE(controller_->WillHandleGestureEvent(update_event));
}

TEST_F(OverscrollControllerOHOSTest,
    WillHandleGestureEvent_Enabled_ScrollUpdate_NotHandled) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, WillHandleScrollUpdate(testing::_))
      .WillOnce(testing::Return(false));
  blink::WebGestureEvent event(blink::WebInputEvent::Type::kGestureScrollUpdate,
      0, base::TimeTicks());
  event.data.scroll_update.delta_x = 10.0f;
  event.data.scroll_update.delta_y = 5.0f;
  EXPECT_FALSE(controller_->WillHandleGestureEvent(event));
}

TEST_F(OverscrollControllerOHOSTest, WillHandleGestureEvent_ScrollEnd) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, OnScrollEnd(gfx::Vector2dF()))
      .WillOnce(testing::Return());
  blink::WebGestureEvent event(blink::WebInputEvent::Type::kGestureScrollEnd, 0,
      base::TimeTicks());
  EXPECT_FALSE(controller_->WillHandleGestureEvent(event));
}

TEST_F(OverscrollControllerOHOSTest,
    WillHandleGestureEvent_FlingStart_NotActive) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, IsActive()).WillOnce(testing::Return(false));
  blink::WebGestureEvent event(blink::WebInputEvent::Type::kGestureFlingStart,
      0, base::TimeTicks());
  event.data.fling_start.velocity_x = 100.0f;
  event.data.fling_start.velocity_y = 50.0f;
  EXPECT_FALSE(controller_->WillHandleGestureEvent(event));
}
TEST_F(OverscrollControllerOHOSTest, WillHandleGestureEvent_FlingStart_Active) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, IsActive()).WillOnce(testing::Return(true));
  blink::WebGestureEvent event(blink::WebInputEvent::Type::kGestureFlingStart,
      0, base::TimeTicks());
  event.data.fling_start.velocity_x = 100.0f;
  event.data.fling_start.velocity_y = 50.0f;
  gfx::Vector2dF scroll_velocity(event.data.fling_start.velocity_x,
      event.data.fling_start.velocity_y);
  EXPECT_CALL(*mock_refresh_ptr, OnScrollEnd(scroll_velocity)).Times(1);
  EXPECT_FALSE(controller_->WillHandleGestureEvent(event));
}

TEST_F(OverscrollControllerOHOSTest, WillHandleGestureEvent_PinchBegin) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, ReleaseWithoutActivation());
  blink::WebGestureEvent event(blink::WebInputEvent::Type::kGesturePinchBegin,
      0, base::TimeTicks());
  EXPECT_FALSE(controller_->WillHandleGestureEvent(event));
}

TEST_F(OverscrollControllerOHOSTest, WillHandleGestureEvent_OtherGesture) {
  controller_->Enable();
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  blink::WebGestureEvent event(blink::WebInputEvent::Type::kGestureTap, 0,
      base::TimeTicks());
  EXPECT_FALSE(controller_->WillHandleGestureEvent(event));
}

TEST_F(OverscrollControllerOHOSTest, DidStopRefresh_EnabledWithRefreshEffect) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, DidStopRefresh()).Times(1);
  EXPECT_NO_FATAL_FAILURE(controller_->DidStopRefresh());
}

TEST_F(OverscrollControllerOHOSTest,
    DidStopRefresh_EnabledWithoutRefreshEffect) {
  controller_->Enable();
  EXPECT_NO_FATAL_FAILURE(controller_->DidStopRefresh());
}

TEST_F(OverscrollControllerOHOSTest, DidStopRefresh_DisabledWithRefreshEffect) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_NO_FATAL_FAILURE(controller_->Disable());
  EXPECT_CALL(*mock_refresh_ptr, DidStopRefresh()).Times(0);
  EXPECT_NO_FATAL_FAILURE(controller_->DidStopRefresh());
}

TEST_F(OverscrollControllerOHOSTest,
    DidStopRefresh_DisabledWithoutRefreshEffect) {
  controller_->Disable();
  EXPECT_NO_FATAL_FAILURE(controller_->DidStopRefresh());
}

TEST_F(OverscrollControllerOHOSTest, DidStopRefresh_MultipleCalls) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, DidStopRefresh()).Times(2);
  EXPECT_NO_FATAL_FAILURE(controller_->DidStopRefresh());
  EXPECT_NO_FATAL_FAILURE(controller_->DidStopRefresh());
}

TEST_F(OverscrollControllerOHOSTest, OnOverscrolled_Disabled) {
  controller_->Disable();
  ui::DidOverscrollParams params;
  cc::OverscrollBehavior behavior;
  behavior.x = cc::OverscrollBehavior::Type::kAuto;
  behavior.y = cc::OverscrollBehavior::Type::kAuto;
  params.overscroll_behavior = behavior;
  EXPECT_NO_FATAL_FAILURE(controller_->OnOverscrolled(params));
}

TEST_F(OverscrollControllerOHOSTest,
    OnOverscrolled_EnabledWithoutRefreshEffect) {
  controller_->Enable();
  ui::DidOverscrollParams params;
  cc::OverscrollBehavior behavior;
  behavior.x = cc::OverscrollBehavior::Type::kAuto;
  behavior.y = cc::OverscrollBehavior::Type::kAuto;
  params.overscroll_behavior = behavior;
  EXPECT_NO_FATAL_FAILURE(controller_->OnOverscrolled(params));
}

TEST_F(OverscrollControllerOHOSTest,
    OnOverscrolled_EnabledWithRefreshEffect_Active) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, IsActive()).WillOnce(testing::Return(true));
  EXPECT_CALL(*mock_refresh_ptr, OnOverscrolled(testing::_)).Times(1);
  ui::DidOverscrollParams params;
  cc::OverscrollBehavior behavior;
  behavior.x = cc::OverscrollBehavior::Type::kAuto;
  behavior.y = cc::OverscrollBehavior::Type::kAuto;
  params.overscroll_behavior = behavior;
  EXPECT_NO_FATAL_FAILURE(controller_->OnOverscrolled(params));
}

TEST_F(OverscrollControllerOHOSTest,
    OnOverscrolled_EnabledWithRefreshEffect_AwaitingAck) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, IsActive()).WillOnce(testing::Return(false));
  EXPECT_CALL(*mock_refresh_ptr, IsAwaitingScrollUpdateAck())
      .WillOnce(testing::Return(true));
  EXPECT_CALL(*mock_refresh_ptr, OnOverscrolled(testing::_)).Times(1);
  ui::DidOverscrollParams params;
  cc::OverscrollBehavior behavior;
  behavior.x = cc::OverscrollBehavior::Type::kAuto;
  behavior.y = cc::OverscrollBehavior::Type::kAuto;
  params.overscroll_behavior = behavior;
  EXPECT_NO_FATAL_FAILURE(controller_->OnOverscrolled(params));
}

TEST_F(OverscrollControllerOHOSTest,
    OnOverscrolled_EnabledWithRefreshEffect_NotActive_ProcessBehavior) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, IsActive()).WillOnce(testing::Return(false));
  EXPECT_CALL(*mock_refresh_ptr, IsAwaitingScrollUpdateAck())
      .WillOnce(testing::Return(false));
  EXPECT_CALL(*mock_refresh_ptr, OnOverscrolled(testing::_)).Times(1);
  ui::DidOverscrollParams params;
  cc::OverscrollBehavior behavior;
  behavior.x = cc::OverscrollBehavior::Type::kAuto;
  behavior.y = cc::OverscrollBehavior::Type::kAuto;
  params.overscroll_behavior = behavior;
  params.accumulated_overscroll = gfx::Vector2dF(10.0f, 20.0f);
  params.latest_overscroll_delta = gfx::Vector2dF(1.0f, 2.0f);
  params.current_fling_velocity = gfx::Vector2dF(5.0f, 10.0f);
  EXPECT_NO_FATAL_FAILURE(controller_->OnOverscrolled(params));
}

TEST_F(OverscrollControllerOHOSTest, OnOverscrolled_BehaviorXNone) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, IsActive()).WillOnce(testing::Return(false));
  EXPECT_CALL(*mock_refresh_ptr, IsAwaitingScrollUpdateAck())
      .WillOnce(testing::Return(false));
  EXPECT_CALL(*mock_refresh_ptr, OnOverscrolled(testing::_)).Times(1);
  ui::DidOverscrollParams params;
  cc::OverscrollBehavior behavior;
  behavior.x = cc::OverscrollBehavior::Type::kNone;
  behavior.y = cc::OverscrollBehavior::Type::kAuto;
  params.overscroll_behavior = behavior;
  params.accumulated_overscroll = gfx::Vector2dF(10.0f, 20.0f);
  params.latest_overscroll_delta = gfx::Vector2dF(1.0f, 2.0f);
  params.current_fling_velocity = gfx::Vector2dF(5.0f, 10.0f);
  EXPECT_NO_FATAL_FAILURE(controller_->OnOverscrolled(params));
}

TEST_F(OverscrollControllerOHOSTest, OnOverscrolled_BehaviorYNone) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, IsActive()).WillOnce(testing::Return(false));
  EXPECT_CALL(*mock_refresh_ptr, IsAwaitingScrollUpdateAck())
      .WillOnce(testing::Return(false));
  EXPECT_CALL(*mock_refresh_ptr, OnOverscrolled(testing::_)).Times(1);
  ui::DidOverscrollParams params;
  cc::OverscrollBehavior behavior;
  behavior.x = cc::OverscrollBehavior::Type::kAuto;
  behavior.y = cc::OverscrollBehavior::Type::kNone;
  params.overscroll_behavior = behavior;
  params.accumulated_overscroll = gfx::Vector2dF(10.0f, 20.0f);
  params.latest_overscroll_delta = gfx::Vector2dF(1.0f, 2.0f);
  params.current_fling_velocity = gfx::Vector2dF(5.0f, 10.0f);
  EXPECT_NO_FATAL_FAILURE(controller_->OnOverscrolled(params));
}

TEST_F(OverscrollControllerOHOSTest, OnOverscrolled_BothBehaviorNone) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, IsActive()).WillOnce(testing::Return(false));
  EXPECT_CALL(*mock_refresh_ptr, IsAwaitingScrollUpdateAck())
      .WillOnce(testing::Return(false));
  EXPECT_CALL(*mock_refresh_ptr, OnOverscrolled(testing::_)).Times(1);
  ui::DidOverscrollParams params;
  cc::OverscrollBehavior behavior;
  behavior.x = cc::OverscrollBehavior::Type::kNone;
  behavior.y = cc::OverscrollBehavior::Type::kNone;
  params.overscroll_behavior = behavior;
  params.accumulated_overscroll = gfx::Vector2dF(10.0f, 20.0f);
  params.latest_overscroll_delta = gfx::Vector2dF(1.0f, 2.0f);
  params.current_fling_velocity = gfx::Vector2dF(5.0f, 10.0f);
  EXPECT_NO_FATAL_FAILURE(controller_->OnOverscrolled(params));
}

TEST_F(OverscrollControllerOHOSTest, Disable_WhenAlreadyDisabled) {
  EXPECT_TRUE(controller_->IsEnabled());
  EXPECT_NO_FATAL_FAILURE(controller_->Disable());
  EXPECT_FALSE(controller_->IsEnabled());
  EXPECT_NO_FATAL_FAILURE(controller_->Disable());
  EXPECT_FALSE(controller_->IsEnabled());
}

TEST_F(OverscrollControllerOHOSTest, Disable_WhenEnabledWithoutRefreshEffect) {
  EXPECT_TRUE(controller_->IsEnabled());
  EXPECT_FALSE(controller_->HasRefreshEffect());
  EXPECT_NO_FATAL_FAILURE(controller_->Disable());
  EXPECT_FALSE(controller_->IsEnabled());
}

TEST_F(OverscrollControllerOHOSTest, Disable_WhenEnabledWithRefreshEffect) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_TRUE(controller_->IsEnabled());
  EXPECT_TRUE(controller_->HasRefreshEffect());
  EXPECT_CALL(*mock_refresh_ptr, Reset()).Times(1);
  EXPECT_NO_FATAL_FAILURE(controller_->Disable());
  EXPECT_FALSE(controller_->IsEnabled());
}

TEST_F(OverscrollControllerOHOSTest, Disable_ToggleEnableDisable) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_TRUE(controller_->IsEnabled());
  EXPECT_CALL(*mock_refresh_ptr, Reset()).Times(1);
  EXPECT_NO_FATAL_FAILURE(controller_->Disable());
  EXPECT_FALSE(controller_->IsEnabled());
  controller_->Enable();
  EXPECT_TRUE(controller_->IsEnabled());
  testing::Mock::VerifyAndClearExpectations(mock_refresh_ptr);
  EXPECT_CALL(*mock_refresh_ptr, Reset()).Times(1);
  EXPECT_NO_FATAL_FAILURE(controller_->Disable());
  EXPECT_FALSE(controller_->IsEnabled());
}

TEST_F(OverscrollControllerOHOSTest, Disable_ChecksEnabledFlagBeforeReset) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_TRUE(controller_->IsEnabled());
  EXPECT_CALL(*mock_refresh_ptr, Reset()).Times(1);
  EXPECT_NO_FATAL_FAILURE(controller_->Disable());
  EXPECT_FALSE(controller_->IsEnabled());
  testing::Mock::VerifyAndClearExpectations(mock_refresh_ptr);
  EXPECT_CALL(*mock_refresh_ptr, Reset()).Times(0);
  EXPECT_NO_FATAL_FAILURE(controller_->Disable());
  EXPECT_FALSE(controller_->IsEnabled());
}

TEST_F(OverscrollControllerOHOSTest, Disable_WithNullRefreshEffect) {
  auto controller2 =
      std::make_unique<TestableOverscrollControllerOHOS>(mock_handler_);
  EXPECT_NO_FATAL_FAILURE(controller2->Enable());
  EXPECT_TRUE(controller2->IsEnabled());
  EXPECT_NO_FATAL_FAILURE(controller2->Disable());
  EXPECT_FALSE(controller2->IsEnabled());
}

TEST_F(OverscrollControllerOHOSTest, OnGestureEventAck_Disabled) {
  controller_->Disable();
  EXPECT_FALSE(controller_->IsEnabled());
  blink::WebGestureEvent scroll_end_event(
      blink::WebInputEvent::Type::kGestureScrollEnd, 0, base::TimeTicks());
  EXPECT_NO_FATAL_FAILURE(controller_->OnGestureEventAck(
      scroll_end_event, blink::mojom::InputEventResultState::kConsumed));
}

TEST_F(OverscrollControllerOHOSTest,
    OnGestureEventAck_GestureScrollEnd_Enabled) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_TRUE(controller_->IsEnabled());
  EXPECT_TRUE(controller_->HasRefreshEffect());
  blink::WebGestureEvent scroll_end_event(
      blink::WebInputEvent::Type::kGestureScrollEnd, 0, base::TimeTicks());
  EXPECT_CALL(*mock_refresh_ptr, OnOverscrolled(testing::_)).Times(1);
  EXPECT_NO_FATAL_FAILURE(controller_->OnGestureEventAck(
      scroll_end_event, blink::mojom::InputEventResultState::kConsumed));
}

TEST_F(OverscrollControllerOHOSTest,
    OnGestureEventAck_GestureFlingStart_Enabled) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_TRUE(controller_->IsEnabled());
  EXPECT_TRUE(controller_->HasRefreshEffect());
  blink::WebGestureEvent fling_start_event(
      blink::WebInputEvent::Type::kGestureFlingStart, 0, base::TimeTicks());
  EXPECT_CALL(*mock_refresh_ptr, OnOverscrolled(testing::_)).Times(1);
  EXPECT_NO_FATAL_FAILURE(controller_->OnGestureEventAck(
      fling_start_event, blink::mojom::InputEventResultState::kConsumed));
}

TEST_F(OverscrollControllerOHOSTest,
    OnGestureEventAck_GestureScrollUpdate_NotAwaitingAck) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, IsAwaitingScrollUpdateAck())
      .WillOnce(testing::Return(false));
  blink::WebGestureEvent scroll_update_event(
      blink::WebInputEvent::Type::kGestureScrollUpdate, 0, base::TimeTicks());
  EXPECT_NO_FATAL_FAILURE(controller_->OnGestureEventAck(
      scroll_update_event, blink::mojom::InputEventResultState::kConsumed));
}

TEST_F(OverscrollControllerOHOSTest,
    OnGestureEventAck_GestureScrollUpdate_AwaitingAck_Consumed) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, IsAwaitingScrollUpdateAck())
      .WillOnce(testing::Return(true));
  EXPECT_CALL(*mock_refresh_ptr, Reset()).Times(1);
  blink::WebGestureEvent scroll_update_event(
      blink::WebInputEvent::Type::kGestureScrollUpdate, 0, base::TimeTicks());
  EXPECT_NO_FATAL_FAILURE(controller_->OnGestureEventAck(
      scroll_update_event, blink::mojom::InputEventResultState::kConsumed));
}

TEST_F(OverscrollControllerOHOSTest,
    OnGestureEventAck_GestureScrollUpdate_AwaitingAck_NotConsumed) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, IsAwaitingScrollUpdateAck())
      .WillOnce(testing::Return(true));
  blink::WebGestureEvent scroll_update_event(
      blink::WebInputEvent::Type::kGestureScrollUpdate, 0, base::TimeTicks());
  EXPECT_NO_FATAL_FAILURE(controller_->OnGestureEventAck(
      scroll_update_event, blink::mojom::InputEventResultState::kNotConsumed));
}

TEST_F(OverscrollControllerOHOSTest, OnGestureEventAck_OtherGestureEvent) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_CALL(*mock_refresh_ptr, IsAwaitingScrollUpdateAck()).Times(0);
  blink::WebGestureEvent tap_event(blink::WebInputEvent::Type::kGestureTap, 0,
      base::TimeTicks());
  EXPECT_NO_FATAL_FAILURE(controller_->OnGestureEventAck(
      tap_event, blink::mojom::InputEventResultState::kConsumed));
}

TEST_F(OverscrollControllerOHOSTest,
    OnGestureEventAck_EnabledWithoutRefreshEffect) {
  controller_->Enable();
  EXPECT_TRUE(controller_->IsEnabled());
  EXPECT_FALSE(controller_->HasRefreshEffect());
  blink::WebGestureEvent scroll_end_event(
      blink::WebInputEvent::Type::kGestureScrollEnd, 0, base::TimeTicks());
  EXPECT_NO_FATAL_FAILURE(controller_->OnGestureEventAck(
      scroll_end_event, blink::mojom::InputEventResultState::kConsumed));
}

TEST_F(OverscrollControllerOHOSTest,
    OnGestureEventAck_CompleteSequence_ScrollUpdate) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  blink::WebGestureEvent scroll_update_event(
      blink::WebInputEvent::Type::kGestureScrollUpdate, 0, base::TimeTicks());
  EXPECT_CALL(*mock_refresh_ptr, IsAwaitingScrollUpdateAck())
      .WillOnce(testing::Return(true));
  EXPECT_CALL(*mock_refresh_ptr, Reset()).Times(1);
  EXPECT_NO_FATAL_FAILURE(controller_->OnGestureEventAck(
      scroll_update_event, blink::mojom::InputEventResultState::kConsumed));
  testing::Mock::VerifyAndClearExpectations(mock_refresh_ptr);
  EXPECT_CALL(*mock_refresh_ptr, IsAwaitingScrollUpdateAck())
      .WillOnce(testing::Return(true));
  EXPECT_NO_FATAL_FAILURE(controller_->OnGestureEventAck(
      scroll_update_event, blink::mojom::InputEventResultState::kNotConsumed));
}

TEST_F(OverscrollControllerOHOSTest,
    OnFrameMetadataUpdated_WithoutRefreshEffect) {
  EXPECT_FALSE(controller_->HasRefreshEffect());
  EXPECT_NO_FATAL_FAILURE(controller_->OnFrameMetadataUpdated(
      1.0f, 2.0f, gfx::SizeF(100.0f, 200.0f), gfx::SizeF(300.0f, 400.0f),
      gfx::PointF(10.0f, 20.0f), false));
}

TEST_F(OverscrollControllerOHOSTest, OnFrameMetadataUpdated_WithRefreshEffect) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_TRUE(controller_->HasRefreshEffect());
  EXPECT_CALL(*mock_refresh_ptr,
      OnFrameUpdated(gfx::SizeF(100.0f, 200.0f),
          gfx::PointF(10.0f, 20.0f), false))
      .Times(1);
  EXPECT_NO_FATAL_FAILURE(controller_->OnFrameMetadataUpdated(
      1.0f, 2.0f, gfx::SizeF(100.0f, 200.0f), gfx::SizeF(300.0f, 400.0f),
      gfx::PointF(10.0f, 20.0f), false));
}

TEST_F(OverscrollControllerOHOSTest, OnFrameMetadataUpdated_WithScaledValues) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_TRUE(controller_->HasRefreshEffect());
  EXPECT_CALL(*mock_refresh_ptr,
      OnFrameUpdated(gfx::SizeF(200.0f, 400.0f),
          gfx::PointF(20.0f, 40.0f), true))
      .Times(1);
  EXPECT_NO_FATAL_FAILURE(controller_->OnFrameMetadataUpdated(
      2.0f, 1.5f, gfx::SizeF(100.0f, 200.0f), gfx::SizeF(300.0f, 400.0f),
      gfx::PointF(10.0f, 20.0f), true));
}

TEST_F(OverscrollControllerOHOSTest, OnFrameMetadataUpdated_ZeroScaleFactor) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_TRUE(controller_->HasRefreshEffect());
  EXPECT_CALL(*mock_refresh_ptr, OnFrameUpdated(gfx::SizeF(0.0f, 0.0f),
      gfx::PointF(0.0f, 0.0f), false))
      .Times(1);
  EXPECT_NO_FATAL_FAILURE(controller_->OnFrameMetadataUpdated(
      0.0f, 1.0f, gfx::SizeF(100.0f, 200.0f), gfx::SizeF(300.0f, 400.0f),
      gfx::PointF(10.0f, 20.0f), false));
}

TEST_F(OverscrollControllerOHOSTest,
    OnFrameMetadataUpdated_VerifyParametersPassedCorrectly) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_TRUE(controller_->HasRefreshEffect());
  float page_scale_factor = 1.5f;
  gfx::SizeF scrollable_viewport_size(800.0f, 600.0f);
  gfx::PointF root_scroll_offset(50.0f, 100.0f);
  bool root_overflow_y_hidden = true;
  gfx::SizeF expected_viewport_size =
      gfx::ScaleSize(scrollable_viewport_size, page_scale_factor);
  gfx::PointF expected_content_scroll_offset =
      gfx::ScalePoint(root_scroll_offset, page_scale_factor);
  EXPECT_CALL(*mock_refresh_ptr, OnFrameUpdated(expected_viewport_size,
      expected_content_scroll_offset,
      root_overflow_y_hidden))
      .Times(1);
  EXPECT_NO_FATAL_FAILURE(controller_->OnFrameMetadataUpdated(
      page_scale_factor, 2.0f, scrollable_viewport_size,
      gfx::SizeF(1000.0f, 800.0f), root_scroll_offset, root_overflow_y_hidden));
}

TEST_F(OverscrollControllerOHOSTest, OnFrameMetadataUpdated_MultipleCalls) {
  ui::MockOverscrollRefresh* mock_refresh_ptr = SetupMockRefreshEffect();
  EXPECT_TRUE(controller_->HasRefreshEffect());
  EXPECT_CALL(*mock_refresh_ptr,
      OnFrameUpdated(gfx::SizeF(100.0f, 200.0f),
          gfx::PointF(10.0f, 20.0f), false))
      .Times(1);
  EXPECT_NO_FATAL_FAILURE(controller_->OnFrameMetadataUpdated(
      1.0f, 1.0f, gfx::SizeF(100.0f, 200.0f), gfx::SizeF(300.0f, 400.0f),
      gfx::PointF(10.0f, 20.0f), false));
  testing::Mock::VerifyAndClearExpectations(mock_refresh_ptr);
  EXPECT_CALL(*mock_refresh_ptr,
      OnFrameUpdated(gfx::SizeF(300.0f, 600.0f),
          gfx::PointF(30.0f, 60.0f), true))
      .Times(1);
  EXPECT_NO_FATAL_FAILURE(controller_->OnFrameMetadataUpdated(
      3.0f, 2.0f, gfx::SizeF(100.0f, 200.0f), gfx::SizeF(500.0f, 600.0f),
      gfx::PointF(10.0f, 20.0f), true));
}

TEST_F(OverscrollControllerOHOSTest, CreateRefreshEffect_WithoutSwitch) {
  base::CommandLine::ForCurrentProcess()->RemoveSwitch(
      switches::kEnableNwebExPullToRefresh);
  auto controller = std::make_unique<OverscrollControllerOHOS>(mock_handler_);
  ASSERT_NE(controller, nullptr);
}

TEST_F(OverscrollControllerOHOSTest, CreateRefreshEffect_WithSwitch) {
  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      switches::kEnableNwebExPullToRefresh);
  auto controller = std::make_unique<OverscrollControllerOHOS>(mock_handler_);
  ASSERT_NE(controller, nullptr);
  base::CommandLine::ForCurrentProcess()->RemoveSwitch(
      switches::kEnableNwebExPullToRefresh);
}
}  // namespace content