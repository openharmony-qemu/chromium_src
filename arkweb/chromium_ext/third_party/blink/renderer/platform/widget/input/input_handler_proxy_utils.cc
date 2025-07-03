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

#include "third_party/blink/renderer/platform/widget/input/input_handler_proxy_utils.h"

#include "cc/layers/layer_impl.h"
#include "third_party/blink/public/common/features.h"
#include "third_party/blink/public/common/input/web_input_event_attribution.h"
#include "third_party/blink/renderer/platform/widget/input/elastic_overscroll_controller.h"
#include "third_party/blink/renderer/platform/widget/input/input_handler_proxy_client.h"
#include "third_party/blink/renderer/platform/widget/input/native_embed_event_queue.h"
#include "third_party/blink/renderer/platform/widget/input/scroll_predictor.h"
#include "cc/input/input_handler_utils.h"

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/widget/input/elastic_overscroll_controller_utils.h"
#endif

namespace blink {

#define NO_NATIVE_TYPE 100

InputHandlerProxyUtils::InputHandlerProxyUtils(InputHandlerProxy* proxy)
    : proxy_(proxy) {
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  native_event_queue_ = std::make_unique<NativeEmbedEventQueue>();
  native_touch_end_queue_ = std::make_unique<NativeEmbedEventQueue>();
  native_mouse_event_queue_ = std::make_unique<NativeEmbedEventQueue>();
  native_mouse_end_queue_ = std::make_unique<NativeEmbedEventQueue>();
#endif
}

InputHandlerProxyUtils::~InputHandlerProxyUtils() {}

#if BUILDFLAG(ARKWEB_SAME_LAYER)
bool IsTouchEventType(WebInputEvent::Type type) {
  switch (type) {
    case WebInputEvent::Type::kTouchStart:
    case WebInputEvent::Type::kTouchMove:
    case WebInputEvent::Type::kTouchEnd:
    case WebInputEvent::Type::kTouchCancel:
      return true;
    default:
      return false;
  }
}

bool IsMouseEventType(WebInputEvent::Type type) {
  switch (type) {
    case WebInputEvent::Type::kMouseDown:
    case WebInputEvent::Type::kMouseUp:
    case WebInputEvent::Type::kMouseMove:
      return true;
    default:
      return false;
  }
}

bool IsSameEventType(WebInputEvent::Type type, WebTouchPoint::State state) {
  switch (type) {
    case WebInputEvent::Type::kTouchStart:
      return state == WebTouchPoint::State::kStatePressed;
    case WebInputEvent::Type::kTouchMove:
      return state == WebTouchPoint::State::kStateMoved;
    case WebInputEvent::Type::kTouchEnd:
      return state == WebTouchPoint::State::kStateReleased;
    case WebInputEvent::Type::kTouchCancel:
      return true;
    default:
      return false;
  }
}

void InputHandlerProxyUtils::NativeHitTestResult(bool native,
                                            size_t fingerId,
                                            int layerId) {
  LOG(DEBUG) << "[NativeEmbed] NativeHitTestResult fingerId is : " << fingerId
             << " and native is : " << native;
  TRACE_EVENT1("input", "InputHandlerProxyUtils::NativeHitTestResult", "native",
               native);
  native_map_[fingerId] = native;
  hit_testing_number_--;
  if (native) {
    native_id_map_[fingerId] = layerId;
    SendNativeEvent(start_touch_event_, WebInputEvent::Type::kTouchStart,
                    fingerId);
  } else if (!native_event_queue_->empty()) {
    SendNativeEvent(start_touch_event_, WebInputEvent::Type::kTouchStart,
                    fingerId, false);
    auto event_with_callback = native_event_queue_->Pop();
    proxy_->DispatchSingleInputEvent(std::move(event_with_callback));
  }
  while (hit_testing_number_ == 0 && !native_touch_end_queue_->empty()) {
    auto callback = native_touch_end_queue_->Pop();
    size_t i = end_index_queue_.front();
    end_index_queue_.pop_front();
    if (native) {
      const WebTouchEvent& touch_event =
          static_cast<const WebTouchEvent&>(callback->event());
      native_event_queue_->Queue(std::move(callback));
      SendNativeEvent(touch_event, WebInputEvent::Type::kTouchEnd, i);
    } else {
      proxy_->DispatchSingleInputEvent(std::move(callback));
    }
  }
}

void InputHandlerProxyUtils::NativeMouseHitTestResult(bool native, int layerId) {
  LOG(DEBUG)<<"[NativeEmbed] NativeMouseHitTestResult native is : " << native;
  TRACE_EVENT1("input", "InputHandlerProxy::NativeMouseHitTestResult", "native", native);
  isMouseNativeArea_ = native;
  mouse_hit_testing_number_--;
  if (native) {
    mouse_native_layer_id_ = layerId;
    SendMouseNativeEvent(start_mouse_event_, WebInputEvent::Type::kMouseDown);
  } else if (!native_mouse_event_queue_->empty()) {
    SendMouseNativeEvent(start_mouse_event_, WebInputEvent::Type::kMouseDown, false);
    auto event_with_callback = native_mouse_event_queue_->Pop();
    proxy_->DispatchSingleInputEvent(std::move(event_with_callback));
  }
  while (mouse_hit_testing_number_ == 0 && !native_mouse_end_queue_->empty()) {
    auto callback = native_mouse_end_queue_->Pop();
    if (native) {
      const WebMouseEvent& mouse_event = static_cast<const WebMouseEvent&>(callback->event());
      native_mouse_event_queue_->Queue(std::move(callback));
      SendMouseNativeEvent(mouse_event, WebInputEvent::Type::kMouseUp);
    } else {
      proxy_->DispatchSingleInputEvent(std::move(callback));
    }
  }
}

void InputHandlerProxyUtils::SendMouseNativeEvent(
    const WebMouseEvent& mouse_event,
    WebInputEvent::Type type,
    bool result) {
  TRACE_EVENT2("input", "InputHandlerProxy::SendNativeEvent", "type",
               WebInputEvent::GetName(type), "result", result);
  auto modifiers = static_cast<WebInputEvent::Modifiers>(mouse_event.GetModifiers());
  if (result) {
    float x = mouse_event.PositionInWidget().x();
    float y = mouse_event.PositionInWidget().y();
    int layer_id = mouse_native_layer_id_;

    cc::LayerImpl* layer_impl =
        proxy_->input_handler_->handler_utils()->GetLayerImplById(layer_id);
    if (layer_impl) {
      if (type == WebInputEvent::Type::kMouseDown) {
        nativeRect_ = layer_impl->layer_impl_utils()->GetNativeRect();
      }
      embed_id_ =
          std::to_string(layer_impl->layer_impl_utils()->native_embed_id());
      x = x - nativeRect_.x();
      y = y - nativeRect_.y();
      LOG(DEBUG) << "[NativeEmbed] SendMouseNativeEvent x = " << x
                 << ",y = " << y;

      proxy_->client_->DidNativeEmbedMouseEvent(type, modifiers, embed_id_, result, x, y);
    } else {
      if (!native_mouse_event_queue_->empty()) {
        auto event_with_callback = native_mouse_event_queue_->Pop();
        proxy_->DispatchSingleInputEvent(std::move(event_with_callback));
      }
      LOG(ERROR) << "[NativeEmbed] SendNativeEvent error layer_impl is null";
    }
  } else {
    proxy_->client_->DidNativeEmbedMouseEvent(type, modifiers, embed_id_, result, 0, 0);
  }
}

void InputHandlerProxyUtils::SendNativeEvent(const WebTouchEvent& touch_event,
                                        WebInputEvent::Type type,
                                        size_t i,
                                        bool result) {
  TRACE_EVENT2("input", "InputHandlerProxyUtils::SendNativeEvent", "type",
               WebInputEvent::GetName(type), "result", result);
  if (result) {
    float x = touch_event.touches[i].PositionInWidget().x();
    float y = touch_event.touches[i].PositionInWidget().y();
    int32_t id = touch_event.touches[i].id;
    int layer_id = native_id_map_[id];
    cc::LayerImpl* layer_impl = proxy_->input_handler_->handler_utils()->GetLayerImplById(layer_id);
    if (layer_impl) {
      if (type == WebInputEvent::Type::kTouchStart) {
        nativeRect_ = layer_impl->layer_impl_utils()->GetNativeRect();
      }
      embed_id_ = std::to_string(layer_impl->layer_impl_utils()->native_embed_id());
      x = x - nativeRect_.x();
      y = y - nativeRect_.y();
      LOG(DEBUG) << "[NativeEmbed] SendNativeEvent x = " << x << ",y = " << y;
      proxy_->client_->DidNativeEmbedEvent(type, embed_id_, id, x, y);
    } else {
      if (!native_event_queue_->empty()) {
        auto event_with_callback = native_event_queue_->Pop();
        proxy_->DispatchSingleInputEvent(std::move(event_with_callback));
      }
      LOG(ERROR) << "[NativeEmbed] SendNativeEvent error layer_impl is null";
    }
  } else {
    proxy_->client_->DidNativeEmbedEvent(type, embed_id_, NO_NATIVE_TYPE, 0, 0);
  }
}

InputHandlerProxyUtils::NativeEventDisposition
InputHandlerProxyUtils::DidNativeEmbedEvent(const WebInputEvent& event) {
  if (!IsTouchEventType(event.GetType())) {
    return NORMAL;
  }
  const WebTouchEvent& touch_event = static_cast<const WebTouchEvent&>(event);
  InputHandlerProxyUtils::NativeEventDisposition result = NORMAL;
  for (size_t i = 0; i < touch_event.touches_length; ++i) {
    WebTouchPoint::State state = touch_event.touches[i].state;
    float x = touch_event.touches[i].PositionInWidget().x();
    float y = touch_event.touches[i].PositionInWidget().y();
    int32_t id = touch_event.touches[i].id;

    if (!IsSameEventType(event.GetType(), state)) {
      continue;
    }
    if (event.GetType() == WebInputEvent::Type::kTouchStart) {
      cc::LayerImpl* video_layer_impl =
          proxy_->input_handler_->handler_utils()->GetLayerImplIsHitByPoint(gfx::Point(x, y));
      if (video_layer_impl &&
          video_layer_impl->layer_impl_utils()->ShouldInterceptTouchEvent()) {
        native_id_map_[id] = video_layer_impl->id();
        SendNativeEvent(touch_event, event.GetType(), i);
        native_map_[id] = true;
        result = SEND_VIDEO;
        continue;
      }
      if (!native_enabled_) {
        continue;
      }
      cc::LayerImpl* native_layer_impl =
          proxy_->input_handler_->handler_utils()->GetNativeLayerImpl(gfx::Point(x, y));
      if (native_layer_impl) {
        start_touch_event_ = touch_event;
        const WebTouchPoint& touch_point = touch_event.touches[i];
        WebPointerEvent pointer_event =
            WebPointerEvent(touch_event, touch_point);
        proxy_->client_->TouchHitTest(pointer_event, id);
        hit_testing_number_++;
        result = SEND_NATIVE;
      } else {
        SendNativeEvent(touch_event, event.GetType(), i, false);
        native_map_[id] = false;
        result = NORMAL;
      }
      continue;
    }

    bool isNativeArea = false;
    if (native_map_.find(id) != native_map_.end()) {
      isNativeArea = native_map_.find(id)->second;
    }
    if (isNativeArea && event.GetType() != WebInputEvent::Type::kTouchEnd) {
      SendNativeEvent(touch_event, event.GetType(), i);
      result = SEND_NATIVE;
    }
    if (event.GetType() != WebInputEvent::Type::kTouchEnd) {
      continue;
    }
    if (hit_testing_number_ != 0) {
      result = END_QUEUE;
      end_index_queue_.emplace_back(i);
      LOG(INFO)
          << "[NativeEmbed] DidNativeEmbedEvent touchStart in hitTesting.";
    }
    if (hit_testing_number_ == 0 && isNativeArea) {
      SendNativeEvent(touch_event, event.GetType(), i);
      result = SEND_NATIVE;
    }
    native_map_[id] = false;
  }
  return result;
}


InputHandlerProxyUtils::NativeEventDisposition
InputHandlerProxyUtils::DidMouseEmbedEvent(const WebInputEvent& event) {
  auto type = event.GetType();
  if (!IsMouseEventType(type)) {
    return NORMAL;
  }
  // for 5.0.x
  if (event.GetModifiers() != WebInputEvent::Modifiers::kLeftButtonDown
    && event.GetModifiers() != (WebInputEvent::Modifiers::kLeftButtonDown | WebInputEvent::Modifiers::kIsAutoRepeat)) {
    return NORMAL;
  }
  const WebMouseEvent& mouse_event = static_cast<const WebMouseEvent&>(event);
  InputHandlerProxyUtils::NativeEventDisposition result = NORMAL;
  float x = mouse_event.PositionInWidget().x();
  float y = mouse_event.PositionInWidget().y();

  if (type == WebInputEvent::Type::kMouseDown) {
    cc::LayerImpl* video_layer_impl =
        proxy_->input_handler_->handler_utils()->GetLayerImplIsHitByPoint(gfx::Point(x, y));
    if (video_layer_impl &&
        video_layer_impl->layer_impl_utils()->ShouldInterceptTouchEvent()) {
      mouse_native_layer_id_ = video_layer_impl->id();
      SendMouseNativeEvent(mouse_event, event.GetType());
      isMouseNativeArea_ = true;
      return SEND_VIDEO;
    }

    if (!native_enabled_) {
        return NORMAL;
    }

    cc::LayerImpl* native_layer_impl = proxy_->input_handler_->handler_utils()->GetNativeLayerImpl(gfx::Point(x, y));
    if (native_layer_impl) {
      start_mouse_event_ = mouse_event;
      proxy_->client_->MouseHitTest(mouse_event);
      mouse_hit_testing_number_++;
      result = SEND_NATIVE;
    } else {
      SendMouseNativeEvent(mouse_event, type, false);
      isMouseNativeArea_ = false;
      result = NORMAL;
    }
    return result;
  }
  // move
  if (isMouseNativeArea_ && type != WebInputEvent::Type::kMouseUp) {
    SendMouseNativeEvent(mouse_event, type);
    result = SEND_NATIVE;
  }
  if (type != WebInputEvent::Type::kMouseUp) {
    return result;
  }
  // up
  if (mouse_hit_testing_number_ != 0) {
    result = END_QUEUE;
    LOG(INFO) << "[NativeEmbed] DidNativeEmbedEvent mouseDown in hitTesting.";
  }
  if (mouse_hit_testing_number_ == 0 && isMouseNativeArea_) {
    SendMouseNativeEvent(mouse_event, type, isMouseNativeArea_);
    result = SEND_NATIVE;
  }
  isMouseNativeArea_ = false;
  return result;
}

void InputHandlerProxyUtils::DidNativeSendEvent(
    std::unique_ptr<EventWithCallback> event_with_callback) {
    NativeEventDisposition result = NORMAL;
    auto& input_event = event_with_callback->event();
    if (IsTouchEventType(input_event.GetType())) {
      result = DidNativeEmbedEvent(input_event);
    }
    if (IsMouseEventType(input_event.GetType())) {
      result = DidMouseEmbedEvent(input_event);
    }
    LOG(DEBUG) << "[NativeEmbed] DidNativeEmbedEvent return result is : "
               << result;
    if (result == SEND_NATIVE || result == SEND_VIDEO) {
      TRACE_EVENT1("input",
                   "InputHandlerProxy::HandleInputEventWithLatencyInfo::"
                   "DidNativeEmbedEvent",
                   "NativeEventDisposition", result);
      if (IsTouchEventType(input_event.GetType())) {
        native_event_queue_->Queue(std::move(event_with_callback));
      }
      if (IsMouseEventType(input_event.GetType())) {
        native_mouse_event_queue_->Queue(std::move(event_with_callback));
      }
    } else if (result == END_QUEUE) {
      TRACE_EVENT1("input",
                   "InputHandlerProxy::HandleInputEventWithLatencyInfo::"
                   "DidNativeEmbedEvent",
                   "NativeEventDisposition", result);
      if (IsTouchEventType(input_event.GetType())) {
        native_touch_end_queue_->Queue(std::move(event_with_callback));
      }
      if (IsMouseEventType(input_event.GetType())) {
        native_mouse_end_queue_->Queue(std::move(event_with_callback));
      }
    } else {
      proxy_->DispatchSingleInputEvent(std::move(event_with_callback));
    }
}

void InputHandlerProxyUtils::SetGestureEventResult(bool result,
                                              bool stopPropagation) {
  LOG(DEBUG) << "[NativeEmbed] SetGestureEventResult result is : " << result
             << " stopPropagation is " << stopPropagation;
  TRACE_EVENT1("input", "InputHandlerProxyUtils::SetGestureEventResult", "result",
               result);
  if (native_event_queue_->empty()) {
    LOG(DEBUG) << "[NativeEmbed] native_event_queue_ is empty";
    return;
  }
  if (result && stopPropagation) {
    proxy_->DispatchSingleInputEvent(native_event_queue_->Pop(), true);
  } else {
    proxy_->DispatchSingleInputEvent(native_event_queue_->Pop());
  }
}

void InputHandlerProxyUtils::SetMouseEventResult(bool result, bool stopPropagation) {
  LOG(DEBUG) << "[NativeEmbed] SetMouseEventResult result is : " << result
             << " stopPropagation is " << stopPropagation;
  TRACE_EVENT1("input", "InputHandlerProxy::SetMouseEventResult", "result", result);
  if (native_mouse_event_queue_->empty()) {
    LOG(DEBUG) << "[NativeEmbed] native_mouse_event_queue_ is empty";
    return;
  }
  if (result && stopPropagation) {
    proxy_->DispatchSingleInputEvent(native_mouse_event_queue_->Pop(), true);
  } else {
    proxy_->DispatchSingleInputEvent(native_mouse_event_queue_->Pop());
  }
}

void InputHandlerProxyUtils::SetNativeEmbedMode(bool flag) {
  native_enabled_ = flag;
  LOG(DEBUG) << "[NativeEmbed] SetNativeEmbedMode native_enabled_ is : "
             << native_enabled_;
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void InputHandlerProxyUtils::ScrollBy(float delta_x, float delta_y) {
  TRACE_EVENT_INSTANT2(
      "input", "ScrollBy", TRACE_EVENT_SCOPE_THREAD, "delta_x",
      delta_x, "delta_y", delta_y);
  gfx::PointF root_offset;
  root_offset.set_x(delta_x);
  root_offset.set_y(delta_y);
  proxy_->SynchronouslySetRootScrollOffset(root_offset);
}

void InputHandlerProxyUtils::SetOverscrollMode(int mode) {
  if (!proxy_->elastic_overscroll_controller_) {
    LOG(ERROR) << "Error:Overscroll controller is not initialized";
    return;
  }
  proxy_->elastic_overscroll_controller_->GetUtils()->SetOverscrollMode(mode);
}

void InputHandlerProxyUtils::NeedFlushScrollUpdateGesture(
  const WebGestureEvent& gesture_event) {
  if (need_flush_scroll_update_gesture_ &&
    gesture_event.GetType() ==
        WebGestureEvent::Type::kGestureScrollUpdate) {
    proxy_->DeliverInputForBeginFrame(current_internal_begin_frame_args_);
  }
}

void InputHandlerProxyUtils::ResetNeedFlushScrollUpdateGesture() {
  if (need_flush_scroll_update_gesture_) {
    LOG(INFO) << "InputHandlerProxy::HandleGestureScrollUpdate "
                 "internalbeginframe scrollupdate";
    need_flush_scroll_update_gesture_ = false;
  }
}

std::unique_ptr<EventWithCallback> InputHandlerProxyUtils::OverScrollRunCallback(
  std::unique_ptr<EventWithCallback> event_with_callback,
  ui::LatencyInfo monitored_latency_info,
  WebInputEventAttribution attribution) {
  if (proxy_->elastic_overscroll_controller_) {
    auto helper = proxy_->elastic_overscroll_controller_->GetUtils()->GetScrollElasticityHelper();
    if (event_with_callback->event().IsGestureScroll() &&
        !proxy_->input_handler_->IsCurrentlyScrolling() && helper &&
        !helper->StretchAmount().IsZero()) {
      event_with_callback->RunCallbacks(InputHandlerProxy::DID_HANDLE, monitored_latency_info,
                                        std::move(proxy_->current_overscroll_params_),
                                        attribution);
      return nullptr;
    }
  }
  return event_with_callback;
}

std::unique_ptr<ScrollPredictor>
InputHandlerProxyUtils::CreateScrollPredictor() {
  return (base::FeatureList::IsEnabled(blink::features::kResamplingScrollEvents) &&
       proxy_->client_->AllowsScrollResampling() &&
       (base::ohos::IsTabletDevice() || base::ohos::IsPcDevice()))
          ? std::make_unique<ScrollPredictor>()
          : nullptr;
}

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
gfx::Vector2dF InputHandlerProxyUtils::GetOverScrollOffset() {
  gfx::Vector2dF overscroll_offset;
  overscroll_offset.set_x(0.0f);
  overscroll_offset.set_y(0.0f);
  if (!proxy_->elastic_overscroll_controller_) {
    LOG(ERROR) << "Error:Overscroll controller is not initialized";
    return overscroll_offset;
  }
  return proxy_->elastic_overscroll_controller_->GetUtils()->GetOverScrollOffset();
}
#endif
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void InputHandlerProxyUtils::SetBypassVsyncCondition(int32_t condition) {
  LOG(INFO) << "InputHandlerProxyUtils::SetBypassVsyncCondition condition:"
            << condition;
  proxy_->SetBypassVsyncCondition(condition);
}
#endif

}