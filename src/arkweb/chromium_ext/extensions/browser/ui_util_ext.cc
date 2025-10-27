/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "extensions/browser/ui_util.h"

#include "extensions/browser/extension_prefs.h"
#include "extensions/common/extension.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_utils.h"

namespace extensions {
namespace ui_util {

bool ShouldDisplayInExtensionSettings(const Extension& extension) {
  if (!ShouldDisplayInExtensionSettings(extension.GetType(),
                                        extension.location())) {
    return false;
  }

  content::BrowserContext* browser_context = OHOS::NWeb::GetBrowserContext();
  if (!browser_context) {
    return true;
  }
  ExtensionPrefs* prefs = ExtensionPrefs::Get(browser_context);
  if (!prefs) {
    return true;
  }

  return !prefs->IsNotDisplayInSettings(extension.id());
}

}  // namespace ui_util
}  // namespace extensions
