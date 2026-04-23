// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_XCOMPONENT_NODE_HANDLE_EVENT_INPUT_EVENT_COMMON_H_
#define OHOS_ADAPTER_XCOMPONENT_NODE_HANDLE_EVENT_INPUT_EVENT_COMMON_H_

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_gesture.h>
#include <multimodalinput/oh_input_manager.h>

#include "ohos/adapter/drag_drop/drag_drop_common.h"
#include "ohos/adapter/export.h"
#include "ohos/adapter/xcomponent/event/node_handle_gesture_event_common.h"

namespace ohos::adapter::xcomponent {

struct NodeHandleMouseEventData {
  float x;
  float y;
  float screenX;
  float screenY;
  float raw_delta_x;
  float raw_delta_y;
  int64_t timestamp;
  int32_t action;
  int32_t button;
  int32_t display_id;
};

struct NodeHandleTouchEventData {
  float x = 0.0f;
  float y = 0.0f;
  float tilt_x = 0.0f;
  float tilt_y = 0.0f;
  float display_x = 0.0f;
  float display_y = 0.0f;
  int32_t id = 0;
  float force = 0.0f;
  int32_t touch_action = 0;
  int32_t tool_type = 0;
  int64_t timestamp;
  int32_t display_id;
};

struct ADAPTER_EXPORT_API NodeHandleInputEventCallBack {
 public:
  void (*touchEventCallback)(const int32_t widget_id,
                             const ArkUI_UIInputEvent* mouse_event);
  void (*mouseEventCallback)(const int32_t widget_id,
                             const ArkUI_UIInputEvent* mouse_event);
  void (*keyEventCallback)(const int32_t widget_id,
                           const ArkUI_UIInputEvent* key_event);
  void (*mouseHoverEventCallback)(const int32_t widget_id,
                                  const bool is_hover);
  void (*dragEnterEventCallback)(const int32_t widget_id,
                                 const OhosDropData& drag_info);
  void (*dragLeaveEventCallback)(const int32_t widget_id);
  void (*dropEventCallback)(const int32_t widget_id,
                            const OhosDropData& drag_info);
  void (*dragEndEventCallback)(const int32_t widget_id);
  void (*dragMoveEventCallback)(const int32_t widget_id,
                                const float window_x,
                                const float window_y);
  void (*panEventCallback)(const ArkUI_GestureEventActionType action_type,
                           const int32_t widget_id,
                           const NodeHandlePanEvent& event);
  void (*pinchEventCallback)(const ArkUI_GestureEventActionType action_type,
                             const int32_t widget_id,
                             const NodeHandlePinchEvent& event);
  void (*sendWindowMouseEventForTabDragCallback)(
      const int32_t widget_id,
      NodeHandleMouseEventData& window_mouse_data);
  void (*sendWindowTouchEventForTabDragCallback)(
      const int32_t widget_id,
      NodeHandleTouchEventData& window_touch_data);
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_NODE_HANDLE_EVENT_INPUT_EVENT_COMMON_H_
