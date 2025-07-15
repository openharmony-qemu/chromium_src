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
 
#include "nweb_extension_window_cef_delegate.h"
 
#include "base/logging.h"
#include "content/public/browser/browser_context.h"
#include "cef/libcef/browser/request_context_impl.h"
#include "cef/ohos_cef_ext/libcef/browser/extensions/api/windows/cef_windows_event_router.h"
#include "nweb_extension_utils.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/extension/nweb_extension_windows_dispatcher.h"
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif
 
namespace OHOS::NWeb {
namespace {
  static std::map<int, WindowCreatedCallback> g_window_created_map_;
  static std::map<int, WindowUpdatedCallback> g_window_updated_map_;
  static std::map<int, WindowRemovedCallback> g_window_removed_map_;
}
 
// static
NweExtensionWindowCefDelegate* NweExtensionWindowCefDelegate::GetInstance() {
  static NweExtensionWindowCefDelegate instance;
  return &instance;
}
 
NweExtensionWindowCefDelegate::NweExtensionWindowCefDelegate() {}

void NweExtensionWindowCefDelegate::WindowCreated(const WebExtensionWindow& window) {
  extensions::CefWindowsEventRouter::GetInstance()->DispatchWindowCreatedEvent(GetBrowserContext(), window);
}
 
void NweExtensionWindowCefDelegate::WindowRemoved(const WebExtensionWindow& window) {
  extensions::CefWindowsEventRouter::GetInstance()->DispatchWindowRemovedEvent(GetBrowserContext(), window);
}
 
void NweExtensionWindowCefDelegate::WindowBoundsChanged(const WebExtensionWindow& window) {
  extensions::CefWindowsEventRouter::GetInstance()->DispatchWindowBoundsChangedEvent(GetBrowserContext(), window);
}
 
void NweExtensionWindowCefDelegate::WindowFocusChanged(const WebExtensionWindow& window) {
  extensions::CefWindowsEventRouter::GetInstance()->DispatchWindowFocusChangedEvent(GetBrowserContext(), window);
}

NO_SANITIZE("cfi-icall")
bool NweExtensionWindowCefDelegate::OnCreateWindow(const WebExtensionWindowCreateData& create_date,
                                                       WindowCreatedCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int request_id = 0;
  request_id++;

  g_window_created_map_[request_id] = std::move(callback);
  if (!NWebExtensionWindowsDispathcher::OnCreateWindow(request_id, create_date)) {
    g_window_created_map_.erase(request_id);
    return false;
  }

  return true;
#endif
}

NO_SANITIZE("cfi-icall")
bool NweExtensionWindowCefDelegate::OnUpdateWindow(int windowId,
                                                       const WebExtensionWindowUpdateInfo& update_info,
                                                       WindowUpdatedCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int request_id = 0;
  request_id++;

  g_window_updated_map_[request_id] = std::move(callback);
  if (!NWebExtensionWindowsDispathcher::OnUpdateWindow(request_id, windowId, update_info)) {
    g_window_updated_map_.erase(request_id);
    return false;
  }

  return true;
#endif
}

NO_SANITIZE("cfi-icall")
bool NweExtensionWindowCefDelegate::OnRemoveWindow(int windowId,
                                                       WindowRemovedCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int request_id = 0;
  request_id++;

  g_window_removed_map_[request_id] = std::move(callback);
  if (!NWebExtensionWindowsDispathcher::OnRemoveWindow(request_id, windowId)) {
    g_window_removed_map_.erase(request_id);
    return false;
  }

  return true;
#endif
}

NO_SANITIZE("cfi-icall")
std::optional<WebExtensionWindow> NweExtensionWindowCefDelegate::OnGetWindow(
      int windowId,
      const WebExtensionWindowQueryOptions& queryOptions) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return std::nullopt;
#else
  return NWebExtensionWindowsDispathcher::OnGetWindow(windowId, queryOptions);
#endif
}

NO_SANITIZE("cfi-icall")
std::vector<WebExtensionWindow>
NweExtensionWindowCefDelegate::OnGetAllWindows(
    const WebExtensionWindowQueryOptions& queryOptions) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return std::vector<WebExtensionWindow>();
#else
  return NWebExtensionWindowsDispathcher::OnGetAllWindows(queryOptions);
#endif
}

NO_SANITIZE("cfi-icall")
std::optional<WebExtensionWindow> NweExtensionWindowCefDelegate::OnGetCurrentWindow(
      int currentWindowId,
      const WebExtensionWindowQueryOptions& queryOptions) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return std::nullopt;
#else
  return NWebExtensionWindowsDispathcher::OnGetCurrentWindow(currentWindowId, queryOptions);
#endif
}

NO_SANITIZE("cfi-icall")
std::optional<WebExtensionWindow> NweExtensionWindowCefDelegate::OnGetLastFocusedWindow(
      const WebExtensionWindowQueryOptions& queryOptions) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return std::nullopt;
#else
  return NWebExtensionWindowsDispathcher::OnGetLastFocusedWindow(queryOptions);
#endif
}
 
void NweExtensionWindowCefDelegate::WindowCreateCallback(int request_id,
                                                             const std::optional<WebExtensionWindow>& window,
                                                             const std::optional<std::string>& error) {
  if (g_window_created_map_.count(request_id)) {
    std::move(g_window_created_map_[request_id]).Run(window, error);
    g_window_created_map_.erase(request_id);
  }
}
 
void NweExtensionWindowCefDelegate::WindowUpdateCallback(int request_id,
                                                             const std::optional<WebExtensionWindow>& window,
                                                             const std::optional<std::string>& error) {
  if (g_window_updated_map_.count(request_id)) {
    std::move(g_window_updated_map_[request_id]).Run(window, error);
    g_window_updated_map_.erase(request_id);
  }
}
 
void NweExtensionWindowCefDelegate::WindowRemoveCallback(int request_id, const std::optional<std::string>& error) {
  if (g_window_removed_map_.count(request_id)) {
    std::move(g_window_removed_map_[request_id]).Run(error);
    g_window_removed_map_.erase(request_id);
  }
}

} // namespace OHOS::NWeb
