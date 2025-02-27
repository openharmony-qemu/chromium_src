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
 
#include "cef/include/cef_extension_window_handler.h"
#include "ohos_nweb/src/capi/nweb_extension_api_callback.h"
#include "ohos_nweb/src/capi/web_extension_window_items.h"
 
namespace OHOS::NWeb {
class NweExtensionWindowDelegateHandler : public CefExtensionWindowHandler {
  public:
    static NweExtensionWindowDelegateHandler* GetInstance();
    static void RegisterWebExtensionWindowsApiListener(
      std::shared_ptr<NWebExtensionWindowsApiCallback> web_extension_windows_api_callback);
    static void UnRegisterWebExtensionWindowsApiListener();

    std::vector<WebExtensionWindow> OnGetAllWindows(
      const WebExtensionWindowQueryOptions& queryOptions) override;
 
  private:
    static std::unique_ptr<NweExtensionWindowDelegateHandler> instance;
    static std::mutex mtx;
};
} // namespace OHOS::NWeb
#endif  // NWEB_EXTENSION_WINDOW_HANDLER_DELEGATE_H