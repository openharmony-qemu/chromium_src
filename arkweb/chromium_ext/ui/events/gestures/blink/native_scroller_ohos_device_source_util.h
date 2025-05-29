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

#ifndef UI_EVENTS_NATIVE_SCROLLER_DEVICE_SOURCE_CONVERTER_H_
#define UI_EVENTS_NATIVE_SCROLLER_DEVICE_SOURCE_CONVERTER_H_

#include "arkweb/chromium_ext/ui/gl/events/native_scroller_ohos.h"
#include "third_party/blink/public/common/input/web_gesture_device.h"

namespace ui {
  namespace ohos_gesture {
    NativeScrollerOhos::GestureDevice ConvertNativeScrollerDeviceSource(
      blink::WebGestureDevice device_source);
  }
}

#endif