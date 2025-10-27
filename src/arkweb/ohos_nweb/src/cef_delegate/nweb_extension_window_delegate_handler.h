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
 
#ifndef NWEB_EXTENSION_WINDOW_HANDLER_DELEGATE_H
#define NWEB_EXTENSION_WINDOW_HANDLER_DELEGATE_H
 
#include <string>
#include <mutex>
#include <memory>
 
#include "base/functional/callback.h"
#include "chrome/browser/extensions/api/tabs/tabs_windows_api.h"
#include "ohos_nweb/src/capi/nweb_extension_api_callback.h"
#include "ohos_nweb/src/capi/web_extension_window_items.h"
 
namespace OHOS::NWeb {
using WindowCreatedCallback = base::RepeatingCallback<void(const std::optional<WebExtensionWindow>&,
                                                           const std::optional<std::string>&)>;
using WindowUpdatedCallback = base::RepeatingCallback<void(const std::optional<WebExtensionWindow>&,
                                                           const std::optional<std::string>&)>;
using WindowRemovedCallback = base::RepeatingCallback<void(const std::optional<std::string>&)>;
 
class NweExtensionWindowDelegateHandler {
  public:
    static NweExtensionWindowDelegateHandler* GetInstance();
    static void RegisterWebExtensionWindowsApiListener(
      std::shared_ptr<NWebExtensionWindowsApiCallback> web_extension_windows_api_callback);
    static void UnRegisterWebExtensionWindowsApiListener();
  
    void WindowCreated(const WebExtensionWindow& window);
    void WindowRemoved(const WebExtensionWindow& window);
    void WindowBoundsChanged(const WebExtensionWindow& window);
    void WindowFocusChanged(const WebExtensionWindow& window);
 
    bool OnCreateWindow(const WebExtensionWindowCreateData& create_date, WindowCreatedCallback callback);
    bool OnUpdateWindow(int windowId,
                        const WebExtensionWindowUpdateInfo& update_info,
                        WindowUpdatedCallback callback);
    bool OnRemoveWindow(int windowId, WindowRemovedCallback callback);
 
    std::optional<WebExtensionWindow> OnGetWindow(
      int windowId,
      const WebExtensionWindowQueryOptions& queryOptions);
    std::optional<WebExtensionWindow> OnGetCurrentWindow(
      int currentWindowId,
      const WebExtensionWindowQueryOptions& queryOptions);
    std::optional<WebExtensionWindow> OnGetLastFocusedWindow(
      const WebExtensionWindowQueryOptions& queryOptions);
 
    void WindowCreateCallback(int request_id,
                              const std::optional<WebExtensionWindow>& window,
                              const std::optional<std::string>& error);
    void WindowUpdateCallback(int request_id,
                              const std::optional<WebExtensionWindow>& window,
                              const std::optional<std::string>& error);
    void WindowRemoveCallback(int request_id, const std::optional<std::string>& error);

    std::vector<WebExtensionWindow> OnGetAllWindows(
      const WebExtensionWindowQueryOptions& queryOptions);
 
  private:
    NweExtensionWindowDelegateHandler();
};
} // namespace OHOS::NWeb
#endif  // NWEB_EXTENSION_WINDOW_HANDLER_DELEGATE_H