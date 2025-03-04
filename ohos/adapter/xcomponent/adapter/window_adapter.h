// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_XCOMPONENT_ADAPTER_WINDOW_ADAPTER_H_
#define OHOS_ADAPTER_XCOMPONENT_ADAPTER_WINDOW_ADAPTER_H_

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "ohos/adapter/export.h"
#include "ohos/adapter/window/window_common.h"
#include "ohos/adapter/xcomponent/event/window_event_common.h"

namespace ohos::adapter::xcomponent {

using WindowType = void*;
using WindowIdType = std::string;
using WindowWidgetType = int32_t;
using namespace ohos::adapter::window;

class WindowInfo {
 public:
  WindowType window;
  WindowIdType windowId;
  WindowWidgetType widgetId;
  bool initialized = false;
};

enum class CrossProcessSyncResult { SUCCESS, FAIL };

class ADAPTER_EXPORT_API WindowStatusListener {
 public:
  virtual ~WindowStatusListener() = default;
  virtual void OnWindowAdd(const WindowInfo& info) = 0;
  virtual void OnWindowRemove(const WindowInfo& info) = 0;
  virtual void OnWindowRequestClose(const WindowInfo& info) = 0;
};

class ADAPTER_EXPORT_API WindowAdapter {
 public:
  static WindowAdapter& GetInstance();

  WindowType GetWindow(WindowIdType index);
  WindowWidgetType GetWidgetId(const WindowIdType index);
  WindowIdType GetWindowId(const WindowWidgetType index);
  void AddWindow(WindowIdType index, WindowType window);
  void RemoveWindow(WindowIdType index);
  void SetWindowWidget(WindowType window, WindowWidgetType widgetId);
  void RegistWindowEvent(WindowWidgetType id, WindowEventCallBack callback);
  void UnregistWindowEvent(WindowWidgetType id);
  void RegistWindowStatus(WindowStatusListener*);
  void NotifyWindowEvent(WindowWidgetType id, std::shared_ptr<Event>);
  void NotifyWindowEvent(WindowType window, std::shared_ptr<Event>);
  void NotifyWindowEvent(WindowIdType windowId, std::shared_ptr<Event>);
  WindowWidgetType NextWindowWidgetId();
  WindowWidgetType GetWindowWidgetId();

  void RequestCloseWindow(WindowIdType windowId);
  CrossProcessSyncResult SyncWindowToGpuProcess();

  WindowRect GetInitialBounds() const;
  void SetInitialBounds(const WindowRect& rect, const WindowRect& content_rect);
  WindowRect GetWindowBounds() const { return window_bounds_; }
  WindowRect GetContentBounds() const { return content_bounds_; }

  void OnWindowInitDone(WindowWidgetType widgetId);
  bool WindowHasInit(WindowWidgetType widgetId);

 private:
  WindowAdapter() = default;
  ~WindowAdapter() = default;

  WindowIdType GetWindowIdByNativeWindow(const WindowType window);
  WindowWidgetType GetWidgetId(const WindowType window);

  std::mutex windows_mutex_;
  std::unordered_map<WindowWidgetType, WindowEventCallBack>
      windowEventCallbacks_;
  WindowWidgetType nextWindowId_ = 0;
  WindowStatusListener* windowStatusObserver_ = nullptr;
  std::unordered_map<WindowIdType, WindowInfo> windows_;

  WindowRect window_bounds_;
  WindowRect content_bounds_;
  WindowRect initial_bounds_;
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_ADAPTER_WINDOW_ADAPTER_H_
