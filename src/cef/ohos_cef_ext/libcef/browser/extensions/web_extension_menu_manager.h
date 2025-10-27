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
 
#ifndef CEF_LIBCEF_BROWSER_WEBEXTENSION_MENU_MANAGER_H_
#define CEF_LIBCEF_BROWSER_WEBEXTENSION_MENU_MANAGER_H_
#pragma once

#include "chrome/browser/extensions/menu_manager.h"
#include "ohos_nweb/src/capi/nweb_context_menus_item.h"
#include "ohos_nweb/src/capi/nweb_context_menus_on_clicked_data.h"
#include "ohos_nweb/src/capi/web_extension_tab_items.h"

class CefWebExtensionMenuManager {
 public:
  static void OnClickedExtensionContextMenus(const std::string& extension_id,
                                             ContextMenusOnClickedData& data,
                                             std::optional<NWebExtensionTab>& tab);
  static void OnClickedExtensionContextMenus(const std::string& extension_id,
                                             ContextMenusOnClickedDataV2& data,
                                             std::optional<NWebExtensionTab>& tab);
  static std::vector<NWebContextMenusItem> GetAllExtensionContextMenus(const std::vector<std::string>& extension_ids);
  static void OnContextMenusCreate(const std::string& extension_id, extensions::MenuItem* menu_item);
  static void OnContextMenusUpdate(const std::string& extension_id, extensions::MenuItem* menu_item);
  static void OnContextMenusRemove(const std::string& extension_id, int menu_item_id);
  static void OnContextMenusRemove(const std::string& extension_id, const std::string& menu_item_id);
  static void OnContextMenusRemoveAll(const std::string& extension_id);

 private:
  static void GetFlattenedMenuItemSubtree(
      std::vector<NWebContextMenusItem>& items,
      const std::unique_ptr<extensions::MenuItem>& item);
};

#endif  // CEF_LIBCEF_BROWSER_WEBEXTENSION_MENU_MANAGER_H_
