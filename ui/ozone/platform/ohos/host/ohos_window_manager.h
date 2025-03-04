// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_OHOS_OHOS_WINDOW_MANAGER_H_
#define UI_OZONE_PLATFORM_OHOS_OHOS_WINDOW_MANAGER_H_

#include <stdint.h>

#include <set>

#include "base/containers/id_map.h"
#include "base/memory/weak_ptr.h"
#include "base/observer_list.h"
#include "base/threading/thread_checker.h"
#include "ohos/adapter/xcomponent/adapter/window_adapter.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/ozone/platform/ohos/host/ohos_window_observer.h"
#include "ui/ozone/public/surface_factory_ozone.h"

namespace ui {

class OhosWindow;

using namespace ohos::adapter::xcomponent;

class OhosWindowManager : public WindowStatusListener {
 public:
  OhosWindowManager();

  OhosWindowManager(OhosWindowManager&&) = delete;
  OhosWindowManager& operator=(OhosWindowManager&&) = delete;
  OhosWindowManager(const OhosWindowManager&) = delete;
  OhosWindowManager& operator=(const OhosWindowManager&) = delete;

  ~OhosWindowManager() override;

  // WindowStatusListener
  void OnWindowAdd(const WindowInfo& info) override;
  void OnWindowRemove(const WindowInfo& info) override;
  void OnWindowRequestClose(const WindowInfo& info) override;

  void AddObserver(OhosWindowObserver* observer);
  void RemoveObserver(OhosWindowObserver* observer);

  // Register a new window. Returns the window id.
  int32_t AddWindow(gfx::AcceleratedWidget widget, OhosWindow* window);

  // Remove a window.
  void RemoveWindow(gfx::AcceleratedWidget widget, OhosWindow* window);

  // Find a window object by id;
  OhosWindow* GetWindow(gfx::AcceleratedWidget widget);

  gfx::AcceleratedWidget GetWidgetAtScreenPoint(const gfx::Point& point);

  // Stores the window that should grab the located events.
  void GrabLocatedEvents(OhosWindow* window);

  // Removes the window that should grab the located events.
  void UngrabLocatedEvents(OhosWindow* window);

  // Returns current event grabber.
  OhosWindow* located_events_grabber() const { return located_events_grabber_; }

  gfx::AcceleratedWidget GetWidgetAtScreenPointWithIgnore(
      const gfx::Point& point,
      const std::set<gfx::AcceleratedWidget>& ignore);

  // Returns a current focused window by pointer.
  base::WeakPtr<OhosWindow> GetCurrentPointerFocusedWindow() const {
    return pointer_focused_window_;
  }

  // Sets the given window as the pointer focused window.
  // If there already is another, the old one will be unset.
  // If nullptr is passed to |window|, it means pointer focus is unset from
  // any window.
  // The given |window| must be managed by this manager.
  void SetPointerFocusedWindow(base::WeakPtr<OhosWindow> window);
  void SetPointerFocusedWindow(const gfx::AcceleratedWidget widget);
  void SetDragSourceWindow(base::WeakPtr<OhosWindow> window) {
    drag_source_window_ = window;
  }
  base::WeakPtr<OhosWindow> GetDragSourceWindow() {
    return drag_source_window_;
  }

 private:
  base::IDMap<OhosWindow*> windows_;
  base::ThreadChecker thread_checker_;
  base::ObserverList<OhosWindowObserver> observers_;
  std::set<OhosWindow*> menu_window_list_;
  raw_ptr<OhosWindow> located_events_grabber_ = nullptr;
  base::WeakPtr<OhosWindow> pointer_focused_window_{nullptr};
  base::WeakPtr<OhosWindow> drag_source_window_{nullptr};
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_OHOS_OHOS_WINDOW_MANAGER_H_
