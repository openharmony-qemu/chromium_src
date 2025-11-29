// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_XCOMPONENT_ADAPTER_WINDOW_ADAPTER_H_
#define OHOS_ADAPTER_XCOMPONENT_ADAPTER_WINDOW_ADAPTER_H_

#include <atomic>
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
using WindowKeyboardHeightCallBack = std::function<void(int32_t, int32_t)>;

class WindowInfo {
 public:
  WindowType window;
  WindowIdType window_id;
  WindowWidgetType widget_id;
  bool initialized = false;
};

enum class CrossProcessSyncResult { SUCCESS, FAIL };

class ADAPTER_EXPORT_API WindowStatusListener {
 public:
  virtual ~WindowStatusListener() = default;
  virtual void OnWindowAdd(const WindowInfo& info) = 0;
  virtual void OnWindowRemove(const WindowInfo& info) = 0;
};

class ADAPTER_EXPORT_API WindowCachedEventQueue {
 public:
  void AddCacheEventToQueue(std::shared_ptr<Event> event);
  std::vector<std::shared_ptr<Event>> GetCachedEvent() {
    return cached_events_;
  }
 
 private:
  std::vector<std::shared_ptr<Event>> cached_events_;
  // first WINDOW_OCCLUDED event will not cached
  // when window is creating
  bool first_occluded_event_received_ = false;
};

class ADAPTER_EXPORT_API WindowCachedEventDispatcher {
 public:
  void SaveCacheEvent(WindowWidgetType widget_id, std::shared_ptr<Event> event);
  void DispatchCachedEvent(WindowWidgetType widget_id, WindowEventCallBack& callback);
 
 private:
  std::unordered_map<WindowWidgetType, std::unique_ptr<WindowCachedEventQueue>>
      cached_event_map_;
};

class ADAPTER_EXPORT_API WindowAdapter {
 public:
  static WindowAdapter& GetInstance();

  WindowType GetWindow(WindowIdType index);
  WindowWidgetType GetWidgetId(const WindowIdType index);
  WindowIdType GetWindowId(const WindowWidgetType index);
  void AddWindow(WindowIdType index, WindowType window);
  void RemoveWindow(WindowIdType index);
  void SetWindowWidget(WindowType window, WindowWidgetType widget_id);
  void RegistWindowEvent(WindowWidgetType id,
                         WindowEventCallBack callback);
  void UnregistWindowEvent(WindowWidgetType id);
  void RegistWindowStatus(WindowStatusListener*);
  void NotifyWindowEvent(WindowIdType window_id, std::shared_ptr<Event>);
  WindowWidgetType NextWindowWidgetId();
  WindowWidgetType PeekNextWindowWidgetId();
  WindowWidgetType GetWindowWidgetId();

  void RegistKeyboardHeightEvent(WindowWidgetType id,
                                 WindowKeyboardHeightCallBack callback);
  void UnRegistKeyboardHeightEvent(WindowWidgetType widget_id);
  void NotifyKeyboardHeightEvent(WindowWidgetType id, int32_t height);
  void NotifyKeyboardHeightEvent(WindowIdType window_id, int32_t height);

  CrossProcessSyncResult SyncWindowToGpuProcess();

  WindowRect GetInitialBounds() const;
  void SetInitialBounds(const WindowRect& rect, const WindowRect& content_rect);
  WindowStatusType GetInitialState() const;
  void SetInitialState(const WindowStatusType state);
  WindowRect GetWindowBounds() const { return window_bounds_; }
  WindowRect GetContentBounds() const { return content_bounds_; }
  void SetSystemWindowLimits(WindowLimits window_limits);
  WindowLimits GetSystemWindowLimits() const;

  void OnWindowInitDone(WindowWidgetType widget_id);
  bool WindowHasInit(WindowWidgetType widget_id);

  void DisableOcclusionFeature() { disable_occlusion_feature_ = true; }
  bool IsDisableOcclusionFeature() { return disable_occlusion_feature_; }

  void SetInitialDisplayId(int64_t initial_display_id) {
    initial_display_id_ = initial_display_id;
  }
  int64_t GetInitialDisplayId() { return initial_display_id_; }

  void SetWindowPrivacyMode(WindowWidgetType widget_id, bool is_privacy_mode);

  void SetCurrentDisplayId(int64_t display_id) {
    current_display_id_ = display_id;
  }
  int64_t GetCurrentDisplayId() { return current_display_id_; }

 private:
  WindowAdapter() = default;
  ~WindowAdapter() = default;

  WindowIdType GetWindowIdByNativeWindow(const WindowType window);
  WindowWidgetType GetWidgetId(const WindowType window);

  std::mutex windows_mutex_;
  std::unordered_map<WindowWidgetType, WindowEventCallBack>
      window_event_callbacks_;
  std::atomic<WindowWidgetType> next_window_id_ = 0;
  WindowStatusListener* window_status_observer_ = nullptr;
  std::unordered_map<WindowIdType, WindowInfo> windows_;

  std::mutex keyboard_height_mutex_;
  std::unordered_map<WindowWidgetType, WindowKeyboardHeightCallBack>
      window_keyboard_height_callbacks_;

  WindowRect window_bounds_;
  WindowRect content_bounds_;
  WindowRect initial_bounds_;
  int64_t initial_display_id_ = -1;
  int64_t current_display_id_ = -1;
  WindowStatusType initial_state_;
  WindowLimits system_window_limits_;  // in vp

  std::unordered_map<WindowWidgetType, std::shared_ptr<Event>>
    event_map_;

  bool disable_occlusion_feature_ = false;
  WindowCachedEventDispatcher cached_event_dispatcher_;
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_ADAPTER_WINDOW_ADAPTER_H_
