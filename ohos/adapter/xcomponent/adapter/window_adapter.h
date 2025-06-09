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
  void RegistWindowEvent(WindowWidgetType id,
                         WindowEventCallBack callback);
  void UnregistWindowEvent(WindowWidgetType id);
  void RegistWindowStatus(WindowStatusListener*);
  void NotifyWindowEvent(WindowWidgetType id, std::shared_ptr<Event>);
  void NotifyWindowEvent(WindowType window, std::shared_ptr<Event>);
  void NotifyWindowEvent(WindowIdType windowId, std::shared_ptr<Event>);
  void TryReissueEvent(WindowWidgetType widgetId, WindowEventCallBack callback);
  void TryStoreEvent(WindowWidgetType widgetId, std::shared_ptr<Event> event);
  WindowWidgetType NextWindowWidgetId();
  WindowWidgetType GetWindowWidgetId();

  CrossProcessSyncResult SyncWindowToGpuProcess();

  WindowRect GetInitialBounds() const;
  void SetInitialBounds(const WindowRect& rect, const WindowRect& content_rect);
  WindowStatusType GetInitialState() const;
  void SetInitialState(const WindowStatusType state);
  WindowRect GetWindowBounds() const { return window_bounds_; }
  WindowRect GetContentBounds() const { return content_bounds_; }

  void OnWindowInitDone(WindowWidgetType widgetId);
  bool WindowHasInit(WindowWidgetType widgetId);

  void DisableOcclusionFeature() { disable_occlusion_feature_ = true; }
  bool GetOcclusionFeature() { return disable_occlusion_feature_; }

 private:
  WindowAdapter() = default;
  ~WindowAdapter() = default;

  WindowIdType GetWindowIdByNativeWindow(const WindowType window);
  WindowWidgetType GetWidgetId(const WindowType window);

  std::mutex windows_mutex_;
  std::unordered_map<WindowWidgetType, WindowEventCallBack>
      windowEventCallbacks_;
  std::atomic<WindowWidgetType> nextWindowId_ = 0;
  WindowStatusListener* windowStatusObserver_ = nullptr;
  std::unordered_map<WindowIdType, WindowInfo> windows_;

  WindowRect window_bounds_;
  WindowRect content_bounds_;
  WindowRect initial_bounds_;
  WindowStatusType initial_state_;

  std::unordered_map<WindowWidgetType, std::shared_ptr<Event>>
    event_map_;

  bool disable_occlusion_feature_ = false;
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_ADAPTER_WINDOW_ADAPTER_H_
