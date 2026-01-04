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

#include "arkweb/chromium_ext/third_party/blink/public/mojom/page/text_recognize_result.mojom-blink.h"
#define private public
#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/web_frame_widget_impl_ext.h"
#undef private
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/widget/widget_base_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/frame/web_frame_widget_impl.h"
#include "third_party/blink/renderer/core/testing/sim/sim_request.h"
#include "third_party/blink/renderer/core/testing/sim/sim_test.h"

using testing::_;
using testing::Return;

namespace blink {
namespace {

class MockWidgetBaseUtils : public WidgetBaseUtils {
 public:
  MOCK_METHOD2(SetZoomLevel, void(float, const gfx::Point&));
  MOCK_METHOD0(GetOverScrollOffset, gfx::Vector2dF());
};

class MockWebFrameWidgetImplExt : public frame_test_helpers::TestWebFrameWidget {
 public:
  using frame_test_helpers::TestWebFrameWidget::TestWebFrameWidget;

  MOCK_METHOD1(HandleInputEvent,
               WebInputEventResult(const WebCoalescedInputEvent&));
  MOCK_METHOD0(DispatchBufferedTouchEvents, WebInputEventResult());

  MOCK_METHOD4(ObserveGestureEventAndResult,
               void(const WebGestureEvent& gesture_event,
                    const gfx::Vector2dF& unused_delta,
                    const cc::OverscrollBehavior& overscroll_behavior,
                    bool event_processed));
};

class WebFrameWidgetImplExtSimTest : public SimTest {
 public:
  frame_test_helpers::TestWebFrameWidget* CreateWebFrameWidget(
      base::PassKey<WebLocalFrame> pass_key,
      CrossVariantMojoAssociatedRemote<
          mojom::blink::FrameWidgetHostInterfaceBase> frame_widget_host,
      CrossVariantMojoAssociatedReceiver<mojom::blink::FrameWidgetInterfaceBase>
          frame_widget,
      CrossVariantMojoAssociatedRemote<mojom::blink::WidgetHostInterfaceBase>
          widget_host,
      CrossVariantMojoAssociatedReceiver<mojom::blink::WidgetInterfaceBase>
          widget,
      scoped_refptr<base::SingleThreadTaskRunner> task_runner,
      const viz::FrameSinkId& frame_sink_id,
      bool hidden,
      bool never_composited,
      bool is_for_child_local_root,
      bool is_for_nested_main_frame,
      bool is_for_scalable_page) override {
    return MakeGarbageCollected<MockWebFrameWidgetImplExt>(
        pass_key, std::move(frame_widget_host), std::move(frame_widget),
        std::move(widget_host), std::move(widget), std::move(task_runner),
        frame_sink_id, hidden, never_composited, is_for_child_local_root,
        is_for_nested_main_frame, is_for_scalable_page);
  }

  MockWebFrameWidgetImplExt* MockMainFrameWidget() {
    return static_cast<MockWebFrameWidgetImplExt*>(MainFrame().FrameWidget());
  }

  EventHandler& GetEventHandler() {
    return GetDocument().GetFrame()->GetEventHandler();
  }

  void SendInputEvent(const WebInputEvent& event,
                      WidgetBaseInputHandler::HandledEventCallback callback) {
    MockMainFrameWidget()->ProcessInputEventSynchronouslyForTesting(
        WebCoalescedInputEvent(event.Clone(), {}, {}, ui::LatencyInfo()),
        std::move(callback));
  }

  void SetWidgetBaseForTesting(std::unique_ptr<WidgetBase> widget_base) {
    MockMainFrameWidget()->widget_base_ = std::move(widget_base);
  }

  void SetUp() override {
    SimTest::SetUp();
    WebView().MainFrameViewWidget()->Resize(gfx::Size(800, 600));
    SimRequest request("https://example.com/test.html", "text/html");
    LoadURL("https://example.com/test.html");
    request.Complete(R"HTML(
      <!DOCTYPE html>
      <html>
      <head></head>
      <body>
        <div>Test content</div>
        <input type="text" id="input1" value="First input">
        <input type="text" id="input2" value="Second input">
      </body>
      </html>
    )HTML");
    Compositor().BeginFrame();
  }
};

TEST_F(WebFrameWidgetImplExtSimTest, SetZoomLevel_WithWidgetBase) {
  MockMainFrameWidget()->SetZoomLevel(1.5, gfx::Point(10, 20));
}

TEST_F(WebFrameWidgetImplExtSimTest, SetOverscrollMode_WithWidgetBase) {
  int mode = 42;
  EXPECT_TRUE(MockMainFrameWidget()->widget_base_);
  MockMainFrameWidget()->SetOverscrollMode(mode);
}

TEST_F(WebFrameWidgetImplExtSimTest, ArkWebHandleTouchEvent_RawKeyDown) {
  WebTouchEvent event(WebInputEvent::Type::kRawKeyDown, 
                         WebInputEvent::kNoModifiers,
                         WebInputEvent::GetStaticTimeStampForTests());
  MockMainFrameWidget()->ArkWebHandleTouchEvent(event);
  EXPECT_EQ(MockMainFrameWidget()->rawKeyDownTime_, 1);

  base::RunLoop run_loop;
  base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
      FROM_HERE, run_loop.QuitClosure(), base::Milliseconds(10));
  run_loop.Run();
}

TEST_F(WebFrameWidgetImplExtSimTest, ArkWebHandleTouchEvent_MouseUp) {
  WebMouseEvent event(WebInputEvent::Type::kMouseUp,
                      WebInputEvent::kNoModifiers,
                      WebInputEvent::GetStaticTimeStampForTests());

  MockMainFrameWidget()->ArkWebHandleTouchEvent(event);
  EXPECT_EQ(MockMainFrameWidget()->rawKeyDownTime_, 1);
}

TEST_F(WebFrameWidgetImplExtSimTest, ArkWebHandleTouchEvent_PointerUp) {
  WebTouchEvent touch_event;
  touch_event.SetType(WebInputEvent::Type::kTouchStart);
  touch_event.SetTimeStamp(base::TimeTicks::Now());
  touch_event.touches_length = 1;
  
  WebTouchPoint touch_point;
  touch_point.state = WebTouchPoint::State::kStatePressed;
  touch_point.SetPositionInWidget(gfx::PointF(10, 10));
  touch_point.SetPositionInScreen(gfx::PointF(10, 10));
  touch_point.radius_x = 5.0f;
  touch_point.radius_y = 5.0f;
  touch_event.touches[0] = touch_point;
  WebPointerEvent event(touch_event, touch_point);

  MockMainFrameWidget()->ArkWebHandleTouchEvent(event);
}

TEST_F(WebFrameWidgetImplExtSimTest, ArkWebHandleTouchEvent_OtherEvent) {
  WebMouseEvent event(WebInputEvent::Type::kMouseMove,
                      WebInputEvent::kNoModifiers,
                      WebInputEvent::GetStaticTimeStampForTests());

  MockMainFrameWidget()->ArkWebHandleTouchEvent(event);
  EXPECT_EQ(MockMainFrameWidget()->rawKeyDownTime_, 0);
}

TEST_F(WebFrameWidgetImplExtSimTest, SelectRangeV2_WithoutFocusedFrame) {
  MockMainFrameWidget()->SelectRangeV2ForTest(gfx::Point(1, 1), false);
}

TEST_F(WebFrameWidgetImplExtSimTest, GetVisibleRectToWeb) {
  gfx::Rect rect = MockMainFrameWidget()->GetVisibleRectToWeb();
  EXPECT_TRUE(rect.IsEmpty());
}

TEST_F(WebFrameWidgetImplExtSimTest, CreateOverlay_WithValidRect) {
  SkBitmap bitmap;
  gfx::Point touch_point(10, 10);
  
  MockMainFrameWidget()->CreateOverlay(
      bitmap, touch_point,
      base::BindRepeating([](gfx::RectF& rect) { rect = gfx::RectF(0, 0, 100, 100); }),
      base::BindRepeating([](bool) {}),
      base::BindRepeating([]() {}));
}

TEST_F(WebFrameWidgetImplExtSimTest, CreateOverlay_WithEmptyRect) {
  SkBitmap bitmap;
  gfx::Point touch_point(10, 10);
  
  MockMainFrameWidget()->CreateOverlay(
      bitmap, touch_point,
      base::BindRepeating([](gfx::RectF& rect) { rect = gfx::RectF(); }),
      base::BindRepeating([](bool) {}),
      base::BindRepeating([]() {}));
}

TEST_F(WebFrameWidgetImplExtSimTest, OnTextRecognized) {
  WTF::Vector<mojom::blink::TextRecognizeResultPtr> results;
  auto result = mojom::blink::TextRecognizeResult::New();
  result->raw_value = "Test";
  result->bounding_box = gfx::RectF(10, 10, 100, 100);
  result->corner_points = {gfx::PointF(10, 10), gfx::PointF(10, 10), gfx::PointF(10, 10), gfx::PointF(10, 10)};
  results.push_back(std::move(result));

  MockMainFrameWidget()->on_text_recognize_callback_ = 
      base::BindRepeating([](std::vector<String>, std::vector<gfx::PointF>, float) {});
  MockMainFrameWidget()->OnTextRecognizedForTest(std::move(results), 1.0f);
}

TEST_F(WebFrameWidgetImplExtSimTest, GetWordSelection_Valid) {
  WTF::String text = "Hello";
  WTF::Vector<int8_t> selection = MockMainFrameWidget()->GetWordSelection(text, 0);
  EXPECT_EQ(selection.size(), 2u);
}

TEST_F(WebFrameWidgetImplExtSimTest, GetWordSelection_FailCase) {
  auto selection = MockMainFrameWidget()->GetWordSelection("测试文本", 3);
  ASSERT_EQ(selection.size(), 2u);
  EXPECT_EQ(selection[0], -1);
  EXPECT_EQ(selection[1], -1);
}

TEST_F(WebFrameWidgetImplExtSimTest, OnTextSelected) {
  MockMainFrameWidget()->on_text_selected_callback_ = base::BindRepeating([](bool) {});
  MockMainFrameWidget()->OnTextSelectedForTest(true);
}

TEST_F(WebFrameWidgetImplExtSimTest, OnDestroyImageAnalyzerOverlay) {
  MockMainFrameWidget()->on_destroy_image_overlay_callback_ = base::BindRepeating([]() {});
  MockMainFrameWidget()->OnDestroyImageAnalyzerOverlayForTest();
}

TEST_F(WebFrameWidgetImplExtSimTest, OnDataDetectorSelectText) {
  MockMainFrameWidget()->OnDataDetectorSelectTextForTest();
}

TEST_F(WebFrameWidgetImplExtSimTest, GetImageRectInner_Valid) {
  MockMainFrameWidget()->get_rect_callback_ = 
      base::BindRepeating([](gfx::RectF& rect) { rect = gfx::RectF(10, 10, 100, 100); });
  
  gfx::Rect rect = MockMainFrameWidget()->GetImageRectInner();
  EXPECT_FALSE(rect.IsEmpty());
}

TEST_F(WebFrameWidgetImplExtSimTest, GetImageRectInner_Empty) {
  MockMainFrameWidget()->get_rect_callback_ = 
      base::BindRepeating([](gfx::RectF& rect) { rect = gfx::RectF(); });
  
  gfx::Rect rect = MockMainFrameWidget()->GetImageRectInner();
  EXPECT_TRUE(rect.IsEmpty());
}

TEST_F(WebFrameWidgetImplExtSimTest, RegisterClippedVisualViewportSelectionBounds) {
  MockMainFrameWidget()->RegisterClippedVisualViewportSelectionBounds(gfx::Rect(10, 10, 100, 100));
}

TEST_F(WebFrameWidgetImplExtSimTest, CleanFocusCache) {
  MockMainFrameWidget()->CleanFocusCache();
}

TEST_F(WebFrameWidgetImplExtSimTest, ShowFreeCopyMenu) {
  MockMainFrameWidget()->ShowFreeCopyMenuForTest();
}

TEST_F(WebFrameWidgetImplExtSimTest, DisableBoost) {
  MockMainFrameWidget()->rawKeyDownTime_ = 1;

  MockMainFrameWidget()->DisableBoost();
  EXPECT_EQ(MockMainFrameWidget()->rawKeyDownTime_, 0);

  MockMainFrameWidget()->rawKeyDownTime_ = 2;
  MockMainFrameWidget()->DisableBoost();
  EXPECT_EQ(MockMainFrameWidget()->rawKeyDownTime_, 1);
}

TEST_F(WebFrameWidgetImplExtSimTest, TouchHitTest) {
  WebTouchEvent touch_event;
  touch_event.SetType(WebInputEvent::Type::kTouchStart);
  touch_event.SetTimeStamp(base::TimeTicks::Now());
  touch_event.touches_length = 1;
  
  WebTouchPoint touch_point;
  touch_point.state = WebTouchPoint::State::kStatePressed;
  touch_point.SetPositionInWidget(gfx::PointF(10, 10));
  touch_point.SetPositionInScreen(gfx::PointF(10, 10));
  touch_point.radius_x = 5.0f;
  touch_point.radius_y = 5.0f;
  touch_event.touches[0] = touch_point;
  WebPointerEvent event(touch_event, touch_point);
  
  MockMainFrameWidget()->TouchHitTest(event, 0);
}

TEST_F(WebFrameWidgetImplExtSimTest, MouseHitTest) {
  WebMouseEvent event(WebInputEvent::Type::kMouseDown,
                      WebInputEvent::kNoModifiers,
                      WebInputEvent::GetStaticTimeStampForTests());
  
  MockMainFrameWidget()->MouseHitTest(event, 1);
}

TEST_F(WebFrameWidgetImplExtSimTest, GetInputElementAttributes) {
  HashMap<String, String> attributes;
  MockMainFrameWidget()->GetInputElementAttributes(attributes);
}

TEST_F(WebFrameWidgetImplExtSimTest, SetOverscrollMode) {
  MockMainFrameWidget()->SetOverscrollMode(1);
}

TEST_F(WebFrameWidgetImplExtSimTest, SelectRangeV2) {
  MockMainFrameWidget()->SelectRangeV2(gfx::Point(10, 10), false);
}

TEST_F(WebFrameWidgetImplExtSimTest, ReportBlank) {
  int64_t start = MockMainFrameWidget()->GetCurrentTimestampMS();
  int64_t end = start + 100;
  
  MockMainFrameWidget()->ReportBlank(start, end);
}

TEST_F(WebFrameWidgetImplExtSimTest, ReportBlank_Branches) {
  int64_t start = MockMainFrameWidget()->GetCurrentTimestampMS();
  int64_t end = start + 10;
  int64_t duration = end - start;
  int64_t kDragBlankTime = 80;
  EXPECT_FALSE(duration > kDragBlankTime);
  MockMainFrameWidget()->ReportBlank(start, end);
}

TEST_F(WebFrameWidgetImplExtSimTest, GetOverScrollOffset_ForTest) {
  auto offset = MockMainFrameWidget()->GetOverScrollOffsetForTest();
  EXPECT_EQ(offset.x(), 0);
  EXPECT_EQ(offset.y(), 0);
}

TEST_F(WebFrameWidgetImplExtSimTest, OnTextSelected_False) {
  MockMainFrameWidget()->OnTextSelectedForTest(true);
}

TEST_F(WebFrameWidgetImplExtSimTest, OnDestroyImageAnalyzerOverlay_False) {
  MockMainFrameWidget()->OnDestroyImageAnalyzerOverlayForTest();
}

TEST_F(WebFrameWidgetImplExtSimTest, ShowFreeCopyMenu_Focused) {
  auto* input_element = GetDocument().getElementById(AtomicString("input1"));
  ASSERT_TRUE(input_element);
  input_element->Focus();
  ASSERT_TRUE(GetDocument().FocusedElement());
  MockMainFrameWidget()->ShowFreeCopyMenuForTest();
}

TEST_F(WebFrameWidgetImplExtSimTest, SelectRangeV2_Focused) {
  auto* input_element = GetDocument().getElementById(AtomicString("input1"));
  ASSERT_TRUE(input_element);
  input_element->Focus();
  ASSERT_TRUE(GetDocument().FocusedElement());
  MockMainFrameWidget()->SelectRangeV2ForTest(gfx::Point(1, 1), false);
}

TEST_F(WebFrameWidgetImplExtSimTest, OnDataDetectorSelectText_Focused) {
  auto* input_element = GetDocument().getElementById(AtomicString("input1"));
  ASSERT_TRUE(input_element);
  input_element->Focus();
  ASSERT_TRUE(GetDocument().FocusedElement());
  MockMainFrameWidget()->OnDataDetectorSelectTextForTest();
}

TEST_F(WebFrameWidgetImplExtSimTest, SetZoomLevel_WithoutWidgetBase) {
  auto widget_base = std::move(MockMainFrameWidget()->widget_base_);
  SetWidgetBaseForTesting(nullptr);
  MockMainFrameWidget()->SetZoomLevel(1.5, gfx::Point(10, 20));
  SetWidgetBaseForTesting(std::move(widget_base));
}

TEST_F(WebFrameWidgetImplExtSimTest, SetOverscrollMode_WithoutWidgetBase) {
  auto widget_base = std::move(MockMainFrameWidget()->widget_base_);
  SetWidgetBaseForTesting(nullptr);
  int mode = 42;
  EXPECT_TRUE(MockMainFrameWidget()->widget_base_);
  MockMainFrameWidget()->SetOverscrollMode(mode);
  SetWidgetBaseForTesting(std::move(widget_base));
}

TEST_F(WebFrameWidgetImplExtSimTest, GetOverScrollOffset_WithoutWidgetBase) {
  auto widget_base = std::move(MockMainFrameWidget()->widget_base_);
  SetWidgetBaseForTesting(nullptr);
  auto offset = MockMainFrameWidget()->GetOverScrollOffsetForTest();
  EXPECT_EQ(offset.x(), 0);
  EXPECT_EQ(offset.y(), 0);
  SetWidgetBaseForTesting(std::move(widget_base));
}

TEST_F(WebFrameWidgetImplExtSimTest, ArkWebHandleTouchEvent_RawKeyDown1) {
  WebTouchEvent event(WebInputEvent::Type::kRawKeyDown, 
                         WebInputEvent::kNoModifiers,
                         WebInputEvent::GetStaticTimeStampForTests());
  MockMainFrameWidget()->ArkWebHandleTouchEvent(event);
  EXPECT_EQ(MockMainFrameWidget()->rawKeyDownTime_, 1);

  base::RunLoop run_loop;
  base::SingleThreadTaskRunner::GetCurrentDefault()->PostDelayedTask(
      FROM_HERE, run_loop.QuitClosure(), base::Milliseconds(10));
  run_loop.Run();
}

TEST_F(WebFrameWidgetImplExtSimTest, ArkWebHandleTouchEvent_MouseUp1) {
  WebMouseEvent event(WebInputEvent::Type::kMouseUp,
                      WebInputEvent::kNoModifiers,
                      WebInputEvent::GetStaticTimeStampForTests());

  MockMainFrameWidget()->ArkWebHandleTouchEvent(event);
  EXPECT_EQ(MockMainFrameWidget()->rawKeyDownTime_, 1);
}

TEST_F(WebFrameWidgetImplExtSimTest, ArkWebHandleTouchEvent_PointerUp1) {
  WebTouchEvent touch_event;
  touch_event.SetType(WebInputEvent::Type::kTouchStart);
  touch_event.SetTimeStamp(base::TimeTicks::Now());
  touch_event.touches_length = 1;
  
  WebTouchPoint touch_point;
  touch_point.state = WebTouchPoint::State::kStatePressed;
  touch_point.SetPositionInWidget(gfx::PointF(10, 10));
  touch_point.SetPositionInScreen(gfx::PointF(10, 10));
  touch_point.radius_x = 5.0f;
  touch_point.radius_y = 5.0f;
  touch_event.touches[0] = touch_point;
  WebPointerEvent event(touch_event, touch_point);

  MockMainFrameWidget()->ArkWebHandleTouchEvent(event);
}

TEST_F(WebFrameWidgetImplExtSimTest, ArkWebHandleTouchEvent_OtherEvent1) {
  WebMouseEvent event(WebInputEvent::Type::kMouseMove,
                      WebInputEvent::kNoModifiers,
                      WebInputEvent::GetStaticTimeStampForTests());

  MockMainFrameWidget()->ArkWebHandleTouchEvent(event);
  EXPECT_EQ(MockMainFrameWidget()->rawKeyDownTime_, 0);
}

TEST_F(WebFrameWidgetImplExtSimTest, SelectRangeV2_WithoutFocusedFrame1) {
  MockMainFrameWidget()->SelectRangeV2ForTest(gfx::Point(1, 1), false);
}

TEST_F(WebFrameWidgetImplExtSimTest, GetVisibleRectToWeb1) {
  gfx::Rect rect = MockMainFrameWidget()->GetVisibleRectToWeb();
  EXPECT_TRUE(rect.IsEmpty());
}

TEST_F(WebFrameWidgetImplExtSimTest, CreateOverlay_WithValidRect1) {
  SkBitmap bitmap;
  gfx::Point touch_point(10, 10);
  
  MockMainFrameWidget()->CreateOverlay(
      bitmap, touch_point,
      base::BindRepeating([](gfx::RectF& rect) { rect = gfx::RectF(0, 0, 100, 100); }),
      base::BindRepeating([](bool) {}),
      base::BindRepeating([]() {}));
}

TEST_F(WebFrameWidgetImplExtSimTest, CreateOverlay_WithEmptyRect1) {
  SkBitmap bitmap;
  gfx::Point touch_point(10, 10);
  
  MockMainFrameWidget()->CreateOverlay(
      bitmap, touch_point,
      base::BindRepeating([](gfx::RectF& rect) { rect = gfx::RectF(); }),
      base::BindRepeating([](bool) {}),
      base::BindRepeating([]() {}));
}

TEST_F(WebFrameWidgetImplExtSimTest, OnTextRecognized1) {
  WTF::Vector<mojom::blink::TextRecognizeResultPtr> results;
  auto result = mojom::blink::TextRecognizeResult::New();
  result->raw_value = "Test";
  result->bounding_box = gfx::RectF(10, 10, 100, 100);
  result->corner_points = {gfx::PointF(10, 10), gfx::PointF(10, 10), gfx::PointF(10, 10), gfx::PointF(10, 10)};
  results.push_back(std::move(result));

  MockMainFrameWidget()->on_text_recognize_callback_ = 
      base::BindRepeating([](std::vector<String>, std::vector<gfx::PointF>, float) {});
  MockMainFrameWidget()->OnTextRecognizedForTest(std::move(results), 1.0f);
}

TEST_F(WebFrameWidgetImplExtSimTest, GetWordSelection_Valid1) {
  WTF::String text = "Hello";
  WTF::Vector<int8_t> selection = MockMainFrameWidget()->GetWordSelection(text, 0);
  EXPECT_EQ(selection.size(), 2u);
}

TEST_F(WebFrameWidgetImplExtSimTest, GetWordSelection_FailCase1) {
  auto selection = MockMainFrameWidget()->GetWordSelection("测试文本", 3);
  ASSERT_EQ(selection.size(), 2u);
  EXPECT_EQ(selection[0], -1);
  EXPECT_EQ(selection[1], -1);
}

TEST_F(WebFrameWidgetImplExtSimTest, OnTextSelected1) {
  MockMainFrameWidget()->on_text_selected_callback_ = base::BindRepeating([](bool) {});
  MockMainFrameWidget()->OnTextSelectedForTest(true);
}

TEST_F(WebFrameWidgetImplExtSimTest, OnDestroyImageAnalyzerOverlay1) {
  MockMainFrameWidget()->on_destroy_image_overlay_callback_ = base::BindRepeating([]() {});
  MockMainFrameWidget()->OnDestroyImageAnalyzerOverlayForTest();
}

TEST_F(WebFrameWidgetImplExtSimTest, OnDataDetectorSelectText1) {
  MockMainFrameWidget()->OnDataDetectorSelectTextForTest();
}

TEST_F(WebFrameWidgetImplExtSimTest, GetImageRectInner_Valid1) {
  MockMainFrameWidget()->get_rect_callback_ = 
      base::BindRepeating([](gfx::RectF& rect) { rect = gfx::RectF(10, 10, 100, 100); });
  
  gfx::Rect rect = MockMainFrameWidget()->GetImageRectInner();
  EXPECT_FALSE(rect.IsEmpty());
}

TEST_F(WebFrameWidgetImplExtSimTest, GetImageRectInner_Empty1) {
  MockMainFrameWidget()->get_rect_callback_ = 
      base::BindRepeating([](gfx::RectF& rect) { rect = gfx::RectF(); });
  
  gfx::Rect rect = MockMainFrameWidget()->GetImageRectInner();
  EXPECT_TRUE(rect.IsEmpty());
}

TEST_F(WebFrameWidgetImplExtSimTest, RegisterClippedVisualViewportSelectionBounds1) {
  MockMainFrameWidget()->RegisterClippedVisualViewportSelectionBounds(gfx::Rect(10, 10, 100, 100));
}

TEST_F(WebFrameWidgetImplExtSimTest, CleanFocusCache1) {
  MockMainFrameWidget()->CleanFocusCache();
}

TEST_F(WebFrameWidgetImplExtSimTest, ShowFreeCopyMenu1) {
  MockMainFrameWidget()->ShowFreeCopyMenuForTest();
}

TEST_F(WebFrameWidgetImplExtSimTest, DisableBoost1) {
  MockMainFrameWidget()->rawKeyDownTime_ = 1;

  MockMainFrameWidget()->DisableBoost();
  EXPECT_EQ(MockMainFrameWidget()->rawKeyDownTime_, 0);

  MockMainFrameWidget()->rawKeyDownTime_ = 2;
  MockMainFrameWidget()->DisableBoost();
  EXPECT_EQ(MockMainFrameWidget()->rawKeyDownTime_, 1);
}

TEST_F(WebFrameWidgetImplExtSimTest, TouchHitTest1) {
  WebTouchEvent touch_event;
  touch_event.SetType(WebInputEvent::Type::kTouchStart);
  touch_event.SetTimeStamp(base::TimeTicks::Now());
  touch_event.touches_length = 1;
  
  WebTouchPoint touch_point;
  touch_point.state = WebTouchPoint::State::kStatePressed;
  touch_point.SetPositionInWidget(gfx::PointF(10, 10));
  touch_point.SetPositionInScreen(gfx::PointF(10, 10));
  touch_point.radius_x = 5.0f;
  touch_point.radius_y = 5.0f;
  touch_event.touches[0] = touch_point;
  WebPointerEvent event(touch_event, touch_point);
  
  MockMainFrameWidget()->TouchHitTest(event, 0);
}

TEST_F(WebFrameWidgetImplExtSimTest, MouseHitTest1) {
  WebMouseEvent event(WebInputEvent::Type::kMouseDown,
                      WebInputEvent::kNoModifiers,
                      WebInputEvent::GetStaticTimeStampForTests());
  
  MockMainFrameWidget()->MouseHitTest(event, 1);
}

TEST_F(WebFrameWidgetImplExtSimTest, GetInputElementAttributes1) {
  HashMap<String, String> attributes;
  MockMainFrameWidget()->GetInputElementAttributes(attributes);
}

TEST_F(WebFrameWidgetImplExtSimTest, SetOverscrollMode1) {
  MockMainFrameWidget()->SetOverscrollMode(1);
}

TEST_F(WebFrameWidgetImplExtSimTest, SelectRangeV21) {
  MockMainFrameWidget()->SelectRangeV2(gfx::Point(10, 10), false);
}

TEST_F(WebFrameWidgetImplExtSimTest, ReportBlank1) {
  int64_t start = MockMainFrameWidget()->GetCurrentTimestampMS();
  int64_t end = start + 100;
  
  MockMainFrameWidget()->ReportBlank(start, end);
}

TEST_F(WebFrameWidgetImplExtSimTest, ReportBlank_Branches1) {
  int64_t start = MockMainFrameWidget()->GetCurrentTimestampMS();
  int64_t end = start + 10;
  int64_t duration = end - start;
  int64_t kDragBlankTime = 80;
  EXPECT_FALSE(duration > kDragBlankTime);
  MockMainFrameWidget()->ReportBlank(start, end);
}

TEST_F(WebFrameWidgetImplExtSimTest, GetOverScrollOffset_ForTest1) {
  auto offset = MockMainFrameWidget()->GetOverScrollOffsetForTest();
  EXPECT_EQ(offset.x(), 0);
  EXPECT_EQ(offset.y(), 0);
}

TEST_F(WebFrameWidgetImplExtSimTest, OnTextSelected_False1) {
  MockMainFrameWidget()->OnTextSelectedForTest(true);
}

TEST_F(WebFrameWidgetImplExtSimTest, OnDestroyImageAnalyzerOverlay_False1) {
  MockMainFrameWidget()->OnDestroyImageAnalyzerOverlayForTest();
}

TEST_F(WebFrameWidgetImplExtSimTest, ShowFreeCopyMenu_Focused1) {
  auto* input_element = GetDocument().getElementById(AtomicString("input1"));
  ASSERT_TRUE(input_element);
  input_element->Focus();
  ASSERT_TRUE(GetDocument().FocusedElement());
  MockMainFrameWidget()->ShowFreeCopyMenuForTest();
}

TEST_F(WebFrameWidgetImplExtSimTest, SelectRangeV2_Focused1) {
  auto* input_element = GetDocument().getElementById(AtomicString("input1"));
  ASSERT_TRUE(input_element);
  input_element->Focus();
  ASSERT_TRUE(GetDocument().FocusedElement());
  MockMainFrameWidget()->SelectRangeV2ForTest(gfx::Point(1, 1), false);
}

TEST_F(WebFrameWidgetImplExtSimTest, OnDataDetectorSelectText_Focused1) {
  auto* input_element = GetDocument().getElementById(AtomicString("input1"));
  ASSERT_TRUE(input_element);
  input_element->Focus();
  ASSERT_TRUE(GetDocument().FocusedElement());
  MockMainFrameWidget()->OnDataDetectorSelectTextForTest();
}

TEST_F(WebFrameWidgetImplExtSimTest, SetZoomLevel_WithoutWidgetBase1) {
  auto widget_base = std::move(MockMainFrameWidget()->widget_base_);
  SetWidgetBaseForTesting(nullptr);
  MockMainFrameWidget()->SetZoomLevel(1.5, gfx::Point(10, 20));
  SetWidgetBaseForTesting(std::move(widget_base));
}

TEST_F(WebFrameWidgetImplExtSimTest, SetOverscrollMode_WithoutWidgetBase1) {
  auto widget_base = std::move(MockMainFrameWidget()->widget_base_);
  SetWidgetBaseForTesting(nullptr);
  int mode = 42;
  EXPECT_TRUE(MockMainFrameWidget()->widget_base_);
  MockMainFrameWidget()->SetOverscrollMode(mode);
  SetWidgetBaseForTesting(std::move(widget_base));
}

TEST_F(WebFrameWidgetImplExtSimTest, GetOverScrollOffset_WithoutWidgetBase1) {
  auto widget_base = std::move(MockMainFrameWidget()->widget_base_);
  SetWidgetBaseForTesting(nullptr);
  auto offset = MockMainFrameWidget()->GetOverScrollOffsetForTest();
  EXPECT_EQ(offset.x(), 0);
  EXPECT_EQ(offset.y(), 0);
  SetWidgetBaseForTesting(std::move(widget_base));
}

}  // namespace
}  // namespace blink