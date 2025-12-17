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
#include <deque>

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/widget/input/elastic_overscroll_controller_utils.h"
#endif

namespace blink {

#define NO_NATIVE_TYPE 100

// LCOV_EXCL_START
InputHandlerProxyUtils::InputHandlerProxyUtils(InputHandlerProxy* proxy)
    : proxy_(proxy) {
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  native_event_queue_ = std::make_unique<NativeEmbedEventQueue>();
  native_touch_end_queue_ = std::make_unique<NativeEmbedEventQueue>();
  native_mouse_event_queue_ = std::make_unique<NativeEmbedEventQueue>();
  native_mouse_end_queue_ = std::make_unique<NativeEmbedEventQueue>();
  for (int i = 0; i < MAX_FINGER_NUMBER; ++i) {
    NativeTouchEventQueues_[i] = std::make_shared<NativeEmbedEventQueue>();
    gesture_status_[i] = WebInputEvent::Type::kTouchEnd;
  }
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
// LCOV_EXCL_STOP

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

void InputHandlerProxyUtils::NativeMouseHitTestResult(bool native,
                                                      int layerId,
                                                      int32_t button) {
  LOG(DEBUG) << "[NativeEmbed] NativeMouseHitTestResult native is : " << native
             << ",button is :" << button;
  TRACE_EVENT1("input", "InputHandlerProxy::NativeMouseHitTestResult", "native",
               native);
  mouse_native_map_[button] = native;
  mouse_hit_testing_number_--;
  if (native) {
    mouse_native_id_map_[button] = layerId;
    SendMouseNativeEvent(start_mouse_event_, WebInputEvent::Type::kMouseDown,
                         button);
  } else if (!native_mouse_event_queue_->empty()) {
    SendMouseNativeEvent(start_mouse_event_, WebInputEvent::Type::kMouseDown,
                         button, false);
    auto event_with_callback = native_mouse_event_queue_->Pop();
    proxy_->DispatchSingleInputEvent(std::move(event_with_callback));
  }
  while (mouse_hit_testing_number_ == 0 && !native_mouse_end_queue_->empty()) {
    auto callback = native_mouse_end_queue_->Pop();
    if (native) {
      const WebMouseEvent& mouse_event = static_cast<const WebMouseEvent&>(callback->event());
      native_mouse_event_queue_->Queue(std::move(callback));
      SendMouseNativeEvent(mouse_event, WebInputEvent::Type::kMouseUp, button);
    } else {
      proxy_->DispatchSingleInputEvent(std::move(callback));
    }
  }
}

void InputHandlerProxyUtils::SendMouseNativeEvent(
    const WebMouseEvent& mouse_event,
    WebInputEvent::Type type,
    int32_t button,
    bool result) {
  TRACE_EVENT2("input", "InputHandlerProxy::SendNativeEvent", "type",
               WebInputEvent::GetName(type), "result", result);
  auto modifiers = static_cast<WebInputEvent::Modifiers>(mouse_event.GetModifiers());
  if (result) {
    float x = mouse_event.PositionInWidget().x();
    float y = mouse_event.PositionInWidget().y();
    int layer_id = mouse_native_id_map_[button];

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
      LOG(DEBUG) << "[NativeEmbed] SendMouseNativeEvent";

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
      LOG(DEBUG) << "[NativeEmbed] SendNativeEvent";
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
  auto modifiers = event.GetModifiers();
  auto is_left_click = modifiers == WebInputEvent::Modifiers::kLeftButtonDown ||
                       modifiers == (WebInputEvent::Modifiers::kLeftButtonDown |
                                    WebInputEvent::Modifiers::kIsAutoRepeat);
  auto is_right_click =
      modifiers == WebInputEvent::Modifiers::kRightButtonDown ||
      modifiers == (WebInputEvent::Modifiers::kRightButtonDown |
                   WebInputEvent::Modifiers::kIsAutoRepeat);
  auto is_mid_click =
      modifiers == WebInputEvent::Modifiers::kMiddleButtonDown ||
      modifiers == (WebInputEvent::Modifiers::kMiddleButtonDown |
                   WebInputEvent::Modifiers::kIsAutoRepeat);

  if (!is_left_click && !is_right_click && !is_mid_click) {
    return NORMAL;
  }

  int32_t button = static_cast<int32_t>(
      is_left_click
          ? WebInputEvent::Modifiers::kLeftButtonDown
          : (is_right_click
                 ? WebInputEvent::Modifiers::kRightButtonDown
                 : (is_mid_click ? WebInputEvent::Modifiers::kMiddleButtonDown
                                 : 0)));
  const WebMouseEvent& mouse_event = static_cast<const WebMouseEvent&>(event);
  InputHandlerProxyUtils::NativeEventDisposition result = NORMAL;
  float x = mouse_event.PositionInWidget().x();
  float y = mouse_event.PositionInWidget().y();

  if (type == WebInputEvent::Type::kMouseDown) {
    cc::LayerImpl* video_layer_impl =
        proxy_->input_handler_->handler_utils()->GetLayerImplIsHitByPoint(gfx::Point(x, y));
    if (video_layer_impl &&
        video_layer_impl->layer_impl_utils()->ShouldInterceptTouchEvent()) {
      mouse_native_id_map_[button] = video_layer_impl->id();
      SendMouseNativeEvent(mouse_event, event.GetType(), button);
      mouse_native_map_[button] = true;
      return SEND_VIDEO;
    }

    if (!native_enabled_) {
        return NORMAL;
    }

    cc::LayerImpl* native_layer_impl = proxy_->input_handler_->handler_utils()->GetNativeLayerImpl(gfx::Point(x, y));
    if (native_layer_impl) {
      start_mouse_event_ = mouse_event;
      proxy_->client_->MouseHitTest(mouse_event, button);
      mouse_hit_testing_number_++;
      result = SEND_NATIVE;
    } else {
      SendMouseNativeEvent(mouse_event, type, button, false);
      mouse_native_map_[button] = false;
      result = NORMAL;
    }
    return result;
  }
  // move
  auto isMouseNativeArea = mouse_native_map_[button];
  if (isMouseNativeArea && type != WebInputEvent::Type::kMouseUp) {
    SendMouseNativeEvent(mouse_event, type, button);
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
  if (mouse_hit_testing_number_ == 0 && isMouseNativeArea) {
    SendMouseNativeEvent(mouse_event, type, button, isMouseNativeArea);
    result = SEND_NATIVE;
  }
  mouse_native_map_[button] = false;
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

// LCOV_EXCL_START
void InputHandlerProxyUtils::SetNativeEmbedMode(bool flag) {
  native_enabled_ = flag;
  LOG(DEBUG) << "[NativeEmbed] SetNativeEmbedMode native_enabled_ is : "
             << native_enabled_;
}
// LCOV_EXCL_STOP

void InputHandlerProxyUtils::SetEnableCustomVideoPlayer(bool flag) {
  enable_custom_video_player_ = flag;
  LOG(DEBUG) << "[NativeEmbed] SetEnableCustomVideoPlayer "
                "enable_custom_video_player_ is : "
             << enable_custom_video_player_;
}

void InputHandlerProxyUtils::NativeEventProcess(
    std::unique_ptr<EventWithCallback> event_with_callback) {
  if (native_enabled_ || enable_custom_video_player_) {
    auto& input_event = event_with_callback->event();
    TRACE_EVENT1("input", "InputHandlerProxyUtils::NativeEventProcess", "type",
                 input_event.GetType());
    if (IsMouseEventType(input_event.GetType())) {
      NativeMouseEventProcess(std::move(event_with_callback));
      return;
    }
    if (IsTouchEventType(input_event.GetType())) {
      NativeTouchEventProcess(std::move(event_with_callback), false);
      return;
    }
  }
  proxy_->DispatchSingleInputEvent(std::move(event_with_callback));
}

bool InputHandlerProxyUtils::NativeTouchEventProcess(
    std::unique_ptr<EventWithCallback> event_with_callback, bool isStartInQueue) {
  const WebInputEvent& input_event = event_with_callback->event();
  auto type = input_event.GetType();
  TRACE_EVENT1("input", "InputHandlerProxyUtils::NativeTouchEventProcess",
               "type", type);
  switch (type) {
    case WebInputEvent::Type::kTouchStart: 
      if (isStartInQueue) {
        return NativeTouchStartProcessInQueue(std::move(event_with_callback));
      } else {
        NativeTouchStartProcess(std::move(event_with_callback));
      }
      break;
    case WebInputEvent::Type::kTouchMove:
      NativeTouchMoveProcess(std::move(event_with_callback));
      break;
    case WebInputEvent::Type::kTouchEnd:
      NativeTouchEndProcess(std::move(event_with_callback));
      break;
    case WebInputEvent::Type::kTouchCancel:
      NativeTouchCancelProcess(std::move(event_with_callback));
      break;
    default:
      break;
  }
  return false;
}

void InputHandlerProxyUtils::NativeMouseEventProcess(
    std::unique_ptr<EventWithCallback> event_with_callback) {
  NativeEventDisposition result = NORMAL;
  auto& input_event = event_with_callback->event();
  result = DidMouseEmbedEvent(input_event);
  LOG(DEBUG) << "[NativeEmbedMouse] DidNativeEmbedEvent return result is : "
             << result;
  if (result == SEND_NATIVE || result == SEND_VIDEO) {
    TRACE_EVENT1("input",
                 "InputHandlerProxy::HandleInputEventWithLatencyInfo::"
                 "NativeMouseEventProcess",
                 "NativeEventDisposition", result);
    native_mouse_event_queue_->Queue(std::move(event_with_callback));
  } else if (result == END_QUEUE) {
    TRACE_EVENT1("input",
                 "InputHandlerProxy::HandleInputEventWithLatencyInfo::"
                 "NativeMouseEventProcess",
                 "NativeEventDisposition", result);
    native_mouse_end_queue_->Queue(std::move(event_with_callback));
  } else {
    proxy_->DispatchSingleInputEvent(std::move(event_with_callback));
  }
}

void InputHandlerProxyUtils::NativeHitTestResultV2(bool native,
                                                   size_t finger_id,
                                                   int layer_Id) {
  LOG(DEBUG) << "[NativeEmbed] NativeHitTestResultV2 finger_id is : " << finger_id
             << " and native is : " << native;
  if (CheckFingerIdOutOfIndex(finger_id)) {
    return;
  }
  std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
      NativeTouchEventQueues_[finger_id];
  std::string trace_name =
      "[NativeEmbedGesture] InputHandlerProxyUtils::NativeHitTestResultV2";
  TRACE_EVENT2(
      "input",
      trace_name.append(", fingerId:").append(std::to_string(finger_id)).c_str(),
      "native", native, "queue is empty:", touchEventQueue->empty());
  if (touchEventQueue->empty()) {
    LOG(ERROR) << "[NativeEmbed] touchEventQueue is empty";
    ResetTouchSequence();
    return;
  }

  if (native) {
    touchEventQueue->SetLayerId(layer_Id);
    touchEventQueue->SetStatus(PEND_NATIVE);
    SendNativeInQueueFrontSeq(finger_id);
  } else {
    touchEventQueue->SetStatus(SEND_BLINK);
    NotifyEventNativeFocusResult(finger_id);
    FlushNativeTouchQueue(finger_id);
  }
}

void InputHandlerProxyUtils::SendNativeInQueueFrontSeq(size_t finger_id) {
  std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
      NativeTouchEventQueues_[finger_id];
  WebInputEvent::Type type = WebInputEvent::Type::kTouchStart;
  std::deque<std::unique_ptr<EventWithCallback>> tmpStack;
  while (type != WebInputEvent::Type::kTouchEnd && !touchEventQueue->empty()) {
    auto event_with_callback = touchEventQueue->Pop();
    const WebTouchEvent& touch_event =
        static_cast<const WebTouchEvent&>(event_with_callback->event());
    type = touch_event.GetType();
    SendEventToNative(touch_event);
    tmpStack.emplace_back(std::move(event_with_callback));
  }
  while (!tmpStack.empty()) {
    auto event_with_callback = std::move(tmpStack.back());
    tmpStack.pop_back();
    touchEventQueue->PushFront(std::move(event_with_callback));
  }
}

bool InputHandlerProxyUtils::HandleTouchStartIfHitVideo(
    const WebTouchEvent& touch_event) {
  int32_t changeIndex = GetTouchChangeIndex(touch_event);
  if (CheckFingerIdOutOfIndex(changeIndex)) {
    return false;
  }
  float x = touch_event.touches[changeIndex].PositionInWidget().x();
  float y = touch_event.touches[changeIndex].PositionInWidget().y();
  int32_t finger_id = touch_event.touches[changeIndex].id;
  if (CheckFingerIdOutOfIndex(finger_id)) {
    return false;
  }
  std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
      NativeTouchEventQueues_[finger_id];

  cc::LayerImpl* video_layer_impl =
      proxy_->input_handler_->handler_utils()->GetLayerImplIsHitByPoint(
          gfx::Point(x, y));
  if (video_layer_impl &&
      video_layer_impl->layer_impl_utils()->ShouldInterceptTouchEvent()) {
    touchEventQueue->SetLayerId(video_layer_impl->id());
    SendEventToNative(touch_event);
    touchEventQueue->SetStatus(PEND_NATIVE);
    return true;
  }
  return false;
}

bool InputHandlerProxyUtils::HandleTouchStartIfHitNative(
    const WebTouchEvent& touch_event) {
  int32_t changeIndex = GetTouchChangeIndex(touch_event);
  if (CheckFingerIdOutOfIndex(changeIndex)) {
    return false;
  }
  float x = touch_event.touches[changeIndex].PositionInWidget().x();
  float y = touch_event.touches[changeIndex].PositionInWidget().y();
  int32_t finger_id = touch_event.touches[changeIndex].id;
  if (CheckFingerIdOutOfIndex(finger_id)) {
    return false;
  }
  if (finger_id < MIN_FINGER_NUMBER || finger_id >= MAX_FINGER_NUMBER) {
    LOG(ERROR) << "[NativeEmbedGesture] finger_id is out of index. fingerId =" << finger_id;
    ResetTouchSequence();
    return false;
  }
  std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
      NativeTouchEventQueues_[finger_id];
  auto status = touchEventQueue->GetStatus();
  cc::LayerImpl* native_layer_impl =
      proxy_->input_handler_->handler_utils()->GetNativeLayerImpl(
          gfx::Point(x, y));
  if (native_layer_impl) {
    const WebTouchPoint& touch_point = touch_event.touches[changeIndex];
    WebPointerEvent pointer_event = WebPointerEvent(touch_event, touch_point);
    std::string trace_name = "InputHandlerProxyUtils::NativeTouchStartProcess";
    TRACE_EVENT2(
        "input",
        trace_name.append(", fingerId:").append(std::to_string(finger_id)).c_str(),
        "isHitNative", "True", "status", status);
    touchEventQueue->SetStatus(PEND_HITTEST);
    proxy_->client_->TouchHitTest(pointer_event, finger_id);
    return true;
  } else {
    std::string trace_name = "InputHandlerProxyUtils::NativeTouchStartProcess";
    TRACE_EVENT2(
        "input",
        trace_name.append(", fingerId:").append(std::to_string(finger_id)).c_str(),
        "isHitNative", "False", "status", status);
    NotifyEventNativeFocusResult(finger_id);
    touchEventQueue->SetStatus(SEND_BLINK);
    return false;
  }
}

void InputHandlerProxyUtils::NativeTouchStartProcess(
    std::unique_ptr<EventWithCallback> event_with_callback) {
  const WebTouchEvent& touch_event =
    static_cast<const WebTouchEvent&>(event_with_callback->event());
  int32_t changeIndex = GetTouchChangeIndex(touch_event);
  if (CheckFingerIdOutOfIndex(changeIndex)) {
    SendToBlink(std::move(event_with_callback));
    return;
  }
  int32_t finger_id = touch_event.touches[changeIndex].id;
  if (CheckFingerIdOutOfIndex(finger_id)) {
    SendToBlink(std::move(event_with_callback));
    return;
  }
  std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
  NativeTouchEventQueues_[finger_id];
  auto status = touchEventQueue->GetStatus();
  switch (status) {
    case INIT:
      HandleTouchStartInit(std::move(event_with_callback), finger_id);
    break;
    case PEND_NATIVE:
    case PEND_HITTEST:
      touchEventQueue->Queue(std::move(event_with_callback));
      break;
    case SEND_BLINK:
    case SEND_NATIVE_BLINK:
    case SEND_TO_NATIVE:
    case SEND_NATIVE_BLINK_CONSUMER:
      LOG(ERROR) << "[NativeEmbedGesture] InputHandlerProxyUtils::NativeTouchStartProcess, status error, "
                 << " status error, status = " << status;
      SendToBlink(std::move(event_with_callback));
      ResetTouchSequence();
      break;
    default:
      break;
  }
}

bool InputHandlerProxyUtils::NativeTouchStartProcessInQueue(
    std::unique_ptr<EventWithCallback> event_with_callback) {
  const WebTouchEvent& touch_event =
  static_cast<const WebTouchEvent&>(event_with_callback->event());
  int32_t changeIndex = GetTouchChangeIndex(touch_event);
  if (CheckFingerIdOutOfIndex(changeIndex)) {
    SendToBlink(std::move(event_with_callback));
    return true;
  }
  int32_t finger_id = touch_event.touches[changeIndex].id;
  if (CheckFingerIdOutOfIndex(finger_id)) {
    SendToBlink(std::move(event_with_callback));
    return true;
  }
  std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
  NativeTouchEventQueues_[finger_id];
  auto status = touchEventQueue->GetStatus();
  switch (status) {
    case INIT:
      return HandleTouchStartInitInQueue(std::move(event_with_callback), finger_id);
    case PEND_NATIVE:
    case PEND_HITTEST:
      LOG(ERROR) << "[NativeEmbedGesture] InputHandlerProxyUtils::NativeTouchStartProcessInQueue"
                 << " status error, status = " << status;
      SendToBlink(std::move(event_with_callback));
      ResetTouchSequence();
      break;
    case SEND_BLINK:
      SendToBlink(std::move(event_with_callback));
      break;
    case SEND_NATIVE_BLINK:
      touchEventQueue->SetStatus(PEND_NATIVE);
      SendToBlink(std::move(event_with_callback));
      break;
    case SEND_TO_NATIVE:
      touchEventQueue->SetStatus(PEND_NATIVE);
      SendToBlink(std::move(event_with_callback), true);
      break;
    case SEND_NATIVE_BLINK_CONSUMER:
      touchEventQueue->SetStatus(PEND_NATIVE);
      SendToBlink(std::move(event_with_callback), false, true);
      break;
    default:
      break;
  }
  return false;
}

void InputHandlerProxyUtils::HandleTouchStartInit(
    std::unique_ptr<EventWithCallback> event_with_callback,
    int32_t finger_id) {
  const WebTouchEvent& touch_event =
      static_cast<const WebTouchEvent&>(event_with_callback->event());
  std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
      NativeTouchEventQueues_[finger_id];
  // customVideoPlayer and nativeEmbed overlay
  bool isHitTopLayer = HandleTouchStartIfHitVideo(touch_event);
  if (isHitTopLayer) {
    touchEventQueue->Queue(std::move(event_with_callback));
    return;
  }
  // nativeEmbedGesture
  if (native_enabled_) {
    bool isHitNativeLayer = HandleTouchStartIfHitNative(touch_event);
    if (isHitNativeLayer) {
      touchEventQueue->Queue(std::move(event_with_callback));
      return;
    }
  }
  touchEventQueue->SetStatus(SEND_BLINK);
  SendToBlink(std::move(event_with_callback));
}

bool InputHandlerProxyUtils::HandleTouchStartInitInQueue(
    std::unique_ptr<EventWithCallback> event_with_callback,
    int32_t finger_id) {
  const WebTouchEvent& touch_event =
      static_cast<const WebTouchEvent&>(event_with_callback->event());
  std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
  NativeTouchEventQueues_[finger_id];  
  // customVideoPlayer and nativeEmbed overlay
  bool isHitTopLayer = HandleTouchStartIfHitVideo(touch_event);
  if (isHitTopLayer) {
    touchEventQueue->PushFront(std::move(event_with_callback));
    return true;
  }
  // nativeEmbedGesture
  if (native_enabled_) {
    bool isHitNativeLayer = HandleTouchStartIfHitNative(touch_event);
    if (isHitNativeLayer) {
      touchEventQueue->PushFront(std::move(event_with_callback));
      return true;
    }
  }
  touchEventQueue->SetStatus(SEND_BLINK);
  SendToBlink(std::move(event_with_callback));
  return false;
}

void InputHandlerProxyUtils::NativeTouchMoveProcess(
    std::unique_ptr<EventWithCallback> event_with_callback) {
  const WebInputEvent& event = event_with_callback->event();
  const WebTouchEvent& touch_event = static_cast<const WebTouchEvent&>(event);
  auto type = event.GetType();
  int32_t changeIndex = GetTouchChangeIndex(touch_event);
  if (CheckFingerIdOutOfIndex(changeIndex)) {
    SendToBlink(std::move(event_with_callback));
    return;
  }
  int32_t finger_id = touch_event.touches[changeIndex].id;
  if (CheckFingerIdOutOfIndex(finger_id)) {
    SendToBlink(std::move(event_with_callback));
    return;
  }
  std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
      NativeTouchEventQueues_[finger_id];
  auto status = touchEventQueue->GetStatus();
  std::string trace_name =
      "InputHandlerProxyUtils::NativeTouchMoveProcess, fingerId:";
  trace_name = trace_name + std::to_string(finger_id) +
               ", changeid:" + std::to_string(changeIndex) +
               ", type: " + WebInputEvent::GetName(type);
  TRACE_EVENT1("input", trace_name.c_str(), "status", status);
  switch (status) {
    case INIT:
    case SEND_BLINK:
      SendToBlink(std::move(event_with_callback));
      break;
    case PEND_HITTEST:
      touchEventQueue->Queue(std::move(event_with_callback));
      break;
    case SEND_NATIVE_BLINK:
      touchEventQueue->SetStatus(PEND_NATIVE);
      SendToBlink(std::move(event_with_callback));
      break;
    case SEND_TO_NATIVE:
      touchEventQueue->SetStatus(PEND_NATIVE);
      SendToBlink(std::move(event_with_callback), true);
      break;
    case SEND_NATIVE_BLINK_CONSUMER:
      touchEventQueue->SetStatus(PEND_NATIVE);
      SendToBlink(std::move(event_with_callback), false, true);
      break;
    case PEND_NATIVE:
      touchEventQueue->Queue(std::move(event_with_callback));
      SendEventToNative(touch_event);
      break;
    default:
      break;
  }
}

void InputHandlerProxyUtils::NativeTouchEndProcess(std::unique_ptr<EventWithCallback> event_with_callback) {
  const WebInputEvent& event = event_with_callback->event();
  const WebTouchEvent& touch_event = static_cast<const WebTouchEvent&>(event);
  auto type = event.GetType();
  int32_t changeIndex = GetTouchChangeIndex(touch_event);
  if (CheckFingerIdOutOfIndex(changeIndex)) {
    SendToBlink(std::move(event_with_callback));
    return;
  }
  int32_t finger_id = touch_event.touches[changeIndex].id;
  if (CheckFingerIdOutOfIndex(finger_id)) {
    SendToBlink(std::move(event_with_callback));
    return;
  }
  std::shared_ptr<NativeEmbedEventQueue> touchEventQueue = NativeTouchEventQueues_[finger_id];
  auto status = touchEventQueue->GetStatus();
  std::string trace_name = "InputHandlerProxyUtils::NativeTouchEndProcess, fingerId:";
  trace_name = trace_name + std::to_string(finger_id) + ", changeid:" + std::to_string(changeIndex) +
               ", type: " + WebInputEvent::GetName(type);
  TRACE_EVENT1("input", trace_name.c_str(), "status", status);
  switch (status) {
    case INIT:
    case SEND_BLINK:
      touchEventQueue->SetStatus(INIT);
      SendToBlink(std::move(event_with_callback));
      break;
    case PEND_HITTEST:
      touchEventQueue->Queue(std::move(event_with_callback));
      break;
    case SEND_NATIVE_BLINK:
      touchEventQueue->SetStatus(INIT);
      SendToBlink(std::move(event_with_callback));
      break;
    case SEND_TO_NATIVE:
      touchEventQueue->SetStatus(INIT);
      SendToBlink(std::move(event_with_callback), true);
      break;
    case SEND_NATIVE_BLINK_CONSUMER:
      touchEventQueue->SetStatus(INIT);
      SendToBlink(std::move(event_with_callback), false, true);
      break;
    case PEND_NATIVE:
      touchEventQueue->Queue(std::move(event_with_callback));
      SendEventToNative(touch_event);
      break;
    default:
      break;
  }
}

void InputHandlerProxyUtils::NativeTouchCancelProcess(
    std::unique_ptr<EventWithCallback> event_with_callback) {
  const WebInputEvent& event = event_with_callback->event();
  const WebTouchEvent& touch_event = static_cast<const WebTouchEvent&>(event);
  for (size_t i = 0; i < touch_event.touches_length; ++i) {
      std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
      NativeTouchEventQueues_[i];
      auto status = touchEventQueue->GetStatus();
      switch (status) {
        case INIT:
        case PEND_HITTEST:
        case SEND_NATIVE_BLINK:
        case SEND_TO_NATIVE:
        case SEND_NATIVE_BLINK_CONSUMER:
        case PEND_NATIVE:
          while (!touchEventQueue->empty()) {
            auto pop_event_with_callback = touchEventQueue->Pop();
            SendToBlink(std::move(pop_event_with_callback), true);
          }
          SendEventToNativeByIndex(touch_event, static_cast<int32_t>(i));
          touchEventQueue->SetStatus(INIT);
          break;
        default:
          break;
    }
  }
  int32_t changeIndex = GetTouchChangeIndex(touch_event);
  if (!CheckFingerIdOutOfIndex(changeIndex)) {
    int32_t finger_id = touch_event.touches[changeIndex].id;
    std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
        NativeTouchEventQueues_[finger_id];
    touchEventQueue->SetStatus(INIT);
  }
  SendToBlink(std::move(event_with_callback));
}

void InputHandlerProxyUtils::SendEventToNativeByIndex(const WebTouchEvent& touch_event, int32_t index) {
  auto type = touch_event.GetType();
  if (CheckFingerIdOutOfIndex(index)) {
    return;
  }
  float x = touch_event.touches[index].PositionInWidget().x();
  float y = touch_event.touches[index].PositionInWidget().y();
  int32_t finger_id = touch_event.touches[index].id;
  if (CheckFingerIdOutOfIndex(index)) {
    return;
  }
  std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
      NativeTouchEventQueues_[finger_id];
  int32_t layer_id = touchEventQueue->GetLayerId();
  cc::LayerImpl* layer_impl =
      proxy_->input_handler_->handler_utils()->GetLayerImplById(layer_id);
  if (layer_impl) {
    if (type == WebInputEvent::Type::kTouchStart) {
      nativeRects_[finger_id] = layer_impl->layer_impl_utils()->GetNativeRect();
    }
    embed_id_ =
        std::to_string(layer_impl->layer_impl_utils()->native_embed_id());
    x = x - nativeRects_[finger_id].x();
    y = y - nativeRects_[finger_id].y();
    LOG(DEBUG) << "[NativeEmbedGesture] SendNativeEvent";
    std::string trace_name = "InputHandlerProxyUtils::SendEventToNative";
    TRACE_EVENT1(
        "input",
        trace_name.append(", fingerId:").append(std::to_string(finger_id)).c_str(),
        "type", WebInputEvent::GetName(type));
    TRACE_EVENT2(
        "input",
        trace_name.append(", fingerId:").append(std::to_string(finger_id)).c_str(),
        "x", x, "y", y);
    proxy_->client_->DidNativeEmbedEvent(type, embed_id_, finger_id, x, y);
  } else {
    LOG(ERROR)
        << "[NativeEmbedGesture] SendNativeEvent error layer_impl is null."
        << "fingerId " << finger_id << ", type: " << type;
  }
}

void InputHandlerProxyUtils::SetGestureEventResult(bool result,
                                                   bool stopPropagation,
                                                   int32_t fingerId) {
  LOG(DEBUG) << "[NativeEmbedGesture] SetGestureEventResult result is : "
             << result << ",stopPropagation is " << stopPropagation
             << ",fingerId: " << fingerId;
  std::string trace_name = "InputHandlerProxyUtils::SetGestureEventResult";
  TRACE_EVENT2(
      "input",
      trace_name.append(", fingerId:").append(std::to_string(fingerId)).c_str(),
      "result", result, "stopPropagation", stopPropagation);
  if (CheckFingerIdOutOfIndex(fingerId)) {
    return;
  }
  std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
      NativeTouchEventQueues_[fingerId];
  if (result && stopPropagation) {
    touchEventQueue->SetStatus(SEND_TO_NATIVE);
  } else if (result && !stopPropagation) {
    touchEventQueue->SetStatus(SEND_NATIVE_BLINK_CONSUMER);
  } else {
    touchEventQueue->SetStatus(SEND_NATIVE_BLINK);
  }
  PopNativeTouchQueue(fingerId);
}

int32_t InputHandlerProxyUtils::GetTouchChangeIndex(
    const WebTouchEvent& touch_event) {
  int32_t changeIndex = -1;
  auto type = touch_event.GetType();
  for (size_t i = 0; i < touch_event.touches_length; ++i) {
    WebTouchPoint::State state = touch_event.touches[i].state;
    if (IsSameEventType(type, state)) {
      changeIndex = i;
    }
  }
  if (changeIndex == -1) {
    std::string str = "Array data:";
    for (size_t i = 0; i < touch_event.touches_length; ++i) {
      int32_t state = static_cast<int32_t>(touch_event.touches[i].state);
      str = str + "{type: " + WebInputEvent::GetName(type) + ", state" +
            std::to_string(state) + "}";
    }
    LOG(ERROR)
        << "[NativeEmbedGesture] InputHandlerProxyUtils::GetTouchChangeIndex, "
           "touches do not satisfy sameEventType:"
        << str;
  }
  return changeIndex;
}

void InputHandlerProxyUtils::FlushNativeTouchQueue(size_t fingerId) {
  std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
      NativeTouchEventQueues_[fingerId];
  auto status = touchEventQueue->GetStatus();
  std::string trace_name = "InputHandlerProxyUtils::FlushNativeTouchQueue";
  TRACE_EVENT1(
      "input",
      trace_name.append(", fingerId:").append(std::to_string(fingerId)).c_str(),
      "status", status);
  while (!touchEventQueue->empty()) {
    auto event_with_callback = touchEventQueue->Pop();
    const WebTouchEvent& touch_event =
        static_cast<const WebTouchEvent&>(event_with_callback->event());
    const WebInputEvent::Type type = touch_event.GetType();

    std::string trace_name2 =
        "InputHandlerProxyUtils::FlushNativeTouchQueue LOOP";
    TRACE_EVENT2("input",
                 trace_name2.append(", fingerId:")
                     .append(std::to_string(fingerId))
                     .c_str(),
                 "status", status, "type", type);
    bool isNeedStopPop = NativeTouchEventProcess(std::move(event_with_callback), true);
    if (isNeedStopPop) {
      break;
    }
  }
}

void InputHandlerProxyUtils::PopNativeTouchQueue(size_t fingerId) {
  std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
      NativeTouchEventQueues_[fingerId];
  auto status = touchEventQueue->GetStatus();
  std::string trace_name = "InputHandlerProxyUtils::PopNativeTouchQueue";
  TRACE_EVENT1(
      "input",
      trace_name.append(", fingerId:").append(std::to_string(fingerId)).c_str(),
      "status", status);
  if (!touchEventQueue->empty()) {
    auto event_with_callback = touchEventQueue->Pop();
    const WebTouchEvent& touch_event =
        static_cast<const WebTouchEvent&>(event_with_callback->event());
    const WebInputEvent::Type type = touch_event.GetType();
    NativeTouchEventProcess(std::move(event_with_callback), true);
  } else {
    std::string trace_name2 =
        "InputHandlerProxyUtils::PopNativeTouchQueue, empty.";
    TRACE_EVENT1("input",
                 trace_name2.append(", fingerId:")
                     .append(std::to_string(fingerId))
                     .c_str(),
                 "status", status);
    ResetTouchSequence();
  }
  if (!touchEventQueue->empty()) {
    auto& event = touchEventQueue->Front();
    const WebTouchEvent& touch_event = static_cast<const WebTouchEvent&>(event);
    const WebInputEvent::Type type = touch_event.GetType();
    if (type == WebInputEvent::Type::kTouchStart) {
      auto event_with_callback = touchEventQueue->Pop();
      NativeTouchStartProcessInQueue(std::move(event_with_callback));
    }
  }
}

void InputHandlerProxyUtils::SendToBlink(
    std::unique_ptr<EventWithCallback> event_with_callback,
    bool isDrop, bool result) {
  TRACE_EVENT1("input", "InputHandlerProxyUtils::SendToBlink", "isDrop",
               isDrop);
  const WebInputEvent& input_event = event_with_callback->event();
  auto type = input_event.GetType();
  const WebTouchEvent& touch_event =
      static_cast<const WebTouchEvent&>(input_event);
  int32_t changeIndex = GetTouchChangeIndex(touch_event);
  if (!CheckFingerIdOutOfIndex(changeIndex)) {
    int32_t finger_id = touch_event.touches[changeIndex].id;
    std::string trace_name = "InputHandlerProxyUtils::SendToBlink";
    TRACE_EVENT1(
        "input",
        trace_name.append(", fingerId:").append(std::to_string(finger_id)).c_str(),
        "type", WebInputEvent::GetName(type));
    if (finger_id >= MIN_FINGER_NUMBER && finger_id < MAX_FINGER_NUMBER) {
      CheckTouchEventSequence(type, finger_id);
    }
  }
  proxy_->DispatchSingleInputEvent(std::move(event_with_callback), isDrop, result);
}

void InputHandlerProxyUtils::CheckTouchEventSequence(WebInputEvent::Type type,
                                                     int32_t finger_id) {
  bool isStartError = type == WebInputEvent::Type::kTouchStart &&
                      gesture_status_[finger_id] != WebInputEvent::Type::kTouchEnd;
  bool isMoveError = type == WebInputEvent::Type::kTouchMove &&
                     gesture_status_[finger_id] != WebInputEvent::Type::kTouchStart;
  bool isEndError = type == WebInputEvent::Type::kTouchEnd &&
                    gesture_status_[finger_id] != WebInputEvent::Type::kTouchStart;
  bool isCancelError = type == WebInputEvent::Type::kTouchCancel &&
                       gesture_status_[finger_id] != WebInputEvent::Type::kTouchStart;
  bool isError = isStartError || isMoveError || isEndError || isCancelError;
  if (isError) {
    LOG(ERROR) << "[NativeEmbedGesture] touch sequence error. the status:"
               << WebInputEvent::GetName(type) << ", last status: "
               << WebInputEvent::GetName(gesture_status_[finger_id])
               << ", fingerId = " << finger_id;
    std::string trace_error_name =
        "InputHandlerProxyUtils::CheckTouchEventSequence, sequence error";
    TRACE_EVENT2("input",
                 trace_error_name.append(", fingerId:")
                     .append(std::to_string(finger_id))
                     .c_str(),
                 "last_type", WebInputEvent::GetName(gesture_status_[finger_id]),
                 "type", WebInputEvent::GetName(type));
  }
  switch (type) {
    case WebInputEvent::Type::kTouchStart:
      gesture_status_[finger_id] = WebInputEvent::Type::kTouchStart;
      break;
    case WebInputEvent::Type::kTouchEnd:
    case WebInputEvent::Type::kTouchCancel:
      gesture_status_[finger_id] = WebInputEvent::Type::kTouchEnd;
      break;
    default:
      break;
  }
}

void InputHandlerProxyUtils::SendEventToNative(const WebTouchEvent& touch_event) {
  auto type = touch_event.GetType();
  int32_t changeIndex = GetTouchChangeIndex(touch_event);
  if (CheckFingerIdOutOfIndex(changeIndex)) {
    return;
  }
  float x = touch_event.touches[changeIndex].PositionInWidget().x();
  float y = touch_event.touches[changeIndex].PositionInWidget().y();
  int32_t finger_id = touch_event.touches[changeIndex].id;
  if (CheckFingerIdOutOfIndex(finger_id)) {
    return;
  }
  std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
      NativeTouchEventQueues_[finger_id];
  int32_t layer_id = touchEventQueue->GetLayerId();
  cc::LayerImpl* layer_impl =
      proxy_->input_handler_->handler_utils()->GetLayerImplById(layer_id);
  if (layer_impl) {
    if (type == WebInputEvent::Type::kTouchStart) {
      nativeRects_[finger_id] = layer_impl->layer_impl_utils()->GetNativeRect();
    }
    embed_id_ =
        std::to_string(layer_impl->layer_impl_utils()->native_embed_id());
    x = x - nativeRects_[finger_id].x();
    y = y - nativeRects_[finger_id].y();
    LOG(DEBUG) << "[NativeEmbedGesture] SendNativeEvent";
    std::string trace_name = "InputHandlerProxyUtils::SendEventToNative";
    TRACE_EVENT1(
        "input",
        trace_name.append(", fingerId:").append(std::to_string(finger_id)).c_str(),
        "type", WebInputEvent::GetName(type));
    TRACE_EVENT2(
        "input",
        trace_name.append(", fingerId:").append(std::to_string(finger_id)).c_str(),
        "x", x, "y", y);
    proxy_->client_->DidNativeEmbedEvent(type, embed_id_, finger_id, x, y);
  } else {
    ResetTouchSequence();
    LOG(ERROR)
        << "[NativeEmbedGesture] SendNativeEvent error layer_impl is null."
        << "fingerId " << finger_id << ", type: " << type;
  }
}

void InputHandlerProxyUtils::NotifyEventNativeFocusResult(size_t fingerId) {
  TRACE_EVENT1("input", "InputHandlerProxyUtils::NotifyEventNativeFocusResult",
               "fingerId", fingerId);
  proxy_->client_->DidNativeEmbedEvent(WebInputEvent::Type::kTouchMove,
                                       embed_id_, NO_NATIVE_TYPE, 0, 0);
}

void InputHandlerProxyUtils::ResetTouchSequence() {
  TRACE_EVENT("input", "InputHandlerProxyUtils::ResetTouchSequence");
  LOG(INFO) << "[NativeEmbedGesture] ResetTouchSequence.";
  for (int i = 0; i < MAX_FINGER_NUMBER; i++) {
    std::shared_ptr<NativeEmbedEventQueue> touchEventQueue =
      NativeTouchEventQueues_[i];
    touchEventQueue->SetStatus(INIT);
    touchEventQueue->SetLayerId(0);
    while (!touchEventQueue->empty()) {
      auto event_with_callback = touchEventQueue->Pop();
      proxy_->DispatchSingleInputEvent(std::move(event_with_callback));
    }
  }
}

bool InputHandlerProxyUtils::CheckFingerIdOutOfIndex(int32_t finger_id) {
  if (finger_id < MIN_FINGER_NUMBER || finger_id >= MAX_FINGER_NUMBER) {
    LOG(ERROR) << "[NativeEmbedGesture] finger_id is out of index. fingerId = " << finger_id;
    ResetTouchSequence();
    return true;
  }
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
// LCOV_EXCL_START
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
// LCOV_EXCL_STOP

void InputHandlerProxyUtils::NeedFlushScrollUpdateGesture(
  const WebGestureEvent& gesture_event) {
  if (need_flush_scroll_update_gesture_ &&
    gesture_event.GetType() ==
        WebGestureEvent::Type::kGestureScrollUpdate) {
    proxy_->DeliverInputForBeginFrame(current_internal_begin_frame_args_);
  }
}

// LCOV_EXCL_START
void InputHandlerProxyUtils::ResetNeedFlushScrollUpdateGesture() {
  if (need_flush_scroll_update_gesture_) {
    LOG(INFO) << "InputHandlerProxy::HandleGestureScrollUpdate "
                 "internalbeginframe scrollupdate";
    need_flush_scroll_update_gesture_ = false;
  }
}
// LCOV_EXCL_STOP

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

// LCOV_EXCL_START
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

void InputHandlerProxyUtils::SetClientForElasticOverScrollController() {
  if (proxy_ && proxy_->elastic_overscroll_controller_ &&
      proxy_->elastic_overscroll_controller_->GetUtils()) {
    proxy_->elastic_overscroll_controller_->GetUtils()
        ->SetInputHandlerProxyClient(proxy_->client_);
  }
}
#endif
// LCOV_EXCL_STOP
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_VSYNC_SCHEDULE)
void InputHandlerProxyUtils::SetBypassVsyncCondition(int32_t condition) {
  LOG(INFO) << "InputHandlerProxyUtils::SetBypassVsyncCondition condition:"
            << condition;
  proxy_->SetBypassVsyncCondition(condition);
}
#endif
// LCOV_EXCL_STOP

}