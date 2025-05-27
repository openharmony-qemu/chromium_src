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
 
#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_INPUT_INPUT_HANDLER_PROXY_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_INPUT_INPUT_HANDLER_PROXY_UTILS_H_


#include "arkweb/build/features/features.h"
#include "third_party/blink/public/mojom/widget/platform_widget.mojom-blink.h"
#include "third_party/blink/renderer/platform/widget/input/input_handler_proxy.h"


namespace blink {

class WebInputEventAttribution;
class InputHandlerProxy;
class EventWithCallback;
class ScrollPredictor;
class NativeEmbedEventQueue;
 
class InputHandlerProxyUtils {
public:
  InputHandlerProxyUtils(InputHandlerProxy* proxy);
  ~InputHandlerProxyUtils();

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  std::unique_ptr<ScrollPredictor> CreateScrollPredictor();

#endif
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  enum NativeEventDisposition {
    NORMAL,
    SEND_NATIVE,
    SEND_VIDEO,
    END_QUEUE,
  };
  using GestureEventCallback = base::OnceCallback<void(bool, bool)>;
  using MouseEventCallback = base::OnceCallback<void(bool, bool)>;
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void WillHandleScrollUpdateForInternalBeginFrame(
      const viz::BeginFrameArgs& args) {
    current_internal_begin_frame_args_ = args;
    need_flush_scroll_update_gesture_ = true;
  }
  void ScrollBy(float delta_x, float delta_y);

  void NeedFlushScrollUpdateGesture(const WebGestureEvent& gesture_event);
  void SetOverscrollMode(int mode);
  void ResetNeedFlushScrollUpdateGesture();
  std::unique_ptr<EventWithCallback> OverScrollRunCallback(
    std::unique_ptr<EventWithCallback> event_with_callback,
    ui::LatencyInfo monitored_latency_info,
    WebInputEventAttribution attribution);
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  gfx::Vector2dF GetOverScrollOffset();
#endif
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void DidNativeSendEvent(std::unique_ptr<EventWithCallback> event_with_callback);
  InputHandlerProxyUtils::NativeEventDisposition DidNativeEmbedEvent(
      const WebInputEvent& event);
  void SetGestureEventResult(bool result, bool stopPropagation);
  void SendNativeEvent(const WebTouchEvent& touch_event,
                       WebInputEvent::Type type,
                       size_t i,
                       bool result = true);
  void NativeHitTestResult(bool native, size_t fingerId, int layerId);
  void SetNativeEmbedMode(bool flag);
  void NativeMouseHitTestResult(bool native, int layerId);
  InputHandlerProxyUtils::NativeEventDisposition DidMouseEmbedEvent(
    const WebInputEvent& event);
  void SetMouseEventResult(bool result, bool stopPropagation);
  void SendMouseNativeEvent(const WebMouseEvent& mouse_event, WebInputEvent::Type type, bool result = true);
#endif
 private:
 raw_ptr<InputHandlerProxy> proxy_;
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  bool need_flush_scroll_update_gesture_ = false;
  viz::BeginFrameArgs current_internal_begin_frame_args_;
#endif
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  std::unique_ptr<NativeEmbedEventQueue> native_event_queue_;
  std::unique_ptr<NativeEmbedEventQueue> native_touch_end_queue_;
  std::string embed_id_ = "-1";
  bool isNativeType_ = true;
  std::unordered_map<size_t, bool> native_map_;
  WebTouchEvent start_touch_event_;
  std::unordered_map<size_t, int> native_id_map_;
  bool native_enabled_ = false;
  gfx::RectF nativeRect_;
  int32_t hit_testing_number_ = 0;
  base::circular_deque<size_t> end_index_queue_;
  std::unique_ptr<NativeEmbedEventQueue> native_mouse_event_queue_;
  std::unique_ptr<NativeEmbedEventQueue> native_mouse_end_queue_;
  WebMouseEvent start_mouse_event_;
  bool isMouseNativeArea_ = false;
  std::unordered_map<size_t, int> mouse_native_layer_map_;
  int mouse_native_layer_id_ = 0;
  int32_t mouse_hit_testing_number_ = 0;
#endif
};
 
}
#endif //THIRD_PARTY_BLINK_RENDERER_PLATFORM_WIDGET_INPUT_INPUT_HANDLER_PROXY_UTILS_H_