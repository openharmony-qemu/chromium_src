/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ohos/adapter/xcomponent/event/window_event_common.h"

#include <iostream>
#include <sstream>
#include <string>

namespace ohos::adapter::xcomponent {
std::string WithEnumValue(int value) {
  std::string tmp = "(";
  tmp += std::to_string(value);
  tmp += ")";
  return tmp;
}

std::string WindowEventToString(WindowEventType eventType) {
  std::string name;
  switch (eventType) {
    case WindowEventType::WINDOW_SHOWN:
      name = "WINDOW_SHOWN";
      break;
    case WindowEventType::WINDOW_ACTIVE:
      name = "WINDOW_ACTIVE";
      break;
    case WindowEventType::WINDOW_INACTIVE:
      name = "WINDOW_INACTIVE";
      break;
    case WindowEventType::WINDOW_HIDDEN:
      name = "WINDOW_HIDDEN";
      break;
    case WindowEventType::WINDOW_OCCLUDED:
      name = "WINDOW_OCCLUDED";
      break;
    case WindowEventType::WINDOW_VISIBLE:
      name = "WINDOW_VISIBLE";
      break;
    case WindowEventType::WINDOW_DESTROYED:
      name = "WINDOW_DESTROYED";
      break;
    case WindowEventType::WINDOW_CLOSE:
      name = "WINDOW_CLOSE";
      break;
    default:
      name = "UNKNOWN_EVENT";
  }
  return name + WithEnumValue(static_cast<int>(eventType));
}

std::string WindowStatusToString(WindowStatusType status) {
  std::string name;
  switch (status) {
    case WindowStatusType::UNDEFINED:
      name = "UNDEFINED";
      break;
    case WindowStatusType::FULL_SCREEN:
      name = "FULL_SCREEN";
      break;
    case WindowStatusType::MAXIMIZE:
      name = "MAXIMIZE";
      break;
    case WindowStatusType::MINIMIZE:
      name = "MINIMIZE";
      break;
    case WindowStatusType::FLOATING:
      name = "FLOATING";
      break;
    case WindowStatusType::SPLIT_SCREEN:
      name = "SPLIT_SCREEN";
      break;
    default:
      name = "UNKNOWN_STATUS";
  }
  return name + WithEnumValue(static_cast<int>(status));
}

std::string RectChangeReasonToString(RectChangeReason reason) {
  std::string name;
  switch (reason) {
    case RectChangeReason::UNDEFINED:
      name = "UNDEFINED";
      break;
    case RectChangeReason::MAXIMIZE:
      name = "MAXIMIZE";
      break;
    case RectChangeReason::RECOVER:
      name = "RECOVER";
      break;
    case RectChangeReason::MOVE:
      name = "MOVE";
      break;
    case RectChangeReason::DRAG:
      name = "DRAG";
      break;
    case RectChangeReason::DRAG_START:
      name = "DRAG_START";
      break;
    case RectChangeReason::DRAG_END:
      name = "DRAG_END";
      break;
    default:
      name = "UNKNOWN_REASON";
  }
  return name + WithEnumValue(static_cast<int>(reason));
}

std::string Event::ToString() {
  std::ostringstream oss;
  oss << "Event(type: " << static_cast<int>(type_) << ")";
  return oss.str();
}

std::string SurfaceEvent::ToString() {
  std::ostringstream oss;
  oss << "SurfaceEvent(size: (" << width << "X" << height << "))";
  return oss.str();
}

std::string WindowRectChangeEvent::ToString() {
  std::ostringstream oss;
  oss << "WindowRectChangeEvent(reason: " << static_cast<int>(reason) <<
      ", pos: (" << left << ", " << top << "), size: (" <<
      width << "X" << height << "))";
  return oss.str();
}

std::string WindowStatusChangeEvent::ToString() {
  std::ostringstream oss;
  oss << "WindowStatusChangeEvent(status: " << static_cast<int>(status) << ")";
  return oss.str();
}

std::string WindowSizeChangeEvent::ToString() {
  std::ostringstream oss;
  oss << "WindowSizeChangeEvent(pos: (" << left << ", " << top <<
      "), size: (" << width << "X" << height << "))";
  return oss.str();
}

std::string WindowEvent::ToString() {
  std::ostringstream oss;
  oss << "WindowEvent(type: " << static_cast<int>(window_event_type_) << ")";
  return oss.str();
}

std::string WindowCaptionButtonRectChangeEvent::ToString() {
  std::ostringstream oss;
  oss << "WindowCaptionButtonRectChangeEvent(pos: (" << right << ", " << top <<
      "), size: (" << width << "X" << height << "))";
  return oss.str();
}
}  // namespace ohos::adapter::xcomponent
