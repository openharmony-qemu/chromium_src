#include "third_party/blink/renderer/core/input/gesture_manager.h"

namespace blink {
namespace {
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

  WebInputEventResult DragOnPoint(gfx::PointF position) {
    DragLongPressEventBuilder drag_event(position);
    return GetEventHandler().HandleGestureEvent(drag_event);
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
}  // namespace blink