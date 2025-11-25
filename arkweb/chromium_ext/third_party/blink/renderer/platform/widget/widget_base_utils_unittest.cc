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

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/widget/widget_base_utils.h"
#include "base/task/single_thread_task_runner.h"
#include "base/test/task_environment.h"
#include "cc/test/layer_tree_test.h"
#include "cc/trees/layer_tree_host.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/platform/scheduler/test/renderer_scheduler_test_support.h"
#include "third_party/blink/renderer/platform/widget/widget_base.h"
#include "third_party/blink/renderer/platform/widget/widget_base_client.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"

using testing::_;

namespace blink {

class StubWidgetBaseClient : public WidgetBaseClient {
 public:
  void OnCommitRequested() override {}
  void BeginMainFrame(base::TimeTicks) override {}
  void UpdateLifecycle(WebLifecycleUpdate, DocumentUpdateReason) override {}
  std::unique_ptr<cc::LayerTreeFrameSink> AllocateNewLayerTreeFrameSink()
      override {
    return nullptr;
  }
  KURL GetURLForDebugTrace() override { return {}; }
  WebInputEventResult DispatchBufferedTouchEvents() override {
    return WebInputEventResult::kNotHandled;
  }
  WebInputEventResult HandleInputEvent(const WebCoalescedInputEvent&) override {
    return WebInputEventResult::kNotHandled;
  }
  bool SupportsBufferedTouchEvents() override { return false; }
  void WillHandleGestureEvent(const WebGestureEvent&, bool* suppress) override {
  }
  void WillHandleMouseEvent(const WebMouseEvent&) override {}
  void ObserveGestureEventAndResult(const WebGestureEvent&,
                                    const gfx::Vector2dF&,
                                    const cc::OverscrollBehavior&,
                                    bool) override {}
  void FocusChanged(mojom::blink::FocusState) override {}
  void UpdateVisualProperties(
      const VisualProperties& visual_properties) override {}
  const display::ScreenInfos& GetOriginalScreenInfos() override {
    return screen_infos_;
  }
  gfx::Rect ViewportVisibleRect() override { return gfx::Rect(); }

 private:
  display::ScreenInfos screen_infos_;
};

class WidgetBaseUtilsTest : public testing::Test {
protected:
  base::test::TaskEnvironment task_environment_;

  void SetUp() override {
    mojo::AssociatedRemote<mojom::blink::Widget> widget_remote;
    mojo::PendingAssociatedReceiver<mojom::blink::Widget> widget_receiver =
        widget_remote.BindNewEndpointAndPassDedicatedReceiver();

    mojo::AssociatedRemote<mojom::blink::WidgetHost> widget_host_remote;
    std::ignore = widget_host_remote.BindNewEndpointAndPassDedicatedReceiver();

    widget_base_ = std::make_unique<WidgetBase>(
        /*widget_base_client=*/&client_, widget_host_remote.Unbind(),
        std::move(widget_receiver),
        scheduler::GetSingleThreadTaskRunnerForTesting(),
        /*is_hidden=*/false,
        /*never_composited=*/false,
        /*is_for_child_local_root=*/false,
        /*is_for_scalable_page=*/true);
    
    utils_ = std::make_unique<WidgetBaseUtils>(widget_base_.get());
  }

  void SetWokerPoolInitial(bool flag) {
    widget_base_->is_worker_pool_initial_ = flag;
  }

  void SetWidgetInputHandlerManager(scoped_refptr<WidgetInputHandlerManager> manager) {
    widget_base_->widget_input_handler_manager_ = manager;
  }

  std::unique_ptr<WidgetBaseUtils> utils_;
  StubWidgetBaseClient client_;
  std::unique_ptr<WidgetBase> widget_base_;
};

TEST_F(WidgetBaseUtilsTest, SetZoomLevel_NullInputHandlerManager) {
  SetWidgetInputHandlerManager(nullptr);
  utils_->SetZoomLevel(1.0f, gfx::Point(10, 10));
}

TEST_F(WidgetBaseUtilsTest, SetZoomLevel_ValidInputHandlerManager) {
  utils_->SetZoomLevel(1.0f, gfx::Point(10, 10));
}

TEST_F(WidgetBaseUtilsTest, SetOverscrollMode_NullInputHandlerManager) {
  SetWidgetInputHandlerManager(nullptr);
  utils_->SetOverscrollMode(1);
}

TEST_F(WidgetBaseUtilsTest, SetOverscrollMode_ValidInputHandlerManager) {
  utils_->SetOverscrollMode(1);
}

TEST_F(WidgetBaseUtilsTest, GetOverScrollOffset_NullInputHandlerManager) {
  SetWidgetInputHandlerManager(nullptr);
  gfx::Vector2dF result = utils_->GetOverScrollOffset();
  EXPECT_EQ(result.x(), 0.0f);
  EXPECT_EQ(result.y(), 0.0f);
}

TEST_F(WidgetBaseUtilsTest, GetOverScrollOffset_ValidInputHandlerManager) {
  utils_->GetOverScrollOffset();
}

TEST_F(WidgetBaseUtilsTest, TouchHitTest_NullFrameWidget) {
  WebPointerProperties properties(0, WebPointerProperties::PointerType::kTouch,
                                  WebPointerProperties::Button::kLeft,
                                  gfx::PointF(10, 10), gfx::PointF(10, 10));

  WebPointerEvent event(WebInputEvent::Type::kPointerDown,
                        properties,
                        5.0f,
                        5.0f);

  utils_->TouchHitTest(event, 0);
}

TEST_F(WidgetBaseUtilsTest, NativeHitTestResult_NullInputHandlerManager) {
  SetWidgetInputHandlerManager(nullptr);
  utils_->NativeHitTestResult(true, 1, 100);
}

TEST_F(WidgetBaseUtilsTest, NativeHitTestResult_ValidInputHandlerManager) {
  utils_->NativeHitTestResult(true, 1, 100);
}

TEST_F(WidgetBaseUtilsTest, DidNativeEmbedEvent_AllTypes) {
  std::vector<WebInputEvent::Type> types = {
      WebInputEvent::Type::kTouchStart,
      WebInputEvent::Type::kTouchMove,
      WebInputEvent::Type::kTouchEnd,
      WebInputEvent::Type::kTouchCancel
  };

  for (auto type : types) {
    utils_->DidNativeEmbedEvent(type, "embed1", 1, 10.0f, 20.0f);
  }
}

TEST_F(WidgetBaseUtilsTest, MouseHitTest_NullFrameWidget) {
  WebMouseEvent event(WebInputEvent::Type::kMouseDown,
                      WebInputEvent::kNoModifiers,
                      base::TimeTicks::Now());
  event.SetPositionInWidget(10, 10);
  utils_->MouseHitTest(event, 0);
}

TEST_F(WidgetBaseUtilsTest, NativeMouseHitTestResult_NullInputHandlerManager) {
  SetWidgetInputHandlerManager(nullptr);
  utils_->NativeMouseHitTestResult(true, 100, 0);
}

TEST_F(WidgetBaseUtilsTest, NativeMouseHitTestResult_ValidInputHandlerManager) {
  utils_->NativeMouseHitTestResult(true, 100, 0);
}

TEST_F(WidgetBaseUtilsTest, DidNativeEmbedMouseEvent_AllTypesAndButtons) {
  std::vector<WebInputEvent::Type> types = {
      WebInputEvent::Type::kMouseDown,
      WebInputEvent::Type::kMouseUp,
      WebInputEvent::Type::kMouseMove,
      WebInputEvent::Type::kMouseLeave
  };

  std::vector<WebInputEvent::Modifiers> buttons = {
      WebInputEvent::Modifiers::kLeftButtonDown,
      WebInputEvent::Modifiers::kRightButtonDown,
      WebInputEvent::Modifiers::kMiddleButtonDown,
      WebInputEvent::Modifiers::kNoModifiers
  };

  for (auto type : types) {
    for (auto button : buttons) {
      utils_->DidNativeEmbedMouseEvent(
          type, button, "embed1", true, 10.0f, 20.0f);
    }
  }
}

} // namespace blink
#pragma clang diagnostic pop