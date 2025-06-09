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
  gfx::AcceleratedWidget GetLocalProcessWidgetAtPoint(
      const gfx::Point& point_in_dip,
      const std::set<gfx::AcceleratedWidget>& ignore,
      const int32_t display_id) const override;

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
