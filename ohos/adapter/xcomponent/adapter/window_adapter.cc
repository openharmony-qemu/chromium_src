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

#include "ohos/adapter/xcomponent/adapter/window_adapter.h"

#include <cctype>
#include <cstddef>
#include <string>
#include <unordered_map>

#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/multiprocess/gpu/gpu_native_process_host.h"

namespace ohos::adapter::xcomponent {

const std::string kBrowserWindowPresuffix = "browser";

static WindowWidgetType ConvertWindowIdToWidgetId(WindowIdType windowId) {
  if (!windowId.starts_with(kBrowserWindowPresuffix) ||
      !std::all_of(windowId.begin() + kBrowserWindowPresuffix.size(),
                   windowId.end(), [](char i) { return isdigit(i); })) {
    return -1;
  }
  std::string windowIdString = windowId.substr(kBrowserWindowPresuffix.size());
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
  WindowIdType windowId = kBrowserWindowPresuffix + std::to_string(index);
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

  ohos::adapter::multiprocess::GpuNativeProcessHost::GetInstance()
      .RemoveWindow(index);
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
  std::lock_guard<std::mutex> lock(windows_mutex_);
  windowEventCallbacks_.emplace(id, callback);
}

void WindowAdapter::UnregistWindowEvent(WindowWidgetType id) {
  std::lock_guard<std::mutex> lock(windows_mutex_);
  windowEventCallbacks_.erase(id);
}

void WindowAdapter::NotifyWindowEvent(WindowWidgetType widgetId,
                                      std::shared_ptr<Event> event) {
  auto it = windowEventCallbacks_.find(widgetId);
  if (it != windowEventCallbacks_.end()) {
    auto callback = it->second;
    callback(widgetId, event);
    TryReissueEvent(widgetId, callback);
  } else {
    TryStoreEvent(widgetId, event);
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

void WindowAdapter::TryReissueEvent(WindowWidgetType widgetId,
                                    WindowEventCallBack callback) {
  auto itEvent = event_map_.find(widgetId);
  if (itEvent != event_map_.end()) {
    LOGI(
        "%{public}s(%{public}d) %{public}s: Reissue event to widget %{public}d",
        __FILE__, __LINE__, __FUNCTION__, widgetId);
    callback(widgetId, event_map_[widgetId]);
    event_map_.erase(widgetId);
  }
}
 
void WindowAdapter::TryStoreEvent(WindowWidgetType widgetId,
                                  std::shared_ptr<Event> event) {
  if (event->type() == EventType::ET_WINDOW_CAPTION_BUTTON_RECT_CHANGE) {
    LOGI(
        "%{public}s(%{public}d) %{public}s: Store caption button event of "
        "widget %{public}d before register callback ",
        __FILE__, __LINE__, __FUNCTION__, widgetId);
    event_map_.emplace(widgetId, event);
  }
}

WindowWidgetType WindowAdapter::NextWindowWidgetId() {
  return ++nextWindowId_;
}

WindowWidgetType WindowAdapter::GetWindowWidgetId() {
  return nextWindowId_.load();
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

void WindowAdapter::SetInitialState(const WindowStatusType state) {
  initial_state_ = state;
}

void WindowAdapter::SetInitialBounds(const WindowRect& rect,
                                     const WindowRect& content_rect) {
  window_bounds_ = std::move(rect);
  content_bounds_ = std::move(content_rect);
  initial_bounds_ = {window_bounds_.left + content_rect.left,
                     window_bounds_.top + content_rect.top,
                     content_rect.width,
                     content_rect.height};
}

WindowStatusType WindowAdapter::GetInitialState() const {
  return initial_state_;
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
