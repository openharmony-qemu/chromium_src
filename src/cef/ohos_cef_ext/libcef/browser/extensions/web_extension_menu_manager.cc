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

#include "web_extension_menu_manager.h"

#include "arkweb/build/features/features.h"
#include "base/values.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "chrome/browser/extensions/menu_manager.h"
#include "chrome/browser/extensions/permissions/active_tab_permission_granter.h"
#include "chrome/browser/extensions/tab_helper.h"
#include "chrome/common/extensions/api/context_menus.h"
#include "extensions/browser/event_router.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/mojom/context_type.mojom.h"
#include "extensions/common/mojom/context_type.mojom-forward.h"
#include "libcef/browser/extensions/tab_extensions_util.h"
#include "libcef/browser/request_context_impl.h"
#include "ohos_nweb/src/nweb_common.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/extension/nweb_extension_context_menus_dispatcher.h"
#endif

std::string GetTypeStr(extensions::MenuItem::Type type) {
  switch (type) {
    case extensions::MenuItem::Type::NORMAL:
      return "normal";
    case extensions::MenuItem::Type::CHECKBOX:
      return "checkbox";
    case extensions::MenuItem::Type::RADIO:
      return "radio";
    case extensions::MenuItem::Type::SEPARATOR:
      return "separator";
    default:
      return nullptr;
  };
}
 
std::string GetContextStr(extensions::MenuItem::Context context) {
  switch (context) {
    case extensions::MenuItem::Context::ALL:
      return "all";
    case extensions::MenuItem::Context::PAGE:
      return "page";
    case extensions::MenuItem::Context::SELECTION:
      return "selection";
    case extensions::MenuItem::Context::LINK:
      return "link";
    case extensions::MenuItem::Context::EDITABLE:
      return "editable";
    case extensions::MenuItem::Context::IMAGE:
      return "image";
    case extensions::MenuItem::Context::VIDEO:
      return "video";
    case extensions::MenuItem::Context::AUDIO:
      return "audio";
    case extensions::MenuItem::Context::FRAME:
      return "frame";
    case extensions::MenuItem::Context::LAUNCHER:
      return "launcher";
    case extensions::MenuItem::Context::BROWSER_ACTION:
      return "browser_action";
    case extensions::MenuItem::Context::PAGE_ACTION:
      return "page_action";
    case extensions::MenuItem::Context::ACTION:
      return "action";
    default:
      return nullptr;
  };
}
 
std::vector<std::string> ContextListToStrVector(
    const extensions::MenuItem::ContextList& contextList) {
  std::vector<std::string> result;
  for (int contextInt = extensions::MenuItem::Context::ALL;
        contextInt <= extensions::MenuItem::Context::ACTION;
        contextInt <<= 1) {
    if (contextList.Contains(static_cast<extensions::MenuItem::Context>(contextInt))) {
      result.push_back(GetContextStr(static_cast<extensions::MenuItem::Context>(contextInt)));
    }
  }
  return result;
}
 
NO_SANITIZE("cfi-icall")
content::BrowserContext* GetBrowserContext() {
  CefRefPtr<CefRequestContext> request_context = CefRequestContext::GetGlobalContext();
  if (!request_context) {
    LOG(ERROR) << "request context is null";
    return nullptr;
  }
 
  CefRequestContextImpl* request_context_impl =
    static_cast<CefRequestContextImpl*>(request_context.get());
  CefBrowserContext* cef_browser_context = request_context_impl->GetBrowserContext();
  if (!cef_browser_context) {
    LOG(ERROR) << "cef browser context is null";
    return nullptr;
  }
  content::BrowserContext* browser_context = cef_browser_context->AsBrowserContext();
  return browser_context;
}
 
NWebContextMenusItem GetNWebContextMenusItem(extensions::MenuItem* menu_item) {
  NWebContextMenusItem item;
  item.checked = menu_item->checked();
  item.contexts = ContextListToStrVector(menu_item->contexts());
  item.documentUrlPatterns = menu_item->document_url_patterns().ToStringVector();
  item.enabled = menu_item->enabled();
  item.id = menu_item->id().string_uid;
  if (menu_item->parent_id()) {
    item.parentId = menu_item->parent_id()->string_uid;
  }
  item.targetUrlPatterns = menu_item->target_url_patterns().ToStringVector();
  item.title = menu_item->title();
  item.type = GetTypeStr(menu_item->type());
  item.visible = menu_item->visible();
  item.extensionId = menu_item->extension_id();
  return item;
}

#if BUILDFLAG(ARKWEB_NWEB_EX)
NWebContextMenusItemV2 GetNWebContextMenusItemV2(extensions::MenuItem* menu_item) {
  NWebContextMenusItemV2 item;
  item.item = GetNWebContextMenusItem(menu_item);
  item.isOffTheRecord = menu_item->incognito();
  item.intId = menu_item->id().uid;
  return item;
}
#endif

void SetContextMenusEventProperties(base::Value::Dict& properties,
                                    ContextMenusOnClickedData& data) {
  properties.Set("menuItemId", data.menuItemId);
  properties.Set("parentMenuItemId", data.parentMenuItemId);
  properties.Set("mediaType", data.mediaType);
  properties.Set("linkUrl", data.linkUrl);
  properties.Set("srcUrl", data.srcUrl);
  properties.Set("pageUrl", data.pageUrl);
  properties.Set("frameUrl", data.frameUrl);
  if (data.selectionText.length() > 0) {
    properties.Set("selectionText", data.selectionText);
  }
  properties.Set("editable", data.editable);
  properties.Set("wasChecked", data.wasChecked);
  properties.Set("checked", data.checked);
  properties.Set("frameId", data.frameId);
}

void SetContextMenusEventProperties(base::Value::Dict& properties,
                                    ContextMenusOnClickedDataV2& data) {
  properties.Set("editable", data.editable);
  if (data.menuItemIdStr.empty()) {
    properties.Set("menuItemId", data.menuItemId);
  } else {
    properties.Set("menuItemId", data.menuItemIdStr);
  }
  if (data.frameId) {
    properties.Set("frameId", *data.frameId);
  }
  if (data.checked) {
    properties.Set("checked", *data.checked);
  }
  if (data.wasChecked) {
    properties.Set("wasChecked", *data.wasChecked);
  }
  if (data.srcUrl) {
    properties.Set("srcUrl", *data.srcUrl);
  }
  if (data.linkUrl) {
    properties.Set("linkUrl", *data.linkUrl);
  }
  if (data.pageUrl) {
    properties.Set("pageUrl", *data.pageUrl);
  }
  if (data.frameUrl) {
    properties.Set("frameUrl", *data.frameUrl);
  }
  if (data.mediaType) {
    properties.Set("mediaType", *data.mediaType);
  }
  if (data.selectionText) {
    properties.Set("selectionText", *data.selectionText);
  }
  if (data.parentMenuItemId) {
    properties.Set("parentMenuItemId", *data.parentMenuItemId);
  } else if (data.parentMenuItemIdStr) {
    properties.Set("parentMenuItemId", *data.parentMenuItemIdStr);
  }
}

void ExtensionContextMenusInvokeActiveTab(
    content::BrowserContext* context,
    int tab_id,
    std::string extension_id) {
  extensions::ExtensionRegistry* registry = extensions::ExtensionRegistry::Get(context);
  if (!registry) {
    LOG(ERROR) << "ExtensionActionInvokeActiveTab registry is null";
    return;
  }
 
  const extensions::Extension* extension =
      registry->enabled_extensions().GetByID(extension_id);
  if (!extension) {
    LOG(ERROR) << "ExtensionActionInvokeActiveTab extension is null";
    return;
  }
 
  content::WebContents* out_contents = nullptr;
  if (extensions::ExtensionTabUtil::GetTabById(tab_id, context, true, &out_contents)) {
    extensions::TabHelper::FromWebContents(out_contents)
        ->active_tab_permission_granter()
        ->GrantIfRequested(extension);
  }
}

base::Value::List BuildContextMenuEventArgs(
    ContextMenusOnClickedDataV2& data,
    std::optional<NWebExtensionTab>& tab,
    content::BrowserContext* browser_context,
    const std::string& extension_id) {
  base::Value::Dict properties;
  SetContextMenusEventProperties(properties, data);

  base::Value::List args;
  args.Append(std::move(properties));

  if (tab) {
    extensions::ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior = {
        extensions::ExtensionTabUtil::kDontScrubTab,
        extensions::ExtensionTabUtil::kDontScrubTab};
    args.Append(extensions::GetTabValue(*tab, scrub_tab_behavior));
    if (tab->id.has_value()) {
      ExtensionContextMenusInvokeActiveTab(browser_context, tab->id.value(),
                                           extension_id);
    }
  }
  return args;
}

NO_SANITIZE("cfi-icall")
void CefWebExtensionMenuManager::OnClickedExtensionContextMenus(const std::string& extension_id,
                                                    ContextMenusOnClickedData& data,
                                                    std::optional<NWebExtensionTab>& tab) {
  content::BrowserContext* browser_context = GetBrowserContext();
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return;
  }
  extensions::MenuManager* menu_manager = extensions::MenuManager::Get(browser_context);
  if (!menu_manager) {
    LOG(ERROR) << "menu_manager is null";
    return;
  }
  extensions::EventRouter* event_router = extensions::EventRouter::Get(browser_context);
  if (!event_router) {
    LOG(ERROR) << "event_router is null";
    return;
  }
  extensions::MenuItem::Id id(browser_context->IsOffTheRecord(),
                              extensions::MenuItem::ExtensionKey(extension_id));
  id.string_uid = data.menuItemId;
  extensions::MenuItem* item = menu_manager->GetItemById(id);
  if (!item) {
    LOG(ERROR) << "item is null";
    return;
  }
  item->SetChecked(data.checked);
  base::Value::Dict properties;
  SetContextMenusEventProperties(properties, data);
  base::Value::List args;
  args.Append(std::move(properties));
  if (tab) {
    extensions::ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior = {
        extensions::ExtensionTabUtil::kDontScrubTab, extensions::ExtensionTabUtil::kDontScrubTab};
    args.Append(extensions::GetTabValue(*tab, scrub_tab_behavior));
    if (tab->id.has_value()) {
      ExtensionContextMenusInvokeActiveTab(browser_context, tab->id.value(), extension_id);
    }
  }
  {
    auto event = std::make_unique<extensions::Event>(extensions::events::CONTEXT_MENUS,
        "contextMenus", args.Clone(), browser_context);
    event->user_gesture = extensions::EventRouter::USER_GESTURE_ENABLED;
    event_router->DispatchEventToExtension(extension_id, std::move(event));
  }
  {
    auto event = std::make_unique<extensions::Event>(extensions::events::CONTEXT_MENUS_ON_CLICKED,
        extensions::api::context_menus::OnClicked::kEventName, std::move(args), browser_context);
    event->user_gesture = extensions::EventRouter::USER_GESTURE_ENABLED;
    event_router->DispatchEventToExtension(extension_id, std::move(event));
  }
}

NO_SANITIZE("cfi-icall")
void CefWebExtensionMenuManager::OnClickedExtensionContextMenus(const std::string& extension_id,
                                                    ContextMenusOnClickedDataV2& data,
                                                    std::optional<NWebExtensionTab>& tab) {
  content::BrowserContext* browser_context = GetBrowserContext();
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return;
  }
  extensions::MenuManager* menu_manager = extensions::MenuManager::Get(browser_context);
  if (!menu_manager) {
    LOG(ERROR) << "menu_manager is null";
    return;
  }
  extensions::EventRouter* event_router = extensions::EventRouter::Get(browser_context);
  if (!event_router) {
    LOG(ERROR) << "event_router is null";
    return;
  }
  extensions::MenuItem::Id id(browser_context->IsOffTheRecord(),
                              extensions::MenuItem::ExtensionKey(extension_id));
  id.uid = data.menuItemId;
  id.string_uid = data.menuItemIdStr;
  extensions::MenuItem* item = menu_manager->GetItemById(id);
  if (!item) {
    LOG(ERROR) << "item is null,id is " << id.uid << " - " << id.string_uid;
    return;
  }
  if (data.checked) {
    item->SetChecked(*data.checked);
  }

  base::Value::List args =
      BuildContextMenuEventArgs(data, tab, browser_context, extension_id);

  {
    auto event = std::make_unique<extensions::Event>(
        extensions::events::CONTEXT_MENUS,
        "contextMenus",
        args.Clone(),
        browser_context);
    event->user_gesture = extensions::EventRouter::USER_GESTURE_ENABLED;
    event_router->DispatchEventToExtension(extension_id, std::move(event));
  }
  {
    auto event = std::make_unique<extensions::Event>(
        extensions::events::CONTEXT_MENUS_ON_CLICKED,
        extensions::api::context_menus::OnClicked::kEventName,
        std::move(args),
        browser_context);
    event->user_gesture = extensions::EventRouter::USER_GESTURE_ENABLED;
    event_router->DispatchEventToExtension(extension_id, std::move(event));
  }
}

NO_SANITIZE("cfi-icall")
std::vector<NWebContextMenusItem> CefWebExtensionMenuManager::GetAllExtensionContextMenus(
  const std::vector<std::string>& extension_ids) {
  std::vector<NWebContextMenusItem> items;
  content::BrowserContext* browser_context = GetBrowserContext();
  if (!browser_context) {
    LOG(ERROR) << "browser_context is null";
    return items;
  }
  extensions::MenuManager* menu_manager = extensions::MenuManager::Get(browser_context);
  if (!menu_manager) {
    LOG(ERROR) << "menu_manager is null";
    return items;
  }
  for (const auto& id : menu_manager->ExtensionIds()) {
    if (extension_ids.size() == 0 ||
      std::find(extension_ids.begin(), extension_ids.end(), id.extension_id) != extension_ids.end()) {
      const extensions::MenuItem::OwnedList* top_items = menu_manager->MenuItems(id);
      for (const std::unique_ptr<extensions::MenuItem>& item : *top_items) {
        GetFlattenedMenuItemSubtree(items, item);
      }
    }
  }
  return items;
}

void CefWebExtensionMenuManager::GetFlattenedMenuItemSubtree(std::vector<NWebContextMenusItem>& items,
    const std::unique_ptr<extensions::MenuItem>& item) {
  items.push_back(GetNWebContextMenusItem(item.get()));
  for (const auto& child : item->children()) {
    GetFlattenedMenuItemSubtree(items, child);
  }
}
 
NO_SANITIZE("cfi-icall")
void CefWebExtensionMenuManager::OnContextMenusCreate(const std::string& extension_id,
    extensions::MenuItem* menu_item) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebContextMenusItem item = GetNWebContextMenusItem(menu_item);
  if (IsNativeApiEnable()) {
    if (NWebExtensionContextMenusDispatcher::HasOnCreateNativeByPbCallback()) {
      NWebContextMenusItemV2 item_v2 = GetNWebContextMenusItemV2(menu_item);
      NWebExtensionContextMenusDispatcher::OnCreateNativeByPb(extension_id, item_v2);
    } else {
      NWebExtensionContextMenusDispatcher::OnCreateNative(extension_id, item);
    }
  } else {
    NWebExtensionContextMenusDispatcher::OnCreate(extension_id, item);
  }
#endif
}
 
NO_SANITIZE("cfi-icall")
void CefWebExtensionMenuManager::OnContextMenusUpdate(const std::string& extension_id,
    extensions::MenuItem* menu_item) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebContextMenusItem item = GetNWebContextMenusItem(menu_item);
  if (IsNativeApiEnable()) {
    if (NWebExtensionContextMenusDispatcher::HasOnUpdateNativeByPbCallback()) {
      NWebContextMenusItemV2 item_v2 = GetNWebContextMenusItemV2(menu_item);
      NWebExtensionContextMenusDispatcher::OnUpdateNativeByPb(extension_id, item_v2);
    } else {
      NWebExtensionContextMenusDispatcher::OnUpdateNative(extension_id, item);
    }
  } else {
    NWebExtensionContextMenusDispatcher::OnUpdate(extension_id, item);
  }
#endif
}

NO_SANITIZE("cfi-icall")
void CefWebExtensionMenuManager::OnContextMenusRemove(const std::string& extension_id,
    int menu_item_id) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebExtensionContextMenusDispatcher::OnRemoveNative(extension_id, menu_item_id, nullptr);
#endif
}

NO_SANITIZE("cfi-icall")
void CefWebExtensionMenuManager::OnContextMenusRemove(const std::string& extension_id,
    const std::string& menu_item_id) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (IsNativeApiEnable()) {
    NWebExtensionContextMenusDispatcher::OnRemoveNative(extension_id, 0,
                                                        menu_item_id.c_str());
  } else {
    NWebExtensionContextMenusDispatcher::OnRemove(extension_id, menu_item_id);
  }
#endif
}
 
NO_SANITIZE("cfi-icall")
void CefWebExtensionMenuManager::OnContextMenusRemoveAll(const std::string& extension_id) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (IsNativeApiEnable()) {
    NWebExtensionContextMenusDispatcher::OnRemoveAllNative(extension_id);
  } else {
    NWebExtensionContextMenusDispatcher::OnRemoveAll(extension_id);
  }
#endif
}
