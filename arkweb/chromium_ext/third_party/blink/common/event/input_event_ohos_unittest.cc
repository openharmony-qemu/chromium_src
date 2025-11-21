/* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "input_event_ohos.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/common/input/web_touch_event.h"

namespace blink {

class InputEventOhosTestForGetWebEventName
    : public ::testing::TestWithParam<
          std::tuple<WebInputEvent::Type, WebInputEvent::Modifiers, std::string>> {};

TEST_P(InputEventOhosTestForGetWebEventName, GetWebEventName) {
  auto event = WebTouchEvent();
  event.SetType(std::get<0>(GetParam()));
  event.SetModifiers(std::get<1>(GetParam()));
  EXPECT_EQ(InputEventOhos::GetWebEventName(event), std::get<2>(GetParam()));
}

TEST(InputEventOhosTest, FilterLogEvent_ExpectTrue) {
  auto event = WebTouchEvent();
  event.SetType(WebInputEvent::Type::kMouseUp);
  EXPECT_TRUE(InputEventOhos::FilterLogEvent(event));
  event.SetType(WebInputEvent::Type::kMouseDown);
  EXPECT_TRUE(InputEventOhos::FilterLogEvent(event));
  event.SetType(WebInputEvent::Type::kTouchStart);
  EXPECT_TRUE(InputEventOhos::FilterLogEvent(event));
  event.SetType(WebInputEvent::Type::kTouchEnd);
  EXPECT_TRUE(InputEventOhos::FilterLogEvent(event));
  event.SetType(WebInputEvent::Type::kTouchMove);
  EXPECT_FALSE(InputEventOhos::FilterLogEvent(event));
}

INSTANTIATE_TEST_SUITE_P(
    ModifierTests, InputEventOhosTestForGetWebEventName,
    ::testing::Values(
        std::make_tuple(WebInputEvent::Type::kMouseDown,
                        WebInputEvent::Modifiers::kLeftButtonDown,
                        "MouseDown { kLeftButtonDown }"),
        std::make_tuple(WebInputEvent::Type::kMouseDown,
                        WebInputEvent::Modifiers::kRightButtonDown,
                        "MouseDown { kRightButtonDown }"),
        std::make_tuple(WebInputEvent::Type::kMouseDown,
                        WebInputEvent::Modifiers::kMiddleButtonDown,
                        "MouseDown { kMiddleButtonDown }"),
        std::make_tuple(WebInputEvent::Type::kMouseDown,
                        WebInputEvent::Modifiers::kBackButtonDown,
                        "MouseDown { kBackButtonDown }"),
        std::make_tuple(WebInputEvent::Type::kMouseDown,
                        WebInputEvent::Modifiers::kForwardButtonDown,
                        "MouseDown { kForwardButtonDown }"),
        std::make_tuple(WebInputEvent::Type::kMouseDown,
                        WebInputEvent::Modifiers::kNoModifiers, "MouseDown {}"),
        std::make_tuple(WebInputEvent::Type::kGestureScrollBegin,
                        WebInputEvent::Modifiers::kNoModifiers, "GestureScrollBegin"),
        std::make_tuple(WebInputEvent::Type::kMouseUp,
                        WebInputEvent::Modifiers::kNoModifiers, "MouseUp"),
        std::make_tuple(WebInputEvent::Type::kMouseMove,
                        WebInputEvent::Modifiers::kNoModifiers, "MouseMove")));
}