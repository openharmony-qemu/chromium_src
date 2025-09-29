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

#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/test/task_environment.h"
#define private public
#include "ui/ohos/overscroll_refresh.h"
#undef private
#include "cc/input/overscroll_behavior.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/ohos/overscroll_refresh_handler.h"

namespace ui {

const float kDipScale = 1.f;
const float kPullFrictionCoff = 0.6f;
const float kMinTriggerAnimateTwice = 1;
const int kTriggerRefreshDistance = 64;
const float kDefaultEdgeWidth =
    OverscrollRefresh::kDefaultNavigationEdgeWidth * kDipScale;

class MockOverscrollRefreshHandler : public ui::OverscrollRefreshHandler {
 public:
  MockOverscrollRefreshHandler() = default;
  ~MockOverscrollRefreshHandler() override = default;

  MOCK_METHOD(bool,
              PullToRefreshAction,
              (ui::PullToRefreshAction action),
              (override));
  MOCK_METHOD(void,
              PullToRefreshUpdate,
              (float x_delta, float y_delta),
              (override));
};

class OverscrollRefreshTest : public ::testing::Test {
 protected:
  void SetUp() override {
    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  };

  void TearDown() override {};

  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::test::TaskEnvironment task_environment_;
};

TEST_F(OverscrollRefreshTest, Destroy001) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  EXPECT_NE(effect.deceleration_animator_, nullptr);
}

TEST_F(OverscrollRefreshTest, Destroy002) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.deceleration_animator_->resetAnimate();
  effect.deceleration_animator_.reset();
  EXPECT_EQ(effect.deceleration_animator_, nullptr);
}

TEST_F(OverscrollRefreshTest, OnOverscrolled001) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::DISABLED;
  effect.OnOverscrolled(cc::OverscrollBehavior());
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::DISABLED);
}

TEST_F(OverscrollRefreshTest, OnOverscrolled002) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.cumulative_scroll_ = gfx::Vector2dF(0, 50);
  effect.scroll_begin_x_ = kDefaultEdgeWidth / 2;
  EXPECT_CALL(mock_handler,
              PullToRefreshAction(ui::PullToRefreshAction::PULL_START))
      .WillOnce(testing::Return(true));
  cc::OverscrollBehavior behavior;
  behavior.y = cc::OverscrollBehavior::Type::kAuto;
  effect.OnOverscrolled(behavior);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::ENABLED);
}

TEST_F(OverscrollRefreshTest, OnOverscrolled003) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.cumulative_scroll_ = gfx::Vector2dF(0, 50);
  effect.scroll_begin_x_ = kDefaultEdgeWidth / 2;
  EXPECT_CALL(mock_handler,
              PullToRefreshAction(ui::PullToRefreshAction::PULL_RESET))
      .WillOnce(testing::Return(true));
  cc::OverscrollBehavior behavior;
  behavior.y = cc::OverscrollBehavior::Type::kContain;
  effect.OnOverscrolled(behavior);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::DISABLED);
}

TEST_F(OverscrollRefreshTest, OnOverscrolled004) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.cumulative_scroll_ = gfx::Vector2dF(0, 50);
  effect.scroll_begin_x_ = kDefaultEdgeWidth / 2;
  EXPECT_CALL(mock_handler,
              PullToRefreshAction(ui::PullToRefreshAction::PULL_START))
      .WillOnce(testing::Return(false));
  cc::OverscrollBehavior behavior;
  behavior.y = cc::OverscrollBehavior::Type::kAuto;
  effect.OnOverscrolled(behavior);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::DISABLED);
}

TEST_F(OverscrollRefreshTest, OnOverscrolled005) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.cumulative_scroll_ = gfx::Vector2dF(-50, 10);
  effect.scroll_begin_x_ = kDefaultEdgeWidth - 1;
  effect.viewport_width_ = 1000;
  EXPECT_CALL(mock_handler,
              PullToRefreshAction(ui::PullToRefreshAction::PULL_START))
      .WillOnce(testing::Return(true));
  cc::OverscrollBehavior behavior;
  behavior.x = cc::OverscrollBehavior::Type::kAuto;
  effect.OnOverscrolled(behavior);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::ENABLED);
}

TEST_F(OverscrollRefreshTest, OnOverscrolled006) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.cumulative_scroll_ = gfx::Vector2dF(50, 10);
  effect.scroll_begin_x_ = 1000 - (kDefaultEdgeWidth - 1);
  effect.viewport_width_ = 1000;
  EXPECT_CALL(mock_handler,
              PullToRefreshAction(ui::PullToRefreshAction::PULL_START))
      .WillOnce(testing::Return(true));
  cc::OverscrollBehavior behavior;
  behavior.x = cc::OverscrollBehavior::Type::kAuto;
  effect.OnOverscrolled(behavior);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::ENABLED);
}

TEST_F(OverscrollRefreshTest, OnOverscrolled007) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.cumulative_scroll_ = gfx::Vector2dF(-50, 10);
  effect.scroll_begin_x_ = kDefaultEdgeWidth - 1;
  effect.viewport_width_ = 1000;
  EXPECT_CALL(mock_handler,
              PullToRefreshAction(ui::PullToRefreshAction::PULL_RESET))
      .WillOnce(testing::Return(true));
  cc::OverscrollBehavior behavior;
  behavior.x = cc::OverscrollBehavior::Type::kContain;
  effect.OnOverscrolled(behavior);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::DISABLED);
}

TEST_F(OverscrollRefreshTest, OnOverscrolled008) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.cumulative_scroll_ = gfx::Vector2dF(-50, 10);
  effect.scroll_begin_x_ = kDefaultEdgeWidth + 10;
  effect.viewport_width_ = 1000;
  cc::OverscrollBehavior behavior;
  behavior.x = cc::OverscrollBehavior::Type::kAuto;
  effect.OnOverscrolled(behavior);
  EXPECT_EQ(
      effect.scroll_consumption_state_,
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK);
}

TEST_F(OverscrollRefreshTest, OnOverscrolled009) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.cumulative_scroll_ = gfx::Vector2dF(30, 25);
  effect.scroll_begin_x_ = kDefaultEdgeWidth / 2;
  cc::OverscrollBehavior behavior;
  behavior.y = cc::OverscrollBehavior::Type::kAuto;
  behavior.x = cc::OverscrollBehavior::Type::kAuto;
  effect.OnOverscrolled(behavior);
  EXPECT_EQ(
      effect.scroll_consumption_state_,
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK);
}

TEST_F(OverscrollRefreshTest, OnOverscrolled010) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.cumulative_scroll_ = gfx::Vector2dF(0, -50);
  effect.scroll_begin_x_ = kDefaultEdgeWidth / 2;
  cc::OverscrollBehavior behavior;
  behavior.y = cc::OverscrollBehavior::Type::kAuto;
  effect.OnOverscrolled(behavior);
  EXPECT_EQ(
      effect.scroll_consumption_state_,
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK);
}

TEST_F(OverscrollRefreshTest, OnOverscrolled011) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.cumulative_scroll_ = gfx::Vector2dF(-50, 10);
  effect.scroll_begin_x_ = kDefaultEdgeWidth - 1;
  effect.viewport_width_ = 1000;
  EXPECT_CALL(mock_handler,
              PullToRefreshAction(ui::PullToRefreshAction::PULL_START))
      .WillOnce(testing::Return(false));
  cc::OverscrollBehavior behavior;
  behavior.x = cc::OverscrollBehavior::Type::kAuto;
  effect.OnOverscrolled(behavior);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::DISABLED);
}

TEST_F(OverscrollRefreshTest, WillHandleScrollUpdate001) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::DISABLED;
  bool result = effect.WillHandleScrollUpdate(gfx::Vector2dF(10, 20));
  EXPECT_FALSE(result);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::DISABLED);
}

TEST_F(OverscrollRefreshTest, WillHandleScrollUpdate002) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.top_at_scroll_start_ = true;
  effect.overflow_y_hidden_ = false;
  bool result = effect.WillHandleScrollUpdate(gfx::Vector2dF(20, 10));

  EXPECT_FALSE(result);
  EXPECT_EQ(
      effect.scroll_consumption_state_,
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK);
  EXPECT_EQ(effect.cumulative_scroll_.x(), 20);
  EXPECT_EQ(effect.cumulative_scroll_.y(), 10);
}

TEST_F(OverscrollRefreshTest, WillHandleScrollUpdate003) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.top_at_scroll_start_ = true;
  effect.overflow_y_hidden_ = false;
  bool result = effect.WillHandleScrollUpdate(gfx::Vector2dF(5, -10));
  EXPECT_FALSE(result);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::DISABLED);
}

TEST_F(OverscrollRefreshTest, WillHandleScrollUpdate004) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.top_at_scroll_start_ = false;
  effect.overflow_y_hidden_ = false;
  bool result = effect.WillHandleScrollUpdate(gfx::Vector2dF(5, 10));
  EXPECT_FALSE(result);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::DISABLED);
}

TEST_F(OverscrollRefreshTest, WillHandleScrollUpdate005) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.top_at_scroll_start_ = true;
  effect.overflow_y_hidden_ = true;
  bool result = effect.WillHandleScrollUpdate(gfx::Vector2dF(5, 10));
  EXPECT_FALSE(result);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::DISABLED);
}

TEST_F(OverscrollRefreshTest, WillHandleScrollUpdate006) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.top_at_scroll_start_ = true;
  effect.overflow_y_hidden_ = false;
  bool result = effect.WillHandleScrollUpdate(gfx::Vector2dF(5, 10));
  EXPECT_FALSE(result);
  EXPECT_EQ(
      effect.scroll_consumption_state_,
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK);
  EXPECT_EQ(effect.cumulative_scroll_.x(), 5);
  EXPECT_EQ(effect.cumulative_scroll_.y(), 10);
}

TEST_F(OverscrollRefreshTest, WillHandleScrollUpdate007) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::ENABLED;
  effect.pulltorefresh_scroll_ = gfx::Vector2dF(0, 0);
  EXPECT_CALL(mock_handler, PullToRefreshUpdate(5.0f * kPullFrictionCoff,
                                                10.0f * kPullFrictionCoff))
      .Times(1);
  bool result = effect.WillHandleScrollUpdate(gfx::Vector2dF(5, 10));
  EXPECT_TRUE(result);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::ENABLED);
  EXPECT_FLOAT_EQ(effect.pulltorefresh_scroll_.x(), 5.0f * kPullFrictionCoff);
  EXPECT_FLOAT_EQ(effect.pulltorefresh_scroll_.y(), 10.0f * kPullFrictionCoff);
}

TEST_F(OverscrollRefreshTest, Release001) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::AWAITING_SCROLL_UPDATE_ACK;
  effect.cumulative_scroll_ = gfx::Vector2dF(10, 20);
  effect.Release(true);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::DISABLED);
  EXPECT_EQ(effect.cumulative_scroll_.x(), 0);
  EXPECT_EQ(effect.cumulative_scroll_.y(), 0);
}

TEST_F(OverscrollRefreshTest, Release002) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::ENABLED;
  effect.pulltorefresh_scroll_ = gfx::Vector2dF(5, kTriggerRefreshDistance);
  effect.deceleration_animator_->resetAnimate();
  effect.deceleration_animator_.reset();
  EXPECT_CALL(mock_handler,
              PullToRefreshAction(ui::PullToRefreshAction::PULL_RELEASE))
      .WillOnce(testing::Return(true));
  EXPECT_CALL(mock_handler,
              PullToRefreshAction(ui::PullToRefreshAction::PULL_CANCEL))
      .WillOnce(testing::Return(true));
  effect.Release(true);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::DISABLED);
  EXPECT_EQ(effect.cumulative_scroll_.x(), 0);
  EXPECT_EQ(effect.cumulative_scroll_.y(), 0);
}

TEST_F(OverscrollRefreshTest, Release003) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::ENABLED;
  effect.pulltorefresh_scroll_ = gfx::Vector2dF(5, kTriggerRefreshDistance - 5);
  effect.deceleration_animator_->resetAnimate();
  effect.deceleration_animator_.reset();
  EXPECT_CALL(mock_handler,
              PullToRefreshAction(ui::PullToRefreshAction::PULL_RELEASE))
      .WillOnce(testing::Return(true));
  EXPECT_CALL(mock_handler,
              PullToRefreshAction(ui::PullToRefreshAction::PULL_CANCEL))
      .WillOnce(testing::Return(true));
  effect.Release(true);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::DISABLED);
  EXPECT_EQ(effect.cumulative_scroll_.x(), 0);
  EXPECT_EQ(effect.cumulative_scroll_.y(), 0);
}

TEST_F(OverscrollRefreshTest, Release004) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::ENABLED;
  effect.pulltorefresh_scroll_ = gfx::Vector2dF(5, 0);
  effect.deceleration_animator_->resetAnimate();
  effect.deceleration_animator_.reset();
  EXPECT_CALL(mock_handler,
              PullToRefreshAction(ui::PullToRefreshAction::PULL_RELEASE))
      .WillOnce(testing::Return(true));
  EXPECT_CALL(mock_handler,
              PullToRefreshAction(ui::PullToRefreshAction::PULL_CANCEL))
      .WillOnce(testing::Return(true));
  effect.Release(true);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::DISABLED);
  EXPECT_EQ(effect.cumulative_scroll_.x(), 0);
  EXPECT_EQ(effect.cumulative_scroll_.y(), 0);
}

TEST_F(OverscrollRefreshTest, StartResetAnimate001) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.pulltorefresh_scroll_ = gfx::Vector2dF(0, kMinTriggerAnimateTwice - 1);
  ASSERT_NO_FATAL_FAILURE(effect.StartResetAnimate());
}

TEST_F(OverscrollRefreshTest, StartResetAnimate002) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.pulltorefresh_scroll_ =
      gfx::Vector2dF(0, kMinTriggerAnimateTwice + 10);
  effect.deceleration_animator_->resetAnimate();
  effect.deceleration_animator_.reset();
  ASSERT_NO_FATAL_FAILURE(effect.StartResetAnimate());
}

TEST_F(OverscrollRefreshTest, AnimateReset001) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.pulltorefresh_scroll_ =
      gfx::Vector2dF(10, kMinTriggerAnimateTwice + 5);
  effect.did_stop_refresh_ = true;
  EXPECT_CALL(mock_handler,
              PullToRefreshAction(ui::PullToRefreshAction::PULL_REFRESH))
      .WillOnce(testing::Return(true));
  effect.AnimateReset(2, 3);
  EXPECT_FLOAT_EQ(effect.pulltorefresh_scroll_.x(), 8);
  EXPECT_FLOAT_EQ(effect.pulltorefresh_scroll_.y(),
                  kMinTriggerAnimateTwice + 2);
  EXPECT_FALSE(effect.did_stop_refresh_);
}

TEST_F(OverscrollRefreshTest, AnimateReset002) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.pulltorefresh_scroll_ =
      gfx::Vector2dF(10, kMinTriggerAnimateTwice - 1);
  effect.did_stop_refresh_ = true;
  effect.scroll_consumption_state_ =
      OverscrollRefresh::ScrollConsumptionState::ENABLED;
  effect.cumulative_scroll_ = gfx::Vector2dF(5, 5);
  EXPECT_CALL(mock_handler,
              PullToRefreshAction(ui::PullToRefreshAction::PULL_RESET))
      .WillOnce(testing::Return(true));
  effect.AnimateReset(2, 1);
  EXPECT_FLOAT_EQ(effect.pulltorefresh_scroll_.x(), 0);
  EXPECT_FLOAT_EQ(effect.pulltorefresh_scroll_.y(), 0);
  EXPECT_FLOAT_EQ(effect.cumulative_scroll_.x(), 0);
  EXPECT_FLOAT_EQ(effect.cumulative_scroll_.y(), 0);
  EXPECT_EQ(effect.scroll_consumption_state_,
            OverscrollRefresh::ScrollConsumptionState::DISABLED);
  EXPECT_TRUE(effect.did_stop_refresh_);
}

TEST_F(OverscrollRefreshTest, DidStopRefresh001) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.did_stop_refresh_ = false;
  effect.DidStopRefresh();
  EXPECT_TRUE(effect.did_stop_refresh_);
}
  
TEST_F(OverscrollRefreshTest, DidStopRefresh002) {
  MockOverscrollRefreshHandler mock_handler;
  OverscrollRefresh effect(&mock_handler, kDefaultEdgeWidth);
  effect.did_stop_refresh_ = true;
  ASSERT_NO_FATAL_FAILURE(effect.DidStopRefresh());
}
}  // namespace ui