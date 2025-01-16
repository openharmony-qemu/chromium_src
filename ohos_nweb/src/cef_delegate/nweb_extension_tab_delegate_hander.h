/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef NWEB_EXTENSION_TAB_CEF_DELEGATE_H_
#define NWEB_EXTENSION_TAB_CEF_DELEGATE_H_
#pragma once

#include <memory>
#include <mutex>

#include "cef/include/cef_web_extension_tab_api_handler.h"
#include "ohos_nweb/src/capi/nweb_extension_api_callback.h"
#include "ohos_nweb/src/capi/web_extension_tab_items.h"

namespace OHOS::NWeb {
class NwebExtensionTabDelegateHandler : public CefWebExtensionTabApiHandler {
 public:
  static NwebExtensionTabDelegateHandler* GetInstance();
  static void RegisterWebExtensionTabApiListener(
      std::shared_ptr<NWebExtensionApiCallback>);
  static void UnRegisterWebExtensionTabApiListener();

  // chrome.tabs.get(tabid)
  std::unique_ptr<NWebExtensionTab> GetTab(int tab_id);
  // chrome.tabs.query(queryInfo)
  std::vector<NWebExtensionTab> QueryTab(
      const NWebExtensionTabQueryInfo& queryInfo);

 private:
  static std::unique_ptr<NwebExtensionTabDelegateHandler> instance;
  static std::mutex mtx;
};

}  // namespace OHOS::NWeb

#endif  // CEF_LIBCEF_BROWSER_WEBEXTENSION_TAB_MANAGER_H_