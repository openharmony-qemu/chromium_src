/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "nweb_extension_window_delegate_handler.h"
 
#include "base/logging.h"
#include "content/public/browser/browser_context.h"
#include "cef/libcef/browser/request_context_impl.h"
#include "cef/ohos_cef_ext/libcef/browser/extensions/api/windows/cef_windows_event_router.h"
#include "nweb_extension_utils.h"

namespace OHOS::NWeb {
namespace {
  std::shared_ptr<NWebExtensionWindowsApiCallback> g_extension_windows_api_listener = nullptr;
  static std::map<int, WindowCreatedCallback> g_window_created_map_;
  static std::map<int, WindowUpdatedCallback> g_window_updated_map_;
  static std::map<int, WindowRemovedCallback> g_window_removed_map_;
}
 
// static
NweExtensionWindowDelegateHandler* NweExtensionWindowDelegateHandler::GetInstance() {
  static NweExtensionWindowDelegateHandler instance;
  return &instance;
}
 
// static
void NweExtensionWindowDelegateHandler::RegisterWebExtensionWindowsApiListener(
      std::shared_ptr<NWebExtensionWindowsApiCallback> web_extension_windows_api_callback) {
  LOG(INFO) << "NweExtensionWindowDelegateHandler::RegisterWebExtensionWindowsApiListener";
  g_extension_windows_api_listener = web_extension_windows_api_callback;
}
 
// static
void NweExtensionWindowDelegateHandler::UnRegisterWebExtensionWindowsApiListener() {
  LOG(INFO) << "NweExtensionWindowDelegateHandler::UnRegisterWebExtensionWindowsApiListener";
  g_extension_windows_api_listener = nullptr;
}

NweExtensionWindowDelegateHandler::NweExtensionWindowDelegateHandler() {}
 
void NweExtensionWindowDelegateHandler::WindowCreated(const WebExtensionWindow& window) {
  extensions::CefWindowsEventRouter::GetInstance()->DispatchWindowCreatedEvent(GetBrowserContext(), window);
}
 
void NweExtensionWindowDelegateHandler::WindowRemoved(const WebExtensionWindow& window) {
  extensions::CefWindowsEventRouter::GetInstance()->DispatchWindowRemovedEvent(GetBrowserContext(), window);
}
 
void NweExtensionWindowDelegateHandler::WindowBoundsChanged(const WebExtensionWindow& window) {
  extensions::CefWindowsEventRouter::GetInstance()->DispatchWindowBoundsChangedEvent(GetBrowserContext(), window);
}
 
void NweExtensionWindowDelegateHandler::WindowFocusChanged(const WebExtensionWindow& window) {
  extensions::CefWindowsEventRouter::GetInstance()->DispatchWindowFocusChangedEvent(GetBrowserContext(), window);
}
 
NO_SANITIZE("cfi-icall")
bool NweExtensionWindowDelegateHandler::OnCreateWindow(const WebExtensionWindowCreateData& create_date,
                                                       WindowCreatedCallback callback) {
  static int request_id = 0;
  if (!g_extension_windows_api_listener) {
    LOG(ERROR) << "No web extension api listener";
    return false;
  }
 
  if (!g_extension_windows_api_listener->OnCreateWindow) {
    LOG(ERROR) << "g_extension_windows_api_listener OnCreateWindow is nullptr";
    return false;
  }
 
  request_id++;
  LOG(DEBUG) << "OnCreateWindow";
  g_window_created_map_[request_id] = std::move(callback);
  g_extension_windows_api_listener->OnCreateWindow(request_id, create_date);
  return true;
}
 
NO_SANITIZE("cfi-icall")
bool NweExtensionWindowDelegateHandler::OnUpdateWindow(int windowId,
                                                       const WebExtensionWindowUpdateInfo& update_info,
                                                       WindowUpdatedCallback callback) {
  static int request_id = 0;
  if (!g_extension_windows_api_listener) {
    LOG(ERROR) << "No web extension api listener";
    return false;
  }
 
  if (!g_extension_windows_api_listener->OnUpdateWindow) {
    LOG(ERROR) << "g_extension_windows_api_listener OnUpdateWindow is nullptr";
    return false;
  }
 
  request_id++;
  LOG(DEBUG) << "OnUpdateWindow";
  g_window_updated_map_[request_id] = std::move(callback);
  g_extension_windows_api_listener->OnUpdateWindow(request_id, windowId, update_info);
  return true;
}
 
NO_SANITIZE("cfi-icall")
bool NweExtensionWindowDelegateHandler::OnRemoveWindow(int windowId,
                                                       WindowRemovedCallback callback) {
  static int request_id = 0;
  if (!g_extension_windows_api_listener) {
    LOG(ERROR) << "No web extension api listener";
    return false;
  }
 
  if (!g_extension_windows_api_listener->OnRemoveWindow) {
    LOG(ERROR) << "g_extension_windows_api_listener OnRemoveWindow is nullptr";
    return false;
  }
 
  request_id++;
  LOG(DEBUG) << "OnRemoveWindow";
  g_window_removed_map_[request_id] = std::move(callback);
  g_extension_windows_api_listener->OnRemoveWindow(request_id, windowId);
  return true;
}
 
NO_SANITIZE("cfi-icall")
std::optional<WebExtensionWindow> NweExtensionWindowDelegateHandler::OnGetWindow(
      int windowId,
      const WebExtensionWindowQueryOptions& queryOptions) {
  LOG(DEBUG) << "NweExtensionWindowDelegateHandler::OnGetWindow";
  if (!g_extension_windows_api_listener) {
    LOG(ERROR) << "extension windows api listener is null";
    return std::nullopt;
  }
  return g_extension_windows_api_listener->OnGetWindow(windowId, queryOptions);
}

NO_SANITIZE("cfi-icall")
std::vector<WebExtensionWindow>
NweExtensionWindowDelegateHandler::OnGetAllWindows(
    const WebExtensionWindowQueryOptions& queryOptions) {
  LOG(DEBUG) << "NweExtensionWindowDelegateHandler::OnGetAllWindows";
  std::vector<WebExtensionWindow> windows;
  if (!g_extension_windows_api_listener) {
    LOG(ERROR) << "extension windows api listener is null";
    return windows;
  }
  g_extension_windows_api_listener->OnGetAllWindows(queryOptions, windows);
  return windows;
}

NO_SANITIZE("cfi-icall")
std::optional<WebExtensionWindow> NweExtensionWindowDelegateHandler::OnGetCurrentWindow(
      int currentWindowId,
      const WebExtensionWindowQueryOptions& queryOptions) {
  LOG(DEBUG) << "NweExtensionWindowDelegateHandler::OnGetCurrentWindow";
  if (!g_extension_windows_api_listener) {
    LOG(ERROR) << "extension windows api listener is null";
    return std::nullopt;
  }
  return g_extension_windows_api_listener->OnGetCurrentWindow(currentWindowId, queryOptions);
}
 
NO_SANITIZE("cfi-icall")
std::optional<WebExtensionWindow> NweExtensionWindowDelegateHandler::OnGetLastFocusedWindow(
      const WebExtensionWindowQueryOptions& queryOptions) {
  LOG(DEBUG) << "NweExtensionWindowDelegateHandler::OnGetLastFocusedWindow";
  if (!g_extension_windows_api_listener) {
    LOG(ERROR) << "extension windows api listener is null";
    return std::nullopt;
  }
  return g_extension_windows_api_listener->OnGetLastFocusedWindow(queryOptions);
}
 
void NweExtensionWindowDelegateHandler::WindowCreateCallback(int request_id,
                                                             const std::optional<WebExtensionWindow>& window,
                                                             const std::optional<std::string>& error) {
  if (g_window_created_map_.count(request_id)) {
    std::move(g_window_created_map_[request_id]).Run(window, error);
    g_window_created_map_.erase(request_id);
  }
}
 
void NweExtensionWindowDelegateHandler::WindowUpdateCallback(int request_id,
                                                             const std::optional<WebExtensionWindow>& window,
                                                             const std::optional<std::string>& error) {
  if (g_window_updated_map_.count(request_id)) {
    std::move(g_window_updated_map_[request_id]).Run(window, error);
    g_window_updated_map_.erase(request_id);
  }
}
 
void NweExtensionWindowDelegateHandler::WindowRemoveCallback(int request_id, const std::optional<std::string>& error) {
  if (g_window_removed_map_.count(request_id)) {
    std::move(g_window_removed_map_[request_id]).Run(error);
    g_window_removed_map_.erase(request_id);
  }
}

} // namespace OHOS::NWeb
