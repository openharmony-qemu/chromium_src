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

#include <memory>
#include "third_party/blink/renderer/core/input/gesture_manager.h"

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

constexpr int POINT_VAL_A = 100;
constexpr int POINT_VAL_B = 500;
constexpr int POINT_VAL_C = 50;

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

class GestureManagerUtilsTest : public SimTestExt {
 protected:
  void SetUpHtmlLink() {
    WebView().MainFrameViewWidget()->Resize(gfx::Size(200, 600));
    SimRequest request("https://example.com/test.html", "text/html");
    LoadURL("https://example.com/test.html");

    request.Complete(R"HTML(
      <!DOCTYPE html>
      <html>
      <head>
          <style>
              .large-text {
                  width: 100%;
                  font-size: 15vw;
                  text-align: center;
                  margin: 0;
                  padding: 0;
              }
          </style>
      </head>
      <body>
          <a href="https://example.com" class="large-text">example.com</a>
      </body>
      </html>
    )HTML");
    Compositor().BeginFrame();
  }

  void SetUpHtmlLinkNoDrag() {
    WebView().MainFrameViewWidget()->Resize(gfx::Size(200, 600));
    SimRequest request("https://example.com/test.html", "text/html");
    LoadURL("https://example.com/test.html");

    request.Complete(R"HTML(
      <!DOCTYPE html>
      <html>
      <head>
          <style>
              .large-text {
                  width: 100%;
                  font-size: 15vw;
                  text-align: center;
                  margin: 0;
                  padding: 0;
              }
          </style>
      </head>
      <body>
          <a href="https://example.com" class="large-text" draggable="false">example.com</a>
      </body>
      </html>
    )HTML");
    Compositor().BeginFrame();
  }

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

  WebInputEventResult DragOnPoint(gfx::PointF position) {
    DragLongPressEventBuilder drag_event(position);
    return GetEventHandler().HandleGestureEvent(drag_event);
  }

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  void SetContextMenuCustomizationEnabled(bool enabled) {
    GetLocalFrame().GetSettings()->SetContextMenuCustomization(enabled);
  }
#endif

  void SetGestureFocusMode(int mode) {
    GetLocalFrame().GetSettings()->SetGestureFocusMode(mode);
  }

  void SetShowContextMenuOnMouseUp(bool state) {
    GetLocalFrame().GetSettings()->SetShowContextMenuOnMouseUp(state);
  }

  const HitTestResult& GetHitTestResultForTest(gfx::PointF position) {
    DragLongPressEventBuilder drag_event(position);
    GestureEventWithHitTestResults targeted_event =
        GetEventHandler().TargetGestureEvent(drag_event);
    return targeted_event.GetHitTestResult();
  }

};

TEST_F(GestureManagerUtilsTest, HandleGestureDragLongPress_DragLink) {
  SetUpHtmlLink();
  SimTestExt::LogCatch();
  EXPECT_EQ(DragOnPoint(gfx::PointF(100, 500)),
            WebInputEventResult::kNotHandled);
  EXPECT_TRUE(SimTestExt::LogCheck("DragDrop HandleGestureDragLongPress"));

  // drag link
  EXPECT_EQ(DragOnPoint(gfx::PointF(100, 20)),
            WebInputEventResult::kHandledSystem);
}

TEST_F(GestureManagerUtilsTest, HandleGestureDragLongPress_NoDrag) {
  SetUpHtmlLinkNoDrag();
  EXPECT_EQ(DragOnPoint(gfx::PointF(100, 20)),
            WebInputEventResult::kNotHandled);
}

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
TEST_F(GestureManagerUtilsTest, UpdateContextMenuForFreeCopy001) {
  SetUpHtmlDefault();
  auto result = GetHitTestResultForTest(gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  result.SetInnerNode(nullptr);
  SetContextMenuCustomizationEnabled(true);
  GestureManagerUtils gesture_manager_utils(&GetGestureManager());
  HitTestLocation location(gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  gesture_manager_utils.UpdateContextMenuForFreeCopy(result, location);
  EXPECT_EQ(result.InnerNode(), nullptr);
}

TEST_F(GestureManagerUtilsTest, UpdateContextMenuForFreeCopy002) {
  SetUpHtmlDefault();
  auto result = GetHitTestResultForTest(gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  auto inner_node_red = result.InnerNodeOrImageMapImage();
  result.SetInnerNode(inner_node_red);
  SetContextMenuCustomizationEnabled(true);
  inner_node_red->SetLayoutObject(nullptr);
  GestureManagerUtils gesture_manager_utils(&GetGestureManager());
  HitTestLocation location(gfx::PointF(POINT_VAL_C, POINT_VAL_C));
  gesture_manager_utils.UpdateContextMenuForFreeCopy(result, location);
  EXPECT_EQ(result.InnerNode()->GetLayoutObject(), nullptr);
}

TEST_F(GestureManagerUtilsTest, UpdateContextMenuForFreeCopy003) {
  SetUpHtmlDefault();
  auto result = GetHitTestResultForTest(gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  auto inner_node_red = result.InnerNodeOrImageMapImage();
  result.SetInnerNode(inner_node_red);
  SetContextMenuCustomizationEnabled(true);
  LayoutObject* layout_object = inner_node_red->GetLayoutObject();
  inner_node_red->SetLayoutObject(layout_object);
  GestureManagerUtils gesture_manager_utils(&GetGestureManager());
  SetGestureFocusMode(false);
  HitTestLocation location(gfx::PointF(POINT_VAL_A, POINT_VAL_B));
  gesture_manager_utils.UpdateContextMenuForFreeCopy(result, location);
  EXPECT_NE(result.InnerNode()->GetLayoutObject(), nullptr);
}

TEST_F(GestureManagerUtilsTest, UpdateContextMenuForFreeCopy004) {
  SetUpHtmlDefault();
  auto result = GetHitTestResultForTest(gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  auto inner_node_red = result.InnerNodeOrImageMapImage();
  result.SetInnerNode(inner_node_red);
  SetContextMenuCustomizationEnabled(true);
  LayoutObject* layout_object = inner_node_red->GetLayoutObject();
  inner_node_red->SetLayoutObject(layout_object);
  GestureManagerUtils gesture_manager_utils(&GetGestureManager());
  SetGestureFocusMode(true);
  SetShowContextMenuOnMouseUp(false);
  HitTestLocation location(gfx::PointF(POINT_VAL_A, POINT_VAL_B));
  gesture_manager_utils.UpdateContextMenuForFreeCopy(result, location);
  EXPECT_NE(result.InnerNode()->GetLayoutObject(), nullptr);
}

TEST_F(GestureManagerUtilsTest, UpdateContextMenuForFreeCopy005) {
  SetUpHtmlDefault();
  auto result = GetHitTestResultForTest(gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  auto inner_node_red = result.InnerNodeOrImageMapImage();
  result.SetInnerNode(inner_node_red);
  SetContextMenuCustomizationEnabled(true);
  LayoutObject* layout_object = inner_node_red->GetLayoutObject();
  inner_node_red->SetLayoutObject(layout_object);
  GestureManagerUtils gesture_manager_utils(&GetGestureManager());
  SetGestureFocusMode(true);
  SetShowContextMenuOnMouseUp(true);
  HitTestLocation location(gfx::PointF(POINT_VAL_A, POINT_VAL_B));
  gesture_manager_utils.UpdateContextMenuForFreeCopy(result, location);
  EXPECT_NE(result.InnerNode()->GetLayoutObject(), nullptr);
}

TEST_F(GestureManagerUtilsTest, UpdateContextMenuForFreeCopy006) {
  SetUpHtmlDefault();
  auto result = GetHitTestResultForTest(gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  result.SetInnerNode(nullptr);
  SetContextMenuCustomizationEnabled(false);
  GestureManagerUtils gesture_manager_utils(&GetGestureManager());
  HitTestLocation location(gfx::PointF(POINT_VAL_C, POINT_VAL_C));
  gesture_manager_utils.UpdateContextMenuForFreeCopy(result, location);
  EXPECT_EQ(result.InnerNode(), nullptr);
}

TEST_F(GestureManagerUtilsTest, UpdateContextMenuForFreeCopy007) {
  SetUpHtmlDefault();
  auto result = GetHitTestResultForTest(gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  auto inner_node_red = result.InnerNodeOrImageMapImage();
  inner_node_red->SetLayoutObject(nullptr);
  result.SetInnerNode(inner_node_red);
  SetContextMenuCustomizationEnabled(false);
  GestureManagerUtils gesture_manager_utils(&GetGestureManager());
  HitTestLocation location(gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  gesture_manager_utils.UpdateContextMenuForFreeCopy(result, location);
  EXPECT_EQ(result.InnerNode()->GetLayoutObject(), nullptr);
}

TEST_F(GestureManagerUtilsTest, UpdateContextMenuForFreeCopy008) {
  SetUpHtmlDefault();
  auto result = GetHitTestResultForTest(gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  auto inner_node_red = result.InnerNodeOrImageMapImage();
  LayoutObject* layout_object = inner_node_red->GetLayoutObject();
  inner_node_red->SetLayoutObject(layout_object);
  result.SetInnerNode(inner_node_red);
  SetContextMenuCustomizationEnabled(false);
  GestureManagerUtils gesture_manager_utils(&GetGestureManager());
  HitTestLocation location(gfx::PointF(POINT_VAL_A, POINT_VAL_B));
  SetGestureFocusMode(false);
  gesture_manager_utils.UpdateContextMenuForFreeCopy(result, location);
  EXPECT_NE(result.InnerNode()->GetLayoutObject(), nullptr);
}

TEST_F(GestureManagerUtilsTest, UpdateContextMenuForFreeCopy009) {
  SetUpHtmlDefault();
  auto result = GetHitTestResultForTest(gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  auto inner_node_red = result.InnerNodeOrImageMapImage();
  LayoutObject* layout_object = inner_node_red->GetLayoutObject();
  inner_node_red->SetLayoutObject(layout_object);
  result.SetInnerNode(inner_node_red);
  SetContextMenuCustomizationEnabled(false);
  GestureManagerUtils gesture_manager_utils(&GetGestureManager());
  HitTestLocation location(gfx::PointF(POINT_VAL_A, POINT_VAL_B));
  SetGestureFocusMode(true);
  SetShowContextMenuOnMouseUp(false);
  gesture_manager_utils.UpdateContextMenuForFreeCopy(result, location);
  EXPECT_NE(result.InnerNode()->GetLayoutObject(), nullptr);
}

TEST_F(GestureManagerUtilsTest, UpdateContextMenuForFreeCopy010) {
  SetUpHtmlDefault();
  auto result = GetHitTestResultForTest(gfx::PointF(POINT_VAL_A, POINT_VAL_A));
  auto inner_node_red = result.InnerNodeOrImageMapImage();
  LayoutObject* layout_object = inner_node_red->GetLayoutObject();
  inner_node_red->SetLayoutObject(layout_object);
  result.SetInnerNode(inner_node_red);
  SetContextMenuCustomizationEnabled(false);
  GestureManagerUtils gesture_manager_utils(&GetGestureManager());
  HitTestLocation location(gfx::PointF(POINT_VAL_A, POINT_VAL_B));
  SetGestureFocusMode(true);
  SetShowContextMenuOnMouseUp(true);
  gesture_manager_utils.UpdateContextMenuForFreeCopy(result, location);
  EXPECT_NE(result.InnerNode()->GetLayoutObject(), nullptr);
}
#endif

}  // namespace blink