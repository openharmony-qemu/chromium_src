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

#ifndef OHOS_ADAPTER_XCOMPONENT_EVENT_INPUT_EVENT_COMMON_H_
#define OHOS_ADAPTER_XCOMPONENT_EVENT_INPUT_EVENT_COMMON_H_

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <multimodalinput/oh_input_manager.h>

#include "ohos/adapter/drag_drop/drag_drop_ohos_adapter.h"
#include "ohos/adapter/export.h"
#include "ohos/adapter/xcomponent/event/gesture_event_common.h"

namespace ohos::adapter::xcomponent {

struct TouchPointCoordinate {
    float tilt_x;
    float tilt_y;
    float display_x;
    float display_y;
};

struct ADAPTER_EXPORT_API InputEventCallBack {
 public:
  void (*mouseEventCallback)(const int32_t widget_id,
                             const OH_NativeXComponent_MouseEvent& mouse_event);
  void (*keyEventCallback)(const int32_t widget_id,
                           OH_NativeXComponent_KeyEvent& key_event);
  void (*touchEventCallback)(const int32_t widget_id,
                             const OH_NativeXComponent_TouchEvent& touch_event,
                             const OH_NativeXComponent_TouchPointToolType tool_type,
                             const TouchPointCoordinate& coordinate);
  void (*pan_event_callback)(const PanAction action,
                             const int32_t widget_id,
                             const PanEvent& event);
  void (*pinch_event_callback)(const std::string& pinch_step,
                               const int32_t widget_id,
                               const PinchEvent& event);
  void (*dragEnterEventCallback)(const int32_t widget_id,
                                 const OhosDropData& drag_info);
  void (*dragLeaveEventCallback)(const int32_t widget_id);
  void (*dropEventCallback)(const int32_t widget_id,
                            const OhosDropData& drag_info);
  void (*dragEndEventCallback)(const int32_t widget_id);
  void (*dragMoveEventCallback)(const int32_t widget_id,
                                const float window_x,
                                const float window_y);
  void (*mouse_hover_event_callback)(const int32_t widget_id,
                                     const bool is_hover);
  void (*sendWindowMouseEventForTabDragCallback)(
                            const int32_t widget_id,
                            Input_MouseEvent* window_mouse_event);
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_EVENT_INPUT_EVENT_COMMON_H_
