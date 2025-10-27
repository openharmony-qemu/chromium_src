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

#ifndef CEF_LIBCEF_BROWSER_EXTENSIONS_TAB_EXTENSIONS_UTIL_H_
#define CEF_LIBCEF_BROWSER_EXTENSIONS_TAB_EXTENSIONS_UTIL_H_

#include <vector>
#include <unordered_map>

#include "base/values.h"
#include "ohos_nweb/src/capi/web_extension_tab_items.h"
#include "chrome/browser/extensions/extension_tab_util.h"

namespace extensions {

base::Value::Dict GetMutedInfoValue(const NWebExtensionTabMutedInfo& mutedInfo);

base::Value::Dict GetTabValue(NWebExtensionTab& tab,
                              const ExtensionTabUtil::ScrubTabBehavior& scrub_tab_behavior);

base::Value::Dict GetTabZoomSettingsValue(const NWebExtensionTabZoomSettings& zoomSettings);

base::Value::Dict GetTabZoomChangeValue(const NWebExtensionTabZoomChangeInfo& tabZoomChangeInfo);

base::Value::List GetTabValueList(const std::vector<NWebExtensionTab>& tabs,
                                  const std::vector<ExtensionTabUtil::ScrubTabBehavior>& scrub_tab_behaviors);

class CefExtensionWindowIdManager {
 public:
  static constexpr int WINDOW_ID_NONE = -1;

  static void ErasePopupWindowId(int nwebId);
  static void EraseSidePanelWindowId(int nwebId);
  static void SetPopupWindowId(int nwebId, int windowId);
  static void SetSidePanelWindowId(int nwebId, int windowId);
  static int GetWindowId(int nwebId);

 private:
  static std::unordered_map<int, int> popup_window_ids_;
  static std::unordered_map<int, int> side_panel_window_ids_;
};

}  // namespace extensions

#endif  // CEF_LIBCEF_BROWSER_EXTENSIONS_TAB_EXTENSIONS_UTIL_H_
