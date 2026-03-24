// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_H_

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <multimodalinput/oh_input_manager.h>

#include "ohos/adapter/xcomponent/event/input_event_common.h"
#include "ui/events/keycodes/dom/dom_code.h"
#include "ui/events/types/event_type.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/ozone/platform/ohos/drag/ohos_window_drag_manager.h"
#include "ui/ozone/platform/ohos/host/ohos_event_source_base.h"

namespace ui {

using namespace ohos::adapter::xcomponent;

class OhosEventSource : public OhosEventSourceBase {
 public:
  OhosEventSource(OhosEventSource&&) = delete;
  OhosEventSource& operator=(OhosEventSource&&) = delete;
  OhosEventSource(const OhosEventSource&) = delete;
  OhosEventSource& operator=(const OhosEventSource&) = delete;
  explicit OhosEventSource(OhosWindowManager* window_manager,
                           OhosWindowDragManager* window_drag_manager);
  ~OhosEventSource() override = default;

  void OnMouseEvent(const gfx::AcceleratedWidget widget_id,
                    const OH_NativeXComponent_MouseEvent& mouse_event,
                    const int32_t display_id,
                    const EventFlags key_flags);
  void OnPanEvent(const PanAction action,
                  const gfx::AcceleratedWidget widget_id,
                  const PanEvent& ohos_event);
  void OnTouchEvent(const gfx::AcceleratedWidget widget_id,
                    const OH_NativeXComponent_TouchEvent& ohos_touch_event,
                    const OH_NativeXComponent_TouchPointToolType ohos_touch_point_tool_type,
                    const TouchPointCoordinate& coordinate,
                    const int32_t display_id);
  void OnPinchEvent(const std::string& pinch_step,
                    const gfx::AcceleratedWidget widget_id,
                    const PinchEvent& gesture_event);

  // OhosWindowObserver
  void OnWindowAdded(OhosWindow* window) override;
  void OnWindowRemoved(OhosWindow* window) override;

  void SimulateTouchUp(const gfx::AcceleratedWidget widget_id) override;

  void SendWindowMouseEventForTabDrag(
      const gfx::AcceleratedWidget widget_id,
      std::shared_ptr<OH_NativeXComponent_MouseEvent> window_mouse_event,
      const int32_t display_id,
      const EventFlags key_flags);
  void SendWindowTouchEventForTabDrag(
      const gfx::AcceleratedWidget widget_id,
      std::shared_ptr<OH_NativeXComponent_TouchEvent> window_touch_event,
      const TouchPointCoordinate& coordinate,
      const int32_t display_id);

 protected:
  void OnMouseMoveEvent(const gfx::AcceleratedWidget widget_id,
                        const OH_NativeXComponent_MouseEvent& mouse_event,
                        const gfx::PointF& original_location,
                        const int32_t display_id);

 private:
  void CreateAndDispatchFlingEvent(const gfx::AcceleratedWidget widget_id,
                                   const PanEvent& ohos_event,
                                   const EventFlags& event_flags,
                                   const bool is_stop);
  std::shared_ptr<InputEventCallBack> event_callback_;
  EventPointerType ConvertOHToolTypeToEventPointerType(
      OH_NativeXComponent_TouchPointToolType ohos_touch_point_tool_type);
  void PrepareXcomponentPointForTouchEvent(
      const gfx::AcceleratedWidget widget_id,
      std::shared_ptr<OH_NativeXComponent_TouchEvent> xcomponent_touch_event,
      const float display_x,
      const float display_y);
  void PrepareXcomponentPointForMouseEvent(
      const gfx::AcceleratedWidget widget_id,
      std::shared_ptr<OH_NativeXComponent_MouseEvent> xcomponent_mouse_event,
      const float display_x,
      const float display_y);
};
}  // namespace ui
#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_H_
