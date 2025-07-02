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

#include "extensions/browser/extension_registry_info_manager.h"

#include "chrome/browser/extensions/menu_manager.h"
#include "chrome/common/extensions/manifest_handlers/settings_overrides_handler.h"
#include "content/public/common/url_constants.h"
#include "ohos_nweb/src/capi/nweb_context_menus_item.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/manifest_constants.h"
#include "extensions/common/manifest_handlers/options_page_info.h"
#include "extensions/common/manifest_url_handlers.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_action_cef_delegate.h"
#include "extensions/browser/extension_action_icon_factory.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extension_system.h"
#include "extensions/browser/ui_util.h"
#include "ui/gfx/image/image_skia_operations.h"
#include "extensions/browser/management_policy.h"
#include "base/logging.h"
#include "ohos_nweb/src/nweb_common.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/extension/nweb_extension_manager_dispatcher.h"
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
extern "C" {
void* __real_malloc(size_t);
}       // extern "C"
#endif

namespace extensions {

namespace {
constexpr char kExtensionsHost[] = "extensions";
constexpr char kUrlSeparator[] = "://";

std::string GetTypeStr(extensions::MenuItem::Type type) {
  switch (type) {
    case extensions::MenuItem::Type::NORMAL : return "normal";
    case extensions::MenuItem::Type::CHECKBOX : return "checkbox";
    case extensions::MenuItem::Type::RADIO : return "radio";
    case extensions::MenuItem::Type::SEPARATOR : return "separator";
  };
  return {};
}
 
std::string GetContextStr(extensions::MenuItem::Context context) {
  switch (context) {
    case extensions::MenuItem::Context::ALL : return "all";
    case extensions::MenuItem::Context::PAGE : return "page";
    case extensions::MenuItem::Context::SELECTION : return "selection";
    case extensions::MenuItem::Context::LINK : return "link";
    case extensions::MenuItem::Context::EDITABLE : return "editable";
    case extensions::MenuItem::Context::IMAGE : return "image";
    case extensions::MenuItem::Context::VIDEO : return "video";
    case extensions::MenuItem::Context::AUDIO : return "audio";
    case extensions::MenuItem::Context::FRAME : return "frame";
    case extensions::MenuItem::Context::LAUNCHER : return "launcher";
    case extensions::MenuItem::Context::BROWSER_ACTION : return "browser_action";
    case extensions::MenuItem::Context::PAGE_ACTION : return "page_action";
    case extensions::MenuItem::Context::ACTION : return "action";
  };
  return {};
}
 
std::vector<std::string> ContextListToStrVector(const extensions::MenuItem::ContextList& contextList) {
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

NWebContextMenusItem GetNWebContextMenusItem(extensions::MenuItem* menu_item) {
  NWebContextMenusItem item;
  item.checked = menu_item->checked();
  item.contexts = ContextListToStrVector(menu_item->contexts());
  item.documentUrlPatterns = menu_item->document_url_str_patterns();
  item.enabled = menu_item->enabled();
  item.id = menu_item->id().string_uid;
  if (menu_item->parent_id()) {
    item.parentId = menu_item->parent_id()->string_uid;
  }
  item.targetUrlPatterns = menu_item->target_url_str_patterns();
  item.title = menu_item->title();
  item.type = GetTypeStr(menu_item->type());
  item.visible = menu_item->visible();
  item.extensionId = menu_item->extension_id();
  return item;
}

void GetFlattenedMenuItemSubtree(std::vector<NWebContextMenusItem>& items,
                                                  const std::unique_ptr<extensions::MenuItem>& item) {
  items.push_back(GetNWebContextMenusItem(item.get()));
  for (const auto& child : item->children()) {
    GetFlattenedMenuItemSubtree(items, child);
  }
}

void GetBadgeBackgroundColor(int32_t tabId, ExtensionAction* extension_action, WebExtensionActionInfo& actionInfo) {
  if (extension_action->HasBadgeBackgroundColor(tabId)) {
    SkColor groudColor = extension_action->GetBadgeBackgroundColor(tabId);
    std::array<int32_t, EXT_COLOR_MAX> colors;
    colors[EXT_COLOR_RED] = static_cast<int32_t>(SkColorGetR(groudColor));
    colors[EXT_COLOR_GREEN] = static_cast<int32_t>(SkColorGetG(groudColor));
    colors[EXT_COLOR_BLUE] = static_cast<int32_t>(SkColorGetB(groudColor));
    colors[EXT_COLOR_ALPHA] = static_cast<int32_t>(SkColorGetA(groudColor));
    actionInfo.badgeBackgroundColor = std::optional<std::array<int32_t, EXT_COLOR_MAX>>(colors);
  }
}

void GetBadgeTextColor(int32_t tabId, ExtensionAction* extension_action, WebExtensionActionInfo& actionInfo) {
  if (extension_action->HasBadgeTextColor(tabId)) {
    SkColor textColor = extension_action->GetBadgeTextColor(tabId);
    std::array<int32_t, EXT_COLOR_MAX> colors;
    colors[EXT_COLOR_RED] = static_cast<int32_t>(SkColorGetR(textColor));
    colors[EXT_COLOR_GREEN] = static_cast<int32_t>(SkColorGetG(textColor));
    colors[EXT_COLOR_BLUE] = static_cast<int32_t>(SkColorGetB(textColor));
    colors[EXT_COLOR_ALPHA] = static_cast<int32_t>(SkColorGetA(textColor));
    actionInfo.badgeTextColor = std::optional<std::array<int32_t, EXT_COLOR_MAX>>(colors);
  }
}

std::optional<WebExtensionManifestOptionsPageInfo> GetManifestOptionsPageInfo(
    const Extension& extension) {
  if (!OptionsPageInfo::HasOptionsPage(&extension)) {
    return std::nullopt;
  }
 
  WebExtensionManifestOptionsPageInfo page;
  page.open_in_tab = OptionsPageInfo::ShouldOpenInTab(&extension);
  if (page.open_in_tab) {
    page.options_page = OptionsPageInfo::GetOptionsPage(&extension).spec();
  } else {
    std::string url_str =
        std::string(content::kArkWebUIScheme) + kUrlSeparator + kExtensionsHost;
    GURL url_to_navigate = GURL(url_str);
    GURL::Replacements replacements;
    std::string query =
        base::StringPrintf("options=%s", extension.id().c_str());
    replacements.SetQueryStr(query);
    page.options_page =
        url_to_navigate.ReplaceComponents(replacements).spec();
  }
  return std::make_optional<WebExtensionManifestOptionsPageInfo>(page);
}

}

std::shared_ptr<NWebExtensionManagerCallBack> g_extension_manager_listener = nullptr;


gfx::Image ExtensionRegistryGetIcon(
    int tabId,
    ExtensionAction* extension_action) {
  gfx::Image icon = extension_action->GetExplicitlySetIcon(tabId);
  if (!icon.IsEmpty()) {
    return icon;
  }

  icon = extension_action->GetDeclarativeIcon(tabId);
  if (!icon.IsEmpty()) {
    return icon;
  }
 
  return extension_action->GetDefaultIconImageV2();
}

ExtensionRegistryInfoManager::ExtensionRegistryInfoManager(
  content::BrowserContext* browser_context) : browser_context_(browser_context) {
  LOG(INFO) << "ExtensionRegistryInfoManager::init";
}

// get extension actioninfo
WebExtensionActionInfo ExtensionRegistryInfoManager::GetExtensionActionInfo(
    const Extension& extension,
    int32_t tabId) const {
  WebExtensionActionInfo actionInfo;
  actionInfo.extensionId = extension.id();
  ExtensionAction* extension_action = ExtensionActionManager::Get(browser_context_)->GetExtensionAction(extension);
  LOG(INFO) << "GetExtensionActionInfo, tabId=" << tabId << " extensionId=" << extension.id();
  if (!extension_action) {
    LOG(ERROR) << "GetExtensionActionInfo, extension_action is null";
    return actionInfo;
  }
  if (extension_action->HasIsVisible(tabId)) {
    actionInfo.isEnabled = extension_action->GetIsVisible(tabId);
  }

  GetBadgeBackgroundColor(tabId, extension_action, actionInfo);

  if (extension_action->HasBadgeText(tabId)) {
    actionInfo.badgeText = extension_action->GetExplicitlySetBadgeText(tabId);
  }
  
  GetBadgeTextColor(tabId, extension_action, actionInfo);
  
  if (extension_action->HasPopupUrl(tabId)) {
    actionInfo.popup = extension_action->GetPopupUrl(tabId).spec();
  }

  if (extension_action->HasTitle(tabId)) {
    actionInfo.title = extension_action->GetTitle(tabId);
  }

  gfx::Image icon_image = ExtensionRegistryGetIcon(tabId, extension_action);
  if (!icon_image.IsEmpty()) {
    constexpr float GET_ICON_SCALE = 8.0f;
    gfx::ImageSkia skia = icon_image.AsImageSkia();
    gfx::ImageSkiaRep rep = skia.GetRepresentation(GET_ICON_SCALE);
    if (rep.scale() != GET_ICON_SCALE) {
      int width_px =
          extensions::ExtensionAction::ActionIconSize() * GET_ICON_SCALE;
      gfx::ImageSkiaRep resized_rep = gfx::ImageSkiaRep(
          skia::ImageOperations::Resize(rep.GetBitmap(),
                                        skia::ImageOperations::RESIZE_BEST,
                                        width_px, width_px),
          GET_ICON_SCALE);
      skia.AddRepresentation(resized_rep);
    }
    OHOS::NWeb::NWebExtensionActionIcon icon =
        OHOS::NWeb::CreateFromImageSkiaReps(
            icon_image.AsImageSkia().image_reps());
#if defined(ADDRESS_SANITIZER) || defined(HWADDRESS_SANITIZER)
    OHOS::NWeb::NWebExtensionActionIcon* iconTmp = new OHOS::NWeb::NWebExtensionActionIcon(icon);
#else
    OHOS::NWeb::NWebExtensionActionIcon* addr = (OHOS::NWeb::NWebExtensionActionIcon*)__real_malloc(sizeof(icon));
    OHOS::NWeb::NWebExtensionActionIcon* iconTmp = new (addr) OHOS::NWeb::NWebExtensionActionIcon(icon);
#endif
    actionInfo.icon = iconTmp;
  }

  return actionInfo;
}

// get extension sidepanel
WebExtensionSidePanelInfo ExtensionRegistryInfoManager::GetExtensionSidePanelInfo (
    const Extension& extension,
    std::optional<int32_t> tabId) const {
  LOG(INFO) << "GetExtensionSidePanelInfo, extensionId=" << extension.id();
  if (tabId) {
    LOG(INFO) << "GetExtensionSidePanelInfo, tabId=" << tabId.value();
  }
  WebExtensionSidePanelInfo sidePanelInfo;
  SidePanelService* spService = extensions::SidePanelService::Get(browser_context_);
  if (!spService) {
    LOG(ERROR) << "GetExtensionSidePanelInfo, spService is null";
    return sidePanelInfo;
  }
  const api::side_panel::PanelOptions& options = spService->GetOptions(extension, tabId);
  if (options.enabled) {
    sidePanelInfo.enable = options.enabled.value();
  }
  if (options.path) {
    sidePanelInfo.path = extension.GetResourceURL(options.path.value()).spec();
  }
  sidePanelInfo.openPanelOnActionClick = spService->OpenSidePanelOnIconClick(extension.id());
  return sidePanelInfo;
}

std::vector<NWebContextMenusItem> ExtensionRegistryInfoManager::GetAllExtensionContextMenus(
  const std::string& extensionId) const {
  // std::this_thread::sleep_for(std::chrono::seconds(5));
  std::vector<NWebContextMenusItem> items;
  extensions::MenuManager* menu_manager = extensions::MenuManager::Get(browser_context_);
  if (!menu_manager) {
    LOG(ERROR) << "menu_manager is null";
    return items;
  }
  for (const auto& id : menu_manager->ExtensionIds()) {
    if (extensionId == id.extension_id) {
      const extensions::MenuItem::OwnedList* top_items = menu_manager->MenuItems(id);
      for (const std::unique_ptr<extensions::MenuItem>& item : *top_items) {
        GetFlattenedMenuItemSubtree(items, item);
      }
    }
  }
  LOG(DEBUG) << "CefMenuManager::GetAllExtensionContextMenus items.size:" << items.size();
  return items;
}

void DeleteExtensionActionInfoIcon(WebExtensionActionInfo& action_info) {
  if (!action_info.icon.has_value()) {
    return;
  }
 
  for (auto it : action_info.icon.value()->bitmaps) {
    delete it.second;
  }
  delete action_info.icon.value();
  action_info.icon.reset();
}

void ExtensionRegistryInfoManager::GetExtensionManifestInfo(
    const Extension& extension,
    WebExtensionManifestInfo& manifest) const {
  const GURL& homepage_url = ManifestURL::GetManifestHomePageURL(&extension);
  if (homepage_url.is_valid()) {
    manifest.homepage_url = homepage_url.spec();
  }
  const SettingsOverrides* settings = SettingsOverrides::Get(&extension);
  if (settings) {
    manifest.settings_overrides.emplace(
        WebExtensionManifestSettingsOverrides());
    if (settings->homepage) {
      manifest.settings_overrides->homepage = settings->homepage->spec();
    }
    if (!settings->startup_pages.empty()) {
      for (const auto& page : settings->startup_pages) {
        manifest.settings_overrides->startup_pages.emplace_back(page.spec());
      }
    }
    if (settings->search_engine) {
      WebExtensionManifestSearchProvider searcher;
      searcher.name = settings->search_engine->name;
      searcher.keyword = settings->search_engine->keyword;
      searcher.favicon_url = settings->search_engine->favicon_url;
      searcher.search_url = settings->search_engine->search_url;
      searcher.encoding = settings->search_engine->encoding;
      searcher.suggest_url = settings->search_engine->suggest_url;
      searcher.image_url = settings->search_engine->image_url;
      searcher.search_url_post_params =
          settings->search_engine->search_url_post_params;
      searcher.suggest_url_post_params =
          settings->search_engine->suggest_url_post_params;
      searcher.image_url_post_params =
          settings->search_engine->image_url_post_params;
      if (settings->search_engine->alternate_urls)
        searcher.alternate_urls = *settings->search_engine->alternate_urls;
      searcher.prepopulated_id = settings->search_engine->prepopulated_id;
      searcher.is_default = settings->search_engine->is_default;
      manifest.settings_overrides->search_provider.emplace(std::move(searcher));
    }
  }
  manifest.options_page = GetManifestOptionsPageInfo(extension);
}

void ExtensionRegistryInfoManager::NotifyOnExtensionLoaded(const Extension& extension) {
  if (!extensions::ui_util::ShouldDisplayInExtensionSettings(extension)) {
    return;
  }
  LOG(INFO) << "ExtensionRegistryInfoManager::NotifyOnExtensionLoaded";
  ManagementPolicy* management_policy = extensions::ExtensionSystem::Get(
        browser_context_)->management_policy();
  if (IsNativeApiEnable()) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    if (NWebExtensionManagerDispatcher::HasPbLoadedCallback()) {
      WebExtensionInfoV2 info;
      info.info.extensionId = extension.id();
      info.info.mustRemainInstalled =
          management_policy->MustRemainInstalled(&extension, nullptr);
      info.info.isOnToolbar = false;
      info.info.action = GetExtensionActionInfo(extension, -1);
      info.info.sidePanel = GetExtensionSidePanelInfo(extension, std::nullopt);
      info.info.contextMenus = GetAllExtensionContextMenus(extension.id());
      GetExtensionManifestInfo(extension, info.manifest_info);
      NWebExtensionManagerDispatcher::OnExtensionLoadedByPb(info);
      DeleteExtensionActionInfoIcon(info.info.action);
    } else {
      WebExtensionInfo loadedInfo;
      loadedInfo.extensionId = extension.id();
      loadedInfo.mustRemainInstalled =
          management_policy->MustRemainInstalled(&extension, nullptr);
      loadedInfo.action = GetExtensionActionInfo(extension, -1);
      loadedInfo.sidePanel = GetExtensionSidePanelInfo(extension, std::nullopt);
      loadedInfo.contextMenus = GetAllExtensionContextMenus(extension.id());
      NWebExtensionManagerDispatcher::OnExtensionLoadedCallBack(loadedInfo);
      DeleteExtensionActionInfoIcon(loadedInfo.action);
    }
#endif
  } else {
    WebExtensionInfo loadedInfo;
    loadedInfo.extensionId = extension.id();
    loadedInfo.mustRemainInstalled =
        management_policy->MustRemainInstalled(&extension, nullptr);
    loadedInfo.action = GetExtensionActionInfo(extension, -1);
    loadedInfo.sidePanel = GetExtensionSidePanelInfo(extension, std::nullopt);
    loadedInfo.contextMenus = GetAllExtensionContextMenus(extension.id());
    ExtensionRegistryInfoManager::OnExtensionLoadedCallBack(loadedInfo);
    if (loadedInfo.action.icon) {
      loadedInfo.action.icon.value()->bitmaps =
          std::map<double, OHOS::NWeb::NWebExtensionActionIconBitmap*>();
    }
  }
}

void ExtensionRegistryInfoManager::Loaded(const std::string& extension_id) {
  LOG(INFO) << "ExtensionRegistryInfoManager::Loaded";
  ExtensionRegistry* registry = ExtensionRegistry::Get(browser_context_);
  const Extension* extension =
      registry->enabled_extensions().GetByID(extension_id);
  NotifyOnExtensionLoaded(*extension);
}

void ExtensionRegistryInfoManager::OnExtensionLoaded(content::BrowserContext* browser_context,
                                                     const Extension* extension) {
}

void ExtensionRegistryInfoManager::OnExtensionReady(content::BrowserContext* browser_context,
                                                    const Extension* extension) {
}

void ExtensionRegistryInfoManager::OnExtensionUnloaded(content::BrowserContext* browser_context,
                                                       const Extension* extension,
                                                       UnloadedExtensionReason reason) {
  // It must be triggered after the observer notification.
  if (IsNativeApiEnable()) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    NWebExtensionManagerDispatcher::OnExtensionUnLoadedCallBack(extension->id());
#endif
  } else {
    OnExtensionUnLoadedCallBack(extension->id());
  }
}

void ExtensionRegistryInfoManager::OnExtensionWillBeInstalled(content::BrowserContext* browser_context,
                                                              const Extension* extension,
                                                              bool is_update,
                                                              const std::string& old_name) {}

void ExtensionRegistryInfoManager::OnExtensionInstalled(content::BrowserContext* browser_context,
                                                        const Extension* extension,
                                                        bool is_update) {}

void ExtensionRegistryInfoManager::OnExtensionUninstalled(content::BrowserContext* browser_context,
                                                          const Extension* extension,
                                                          UninstallReason reason) {}

void ExtensionRegistryInfoManager::OnExtensionUninstallationDenied(content::BrowserContext* browser_context,
                                                                   const Extension* extension) {}

void ExtensionRegistryInfoManager::OnShutdown(ExtensionRegistry* registry) {}

// static
void ExtensionRegistryInfoManager::RegisterWebExtensionManagerListener(
    std::shared_ptr<NWebExtensionManagerCallBack> web_extension_manager_listener) {
  LOG(INFO) << "ExtensionRegistryInfoManager::RegisterWebExtensionManagerListener";
  g_extension_manager_listener = web_extension_manager_listener;
}

// static
void ExtensionRegistryInfoManager::UnRegisterWebExtensionManagerListener() {
  LOG(INFO) << "ExtensionRegistryInfoManager::RegisterWebExtensionManagerListener";
  g_extension_manager_listener = nullptr;
}

//static
NO_SANITIZE("cfi-icall")
void ExtensionRegistryInfoManager::OnExtensionLoadedCallBack(const WebExtensionInfo& loadedInfo) {
  LOG(INFO) << "ExtensionRegistryInfoManager::OnExtensionLoadedCallBack";
  if (!g_extension_manager_listener) {
    LOG(ERROR) << "No web extension manager listener";
    return;
  }

  if (!g_extension_manager_listener->OnWebExtensionLoaded) {
    LOG(ERROR) << "No OnWebExtensionLoaded listener";
    return;
  }

  g_extension_manager_listener->OnWebExtensionLoaded(loadedInfo);
}

//static
NO_SANITIZE("cfi-icall")
void ExtensionRegistryInfoManager::OnExtensionUnLoadedCallBack(const std::string& eid) {
  LOG(INFO) << "ExtensionRegistryInfoManager::OnExtensionUnLoadedCallBack";
  if (!g_extension_manager_listener) {
    LOG(ERROR) << "No web extension manager listener";
    return;
  }

  if (!g_extension_manager_listener->OnWebExtensionUnLoaded) {
    LOG(ERROR) << "No OnWebExtensionUnLoaded listener";
    return;
  }

  g_extension_manager_listener->OnWebExtensionUnLoaded(eid);
}

//static
NO_SANITIZE("cfi-icall")
void ExtensionRegistryInfoManager::OnExtensionOpenUrlCallBack(const std::string& url) {
  LOG(INFO) << "ExtensionRegistryInfoManager::OnExtensionOpenUrlCallBack";
  if (!g_extension_manager_listener) {
    LOG(ERROR) << "No web extension manager listener";
    return;
  }

  if (!g_extension_manager_listener->OnWebExtensionOpenUrlFun) {
    LOG(ERROR) << "No OnWebExtensionOpenUrlFun listener";
    return;
  }

  g_extension_manager_listener->OnWebExtensionOpenUrlFun(url);
}

}  // namespace extensions
