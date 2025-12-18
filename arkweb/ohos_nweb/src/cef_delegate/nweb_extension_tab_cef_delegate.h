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

#include "base/functional/callback.h"
#include "ohos_nweb/src/capi/web_extension_tab_items.h"
#include "ohos_nweb/src/capi/web_extension_window_items.h"

using TabRemovedCallback = base::RepeatingCallback<void(std::optional<std::string>&)>;
using TabMovedCallback =
  base::RepeatingCallback<void(const std::vector<NWebExtensionTab>&, std::optional<std::string>&)>;
using TabCreatedCallback = base::RepeatingCallback<void(const NWebExtensionTab&, std::optional<std::string>&)>;
using TabUpdatedCallback = base::RepeatingCallback<void(NWebExtensionTab&, std::optional<std::string>&)>;
using TabDiscardedCallback = base::RepeatingCallback<void(NWebExtensionTab&, std::optional<std::string>&)>;
using TabDuplicatedCallback = base::RepeatingCallback<void(NWebExtensionTab&, std::optional<std::string>&)>;
using TabGroupedCallback = base::RepeatingCallback<void(int, std::optional<std::string>&)>;
using TabUngroupedCallback = base::RepeatingCallback<void(std::optional<std::string>&)>;
using TabHighlightedCallback = base::RepeatingCallback<void(WebExtensionWindow&, std::optional<std::string>&)>;

namespace OHOS::NWeb {

class NWebExtensionTabCefDelegate {
 public:
  static bool HasExtensionListener();

  // chrome.tabs.get
  static std::unique_ptr<NWebExtensionTab> GetTab(
      int tabId,
      std::optional<std::string> contextType = std::nullopt,
      std::optional<bool> includeIncognitoInfo = std::nullopt);

  // chrome.tabs.query
  static std::vector<NWebExtensionTab> QueryTab(
      NWebExtensionTabQueryInfoV2& queryInfo);

  // chrome.tabs.update
  static bool UpdateTab(
      int tab_id,
      NWebExtensionTabUpdatePropertiesV2& update_properties,
      TabUpdatedCallback callback);

  static void TabUpdateCallback(int request_id,
      NWebExtensionTab& tab, std::optional<std::string>& error);

  // chrome.tabs.create
  static bool CreateTab(NWebTabCreateInfoV2& create_info,
                 TabCreatedCallback callback);

  static void TabCreateCallback(int request_id,
      const NWebExtensionTab& tab, std::optional<std::string>& error);

  // chrome.tabs.move
  static bool MoveTab(std::vector<int>& tab_ids,
                      NWebExtensionTabMovePropertiesV2& move_info,
                      TabMovedCallback callback);

  static void TabMoveCallback(
      int requestId,
      const std::vector<NWebExtensionTab>& tabs,
      std::optional<std::string>& error);

  // chrome.tabs.remove
  static bool RemoveTab(NWebExtensionTabRemoveParams& params,
                        TabRemovedCallback callback);

  static void TabRemoveCallback(
      int requestId,
      std::optional<std::string>& error);

  // chrome.tabs.discard
  static bool DiscardTab(int tab_id,
                         NWebExtensionTabDiscardInfo& discard_info,
                         TabDiscardedCallback callback);

  static void TabDiscardCallback(int request_id,
      NWebExtensionTab& tab, std::optional<std::string>& error);

  // chrome.tabs.duplicate
  static bool DuplicateTab(int tab_id,
                           NWebExtensionTabDuplicateInfo& duplicate_info,
                           TabDuplicatedCallback callback);

  static void TabDuplicateCallback(int request_id,
      NWebExtensionTab& tab, std::optional<std::string>& error);

  // chrome.tabs.group
  static bool GroupTab(NWebExtensionTabGroupOptions& options, TabGroupedCallback callback);

  static void TabGroupCallback(int request_id,
      int group_id, std::optional<std::string>& error);

  // chrome.tabs.ungroup
  static bool UngroupTab(NWebExtensionTabUngroupParams& params,
                         TabUngroupedCallback callback);

  static void TabUngroupCallback(int request_id, std::optional<std::string>& error);

  // chrome.tabs.highlight
  static bool HighlightTab(NWebExtensionTabHighlightInfo& info, TabHighlightedCallback callback);

  static void TabHighlightCallback(int request_id,
      WebExtensionWindow& window, std::optional<std::string>& error);

  // like chrome::FindAnyBrowser, get an existing tab.
  static int GetAnyTab(NWebExtensionTabGetAnyTabParams& params);

  // onActivated event
  static void OnTabActivated(std::unique_ptr<NWebExtensionTabActiveInfo> activeInfo);

  // onCreated event
  static void OnTabCreated(std::unique_ptr<NWebExtensionTab> tab);

  // OnTabHighlighted event
  static void OnTabHighlighted(
      const NWebExtensionTabHighlightInfo& highlightInfo);
};

}  // namespace OHOS::NWeb

#endif  // NWEB_EXTENSION_TAB_CEF_DELEGATE_H_
