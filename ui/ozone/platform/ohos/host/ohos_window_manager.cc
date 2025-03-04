// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/ozone/platform/ohos/host/ohos_window_manager.h"

#include "base/containers/contains.h"
#include "ohos/adapter/cursor/cursor.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
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

void OhosWindowManager::OnWindowRequestClose(const WindowInfo& info) {
  auto ohosWindow = windows_.Lookup(info.widgetId);
  if (ohosWindow != nullptr) {
    ohosWindow->OnRequestClose();
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

  for (OhosWindowObserver& observer : observers_) {
    observer.OnWindowAdded(window);
  }

  return widget;
}

void OhosWindowManager::RemoveWindow(gfx::AcceleratedWidget widget,
                                     OhosWindow* window) {
  DCHECK_EQ(window, windows_.Lookup(widget));
  windows_.Remove(widget);

  for (OhosWindowObserver& observer : observers_) {
    observer.OnWindowRemoved(window);
  }
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
    const std::set<gfx::AcceleratedWidget>& ignore) {
  gfx::AcceleratedWidget widget = gfx::kNullAcceleratedWidget;
  for (base::IDMap<OhosWindow*>::const_iterator iter(&windows_);
       !iter.IsAtEnd(); iter.Advance()) {
    const OhosWindow* window = iter.GetCurrentValue();
    if (window->IsVisible() && window->GetBoundsInDIP().Contains(point)) {
      gfx::AcceleratedWidget widget_temp = iter.GetCurrentKey();
      if (!base::Contains(ignore, widget_temp)) {
        widget = widget_temp;
        break;
      }
    }
  }
  return widget;
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

}  // namespace ui
