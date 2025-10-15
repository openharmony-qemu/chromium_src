// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_NODE_HANDLE_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_NODE_HANDLE_H_

#include <ace/xcomponent/native_interface_xcomponent.h>

#include "ohos/adapter/xcomponent/event/node_handle_input_event_common.h"
#include "ui/events/keycodes/dom/dom_code.h"
#include "ui/events/types/event_type.h"
#include "ui/ozone/platform/ohos/host/ohos_event_source_base.h"

namespace ui {

using namespace ohos::adapter::xcomponent;

struct ArkUI_TouchEventData {
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
};

struct ArkUI_MouseEventData {
  float x;
  float y;
  float screenX;
  float screenY;
  int64_t timestamp;
  int32_t action;
  int32_t button;
};

class OhosEventSourceNodeHandle : public OhosEventSourceBase {
 public:
  OhosEventSourceNodeHandle(OhosEventSourceNodeHandle&&) = delete;
  OhosEventSourceNodeHandle& operator=(OhosEventSourceNodeHandle&&) = delete;
  OhosEventSourceNodeHandle(const OhosEventSourceNodeHandle&) = delete;
  OhosEventSourceNodeHandle& operator=(const OhosEventSourceNodeHandle&) =
      delete;
  explicit OhosEventSourceNodeHandle(OhosWindowManager* window_manager);
  ~OhosEventSourceNodeHandle() override = default;

  void OnTouchEvent(const gfx::AcceleratedWidget widget_id,
                    const ArkUI_TouchEventData& touch_event_data);
  void OnMouseEvent(const gfx::AcceleratedWidget widget_id,
                    const ArkUI_MouseEventData& mouse_event_data);
  void SimulateLeftButtonUp(const gfx::AcceleratedWidget widget_id) override;

  EventType GetTouchAction(const int32_t touch_action);
  EventPointerType GetPointType(const int32_t tool_type);

  // gesture event
  void OnPanEvent(const ArkUI_GestureEventActionType action_type,
                  const gfx::AcceleratedWidget widget_id,
                  const NodeHandlePanEvent& ohos_event);
  void OnPinchEvent(const ArkUI_GestureEventActionType action_type,
                    const gfx::AcceleratedWidget widget_id,
                    const NodeHandlePinchEvent& gesture_event);
  void OnDoubleTapEvent(const gfx::AcceleratedWidget widget_id,
                        const NodeHandleTapEvent& event);

  // OhosWindowObserver
  void OnWindowAdded(OhosWindow* window) override;
  void OnWindowRemoved(OhosWindow* window) override;

 private:
  void OnMouseMoveEvent(const gfx::AcceleratedWidget widget_id,
                        const ArkUI_MouseEventData& mouse_event_data,
                        const gfx::PointF& original_location);
  void CreateAndDispatchFlingEvent(const gfx::AcceleratedWidget widget_id,
                                   const NodeHandlePanEvent& ohos_event,
                                   const EventFlags& event_flags,
                                   const bool is_stop);

  std::shared_ptr<NodeHandleInputEventCallBack> event_callback_;
};
}  // namespace ui
#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_NODE_HANDLE_H_
