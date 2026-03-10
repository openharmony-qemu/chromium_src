/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef NWEB_EXTENSION_MANAGER_CEF_DELEGATE_H_
#define NWEB_EXTENSION_MANAGER_CEF_DELEGATE_H_

#include <functional>
#include <string>
#include <vector>

#include "extensions/browser/extension_prefs.h"
#include "extensions/browser/extension_registry.h"
#include "ohos_nweb/src/capi/nweb_extension_manager_callback.h"

namespace OHOS::NWeb {

using GetInstalledExtensionInfosCallback =
    std::function<void(const std::vector<NWebInstalledExtensionInfo>&)>;

using OnEnableExtensionCallback = std::function<void(std::optional<bool>)>;

using OnSetIncognitoEnabledCallback = std::function<void(std::optional<bool>)>;

class NWebExtensionManagerCefDelegate {
 public:
  static NWebExtensionManagerCefDelegate* GetInstance();

  void EnableWebExtension(const std::string& extension_id,
                          OnEnableExtensionCallback callback);

  void SetIncognitoEnabled(const std::string& extension_id,
                           bool enabled,
                           OnSetIncognitoEnabledCallback callback);

  void GetInstalledExtensionInfos(GetInstalledExtensionInfosCallback callback);

 private:
  static NWebInstalledExtensionInfo BuildExtensionInfoItem(
      const extensions::Extension* extension,
      content::BrowserContext* browser_context,
      const extensions::ExtensionRegistry* extension_registry,
      extensions::ExtensionPrefs* extension_prefs);
};

}  // namespace OHOS::NWeb

#endif  // NWEB_EXTENSION_MANAGER_CEF_DELEGATE_H_