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
#include <window_manager/oh_window.h>

#include "ohos/adapter/common/constants.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/multiprocess/gpu/gpu_native_process_host.h"
#include "ohos/adapter/window/app_window_adapter.h"

namespace ohos::adapter::xcomponent {

const std::string kBrowserWindowPresuffix = "browser";

static WindowWidgetType ConvertWindowIdToWidgetId(WindowIdType window_id) {
  if (!window_id.starts_with(kBrowserWindowPresuffix) ||
      !std::all_of(window_id.begin() + kBrowserWindowPresuffix.size(),
                   window_id.end(), [](char i) { return isdigit(i); })) {
    return -1;
  }
  std::string window_id_string = window_id.substr(kBrowserWindowPresuffix.size());
  return std::stoi(window_id_string);
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
    return search->second.widget_id;
  }
  return -1;
}

WindowIdType WindowAdapter::GetWindowId(const WindowWidgetType index) {
  WindowIdType window_id = kBrowserWindowPresuffix + std::to_string(index);
  return window_id;
}

WindowIdType WindowAdapter::GetWindowIdByNativeWindow(const WindowType window) {
  WindowIdType window_id;
  for (const auto& [index, window_info] : windows_) {
    if (window_info.window == window) {
      window_id = window_info.window_id;
    }
  }
  return window_id;
}

WindowWidgetType WindowAdapter::GetWidgetId(const WindowType window) {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);
  WindowWidgetType widget_id = -1;
  for (const auto& [index, window_info] : windows_) {
    if (window_info.window == window) {
      widget_id = window_info.widget_id;
    }
  }
  return widget_id;
}

void WindowAdapter::AddWindow(WindowIdType index, WindowType window) {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);

  WindowInfo window_info;
  window_info.window = window;
  window_info.window_id = index;
  window_info.widget_id = ConvertWindowIdToWidgetId(index);
  windows_.emplace(index, window_info);
  if (window_status_observer_ != nullptr) {
    window_status_observer_->OnWindowAdd(window_info);
  }

  ohos::adapter::multiprocess::GpuNativeProcessHost::GetInstance().AddWindow(
      index, window);
}

void WindowAdapter::RemoveWindow(WindowIdType index) {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);

  auto search = windows_.find(index);
  if (search != windows_.end()) {
    if (window_status_observer_ != nullptr) {
      window_status_observer_->OnWindowRemove(search->second);
    }
    windows_.erase(search);
  }

  ohos::adapter::multiprocess::GpuNativeProcessHost::GetInstance()
      .RemoveWindow(index);
}

void WindowAdapter::SetWindowWidget(WindowType window,
                                    WindowWidgetType widget_id) {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);

  for (auto& [index, window_info] : windows_) {
    if (window_info.window == window) {
      window_info.widget_id = widget_id;
    }
  }

  ohos::adapter::multiprocess::GpuNativeProcessHost::GetInstance()
      .SetWindowWidget(GetWindowIdByNativeWindow(window), widget_id);
}

void WindowAdapter::RegistWindowStatus(WindowStatusListener* listener) {
  window_status_observer_ = listener;
}

void WindowAdapter::RegistWindowEvent(WindowWidgetType id,
                                      WindowEventCallBack callback) {
  std::lock_guard<std::mutex> lock(windows_mutex_);
  windowEventCallbacks_.emplace(id, callback);

  cached_event_dispatcher_.DispatchCachedEvent(id, callback);
}

void WindowAdapter::UnregistWindowEvent(WindowWidgetType id) {
  std::lock_guard<std::mutex> lock(windows_mutex_);
  windowEventCallbacks_.erase(id);
}

void WindowAdapter::NotifyWindowEvent(WindowIdType window_id,
                                      std::shared_ptr<Event> event) {
  std::lock_guard<std::mutex> lock(windows_mutex_);
  int32_t widget_id = ConvertWindowIdToWidgetId(window_id);
  auto it = windowEventCallbacks_.find(widget_id);
  if (it != windowEventCallbacks_.end()) {
    auto callback = it->second;
    callback(widget_id, event);
  } else {
    cached_event_dispatcher_.SaveCacheEvent(widget_id, event);
  }
}

WindowWidgetType WindowAdapter::PeekNextWindowWidgetId() {
  return next_window_id_.load() + 1;
}

WindowWidgetType WindowAdapter::NextWindowWidgetId() {
  return ++next_window_id_;
}

WindowWidgetType WindowAdapter::GetWindowWidgetId() {
  return next_window_id_.load();
}

void WindowAdapter::RegistKeyboardHeightEvent(
    WindowWidgetType id,
    WindowKeyboardHeightCallBack callback) {
  std::lock_guard<std::mutex> lock(keyboard_height_mutex_);
  window_keyboard_height_callbacks_.emplace(id, callback);
}

void WindowAdapter::UnRegistKeyboardHeightEvent(WindowWidgetType id) {
  std::lock_guard<std::mutex> lock(keyboard_height_mutex_);
  window_keyboard_height_callbacks_.erase(id);
}

void WindowAdapter::NotifyKeyboardHeightEvent(WindowWidgetType widget_id,
                                              int32_t height) {
  std::lock_guard<std::mutex> lock(keyboard_height_mutex_);
  auto itCallback = window_keyboard_height_callbacks_.find(widget_id);
  if (itCallback != window_keyboard_height_callbacks_.end()) {
    LOGI(
        "WindowAdapter::NotifyKeyboardHeightEvent widgetId: %{public}d, "
        "keyboardHeight: %{public}d", widget_id,
        height);
    auto callback = window_keyboard_height_callbacks_[widget_id];
    callback(widget_id, height);
  }
}

void WindowAdapter::NotifyKeyboardHeightEvent(WindowIdType window_id,
                                              int32_t height) {
  int32_t widget_id = GetWidgetId(window_id);
  if (widget_id < 0) {
    LOGE(
        "WindowAdapter::NotifyKeyboardHeightEvent widget_id "
        "%{public}d of windowId: %{public}s is invaild",
        widget_id, window_id.c_str());
    return;
  }
  NotifyKeyboardHeightEvent(widget_id, height);
}

CrossProcessSyncResult WindowAdapter::SyncWindowToGpuProcess() {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);

  std::vector<std::pair<std::string, void*>> windows;
  for (auto& [window_id, window_info] : windows_) {
    windows.emplace_back(
        std::pair<std::string, void*>(window_id, window_info.window));
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

void WindowAdapter::OnWindowInitDone(WindowWidgetType widget_id) {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);

  for (auto& [index, window_info] : windows_) {
    if (window_info.widget_id == widget_id) {
      window_info.initialized = true;
    }
  }
}

bool WindowAdapter::WindowHasInit(WindowWidgetType widget_id) {
  std::lock_guard<std::mutex> lock_protect(windows_mutex_);

  for (auto& [index, window_info] : windows_) {
    if (window_info.widget_id == widget_id) {
      return window_info.initialized;
    }
  }

  return false;
}

void WindowAdapter::SetWindowPrivacyMode(WindowWidgetType widget_id, bool is_privacy_mode) {
  // get ohos window id form widget_id
  std::vector<int32_t> ids = AppWindowAdapter::GetInstance().GetOriginWindowIds({widget_id});
  if (!ids.empty()) {
    LOGI(
        "OH_WindowManager_SetWindowPrivacyMode window_id: %{public}d,  "
        "privacy_mode: %{public}d",
        widget_id, is_privacy_mode);
    int32_t result = OH_WindowManager_SetWindowPrivacyMode(ids[0], is_privacy_mode);
    if (result != 0) {
      LOGE(
          "OH_WindowManager_SetWindowPrivacyMode result: %{public}d, "
          "widget_id:%{public}d ",
          result, widget_id);
    }
  }
}

void WindowAdapter::SetSystemWindowLimits(WindowLimits window_limits) {
  system_window_limits_ = window_limits;
}

WindowLimits WindowAdapter::GetSystemWindowLimits() const {
  return system_window_limits_;
}

WindowAdapter& WindowAdapter::GetInstance() {
  static WindowAdapter instance;
  return instance;
}

void WindowCachedEventQueue::AddCacheEventToQueue(std::shared_ptr<Event> event) {
  if (event->type() == EventType::ET_WINDOW_CHANGE &&
      !first_occluded_event_received_) {
    auto window_event = static_pointer_cast<WindowEvent>(event);
    if (window_event->window_event_type_ == WindowEventType::WINDOW_OCCLUDED) {
      LOGW("first WINDOW_OCCLUDED event will not cached");
      first_occluded_event_received_ = true;
      return;
    }
  }
  cached_events_.push_back(event);
}

void WindowCachedEventDispatcher::SaveCacheEvent(WindowWidgetType widget_id,
                                                 std::shared_ptr<Event> event) {
  if (widget_id < 0) {
    return;
  }
  auto it = cached_event_map_.find(widget_id);
  if (it == cached_event_map_.end()) {
    auto event_queue = std::make_unique<WindowCachedEventQueue>();
    event_queue->AddCacheEventToQueue(event);
    cached_event_map_[widget_id] = std::move(event_queue);
  } else {
    it->second->AddCacheEventToQueue(event);
  }
}
 
void WindowCachedEventDispatcher::DispatchCachedEvent(
    WindowWidgetType widget_id,
    WindowEventCallBack& callback) {
  auto it = cached_event_map_.find(widget_id);
  if (it != cached_event_map_.end()) {
    std::vector<std::shared_ptr<Event>> events = it->second->GetCachedEvent();
    for (auto event : events) {
      callback(widget_id, event);
    }
  }

  cached_event_map_.erase(widget_id);
}
 
}  // namespace ohos::adapter::xcomponent
