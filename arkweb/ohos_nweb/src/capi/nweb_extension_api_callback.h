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

#ifndef OHOS_NWEB_SRC_NWEB_EXTENSION_API_CALLBACK_H_
#define OHOS_NWEB_SRC_NWEB_EXTENSION_API_CALLBACK_H_

#include <map>
#include <stddef.h>
#include <string>
#include "ohos_nweb/src/capi/web_extension_tab_items.h"
#include "ohos_nweb/src/capi/nweb_extension_action_icon.h"
#include "ohos_nweb/src/capi/web_extension_window_items.h"
#include "nweb_extension_api_struct_info.h"
#include "web_extension_tab_items.h"

struct NWebExtensionApiCallback {
  size_t struct_size = sizeof(NWebExtensionApiCallback);
  int nweb_id{0};
  void (*OnUpdateTabUrl)(int tab_id, const char* url);
  void (*OnUpdateTab)(
      int tab_id,
      const NWebExtensionTabUpdateProperties* update_properties);
  void (*OnCreateTab)(const NWebTabCreateInfo& create_info, int request_id);
  void (*NotifyCaptureVisibleTab)(std::optional<int32_t> windowId,
                                  NWebExtensionTabImageDetails* options);
  void (*NotifyDiscard)(int32_t tabId);
  void (*NotifyDuplicate)(int32_t tabId);
  std::unique_ptr<NWebExtensionTab> (*NotifyGet)(int32_t tabId);
  void (*NotifyGetZoom)(int32_t tabId);
  void (*NotifyGetZoomSettings)(int32_t tabId);
  void (*NotifyGroup)(NWebExtensionTabGroupOptions& options);
  void (*NotifyHighlight)(NWebExtensionTabHighlightInfo& highlightInfo);
  void (*NotifyMove)(std::vector<int32_t>& tabIds,
                     NWebExtensionTabMoveProperties& moveProperties);
  void (*NotifyQuery)(const NWebExtensionTabQueryInfo& queryInfo, std::vector<NWebExtensionTab>& tabs);
  void (*NotifyReload)(int32_t tabId,
                       NWebExtensionTabReloadProperties* reloadProperties);
  void (*NotifyRemove)(std::vector<int32_t>& tabIds);
  void (*NotifySetZoom)(int32_t tabId, int32_t zoomFactor);
  void (*NotifySetZoomSettings)(int32_t tabId,
                                NWebExtensionTabZoomSettings& zoomSettings);
  void (*NotifyUngroup)(std::vector<int32_t>& tabIds);
};
 
struct NWebExtensionWindowsApiCallback {
  size_t struct_size = sizeof(NWebExtensionWindowsApiCallback);
  int nweb_id{0};
  void (*OnGetAllWindows)(const WebExtensionWindowQueryOptions& queryOptions,
                           std::vector<WebExtensionWindow>& result);
};
 
struct NWebExtensionSidePanelApiCallback {
  size_t struct_size = sizeof(NWebExtensionSidePanelApiCallback);
  void (*OnSidePanelOpen)(const char* extension_id, int tab_id, int window_id);
  void (*OnSidePanelSetOptions)(std::string extension_id,
                                std::optional<bool> enabled,
                                std::optional<int> tab_id,
                                std::optional<std::string> path);
  void (*OnSidePanelSetPanelBehavior)(const char* extension_id,
                                      int open_panel_on_action_click);
};
 
struct NWebExtensionActionApiCallback {
  size_t struct_size = sizeof(NWebExtensionActionApiCallback);
  void (*OnSetIcon)(const char* extension_id,
                    OHOS::NWeb::NWebExtensionActionIcon* icon,
                    int tab_id);
  void (*OnDisable)(const std::string& extensionId, std::optional<int>& tabId);
  void (*OnEnable)(const std::string& extensionId, std::optional<int>& tabId);
};

#endif  // OHOS_NWEB_SRC_NWEB_EXTENSION_API_CALLBACK_H_
