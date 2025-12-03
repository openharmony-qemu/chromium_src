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
#include "base/logging.h"
#include "url/gurl.h"
#include "ohos_cef_ext/libcef/browser/extensions/tab_extensions_util.h"

namespace extensions {

base::Value::Dict GetMutedInfoValue(const NWebExtensionTabMutedInfo& mutedInfo) {
  base::Value::Dict dict;
  if (mutedInfo.extensionId) {
    dict.Set("extensionId", *(mutedInfo.extensionId));
  }
  dict.Set("muted", mutedInfo.muted);
  if (mutedInfo.reason) {
    dict.Set("reason", *(mutedInfo.reason));
  }
  return dict;
}

void ScrubTabValue(NWebExtensionTab& tab, const ExtensionTabUtil::ScrubTabBehavior& scrub_tab_behavior) {
  switch (scrub_tab_behavior.committed_info) {
    case ExtensionTabUtil::kScrubTabFully:
      tab.url = std::nullopt;
      tab.title = std::nullopt;
      tab.favIconUrl = std::nullopt;
      break;
    case ExtensionTabUtil::kScrubTabUrlToOrigin:
      if (tab.url) {
        tab.url = GURL(*tab.url).DeprecatedGetOriginAsURL().spec();
      }
      break;
    case ExtensionTabUtil::kDontScrubTab:
      break;
  }
  if (tab.pendingUrl) {
    switch (scrub_tab_behavior.pending_info) {
      case ExtensionTabUtil::kScrubTabFully:
        tab.pendingUrl = std::nullopt;
        break;
      case ExtensionTabUtil::kScrubTabUrlToOrigin:
        tab.pendingUrl = GURL(*tab.pendingUrl).DeprecatedGetOriginAsURL().spec();
        break;
      case ExtensionTabUtil::kDontScrubTab:
        break;
    }
  }
}
 
base::Value::Dict GetTabValue(NWebExtensionTab& tab, const ExtensionTabUtil::ScrubTabBehavior& scrub_tab_behavior) {
  ScrubTabValue(tab, scrub_tab_behavior);
  base::Value::Dict dict;
  if (tab.id) {
    dict.Set("id", *tab.id);
  }
  dict.Set("index", tab.index);
  dict.Set("groupId", tab.groupId);
  dict.Set("windowId", tab.windowId);
  if (tab.openerTabId) {
    dict.Set("openerTabId", *tab.openerTabId);
  }
  // "lastAccessed" not supported yet in M114.
  dict.Set("highlighted", tab.highlighted);
  dict.Set("active", tab.active);
  dict.Set("pinned", tab.pinned);
  if (tab.audible) {
    dict.Set("audible", *tab.audible);
  }
  dict.Set("discarded", tab.discarded);
  dict.Set("autoDiscardable", tab.autoDiscardable);
  if (tab.mutedInfo) {
    dict.Set("mutedInfo", GetMutedInfoValue(*tab.mutedInfo));
  }
  if (tab.url) {
    dict.Set("url", *tab.url);
  }
  if (tab.pendingUrl) {
    dict.Set("pendingUrl", *tab.pendingUrl);
  }
  if (tab.title) {
    dict.Set("title", *tab.title);
  }
  if (tab.favIconUrl) {
    dict.Set("favIconUrl", *tab.favIconUrl);
  }
  if (tab.status) {
    dict.Set("status", *tab.status);
  }
  dict.Set("incognito", tab.incognito);
  if (tab.width) {
    dict.Set("width", *tab.width);
  }
  if (tab.height) {
    dict.Set("height", *tab.height);
  }
  if (tab.sessionId) {
    dict.Set("sessionId", *tab.sessionId);
  }
  return dict;
}

std::string GetZoomSettingsModeStr(NWebExtensionTabZoomSettingsMode mode) {
  switch (mode) {
    case NWebExtensionTabZoomSettingsMode::AUTOMATIC:
      return "automatic";
    case NWebExtensionTabZoomSettingsMode::MANUAL:
      return "manual";
    case NWebExtensionTabZoomSettingsMode::DISABLE:
      return "disabled";
  }
  return {};
}

std::string GetZoomSettingsScopeStr(NWebExtensionTabZoomSettingsScope scope) {
  switch (scope) {
    case NWebExtensionTabZoomSettingsScope::PER_ORIGIN:
      return "per-origin";
    case NWebExtensionTabZoomSettingsScope::PER_TAB:
      return "per-tab";
  }
  return {};
}

base::Value::Dict GetTabZoomSettingsValue(const NWebExtensionTabZoomSettings& zoomSettings) {
  base::Value::Dict dict;
  if (zoomSettings.defaultZoomFactor) {
    dict.Set("defaultZoomFactor", *zoomSettings.defaultZoomFactor);
  }
  if (zoomSettings.mode) {
    dict.Set("mode", GetZoomSettingsModeStr(*zoomSettings.mode));
  }
  if (zoomSettings.scope) {
    dict.Set("scope", GetZoomSettingsScopeStr(*zoomSettings.scope));
  }
  return dict;
}

base::Value::Dict GetTabZoomChangeValue(const NWebExtensionTabZoomChangeInfo& tabZoomChangeInfo) {
  base::Value::Dict dict;
  dict.Set("newZoomFactor", tabZoomChangeInfo.newZoomFactor);
  dict.Set("oldZoomFactor", tabZoomChangeInfo.oldZoomFactor);
  dict.Set("tabId", tabZoomChangeInfo.tabId);
  dict.Set("zoomSettings", GetTabZoomSettingsValue(tabZoomChangeInfo.zoomSettings));
  return dict;
}

base::Value::List GetTabValueList(const std::vector<NWebExtensionTab>& tabs,
                                  const std::vector<ExtensionTabUtil::ScrubTabBehavior>& scrub_tab_behaviors) {
  base::Value::List tab_list;
  size_t i = 0;
  for (NWebExtensionTab tab : tabs) {
    if (i == scrub_tab_behaviors.size()) break;
    LOG(DEBUG) << "GetTabValueList tab id: " << tab.id.value_or(-1);
    tab_list.Append(GetTabValue(tab, scrub_tab_behaviors[i++]));
  }
  return tab_list;
}

// static
std::unordered_map<int, int>
    CefExtensionWindowIdManager::popup_window_ids_ = {};
// static
std::unordered_map<int, int>
    CefExtensionWindowIdManager::side_panel_window_ids_ = {};

// static
void CefExtensionWindowIdManager::ErasePopupWindowId(int nwebId) {
  popup_window_ids_.erase(nwebId);
}

// static
void CefExtensionWindowIdManager::EraseSidePanelWindowId(int nwebId) {
  side_panel_window_ids_.erase(nwebId);
}

// static
void CefExtensionWindowIdManager::SetPopupWindowId(int nwebId, int windowId) {
  popup_window_ids_[nwebId] = windowId;
}

// static
void CefExtensionWindowIdManager::SetSidePanelWindowId(int nwebId,
                                                        int windowId) {
  side_panel_window_ids_[nwebId] = windowId;
}

// static
int CefExtensionWindowIdManager::GetWindowId(int nwebId) {
  auto popup_result = popup_window_ids_.find(nwebId);
  if (popup_result != popup_window_ids_.end()) {
    LOG(INFO) << "GetWindowId windowId found in popup nweb";
    return popup_result->second;
  }

  auto side_panel_result = side_panel_window_ids_.find(nwebId);
  if (side_panel_result != side_panel_window_ids_.end()) {
    LOG(INFO) << "GetWindowId windowId found in side panel nweb";
    return side_panel_result->second;
  }

  LOG(ERROR) << "GetWindowId windowId not found";
  return WINDOW_ID_NONE;
}

}  // namespace extensions
