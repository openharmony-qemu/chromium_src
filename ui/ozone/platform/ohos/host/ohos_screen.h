// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_OHOS_SCREEN_H_
#define UI_OZONE_PLATFORM_OHOS_OHOS_SCREEN_H_

#include <vector>
#include <window_manager/oh_display_manager.h>

#include "base/observer_list.h"
#include "ohos/adapter/common/shared_library.h"
#include "ohos/adapter/screen/screen_adapter.h"
#include "ohos/adapter/window/window_common.h"
#include "ui/display/display_list.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/gfx/geometry/point.h"
#include "ui/ozone/platform/ohos/host/ohos_window_manager.h"
#include "ui/ozone/public/platform_screen.h"

namespace ui {

using RegisterAvailableAreaListenerFunc = NativeDisplayManager_ErrorCode(
    OH_NativeDisplayManager_AvailableAreaChangeCallback
        available_area_change_callback,
    uint32_t* listener_index);
using UnregisterAvailableAreaListenerFunc =
    NativeDisplayManager_ErrorCode(uint32_t listener_index);
using AccessAvailableAreaFunc =
    NativeDisplayManager_ErrorCode(uint64_t display_id,
                                   NativeDisplayManager_Rect** available_area);
using DestroyAvailableAreaFunc =
    NativeDisplayManager_ErrorCode(NativeDisplayManager_Rect* available_area);

class OhosScreen : public PlatformScreen {
 public:
  OhosScreen(const OhosScreen&) = delete;
  OhosScreen& operator=(const OhosScreen&) = delete;
  OhosScreen(OhosWindowManager* window_manager,
             PlatformEventSource* event_source);

  ~OhosScreen() override;

  // Overridden from ui::PlatformScreen:
  const std::vector<display::Display>& GetAllDisplays() const override;
  display::Display GetPrimaryDisplay() const override;
  display::Display GetDisplayForAcceleratedWidget(
      gfx::AcceleratedWidget widget) const override;
  gfx::Point GetCursorScreenPoint() const override;
  gfx::AcceleratedWidget GetAcceleratedWidgetAtScreenPoint(
      const gfx::Point& point) const override;
  display::Display GetDisplayNearestPoint(
      const gfx::Point& point) const override;
  display::Display GetDisplayMatching(
      const gfx::Rect& match_rect) const override;
  void AddObserver(display::DisplayObserver* observer) override;
  void RemoveObserver(display::DisplayObserver* observer) override;
  gfx::AcceleratedWidget GetLocalProcessWidgetAtPoint(
      const gfx::Point& point,
      const std::set<gfx::AcceleratedWidget>& ignore) const override;
  void OnDisplayEventCallback(const std::string& event, int32_t display_id);
  void OnDisplayEvent(const std::string& event, int32_t display_id);
  void OnAvailableAreaCallback(ohos::adapter::window::WindowRect work_area, int32_t display_id);
  void OnAvailableAreaEvent(ohos::adapter::window::WindowRect work_area, int32_t display_id);
  gfx::AcceleratedWidget GetLocalProcessWidgetAtPoint(
      const gfx::Point& point_in_dip,
      const std::set<gfx::AcceleratedWidget>& ignore,
      const int32_t display_id) const override;
  gfx::Point GetCursorScreenPoint(const int32_t display_id) const override;
  void OnAvailableAreaChange(uint64_t display_id) override;
  void OnAvoidAreaCallback(int32_t status_bar_height);

 private:
  bool LoadAllAvailableAreaFunctions();
  bool ConvertDisplay(ohos::adapter::OhosDisplay& ohos_display,
                      display::Display& dst_display);
  void FetchDisplays(display::DisplayList& displays);
  void RegisterDisplayMonitor();
  void RegisterDisplayCallback();
  void RegisterAvailableAreaMonitor();
  void RegisterAvailableAreaCallback();
  void RegisterAvailableAreaMonitorByNdk();
  void RegisterAvoidAreaCallback();
  NativeDisplayManager_ErrorCode RegisterAvailableAreaListener(
      OH_NativeDisplayManager_AvailableAreaChangeCallback
          available_area_change_callback,
      uint32_t* listener_index);
  NativeDisplayManager_ErrorCode UnregisterAvailableAreaListener(
      uint32_t listener_index);
  NativeDisplayManager_ErrorCode AccessAvailableArea(
      uint64_t display_id,
      NativeDisplayManager_Rect** available_area);
  NativeDisplayManager_ErrorCode DestroyAvailableArea(
      NativeDisplayManager_Rect* available_area);

  RegisterAvailableAreaListenerFunc* register_available_area_listener_ =
      nullptr;
  UnregisterAvailableAreaListenerFunc* unregister_available_area_listener_ =
      nullptr;
  AccessAvailableAreaFunc* access_available_area_ = nullptr;
  DestroyAvailableAreaFunc* destroy_available_area_ = nullptr;

  const raw_ptr<OhosWindowManager> window_manager_;
  const raw_ptr<PlatformEventSource> event_source_;
  display::DisplayList display_list_;
  base::ObserverList<display::DisplayObserver> observers_;
  float device_scale_factor_ = 1.0f;
  bool is_available_area_monitor_by_ndk_ = false;
  ohos::adapter::common::SharedLibrary native_display_manager_lib_;
  uint32_t available_area_listener_index_;

  base::WeakPtrFactory<OhosScreen> weak_ptr_factory_{this};
};

void AvailableAreaChangeCallback(uint64_t display_id);

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_OHOS_SCREEN_H_
