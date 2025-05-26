 /* Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "input_event_ohos.h"
 
namespace blink {
 
std::string InputEventOhos::GetWebEventName(const WebInputEvent& event) {
  WebInputEvent::Type type = event.GetType();
  std::string eventName;
  switch (type) {
    case WebInputEvent::Type::kMouseDown:
      if (event.GetModifiers() & WebInputEvent::Modifiers::kLeftButtonDown) {
        eventName = std::string("MouseDown { kLeftButtonDown }");
      } else if (event.GetModifiers() & WebInputEvent::Modifiers::kRightButtonDown) {
        eventName = std::string("MouseDown { kRightButtonDown }");
      } else if (event.GetModifiers() & WebInputEvent::Modifiers::kMiddleButtonDown) {
        eventName = std::string("MouseDown { kMiddleButtonDown }");
      } else if (event.GetModifiers() & WebInputEvent::Modifiers::kBackButtonDown) {
        eventName = std::string("MouseDown { kBackButtonDown }");
      } else if (event.GetModifiers() & WebInputEvent::Modifiers::kForwardButtonDown) {
        eventName = std::string("MouseDown { kForwardButtonDown }");
      } else {
        eventName = std::string("MouseDown {}");
      }
      break;
    default:
      eventName = WebInputEvent::GetName(event.GetType());
      break;
  }
  return eventName;
}
 
bool InputEventOhos::FilterLogEvent(const WebInputEvent& event) {
  WebInputEvent::Type type = event.GetType();
  switch (type) {
    case WebInputEvent::Type::kMouseUp:
    case WebInputEvent::Type::kMouseDown:
    case WebInputEvent::Type::kTouchStart:
    case WebInputEvent::Type::kTouchEnd:
      return true;
    default:
      return false;
  }
}
 
}  // namespace blink