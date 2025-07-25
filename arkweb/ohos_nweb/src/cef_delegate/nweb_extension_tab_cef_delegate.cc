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

#include "nweb_extension_tab_cef_delegate.h"

#include <map>
#include "base/logging.h"
#include "chrome/browser/extensions/api/tabs/tabs_windows_api.h"
#include "nweb_extension_utils.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/extension/nweb_extension_tabs_dispatcher.h"
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

namespace OHOS::NWeb {

namespace {
static std::map<int, TabCreatedCallback> g_tab_created_map_;
static std::map<int, TabUpdatedCallback> g_tab_updated_map_;
static std::map<int, TabMovedCallback> g_tab_moved_map_;
static std::map<int, TabRemovedCallback> g_tab_removed_map_;
static std::map<int, TabDiscardedCallback> g_tab_discarded_map_;
static std::map<int, TabDuplicatedCallback> g_tab_duplicated_map_;
static std::map<int, TabGroupedCallback> g_tab_grouped_map_;
static std::map<int, TabUngroupedCallback> g_tab_ungrouped_map_;
static std::map<int, TabHighlightedCallback> g_tab_highlighted_map_;
}

bool NWebExtensionTabCefDelegate::HasExtensionListener() {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  return NWebExtensionTabDispatcher::HasExtensionListener();
#endif
}

bool NWebExtensionTabCefDelegate::HasUpdateTabCallback() {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  return NWebExtensionTabDispatcher::HasUpdateTabCallback();
#endif
}

bool NWebExtensionTabCefDelegate::CreateTab(
    NWebTabCreateInfo& create_info,
    TabCreatedCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int request_id = 0;
  request_id++;
  g_tab_created_map_[request_id] = std::move(callback);
  bool result = NWebExtensionTabDispatcher::CreateTab(request_id, create_info);
  if (!result) {
    g_tab_created_map_.erase(request_id);
  }
  return result;
#endif
}

void NWebExtensionTabCefDelegate::TabCreateCallback(
    int request_id,
    const NWebExtensionTab& tab,
    std::optional<std::string>& error) {
  if (g_tab_created_map_.count(request_id)) {
    std::move(g_tab_created_map_[request_id]).Run(tab, error);
    g_tab_created_map_.erase(request_id);
  }
}

void NWebExtensionTabCefDelegate::UpdateTab(
    int tab_id,
    NWebExtensionTabUpdateProperties& update_properties) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return;
#else
  NWebExtensionTabDispatcher::UpdateTab(-1, tab_id, update_properties);
#endif
}

bool NWebExtensionTabCefDelegate::UpdateTab(
    int tab_id,
    NWebExtensionTabUpdateProperties& update_properties,
    TabUpdatedCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int request_id = 0;
  request_id++;
  g_tab_updated_map_[request_id] = std::move(callback);
  bool result = NWebExtensionTabDispatcher::UpdateTab(request_id, tab_id, update_properties);
  if (!result) {
    g_tab_updated_map_.erase(request_id);
  }
  return result;
#endif
}

void NWebExtensionTabCefDelegate::TabUpdateCallback(
    int request_id,
    NWebExtensionTab& tab,
    std::optional<std::string>& error) {
  if (g_tab_updated_map_.count(request_id)) {
    std::move(g_tab_updated_map_[request_id]).Run(tab, error);
    g_tab_updated_map_.erase(request_id);
  }
}

bool NWebExtensionTabCefDelegate::MoveTab(
    std::vector<int32_t>& tab_ids,
    NWebExtensionTabMoveProperties& move_properties,
    TabMovedCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int request_id = 0;
  request_id++;
  g_tab_moved_map_[request_id] = std::move(callback);
  bool result = NWebExtensionTabDispatcher::MoveTab(request_id, tab_ids, move_properties);
  if (!result) {
    g_tab_moved_map_.erase(request_id);
  }
  return result;
#endif
}

void NWebExtensionTabCefDelegate::TabMoveCallback(
    int request_id,
    const std::vector<NWebExtensionTab>& tabs,
    std::optional<std::string>& error) {
  if (g_tab_moved_map_.count(request_id)) {
    std::move(g_tab_moved_map_[request_id]).Run(tabs, error);
    g_tab_moved_map_.erase(request_id);
  }
}

bool NWebExtensionTabCefDelegate::RemoveTab(
    std::vector<int>& tab_ids, TabRemovedCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int request_id = 0;
  request_id++;
  g_tab_removed_map_[request_id] = std::move(callback);
  bool result = NWebExtensionTabDispatcher::RemoveTab(request_id, tab_ids);
  if (!result) {
    g_tab_removed_map_.erase(request_id);
  }
  return result;
#endif
}

void NWebExtensionTabCefDelegate::TabRemoveCallback(
    int request_id,
    std::optional<std::string>& error) {
  if (g_tab_removed_map_.count(request_id)) {
    std::move(g_tab_removed_map_[request_id]).Run(error);
    g_tab_removed_map_.erase(request_id);
  }
}

bool NWebExtensionTabCefDelegate::DiscardTab(
    int tab_id, TabDiscardedCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int request_id = 0;
  request_id++;
  g_tab_discarded_map_[request_id] = std::move(callback);
  bool result = NWebExtensionTabDispatcher::DiscardTab(request_id, tab_id);
  if (!result) {
    g_tab_discarded_map_.erase(request_id);
  }
  return result;
#endif
}

void NWebExtensionTabCefDelegate::TabDiscardCallback(
    int request_id,
    NWebExtensionTab& tab, std::optional<std::string>& error) {
  if (g_tab_discarded_map_.count(request_id)) {
    std::move(g_tab_discarded_map_[request_id]).Run(tab, error);
    g_tab_discarded_map_.erase(request_id);
  }
}

bool NWebExtensionTabCefDelegate::DuplicateTab(
    int tab_id, TabDuplicatedCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int request_id = 0;
  request_id++;
  g_tab_duplicated_map_[request_id] = std::move(callback);
  bool result = NWebExtensionTabDispatcher::DuplicateTab(request_id, tab_id);
  if (!result) {
    g_tab_duplicated_map_.erase(request_id);
  }
  return result;
#endif
}

void NWebExtensionTabCefDelegate::TabDuplicateCallback(
    int request_id,
    NWebExtensionTab& tab, std::optional<std::string>& error) {
  if (g_tab_duplicated_map_.count(request_id)) {
    std::move(g_tab_duplicated_map_[request_id]).Run(tab, error);
    g_tab_duplicated_map_.erase(request_id);
  }
}

bool NWebExtensionTabCefDelegate::GroupTab(
    NWebExtensionTabGroupOptions& options,
    TabGroupedCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int request_id = 0;
  request_id++;
  g_tab_grouped_map_[request_id] = std::move(callback);
  bool result = NWebExtensionTabDispatcher::GroupTab(request_id, options);
  if (!result) {
    g_tab_grouped_map_.erase(request_id);
  }
  return result;
#endif
}

void NWebExtensionTabCefDelegate::TabGroupCallback(
    int request_id,
    int group_id,
    std::optional<std::string>& error) {
  if (g_tab_grouped_map_.count(request_id)) {
    std::move(g_tab_grouped_map_[request_id]).Run(group_id, error);
    g_tab_grouped_map_.erase(request_id);
  }
}

bool NWebExtensionTabCefDelegate::UngroupTab(
    std::vector<int>& tabs,
    TabUngroupedCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int request_id = 0;
  request_id++;
  g_tab_ungrouped_map_[request_id] = std::move(callback);
  bool result = NWebExtensionTabDispatcher::UngroupTab(request_id, tabs);
  if (!result) {
    g_tab_ungrouped_map_.erase(request_id);
  }
  return result;
#endif
}

void NWebExtensionTabCefDelegate::TabUngroupCallback(
    int request_id,
    std::optional<std::string>& error) {
  if (g_tab_ungrouped_map_.count(request_id)) {
    std::move(g_tab_ungrouped_map_[request_id]).Run(error);
    g_tab_ungrouped_map_.erase(request_id);
  }
}

bool NWebExtensionTabCefDelegate::HighlightTab(
    NWebExtensionTabHighlightInfo& info,
    TabHighlightedCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int request_id = 0;
  request_id++;
  g_tab_highlighted_map_[request_id] = std::move(callback);
  bool result = NWebExtensionTabDispatcher::HighlightTab(request_id, info);
  if (!result) {
    g_tab_highlighted_map_.erase(request_id);
  }
  return result;
#endif
}

void NWebExtensionTabCefDelegate::TabHighlightCallback(
    int request_id,
    WebExtensionWindow& window,
    std::optional<std::string>& error) {
  if (g_tab_highlighted_map_.count(request_id)) {
    std::move(g_tab_highlighted_map_[request_id]).Run(window, error);
    g_tab_highlighted_map_.erase(request_id);
  }
}

std::unique_ptr<NWebExtensionTab> NWebExtensionTabCefDelegate::GetTab(
    int tab_id) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return nullptr;
#else
  return NWebExtensionTabDispatcher::GetTab(tab_id);
#endif
}

std::vector<NWebExtensionTab> NWebExtensionTabCefDelegate::QueryTab(
    NWebExtensionTabQueryInfo& queryInfo) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return std::vector<NWebExtensionTab>();
#else
  return NWebExtensionTabDispatcher::QueryTab(queryInfo);
#endif
}

int NWebExtensionTabCefDelegate::GetAnyTab(int windowId) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return -1;
#else
  return NWebExtensionTabDispatcher::GetAnyTab(windowId);
#endif
}

void NWebExtensionTabCefDelegate::OnTabCreated(std::unique_ptr<NWebExtensionTab> tab) {
  if (!tab) {
    LOG(ERROR) << "OnTabCreated tab is null";
    return;
  }

  auto browserContext = GetBrowserContext();
  if (!browserContext) {
    return;
  }

  int tabId = tab->id ? tab->id.value() : -1;
  extensions::TabsWindowsAPI::Get(browserContext)
      ->TabCreated(tabId, browserContext, std::move(tab));
}

}  // namespace OHOS::NWeb
