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

#include <cstddef>
#include <memory>

#include "content/browser/renderer_host/input/mock_input_router_client.h"
#include "content/browser/renderer_host/input/mock_input_disposition_handler.h"
#include "components/input/event_with_latency_info.h"
#if BUILDFLAG(ARKWEB_TEST)
#define private public
#include "components/input/arkweb_input_router_impl_utils.h"
#include "components/input/input_router_impl.h"
#endif
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "third_party/blink/public/common/input/web_input_event.h"

namespace input {
using namespace content;
using blink::WebInputEvent;
using blink::WebMouseEvent;
using namespace viz;
using namespace base;
using namespace base::subtle;

constexpr int ZERO_VAL = 0;
constexpr int TRACE_ID = 123;

class ArkwebInputRouterImplUtilsTest: public ::testing::Test {
public:
    void SetUp() {
        client_ = std::make_unique<MockInputRouterClient>();
        ASSERT_TRUE(client_);
        handler_ = std::make_unique<MockInputDispositionHandler>();
        ASSERT_TRUE(handler_);
        impl_ = std::make_unique<InputRouterImpl>(
            client_.get(), handler_.get(), client_.get(), config_);
        ASSERT_TRUE(impl_);
        client_->set_input_router(impl_.get());
        handler_->set_input_router(impl_.get());
        implUtils_ = std::make_shared<ArkwebInputRouterImplUtils>(impl_.get());
    }

private:
    std::shared_ptr<InputRouterImpl> impl_;
    std::shared_ptr<MockInputDispositionHandler> handler_;
    std::shared_ptr<MockInputRouterClient> client_;
    std::shared_ptr<ArkwebInputRouterImplUtils> implUtils_;
    InputRouter::Config config_;
};

TEST_F(ArkwebInputRouterImplUtilsTest, SendGestureEventEx001) {
    GestureEventWithLatencyInfo gesture(
      WebInputEvent::Type::kGestureScrollUpdate, WebInputEvent::kNoModifiers,
      base::TimeTicks() , ui::LatencyInfo());

    implUtils_->prePerfTimeStamp_ = ZERO_VAL;
    implUtils_->SendGestureEventEx(gesture);
    EXPECT_NE(implUtils_->prePerfTimeStamp_, ZERO_VAL);
}

TEST_F(ArkwebInputRouterImplUtilsTest, TracingAndSceneReport001) {
    ui::LatencyInfo latencyInfo;
    latencyInfo.set_trace_id(TRACE_ID);

    WebMouseEvent scrollEvent(WebInputEvent::Type::kGestureScrollUpdate,
                              WebInputEvent::kNoModifiers, base::TimeTicks());
    implUtils_->TracingAndSceneReport(scrollEvent, latencyInfo);

    WebMouseEvent clickEvent(WebInputEvent::Type::kMouseDown,
        WebInputEvent::kNoModifiers, base::TimeTicks());
    implUtils_->TracingAndSceneReport(clickEvent, latencyInfo);

    GestureEventWithLatencyInfo gesture(
      WebInputEvent::Type::kGestureScrollUpdate, WebInputEvent::kNoModifiers,
      base::TimeTicks() , ui::LatencyInfo());
    implUtils_->prePerfTimeStamp_ = ZERO_VAL;
    implUtils_->SendGestureEventEx(gesture);
    EXPECT_NE(implUtils_->prePerfTimeStamp_, ZERO_VAL);
}

TEST_F(ArkwebInputRouterImplUtilsTest, ProcessFilteredEvent001) {
    WebMouseEvent scrollEvent(WebInputEvent::Type::kGestureScrollUpdate,
                              WebInputEvent::kNoModifiers, base::TimeTicks());
    implUtils_->ProcessFilteredEvent(scrollEvent);

    WebMouseEvent moveEvent(WebInputEvent::Type::kTouchMove, WebInputEvent::kNoModifiers,
        base::TimeTicks());
    implUtils_->ProcessFilteredEvent(moveEvent);

    WebMouseEvent pinchEvent(WebInputEvent::Type::kGesturePinchUpdate,
        WebInputEvent::kNoModifiers, base::TimeTicks());
    implUtils_->ProcessFilteredEvent(pinchEvent);

    WebMouseEvent actionEvent(WebInputEvent::Type::kPointerCausedUaAction,
        WebInputEvent::kNoModifiers,
            base::TimeTicks());
    implUtils_->ProcessFilteredEvent(actionEvent);

    WebMouseEvent touchStartEvent(WebInputEvent::Type::kTouchStart,
        WebInputEvent::kNoModifiers, base::TimeTicks());
    implUtils_->ProcessFilteredEvent(touchStartEvent);
    ASSERT_NE(implUtils_->input_router_impl_, nullptr);
    EXPECT_EQ(implUtils_->input_router_impl_->GetNativeResult(), false);

    WebMouseEvent mouseDownEvent(WebInputEvent::Type::kMouseDown,
        WebInputEvent::kNoModifiers, base::TimeTicks());
    implUtils_->ProcessFilteredEvent(mouseDownEvent);
    EXPECT_EQ(implUtils_->input_router_impl_->GetMouseNativeResult(), false);
}

} // namespace input

#if BUILDFLAG(ARKWEB_TEST)
#undef private
#endif