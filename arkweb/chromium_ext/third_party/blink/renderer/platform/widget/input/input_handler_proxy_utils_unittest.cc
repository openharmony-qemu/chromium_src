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
#include "input_handler_proxy_utils.h"
#include "base/containers/circular_deque.h"
#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/memory/shared_memory_mapping.h"
#include "base/test/bind.h"
#include "base/test/metrics/histogram_tester.h"
#include "base/test/scoped_feature_list.h"
#include "base/test/simple_test_tick_clock.h"
#include "base/test/task_environment.h"
#include "build/build_config.h"
#include "cc/animation/animation_host.h"
#include "cc/base/features.h"
#define private public
#include "cc/input/input_handler_utils.h"
#include "cc/layers/layer_impl.h"
#undef private
#include "cc/mojo_embedder/software_compositor_renderer_ohos.h"
#include "cc/test/fake_impl_task_runner_provider.h"
#include "cc/test/fake_layer_tree_host.h"
#include "cc/test/fake_layer_tree_host_client.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/test_task_graph_runner.h"
#include "cc/trees/latency_info_swap_promise_monitor.h"
#include "cc/trees/layer_tree_settings.h"
#include "components/viz/common/quads/compositor_frame.h"
#include "third_party/blink/public/common/input/web_input_event.h"
#include "third_party/blink/public/common/input/web_input_event_attribution.h"
#include "third_party/blink/public/common/input/web_keyboard_event.h"
#include "third_party/blink/public/common/input/web_mouse_event.h"
#include "third_party/blink/public/common/input/web_mouse_wheel_event.h"
#include "third_party/blink/public/common/input/web_pointer_event.h"
#include "third_party/blink/public/common/input/web_touch_event.h"
#include "third_party/blink/renderer/platform/widget/input/compositor_thread_event_queue.h"
#include "third_party/blink/renderer/platform/widget/input/event_with_callback.h"
#include "third_party/blink/renderer/platform/widget/input/input_handler_proxy.h"
#include "third_party/blink/renderer/platform/widget/input/input_handler_proxy_client.h"
#include "third_party/blink/renderer/platform/widget/input/native_embed_event_queue.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"
namespace blink {

class FakeCompositorDelegateForInput : public cc::CompositorDelegateForInput {
 public:
  FakeCompositorDelegateForInput()
      : host_impl_(&task_runner_provider_, &task_graph_runner_) {}
  void BindToInputHandler(
      std::unique_ptr<cc::InputDelegateForCompositor> delegate) override {}
  cc::ScrollTree& GetScrollTree() const override { return scroll_tree_; }
  bool HasAnimatedScrollbars() const override { return false; }
  void SetNeedsCommit() override {}
  void SetNeedsFullViewportRedraw() override {}
  void SetDeferBeginMainFrame(bool defer_begin_main_frame) const override {}
  void DidUpdateScrollAnimationCurve() override {}
  void AccumulateScrollDeltaForTracing(const gfx::Vector2dF& delta) override {}
  void DidStartPinchZoom() override {}
  void DidUpdatePinchZoom() override {}
  void DidEndPinchZoom() override {}
  void DidStartScroll() override {}
  void DidEndScroll() override {}
  void DidMouseLeave() override {}
  bool IsInHighLatencyMode() const override { return false; }
  void WillScrollContent(cc::ElementId element_id) override {}
  void DidScrollContent(cc::ElementId element_id, bool animated) override {}
  float DeviceScaleFactor() const override { return 0; }
  float PageScaleFactor() const override { return 0; }
  gfx::Size VisualDeviceViewportSize() const override { return gfx::Size(); }
  const cc::LayerTreeSettings& GetSettings() const override {
    return settings_;
  }
  cc::LayerTreeHostImpl& GetImplDeprecated() override { return host_impl_; }

  const cc::LayerTreeHostImpl& GetImplDeprecated() const override {
    return host_impl_;
  }
  void UpdateBrowserControlsState(
      cc::BrowserControlsState constraints,
      cc::BrowserControlsState current,
      bool animate,
      base::optional_ref<const cc::BrowserControlsOffsetTagsInfo>
          offset_tags_info) override {}
  bool HasScrollLinkedAnimation(cc::ElementId for_scroller) const override {
    return false;
  }

 private:
  mutable cc::ScrollTree scroll_tree_;
  cc::LayerTreeSettings settings_;
  cc::FakeImplTaskRunnerProvider task_runner_provider_;
  cc::TestTaskGraphRunner task_graph_runner_;
  cc::FakeLayerTreeHostImpl host_impl_;
};

base::LazyInstance<FakeCompositorDelegateForInput>::Leaky
    g_fake_compositor_delegate = LAZY_INSTANCE_INITIALIZER;

class MockInputHandler : public cc::InputHandler {
 public:
  MockInputHandler() : cc::InputHandler(g_fake_compositor_delegate.Get()) {}
  MockInputHandler(const MockInputHandler&) = delete;
  MockInputHandler& operator=(const MockInputHandler&) = delete;

  ~MockInputHandler() override = default;

  base::WeakPtr<InputHandler> AsWeakPtr() override {
    return weak_ptr_factory_.GetWeakPtr();
  }
  MOCK_METHOD2(PinchGestureBegin,
               void(const gfx::Point& anchor, ui::ScrollInputType type));
  MOCK_METHOD2(PinchGestureUpdate,
               void(float magnify_delta, const gfx::Point& anchor));
  MOCK_METHOD1(PinchGestureEnd, void(const gfx::Point& anchor));

  MOCK_METHOD0(SetNeedsAnimateInput, void());

  MOCK_METHOD2(ScrollBegin,
               ScrollStatus(cc::ScrollState*, ui::ScrollInputType type));
  MOCK_METHOD2(RootScrollBegin,
               ScrollStatus(cc::ScrollState*, ui::ScrollInputType type));
  MOCK_METHOD2(ScrollUpdate,
               cc::InputHandlerScrollResult(cc::ScrollState, base::TimeDelta));
  MOCK_METHOD1(ScrollEnd, void(bool));
  MOCK_METHOD2(RecordScrollBegin,
               void(ui::ScrollInputType type,
                    cc::ScrollBeginThreadState state));
  MOCK_METHOD1(RecordScrollEnd, void(ui::ScrollInputType type));
  MOCK_METHOD1(HitTest,
               cc::PointerResultType(const gfx::PointF& mouse_position));
  MOCK_METHOD2(MouseDown,
               cc::InputHandlerPointerResult(const gfx::PointF& mouse_position,
                                             const bool shift_modifier));
  MOCK_METHOD1(
      MouseUp,
      cc::InputHandlerPointerResult(const gfx::PointF& mouse_position));
  MOCK_METHOD1(SetIsHandlingTouchSequence, void(bool));
  void NotifyInputEvent() override {}

  std::unique_ptr<cc::LatencyInfoSwapPromiseMonitor>
  CreateLatencyInfoSwapPromiseMonitor(ui::LatencyInfo* latency) override {
    return nullptr;
  }

  std::unique_ptr<cc::EventsMetricsManager::ScopedMonitor>
  GetScopedEventMetricsMonitor(
      cc::EventsMetricsManager::ScopedMonitor::DoneCallback) override {
    return nullptr;
  }

  cc::ScrollElasticityHelper* CreateScrollElasticityHelper() override {
    return nullptr;
  }
  void DestroyScrollElasticityHelper() override {}

  bool GetScrollOffsetForLayer(cc::ElementId element_id,
                               gfx::PointF* offset) override {
    return false;
  }
  bool ScrollLayerTo(cc::ElementId element_id,
                     const gfx::PointF& offset) override {
    return false;
  }

  void BindToClient(cc::InputHandlerClient* client) override {}

  void MouseLeave() override {}

  MOCK_METHOD1(FindFrameElementIdAtPoint, cc::ElementId(const gfx::PointF&));

  cc::InputHandlerPointerResult MouseMoveAt(
      const gfx::Point& mouse_position) override {
    return cc::InputHandlerPointerResult();
  }

  MOCK_CONST_METHOD1(
      GetEventListenerProperties,
      cc::EventListenerProperties(cc::EventListenerClass event_class));
  MOCK_METHOD2(EventListenerTypeForTouchStartOrMoveAt,
               cc::InputHandler::TouchStartOrMoveEventListenerType(
                   const gfx::Point& point,
                   cc::TouchAction* touch_action));
  MOCK_CONST_METHOD1(HasBlockingWheelEventHandlerAt, bool(const gfx::Point&));

  MOCK_METHOD0(RequestUpdateForSynchronousInputHandler, void());
  MOCK_METHOD1(SetSynchronousInputHandlerRootScrollOffset,
               void(const gfx::PointF& root_offset));

  bool IsCurrentlyScrollingViewport() const override {
    return is_scrolling_root_;
  }
  void set_is_scrolling_root(bool is) { is_scrolling_root_ = is; }

  MOCK_METHOD4(GetSnapFlingInfoAndSetAnimatingSnapTarget,
               bool(const gfx::Vector2dF& current_delta,
                    const gfx::Vector2dF& natural_displacement,
                    gfx::PointF* initial_offset,
                    gfx::PointF* target_offset));
  MOCK_METHOD1(ScrollEndForSnapFling, void(bool));

  bool ScrollbarScrollIsActive() override { return false; }

  void SetDeferBeginMainFrame(bool defer_begin_main_frame) const override {}

  MOCK_METHOD4(UpdateBrowserControlsState,
               void(cc::BrowserControlsState constraints,
                    cc::BrowserControlsState current,
                    bool animate,
                    base::optional_ref<const cc::BrowserControlsOffsetTagsInfo>
                        offset_tags_info));

 private:
  bool is_scrolling_root_ = true;

  base::WeakPtrFactory<MockInputHandler> weak_ptr_factory_{this};
};

class MockSynchronousInputHandler : public SynchronousInputHandler {
 public:
  MOCK_METHOD6(UpdateRootLayerState,
               void(const gfx::PointF& total_scroll_offset,
                    const gfx::PointF& max_scroll_offset,
                    const gfx::SizeF& scrollable_size,
                    float page_scale_factor,
                    float min_page_scale_factor,
                    float max_page_scale_factor));
};

class MockInputHandlerProxyClient : public InputHandlerProxyClient {
 public:
  MockInputHandlerProxyClient() {}
  MockInputHandlerProxyClient(const MockInputHandlerProxyClient&) = delete;
  MockInputHandlerProxyClient& operator=(const MockInputHandlerProxyClient&) =
      delete;

  ~MockInputHandlerProxyClient() override {}

  void WillShutdown() override {}

  MOCK_METHOD3(GenerateScrollBeginAndSendToMainThread,
               void(const WebGestureEvent& update_event,
                    const WebInputEventAttribution&,
                    const cc::EventMetrics*));
#if BUILDFLAG(ARKWEB_UNITTESTS)
  MOCK_METHOD5(
      DidNativeEmbedEvent,
      void(blink::WebInputEvent::Type, std::string, int32_t, float, float));
  MOCK_METHOD2(TouchHitTest, void(const WebPointerEvent& event, size_t i));
  MOCK_METHOD6(DidNativeEmbedMouseEvent,
               void(blink::WebInputEvent::Type,
                    blink::WebInputEvent::Modifiers modifiers,
                    std::string,
                    bool,
                    float,
                    float));
  MOCK_METHOD2(MouseHitTest, void(const WebMouseEvent& event, int32_t button));
#endif  // ARKWEB_UNITTESTS
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  void OnOverScrollOffsetChanged(float offset_x, float offset_y) override {}
#endif
  MOCK_METHOD5(DidOverscroll,
               void(const gfx::Vector2dF& accumulated_overscroll,
                    const gfx::Vector2dF& latest_overscroll_delta,
                    const gfx::Vector2dF& current_fling_velocity,
                    const gfx::PointF& causal_event_viewport_point,
                    const cc::OverscrollBehavior& overscroll_behavior));
  void DidStartScrollingViewport() override {}
  MOCK_METHOD1(SetAllowedTouchAction, void(cc::TouchAction touch_action));
  bool AllowsScrollResampling() override { return true; }
};

class MockInputHandlerProxy : public InputHandlerProxy {
 public:
  MockInputHandlerProxy(cc::InputHandler& input_handler,
                        InputHandlerProxyClient* client)
      : InputHandlerProxy(input_handler, client) {}

  MOCK_METHOD0(SetNeedsAnimateInput, void());
};

class InputHandlerProxyUtilsTest : public ::testing::Test {
 public:
  InputHandlerProxyUtilsTest()
      : host_impl_(&task_runner_provider_, &task_graph_runner_) {}
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();
  void SetHitTestingNumber(InputHandlerProxyUtils* utils,
                           int32_t hit_testing_number) {
    utils->hit_testing_number_ = hit_testing_number;
  }
  void SetEvent(InputHandlerProxyUtils* utils,
                const WebInputEvent& event,
                const ui::LatencyInfo& info,
                std::unique_ptr<cc::EventMetrics> metrics,
                WebTouchEvent& start_touch_event) {
    auto webEvent = std::make_unique<EventWithCallback>(
        std::make_unique<WebCoalescedInputEvent>(event, info),
        base::DoNothing(), std::move(metrics));
    utils->native_event_queue_->Queue(std::move(webEvent));
    utils->start_touch_event_ = start_touch_event;
  }
  void SetTouchEvent(InputHandlerProxyUtils* utils,
                     const WebInputEvent& event,
                     const ui::LatencyInfo& info,
                     std::unique_ptr<cc::EventMetrics> metrics,
                     WebTouchEvent& start_touch_event) {
    auto webEventEnd = std::make_unique<EventWithCallback>(
        std::make_unique<WebCoalescedInputEvent>(event, info),
        base::DoNothing(), std::move(metrics));
    utils->native_touch_end_queue_->Queue(std::move(webEventEnd));
    utils->start_touch_event_ = start_touch_event;
  }

  void SetLayer(InputHandlerProxyUtils* utils, int id, int32_t button) {
    utils->mouse_native_id_map_[button] = id;
  }
  void SetEndIndexQueue(InputHandlerProxyUtils* utils, size_t index) {
    utils->end_index_queue_.emplace_back(index);
  }
  int32_t GetHitTestNumber(InputHandlerProxyUtils* utils) {
    return utils->hit_testing_number_;
  }

  void SetMouseHitTestingNumber(InputHandlerProxyUtils* utils,
                                int32_t hit_testing_number) {
    utils->mouse_hit_testing_number_ = hit_testing_number;
  }
  void SetMouseEvent(InputHandlerProxyUtils* utils,
                     const WebInputEvent& event,
                     const ui::LatencyInfo& info,
                     std::unique_ptr<cc::EventMetrics> metrics,
                     WebMouseEvent& start_touch_event) {
    auto webEventEnd = std::make_unique<EventWithCallback>(
        std::make_unique<WebCoalescedInputEvent>(event, info),
        base::DoNothing(), std::move(metrics));
    utils->native_mouse_event_queue_->Queue(std::move(webEventEnd));
    utils->start_mouse_event_ = start_touch_event;
  }
  void SetMouseEventEnd(InputHandlerProxyUtils* utils,
                        const WebInputEvent& event,
                        const ui::LatencyInfo& info,
                        std::unique_ptr<cc::EventMetrics> metrics,
                        WebMouseEvent& start_touch_event) {
    auto webEventEnd = std::make_unique<EventWithCallback>(
        std::make_unique<WebCoalescedInputEvent>(event, info),
        base::DoNothing(), std::move(metrics));
    utils->native_mouse_end_queue_->Queue(std::move(webEventEnd));
    utils->start_mouse_event_ = start_touch_event;
  }
  int32_t GetMouseHitTestNumber(InputHandlerProxyUtils* utils) {
    return utils->mouse_hit_testing_number_;
  }
  void SetNativeEnabled(InputHandlerProxyUtils* utils, bool enable) {
    utils->native_enabled_ = enable;
  }
  bool GetNativeEnabled(InputHandlerProxyUtils* utils) {
    return utils->native_enabled_;
  }
  void SetNeedFlushScrollUpdateGesture(InputHandlerProxyUtils* utils,
                                       bool status) {
    utils->need_flush_scroll_update_gesture_ = status;
  }
  bool GetNeedFlushScrollUpdateGesture(InputHandlerProxyUtils* utils) {
    return utils->need_flush_scroll_update_gesture_;
  }
  void SetMouseNativeMap(InputHandlerProxyUtils* utils,
                         int32_t button,
                         bool value) {
    utils->mouse_native_map_[button] = value;
  }
  bool GetEnableCustomVideoPlayer(InputHandlerProxyUtils* utils) {
    return utils->enable_custom_video_player_;
  }

 protected:
  base::test::SingleThreadTaskEnvironment task_environment_;
  cc::FakeLayerTreeHostClient fake_client_;
  cc::FakeImplTaskRunnerProvider task_runner_provider_;
  cc::TestTaskGraphRunner task_graph_runner_;
  std::unique_ptr<cc::AnimationHost> animation_host_;
  std::unique_ptr<cc::FakeLayerTreeHost> layer_tree_host_;
  cc::FakeLayerTreeHostImpl host_impl_;
};

void InputHandlerProxyUtilsTest::SetUpTestCase(void) {}

void InputHandlerProxyUtilsTest::TearDownTestCase(void) {}

void InputHandlerProxyUtilsTest::SetUp(void) {
  animation_host_ =
      cc::AnimationHost::CreateForTesting(cc::ThreadInstance::kMain);
  layer_tree_host_ = cc::FakeLayerTreeHost::Create(
      &fake_client_, &task_graph_runner_, animation_host_.get());
  layer_tree_host_->SetViewportRectAndScale(gfx::Rect(1, 1), 1.f,
                                            viz::LocalSurfaceId());
  host_impl_.CreatePendingTree();
}

void InputHandlerProxyUtilsTest::TearDown(void) {
  layer_tree_host_->SetRootLayer(nullptr);
  layer_tree_host_ = nullptr;
}

TEST_F(InputHandlerProxyUtilsTest, ResetNeedFlushScrollUpdateGesture_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  SetNeedFlushScrollUpdateGesture(&utils, true);
  utils.ResetNeedFlushScrollUpdateGesture();
  EXPECT_EQ(GetNeedFlushScrollUpdateGesture(&utils), false);
  SetNeedFlushScrollUpdateGesture(&utils, false);
  utils.ResetNeedFlushScrollUpdateGesture();
  EXPECT_EQ(GetNeedFlushScrollUpdateGesture(&utils), false);
}

TEST_F(InputHandlerProxyUtilsTest, SendNativeEvent_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  WebTouchEvent touch_event;
  utils.SendNativeEvent(touch_event, WebInputEvent::Type::kTouchStart, 0);
}

TEST_F(InputHandlerProxyUtilsTest, DidNativeEmbedEvent_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  WebTouchEvent event;
  auto result = utils.DidNativeEmbedEvent(event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  event.SetType(WebInputEvent::Type::kMouseUp);
  result = utils.DidNativeEmbedEvent(event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  event.touches_length = 0;
  event.SetType(WebInputEvent::Type::kTouchStart);
  result = utils.DidNativeEmbedEvent(event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  event.touches_length = 0;
  event.SetType(WebInputEvent::Type::kTouchMove);
  result = utils.DidNativeEmbedEvent(event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  event.touches_length = 0;
  event.SetType(WebInputEvent::Type::kTouchEnd);
  result = utils.DidNativeEmbedEvent(event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  event.touches_length = 0;
  event.SetType(WebInputEvent::Type::kTouchCancel);
  result = utils.DidNativeEmbedEvent(event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  event.touches_length = 1;
  event.touches[0].state = WebTouchPoint::State::kStatePressed;
  event.unique_touch_event_id = 1;
  event.SetType(WebInputEvent::Type::kTouchStart);
  result = utils.DidNativeEmbedEvent(event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  event.touches_length = 1;
  event.touches[0].state = WebTouchPoint::State::kStateMoved;
  event.unique_touch_event_id = 1;
  event.SetType(WebInputEvent::Type::kTouchStart);
  result = utils.DidNativeEmbedEvent(event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
}

TEST_F(InputHandlerProxyUtilsTest, DidNativeEmbedEvent_002) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  WebTouchEvent event;
  event.touches_length = 1;
  event.touches[0].state = WebTouchPoint::State::kStateMoved;
  event.SetType(WebInputEvent::Type::kTouchMove);
  auto result = utils.DidNativeEmbedEvent(event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  event.touches_length = 1;
  event.touches[0].state = WebTouchPoint::State::kStatePressed;
  event.SetType(WebInputEvent::Type::kTouchMove);
  result = utils.DidNativeEmbedEvent(event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  event.touches_length = 1;
  event.touches[0].state = WebTouchPoint::State::kStateReleased;
  event.SetType(WebInputEvent::Type::kTouchEnd);
  result = utils.DidNativeEmbedEvent(event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  event.touches_length = 1;
  event.touches[0].state = WebTouchPoint::State::kStateMoved;
  event.SetType(WebInputEvent::Type::kTouchEnd);
  result = utils.DidNativeEmbedEvent(event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  event.touches_length = 1;
  event.SetType(WebInputEvent::Type::kTouchCancel);
  result = utils.DidNativeEmbedEvent(event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
}

TEST_F(InputHandlerProxyUtilsTest, NativeHitTestResult_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  SetHitTestingNumber(&utils, 0);
  utils.NativeHitTestResult(true, 0, 0);
  EXPECT_NE(GetHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, NativeHitTestResult_003) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  SetHitTestingNumber(&utils, 1);
  WebTouchEvent touch_event;
  WebTouchEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetTouchEvent(&utils, touch_event, info, std::move(metrics), event);
  SetEndIndexQueue(&utils, 0);
  utils.NativeHitTestResult(false, 0, 0);
  EXPECT_EQ(GetHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, NativeHitTestResult_004) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  WebTouchEvent touch_event;
  WebTouchEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetTouchEvent(&utils, touch_event, info, std::move(metrics), event);
  SetHitTestingNumber(&utils, 1);
  SetEndIndexQueue(&utils, 0);
  utils.NativeHitTestResult(true, 0, 0);
  EXPECT_EQ(GetHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, NativeHitTestResult_005) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  WebTouchEvent touch_event;
  WebTouchEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetTouchEvent(&utils, touch_event, info, std::move(metrics), event);
  SetHitTestingNumber(&utils, 1);
  SetEndIndexQueue(&utils, 0);
  utils.NativeHitTestResult(false, 0, 0);
  EXPECT_EQ(GetHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, NativeHitTestResult_006) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  WebTouchEvent touch_event;
  WebTouchEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetTouchEvent(&utils, touch_event, info, std::move(metrics), event);
  SetHitTestingNumber(&utils, 1);
  SetEndIndexQueue(&utils, 0);
  utils.NativeHitTestResult(false, 0, 0);
  EXPECT_EQ(GetHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, NativeMouseHitTestResult_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  SetMouseHitTestingNumber(&utils, 0);
  utils.NativeMouseHitTestResult(true, 0, 0);
  EXPECT_NE(GetMouseHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, NativeMouseHitTestResult_002) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  SetMouseHitTestingNumber(&utils, 1);
  utils.NativeMouseHitTestResult(false, 0, 0);
  EXPECT_EQ(GetMouseHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, NativeMouseHitTestResult_003) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  SetMouseHitTestingNumber(&utils, 1);
  WebMouseEvent touch_event;
  WebMouseEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetMouseEventEnd(&utils, touch_event, info, std::move(metrics), event);
  utils.NativeMouseHitTestResult(false, 0, 0);
  EXPECT_EQ(GetHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, NativeMouseHitTestResult_004) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  WebMouseEvent touch_event;
  WebMouseEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetMouseEventEnd(&utils, touch_event, info, std::move(metrics), event);
  SetMouseHitTestingNumber(&utils, 1);
  utils.NativeMouseHitTestResult(true, 0, 0);
}

TEST_F(InputHandlerProxyUtilsTest, SendMouseNativeEvent_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  WebTouchEvent touch_event;
  blink::WebMouseEvent mouse_event(
      blink::WebInputEvent::Type::kMouseMove,
      blink::WebInputEvent::kNoModifiers,
      blink::WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.pointer_type = blink::WebPointerProperties::PointerType::kMouse;
  utils.SendMouseNativeEvent(mouse_event, WebInputEvent::Type::kMouseDown, 0,
                             true);
}

TEST_F(InputHandlerProxyUtilsTest, DidMouseEmbedEvent_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  blink::WebMouseEvent mouse_event(
      blink::WebInputEvent::Type::kMouseMove,
      blink::WebInputEvent::kNoModifiers,
      blink::WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.pointer_type = blink::WebPointerProperties::PointerType::kMouse;
  auto result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  mouse_event.SetType(blink::WebInputEvent::Type::kTouchStart);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  mouse_event.SetType(blink::WebInputEvent::Type::kMouseMove);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kLeftButtonDown);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  mouse_event.SetType(blink::WebInputEvent::Type::kMouseMove);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kLeftButtonDown |
                           WebInputEvent::Modifiers::kIsAutoRepeat);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  mouse_event.SetType(blink::WebInputEvent::Type::kMouseMove);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kRightButtonDown);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  mouse_event.SetType(blink::WebInputEvent::Type::kMouseMove);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kRightButtonDown |
                           WebInputEvent::Modifiers::kIsAutoRepeat);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  mouse_event.SetType(blink::WebInputEvent::Type::kMouseMove);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kMiddleButtonDown);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  mouse_event.SetType(blink::WebInputEvent::Type::kMouseMove);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kMiddleButtonDown |
                           WebInputEvent::Modifiers::kIsAutoRepeat);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
}

TEST_F(InputHandlerProxyUtilsTest, DidMouseEmbedEvent_003) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  blink::WebMouseEvent mouse_event(
      blink::WebInputEvent::Type::kMouseDown,
      blink::WebInputEvent::kNoModifiers,
      blink::WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kLeftButtonDown);
  auto result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kLeftButtonDown |
                           WebInputEvent::Modifiers::kIsAutoRepeat);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kRightButtonDown);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kRightButtonDown |
                           WebInputEvent::Modifiers::kIsAutoRepeat);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kMiddleButtonDown);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kMiddleButtonDown |
                           WebInputEvent::Modifiers::kIsAutoRepeat);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::NORMAL);
}

TEST_F(InputHandlerProxyUtilsTest, DidMouseEmbedEvent_004) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  blink::WebMouseEvent mouse_event(
      blink::WebInputEvent::Type::kMouseMove,
      blink::WebInputEvent::kNoModifiers,
      blink::WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kLeftButtonDown);
  SetMouseNativeMap(&utils, WebInputEvent::Modifiers::kLeftButtonDown, true);
  auto result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result,
            InputHandlerProxyUtils::NativeEventDisposition::SEND_NATIVE);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kLeftButtonDown |
                           WebInputEvent::Modifiers::kIsAutoRepeat);
  SetMouseNativeMap(&utils,
                    WebInputEvent::Modifiers::kLeftButtonDown |
                        WebInputEvent::Modifiers::kIsAutoRepeat,
                    true);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result,
            InputHandlerProxyUtils::NativeEventDisposition::SEND_NATIVE);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kRightButtonDown);
  SetMouseNativeMap(&utils, WebInputEvent::Modifiers::kRightButtonDown, true);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result,
            InputHandlerProxyUtils::NativeEventDisposition::SEND_NATIVE);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kRightButtonDown |
                           WebInputEvent::Modifiers::kIsAutoRepeat);
  SetMouseNativeMap(&utils,
                    WebInputEvent::Modifiers::kRightButtonDown |
                        WebInputEvent::Modifiers::kIsAutoRepeat,
                    true);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result,
            InputHandlerProxyUtils::NativeEventDisposition::SEND_NATIVE);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kMiddleButtonDown);
  SetMouseNativeMap(&utils, WebInputEvent::Modifiers::kMiddleButtonDown, true);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result,
            InputHandlerProxyUtils::NativeEventDisposition::SEND_NATIVE);
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kMiddleButtonDown |
                           WebInputEvent::Modifiers::kIsAutoRepeat);
  SetMouseNativeMap(&utils,
                    WebInputEvent::Modifiers::kMiddleButtonDown |
                        WebInputEvent::Modifiers::kIsAutoRepeat,
                    true);
  result = utils.DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result,
            InputHandlerProxyUtils::NativeEventDisposition::SEND_NATIVE);
}

TEST_F(InputHandlerProxyUtilsTest, SetGestureEventResult_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  utils.SetGestureEventResult(false, false);
  WebTouchEvent touch_event;
  WebTouchEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetEvent(&utils, touch_event, info, std::move(metrics), event);
  utils.SetGestureEventResult(false, false);
  EXPECT_EQ(GetHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, SetGestureEventResult_002) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  WebTouchEvent touch_event;
  WebTouchEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetEvent(&utils, touch_event, info, std::move(metrics), event);
  utils.SetGestureEventResult(true, true);
  EXPECT_EQ(GetHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, SetGestureEventResult_003) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  WebTouchEvent touch_event;
  WebTouchEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetEvent(&utils, touch_event, info, std::move(metrics), event);
  utils.SetGestureEventResult(false, true);
  EXPECT_EQ(GetHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, SetMouseEventResult_003) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  WebMouseEvent touch_event;
  WebMouseEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetMouseEvent(&utils, touch_event, info, std::move(metrics), event);
  utils.SetMouseEventResult(false, true);
  EXPECT_EQ(GetHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, SetMouseEventResult_004) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  WebMouseEvent touch_event;
  WebMouseEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetMouseEvent(&utils, touch_event, info, std::move(metrics), event);
  utils.SetMouseEventResult(true, false);
  EXPECT_EQ(GetHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, SetNativeEmbedMode_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  utils.SetNativeEmbedMode(true);
  EXPECT_EQ(GetNativeEnabled(&utils), true);
  utils.SetNativeEmbedMode(false);
  EXPECT_EQ(GetNativeEnabled(&utils), false);
}

TEST_F(InputHandlerProxyUtilsTest, NeedFlushScrollUpdateGesture_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  SetNeedFlushScrollUpdateGesture(&utils, true);
  WebGestureEvent gesture_event;
  gesture_event.SetType(WebGestureEvent::Type::kGestureScrollUpdate);
  utils.NeedFlushScrollUpdateGesture(gesture_event);
  SetNeedFlushScrollUpdateGesture(&utils, true);
  gesture_event.SetType(WebGestureEvent::Type::kGestureFlingStart);
  utils.NeedFlushScrollUpdateGesture(gesture_event);
  SetNeedFlushScrollUpdateGesture(&utils, false);
  gesture_event.SetType(WebGestureEvent::Type::kGestureFlingStart);
  utils.NeedFlushScrollUpdateGesture(gesture_event);
}

TEST_F(InputHandlerProxyUtilsTest, SetBypassVsyncCondition_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  utils.SetBypassVsyncCondition(0);
}

TEST_F(InputHandlerProxyUtilsTest, GetOverScrollOffset_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  utils.GetOverScrollOffset();
}

TEST_F(InputHandlerProxyUtilsTest, SetEnableCustomVideoPlayer_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  utils.SetEnableCustomVideoPlayer(true);
  EXPECT_EQ(GetEnableCustomVideoPlayer(&utils), true);
}
#pragma clang diagnostic pop
}  // namespace blink