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

#include <native_window/external_window.h>

#include "ui/ozone/platform/ohos/host/ohos_window_manager.h"

#include "base/containers/contains.h"
#include "ohos/adapter/cursor/cursor.h"
#include "ohos/adapter/device_info/device_info.h"
#include "ohos/adapter/window/app_window_adapter.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ui/display/screen.h"
#include "ui/gfx/geometry/transform.h"
#include "ui/ozone/platform/ohos/common/ohos_util.h"
#include "ui/ozone/platform/ohos/host/ohos_window.h"

namespace ui {

OhosWindowManager::OhosWindowManager() {
  WindowAdapter::GetInstance().RegistWindowStatus(this);
}

OhosWindowManager::~OhosWindowManager() {
  DCHECK(thread_checker_.CalledOnValidThread());
}

void OhosWindowManager::OnWindowAdd(const WindowInfo& info) {
  auto ohosWindow = windows_.Lookup(info.widgetId);
  if (ohosWindow != nullptr) {
    ohosWindow->OnSurfaceCreated();
  }
}

void OhosWindowManager::OnWindowRemove(const WindowInfo& info) {
  auto ohosWindow = windows_.Lookup(info.widgetId);
  if (ohosWindow != nullptr) {
    ohosWindow->OnSurfaceDestoryed();
  }
}

void OhosWindowManager::AddObserver(OhosWindowObserver* observer) {
  observers_.AddObserver(observer);
}

void OhosWindowManager::RemoveObserver(OhosWindowObserver* observer) {
  observers_.RemoveObserver(observer);
}

int32_t OhosWindowManager::AddWindow(gfx::AcceleratedWidget widget,
                                     OhosWindow* window) {
  windows_.AddWithID(window, widget);

  for (OhosWindowObserver& observer : observers_)
    observer.OnWindowAdded(window);

  uint64_t surface_id;
  auto native_window =  util::GetWindowFromWidget(widget);
  if (native_window) {
    auto ret = OH_NativeWindow_GetSurfaceId(reinterpret_cast<OHNativeWindow*>(native_window), &surface_id);
    if (ret != 0) {
      LOG(ERROR) << "get xcomponent surface id error";
      return widget;
    }

    LOG(INFO) << "Get surface id: " << surface_id;
    window->SetSurfaceId(surface_id);
  }

  return widget;
}

void OhosWindowManager::RemoveWindow(gfx::AcceleratedWidget widget,
                                     OhosWindow* window) {
  DCHECK_EQ(window, windows_.Lookup(widget));
  windows_.Remove(widget);

  for (OhosWindowObserver& observer : observers_)
    observer.OnWindowRemoved(window);
}

OhosWindow* OhosWindowManager::GetWindow(gfx::AcceleratedWidget widget) {
  return windows_.Lookup(widget);
}

gfx::AcceleratedWidget OhosWindowManager::GetWidgetAtScreenPoint(
    const gfx::Point& point) {
  gfx::AcceleratedWidget widget = gfx::kNullAcceleratedWidget;
  for (base::IDMap<OhosWindow*>::const_iterator iter(&windows_);
       !iter.IsAtEnd(); iter.Advance()) {
    const OhosWindow* window = iter.GetCurrentValue();
    if (window->GetBoundsInDIP().Contains(point)) {
      widget = iter.GetCurrentKey();
      break;
    }
  }
  return widget;
}

void OhosWindowManager::GrabLocatedEvents(OhosWindow* window) {
  if (window == located_events_grabber_) {
    return;
  }
  auto* old_grabber = located_events_grabber_.get();
  located_events_grabber_ = window;
  if (old_grabber) {
    old_grabber->OnWindowLostCapture();
  }
}

// Removes the window that should grab the located events.
void OhosWindowManager::UngrabLocatedEvents(OhosWindow* window) {
  if (window != located_events_grabber_) {
    return;
  }
  auto* old_grabber = located_events_grabber_.get();
  located_events_grabber_ = nullptr;
  if (old_grabber) {
    old_grabber->OnWindowLostCapture();
  }
}

gfx::AcceleratedWidget OhosWindowManager::GetWidgetAtScreenPointWithIgnore(
    const gfx::Point& point,
    const std::set<gfx::AcceleratedWidget>& ignore,
    const int32_t display_id) {
  if (ohos::adapter::device_info::DeviceInfo::SdkApi() >=
      ohos::adapter::device_info::SDK_VERSION_14) {
    float device_scale_factor =
        display::Screen::GetScreen()->GetPrimaryDisplay().device_scale_factor();
    gfx::PointF point_f(point);
    gfx::Transform trans;
    trans.PostScale(device_scale_factor, device_scale_factor);
    gfx::PointF transformed_point = trans.MapPoint(point_f);

    PointCoordinate coordinate;
    coordinate.x = transformed_point.x();
    coordinate.y = transformed_point.y();
    coordinate.displayId = display_id;

    std::vector<std::string> window_ids =
        AppWindowAdapter::GetInstance().GetWindowsByCoordinate(coordinate);
    for (auto& xcomponent_id : window_ids) {
      gfx::AcceleratedWidget widget_id =
          ui::util::ConvertWindowIdToWidgetId(xcomponent_id);
      auto window = GetWindow(widget_id);
      if (HitWindowAtPoint(window, point, ignore)) {
        return widget_id;
      }
    }
  } else {
    for (base::IDMap<OhosWindow*>::const_iterator iter(&windows_);
         !iter.IsAtEnd(); iter.Advance()) {
      const OhosWindow* window = iter.GetCurrentValue();
      if (HitWindowAtPoint(window, point, ignore)) {
        return iter.GetCurrentKey();
      }
    }
  }
  return gfx::kNullAcceleratedWidget;
}

bool OhosWindowManager::HitWindowAtPoint(
    const OhosWindow* window, const gfx::Point& point,
    const std::set<gfx::AcceleratedWidget>& ignore) {
  if (window && window->IsVisible() &&
      window->GetBoundsInDIP().Contains(point)) {
    gfx::AcceleratedWidget widget_id = window->GetWidget();
    if (!base::Contains(ignore, widget_id)) {
      return true;
    }
  }
  return false;
}

void OhosWindowManager::SetPointerFocusedWindow(
    base::WeakPtr<OhosWindow> window) {
  if (!window) {
      ohos::adapter::SetCursorVisible(true);
      return;
  }
  auto old_focused_window = GetCurrentPointerFocusedWindow();
  if (old_focused_window) {
    old_focused_window->OnPointerFocusChanged(false);
  }
  pointer_focused_window_ = window;
  if (window) {
    window->OnPointerFocusChanged(true);
  }
}

void OhosWindowManager::SetPointerFocusedWindow(
    const gfx::AcceleratedWidget widget) {
  auto window = GetWindow(widget);
  if (window != nullptr) {
    SetPointerFocusedWindow(window->AsWeakPtr());
  }
}

bool OhosWindowManager::IsWindowAtLast() {
  int topWindowCount = 0;
  for (base::IDMap<OhosWindow*>::iterator iter(&windows_);
       !iter.IsAtEnd(); iter.Advance()) {
    OhosWindow* window = iter.GetCurrentValue();
    if (window->AsOhosToplevelWindow()) {
      topWindowCount++;
    }
  }
  return topWindowCount == 1;
}

}  // namespace ui
