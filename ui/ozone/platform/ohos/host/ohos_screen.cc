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

#include "ui/ozone/platform/ohos/host/ohos_screen.h"

#include <string_view>
#include <vector>

#include "base/command_line.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "ohos/adapter/context/context_adapter.h"
#include "ohos/adapter/device_info/device_info.h"
#include "ui/aura/screen_ozone.h"
#include "ui/display/display_finder.h"
#include "ui/display/screen.h"
#include "ui/display/util/display_util.h"
#include "ui/gfx/geometry/dip_util.h"
#include "ui/ozone/platform/ohos/host/ohos_event_source_base.h"
#include "ui/ozone/platform/ohos/host/ohos_window.h"
#include "ui/ozone/public/ozone_switches.h"

namespace ui {

namespace {
// Ozone/ohos display defaults.
constexpr gfx::Size kOhosDisplaySize(-1, -1);

// Parse comma-separated screen width and height.
bool ParseScreenSize(const std::string& screen_size, int* width, int* height) {
  std::vector<std::string_view> width_and_height = base::SplitStringPiece(
      screen_size, ",", base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
  size_t expected_size = 2;
  if (width_and_height.size() != expected_size) {
    return false;
  }

  if (!base::StringToInt(width_and_height[0], width) ||
      !base::StringToInt(width_and_height[1], height)) {
    return false;
  }

  return true;
}

gfx::Rect GetDisplayBounds() {
  gfx::Rect bounds(kOhosDisplaySize);

  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(switches::kOzoneOverrideScreenSize)) {
    int width;
    int height;
    std::string screen_size =
        command_line.GetSwitchValueASCII(switches::kOzoneOverrideScreenSize);
    if (ParseScreenSize(screen_size, &width, &height)) {
      bounds.set_size(gfx::Size(width, height));
    }
  }

  return bounds;
}

}  // namespace

OhosScreen::OhosScreen(OhosWindowManager* window_manager,
                       PlatformEventSource* event_source)
    : window_manager_(window_manager),
      event_source_(event_source),
      native_display_manager_lib_("native_display_manager") {
  FetchDisplays(display_list_);
  RegisterDisplayCallback();
  RegisterDisplayMonitor();

  if (ohos::adapter::device_info::DeviceInfo::SdkApi() <
          ohos::adapter::device_info::SDK_VERSION_20 ||
      !native_display_manager_lib_.IsLoaded() ||
      !LoadAllAvailableAreaFunctions()) {
    RegisterAvailableAreaCallback();
    RegisterAvailableAreaMonitor();
  } else {
    RegisterAvoidAreaCallback();
    RegisterAvailableAreaMonitorByNdk();
  }
}

OhosScreen::~OhosScreen() {
  if (is_available_area_monitor_by_ndk_) {
    auto result =
        UnregisterAvailableAreaListener(available_area_listener_index_);
    if (result != NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK) {
      LOG(ERROR)
          << "Failed to unregister available area change callback. Code: "
          << result;
    }
  }
  register_available_area_listener_ = nullptr;
  unregister_available_area_listener_ = nullptr;
  access_available_area_ = nullptr;
  destroy_available_area_ = nullptr;
}

bool OhosScreen::LoadAllAvailableAreaFunctions() {
  return native_display_manager_lib_.LoadFunction(
             &register_available_area_listener_,
             "OH_NativeDisplayManager_RegisterAvailableAreaChangeListener") &&
         native_display_manager_lib_.LoadFunction(
             &unregister_available_area_listener_,
             "OH_NativeDisplayManager_UnregisterAvailableAreaChangeListener") &&
         native_display_manager_lib_.LoadFunction(
             &access_available_area_,
             "OH_NativeDisplayManager_CreateAvailableArea") &&
         native_display_manager_lib_.LoadFunction(
             &destroy_available_area_,
             "OH_NativeDisplayManager_DestroyAvailableArea");
}

const std::vector<display::Display>& OhosScreen::GetAllDisplays() const {
  return display_list_.displays();
}

display::Display OhosScreen::GetPrimaryDisplay() const {
  auto iter = display_list_.GetPrimaryDisplayIterator();
  DCHECK(iter != display_list_.displays().end());
  return *iter;
}

display::Display OhosScreen::GetDisplayForAcceleratedWidget(
    gfx::AcceleratedWidget widget) const {
  OhosWindow* ohos_window =  window_manager_->GetWindow(widget);
  if (ohos_window == nullptr) {
    LOG(WARNING) << "GetDisplayForAcceleratedWidget GetWindow fail, use primary "
                    "display,widget id:"
                 << widget;
    return GetPrimaryDisplay();
  }

  display::Display target_display = ohos_window->GetCurrentDisplay();
  if (!target_display.is_valid()) {
    LOG(WARNING) << __FUNCTION__ << " get display fail, use primary display,"
                 << "widget id:" << widget;
    return GetPrimaryDisplay();
  }
  return target_display;
}

gfx::Point OhosScreen::GetCursorScreenPoint() const {
  gfx::Point point = (static_cast<OhosEventSourceBase*>(event_source_))
                         ->GetCursorScreenPoint();
  return gfx::ToFlooredPoint(
      gfx::ConvertPointToDips(point, device_scale_factor_));
}

gfx::AcceleratedWidget OhosScreen::GetAcceleratedWidgetAtScreenPoint(
    const gfx::Point& point) const {
  return window_manager_->GetWidgetAtScreenPoint(point);
}

display::Display OhosScreen::GetDisplayNearestPoint(
    const gfx::Point& point) const {
  auto displays = GetAllDisplays();
  if (displays.size() <= 1) {
    return GetPrimaryDisplay();
  }
  return *display::FindDisplayNearestPoint(displays, point);
}

display::Display OhosScreen::GetDisplayMatching(
    const gfx::Rect& match_rect) const {
  const display::Display* matching_display =
      display::FindDisplayWithBiggestIntersection(display_list_.displays(),
                                                  match_rect);
  return matching_display ? *matching_display : GetPrimaryDisplay();
}

void OhosScreen::AddObserver(display::DisplayObserver* observer) {
  display_list_.AddObserver(observer);
}

void OhosScreen::RemoveObserver(display::DisplayObserver* observer) {
  display_list_.RemoveObserver(observer);
}

bool OhosScreen::ConvertDisplay(ohos::adapter::OhosDisplay& ohos_display,
                                display::Display& dst_display) {
  dst_display.set_id(ohos_display.id);

  gfx::Rect configRect = GetDisplayBounds();
  if (configRect.size() == kOhosDisplaySize) {
    dst_display.SetScaleAndBounds(
        ohos_display.scaled_density,
        gfx::Rect(gfx::Size(ohos_display.width,
                            ohos_display.height)));
  } else {
    dst_display.SetScaleAndBounds(ohos_display.scaled_density, configRect);
  }
  gfx::Rect work_area_in_pixels(ohos_display.avail_area.left,
                                ohos_display.avail_area.top,
                                ohos_display.avail_area.width,
                                ohos_display.avail_area.height);
  dst_display.set_work_area(
      gfx::ScaleToEnclosingRect(work_area_in_pixels,
                                1.0f / ohos_display.scaled_density));
  if (dst_display.id() < 0 || dst_display.work_area().IsEmpty()) {
    LOG(ERROR) << "OhosScreen::ConvertDisplay display is invalid, display id: " << dst_display.id()
               << ", display work area is empty: " << dst_display.work_area().IsEmpty();
    return false;
  }

  dst_display.set_label(ohos_display.name);

  switch (ohos_display.rotation) {
    case ohos::adapter::OhosDisplayRotation::ROTATE_0:
      dst_display.set_rotation(display::Display::ROTATE_0);
      break;
    case ohos::adapter::OhosDisplayRotation::ROTATE_90:
      dst_display.set_rotation(display::Display::ROTATE_90);
      break;
    case ohos::adapter::OhosDisplayRotation::ROTATE_180:
      dst_display.set_rotation(display::Display::ROTATE_180);
      break;
    case ohos::adapter::OhosDisplayRotation::ROTATE_270:
      dst_display.set_rotation(display::Display::ROTATE_270);
      break;
    default:
      break;
  }

  dst_display.set_display_frequency(ohos_display.refresh_rate);
  dst_display.set_is_monochrome(false);

  return true;
}

void OhosScreen::FetchDisplays(display::DisplayList& displays) {
  display::Display dst_display;
  ohos::adapter::OhosDisplay ohos_display;

  auto& screenAdapter = ohos::adapter::ScreenAdapter::GetInstance();
  screenAdapter.GetPrimaryDisplay(ohos_display);
  if (ConvertDisplay(ohos_display, dst_display)) {
    device_scale_factor_ = dst_display.device_scale_factor();
    display::SetInternalDisplayIds({ohos_display.id});
    displays.AddOrUpdateDisplay(dst_display,
                                display::DisplayList::Type::PRIMARY);
  }

  std::vector<ohos::adapter::OhosDisplay> ohosdisplays;
  screenAdapter.GetAllDisplays(ohosdisplays);
  for (auto iterator : ohosdisplays) {
    if (!ConvertDisplay(iterator, dst_display)) {
      continue;
    }
    displays.AddOrUpdateDisplay(dst_display,
                                display::DisplayList::Type::NOT_PRIMARY);
  }
}

gfx::AcceleratedWidget OhosScreen::GetLocalProcessWidgetAtPoint(
    const gfx::Point& point,
    const std::set<gfx::AcceleratedWidget>& ignore) const {
  int32_t default_display_id = 0;
  ohos::adapter::OhosDisplay ohos_display;
  auto& screenAdapter = ohos::adapter::ScreenAdapter::GetInstance();
  screenAdapter.GetPrimaryDisplay(ohos_display);
  if (ohos_display.id >= 0) {
    default_display_id = ohos_display.id;
  } else {
    LOG(ERROR)
      << "GetLocalProcessWidgetAtPoint GetPrimaryDisplay fail, display id:"
      << ohos_display.id;
  }
  return window_manager_->GetWidgetAtScreenPointWithIgnore(point, ignore,
                                                           default_display_id);
}

gfx::AcceleratedWidget OhosScreen::GetLocalProcessWidgetAtPoint(
    const gfx::Point& point,
    const std::set<gfx::AcceleratedWidget>& ignore,
    const int32_t display_id) const {
  return window_manager_->GetWidgetAtScreenPointWithIgnore(point, ignore, display_id);
}

gfx::Point OhosScreen::GetCursorScreenPoint(const int32_t display_id) const {
  gfx::Point point = (static_cast<OhosEventSourceBase*>(event_source_))
                         ->GetCursorScreenPoint();
  float device_scale_factor = device_scale_factor_;
  auto it = display_list_.FindDisplayById(display_id);
  if (it != display_list_.displays().end()) {
    device_scale_factor = (*it).device_scale_factor();
  }
  return gfx::ToFlooredPoint(
      gfx::ConvertPointToDips(point, device_scale_factor));
}

void OhosScreen::OnDisplayEventCallback(
    const std::string& event, int32_t display_id) {
  auto task = base::BindOnce(
      [](OhosScreen* screen, const std::string& event, int32_t display_id) {
        screen->OnDisplayEvent(event, display_id);
      },
      this, event, display_id);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE, std::move(task));
}

void OhosScreen::OnDisplayEvent(
    const std::string& event, int32_t display_id) {
  LOG(INFO) << "OhosScreen::OnDisplayEvent, display event: " << event
            << " displayId: " << display_id;
  if (event == "add" || event == "change") {
    display::DisplayList list;
    FetchDisplays(list);
    auto iter = list.FindDisplayById(display_id);
    if (iter != list.displays().end()) {
      if (list.GetPrimaryDisplayIterator()->id() == display_id) {
        display_list_.AddOrUpdateDisplay(
            *iter, display::DisplayList::Type::PRIMARY);
      } else {
        display_list_.AddOrUpdateDisplay(
            *iter, display::DisplayList::Type::NOT_PRIMARY);
      }
    }
  } else if (event == "remove") {
    auto it = display_list_.FindDisplayById(display_id);
    if (it != display_list_.displays().end()) {
      display_list_.RemoveDisplay(display_id);
    }
  }
}

void OhosScreen::RegisterDisplayMonitor() {
  auto& screenAdapter = ohos::adapter::ScreenAdapter::GetInstance();
  screenAdapter.RegisterDisplayMonitor();
}

void OhosScreen::RegisterDisplayCallback() {
  auto& screenAdapter = ohos::adapter::ScreenAdapter::GetInstance();
  screenAdapter.RegisterCallback(
      std::bind(&OhosScreen::OnDisplayEventCallback, this,
                std::placeholders::_1, std::placeholders::_2));
}

void OhosScreen::OnAvailableAreaCallback(
    ohos::adapter::window::WindowRect work_area,
    int32_t display_id) {
  if (display_id < 0 || !work_area.width || !work_area.height) {
    LOG(ERROR) << "OhosScreen::OnAvailableAreaCallback callback is invalid, display id: "
               << display_id << ", work area is (width: " << work_area.width
               << ", height: " << work_area.height << ")";
    return;
  }
 
  auto task =
      base::BindOnce(&OhosScreen::OnAvailableAreaEvent,
                     weak_ptr_factory_.GetWeakPtr(), work_area, display_id);
  base::TaskRunnerOHOS::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
                                                          std::move(task));
}
 
void OhosScreen::OnAvailableAreaEvent(
    ohos::adapter::window::WindowRect work_area,
    int32_t display_id) {
  auto iter = display_list_.FindDisplayById(display_id);
  if (iter == display_list_.displays().end()) {
    LOG(WARNING) << __FUNCTION__
                 << " can not find display by id: " << display_id;
    return;
  }
 
  auto changed_display = *iter;
  float scale_factor = changed_display.device_scale_factor();
  if (std::fabs(scale_factor) < 1e-6f) {
    LOG(ERROR) << __FUNCTION__
                 << " display scale_factor is zero, display id: " << display_id;
    return;
  }
  gfx::Rect work_area_in_pixels(work_area.left, work_area.top, work_area.width,
                                work_area.height);
  changed_display.set_work_area(
      gfx::ScaleToEnclosingRect(work_area_in_pixels, 1.0f / scale_factor));
  display_list_.UpdateDisplay(changed_display);
}
 
void OhosScreen::RegisterAvailableAreaMonitor() {
  auto& screen_adapter = ohos::adapter::ScreenAdapter::GetInstance();
  screen_adapter.RegisterAvailableAreaMonitor();
}
 
void OhosScreen::RegisterAvailableAreaCallback() {
  auto& screen_adapter = ohos::adapter::ScreenAdapter::GetInstance();
  screen_adapter.RegisterAvailableAreaCallback(
      [this](ohos::adapter::window::WindowRect work_area, int32_t display_id) {
        this->OnAvailableAreaCallback(work_area, display_id);
      });
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
NativeDisplayManager_ErrorCode OhosScreen::RegisterAvailableAreaListener(
    OH_NativeDisplayManager_AvailableAreaChangeCallback
        available_area_change_callback,
    uint32_t* listener_index) {
  if (register_available_area_listener_ == nullptr) {
    LOGE("OhosScreen::RegisterAvailableAreaListener func null");
    return DISPLAY_MANAGER_ERROR_DEVICE_NOT_SUPPORTED;
  }
  return register_available_area_listener_(available_area_change_callback, listener_index);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
NativeDisplayManager_ErrorCode OhosScreen::UnregisterAvailableAreaListener(
    uint32_t listener_index) {
  if (unregister_available_area_listener_ == nullptr) {
    LOGE("OhosScreen::UnregisterAvailableAreaListener func null");
    return DISPLAY_MANAGER_ERROR_DEVICE_NOT_SUPPORTED;
  }
  return unregister_available_area_listener_(listener_index);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
NativeDisplayManager_ErrorCode OhosScreen::AccessAvailableArea(
    uint64_t display_id,
    NativeDisplayManager_Rect** available_area) {
  if (access_available_area_ == nullptr) {
    LOGE("OhosScreen::AccessAvailableArea func null");
    return DISPLAY_MANAGER_ERROR_DEVICE_NOT_SUPPORTED;
  }
  return access_available_area_(display_id, available_area);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
NativeDisplayManager_ErrorCode OhosScreen::DestroyAvailableArea(
    NativeDisplayManager_Rect* available_area) {
  if (destroy_available_area_ == nullptr) {
    LOGE("OhosScreen::DestroyAvailableArea func null");
    return DISPLAY_MANAGER_ERROR_DEVICE_NOT_SUPPORTED;
  }
  return destroy_available_area_(available_area);
}

void OhosScreen::OnAvoidAreaCallback(int32_t status_bar_height) {
  if (!is_available_area_monitor_by_ndk_ ||
      !ohos::adapter::ContextAdapter::GetInstance().IsNormalWindowMode()) {
    return;
  }
  auto primary_display = GetPrimaryDisplay();
  int32_t display_id = primary_display.id();
  NativeDisplayManager_Rect* available_area = nullptr;
  auto result = AccessAvailableArea(display_id, &available_area);
  if (result != NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK ||
      !available_area) {
    LOG(ERROR) << "Failed to abtain available area of primary display. Code: "
               << result;
    return;
  }

  available_area->top += status_bar_height;
  available_area->height -= status_bar_height;
  ohos::adapter::window::WindowRect work_area(
      available_area->left, available_area->top,
      static_cast<int32_t>(available_area->width),
      static_cast<int32_t>(available_area->height));
  OnAvailableAreaCallback(work_area, static_cast<int32_t>(display_id));

  DestroyAvailableArea(available_area);
  available_area = nullptr;
}

void OhosScreen::RegisterAvoidAreaCallback() {
  auto& screen_adapter = ohos::adapter::ScreenAdapter::GetInstance();
  screen_adapter.RegisterAvoidAreaCallback(
      [this](int32_t status_bar_height) {
        this->OnAvoidAreaCallback(status_bar_height);
      });
}

void OhosScreen::RegisterAvailableAreaMonitorByNdk() {
  auto result = RegisterAvailableAreaListener(
      AvailableAreaChangeCallback, &available_area_listener_index_);
  if (result != NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK) {
    LOG(ERROR) << "Failed to register available area change callback. Code: "
               << result;
    return;
  }

  is_available_area_monitor_by_ndk_ = true;
}

void OhosScreen::OnAvailableAreaChange(uint64_t display_id) {
  NativeDisplayManager_Rect* available_area = nullptr;
  auto result = AccessAvailableArea(display_id, &available_area);
  if (result != NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK ||
      !available_area) {
    LOG(ERROR) << "Failed to abtain available area of display: " << display_id
               << ". Code: " << result;
    return;
  }

  if (ohos::adapter::ContextAdapter::GetInstance().IsNormalWindowMode() &&
      static_cast<int64_t>(display_id) == GetPrimaryDisplay().id()) {
    auto status_bar_height =
        ohos::adapter::ScreenAdapter::GetInstance().GetStatusBarHeight();
    available_area->top += status_bar_height;
    available_area->height -= status_bar_height;
  }

  ohos::adapter::window::WindowRect work_area(
      available_area->left, available_area->top,
      static_cast<int32_t>(available_area->width),
      static_cast<int32_t>(available_area->height));
  OnAvailableAreaCallback(work_area, static_cast<int32_t>(display_id));

  DestroyAvailableArea(available_area);
  available_area = nullptr;
}

void AvailableAreaChangeCallback(uint64_t display_id) {
  aura::ScreenOzone* screen =
      static_cast<aura::ScreenOzone*>(display::Screen::GetScreen());
  if (screen != nullptr) {
    screen->OnDisplayAvailableAreaChange(display_id);
  }
}

}  // namespace ui
