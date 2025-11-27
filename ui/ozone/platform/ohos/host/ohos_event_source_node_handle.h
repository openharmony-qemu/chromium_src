/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
  float raw_delta_x;
  float raw_delta_y;
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
  explicit OhosEventSourceNodeHandle(OhosWindowManager* window_manager,
                                     OhosWindowDragManager* window_drag_manager);
  ~OhosEventSourceNodeHandle() override = default;

  void OnTouchEvent(const gfx::AcceleratedWidget widget_id,
                    const ArkUI_TouchEventData& touch_event_data);
  void OnMouseEvent(const gfx::AcceleratedWidget widget_id,
                    const ArkUI_MouseEventData& mouse_event_data,
                    const int32_t display_id,
                    const EventFlags key_flags);
  void SimulateTouchUp(const gfx::AcceleratedWidget widget_id) override;

  EventType GetTouchAction(const int32_t touch_action);
  EventPointerType GetPointType(const int32_t tool_type);

  // gesture event
  void OnPanEvent(const ArkUI_GestureEventActionType action_type,
                  const gfx::AcceleratedWidget widget_id,
                  const NodeHandlePanEvent& ohos_event);
  void OnPinchEvent(const ArkUI_GestureEventActionType action_type,
                    const gfx::AcceleratedWidget widget_id,
                    const NodeHandlePinchEvent& gesture_event);

  // OhosWindowObserver
  void OnWindowAdded(OhosWindow* window) override;
  void OnWindowRemoved(OhosWindow* window) override;

  void SendWindowMouseEventForTabDragNodeHandle(
      const gfx::AcceleratedWidget widget_id,
      std::shared_ptr<ArkUI_MouseEventData> mouse_event_data,
      const int32_t display_id,
      const EventFlags key_flags);

 private:
  void OnMouseMoveEvent(const gfx::AcceleratedWidget widget_id,
                        const ArkUI_MouseEventData& mouse_event_data,
                        const gfx::PointF& original_location,
                        const int32_t display_id);
  void CreateAndDispatchFlingEvent(const gfx::AcceleratedWidget widget_id,
                                   const NodeHandlePanEvent& ohos_event,
                                   const EventFlags& event_flags,
                                   const bool is_stop);

  std::shared_ptr<NodeHandleInputEventCallBack> event_callback_;
};
}  // namespace ui
#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_NODE_HANDLE_H_
