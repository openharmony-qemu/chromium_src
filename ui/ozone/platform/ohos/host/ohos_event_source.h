// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_H_

#include <ace/xcomponent/native_interface_xcomponent.h>

#include "base/memory/raw_ptr.h"
#include "ohos/adapter/xcomponent/event/input_event_common.h"
#include "ui/events/event.h"
#include "ui/events/event_constants.h"
#include "ui/events/keycodes/dom/dom_code.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/events/types/event_type.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/ozone/platform/ohos/host/ohos_window_manager.h"
#include "ui/ozone/platform/ohos/host/ohos_window_observer.h"

namespace ui {

using namespace ohos::adapter::xcomponent;

class OhosEventSource : public PlatformEventSource, public OhosWindowObserver {
 public:
  OhosEventSource(OhosEventSource&&) = delete;
  OhosEventSource& operator=(OhosEventSource&&) = delete;
  OhosEventSource(const OhosEventSource&) = delete;
  OhosEventSource& operator=(const OhosEventSource&) = delete;
  explicit OhosEventSource(OhosWindowManager* window_manager);
  ~OhosEventSource() override;

  void OnMouseEvent(const gfx::AcceleratedWidget widget_id,
                    const OH_NativeXComponent_MouseEvent& mouse_event);
  void OnMouseHoverEvent(const gfx::AcceleratedWidget widget_id,
                         const bool is_hover);
  void OnPanEvent(const PanAction action,
                  const gfx::AcceleratedWidget widget_id,
                  const PanEvent& ohos_event);
  void OnKeyEvent(const gfx::AcceleratedWidget widget_id,
                  ui::KeyEvent& key_event);
  void OnTouchEvent(
      const gfx::AcceleratedWidget widget_id,
      const OH_NativeXComponent_TouchEvent& ohos_touch_event,
      const OH_NativeXComponent_TouchPointToolType ohos_touch_point_tool_type,
      const float tilt_x,
      const float tilt_y);
  void OnPinchEvent(const std::string& pinch_step,
                    const gfx::AcceleratedWidget widget_id,
                    const PinchEvent& gesture_event);
  void OnDragEnterEvent(const gfx::AcceleratedWidget widget_id,
                        const ohos::adapter::OhosDragInfo& drag_info);
  void OnDropEvent(const gfx::AcceleratedWidget widget_id,
                   const ohos::adapter::OhosDragInfo& drag_info);
  void OnDragLeaveEvent(const gfx::AcceleratedWidget widget_id);
  void OnDragEndEvent(const gfx::AcceleratedWidget widget_id);
  void OnDragMoveEvent(const gfx::AcceleratedWidget widget_id,
                       const float window_x,
                       const float window_y);
  EventFlags GetKeyFlags() { return key_flags_; }

  void UpdateKeyFlags(const EventType type, const DomCode dom_code);
  void UpdateKeyFlags();

  // OhosWindowObserver
  void OnWindowAdded(OhosWindow* window) override;
  void OnWindowRemoved(OhosWindow* window) override;

  gfx::Point GetCursorScreenPoint();
  void SimulateLeftButtonUp(const gfx::AcceleratedWidget widget_id);

 protected:
  void OnMouseMoveEvent(const gfx::AcceleratedWidget widget_id,
                        const OH_NativeXComponent_MouseEvent& mouse_event);
  void SetTargetAndDispatchEvent(const gfx::AcceleratedWidget widget_id,
                                 Event& event);

 protected:
  EventFlags pointer_flags_{EF_NONE};
  EventFlags key_flags_{EF_NONE};
  gfx::PointF pointer_location_;
  gfx::PointF mouse_wheel_offset_;
  gfx::PointF cursor_screen_point_;
  bool is_fling_active_{false};

 private:
  void CreateAndDispatchFlingEvent(const gfx::AcceleratedWidget widget_id,
                                   const PanEvent& ohos_event,
                                   const EventFlags& event_flags,
                                   const bool is_stop);
  const raw_ptr<OhosWindowManager> window_manager_;
  std::shared_ptr<InputEventCallBack> event_callback_;
  void EndSourceDragIfNeeded();
};
}  // namespace ui
#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_H_
