/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 
#ifndef OHOS_ADAPTER_XCOMPONENT_NODE_HANDLE_EVENT_GESTURE_EVENT_COMMON_H_
#define OHOS_ADAPTER_XCOMPONENT_NODE_HANDLE_EVENT_GESTURE_EVENT_COMMON_H_
 
#include <cstdint>
#include <string>
 
#include <arkui/native_gesture.h>
#include <arkui/ui_input_event.h>
 
namespace ohos::adapter::xcomponent {
 
class NodeHandleBaseGestureEvent {
 public:
  NodeHandleBaseGestureEvent(ArkUI_GestureRecognizerType gesture_type,
                             int32_t tool_type,
                             float offset_x,
                             float offset_y)
      : gesture_type_(gesture_type),
        tool_type_(tool_type),
        offset_x_(offset_x),
        offset_y_(offset_y) {};
  virtual ~NodeHandleBaseGestureEvent() = default;
 
  ArkUI_GestureRecognizerType gesture_type() const { return gesture_type_; }
  int32_t tool_type() const { return tool_type_; }
  float offset_x() const { return offset_x_; }
  float offset_y() const { return offset_y_; }
 
  virtual std::string ToString() const = 0;
 
 private:
  ArkUI_GestureRecognizerType gesture_type_;
  // From OH_ArkUI_UIInputEvent_GetToolType, the input tool is the device used
  // to interact with the input source, such as a finger or stylus.
  int32_t tool_type_;
  float offset_x_;
  float offset_y_;
};
 
class NodeHandlePanEvent : public NodeHandleBaseGestureEvent {
 public:
  NodeHandlePanEvent(ArkUI_GestureRecognizerType gesture_type,
                     int32_t tool_type,
                     float offset_x,
                     float offset_y,
                     float velocity_x,
                     float velocity_y)
      : NodeHandleBaseGestureEvent(gesture_type, tool_type, offset_x, offset_y),
        velocity_x_(velocity_x),
        velocity_y_(velocity_y) {}
 
  float velocity_x() const { return velocity_x_; }
  float velocity_y() const { return velocity_y_; }
 
  std::string ToString() const override {
    std::ostringstream oss;
    oss << "NodeHandlePanEvent: { tool_type:" << tool_type()
        << ", offset_x:" << offset_x() << ", offset_y:" << offset_y()
        << ", velocity_x: " << velocity_x_ << ", velocity_y: " << velocity_y_
        << " }";
    return oss.str();
  }
 
 private:
  float velocity_x_;
  float velocity_y_;
};
 
class NodeHandlePinchEvent : public NodeHandleBaseGestureEvent {
 public:
  NodeHandlePinchEvent(ArkUI_GestureRecognizerType gesture_type,
                       int32_t tool_type,
                       float offset_x,
                       float offset_y,
                       float scale)
      : NodeHandleBaseGestureEvent(gesture_type, tool_type, offset_x, offset_y),
        scale_(scale) {};
 
  float scale() const { return scale_; }
 
  std::string ToString() const override {
    std::ostringstream oss;
    oss << "NodeHandlePinchEvent: { tool_type:" << tool_type()
        << ", offset_x:" << offset_x() << ", offset_y:" << offset_y()
        << ", scale: " << scale_ << " }";
    return oss.str();
  }
 
 private:
  float scale_;
};
 
}  // namespace ohos::adapter::xcomponent
 
#endif  // OHOS_ADAPTER_XCOMPONENT_EVENT_GESTURE_EVENT_COMMON_H_