// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_OHOS_SCREEN_H_
#define UI_OZONE_PLATFORM_OHOS_OHOS_SCREEN_H_

#include <vector>

#include "base/observer_list.h"
#include "ohos/adapter/screen/screen_adapter.h"
#include "ui/display/display_list.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/gfx/geometry/point.h"
#include "ui/ozone/platform/ohos/host/ohos_window_manager.h"
#include "ui/ozone/public/platform_screen.h"

namespace ui {

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

 private:
  void ConvertDisplay(ohos::adapter::OhosDisplay& ohos_display,
                      display::Display& dst_display);
  void FetchDisplays(display::DisplayList& displays);
  void RegisterDisplayMonitor();
  void RegisterDisplayCallback();

  const raw_ptr<OhosWindowManager> window_manager_;
  const raw_ptr<PlatformEventSource> event_source_;
  display::DisplayList display_list_;
  base::ObserverList<display::DisplayObserver> observers_;
  float device_scale_factor_ = 1.0f;
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_OHOS_SCREEN_H_
