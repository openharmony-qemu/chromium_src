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
 
#include "third_party/googletest/src/googletest/include/gtest/gtest.h"
 
namespace input {
void NoSuppressionIfDisabledCommonCase(
    MockTapSuppressionController* tap_suppression_controller) {
  // Send GestureFlingCancel Ack.
  tap_suppression_controller->NotifyGestureFlingCancelStoppedFling();
  EXPECT_EQ(MockTapSuppressionController::NONE,
            tap_suppression_controller->last_actions());
  EXPECT_EQ(MockTapSuppressionController::DISABLED,
            tap_suppression_controller->state());
 
  // Send TapDown. This TapDown should not be suppressed.
  tap_suppression_controller->SendTapDown();
  EXPECT_EQ(MockTapSuppressionController::TAP_DOWN_FORWARDED,
            tap_suppression_controller->last_actions());
  EXPECT_EQ(MockTapSuppressionController::DISABLED,
            tap_suppression_controller->state());
 
  // Send TapUp. This TapUp should not be suppressed.
  tap_suppression_controller->SendTapUp();
  EXPECT_EQ(MockTapSuppressionController::TAP_UP_FORWARDED,
            tap_suppression_controller->last_actions());
  EXPECT_EQ(MockTapSuppressionController::DISABLED,
            tap_suppression_controller->state());
}
 
TEST_F(TapSuppressionControllerTest,
       SetScrollableTrueAndNoSuppressionIfDisabled) {
  TapSuppressionController::Config disabled_config;
  disabled_config.enabled = false;
  tap_suppression_controller_ =
      std::make_unique<MockTapSuppressionController>(disabled_config);
  tap_suppression_controller_->SetScrollable(true);
  NoSuppressionIfDisabledCommonCase(tap_suppression_controller_.get());
}
 
TEST_F(TapSuppressionControllerTest,
       SetScrollableFalseAndNoSuppressionIfDisabled) {
  TapSuppressionController::Config disabled_config;
  disabled_config.enabled = false;
  tap_suppression_controller_ =
      std::make_unique<MockTapSuppressionController>(disabled_config);
  tap_suppression_controller_->SetScrollable(false);
  NoSuppressionIfDisabledCommonCase(tap_suppression_controller_.get());
}
 
TEST_F(TapSuppressionControllerTest,
       SetScrollableFlaseAndSuppressionIfEnabled) {
  TapSuppressionController::Config enabled_config;
  enabled_config.enabled = true;
  tap_suppression_controller_ =
      std::make_unique<MockTapSuppressionController>(enabled_config);
  tap_suppression_controller_->SetScrollable(false);
  NoSuppressionIfDisabledCommonCase(tap_suppression_controller_.get());
}
 
TEST_F(TapSuppressionControllerTest, SetScrollableTrueAndSuppressionIfEnabled) {
  tap_suppression_controller_->SetScrollable(true);
  // Notify the controller that the GFC has stooped an active fling.
  tap_suppression_controller_->NotifyGestureFlingCancelStoppedFling();
  EXPECT_EQ(MockTapSuppressionController::NONE,
            tap_suppression_controller_->last_actions());
  EXPECT_EQ(MockTapSuppressionController::LAST_CANCEL_STOPPED_FLING,
            tap_suppression_controller_->state());
 
  // Wait less than allowed delay between GestureFlingCancel and TapDown, so the
  // TapDown is still considered associated with the GestureFlingCancel.
  tap_suppression_controller_->AdvanceTime(base::Milliseconds(7));
  EXPECT_EQ(MockTapSuppressionController::NONE,
            tap_suppression_controller_->last_actions());
  EXPECT_EQ(MockTapSuppressionController::LAST_CANCEL_STOPPED_FLING,
            tap_suppression_controller_->state());
 
  // Send TapDown. This TapDown should be suppressed.
  tap_suppression_controller_->SendTapDown();
  EXPECT_EQ(MockTapSuppressionController::TAP_DOWN_SUPPRESSED,
            tap_suppression_controller_->last_actions());
  EXPECT_EQ(MockTapSuppressionController::SUPPRESSING_TAPS,
            tap_suppression_controller_->state());
 
  // Send TapUp. This TapUp should be suppressed.
  tap_suppression_controller_->SendTapUp();
  EXPECT_EQ(MockTapSuppressionController::TAP_UP_SUPPRESSED,
            tap_suppression_controller_->last_actions());
  EXPECT_EQ(MockTapSuppressionController::SUPPRESSING_TAPS,
            tap_suppression_controller_->state());
}
 
}  // namespace input