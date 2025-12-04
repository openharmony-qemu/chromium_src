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

#ifndef UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_BASE_H_
#define UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_BASE_H_

#include "base/memory/raw_ptr.h"
#include "ohos/adapter/drag_drop/drag_drop_common.h"
#include "ui/events/event.h"
#include "ui/events/event_constants.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/ozone/platform/ohos/drag/ohos_window_drag_manager.h"
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
  explicit OhosEventSourceBase(OhosWindowManager* window_manager,
                               OhosWindowDragManager* window_drag_manager);
  ~OhosEventSourceBase() override;

  static constexpr float kMouseMoveLimitDistance = 0.01;
  static constexpr int32_t kTouchpadScrollFingerCount = 2;
  static constexpr int32_t kMicrosecondsUnit = 1000;
  static constexpr int32_t kSimulateTouchEventId = 0;

  static void UpdateKeyFlagsByOhKeyState(EventFlags& key_flags);
  static void UpdateKeyPressedState(EventFlags& key_flags,
                                    const int32_t key_code_left,
                                    const int32_t key_code_right,
                                    const EventFlags event_flag);
  static void UpdateSwitchState(EventFlags& key_flags,
                                const int32_t key_code,
                                const EventFlags event_flag);

  void OnMouseHoverEvent(const gfx::AcceleratedWidget widget_id, const bool is_hover);
  void OnKeyEvent(const gfx::AcceleratedWidget widget_id, KeyEvent& key_event);
  EventFlags GetKeyFlags() { return key_flags_; }

  void UpdateKeyFlags(const EventFlags& key_flags);
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
  virtual void SimulateTouchUp(const gfx::AcceleratedWidget widget_id) {} // NOT_IMPLEMENTED
  void ShiftWindowEvent(const gfx::AcceleratedWidget source_widget_id,
                        const gfx::AcceleratedWidget target_widget_id);
  void StartTabDragging(const gfx::AcceleratedWidget widget_id);
  void EndTabDragging();
  void StartTabDraggingByTouch(const gfx::AcceleratedWidget widget_id,
                               const int32_t finger_id);

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
  const raw_ptr<OhosWindowDragManager> window_drag_manager_;
};
}  // namespace ui
#endif  // UI_OZONE_PLATFORM_OHOS_HOST_OHOS_EVENT_SOURCE_BASE_H_
