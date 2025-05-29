// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

#include <cctype>
#include <cstddef>
#include <string>
#include <unordered_map>

#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/multiprocess/gpu/gpu_native_process_host.h"

namespace ohos::adapter::xcomponent {

const std::string BROWSER_WINDOW_PRESUFFIX = "browser";
const std::string kDefaultWindow = "browser1";

static WindowWidgetType ConvertWindowIdToWidgetId(WindowIdType windowId) {
  if (!windowId.starts_with(BROWSER_WINDOW_PRESUFFIX) ||
      !std::all_of(windowId.begin() + BROWSER_WINDOW_PRESUFFIX.size(),
                   windowId.end(), [](char i) { return isdigit(i); })) {
    return -1;
  }
  std::string windowIdString = windowId.substr(BROWSER_WINDOW_PRESUFFIX.size());
  return std::stoi(windowIdString);
}

WindowType WindowAdapter::GetWindow(WindowIdType index) {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);
  auto search = windows_.find(index);
  if (search != windows_.end()) {
    return search->second.window;
  }
  return nullptr;
}

WindowWidgetType WindowAdapter::GetWidgetId(const WindowIdType index) {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);
  auto search = windows_.find(index);
  if (search != windows_.end()) {
    return search->second.widgetId;
  }
  return -1;
}

WindowIdType WindowAdapter::GetWindowId(const WindowWidgetType index) {
  WindowIdType windowId = BROWSER_WINDOW_PRESUFFIX + std::to_string(index);
  return windowId;
}

WindowIdType WindowAdapter::GetWindowIdByNativeWindow(const WindowType window) {
  WindowIdType windowId;
  for (const auto& [index, windowInfo] : windows_) {
    if (windowInfo.window == window) {
      windowId = windowInfo.windowId;
    }
  }
  return windowId;
}

WindowWidgetType WindowAdapter::GetWidgetId(const WindowType window) {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);
  WindowWidgetType widgetId = -1;
  for (const auto& [index, windowInfo] : windows_) {
    if (windowInfo.window == window) {
      widgetId = windowInfo.widgetId;
    }
  }
  return widgetId;
}

void WindowAdapter::AddWindow(WindowIdType index, WindowType window) {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);

  WindowInfo windowInfo;
  windowInfo.window = window;
  windowInfo.windowId = index;
  windowInfo.widgetId = ConvertWindowIdToWidgetId(index);
  windows_.emplace(index, windowInfo);
  if (windowStatusObserver_ != nullptr) {
    windowStatusObserver_->OnWindowAdd(windowInfo);
  }

  ohos::adapter::multiprocess::GpuNativeProcessHost::GetInstance().AddWindow(
      index, window);
}

void WindowAdapter::RemoveWindow(WindowIdType index) {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);

  auto search = windows_.find(index);
  if (search != windows_.end()) {
    if (windowStatusObserver_ != nullptr) {
      windowStatusObserver_->OnWindowRemove(search->second);
    }
    windows_.erase(search);
  }

  ohos::adapter::multiprocess::GpuNativeProcessHost::GetInstance().RemoveWindow(
      index);
}

void WindowAdapter::SetWindowWidget(WindowType window,
                                    WindowWidgetType widgetId) {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);

  for (auto& [index, windowInfo] : windows_) {
    if (windowInfo.window == window) {
      windowInfo.widgetId = widgetId;
    }
  }

  ohos::adapter::multiprocess::GpuNativeProcessHost::GetInstance()
      .SetWindowWidget(GetWindowIdByNativeWindow(window), widgetId);
}

void WindowAdapter::RegistWindowStatus(WindowStatusListener* listener) {
  windowStatusObserver_ = listener;
}

void WindowAdapter::RegistWindowEvent(WindowWidgetType id,
                                      WindowEventCallBack callback) {
  windowEventCallbacks_.emplace(id, callback);
}

void WindowAdapter::UnregistWindowEvent(WindowWidgetType id) {
  windowEventCallbacks_.erase(id);
}

void WindowAdapter::NotifyWindowEvent(WindowWidgetType widgetId,
                                      std::shared_ptr<Event> event) {
  for (auto& [id, callback] : windowEventCallbacks_) {
    if (id != widgetId) {
      continue;
    }
    callback(id, event);
  }
}

void WindowAdapter::NotifyWindowEvent(WindowType window,
                                      std::shared_ptr<Event> event) {
  int32_t widget_id = GetWidgetId(window);
  if (widget_id < 0) {
    return;
  }
  NotifyWindowEvent(widget_id, event);
}

void WindowAdapter::NotifyWindowEvent(WindowIdType windowId,
                                      std::shared_ptr<Event> event) {
  int32_t widget_id = GetWidgetId(windowId);
  if (widget_id < 0) {
    return;
  }
  NotifyWindowEvent(widget_id, event);
}

WindowWidgetType WindowAdapter::NextWindowWidgetId() {
  return ++nextWindowId_;
}

WindowWidgetType WindowAdapter::GetWindowWidgetId() {
  return nextWindowId_;
}

void WindowAdapter::RequestCloseWindow(WindowIdType windowId) {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);

  auto search = windows_.find(windowId);
  if (search != windows_.end()) {
    if (windowStatusObserver_ != nullptr) {
      windowStatusObserver_->OnWindowRequestClose(search->second);
    }
  }
}

CrossProcessSyncResult WindowAdapter::SyncWindowToGpuProcess() {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);

  std::vector<std::pair<std::string, void*>> windows;
  for (auto& [windowId, windowInfo] : windows_) {
    windows.emplace_back(
        std::pair<std::string, void*>(windowId, windowInfo.window));
  }
  if (ohos::adapter::multiprocess::GpuNativeProcessHost::GetInstance()
          .InitializeWindowAdapter(windows) != 0) {
    return CrossProcessSyncResult::FAIL;
  }

  return CrossProcessSyncResult::SUCCESS;
}

WindowRect WindowAdapter::GetInitialBounds() const {
  return initial_bounds_;
}

void WindowAdapter::SetInitialBounds(const WindowRect& rect,
                                     const WindowRect& content_rect) {
  window_bounds_ = std::move(rect);
  content_bounds_ = std::move(content_rect);
  initial_bounds_ = {window_bounds_.left + content_rect.left,
                     window_bounds_.top + content_rect.top, content_rect.width,
                     content_rect.height};
}

void WindowAdapter::OnWindowInitDone(WindowWidgetType widgetId) {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);

  for (auto& [index, windowInfo] : windows_) {
    if (windowInfo.widgetId == widgetId) {
      windowInfo.initialized = true;
    }
  }
}

bool WindowAdapter::WindowHasInit(WindowWidgetType widgetId) {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);

  for (auto& [index, windowInfo] : windows_) {
    if (windowInfo.widgetId == widgetId) {
      return windowInfo.initialized;
    }
  }

  return false;
}

WindowAdapter& WindowAdapter::GetInstance() {
  static WindowAdapter instance;
  return instance;
}

}  // namespace ohos::adapter::xcomponent
