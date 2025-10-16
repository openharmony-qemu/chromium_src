// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_BASE_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_BASE_H_

#include "base/memory/raw_ptr.h"
#include "ohos/adapter/drag_drop/drag_drop_common.h"
#include "ui/events/event.h"
#include "ui/events/event_constants.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/ozone/platform/ohos/host/ohos_window_observer.h"
#include "ui/ozone/platform/ohos/host/ohos_window_manager.h"

namespace ui {

using namespace ohos::adapter::xcomponent;

class OhosEventSourceBase : public PlatformEventSource,
                            public OhosWindowObserver {
 public:
  OhosEventSourceBase(OhosEventSourceBase&&) = delete;
  OhosEventSourceBase& operator=(OhosEventSourceBase&&) = delete;
  OhosEventSourceBase(const OhosEventSourceBase&) = delete;
  OhosEventSourceBase& operator=(const OhosEventSourceBase&) = delete;
  explicit OhosEventSourceBase(OhosWindowManager* window_manager);
  ~OhosEventSourceBase() override;

  static constexpr float kFlingVelocityFactor = 2.25;
  static constexpr float kMouseMoveLimitDistance = 0.01;
  static constexpr int32_t kTouchpadScrollFingerCount = 2;

  void OnMouseHoverEvent(const gfx::AcceleratedWidget widget_id, const bool is_hover);
  void OnKeyEvent(const gfx::AcceleratedWidget widget_id, KeyEvent& key_event);
  EventFlags GetKeyFlags() { return key_flags_; }

  void UpdateKeyFlags(const EventType type, const DomCode dom_code);
  void UpdateKeyFlags();
  void OnDragEnterEvent(const gfx::AcceleratedWidget widget_id,
                        const ohos::adapter::OhosDropData& drop_data);
  void OnDragMoveEvent(const gfx::AcceleratedWidget widget_id,
                       const float window_x,
                       const float window_y);
  void OnDragLeaveEvent(const gfx::AcceleratedWidget widget_id);
  void OnDropEvent(const gfx::AcceleratedWidget widget_id,
                   const ohos::adapter::OhosDropData& drop_data);
  void OnDragEndEvent(const gfx::AcceleratedWidget widget_id);

  gfx::Point GetCursorScreenPoint();
  virtual void SimulateLeftButtonUp(const gfx::AcceleratedWidget widget_id) {} // NOT_IMPLEMENTED

 protected:
  void SetTargetAndDispatchEvent(const gfx::AcceleratedWidget widget_id, Event& event);
  void EndSourceDragIfNeeded();
  bool NearZero(const float num);

  gfx::PointF mouse_wheel_offset_;
  bool is_fling_active_{false};
  float last_scale_ = 1.0f;
  EventFlags pointer_flags_{EF_NONE};
  EventFlags key_flags_{EF_NONE};
  gfx::PointF pointer_location_;
  gfx::PointF cursor_screen_point_;
  const raw_ptr<OhosWindowManager> window_manager_;
};
}  // namespace ui
#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_BASE_H_
