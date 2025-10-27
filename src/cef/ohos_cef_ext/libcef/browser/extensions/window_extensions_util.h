/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CEF_LIBCEF_BROWSER_EXTENSIONS_WINDOW_EXTENSIONS_UTIL_H_
#define CEF_LIBCEF_BROWSER_EXTENSIONS_WINDOW_EXTENSIONS_UTIL_H_

#include <vector>
#include <unordered_map>
#include "base/values.h"
#include "ohos_nweb/src/capi/web_extension_window_items.h"
#include "content/public/browser/web_contents.h"
#include "chrome/browser/extensions/extension_tab_util.h"

namespace extensions {

base::Value::Dict GetWindowValue(
    const WebExtensionWindow& window,
    const std::vector<ExtensionTabUtil::ScrubTabBehavior>& scrub_tab_behaviors,
    bool populate = true);

base::Value::List GetWindowValueList(
    const std::vector<WebExtensionWindow>& windows,
    const std::vector<std::vector<ExtensionTabUtil::ScrubTabBehavior>>& scrub_tab_behaviors_combined,
    bool populate = true);

int32_t GetCurrentWindowId(content::WebContents* webcontents, int32_t default_window_id);

}  // namespace extensions

#endif // CEF_LIBCEF_BROWSER_EXTENSIONS_WINDOW_EXTENSIONS_UTIL_H_
