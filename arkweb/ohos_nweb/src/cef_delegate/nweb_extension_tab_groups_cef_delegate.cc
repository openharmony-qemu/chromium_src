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

#include "nweb_extension_tab_groups_cef_delegate.h"

#include <map>

#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "cef/ohos_cef_ext/libcef/browser/extensions/api/tabGroups/tab_groups_event_router_ext.h"
#include "content/public/browser/browser_context.h"
#include "nweb_common.h"
#include "nweb_extension_utils.h"
#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/extension/nweb_extension_tab_groups_dispatcher.h"
#endif

namespace OHOS::NWeb {

namespace {
static std::map<int32_t, GetTabGroupsCallback> g_get_tab_group_callback_map_;
static std::map<int32_t, MoveTabGroupsCallback> g_move_tab_group_callback_map_;
static std::map<int32_t, QueryTabGroupsCallback> g_query_tab_group_callback_map_;
static std::map<int32_t, UpdateTabGroupsCallback> g_update_tab_group_callback_map_;

content::BrowserContext* GetBrowserContextInUse(const NWebExtensionTabGroup& tabGroup) {
  auto browser_context = GetBrowserContext();
  if (tabGroup.incognito && tabGroup.incognito.value()) {
    browser_context = GetIncognitoContext(browser_context);
  }

  return browser_context;
}
} // namespace

bool NWebExtensionTabGroupsCefDelegate::GetTabGroup(GetTabGroupsCallback callback, int32_t groupId,
    const NWebExtensionFunctionContext& function_context) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int32_t request_id = 0;
  request_id++;
  g_get_tab_group_callback_map_[request_id] = std::move(callback);
  bool result = NWebExtensionTabGroupsDispatcher::GetTabGroup(request_id, groupId, function_context);
  if (!result) {
    g_get_tab_group_callback_map_.erase(request_id);
  }
  return result;
#endif
}

void NWebExtensionTabGroupsCefDelegate::GetTabGroupCallback(
    int32_t request_id, const NWebExtensionTabGroup& tabGroup, std::optional<std::string>& error) {
  if (g_get_tab_group_callback_map_.count(request_id)) {
    std::move(g_get_tab_group_callback_map_[request_id]).Run(tabGroup, error);
    g_get_tab_group_callback_map_.erase(request_id);
  }
}

bool NWebExtensionTabGroupsCefDelegate::MoveTabGroup(
    MoveTabGroupsCallback callback, int32_t groupId, const NWebExtensionTabGroupsMoveProperties& move_properties) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int32_t request_id = 0;
  request_id++;
  g_move_tab_group_callback_map_[request_id] = std::move(callback);
  bool result = NWebExtensionTabGroupsDispatcher::MoveTabGroup(request_id, groupId, move_properties);
  if (!result) {
    g_move_tab_group_callback_map_.erase(request_id);
  }
  return result;
#endif
}

void NWebExtensionTabGroupsCefDelegate::MoveTabGroupCallback(
    int32_t request_id, const NWebExtensionTabGroup& tabGroup, std::optional<std::string>& error) {
  if (g_move_tab_group_callback_map_.count(request_id)) {
    std::move(g_move_tab_group_callback_map_[request_id]).Run(tabGroup, error);
    g_move_tab_group_callback_map_.erase(request_id);
  }
}

bool NWebExtensionTabGroupsCefDelegate::QueryTabGroup(
    QueryTabGroupsCallback callback, const NWebExtensionTabGroupsQueryInfo& query_info) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int32_t request_id = 0;
  request_id++;
  g_query_tab_group_callback_map_[request_id] = std::move(callback);
  bool result = NWebExtensionTabGroupsDispatcher::QueryTabGroup(request_id, query_info);
  if (!result) {
    g_query_tab_group_callback_map_.erase(request_id);
  }
  return result;
#endif
}

void NWebExtensionTabGroupsCefDelegate::QueryTabGroupCallback(
    int32_t request_id, const std::vector<NWebExtensionTabGroup>& tabGroups, std::optional<std::string>& error) {
  if (g_query_tab_group_callback_map_.count(request_id)) {
    std::move(g_query_tab_group_callback_map_[request_id]).Run(tabGroups, error);
    g_query_tab_group_callback_map_.erase(request_id);
  }
}

bool NWebExtensionTabGroupsCefDelegate::UpdateTabGroup(
    UpdateTabGroupsCallback callback, int32_t groupId, const NWebExtensionTabGroupsUpdateProperties& update_properties) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  static int32_t request_id = 0;
  request_id++;
  g_update_tab_group_callback_map_[request_id] = std::move(callback);
  bool result = NWebExtensionTabGroupsDispatcher::UpdateTabGroup(request_id, groupId, update_properties);
  if (!result) {
    g_update_tab_group_callback_map_.erase(request_id);
  }
  return result;
#endif
}

void NWebExtensionTabGroupsCefDelegate::UpdateTabGroupCallback(
    int32_t request_id, const NWebExtensionTabGroup& tabGroup, std::optional<std::string>& error) {
  if (g_update_tab_group_callback_map_.count(request_id)) {
    std::move(g_update_tab_group_callback_map_[request_id]).Run(tabGroup, error);
    g_update_tab_group_callback_map_.erase(request_id);
  }
}

void NWebExtensionTabGroupsCefDelegate::TabGroupCreated(const NWebExtensionTabGroup& tabGroup) {
  auto browser_context = GetBrowserContextInUse(tabGroup);
  if (!browser_context) {
    LOG(ERROR) << "TabGroupCreated get browser context failed.";
    return;
  }
  extensions::CefTabGroupsEventRouter::GetInstance()
      .DispatchTabGroupsCreatedEvent(browser_context, tabGroup);
}

void NWebExtensionTabGroupsCefDelegate::TabGroupMoved(const NWebExtensionTabGroup& tabGroup) {
  auto browser_context = GetBrowserContextInUse(tabGroup);
  if (!browser_context) {
    LOG(ERROR) << "TabGroupMoved get browser context failed.";
    return;
  }
  extensions::CefTabGroupsEventRouter::GetInstance()
      .DispatchTabGroupsMovedEvent(browser_context, tabGroup);
}

void NWebExtensionTabGroupsCefDelegate::TabGroupRemoved(const NWebExtensionTabGroup& tabGroup) {
  auto browser_context = GetBrowserContextInUse(tabGroup);
  if (!browser_context) {
    LOG(ERROR) << "TabGroupRemoved get browser context failed.";
    return;
  }
  extensions::CefTabGroupsEventRouter::GetInstance()
      .DispatchTabGroupsRemovedEvent(browser_context, tabGroup);
}

void NWebExtensionTabGroupsCefDelegate::TabGroupUpdated(const NWebExtensionTabGroup& tabGroup) {
  auto browser_context = GetBrowserContextInUse(tabGroup);
  if (!browser_context) {
    LOG(ERROR) << "TabGroupUpdated get browser context failed.";
    return;
  }
  extensions::CefTabGroupsEventRouter::GetInstance()
      .DispatchTabGroupsUpdatedEvent(browser_context, tabGroup);
}

}  // namespace OHOS::NWeb