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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/input/mouse_event_manager_ext.h"
#include "third_party/blink/renderer/core/dom/tree_scope.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/dom/shadow_root.h"
#include "third_party/blink/renderer/core/testing/null_execution_context.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/testing/task_environment.h"

namespace blink {

namespace {
constexpr char BASE64_RED_IMAGE_DATA_100_100[] =
    "iVBORw0KGgoAAAANSUhEUgAAAGQAAABkCAIAAAD/"
    "gAIDAAAA40lEQVR4nO3QsQEAIAyAsOr/"
    "P+"
    "sLZU9mJs4btu66xKzCrMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCs"
    "wKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArM"
    "CswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzA"
    "rMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArNn7il"
    "4Bx2GaB88AAAAASUVORK5CYII=";

constexpr char BASE64_GREEN_IMAGE_DATA_100_100[] =
    "iVBORw0KGgoAAAANSUhEUgAAAGQAAABkCAIAAAD/"
    "gAIDAAAA5ElEQVR4nO3QsQEAIAyAsOr/"
    "P+"
    "sLZU9mJs68YeluQ8xKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArM"
    "CswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzA"
    "rMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswK"
    "zArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzJq9D4"
    "lfAcdz9KK2AAAAAElFTkSuQmCC";

constexpr char BASE64_RED_IMAGE_DATA_99_100[] =
    "iVBORw0KGgoAAAANSUhEUgAAAGMAAABkCAIAAAAdXBl6AAAA40lEQVR4nO3QMQEAMAyAsLb+"
    "PW8WIoDcXOybkLMsnWKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnV"
    "KdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKd"
    "Up1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp"
    "1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdUp1SnVKdGvQB6RABx5IOIWwAAAAA"
    "SUVORK5CYII=";

constexpr char BASE64_RED_IMAGE_DATA_100_99[] =
    "iVBORw0KGgoAAAANSUhEUgAAAGQAAABjCAIAAADihTK7AAAA4UlEQVR4nO3QsQEAIAyAsOr/"
    "P+"
    "sLZU9mJs4btu66xKzCrMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzA"
    "rMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArM"
    "CswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCs"
    "wKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzArMCswKzZ+"
    "3NFAcW1S1gAAAAAAElFTkSuQmCC";

constexpr int POINT_VAL_A = 100;
constexpr int POINT_VAL_B = 700;
constexpr int POINT_VAL_C = 0;

class CreateOverlayEventBuilder : public WebGestureEvent {
 public:
  explicit CreateOverlayEventBuilder(gfx::PointF position)
      : WebGestureEvent(WebInputEvent::Type::kGestureCreateOverlay,
                        WebInputEvent::kNoModifiers,
                        base::TimeTicks::Now(),
                        WebGestureDevice::kTouchscreen) {
    SetPositionInWidget(position);
    SetPositionInScreen(position);
    data.long_press.width = 5;
    data.long_press.height = 5;
    frame_scale_ = 1;
  }
};

class ShortPressEventBuilder : public WebGestureEvent {
 public:
  explicit ShortPressEventBuilder(gfx::PointF position)
      : WebGestureEvent(WebInputEvent::Type::kGestureShortPress,
                        WebInputEvent::kNoModifiers,
                        base::TimeTicks::Now(),
                        WebGestureDevice::kTouchscreen) {
    SetPositionInWidget(position);
    SetPositionInScreen(position);
    data.long_press.width = 5;
    data.long_press.height = 5;
    frame_scale_ = 1;
  }
};

class DragLongPressEventBuilder : public WebGestureEvent {
 public:
  explicit DragLongPressEventBuilder(gfx::PointF position)
      : WebGestureEvent(WebInputEvent::Type::kGestureDragLongPress,
                        WebInputEvent::kNoModifiers,
                        base::TimeTicks::Now(),
                        WebGestureDevice::kTouchscreen) {
    SetPositionInWidget(position);
    SetPositionInScreen(position);
    data.long_press.width = 5;
    data.long_press.height = 5;
    frame_scale_ = 1;
  }
};
}  // namespace

class MouseEventManagerExtTest : public SimTestExt {
 protected:
  MouseEventManagerExt& GetMouseEventManagerExt() {
    return *GetMouseEventManager().AsMouseEventManagerExt();
  }

  WebMouseEvent CreateTestMouseEvent(WebInputEvent::Type type,
                                     const gfx::PointF& coordinates) {
    WebMouseEvent event(type, coordinates, coordinates,
                        WebPointerProperties::Button::kLeft, 0,
                        WebInputEvent::kLeftButtonDown,
                        WebInputEvent::GetStaticTimeStampForTests());
    event.SetFrameScale(1);
    return event;
  }

  bool& GetOverlayCreatingRef() {
    return GetMouseEventManagerExt().overlay_creating_;
  }

  bool& GetOverlayInProgressRef() {
    return GetMouseEventManagerExt().overlay_in_progress_;
  }

  WebInputEventResult CreateOverlayOnPoint(gfx::PointF position) {
    CreateOverlayEventBuilder create_overlay_event(position);
    return GetEventHandler().HandleGestureEvent(create_overlay_event);
  }

  void CreateOverlayMouseOnPoint(gfx::PointF position) {
    GetMouseEventManagerExt().last_mouse_drag_ =
        CreateTestMouseEvent(WebInputEvent::Type::kMouseDown, position);
    GetMouseEventManagerExt().CreateOverlayCallback();
  }

  const HitTestResult& GetHitTestResultForTest(gfx::PointF position) {
    CreateOverlayEventBuilder create_overlay_event(position);
    GestureEventWithHitTestResults targeted_event =
        GetEventHandler().TargetGestureEvent(create_overlay_event);
    return targeted_event.GetHitTestResult();
  }

  void SetHitImageNodeForTest(Node* node) {
    GetMouseEventManagerExt().hit_image_node_ = node;
  }

  // 200x600 red 100x100 green 100x100
  void SetUpHtmlDefault() {
    WebView().MainFrameViewWidget()->Resize(gfx::Size(200, 600));
    SimRequest request("https://example.com/test.html", "text/html");
    LoadURL("https://example.com/test.html");

    request.Complete(R"HTML(
      <!DOCTYPE html>
      <head></head>
      <body>
        <img src="data:image/png;base64,)HTML" +
                     String(BASE64_RED_IMAGE_DATA_100_100) +
                     R"HTML(" width="100%" height="auto">
        <img src="data:image/png;base64,)HTML" +
                     String(BASE64_GREEN_IMAGE_DATA_100_100) +
                     R"HTML(" width="100%" height="auto">
      </body>
    )HTML");
    Compositor().BeginFrame();
  }

  void SetUpHtmlSelectNone() {
    WebView().MainFrameViewWidget()->Resize(gfx::Size(200, 600));
    SimRequest request("https://example.com/test.html", "text/html");
    LoadURL("https://example.com/test.html");
    request.Complete(R"HTML(
      <!DOCTYPE html>
      <head></head>
      <body>
        <img src="data:image/png;base64,)HTML" +
        String(BASE64_RED_IMAGE_DATA_100_100) +
        R"HTML(" width="100%" height="auto" style="user-select: none;">
      </body>
    )HTML");
    Compositor().BeginFrame();
  }

  void SetUpHtmlSmallImage() {
    WebView().MainFrameViewWidget()->Resize(gfx::Size(200, 600));
    SimRequest request("https://example.com/test.html", "text/html");
    LoadURL("https://example.com/test.html");

    request.Complete(R"HTML(
      <!DOCTYPE html>
      <head></head>
      <body>
        <img src="data:image/png;base64,)HTML" +
                     String(BASE64_RED_IMAGE_DATA_99_100) +
                     R"HTML(" width="100%" height="auto">
        <img src="data:image/png;base64,)HTML" +
                     String(BASE64_RED_IMAGE_DATA_100_99) +
                     R"HTML(" width="100%" height="auto">
        <img src="data:image/png;base64,)HTML" +
                     String(BASE64_RED_IMAGE_DATA_100_100) +
                     R"HTML(" width="50%" height="auto"> 
      </body>
    )HTML");
  }
};

TEST_F(MouseEventManagerExtTest, IsDraging) {
  EXPECT_FALSE(GetMouseEventManagerExt().IsDraging());
}

TEST_F(MouseEventManagerExtTest, IsValidOverlayNode) {
  SetUpHtmlDefault();
  auto result = GetHitTestResultForTest(gfx::PointF(100, 500));  // empty result
  auto inner_node = result.InnerNode();

  SimTestExt::LogCatch();
  EXPECT_FALSE(GetMouseEventManagerExt().IsValidOverlayNode(inner_node));
  EXPECT_TRUE(SimTestExt::LogCheck("IsOverlayNodeValid node is null"));

  result = GetHitTestResultForTest(gfx::PointF(100, 100));
  inner_node = result.InnerNode();
  EXPECT_TRUE(GetMouseEventManagerExt().IsValidOverlayNode(inner_node));
}

TEST_F(MouseEventManagerExtTest, GetHitOverlayStatus) {
  SetUpHtmlDefault();
  SimTestExt::LogCatch();
  HitTestResult result;
  EXPECT_EQ(GetMouseEventManagerExt().GetHitOverlayStatus(result, false),
            HitOverlayStatus::kNone);

  auto result_red = GetHitTestResultForTest(gfx::PointF(100, 100));
  auto result_green = GetHitTestResultForTest(gfx::PointF(100, 300));

  auto inner_node_red = result_red.InnerNodeOrImageMapImage();
  auto inner_node_green = result_green.InnerNodeOrImageMapImage();

  SetHitImageNodeForTest(inner_node_red);

  EXPECT_EQ(GetMouseEventManagerExt().GetHitOverlayStatus(result_red, true),
            HitOverlayStatus::kCreated);

  GetOverlayInProgressRef() = true;
  EXPECT_EQ(GetMouseEventManagerExt().GetHitOverlayStatus(result_red, false),
            HitOverlayStatus::kCreated);

  GetOverlayCreatingRef() = true;
  EXPECT_EQ(GetMouseEventManagerExt().GetHitOverlayStatus(result_red, false),
            HitOverlayStatus::kCreating);

  EXPECT_EQ(GetMouseEventManagerExt().GetHitOverlayStatus(result_green, false),
            HitOverlayStatus::kNone);
  SimTestExt::LogCheck("");
}

TEST_F(MouseEventManagerExtTest, CloseImageOverlay) {
  SetUpHtmlDefault();
  SimTestExt::LogCatch();
  GetOverlayInProgressRef() = false;
  GetMouseEventManagerExt().CloseImageOverlay();
  EXPECT_TRUE(SimTestExt::LogCheck("No image overlay to closed"));
  
  GetOverlayInProgressRef() = true;
  GetMouseEventManagerExt().CloseImageOverlay();
}

TEST_F(MouseEventManagerExtTest, GetAbsImageRect) {
  SetUpHtmlDefault();
  gfx::RectF res_rect;
  SimTestExt::LogCatch();
  GetMouseEventManagerExt().GetAbsImageRect(res_rect);
  EXPECT_TRUE(SimTestExt::LogCheck("cannot get image from hit_image_node"));

  SimTestExt::LogCatch();
  CreateOverlayOnPoint(gfx::PointF(100, 100));
  EXPECT_TRUE(SimTestExt::LogCheck("getting layout box rect from hit_image_node"));
}

TEST_F(MouseEventManagerExtTest, SetOverlayInProgress) {
  SetUpHtmlDefault();
  GetMouseEventManagerExt().SetOverlayInProgress(true);
  EXPECT_TRUE(GetOverlayInProgressRef());

  GetMouseEventManagerExt().SetOverlayInProgress(false);
  EXPECT_FALSE(GetOverlayInProgressRef());
}

TEST_F(MouseEventManagerExtTest, SetOverlayCreatingStatus) {
  SetUpHtmlDefault();
  GetMouseEventManagerExt().SetOverlayCreatingStatus(true);
  EXPECT_TRUE(GetOverlayCreatingRef());

  GetMouseEventManagerExt().SetOverlayCreatingStatus(false);
  EXPECT_FALSE(GetOverlayCreatingRef());
}

TEST_F(MouseEventManagerExtTest, OnDestroyImageAnalyzerOverlay) {
  SetUpHtmlDefault();
  SimTestExt::LogCatch();
  GetMouseEventManagerExt().OnDestroyImageAnalyzerOverlay();
  EXPECT_TRUE(SimTestExt::LogCheck("OnDestroyImageAnalyzerOverlay"));
}

TEST_F(MouseEventManagerExtTest, OnFoldStatusChanged) {
  SetUpHtmlDefault();
  for (uint32_t i = 0 ; i < 5; ++i) {
    SimTestExt::LogCatch();
    GetMouseEventManagerExt().OnFoldStatusChanged(i);
    EXPECT_TRUE(SimTestExt::LogCheck("OnFoldStatusChanged"));
  }
}

TEST_F(MouseEventManagerExtTest, CreateOverlay_Success) {
  SetUpHtmlDefault();
  SimTestExt::LogCatch();

  CreateOverlayOnPoint(gfx::PointF(100, 100));
  EXPECT_TRUE(GetOverlayCreatingRef());
  EXPECT_FALSE(GetMouseEventManagerExt().GetOverlayInProgress());

  CreateOverlayOnPoint(gfx::PointF(100, 100));
  
  EXPECT_TRUE(SimTestExt::LogCheck("hit last analyzer image"));
}

TEST_F(MouseEventManagerExtTest, CreateOverlay_NoImage) {
  SetUpHtmlDefault();
  SimTestExt::LogCatch();

  CreateOverlayOnPoint(gfx::PointF(100, 700));
  EXPECT_FALSE(GetOverlayCreatingRef());

  EXPECT_TRUE(SimTestExt::LogCheck("invalid or has no image"));
}

TEST_F(MouseEventManagerExtTest, CreateOverlay_SelectNone) {
  SetUpHtmlSelectNone();
  SimTestExt::LogCatch();

  CreateOverlayOnPoint(gfx::PointF(100, 100));
  EXPECT_FALSE(GetOverlayCreatingRef());

  EXPECT_TRUE(SimTestExt::LogCheck("image is not selectable"));
}

TEST_F(MouseEventManagerExtTest, CreateOverlay_SmallImage) {
  SetUpHtmlSmallImage();

  CreateOverlayOnPoint(gfx::PointF(100, 100));
  EXPECT_FALSE(GetOverlayCreatingRef());

  CreateOverlayOnPoint(gfx::PointF(100, 300));
  EXPECT_FALSE(GetOverlayCreatingRef());

  CreateOverlayOnPoint(gfx::PointF(50, 450));
  EXPECT_FALSE(GetOverlayCreatingRef());
}

TEST_F(MouseEventManagerExtTest, CreateOverlayMouse_Success) {
  SetUpHtmlDefault();
  SimTestExt::LogCatch();

  CreateOverlayMouseOnPoint(gfx::PointF(100, 100));
  EXPECT_TRUE(GetOverlayCreatingRef());
  EXPECT_FALSE(GetMouseEventManagerExt().GetOverlayInProgress());

  CreateOverlayMouseOnPoint(gfx::PointF(100, 100));
  
  EXPECT_TRUE(SimTestExt::LogCheck("hit last analyzer image"));
}

TEST_F(MouseEventManagerExtTest, CreateOverlayMouse_NoImage) {
  SetUpHtmlDefault();
  SimTestExt::LogCatch();

  CreateOverlayMouseOnPoint(gfx::PointF(100, 700));
  EXPECT_FALSE(GetOverlayCreatingRef());

  EXPECT_TRUE(SimTestExt::LogCheck("invalid or has no image"));
}

TEST_F(MouseEventManagerExtTest, CreateOverlayMouse_SelectNone) {
  SetUpHtmlSelectNone();
  SimTestExt::LogCatch();

  CreateOverlayMouseOnPoint(gfx::PointF(100, 100));
  EXPECT_FALSE(GetOverlayCreatingRef());

  EXPECT_TRUE(SimTestExt::LogCheck("image is not selectable"));
}

TEST_F(MouseEventManagerExtTest, CreateOverlayMouse_SmallImage) {
  SetUpHtmlSmallImage();

  CreateOverlayMouseOnPoint(gfx::PointF(100, 100));
  EXPECT_FALSE(GetOverlayCreatingRef());

  CreateOverlayMouseOnPoint(gfx::PointF(100, 300));
  EXPECT_FALSE(GetOverlayCreatingRef());

  CreateOverlayMouseOnPoint(gfx::PointF(50, 450));
  EXPECT_FALSE(GetOverlayCreatingRef());
}

TEST_F(MouseEventManagerExtTest, CloseAIOverlay) {
  SetUpHtmlDefault();
  CreateOverlayOnPoint(gfx::PointF(100, 100));  // create overlay first

  SimTestExt::LogCatch();
  ShortPressEventBuilder short_press_event(gfx::PointF(100, 100));
  GetEventHandler().HandleGestureEvent(short_press_event);
  EXPECT_FALSE(
      SimTestExt::LogCheck("HandleGestureShortPress CloseImageOverlay"));

  GetOverlayCreatingRef() = false;
  GetOverlayInProgressRef() = true;

  SimTestExt::LogCatch();
  ShortPressEventBuilder short_press_event_new(gfx::PointF(100, 100));
  GetEventHandler().HandleGestureEvent(short_press_event_new);
  EXPECT_TRUE(
      SimTestExt::LogCheck("HandleGestureShortPress CloseImageOverlay"));
}

TEST_F(MouseEventManagerExtTest, HandleGestureDragLongPress_HitOverlay) {
  SetUpHtmlDefault();
  CreateOverlayOnPoint(gfx::PointF(100, 100));

  SimTestExt::LogCatch();
  DragLongPressEventBuilder drag_event(gfx::PointF(100, 100));
  GetEventHandler().HandleGestureEvent(drag_event);
  EXPECT_TRUE(SimTestExt::LogCheck("DragDrop HandleGestureDragLongPress"));
}

TEST_F(MouseEventManagerExtTest, StopCreateOverlayTimer) {
  SetUpHtmlDefault();
  auto& manager = GetMouseEventManagerExt();
  manager.create_overlay_timer_.Reset();
  EXPECT_TRUE(manager.create_overlay_timer_.IsRunning());

  manager.StopCreateOverlayTimer();
  EXPECT_FALSE(manager.create_overlay_timer_.IsRunning());
}

TEST_F(MouseEventManagerExtTest, IsValidOverlayNode002) {
  SetUpHtmlDefault();
  auto result = GetHitTestResultForTest(gfx::PointF(POINT_VAL_C, POINT_VAL_C));
  auto inner_node = result.InnerNode();

  SimTestExt::LogCatch();
  EXPECT_FALSE(GetMouseEventManagerExt().IsValidOverlayNode(inner_node));
}

TEST_F(MouseEventManagerExtTest, IsValidOverlayNode003) {
  EXPECT_FALSE(GetMouseEventManagerExt().IsValidOverlayNode(nullptr));
}

TEST_F(MouseEventManagerExtTest, CloseImageOverlayWhenMousePress) {
  SetUpHtmlDefault();
  auto& manager = GetMouseEventManagerExt();
  CreateOverlayOnPoint(gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  EXPECT_FALSE(manager.GetOverlayInProgress());

  WebMouseEvent mouse_event_in =
    CreateTestMouseEvent(WebInputEvent::Type::kMouseDown, gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  HitTestLocation location_in(gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  HitTestResult hit_in = GetHitTestResultForTest(gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  MouseEventWithHitTestResults event_in(mouse_event_in, location_in, hit_in);
  manager.CloseImageOverlayWhenMousePress(event_in);
  EXPECT_FALSE(manager.GetOverlayInProgress());

  WebMouseEvent mouse_event_out =
    CreateTestMouseEvent(WebInputEvent::Type::kMouseDown, gfx::PointF(POINT_VAL_A, POINT_VAL_B));
  HitTestLocation location_out(gfx::PointF(POINT_VAL_A, POINT_VAL_B));
  HitTestResult hit_out = GetHitTestResultForTest(gfx::PointF(POINT_VAL_A, POINT_VAL_B));
  MouseEventWithHitTestResults event_out(mouse_event_out, location_out, hit_out);
  manager.CloseImageOverlayWhenMousePress(event_out);
  EXPECT_FALSE(manager.GetOverlayInProgress());
}

TEST_F(MouseEventManagerExtTest, OverLayerMouseLeaveEventListener001) {
  SetUpHtmlDefault();
  auto& manager = GetMouseEventManagerExt();
  Element* element = GetDocument().getElementById(AtomicString("scroller"));
  MouseEventManagerExt::OverLayerMouseLeaveEventListener listener(element);
  Event *event = Event::Create();
  event->SetType(AtomicString("foo"));
  ASSERT_NO_FATAL_FAILURE(listener.Invoke(nullptr, event));

  event->SetType(event_type_names::kMouseleave);
  ASSERT_NO_FATAL_FAILURE(listener.Invoke(nullptr, event));
}

TEST_F(MouseEventManagerExtTest, OverLayerMouseLeaveEventListener002) {
  SetUpHtmlDefault();
  auto& manager = GetMouseEventManagerExt();
  Element* element = GetDocument().getElementById(AtomicString("scroller"));
  MouseEventManagerExt::OverLayerMouseLeaveEventListener listener(nullptr);
  Event *event = Event::Create();
  ASSERT_NO_FATAL_FAILURE(listener.Invoke(nullptr, event));
}

// HandleCreateOverlayWhenDrag tests
TEST_F(MouseEventManagerExtTest, HandleCreateOverlayWhenDrag_MousePressed) {
  SetUpHtmlDefault();
  auto& manager = GetMouseEventManagerExt();

  manager.create_overlay_timer_.Reset();
  EXPECT_TRUE(manager.create_overlay_timer_.IsRunning());

  manager.SetMousePressedForTest(true);
  WebMouseEvent mouse_event = CreateTestMouseEvent(WebInputEvent::Type::kMouseMove, gfx::PointF(100, 100));
  HitTestLocation location(gfx::PointF(100, 100));
  HitTestResult hit_test = GetHitTestResultForTest(gfx::PointF(100, 100));
  MouseEventWithHitTestResults event(mouse_event, location, hit_test);

  manager.HandleCreateOverlayWhenDrag(event);
  EXPECT_FALSE(manager.create_overlay_timer_.IsRunning());
}

TEST_F(MouseEventManagerExtTest, HandleCreateOverlayWhenDrag_MousePositionUnknown) {
  SetUpHtmlDefault();
  auto& manager = GetMouseEventManagerExt();

  manager.create_overlay_timer_.Reset();
  EXPECT_TRUE(manager.create_overlay_timer_.IsRunning());

  manager.SetMousePressedForTest(false);
  manager.SetMousePositionUnknownForTest(true);
  WebMouseEvent mouse_event = CreateTestMouseEvent(WebInputEvent::Type::kMouseMove, gfx::PointF(100, 100));
  HitTestLocation location(gfx::PointF(100, 100));
  HitTestResult hit_test = GetHitTestResultForTest(gfx::PointF(100, 100));
  MouseEventWithHitTestResults event(mouse_event, location, hit_test);

  manager.HandleCreateOverlayWhenDrag(event);
  EXPECT_FALSE(manager.create_overlay_timer_.IsRunning());
}

TEST_F(MouseEventManagerExtTest, HandleCreateOverlayWhenDrag_ResetTimer) {
  SetUpHtmlDefault();
  auto& manager = GetMouseEventManagerExt();

  manager.create_overlay_timer_.Stop();
  EXPECT_FALSE(manager.create_overlay_timer_.IsRunning());

  manager.SetMousePressedForTest(false);
  manager.SetMousePositionUnknownForTest(false);
  WebMouseEvent mouse_event = CreateTestMouseEvent(WebInputEvent::Type::kMouseMove, gfx::PointF(100, 100));
  HitTestLocation location(gfx::PointF(100, 100));
  HitTestResult hit_test = GetHitTestResultForTest(gfx::PointF(100, 100));
  MouseEventWithHitTestResults event(mouse_event, location, hit_test);

  manager.HandleCreateOverlayWhenDrag(event);
  EXPECT_TRUE(manager.create_overlay_timer_.IsRunning());
}

// GetHitOverlayStatusFromMouseEvent test
TEST_F(MouseEventManagerExtTest, GetHitOverlayStatusFromMouseEvent) {
  SetUpHtmlDefault();
  auto& manager = GetMouseEventManagerExt();

  WebMouseEvent mouse_event = CreateTestMouseEvent(WebInputEvent::Type::kMouseMove, gfx::PointF(100, 100));
  HitTestLocation location(gfx::PointF(100, 100));
  HitTestResult hit_test = GetHitTestResultForTest(gfx::PointF(100, 100));
  MouseEventWithHitTestResults event(mouse_event, location, hit_test);

  auto result = manager.GetHitOverlayStatusFromMouseEvent(event);
  EXPECT_EQ(result, HitOverlayStatus::kNone);
}

// GetAbsImageRect remaining branches
TEST_F(MouseEventManagerExtTest, GetAbsImageRect_NodeConnected) {
  SetUpHtmlDefault();
  auto& manager = GetMouseEventManagerExt();

  auto result = GetHitTestResultForTest(gfx::PointF(100, 100));
  auto inner_node = result.InnerNodeOrImageMapImage();
  SetHitImageNodeForTest(inner_node);

  SimTestExt::LogCatch();
  gfx::RectF res_rect;
  manager.GetAbsImageRect(res_rect);

  EXPECT_TRUE(SimTestExt::LogCheck("getting layout box rect"));
}

TEST_F(MouseEventManagerExtTest, GetAbsImageRect_NoLayoutBox) {
  SetUpHtmlDefault();
  auto& manager = GetMouseEventManagerExt();

  auto result = GetHitTestResultForTest(gfx::PointF(100, 100));
  auto inner_node = result.InnerNodeOrImageMapImage();
  SetHitImageNodeForTest(inner_node);

  SimTestExt::LogCatch();
  gfx::RectF res_rect;
  manager.GetAbsImageRect(res_rect);

  EXPECT_TRUE(SimTestExt::LogCheck("getting layout box rect"));
}

}  // namespace blink