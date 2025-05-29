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

#include "arkweb/chromium_ext/ui/events/gestures/blink/native_scroller_ohos_device_source_util.h"

namespace ui {
  namespace ohos_gesture {
    NativeScrollerOhos::GestureDevice ConvertNativeScrollerDeviceSource(
        blink::WebGestureDevice device_source) {
      switch (device_source) {
        case blink::WebGestureDevice::kUninitialized:
          return NativeScrollerOhos::GestureDevice::kUninitialized;
        case blink::WebGestureDevice::kTouchpad:
          return NativeScrollerOhos::GestureDevice::kTouchpad;
        case blink::WebGestureDevice::kTouchscreen:
          return NativeScrollerOhos::GestureDevice::kTouchscreen;
        case blink::WebGestureDevice::kSyntheticAutoscroll:
          return NativeScrollerOhos::GestureDevice::kSyntheticAutoscroll;
        case blink::WebGestureDevice::kScrollbar:
          return NativeScrollerOhos::GestureDevice::kScrollbar;
        default:
          return NativeScrollerOhos::GestureDevice::kUninitialized;
      }
    }
  }
}