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
#include "arkweb/chromium_ext/cc/input/input_handler_utils.h"
#include "arkweb/chromium_ext/cc/trees/layer_tree_impl_utils.h"
#include "cc/mojo_embedder/software_compositor_renderer_ohos.h"
#include "cc/test/fake_impl_task_runner_provider.h"
#include "cc/test/fake_layer_tree_host.h"
#include "cc/test/fake_layer_tree_host_client.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/test_task_graph_runner.h"
#include "cc/trees/latency_info_swap_promise_monitor.h"
#include "cc/trees/layer_tree_settings.h"
#include "cc/test/layer_tree_impl_test_base.h"
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
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"
namespace blink {
using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::AtLeast;

class MockInputHandlerUtils : public cc::InputHandlerUtils {
 public:
  explicit MockInputHandlerUtils(cc::InputHandler* handler)
      : cc::InputHandlerUtils(handler){};
  ~MockInputHandlerUtils() {}

  MOCK_METHOD1(GetLayerImplIsHitByPoint,
               cc::LayerImpl*(const gfx::Point& viewport_point));
  MOCK_METHOD1(GetNativeLayerImpl,
               cc::LayerImpl*(const gfx::Point& viewport_point));
  MOCK_METHOD1(GetLayerImplById, cc::LayerImpl*(int));
};

// mock_layer_impl.h
class MockLayerImpl : public cc::LayerImpl {
 public:
  MockLayerImpl(cc::LayerTreeImpl* tree_impl,
                int id,
                bool will_always_push_properties)
      : cc::LayerImpl(tree_impl, id, will_always_push_properties){};
  ~MockLayerImpl(){};

  MOCK_METHOD0(layer_impl_utils, cc::LayerImplUtils*());
};

class MockLayerImplUtils : public cc::LayerImplUtils {
 public:
  explicit MockLayerImplUtils(cc::LayerImpl* layer_impl)
      : cc::LayerImplUtils(layer_impl){};
  ~MockLayerImplUtils(){};
};

class MockLayerTreeImpl : public cc::LayerTreeImplTestBase {
 public:
  MockLayerTreeImpl() = default;
  explicit MockLayerTreeImpl(const cc::LayerTreeSettings& settings)
      : cc::LayerTreeImplTestBase(settings) {}

  cc::FakeLayerTreeHostImpl& host_impl() const {
    return *cc::LayerTreeImplTestBase::host_impl();
  }
};

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

  MOCK_METHOD0(handler_utils, cc::InputHandlerUtils*());

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

  bool IsNativeMouseEventQueueEmpty(InputHandlerProxyUtils* utils) {
    if (!utils->native_mouse_event_queue_) {
      return false;
    }
    return utils->native_mouse_event_queue_->empty();
  }
  bool IsNativeMouseEndQueueEmpty(InputHandlerProxyUtils* utils) {
    if (!utils->native_mouse_end_queue_) {
      return false;
    }
    return utils->native_mouse_end_queue_->empty();
  }
  gfx::RectF& GetNativeRectByFingerId(InputHandlerProxyUtils* utils, int32_t finger_id) {
    return utils->nativeRects_[finger_id];
  }
  void SetNativeRectByFingerId(InputHandlerProxyUtils* utils, const gfx::RectF& rect,
                                       int32_t finger_id) {
    utils->nativeRects_[finger_id] = rect;
  }
  WebInputEvent::Type GetGestureStatusByFingerId(InputHandlerProxyUtils* utils, int32_t finger_id) {
    return utils->gesture_status_[finger_id];
  }
  void SetGestureStatusByFingerId(InputHandlerProxyUtils* utils, WebInputEvent::Type status,
                                       int32_t finger_id) {
    utils->gesture_status_[finger_id] = status;
  }

 protected:
  base::test::SingleThreadTaskEnvironment task_environment_;
  cc::FakeLayerTreeHostClient fake_client_;
  cc::FakeImplTaskRunnerProvider task_runner_provider_;
  cc::TestTaskGraphRunner task_graph_runner_;
  std::unique_ptr<cc::AnimationHost> animation_host_;
  std::unique_ptr<cc::FakeLayerTreeHost> layer_tree_host_;
  cc::FakeLayerTreeHostImpl host_impl_;
  std::unique_ptr<testing::NiceMock<MockInputHandler>> mock_input_handler_;
  std::unique_ptr<testing::NiceMock<MockInputHandlerProxyClient>> mock_client_;
  std::unique_ptr<testing::NiceMock<MockSynchronousInputHandler>>
      mock_synchronous_handler_;
  std::unique_ptr<testing::NiceMock<MockLayerTreeImpl>> mock_layer_tree_impl_;
  std::unique_ptr<testing::NiceMock<MockLayerImpl>> mock_layer_impl_;
  std::unique_ptr<testing::NiceMock<MockLayerImplUtils>> mock_layer_utils_;
  std::unique_ptr<testing::NiceMock<MockInputHandlerUtils>>
      mock_input_handler_utils_;
  std::unique_ptr<InputHandlerProxy> proxy_;
  std::unique_ptr<InputHandlerProxyUtils> utils_;
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
  const cc::LayerTreeSettings settings;
  mock_input_handler_ = std::make_unique<testing::NiceMock<MockInputHandler>>();
  mock_client_ =
      std::make_unique<testing::NiceMock<MockInputHandlerProxyClient>>();
  mock_synchronous_handler_ =
      std::make_unique<testing::NiceMock<MockSynchronousInputHandler>>();
  mock_layer_tree_impl_ =
      std::make_unique<testing::NiceMock<MockLayerTreeImpl>>(settings);
  mock_layer_impl_ =
      std::make_unique<testing::NiceMock<MockLayerImpl>>(mock_layer_tree_impl_->host_impl().active_tree(), 1, false);
  mock_layer_utils_ =
      std::make_unique<testing::NiceMock<MockLayerImplUtils>>(mock_layer_impl_.get());
  mock_input_handler_utils_ =
      std::make_unique<testing::NiceMock<MockInputHandlerUtils>>(
          mock_input_handler_.get());

  proxy_ = std::make_unique<InputHandlerProxy>(*mock_input_handler_,
                                               mock_client_.get());
  proxy_->SetSynchronousInputHandler(mock_synchronous_handler_.get());
  utils_ = std::make_unique<InputHandlerProxyUtils>(proxy_.get());

  // Setup default mock behaviors
  ON_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillByDefault(Return(nullptr));
  ON_CALL(*mock_input_handler_utils_, GetNativeLayerImpl(_))
      .WillByDefault(Return(nullptr));
  ON_CALL(*mock_input_handler_utils_, GetLayerImplById(_))
      .WillByDefault(Return(nullptr));
  ON_CALL(*mock_layer_impl_, layer_impl_utils())
    .WillByDefault(Return(mock_layer_utils_.get()));
  ON_CALL(*mock_input_handler_, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
}

void InputHandlerProxyUtilsTest::TearDown(void) {
  layer_tree_host_->SetRootLayer(nullptr);
  layer_tree_host_ = nullptr;
}

// Helper function to create touch events
std::unique_ptr<EventWithCallback> CreateTouchEventCallback(
    WebInputEvent::Type type,
    int finger_id,
    WebTouchPoint::State state) {
  WebTouchEvent touch_event;
  touch_event.SetType(type);
  touch_event.touches_length = 1;
  touch_event.touches[0].id = finger_id;
  touch_event.touches[0].state = state;
  std::unique_ptr<cc::EventMetrics> metrics;
  const ui::LatencyInfo info;
  return std::make_unique<EventWithCallback>(
      std::make_unique<WebCoalescedInputEvent>(touch_event, info),
      base::DoNothing(), std::move(metrics));
}

WebTouchEvent CreateTouchEvent(WebInputEvent::Type type,
                               int finger_id,
                               WebTouchPoint::State state) {
  WebTouchEvent touch_event;
  touch_event.SetType(type);
  touch_event.touches_length = 1;
  touch_event.touches[0].id = finger_id;
  touch_event.touches[0].state = state;
  return touch_event;
}

// Helper function to create mouse events
std::unique_ptr<EventWithCallback> CreateMouseEventCallback(
    WebInputEvent::Type type,
    int modifiers_param) {
  WebMouseEvent mouse_event(type, modifiers_param,
                            WebInputEvent::GetStaticTimeStampForTests());
  std::unique_ptr<cc::EventMetrics> metrics;
  const ui::LatencyInfo info;
  return std::make_unique<EventWithCallback>(
      std::make_unique<WebCoalescedInputEvent>(mouse_event, info),
      base::DoNothing(), std::move(metrics));
}

TEST_F(InputHandlerProxyUtilsTest, ResetNeedFlushScrollUpdateGesture_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
  WebTouchEvent touch_event;
  utils.SendNativeEvent(touch_event, WebInputEvent::Type::kTouchStart, 0);
}

TEST_F(InputHandlerProxyUtilsTest, SendNativeEvent_002) {
  WebTouchEvent touch_event = CreateTouchEvent(WebInputEvent::Type::kTouchStart, 0,
      WebTouchPoint::State::kStatePressed);
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplById(0))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  int layer_id = 88; // 88 is the id of the layer
  int finger_id = 2; // 2 is the finger id
  size_t i = 0;
  utils_->NativeHitTestResult(true, finger_id, layer_id);
  touch_event.SetType(blink::WebInputEvent::Type::kTouchStart);
  touch_event.touches_length = 1;
  touch_event.touches[i].id = finger_id;
  touch_event.touches[i].SetPositionInWidget(gfx::PointF(15, 25)); // x=15, y=25
  touch_event.touches[i].state = blink::WebTouchPoint::State::kStatePressed;
  EXPECT_CALL(*mock_client_,
      DidNativeEmbedEvent(WebInputEvent::Type::kTouchStart, "0", 100, 0, 0)); // 100 is the hit testing number
  utils_->SendNativeEvent(touch_event, blink::WebInputEvent::Type::kTouchStart, i, false);
}

TEST_F(InputHandlerProxyUtilsTest, SendNativeEvent_003) {
  int layer_id = 88; // 88 is the id of the layer
  int finger_id = 2; // 2 is the finger id
  size_t i = 0;
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplById(0))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  WebTouchEvent touch_start_event;
  touch_start_event.SetType(blink::WebInputEvent::Type::kTouchStart);
  touch_start_event.touches_length = 1;
  touch_start_event.touches[i].id = finger_id;
  touch_start_event.touches[i].SetPositionInWidget(gfx::PointF(10, 20)); // x=10, y=20
  touch_start_event.touches[i].state = blink::WebTouchPoint::State::kStatePressed;
  utils_->SendNativeEvent(touch_start_event, blink::WebInputEvent::Type::kTouchStart, i);
  WebTouchEvent touch_end_event;
  touch_end_event.SetType(blink::WebInputEvent::Type::kTouchEnd);
  touch_end_event.touches_length = 1;
  touch_end_event.touches[i].id = finger_id;
  touch_end_event.touches[i].SetPositionInWidget(gfx::PointF(15, 25)); // x=15, y=25
  touch_end_event.touches[i].state = blink::WebTouchPoint::State::kStateReleased;
  EXPECT_CALL(*mock_client_, DidNativeEmbedEvent(WebInputEvent::Type::kTouchEnd,
      "0",finger_id, 15, 25)); // 15, 25 is the position of the touch end event
  utils_->SendNativeEvent(touch_end_event, blink::WebInputEvent::Type::kTouchEnd, i);
}

TEST_F(InputHandlerProxyUtilsTest, DidNativeEmbedEvent_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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

TEST_F(InputHandlerProxyUtilsTest, DidNativeEmbedEvent_VideoLayerIntercept) {
  WebTouchEvent touch_event = CreateTouchEvent(WebInputEvent::Type::kTouchStart, 0,
      WebTouchPoint::State::kStatePressed);
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  auto result = utils_->DidNativeEmbedEvent(touch_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::SEND_VIDEO);
}

TEST_F(InputHandlerProxyUtilsTest, DidNativeEmbedEvent_NativeLayerImpl) {
  WebTouchEvent touch_event = CreateTouchEvent(WebInputEvent::Type::kTouchStart, 0,
      WebTouchPoint::State::kStatePressed);
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  SetNativeEnabled(utils_.get(), true);
  EXPECT_CALL(*mock_input_handler_utils_, GetNativeLayerImpl(_))
      .WillOnce(Return(mock_layer_impl_.get()));
  auto result = utils_->DidNativeEmbedEvent(touch_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::SEND_NATIVE);
}

TEST_F(InputHandlerProxyUtilsTest, DidNativeEmbedEvent_NativeLayerImplNull) {
  WebTouchEvent touch_event = CreateTouchEvent(WebInputEvent::Type::kTouchStart, 0,
      WebTouchPoint::State::kStatePressed);
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  SetNativeEnabled(utils_.get(), true);
  EXPECT_CALL(*mock_input_handler_utils_, GetNativeLayerImpl(_))
      .WillOnce(Return(nullptr));
  auto result = utils_->DidNativeEmbedEvent(touch_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NORMAL);
}

TEST_F(InputHandlerProxyUtilsTest, DidNativeEmbedEvent_kTouchMove) {
  WebTouchEvent touch_event = CreateTouchEvent(WebInputEvent::Type::kTouchMove, 0,
      WebTouchPoint::State::kStateMoved);
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  SetNativeEnabled(utils_.get(), true);
  utils_->NativeHitTestResult(true, touch_event.touches[0].id, 88); // 88 is the layer id
  SetHitTestingNumber(utils_.get(), 1);
  auto result = utils_->DidNativeEmbedEvent(touch_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::SEND_NATIVE);
}

TEST_F(InputHandlerProxyUtilsTest, DidNativeEmbedEvent_kTouchEndWithNumber) {
  WebTouchEvent touch_event = CreateTouchEvent(WebInputEvent::Type::kTouchEnd, 0,
      WebTouchPoint::State::kStateReleased);
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  SetNativeEnabled(utils_.get(), true);
  utils_->NativeHitTestResult(true, touch_event.touches[0].id, 88); // 88 is the layer id
  SetHitTestingNumber(utils_.get(), 1);
  auto result = utils_->DidNativeEmbedEvent(touch_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::END_QUEUE);
}

TEST_F(InputHandlerProxyUtilsTest, DidNativeEmbedEvent_kTouchEndWithOutNumber) {
  WebTouchEvent touch_event = CreateTouchEvent(WebInputEvent::Type::kTouchEnd, 0,
      WebTouchPoint::State::kStateReleased);
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  SetNativeEnabled(utils_.get(), true);
  utils_->NativeHitTestResult(true, touch_event.touches[0].id, 88); // 88 is the layer id
  SetHitTestingNumber(utils_.get(), 0);
  auto result = utils_->DidNativeEmbedEvent(touch_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::SEND_NATIVE);
}

TEST_F(InputHandlerProxyUtilsTest, NativeHitTestResult_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
  WebMouseEvent touch_event;
  WebMouseEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetMouseEventEnd(&utils, touch_event, info, std::move(metrics), event);
  SetMouseHitTestingNumber(&utils, 1);
  utils.NativeMouseHitTestResult(true, 0, 0);
}

TEST_F(InputHandlerProxyUtilsTest, NativeMouseHitTestResult_005) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
  SetMouseHitTestingNumber(&utils, 1);
  WebMouseEvent mouse_event(blink::WebInputEvent::Type::kMouseDown,
                            blink::WebInputEvent::kNoModifiers,
                            blink::WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.pointer_type = blink::WebPointerProperties::PointerType::kMouse;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetMouseEvent(&utils, mouse_event, info, std::move(metrics), mouse_event);
  auto modifiers = static_cast<WebInputEvent::Modifiers>(mouse_event.GetModifiers());
  EXPECT_CALL(mock_client, DidNativeEmbedMouseEvent(
      blink::WebInputEvent::Type::kMouseDown, modifiers, "-1", false, 0, 0));
  utils.NativeMouseHitTestResult(false, 0, 0);
  EXPECT_EQ(GetHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, SendMouseNativeEvent_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
  WebTouchEvent touch_event;
  blink::WebMouseEvent mouse_event(
      blink::WebInputEvent::Type::kMouseMove,
      blink::WebInputEvent::kNoModifiers,
      blink::WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.pointer_type = blink::WebPointerProperties::PointerType::kMouse;
  utils.SendMouseNativeEvent(mouse_event, WebInputEvent::Type::kMouseDown, 0,
                             true);
}

TEST_F(InputHandlerProxyUtilsTest, SendMouseNativeEvent_002) {
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplById(0))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  blink::WebMouseEvent mouse_event(
      blink::WebInputEvent::Type::kMouseMove,
      blink::WebInputEvent::kNoModifiers,
      blink::WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetPositionInWidget(gfx::PointF(15, 25)); // 15, 25 is the position of the mouse down event
  auto modifiers = static_cast<WebInputEvent::Modifiers>(mouse_event.GetModifiers());
  EXPECT_CALL(*mock_client_, DidNativeEmbedMouseEvent(blink::WebInputEvent::Type::kMouseDown, modifiers,
      "0", true, 15, 25)); // 15, 25 is the position of the mouse down event
  utils_->SendMouseNativeEvent(mouse_event, blink::WebInputEvent::Type::kMouseDown, 1, true);
}

TEST_F(InputHandlerProxyUtilsTest, SendMouseNativeEvent_003) {
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplById(0))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  blink::WebMouseEvent mouse_event(
      blink::WebInputEvent::Type::kMouseMove,
      blink::WebInputEvent::kNoModifiers,
      blink::WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetPositionInWidget(gfx::PointF(15, 25)); // 15, 25 is the position of the mouse move event
  auto modifiers = static_cast<WebInputEvent::Modifiers>(mouse_event.GetModifiers());
  EXPECT_CALL(*mock_client_, DidNativeEmbedMouseEvent(blink::WebInputEvent::Type::kMouseMove, modifiers,
      "0", true, 15, 25)); // 15, 25 is the position of the mouse move event
  utils_->SendMouseNativeEvent(mouse_event, blink::WebInputEvent::Type::kMouseMove, 1, true);
}

TEST_F(InputHandlerProxyUtilsTest, DidMouseEmbedEvent_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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

TEST_F(InputHandlerProxyUtilsTest, DidMouseEmbedEvent_006) {
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  SetNativeEnabled(utils_.get(), true);
  blink::WebMouseEvent mouse_event(
      blink::WebInputEvent::Type::kMouseDown,
      blink::WebInputEvent::kNoModifiers,
      blink::WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kMiddleButtonDown | WebInputEvent::Modifiers::kIsAutoRepeat);
  auto result = utils_->DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::SEND_VIDEO);
}

TEST_F(InputHandlerProxyUtilsTest, DidMouseEmbedEvent_007) {
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  SetNativeEnabled(utils_.get(), true);
  EXPECT_CALL(*mock_input_handler_utils_, GetNativeLayerImpl(_))
      .WillOnce(Return(mock_layer_impl_.get()));
  blink::WebMouseEvent mouse_event(
      blink::WebInputEvent::Type::kMouseDown,
      blink::WebInputEvent::kNoModifiers,
      blink::WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.SetModifiers(WebInputEvent::Modifiers::kMiddleButtonDown | WebInputEvent::Modifiers::kIsAutoRepeat);
  auto result = utils_->DidMouseEmbedEvent(mouse_event);
  EXPECT_EQ(result, InputHandlerProxyUtils::NativeEventDisposition::SEND_NATIVE);
}

TEST_F(InputHandlerProxyUtilsTest, DidNativeSendEvent_TouchEvent) {
  SetNativeEnabled(utils_.get(), true);
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed);
  utils_->DidNativeSendEvent(std::move(event));
  EXPECT_TRUE(IsNativeMouseEventQueueEmpty(utils_.get()));
}

TEST_F(InputHandlerProxyUtilsTest, DidNativeSendEvent_MouseEvent) {
  SetNativeEnabled(utils_.get(), true);
  auto event = CreateMouseEventCallback(WebInputEvent::Type::kMouseDown, WebInputEvent::Modifiers::kLeftButtonDown);
  utils_->DidNativeSendEvent(std::move(event));
  EXPECT_TRUE(IsNativeMouseEventQueueEmpty(utils_.get()));
}

TEST_F(InputHandlerProxyUtilsTest, DidNativeSendEvent_SendNative) {
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  SetNativeEnabled(utils_.get(), true);
  EXPECT_EQ(GetNativeEnabled(utils_.get()), true);
  EXPECT_CALL(*mock_input_handler_utils_, GetNativeLayerImpl(_))
      .WillOnce(Return(mock_layer_impl_.get()));
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed);
  SetMouseNativeMap(utils_.get(), WebInputEvent::Modifiers::kLeftButtonDown, true);
  utils_->DidNativeSendEvent(std::move(event));
  EXPECT_TRUE(IsNativeMouseEventQueueEmpty(utils_.get()));
}

TEST_F(InputHandlerProxyUtilsTest, DidNativeSendEvent_SendVedio) {
  auto event = CreateMouseEventCallback(WebInputEvent::Type::kMouseDown, WebInputEvent::Modifiers::kLeftButtonDown);
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  utils_->DidNativeSendEvent(std::move(event));
  EXPECT_FALSE(IsNativeMouseEventQueueEmpty(utils_.get()));
}

TEST_F(InputHandlerProxyUtilsTest, DidNativeSendEvent_MouseEndQueue) {
  auto event = CreateMouseEventCallback(WebInputEvent::Type::kMouseUp, WebInputEvent::Modifiers::kLeftButtonDown);
  SetMouseHitTestingNumber(utils_.get(), 1);
  utils_->DidNativeSendEvent(std::move(event));
  EXPECT_FALSE(IsNativeMouseEndQueueEmpty(utils_.get()));
}

TEST_F(InputHandlerProxyUtilsTest, DidNativeSendEvent_TouchEndQueue) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchEnd, 0, WebTouchPoint::State::kStateReleased);
  SetHitTestingNumber(utils_.get(), 1);
  utils_->DidNativeSendEvent(std::move(event));
  EXPECT_TRUE(IsNativeMouseEndQueueEmpty(utils_.get()));
}

TEST_F(InputHandlerProxyUtilsTest, DidNativeSendEvent_Normal) {
  auto event = CreateMouseEventCallback(WebInputEvent::Type::kMouseDown, WebInputEvent::Modifiers::kRightButtonDown);
  utils_->DidNativeSendEvent(std::move(event));
  EXPECT_TRUE(IsNativeMouseEventQueueEmpty(utils_.get()));
}

TEST_F(InputHandlerProxyUtilsTest, SetGestureEventResult_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
  WebTouchEvent touch_event;
  WebTouchEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetEvent(&utils, touch_event, info, std::move(metrics), event);
  utils.SetGestureEventResult(false, true);
  EXPECT_EQ(GetHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, SetGestureEventResult_004) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  ON_CALL(mock_input_handler, handler_utils()).WillByDefault(Return(mock_input_handler_utils_.get()));
  WebTouchEvent touch_event;
  WebTouchEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetEvent(&utils, touch_event, info, std::move(metrics), event);
  utils.SetGestureEventResult(true, false);
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
  WebMouseEvent touch_event;
  WebMouseEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetMouseEvent(&utils, touch_event, info, std::move(metrics), event);
  utils.SetMouseEventResult(true, false);
  EXPECT_EQ(GetHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, SetMouseEventResult_005) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  ON_CALL(mock_input_handler, handler_utils()).WillByDefault(Return(mock_input_handler_utils_.get()));
  WebMouseEvent touch_event;
  WebMouseEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  SetMouseEvent(&utils, touch_event, info, std::move(metrics), event);
  utils.SetMouseEventResult(true, true);
  EXPECT_EQ(GetHitTestNumber(&utils), 0);
}

TEST_F(InputHandlerProxyUtilsTest, SetMouseEventResult_006) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  ON_CALL(mock_input_handler, handler_utils()).WillByDefault(Return(mock_input_handler_utils_.get()));
  WebMouseEvent touch_event;
  WebMouseEvent event;
  const ui::LatencyInfo info;
  std::unique_ptr<cc::EventMetrics> metrics;
  utils.SetMouseEventResult(true, true);
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
  utils.SetBypassVsyncCondition(0);
}

TEST_F(InputHandlerProxyUtilsTest, SetOverscrollMode_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  proxy.UpdateElasticOverscroll();
  InputHandlerProxyUtils utils(&proxy);
  ON_CALL(mock_input_handler, handler_utils()).WillByDefault(Return(mock_input_handler_utils_.get()));
  utils.SetOverscrollMode(0);
}

TEST_F(InputHandlerProxyUtilsTest, GetOverScrollOffset_001) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  InputHandlerProxyUtils utils(&proxy);
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
  utils.GetOverScrollOffset();
}

TEST_F(InputHandlerProxyUtilsTest, GetOverScrollOffset_002) {
  testing::NiceMock<MockInputHandler> mock_input_handler;
  testing::StrictMock<MockInputHandlerProxyClient> mock_client;
  testing::StrictMock<MockSynchronousInputHandler>
      mock_synchronous_input_handler;
  InputHandlerProxy proxy(mock_input_handler, &mock_client);
  proxy.SetSynchronousInputHandler(&mock_synchronous_input_handler);
  proxy.UpdateElasticOverscroll();
  InputHandlerProxyUtils utils(&proxy);
  ON_CALL(mock_input_handler, handler_utils()).WillByDefault(Return(mock_input_handler_utils_.get()));
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
  ON_CALL(mock_input_handler, handler_utils())
    .WillByDefault(Return(mock_input_handler_utils_.get()));
  utils.SetEnableCustomVideoPlayer(true);
  EXPECT_EQ(GetEnableCustomVideoPlayer(&utils), true);
}


TEST_F(InputHandlerProxyUtilsTest, NativeEventProcess_NonEnabled) {
  SetNativeEnabled(utils_.get(), false);
  utils_->SetEnableCustomVideoPlayer(false);

  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  auto queue = utils_->NativeTouchEventQueues_[0];
  utils_->NativeEventProcess(std::move(event));
  EXPECT_TRUE(queue->empty());
}

TEST_F(InputHandlerProxyUtilsTest, NativeEventProcess_MouseEvent) {
  SetNativeEnabled(utils_.get(), true);
  auto event = CreateMouseEventCallback(WebInputEvent::Type::kMouseDown, WebInputEvent::Modifiers::kLeftButtonDown);

  // Will call NativeMouseEventProcess which we'll test separately
  utils_->NativeEventProcess(std::move(event));
  EXPECT_TRUE(IsNativeMouseEventQueueEmpty(utils_.get()));
}

TEST_F(InputHandlerProxyUtilsTest, NativeEventProcess_TouchEvent) {
  SetNativeEnabled(utils_.get(), true);

  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Will call NativeTouchEventProcess which we'll test separately
  utils_->NativeEventProcess(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeEventProcess_EnableCustomVideoPlayer) {
  SetNativeEnabled(utils_.get(), false);
  utils_->SetEnableCustomVideoPlayer(true);
  auto event = CreateMouseEventCallback(WebInputEvent::Type::kMouseDown, WebInputEvent::Modifiers::kLeftButtonDown);
  utils_->NativeEventProcess(std::move(event));
  EXPECT_TRUE(IsNativeMouseEventQueueEmpty(utils_.get()));
}

TEST_F(InputHandlerProxyUtilsTest, NativeEventProcess_IsTouchEventTypeFalse) {
  SetNativeEnabled(utils_.get(), false);
  utils_->SetEnableCustomVideoPlayer(true);
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchTypeLast, 0, WebTouchPoint::State::kStatePressed);
  utils_->NativeEventProcess(std::move(event));
  EXPECT_TRUE(IsNativeMouseEventQueueEmpty(utils_.get()));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchEventProcess_TouchStart) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Test both isStartInQueue cases
  utils_->NativeTouchEventProcess(std::move(event), false);

  auto event2 = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                         WebTouchPoint::State::kStatePressed);
  utils_->NativeTouchEventProcess(std::move(event2), true);
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchEventProcess_TouchMove) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateMoved);

  utils_->NativeTouchEventProcess(std::move(event), false);
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchEventProcess_TouchEnd) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchEnd, 0,
                                        WebTouchPoint::State::kStateReleased);

  utils_->NativeTouchEventProcess(std::move(event), false);
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchEventProcess_TouchCancel) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchCancel, 0,
                                        WebTouchPoint::State::kStateCancelled);

  utils_->NativeTouchEventProcess(std::move(event), false);
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchEventProcess_UnknownType) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kGestureTap, 0,
                                        WebTouchPoint::State::kStateStationary);

  // Should do nothing for unknown types
  utils_->NativeTouchEventProcess(std::move(event), false);
}

TEST_F(InputHandlerProxyUtilsTest, NativeMouseEventProcess_SendNative) {
  auto event =
      CreateMouseEventCallback(WebInputEvent::Type::kMouseMove,
                                   WebInputEvent::Modifiers::kLeftButtonDown);

  SetMouseNativeMap(utils_.get(), WebInputEvent::Modifiers::kLeftButtonDown, true);
  // Setup DidMouseEmbedEvent to return SEND_NATIVE
  utils_->NativeMouseEventProcess(std::move(event));
  EXPECT_FALSE(IsNativeMouseEventQueueEmpty(utils_.get()));
}

TEST_F(InputHandlerProxyUtilsTest, NativeMouseEventProcess_SendVedio) {
  auto event =
      CreateMouseEventCallback(WebInputEvent::Type::kMouseDown,
                                   WebInputEvent::Modifiers::kLeftButtonDown);

  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  // Setup DidMouseEmbedEvent to return SEND_NATIVE
  utils_->NativeMouseEventProcess(std::move(event));
  EXPECT_FALSE(IsNativeMouseEventQueueEmpty(utils_.get()));
}

TEST_F(InputHandlerProxyUtilsTest, NativeMouseEventProcess_EndQueue) {
  auto event = CreateMouseEventCallback(
      WebInputEvent::Type::kMouseUp, WebInputEvent::Modifiers::kLeftButtonDown);

  SetMouseHitTestingNumber(utils_.get(), 1);

  // Setup DidMouseEmbedEvent to return END_QUEUE
  utils_->NativeMouseEventProcess(std::move(event));
  EXPECT_FALSE(IsNativeMouseEndQueueEmpty(utils_.get()));
}

TEST_F(InputHandlerProxyUtilsTest, NativeMouseEventProcess_Normal) {
  auto event =
      CreateMouseEventCallback(WebInputEvent::Type::kMouseDown,
                                   WebInputEvent::Modifiers::kRightButtonDown);

  // Setup DidMouseEmbedEvent to return END_QUEUE
  utils_->NativeMouseEventProcess(std::move(event));
  EXPECT_TRUE(IsNativeMouseEventQueueEmpty(utils_.get()));
}

TEST_F(InputHandlerProxyUtilsTest, NativeHitTestResultV2_InvalidFingerId) {
  auto empty_queue = std::make_shared<NativeEmbedEventQueue>();
  utils_->NativeTouchEventQueues_[0] = empty_queue;
  empty_queue->SetLayerId(2);
  utils_->NativeHitTestResultV2(true, 100, 1);
  EXPECT_EQ(empty_queue->GetLayerId(), 0);
}

TEST_F(InputHandlerProxyUtilsTest, NativeHitTestResultV2_EmptyQueue) {
  // Setup empty queue
  auto empty_queue = std::make_shared<NativeEmbedEventQueue>();
  utils_->NativeTouchEventQueues_[0] = empty_queue;

  empty_queue->SetLayerId(2);
  utils_->NativeHitTestResultV2(true, 0, 1);
  EXPECT_EQ(empty_queue->GetLayerId(), 0);
}

TEST_F(InputHandlerProxyUtilsTest, NativeHitTestResultV2_NativeTrue) {
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed));
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeHitTestResultV2(true, 0, 1);
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::INIT);
  EXPECT_EQ(queue->GetLayerId(), 0);
}

TEST_F(InputHandlerProxyUtilsTest, NativeHitTestResultV2_NativeFalse) {
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed));
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeHitTestResultV2(false, 0, 1);
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::SEND_BLINK);
}

TEST_F(InputHandlerProxyUtilsTest,
       SendNativeInQueueFrontSeq_TouchEndOutOfLoop) {
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchEnd, 0,
                                        WebTouchPoint::State::kStateReleased));
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->SendNativeInQueueFrontSeq(0);
  EXPECT_FALSE(queue->empty());
}

TEST_F(InputHandlerProxyUtilsTest,
       SendNativeInQueueFrontSeq_QueueEmptyOutOfLoop) {
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed));
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->SendNativeInQueueFrontSeq(0);
  EXPECT_FALSE(queue->empty());
}

TEST_F(InputHandlerProxyUtilsTest,
       SendNativeInQueueFrontSeq_StackEmptyOutOfLoop) {
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed));
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateMoved));
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchEnd, 0,
                                        WebTouchPoint::State::kStateReleased));
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->SendNativeInQueueFrontSeq(0);
  EXPECT_FALSE(queue->empty());
}

TEST_F(InputHandlerProxyUtilsTest,
       HandleTouchStartIfHitVideo_InvalidChangeIndex) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 0;  // No touches

  EXPECT_FALSE(utils_->HandleTouchStartIfHitVideo(touch_event));
}

TEST_F(InputHandlerProxyUtilsTest, HandleTouchStartIfHitVideo_InvalidFingerId) {
  WebTouchEvent touch_event = CreateTouchEvent(
    WebInputEvent::Type::kTouchStart, 100, WebTouchPoint::State::kStatePressed);

  EXPECT_FALSE(utils_->HandleTouchStartIfHitVideo(touch_event));
}

TEST_F(InputHandlerProxyUtilsTest, HandleTouchStartIfHitVideo_HitVideoLayer) {
  WebTouchEvent touch_event = CreateTouchEvent(
    WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed);

  // Setup video layer hit
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);

  EXPECT_TRUE(utils_->HandleTouchStartIfHitVideo(touch_event));

  auto queue = utils_->NativeTouchEventQueues_[0];
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::PEND_NATIVE);
}

TEST_F(InputHandlerProxyUtilsTest, HandleTouchStartIfHitVideo_NoHit) {
  WebTouchEvent touch_event = CreateTouchEvent(
    WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed);

  // No layer hit
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(nullptr));

  EXPECT_FALSE(utils_->HandleTouchStartIfHitVideo(touch_event));
}

TEST_F(InputHandlerProxyUtilsTest, HandleTouchStartInit_HitTopLayer) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed);
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(gfx::Point(0, 0)))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  utils_->HandleTouchStartInit(std::move(event), 0);
  auto queue = utils_->NativeTouchEventQueues_[0];
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::PEND_NATIVE);
}

TEST_F(InputHandlerProxyUtilsTest, HandleTouchStartInit_HitNitiveLayer) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed);
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetNativeLayerImpl(gfx::Point(0, 0)))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  SetNativeEnabled(utils_.get(), true);
  utils_->HandleTouchStartInit(std::move(event), 0);
  auto queue = utils_->NativeTouchEventQueues_[0];
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::PEND_HITTEST);
}

TEST_F(InputHandlerProxyUtilsTest, HandleTouchStartInitInQueue_HitTopLayer) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed);
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(gfx::Point(0, 0)))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  utils_->HandleTouchStartInitInQueue(std::move(event), 0);
  auto queue = utils_->NativeTouchEventQueues_[0];
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::PEND_NATIVE);
}

TEST_F(InputHandlerProxyUtilsTest, HandleTouchStartInitInQueue_HitNativeLayerFalse) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed);
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetNativeLayerImpl(gfx::Point(0, 0)))
      .WillRepeatedly(Return(nullptr));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  SetNativeEnabled(utils_.get(), true);
  utils_->HandleTouchStartInitInQueue(std::move(event), 0);
}

TEST_F(InputHandlerProxyUtilsTest, HandleTouchStartInitInQueue_HitNativeLayer) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed);
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetNativeLayerImpl(gfx::Point(0, 0)))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  SetNativeEnabled(utils_.get(), true);
  utils_->HandleTouchStartInitInQueue(std::move(event), 0);
  auto queue = utils_->NativeTouchEventQueues_[0];
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::PEND_HITTEST);
}

TEST_F(InputHandlerProxyUtilsTest,
       HandleTouchStartIfHitNative_InvalidChangeIndex) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 0;  // No touches

  EXPECT_FALSE(utils_->HandleTouchStartIfHitNative(touch_event));
}

TEST_F(InputHandlerProxyUtilsTest,
       HandleTouchStartIfHitNative_InvalidFingerId) {
  WebTouchEvent touch_event = CreateTouchEvent(
    WebInputEvent::Type::kTouchStart, 100, WebTouchPoint::State::kStatePressed);
  EXPECT_FALSE(utils_->HandleTouchStartIfHitNative(touch_event));
  touch_event = CreateTouchEvent(WebInputEvent::Type::kTouchStart, -1, WebTouchPoint::State::kStatePressed);
  EXPECT_FALSE(utils_->HandleTouchStartIfHitNative(touch_event));
}

TEST_F(InputHandlerProxyUtilsTest, HandleTouchStartIfHitNative_HitNativeLayer) {
  WebTouchEvent touch_event = CreateTouchEvent(
    WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed);

  // Setup native layer hit
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetNativeLayerImpl(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));

  EXPECT_TRUE(utils_->HandleTouchStartIfHitNative(touch_event));

  auto queue = utils_->NativeTouchEventQueues_[0];
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::PEND_HITTEST);
}

TEST_F(InputHandlerProxyUtilsTest, HandleTouchStartIfHitNative_NoHit) {
  WebTouchEvent touch_event = CreateTouchEvent(
    WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed);

  // No layer hit
  EXPECT_CALL(*mock_input_handler_utils_, GetNativeLayerImpl(_))
      .WillRepeatedly(Return(nullptr));

  EXPECT_FALSE(utils_->HandleTouchStartIfHitNative(touch_event));

  auto queue = utils_->NativeTouchEventQueues_[0];
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::SEND_BLINK);
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchStartProcess_InvalidChangeIndex) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 100,
                                        WebTouchPoint::State::kStateReleased);

  utils_->NativeTouchStartProcess(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchStartProcess_InvalidFingerId) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 100,
                                        WebTouchPoint::State::kStatePressed);

  utils_->NativeTouchStartProcess(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchStartProcess_InitStatus) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Setup queue with INIT status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::INIT);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchStartProcess(std::move(event));
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::SEND_BLINK);
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchStartProcess_PendStatus) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Setup queue with PEND status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::PEND_NATIVE);
  utils_->NativeTouchEventQueues_[0] = queue;

  // Should queue the event
  utils_->NativeTouchStartProcess(std::move(event));
  EXPECT_FALSE(queue->empty());
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchStartProcess_PendHittest) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed);
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::PEND_HITTEST);
  utils_->NativeTouchEventQueues_[0] = queue;
  utils_->NativeTouchStartProcess(std::move(event));
  EXPECT_FALSE(queue->empty());
}

TEST_F(InputHandlerProxyUtilsTest,
       NativeTouchStartProcess_ErrorStatus_SendBlink) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Setup queue with error status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_BLINK);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchStartProcess(std::move(event));
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::INIT);
}

TEST_F(InputHandlerProxyUtilsTest,
       NativeTouchStartProcess_ErrorStatus_SendNativeBlink) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Setup queue with error status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_NATIVE_BLINK);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchStartProcess(std::move(event));
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::INIT);
}

TEST_F(InputHandlerProxyUtilsTest,
       NativeTouchStartProcess_ErrorStatus_SendToNative) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Setup queue with error status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_TO_NATIVE);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchStartProcess(std::move(event));
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::INIT);
}

TEST_F(InputHandlerProxyUtilsTest,
       NativeTouchStartProcess_ErrorStatus_SendNativeBlinkConsumer) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Setup queue with error status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_NATIVE_BLINK_CONSUMER);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchStartProcess(std::move(event));
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::INIT);
}

TEST_F(InputHandlerProxyUtilsTest,
       NativeTouchStartProcess_ErrorStatus_Default) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Setup queue with error status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(static_cast<InputHandlerProxyUtils::NativeEventState>(100));
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchStartProcess(std::move(event));
  EXPECT_NE(queue->GetStatus(), InputHandlerProxyUtils::INIT);
}

TEST_F(InputHandlerProxyUtilsTest,
       NativeTouchStartProcessInQueue_InvalidChangeIndex) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStateReleased);

  utils_->NativeTouchStartProcessInQueue(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest,
       NativeTouchStartProcessInQueue_InvalidFingerId) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 100,
                                        WebTouchPoint::State::kStatePressed);

  utils_->NativeTouchStartProcessInQueue(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchStartProcessInQueue_InitStatus) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Setup queue with INIT status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::INIT);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchStartProcessInQueue(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest,
       NativeTouchStartProcessInQueue_PendStatusError_PendNative) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Setup queue with PEND status (should be error)
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::PEND_NATIVE);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchStartProcessInQueue(std::move(event));
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::INIT);
}

TEST_F(InputHandlerProxyUtilsTest,
       NativeTouchStartProcessInQueue_PendStatusError_PendHittest) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Setup queue with PEND status (should be error)
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::PEND_HITTEST);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchStartProcessInQueue(std::move(event));
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::INIT);
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchStartProcessInQueue_SendBlink) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Setup queue with SEND_BLINK status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_BLINK);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchStartProcessInQueue(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest,
       NativeTouchStartProcessInQueue_SendNativeBlink) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Setup queue with SEND_BLINK status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_NATIVE_BLINK);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchStartProcessInQueue(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest,
       NativeTouchStartProcessInQueue_SendToNative) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Setup queue with SEND_BLINK status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_TO_NATIVE);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchStartProcessInQueue(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest,
       NativeTouchStartProcessInQueue_SendNativeBlinkConsumer) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed);

  // Setup queue with SEND_BLINK status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_NATIVE_BLINK_CONSUMER);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchStartProcessInQueue(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchStartProcessInQueue_Default) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed);
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(static_cast<InputHandlerProxyUtils::NativeEventState>(-1));
  utils_->NativeTouchEventQueues_[0] = queue;
  utils_->NativeTouchStartProcessInQueue(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchMoveProcess_InvalidChangeIndex) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStateReleased);

  utils_->NativeTouchMoveProcess(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchMoveProcess_InvalidFingerId) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 100,
                                        WebTouchPoint::State::kStatePressed);

  utils_->NativeTouchMoveProcess(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchMoveProcess_InitStatus) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateMoved);

  // Setup queue with INIT status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::INIT);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchMoveProcess(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchMoveProcess_SendBlink) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateMoved);

  // Setup queue with INIT status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_BLINK);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchMoveProcess(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchMoveProcess_PendHitTest) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateMoved);

  // Setup queue with PEND_HITTEST status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::PEND_HITTEST);
  utils_->NativeTouchEventQueues_[0] = queue;

  // Should queue the event
  utils_->NativeTouchMoveProcess(std::move(event));
  EXPECT_FALSE(queue->empty());
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchMoveProcess_SendNativeBlink) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateMoved);

  // Setup queue with SEND_NATIVE_BLINK status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_NATIVE_BLINK);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchMoveProcess(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchMoveProcess_SendToNative) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateMoved);

  // Setup queue with SEND_NATIVE_BLINK status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_TO_NATIVE);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchMoveProcess(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest,
       NativeTouchMoveProcess_SendNativeBlinkConsumer) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateMoved);

  // Setup queue with SEND_NATIVE_BLINK status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_NATIVE_BLINK_CONSUMER);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchMoveProcess(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchMoveProcess_PendNative) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateMoved);

  // Setup queue with SEND_NATIVE_BLINK status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::PEND_NATIVE);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchMoveProcess(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchMoveProcess_Default) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateMoved);

  // Setup queue with SEND_NATIVE_BLINK status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(static_cast<InputHandlerProxyUtils::NativeEventState>(100));
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchMoveProcess(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchEndProcess_InvalidChangeIndex) {
  auto event = CreateTouchEventCallback(
      WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStateReleased);

  utils_->NativeTouchEndProcess(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchEndProcess_InvalidFingerId) {
  auto event = CreateTouchEventCallback(
      WebInputEvent::Type::kTouchStart, 100, WebTouchPoint::State::kStatePressed);

  utils_->NativeTouchEndProcess(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchEndProcess_InitStatus) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchEnd, 0,
                                        WebTouchPoint::State::kStateReleased);

  // Setup queue with INIT status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::INIT);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchEndProcess(std::move(event));
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::INIT);
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchEndProcess_SendBlink) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchEnd, 0,
                                        WebTouchPoint::State::kStateReleased);

  // Setup queue with INIT status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_BLINK);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchEndProcess(std::move(event));
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::INIT);
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchEndProcess_PendHitTest) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchEnd, 0,
                                        WebTouchPoint::State::kStateReleased);

  // Setup queue with PEND_HITTEST status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::PEND_HITTEST);
  utils_->NativeTouchEventQueues_[0] = queue;

  // Should queue the event
  utils_->NativeTouchEndProcess(std::move(event));
  EXPECT_FALSE(queue->empty());
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchEndProcess_SendNativeBlink) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchEnd, 0,
                                        WebTouchPoint::State::kStateReleased);

  // Setup queue with SEND_NATIVE_BLINK status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_NATIVE_BLINK);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchEndProcess(std::move(event));
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::INIT);
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchEndProcess_SendToNative) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchEnd, 0,
                                        WebTouchPoint::State::kStateReleased);

  // Setup queue with SEND_NATIVE_BLINK status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_TO_NATIVE);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchEndProcess(std::move(event));
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::INIT);
}

TEST_F(InputHandlerProxyUtilsTest,
       NativeTouchEndProcess_SendNativeBlinkConsumer) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchEnd, 0,
                                        WebTouchPoint::State::kStateReleased);

  // Setup queue with SEND_NATIVE_BLINK status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::SEND_NATIVE_BLINK_CONSUMER);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchEndProcess(std::move(event));
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::INIT);
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchEndProcess_PendNative) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchEnd, 0,
                                        WebTouchPoint::State::kStateReleased);

  // Setup queue with SEND_NATIVE_BLINK status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::PEND_NATIVE);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchEndProcess(std::move(event));
  EXPECT_NE(queue->GetStatus(), InputHandlerProxyUtils::PEND_NATIVE);
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchEndProcess_Others) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchEnd, 0,
                                        WebTouchPoint::State::kStateReleased);

  // Setup queue with SEND_NATIVE_BLINK status
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(static_cast<InputHandlerProxyUtils::NativeEventState>(100));
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchEndProcess(std::move(event));
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchCancelProcess_Normal) {
  InputHandlerProxyUtils::NativeEventState states[6] = {
      InputHandlerProxyUtils::INIT,
      InputHandlerProxyUtils::PEND_HITTEST,
      InputHandlerProxyUtils::SEND_NATIVE_BLINK,
      InputHandlerProxyUtils::SEND_TO_NATIVE,
      InputHandlerProxyUtils::SEND_NATIVE_BLINK_CONSUMER,
      InputHandlerProxyUtils::PEND_NATIVE};

  for (auto state : states) {
    auto event =
        CreateTouchEventCallback(WebInputEvent::Type::kTouchCancel, 0,
                                 WebTouchPoint::State::kStateCancelled);

    // Setup queues for all fingers
    auto queue = std::make_shared<NativeEmbedEventQueue>();
    queue->Queue(
        CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                  WebTouchPoint::State::kStatePressed));
    queue->SetStatus(state);
    utils_->NativeTouchEventQueues_[0] = queue;

    utils_->NativeTouchCancelProcess(std::move(event));

    // Verify all queues are reset
    EXPECT_EQ(utils_->NativeTouchEventQueues_[0]->GetStatus(),
              InputHandlerProxyUtils::INIT);
    EXPECT_TRUE(utils_->NativeTouchEventQueues_[0]->empty());
  }
}

TEST_F(InputHandlerProxyUtilsTest, NativeTouchCancelProcess_Default) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchCancel, 0,
                                        WebTouchPoint::State::kStateCancelled);

  // Setup queues for all fingers
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed));
  queue->SetStatus(InputHandlerProxyUtils::SEND_BLINK);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->NativeTouchCancelProcess(std::move(event));

  // Verify all queues are reset
  EXPECT_NE(utils_->NativeTouchEventQueues_[0]->GetStatus(),
            InputHandlerProxyUtils::INIT);
  EXPECT_FALSE(utils_->NativeTouchEventQueues_[0]->empty());
}

// Test Cases for SendEventToNativeByIndex
TEST_F(InputHandlerProxyUtilsTest, SendEventToNativeByIndex_InvalidIndex) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 1;
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed));
  queue->SetStatus(InputHandlerProxyUtils::SEND_BLINK);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->SendEventToNativeByIndex(touch_event, -1);
  EXPECT_EQ(utils_->NativeTouchEventQueues_[0]->GetStatus(),
            InputHandlerProxyUtils::INIT);
  EXPECT_TRUE(utils_->NativeTouchEventQueues_[0]->empty());
}

TEST_F(InputHandlerProxyUtilsTest,
       SendEventToNativeByIndex_ValidIndexButInvalidFingerId) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 2;
  touch_event.touches[0].id = 0;
  touch_event.touches[1].id = MAX_FINGER_NUMBER;  // Invalid
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 1,
                                        WebTouchPoint::State::kStatePressed));
  queue->SetStatus(InputHandlerProxyUtils::SEND_BLINK);
  utils_->NativeTouchEventQueues_[1] = queue;

  utils_->SendEventToNativeByIndex(touch_event, 1);
  EXPECT_EQ(utils_->NativeTouchEventQueues_[1]->GetStatus(),
            InputHandlerProxyUtils::INIT);
  EXPECT_TRUE(utils_->NativeTouchEventQueues_[1]->empty());
}

TEST_F(InputHandlerProxyUtilsTest,
       SendEventToNativeByIndex_ValidIndexNoLayerImpl) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 1;
  touch_event.touches[0].id = 0;
  touch_event.touches[0].state = WebTouchPoint::State::kStatePressed;
  touch_event.SetType(WebInputEvent::Type::kTouchStart);

  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplById(0))
      .WillRepeatedly(Return(nullptr));

  utils_->SendEventToNativeByIndex(touch_event, 0);
}

TEST_F(InputHandlerProxyUtilsTest,
       SendEventToNativeByIndex_TouchStartWithLayerImpl) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 1;
  touch_event.touches[0].id = 0;
  touch_event.touches[0].state = WebTouchPoint::State::kStatePressed;
  touch_event.touches[0].SetPositionInWidget(gfx::PointF(100.0f, 200.0f));
  touch_event.SetType(WebInputEvent::Type::kTouchStart);

  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplById(0))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  mock_layer_utils_->SetInitScale(1.0f);
  mock_layer_utils_->SetNativeRect(gfx::RectF(50, 50, 100, 100));
  mock_layer_utils_->set_native_embed_id(12345);
  EXPECT_CALL(*mock_client_,
              DidNativeEmbedEvent(WebInputEvent::Type::kTouchStart, "12345", 0,
                                  50.0f, 150.0f));

  utils_->SendEventToNativeByIndex(touch_event, 0);
}

TEST_F(InputHandlerProxyUtilsTest,
       SendEventToNativeByIndex_TouchMoveWithLayerImpl) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 1;
  touch_event.touches[0].id = 0;
  touch_event.touches[0].state = WebTouchPoint::State::kStateMoved;
  touch_event.touches[0].SetPositionInWidget(gfx::PointF(150.0f, 250.0f));
  touch_event.SetType(WebInputEvent::Type::kTouchMove);

  // Pre-set native rect for finger 0
  SetNativeRectByFingerId(utils_.get(), gfx::RectF(50, 50, 100, 100), 0);

  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplById(0))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  mock_layer_utils_->set_native_embed_id(12345);
  EXPECT_CALL(*mock_client_,
              DidNativeEmbedEvent(WebInputEvent::Type::kTouchMove, "12345", 0,
                                  100.0f, 200.0f));

  utils_->SendEventToNativeByIndex(touch_event, 0);
}

// Test Cases for SetGestureEventResult
TEST_F(InputHandlerProxyUtilsTest, SetGestureEventResult_InvalidFingerId) {
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 1,
                                        WebTouchPoint::State::kStatePressed));
  queue->SetStatus(InputHandlerProxyUtils::SEND_BLINK);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->SetGestureEventResult(true, true, -1);
  EXPECT_EQ(utils_->NativeTouchEventQueues_[0]->GetStatus(),
            InputHandlerProxyUtils::INIT);
  EXPECT_TRUE(utils_->NativeTouchEventQueues_[0]->empty());
}

TEST_F(InputHandlerProxyUtilsTest,
       SetGestureEventResult_ResultTrueStopPropagationTrue) {
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 1,
                                        WebTouchPoint::State::kStatePressed));
  queue->SetStatus(InputHandlerProxyUtils::PEND_NATIVE);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->SetGestureEventResult(true, true, 0);
  EXPECT_EQ(utils_->NativeTouchEventQueues_[0]->GetStatus(),
            InputHandlerProxyUtils::SEND_TO_NATIVE);
}

TEST_F(InputHandlerProxyUtilsTest,
       SetGestureEventResult_ResultTrueStopPropagationFalse) {
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 1,
                                        WebTouchPoint::State::kStatePressed));
  queue->SetStatus(InputHandlerProxyUtils::PEND_NATIVE);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->SetGestureEventResult(true, false, 0);
  EXPECT_EQ(utils_->NativeTouchEventQueues_[0]->GetStatus(),
            InputHandlerProxyUtils::SEND_NATIVE_BLINK_CONSUMER);
}

TEST_F(InputHandlerProxyUtilsTest, SetGestureEventResult_ResultFalse) {
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 1,
                                        WebTouchPoint::State::kStatePressed));
  queue->SetStatus(InputHandlerProxyUtils::PEND_NATIVE);
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->SetGestureEventResult(false, true, 0);
  EXPECT_EQ(utils_->NativeTouchEventQueues_[0]->GetStatus(),
            InputHandlerProxyUtils::SEND_NATIVE_BLINK);
}

TEST_F(InputHandlerProxyUtilsTest, SetGestureEventResult_False) {
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 1, WebTouchPoint::State::kStatePressed));
  queue->SetStatus(InputHandlerProxyUtils::PEND_NATIVE);
  utils_->NativeTouchEventQueues_[0] = queue;
  utils_->SetGestureEventResult(false, false, 0);
  EXPECT_EQ(utils_->NativeTouchEventQueues_[0]->GetStatus(), InputHandlerProxyUtils::SEND_NATIVE_BLINK);
}

// Test Cases for GetTouchChangeIndex
TEST_F(InputHandlerProxyUtilsTest,
       GetTouchChangeIndex_TouchStartWithPressedState) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 1;
  touch_event.touches[0].state = WebTouchPoint::State::kStatePressed;
  touch_event.SetType(WebInputEvent::Type::kTouchStart);

  int32_t index = utils_->GetTouchChangeIndex(touch_event);
  EXPECT_EQ(index, 0);
}

TEST_F(InputHandlerProxyUtilsTest,
       GetTouchChangeIndex_TouchMoveWithMovedState) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 1;
  touch_event.touches[0].state = WebTouchPoint::State::kStateMoved;
  touch_event.SetType(WebInputEvent::Type::kTouchMove);

  int32_t index = utils_->GetTouchChangeIndex(touch_event);
  EXPECT_EQ(index, 0);
}

TEST_F(InputHandlerProxyUtilsTest,
       GetTouchChangeIndex_TouchEndWithReleasedState) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 1;
  touch_event.touches[0].state = WebTouchPoint::State::kStateReleased;
  touch_event.SetType(WebInputEvent::Type::kTouchEnd);

  int32_t index = utils_->GetTouchChangeIndex(touch_event);
  EXPECT_EQ(index, 0);
}

TEST_F(InputHandlerProxyUtilsTest,
       GetTouchChangeIndex_TouchCancelWithCancelledState) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 1;
  touch_event.touches[0].state = WebTouchPoint::State::kStateCancelled;
  touch_event.SetType(WebInputEvent::Type::kTouchCancel);

  int32_t index = utils_->GetTouchChangeIndex(touch_event);
  EXPECT_EQ(index, 0);
}

TEST_F(InputHandlerProxyUtilsTest, GetTouchChangeIndex_NoMatchingState) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 1;
  touch_event.touches[0].state = WebTouchPoint::State::kStateStationary;
  touch_event.SetType(WebInputEvent::Type::kTouchStart);

  int32_t index = utils_->GetTouchChangeIndex(touch_event);
  EXPECT_EQ(index, -1);
}

TEST_F(InputHandlerProxyUtilsTest,
       GetTouchChangeIndex_MultipleTouchesFindCorrectOne) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 3;
  touch_event.touches[0].state = WebTouchPoint::State::kStateStationary;
  touch_event.touches[1].state = WebTouchPoint::State::kStatePressed;
  touch_event.touches[2].state = WebTouchPoint::State::kStateStationary;
  touch_event.SetType(WebInputEvent::Type::kTouchStart);

  int32_t index = utils_->GetTouchChangeIndex(touch_event);
  EXPECT_EQ(index, 1);
}

TEST_F(InputHandlerProxyUtilsTest, GetTouchChangeIndex_EmptyTouches) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 0;
  touch_event.SetType(WebInputEvent::Type::kTouchStart);

  int32_t index = utils_->GetTouchChangeIndex(touch_event);
  EXPECT_EQ(index, -1);
}

TEST_F(InputHandlerProxyUtilsTest, GetTouchChangeIndex_NotSameEventType) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 1;
  touch_event.touches[0].state = WebTouchPoint::State::kStatePressed;
  touch_event.SetType(WebInputEvent::Type::kTouchTypeLast);
  int32_t index = utils_->GetTouchChangeIndex(touch_event);
  EXPECT_EQ(index, -1);
}

// Test Cases for FlushNativeTouchQueue
TEST_F(InputHandlerProxyUtilsTest, FlushNativeTouchQueue_EmptyQueue) {
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->SetStatus(InputHandlerProxyUtils::INIT);
  utils_->NativeTouchEventQueues_[0] = queue;
  utils_->FlushNativeTouchQueue(0);
}

TEST_F(InputHandlerProxyUtilsTest, FlushNativeTouchQueue_NonEmptyQueueNoStop) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateMoved);

  utils_->FlushNativeTouchQueue(0);
}

TEST_F(InputHandlerProxyUtilsTest, FlushNativeTouchQueue_NeedStopPop) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed);
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed));
  utils_->NativeTouchEventQueues_[0] = queue;
  utils_->FlushNativeTouchQueue(0);
  EXPECT_FALSE(queue->empty());
}

TEST_F(InputHandlerProxyUtilsTest, PopNativeTouchQueue_OneStart) {
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStateReleased));
  utils_->NativeTouchEventQueues_[0] = queue;
  utils_->PopNativeTouchQueue(0);
  EXPECT_TRUE(queue->empty());
}

TEST_F(InputHandlerProxyUtilsTest, PopNativeTouchQueue_QueueEmpty) {
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  utils_->NativeTouchEventQueues_[0] = queue;
  utils_->PopNativeTouchQueue(0);
  EXPECT_TRUE(queue->empty());
}

TEST_F(InputHandlerProxyUtilsTest, PopNativeTouchQueue_PopSecMove) {
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateMoved));
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateMoved));

  utils_->NativeTouchEventQueues_[0] = queue;
  utils_->PopNativeTouchQueue(0);
  EXPECT_FALSE(queue->empty());
}

TEST_F(InputHandlerProxyUtilsTest, PopNativeTouchQueue_PopSecStart) {
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplIsHitByPoint(_))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  mock_layer_utils_->SetShouldInterceptTouchEvent(true);
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateMoved));
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStateReleased));

  utils_->NativeTouchEventQueues_[0] = queue;
  utils_->PopNativeTouchQueue(0);
  EXPECT_TRUE(queue->empty());
}

// Test Cases for SendToBlink
TEST_F(InputHandlerProxyUtilsTest, SendToBlink_InvalidChangeIndex) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateStationary);

  utils_->SendToBlink(std::move(event), false, false);
}

TEST_F(InputHandlerProxyUtilsTest,
       SendToBlink_ValidChangeIndexInValidFingerIdLower) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, -1,
                                        WebTouchPoint::State::kStateMoved);

  utils_->SendToBlink(std::move(event), false, false);
}

TEST_F(InputHandlerProxyUtilsTest,
       SendToBlink_ValidChangeIndexInValidFingerIdUpper) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 20,
                                        WebTouchPoint::State::kStateMoved);

  utils_->SendToBlink(std::move(event), false, false);
}

TEST_F(InputHandlerProxyUtilsTest,
       SendToBlink_ValidChangeIndexInvalidFingerId) {
  auto event = CreateTouchEventCallback(WebInputEvent::Type::kTouchMove, 0,
                                        WebTouchPoint::State::kStateMoved);

  utils_->SendToBlink(std::move(event), false, false);
}

// Test Cases for CheckTouchEventSequence
TEST_F(InputHandlerProxyUtilsTest,
       CheckTouchEventSequence_InValidStateAfterInValid) {
  SetGestureStatusByFingerId(utils_.get(), WebInputEvent::Type::kTouchTypeLast, 0);
  utils_->CheckTouchEventSequence(WebInputEvent::Type::kTouchTypeLast, 0);
  EXPECT_EQ(GetGestureStatusByFingerId(utils_.get(), 0), WebInputEvent::Type::kTouchTypeLast);
}

TEST_F(InputHandlerProxyUtilsTest, CheckTouchEventSequence_ValidStartAfterEnd) {
  SetGestureStatusByFingerId(utils_.get(), WebInputEvent::Type::kTouchEnd, 0);
  utils_->CheckTouchEventSequence(WebInputEvent::Type::kTouchStart, 0);
  EXPECT_EQ(GetGestureStatusByFingerId(utils_.get(), 0), WebInputEvent::Type::kTouchStart);
}

TEST_F(InputHandlerProxyUtilsTest,
       CheckTouchEventSequence_InvalidStartAfterStart) {
  SetGestureStatusByFingerId(utils_.get(), WebInputEvent::Type::kTouchStart, 0);
  utils_->CheckTouchEventSequence(WebInputEvent::Type::kTouchStart, 0);
  // Should log error but still update status
  EXPECT_EQ(GetGestureStatusByFingerId(utils_.get(), 0), WebInputEvent::Type::kTouchStart);
}

TEST_F(InputHandlerProxyUtilsTest,
       CheckTouchEventSequence_ValidMoveAfterStart) {
  SetGestureStatusByFingerId(utils_.get(), WebInputEvent::Type::kTouchStart, 0);
  utils_->CheckTouchEventSequence(WebInputEvent::Type::kTouchMove, 0);
  EXPECT_EQ(GetGestureStatusByFingerId(utils_.get(), 0), WebInputEvent::Type::kTouchStart);
}

TEST_F(InputHandlerProxyUtilsTest,
       CheckTouchEventSequence_InvalidMoveAfterEnd) {
  SetGestureStatusByFingerId(utils_.get(), WebInputEvent::Type::kTouchEnd, 0);
  utils_->CheckTouchEventSequence(WebInputEvent::Type::kTouchMove, 0);
  EXPECT_EQ(GetGestureStatusByFingerId(utils_.get(), 0), WebInputEvent::Type::kTouchEnd);
}

TEST_F(InputHandlerProxyUtilsTest, CheckTouchEventSequence_ValidEndAfterStart) {
  SetGestureStatusByFingerId(utils_.get(), WebInputEvent::Type::kTouchStart, 0);
  utils_->CheckTouchEventSequence(WebInputEvent::Type::kTouchEnd, 0);
  EXPECT_EQ(GetGestureStatusByFingerId(utils_.get(), 0), WebInputEvent::Type::kTouchEnd);
}

TEST_F(InputHandlerProxyUtilsTest, CheckTouchEventSequence_InvalidEndAfterEnd) {
  SetGestureStatusByFingerId(utils_.get(), WebInputEvent::Type::kTouchEnd, 0);
  utils_->CheckTouchEventSequence(WebInputEvent::Type::kTouchEnd, 0);
  EXPECT_EQ(GetGestureStatusByFingerId(utils_.get(), 0), WebInputEvent::Type::kTouchEnd);
}

TEST_F(InputHandlerProxyUtilsTest,
       CheckTouchEventSequence_ValidCancelAfterStart) {
  SetGestureStatusByFingerId(utils_.get(), WebInputEvent::Type::kTouchStart, 0);
  utils_->CheckTouchEventSequence(WebInputEvent::Type::kTouchCancel, 0);
  EXPECT_EQ(GetGestureStatusByFingerId(utils_.get(), 0), WebInputEvent::Type::kTouchEnd);
}

TEST_F(InputHandlerProxyUtilsTest,
       CheckTouchEventSequence_InvalidCancelAfterEnd) {
  SetGestureStatusByFingerId(utils_.get(), WebInputEvent::Type::kTouchEnd, 0);
  utils_->CheckTouchEventSequence(WebInputEvent::Type::kTouchCancel, 0);
  EXPECT_EQ(GetGestureStatusByFingerId(utils_.get(), 0), WebInputEvent::Type::kTouchEnd);
}

// Test Cases for SendEventToNative
TEST_F(InputHandlerProxyUtilsTest, SendEventToNative_InvalidChangeIndex) {
  WebTouchEvent touch_event;
  touch_event.touches_length = 0;

  utils_->SendEventToNative(touch_event);
}

TEST_F(InputHandlerProxyUtilsTest,
       SendEventToNative_ValidChangeIndexInvalidFingerId) {
  auto touch_event = CreateTouchEvent(WebInputEvent::Type::kTouchStart, 0,
                                      WebTouchPoint::State::kStatePressed);
  touch_event.touches[0].id = MAX_FINGER_NUMBER;

  utils_->SendEventToNative(touch_event);
}

TEST_F(InputHandlerProxyUtilsTest,
       SendEventToNative_ValidChangeIndexNoLayerImpl) {
  auto touch_event = CreateTouchEvent(WebInputEvent::Type::kTouchStart, 0,
                                      WebTouchPoint::State::kStatePressed);

  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplById(0))
      .WillRepeatedly(Return(nullptr));

  utils_->SendEventToNative(touch_event);
}

TEST_F(InputHandlerProxyUtilsTest,
       SendEventToNative_ValidChangeIndexHitLayerImpl) {
  auto touch_event = CreateTouchEvent(WebInputEvent::Type::kTouchStart, 0,
                                      WebTouchPoint::State::kStatePressed);

  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplById(0))
      .WillRepeatedly(Return(nullptr));

  utils_->SendEventToNative(touch_event);
}

TEST_F(InputHandlerProxyUtilsTest, SendEventToNative_TouchStartWithLayerImpl) {
  WebTouchEvent touch_event = CreateTouchEvent(
      WebInputEvent::Type::kTouchStart, 0, WebTouchPoint::State::kStatePressed);
  touch_event.touches_length = 1;
  touch_event.touches[0].id = 0;
  touch_event.touches[0].state = WebTouchPoint::State::kStatePressed;
  touch_event.touches[0].SetPositionInWidget(gfx::PointF(100.0f, 200.0f));
  touch_event.SetType(WebInputEvent::Type::kTouchStart);

  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplById(0))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  mock_layer_utils_->SetInitScale(1.0f);
  mock_layer_utils_->SetNativeRect(gfx::RectF(50, 50, 100, 100));
  mock_layer_utils_->set_native_embed_id(12345);
  EXPECT_CALL(*mock_client_,
              DidNativeEmbedEvent(WebInputEvent::Type::kTouchStart, "12345", 0,
                                  50.0f, 150.0f));

  utils_->SendEventToNative(touch_event);
}

TEST_F(InputHandlerProxyUtilsTest, SendEventToNative_TouchMoveWithLayerImpl) {
  WebTouchEvent touch_event = CreateTouchEvent(
      WebInputEvent::Type::kTouchMove, 0, WebTouchPoint::State::kStateMoved);
  touch_event.touches_length = 1;
  touch_event.touches[0].id = 0;
  touch_event.touches[0].state = WebTouchPoint::State::kStateMoved;
  touch_event.touches[0].SetPositionInWidget(gfx::PointF(150.0f, 250.0f));
  touch_event.SetType(WebInputEvent::Type::kTouchMove);

  // Pre-set native rect for finger 0
  SetNativeRectByFingerId(utils_.get(), gfx::RectF(50, 50, 100, 100), 0);

  EXPECT_CALL(*mock_input_handler_utils_, GetLayerImplById(0))
      .WillRepeatedly(Return(mock_layer_impl_.get()));
  EXPECT_CALL(*mock_layer_impl_, layer_impl_utils())
      .WillRepeatedly(Return(mock_layer_utils_.get()));
  EXPECT_CALL(*mock_input_handler_, handler_utils())
      .WillRepeatedly(Return(mock_input_handler_utils_.get()));
  mock_layer_utils_->set_native_embed_id(12345);
  EXPECT_CALL(*mock_client_,
              DidNativeEmbedEvent(WebInputEvent::Type::kTouchMove, "12345", 0,
                                  100.0f, 200.0f));

  utils_->SendEventToNative(touch_event);
}

// Test Cases for NotifyEventNativeFocusResult
TEST_F(InputHandlerProxyUtilsTest, NotifyEventNativeFocusResult) {
  utils_->NotifyEventNativeFocusResult(0);
}

// Test Cases for ResetTouchSequence
TEST_F(InputHandlerProxyUtilsTest, ResetTouchSequence) {
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->ResetTouchSequence();
  EXPECT_EQ(queue->GetStatus(), InputHandlerProxyUtils::INIT);
}

TEST_F(InputHandlerProxyUtilsTest, ResetTouchSequence_SomeEmptyQueues) {
  auto queue = std::make_shared<NativeEmbedEventQueue>();
  queue->Queue(CreateTouchEventCallback(WebInputEvent::Type::kTouchStart, 0,
                                        WebTouchPoint::State::kStatePressed));
  utils_->NativeTouchEventQueues_[0] = queue;

  utils_->ResetTouchSequence();
  EXPECT_TRUE(queue->empty());
}

// Test Cases for CheckFingerIdOutOfIndex
TEST_F(InputHandlerProxyUtilsTest, CheckFingerIdOutOfIndex_Valid) {
  bool result = utils_->CheckFingerIdOutOfIndex(0);
  EXPECT_FALSE(result);
}

TEST_F(InputHandlerProxyUtilsTest, CheckFingerIdOutOfIndex_TooSmall) {
  bool result = utils_->CheckFingerIdOutOfIndex(-1);
  EXPECT_TRUE(result);
}

TEST_F(InputHandlerProxyUtilsTest, CheckFingerIdOutOfIndex_TooLarge) {
  bool result = utils_->CheckFingerIdOutOfIndex(20);
  EXPECT_TRUE(result);
}
#pragma clang diagnostic pop
}  // namespace blink