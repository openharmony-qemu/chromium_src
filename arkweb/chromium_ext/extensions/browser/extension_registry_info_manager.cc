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

#include "base/logging.h"
#include "base/no_destructor.h"
#include "chrome/browser/extensions/menu_manager.h"
#include "chrome/common/extensions/api/omnibox/omnibox_handler.h"
#include "chrome/common/extensions/chrome_manifest_url_handlers.h"
#include "chrome/common/extensions/manifest_handlers/settings_overrides_handler.h"
#include "content/public/common/url_constants.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/constants.h"
#include "extensions/common/manifest_constants.h"
#include "extensions/common/manifest_handlers/icons_handler.h"
#include "extensions/common/manifest_handlers/options_page_info.h"
#include "extensions/common/manifest_url_handlers.h"
#include "extensions/browser/extension_action_icon_factory.h"
#include "extensions/browser/extension_icon_placeholder.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extension_system.h"
#include "extensions/browser/install_prefs_helper.h"
#include "extensions/browser/ui_util.h"
#include "extensions/browser/management_policy.h"
#include "extensions/browser/unloaded_extension_reason.h"
#include "extensions/browser/extension_prefs.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_action_cef_delegate.h"
#include "ohos_nweb/src/capi/nweb_context_menus_item.h"
#include "ohos_nweb/src/nweb_common.h"
#include "ui/gfx/image/image_skia_operations.h"

#if BUILDFLAG(ARKWEB_TEST)
#include "extension_registry_info_manager_test.h"
#endif

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "extensions/browser/extension_util.h"
#include "extensions/common/manifest_handlers/incognito_info.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_manager_dispatcher.h"
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

namespace extensions {
#ifndef BUILDFLAG(ARKWEB_TEST)
namespace {
#endif
enum LoadPhase {
  kStartInitialLoad,
  kEndInitialLoad,
  kNormalLoad,
};
 
LoadPhase g_load_phase = kNormalLoad;
std::set<std::string>& g_initial_loaded_extensions() {
  static base::NoDestructor<std::set<std::string>> initial_loaded_extensions;
  return *initial_loaded_extensions;
}

constexpr char kExtensionsHost[] = "extensions";
constexpr char kUrlSeparator[] = "://";
constexpr int kTabIdNone = -1;
constexpr float kBrowserIconScale = 2.0f;

std::shared_ptr<NWebExtensionManagerCallBack>& g_extension_manager_listener() {
  static base::NoDestructor<std::shared_ptr<NWebExtensionManagerCallBack>>
      extension_manager_listener(nullptr);
  return *extension_manager_listener;
}

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
  for (uint32_t contextInt = extensions::MenuItem::Context::ALL;
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

NWebContextMenusItemV2 GetNWebContextMenusItemV2(extensions::MenuItem* menu_item) {
  NWebContextMenusItemV2 item;
  item.item = GetNWebContextMenusItem(menu_item);
  item.isOffTheRecord = menu_item->incognito();
  item.intId = menu_item->id().uid;
  return item;
}

void GetFlattenedMenuItemSubtree(std::vector<NWebContextMenusItem>& items,
                                 const std::unique_ptr<extensions::MenuItem>& item) {
  items.push_back(GetNWebContextMenusItem(item.get()));
  for (const auto& child : item->children()) {
    GetFlattenedMenuItemSubtree(items, child);
  }
}

void GetFlattenedMenuItemSubtreeV2(std::vector<NWebContextMenusItemV2>& items,
                                   const std::unique_ptr<extensions::MenuItem>& item) {
  items.push_back(GetNWebContextMenusItemV2(item.get()));
  for (const auto& child : item->children()) {
    GetFlattenedMenuItemSubtreeV2(items, child);
  }
}

std::optional<std::array<int32_t, EXT_COLOR_MAX>> GetBadgeBackgroundColor(
    int32_t tabId,
    ExtensionAction& extension_action) {
  if (!extension_action.HasBadgeBackgroundColor(tabId)) {
    return std::nullopt;
  }
  SkColor groudColor = extension_action.GetBadgeBackgroundColor(tabId);
  std::array<int32_t, EXT_COLOR_MAX> color;
  color[EXT_COLOR_RED] = static_cast<int32_t>(SkColorGetR(groudColor));
  color[EXT_COLOR_GREEN] = static_cast<int32_t>(SkColorGetG(groudColor));
  color[EXT_COLOR_BLUE] = static_cast<int32_t>(SkColorGetB(groudColor));
  color[EXT_COLOR_ALPHA] = static_cast<int32_t>(SkColorGetA(groudColor));

  return color;
}

std::optional<std::array<int32_t, EXT_COLOR_MAX>> GetBadgeTextColor(
    int32_t tabId,
    ExtensionAction& extension_action) {
  if (!extension_action.HasBadgeTextColor(tabId)) {
    return std::nullopt;
  }
  SkColor textColor = extension_action.GetBadgeTextColor(tabId);
  std::array<int32_t, EXT_COLOR_MAX> color;
  color[EXT_COLOR_RED] = static_cast<int32_t>(SkColorGetR(textColor));
  color[EXT_COLOR_GREEN] = static_cast<int32_t>(SkColorGetG(textColor));
  color[EXT_COLOR_BLUE] = static_cast<int32_t>(SkColorGetB(textColor));
  color[EXT_COLOR_ALPHA] = static_cast<int32_t>(SkColorGetA(textColor));

  return color;
}

std::optional<WebExtensionManifestOmnibox> GetManifestOmnibox(
    const Extension* extension) {
  const std::string& keyword = OmniboxInfo::GetKeyword(extension);
  if (keyword.empty()) {
    return std::nullopt;
  }

  WebExtensionManifestOmnibox omnibox;
  omnibox.keyword = keyword;
  return std::make_optional<WebExtensionManifestOmnibox>(keyword);
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

int UnloadedExtensionReasonEnumToInt(UnloadedExtensionReason reason) {
  switch (reason) {
    case UnloadedExtensionReason::UNDEFINED:
      return 0;
    case UnloadedExtensionReason::DISABLE:
      return 1;
    case UnloadedExtensionReason::UPDATE:
      return 2;
    case UnloadedExtensionReason::UNINSTALL:
      return 3;
    default:
      return 0;
  }
}

void GetManifestUrlOverrideInfo(const Extension& extension,
                                WebExtensionManifestInfo& manifest) {
  const URLOverrides::URLOverrideMap& overrides =
      URLOverrides::GetChromeURLOverrides(&extension);
  if (!overrides.empty()) {
    manifest.url_override.emplace(
        WebExtensionManifestUrlOverride());
    if (overrides.count("newtab")) {
      manifest.url_override->newtab = overrides.find("newtab")->second.spec();
    }
    if (overrides.count("bookmarks")) {
      manifest.url_override->bookmarks = overrides.find("bookmarks")->second.spec();
    }
    if (overrides.count("history")) {
      manifest.url_override->history = overrides.find("history")->second.spec();
    }
  }
}

// Same process as in SubstituteInstallParam() in settings_overrides_api.cc.
std::string SubstituteInstallParam(Profile* profile,
                                   const Extension& extension,
                                   std::string str) {
  std::string install_parameter;
  auto prefs = ExtensionPrefs::Get(profile);
  if (prefs) {
    install_parameter = GetInstallParam(prefs, extension.id());
  }
  base::ReplaceSubstringsAfterOffset(&str, 0, "__PARAM__", install_parameter);
  return str;
}

WebExtensionManifestSearchProvider GetSearchProvider(
    Profile* profile,
    const Extension& extension,
    const SettingsOverrides* settings) {
  WebExtensionManifestSearchProvider searcher;

  searcher.name = settings->search_engine->name;
  searcher.keyword = settings->search_engine->keyword;
  if (settings->search_engine->favicon_url) {
    searcher.favicon_url = SubstituteInstallParam(
        profile, extension, settings->search_engine->favicon_url.value());
  }
  searcher.search_url = SubstituteInstallParam(
      profile, extension, settings->search_engine->search_url);
  searcher.encoding = settings->search_engine->encoding;
  if (settings->search_engine->suggest_url) {
    searcher.suggest_url = SubstituteInstallParam(
        profile, extension, settings->search_engine->suggest_url.value());
  }
  if (settings->search_engine->image_url) {
    searcher.image_url = SubstituteInstallParam(
        profile, extension, settings->search_engine->image_url.value());
  }
  searcher.search_url_post_params =
      settings->search_engine->search_url_post_params;
  searcher.suggest_url_post_params =
      settings->search_engine->suggest_url_post_params;
  searcher.image_url_post_params =
      settings->search_engine->image_url_post_params;
  if (settings->search_engine->alternate_urls) {
    for (const auto& url : *settings->search_engine->alternate_urls) {
      if (!url.empty()) {
        searcher.alternate_urls.push_back(
            SubstituteInstallParam(profile, extension, url));
      }
    }
  }
  searcher.prepopulated_id = settings->search_engine->prepopulated_id;
  searcher.is_default = settings->search_engine->is_default;
  return searcher;
}

void GetManifestSettingsOverridesInfo(Profile* profile,
                                      const Extension& extension,
                                      WebExtensionManifestInfo& manifest) {
  const SettingsOverrides* settings = SettingsOverrides::Get(&extension);
  if (!settings)
    return;

  manifest.settings_overrides.emplace();
  if (settings->homepage) {
    manifest.settings_overrides->homepage =
        SubstituteInstallParam(profile, extension, settings->homepage->spec());
  }
  if (!settings->startup_pages.empty()) {
    for (const auto& page : settings->startup_pages) {
      manifest.settings_overrides->startup_pages.emplace_back(
          SubstituteInstallParam(profile, extension, page.spec()));
    }
  }
  if (settings->search_engine) {
    manifest.settings_overrides->search_provider.emplace(
        GetSearchProvider(profile, extension, settings));
  }
}
#ifndef BUILDFLAG(ARKWEB_TEST)
}
#endif

ExtensionRegistryInfoManager::BrowserNotifier::BrowserNotifier(
    const Extension& extension,
    content::BrowserContext* browser_context,
    ExtensionRegistryInfoManager* info_manager)
    : extension_(extension),
      extension_id_(extension.id()),
      browser_context_(browser_context),
      info_manager_(info_manager),
      action_icon_is_ready_(false),
      manifest_icon_is_ready_(false) {
  ExtensionAction* action = ExtensionActionManager::Get(browser_context_)
                                ->GetExtensionAction(extension);
  placeholder_icon_ =
      action ? action->GetPlaceholderIconImage()
             : ExtensionIconPlaceholder::CreateImage(
                   extension_misc::EXTENSION_ICON_BITTY, extension.name());
  placeholder_icon_.AsImageSkia().GetRepresentation(kBrowserIconScale);

  if (action && action->default_icon()) {
    action_icon_image_ = std::make_unique<IconImage>(
        browser_context_, &extension, *action->default_icon(),
        extension_misc::EXTENSION_ICON_BITTY, placeholder_icon_.AsImageSkia(),
        /*observer=*/nullptr);
  } else {
    action_icon_image_ = nullptr;
  }

  if (!IconsInfo::GetIcons(&extension).empty()) {
    manifest_icon_image_ = std::make_unique<IconImage>(
        browser_context_, &extension, IconsInfo::GetIcons(&extension),
        extension_misc::EXTENSION_ICON_BITTY, placeholder_icon_.AsImageSkia(),
        /*observer=*/nullptr);
  } else {
    manifest_icon_image_ = nullptr;
  }

  PopulateAllSyncInfo();
}

ExtensionRegistryInfoManager::BrowserNotifier::~BrowserNotifier() {
  DeleteExtensionActionInfoIcon(loaded_info_.info.action);
}

void ExtensionRegistryInfoManager::BrowserNotifier::HandleImageEvent(
    IconImage* icon_image) {
  if (!IsExtensionValid() || !icon_image->is_valid()) {
    LOG(INFO) << "extension has been unloaded";

    if (g_load_phase != kNormalLoad) {
      g_initial_loaded_extensions().erase(extension_id_);
      if (g_load_phase == kEndInitialLoad &&
          g_initial_loaded_extensions().empty()) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
        NWebExtensionManagerDispatcher::OnExtensionInitLoadEndCallBack();
#endif
        g_load_phase = kNormalLoad;
      }
    }
    return;
  }

  if (icon_image == action_icon_image()) {
    PopulateActionIcon(icon_image->image());
  } else if (icon_image == manifest_icon_image()) {
    PopulateManifestIcon(icon_image->image());
  } else {
    LOG(ERROR) << "Unexpected IconImage event in ExtensionRegistryInfoManager";
  }
  NotifyIfReady();
}

void ExtensionRegistryInfoManager::BrowserNotifier::NotifyIfReady() {
  if (!action_icon_is_ready_ || !manifest_icon_is_ready_) {
    return;
  }
  LOG(INFO) << "BrowserNotifier ready to notify extension: " << extension_id_;
  NotifyManagerExtensionLoaded();

  if (g_load_phase != kNormalLoad) {
    g_initial_loaded_extensions().erase(extension_id_);
    if (g_load_phase == kEndInitialLoad && g_initial_loaded_extensions().empty()) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
      NWebExtensionManagerDispatcher::OnExtensionInitLoadEndCallBack();
#endif
      g_load_phase = kNormalLoad;
    }
  }
}

void ExtensionRegistryInfoManager::BrowserNotifier::PopulateActionIcon(
    const gfx::Image& image) {
  if (action_icon_is_ready_) {
    LOG(ERROR) << "PopulateActionIcon entered more than once";
    return;
  }
  LOG(INFO) << "BrowserNotifier PopulateActionIcon";

  loaded_info_.info.action.icon = new NWebExtensionActionIcon(
      OHOS::NWeb::CreateFromImageSkiaReps(image.AsImageSkia().image_reps()));

  loaded_info_.action_v2.icon = OHOS::NWeb::CreateNWebIconFromImageSkiaRepsV2(
      image.AsImageSkia().image_reps());

  action_icon_is_ready_ = true;
}

void ExtensionRegistryInfoManager::BrowserNotifier::PopulateManifestIcon(
    const gfx::Image& image) {
  if (manifest_icon_is_ready_) {
    LOG(ERROR) << "PopulateManifestIcon entered more than once";
    return;
  }
  LOG(INFO) << "BrowserNotifier PopulateManifestIcon";

  loaded_info_.manifest_info.icons =
      OHOS::NWeb::CreateNWebIconFromImageSkiaRepsV2(
          image.AsImageSkia().image_reps());

  manifest_icon_is_ready_ = true;
}

void ExtensionRegistryInfoManager::BrowserNotifier::PopulateAllSyncInfo() {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  ManagementPolicy* management_policy =
      extensions::ExtensionSystem::Get(browser_context_)->management_policy();
  if (!management_policy) {
    return;
  }
  loaded_info_.info.extensionId = extension_->id();
  loaded_info_.info.mustRemainInstalled =
      management_policy->MustRemainInstalled(&extension_.get(), nullptr);

  loaded_info_.info.action =
      info_manager_->GetExtensionActionInfo(*extension_, kTabIdNone);
  loaded_info_.info.sidePanel =
      info_manager_->GetExtensionSidePanelInfo(*extension_, std::nullopt);
  loaded_info_.info.contextMenus =
      info_manager_->GetAllExtensionContextMenus(extension_->id());
  info_manager_->GetExtensionManifestInfo(*extension_,
                                          loaded_info_.manifest_info);
  loaded_info_.is_incognito_enabled =
      util::IsIncognitoEnabled(extension_->id(), browser_context_);
  loaded_info_.contextMenusV2 =
      info_manager_->GetAllExtensionContextMenusV2(extension_->id());
  loaded_info_.action_v2 =
      info_manager_->GetExtensionActionInfoV2(*extension_, kTabIdNone);
  loaded_info_.install_time = ExtensionPrefs::Get(browser_context_)
                                  ->GetFirstInstallTime(extension_->id())
                                  .InMillisecondsFSinceUnixEpoch();
#endif
}

bool ExtensionRegistryInfoManager::BrowserNotifier::IsExtensionValid() const {
  ExtensionRegistry* registry = ExtensionRegistry::Get(browser_context_);
  const Extension* extension =
      registry ? registry->enabled_extensions().GetByID(extension_id_)
               : nullptr;
  return extension != nullptr;
}

void ExtensionRegistryInfoManager::BrowserNotifier::
    NotifyManagerExtensionLoaded() {
  LOG(INFO) << "BrowserNotifier NotifyManagerExtensionLoaded";
#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (NWebExtensionManagerDispatcher::HasPbLoadedCallback()) {
    NWebExtensionManagerDispatcher::OnExtensionLoadedByPb(loaded_info_);
  } else {
    NWebExtensionManagerDispatcher::OnExtensionLoadedCallBack(
        loaded_info_.info);
  }
#endif
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

  actionInfo.badgeBackgroundColor =
      GetBadgeBackgroundColor(tabId, *extension_action);

  if (extension_action->HasBadgeText(tabId)) {
    actionInfo.badgeText = extension_action->GetExplicitlySetBadgeText(tabId);
  }

  actionInfo.badgeTextColor = GetBadgeTextColor(tabId, *extension_action);

  if (extension_action->HasPopupUrl(tabId)) {
    actionInfo.popup = extension_action->GetPopupUrl(tabId).spec();
  }

  if (extension_action->HasTitle(tabId)) {
    actionInfo.title = extension_action->GetTitle(tabId);
  }

  return actionInfo;
}

// get extension actioninfoV2
WebExtensionActionInfoV2 ExtensionRegistryInfoManager::GetExtensionActionInfoV2(
    const Extension& extension,
    int32_t tabId) const {
  WebExtensionActionInfoV2 action_info;

  ExtensionAction* extension_action =
      ExtensionActionManager::Get(browser_context_)
          ->GetExtensionAction(extension);
  if (!extension_action) {
    LOG(ERROR) << "GetExtensionActionInfoV2, extension_action is null";
    return action_info;
  }

  LOG(INFO) << "GetExtensionActionInfoV2, tabId=" << tabId
            << " extensionId=" << extension.id();

  action_info.extensionId = extension.id();
  if (extension_action->HasIsVisible(tabId)) {
    action_info.isEnabled = extension_action->GetIsVisible(tabId);
  }
  action_info.badgeBackgroundColor =
      GetBadgeBackgroundColor(tabId, *extension_action);
  if (extension_action->HasBadgeText(tabId)) {
    action_info.badgeText = extension_action->GetExplicitlySetBadgeText(tabId);
  }
  action_info.badgeTextColor = GetBadgeTextColor(tabId, *extension_action);
  if (extension_action->HasPopupUrl(tabId)) {
    action_info.popup = extension_action->GetPopupUrl(tabId).spec();
  }
  if (extension_action->HasTitle(tabId)) {
    action_info.title = extension_action->GetTitle(tabId);
  }

  return action_info;
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

std::vector<NWebContextMenusItemV2> ExtensionRegistryInfoManager::GetAllExtensionContextMenusV2(
    const std::string& extensionId) const {
  std::vector<NWebContextMenusItemV2> items;
  extensions::MenuManager* menu_manager = extensions::MenuManager::Get(browser_context_);
  if (!menu_manager) {
    LOG(ERROR) << "menu_manager is null";
    return items;
  }
  for (const auto& id : menu_manager->ExtensionIds()) {
    if (extensionId == id.extension_id) {
      const extensions::MenuItem::OwnedList* top_items = menu_manager->MenuItems(id);
      for (const std::unique_ptr<extensions::MenuItem>& item : *top_items) {
        GetFlattenedMenuItemSubtreeV2(items, item);
      }
    }
  }
  LOG(DEBUG) << "CefMenuManager::GetAllExtensionContextMenusV2 items.size:" << items.size();
  return items;
}

void ExtensionRegistryInfoManager::GetExtensionManifestInfo(
    const Extension& extension,
    WebExtensionManifestInfo& manifest) const {
  manifest.name = extension.name();
  const GURL& homepage_url = ManifestURL::GetManifestHomePageURL(&extension);
  if (homepage_url.is_valid()) {
    manifest.homepage_url = homepage_url.spec();
  }
  GetManifestSettingsOverridesInfo(
      Profile::FromBrowserContext(browser_context_), extension, manifest);
  GetManifestUrlOverrideInfo(extension, manifest);
  manifest.options_page = GetManifestOptionsPageInfo(extension);
#if BUILDFLAG(ARKWEB_NWEB_EX)
  manifest.incognito_mode =
      std::make_optional<ExtensionIncognitoMode>(GetExtensionIncognitoMode(&extension));
#endif
  manifest.omnibox = GetManifestOmnibox(&extension);
  manifest.short_name = extension.short_name();
}

#if BUILDFLAG(ARKWEB_NWEB_EX)
ExtensionIncognitoMode ExtensionRegistryInfoManager::GetExtensionIncognitoMode(
    const Extension* extension) const {
  if (!IncognitoInfo::IsIncognitoAllowed(extension)) {
    return EXT_INCOGNITO_NOT_ALLOWED;
  }

  if (IncognitoInfo::IsSpanningMode(extension)) {
    return EXT_INCOGNITO_SPANNING;
  }

  if (IncognitoInfo::IsSplitMode(extension)) {
    return EXT_INCOGNITO_SPLIT;
  }

  return EXT_INCOGNITO_NONE;
}
#endif

void ExtensionRegistryInfoManager::Loaded(const std::string& extension_id) {
  LOG(INFO) << "ExtensionRegistryInfoManager::Loaded";
  ExtensionRegistry* registry = ExtensionRegistry::Get(browser_context_);
  const Extension* extension =
      registry ? registry->enabled_extensions().GetByID(extension_id) : nullptr;
  if (!extension) {
    LOG(ERROR) << "Loaded called with unknown extension id";
    return;
  }

  if (!extensions::ui_util::ShouldDisplayInExtensionSettings(*extension)) {
    return;
  }

  if (g_load_phase == kStartInitialLoad) {
    g_initial_loaded_extensions().insert(extension_id);
  }

  StartNotifyingExtensionLoaded(*extension);
}

void ExtensionRegistryInfoManager::OnExtensionUnloaded(content::BrowserContext* browser_context,
                                                       const Extension* extension,
                                                       UnloadedExtensionReason reason) {
  // It must be triggered after the observer notification.
#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebExtensionManagerDispatcher::OnExtensionUnLoadedCallBack(
      extension->id(), UnloadedExtensionReasonEnumToInt(reason));
#endif
}

void ExtensionRegistryInfoManager::OnExtensionInstalled(content::BrowserContext* browser_context,
                                                        const Extension* extension,
                                                        bool is_update) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (!extensions::ui_util::ShouldDisplayInExtensionSettings(*extension)) {
    return;
  }

  NWebExtensionManagerDispatcher::OnExtensionInstalledCallBack(
      extension->id(), extension->creation_flags(), static_cast<int>(extension->manifest()->location()));
#endif
}

void ExtensionRegistryInfoManager::OnExtensionUninstalled(content::BrowserContext* browser_context,
                                                          const Extension* extension,
                                                          UninstallReason reason) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    NWebExtensionManagerDispatcher::OnExtensionUninstalledCallBack(
      extension->id(), static_cast<int>(reason));
#endif
}

// static
void ExtensionRegistryInfoManager::RegisterWebExtensionManagerListener(
    std::shared_ptr<NWebExtensionManagerCallBack> web_extension_manager_listener) {
  LOG(INFO) << "ExtensionRegistryInfoManager::RegisterWebExtensionManagerListener";
  g_extension_manager_listener() = web_extension_manager_listener;
}

// static
void ExtensionRegistryInfoManager::UnRegisterWebExtensionManagerListener() {
  LOG(INFO) << "ExtensionRegistryInfoManager::UnRegisterWebExtensionManagerListener";
  g_extension_manager_listener() = nullptr;
}

//static
NO_SANITIZE("cfi-icall")
void ExtensionRegistryInfoManager::OnExtensionLoadedCallBack(const WebExtensionInfo& loadedInfo) {
  LOG(INFO) << "ExtensionRegistryInfoManager::OnExtensionLoadedCallBack";
  if (!g_extension_manager_listener()) {
    LOG(ERROR) << "No web extension manager listener";
    return;
  }

  if (!g_extension_manager_listener()->OnWebExtensionLoaded) {
    LOG(ERROR) << "No OnWebExtensionLoaded listener";
    return;
  }

  g_extension_manager_listener()->OnWebExtensionLoaded(loadedInfo);
}

//static
NO_SANITIZE("cfi-icall")
void ExtensionRegistryInfoManager::OnExtensionUnLoadedCallBack(const std::string& eid) {
  LOG(INFO) << "ExtensionRegistryInfoManager::OnExtensionUnLoadedCallBack";
  if (!g_extension_manager_listener()) {
    LOG(ERROR) << "No web extension manager listener";
    return;
  }

  if (!g_extension_manager_listener()->OnWebExtensionUnLoaded) {
    LOG(ERROR) << "No OnWebExtensionUnLoaded listener";
    return;
  }

  g_extension_manager_listener()->OnWebExtensionUnLoaded(eid);
}

//static
NO_SANITIZE("cfi-icall")
void ExtensionRegistryInfoManager::OnExtensionOpenUrlCallBack(const std::string& url) {
  LOG(INFO) << "ExtensionRegistryInfoManager::OnExtensionOpenUrlCallBack";
  if (!g_extension_manager_listener()) {
    LOG(ERROR) << "No web extension manager listener";
    return;
  }

  if (!g_extension_manager_listener()->OnWebExtensionOpenUrlFun) {
    LOG(ERROR) << "No OnWebExtensionOpenUrlFun listener";
    return;
  }

  g_extension_manager_listener()->OnWebExtensionOpenUrlFun(url);
}

void ExtensionRegistryInfoManager::OnExtensionIconImageChanged(
    IconImage* image) {
  LOG(INFO) << "ExtensionRegistryInfoManager OnExtensionIconImageChanged start";
  std::shared_ptr<BrowserNotifier> notifier = pending_notifiers_[image];
  notifier->HandleImageEvent(image);
  pending_notifiers_.erase(image);
}

void ExtensionRegistryInfoManager::AddIconImageObservation(
    IconImage* image,
    std::shared_ptr<BrowserNotifier> notifier) {
  if (icon_observations_.IsObservingSource(image)) {
    LOG(ERROR) << "observe one image source for multiple times";
    return;
  }

  LOG(INFO) << "ExtensionRegistryInfoManager AddIconImageObservation";
  icon_observations_.AddObservation(image);
  pending_notifiers_[image] = notifier;
}

void ExtensionRegistryInfoManager::StartNotifyingExtensionLoaded(
    const Extension& extension) {
  LOG(INFO) << "ExtensionRegistryInfoManager StartNotifyingExtensionLoaded for "
               "extension "
            << extension.id();

  std::shared_ptr<BrowserNotifier> notifier =
      std::make_shared<BrowserNotifier>(extension, browser_context_, this);

  if (notifier->action_icon_image()) {
    IconImage* action_image = notifier->action_icon_image();
    if (action_image->did_complete_initial_load()) {
      notifier->PopulateActionIcon(action_image->image());
    } else {
      AddIconImageObservation(action_image, notifier);
      gfx::Image icon = action_image->image();
      icon.AsImageSkia().GetRepresentation(kBrowserIconScale);
    }
  } else {
    notifier->PopulateActionIcon(notifier->placeholder_icon());
  }

  if (notifier->manifest_icon_image()) {
    IconImage* manifest_image = notifier->manifest_icon_image();
    if (manifest_image->did_complete_initial_load()) {
      notifier->PopulateManifestIcon(manifest_image->image());
    } else {
      AddIconImageObservation(manifest_image, notifier);
      gfx::Image icon = manifest_image->image();
      icon.AsImageSkia().GetRepresentation(kBrowserIconScale);
    }
  } else {
    notifier->PopulateManifestIcon(notifier->placeholder_icon());
  }

  notifier->NotifyIfReady();
}

void ExtensionRegistryInfoManager::StartInitialLoad() {
  g_load_phase = kStartInitialLoad;
}
 
void ExtensionRegistryInfoManager::StopInitialLoad() {
  g_load_phase = kEndInitialLoad;
  if (g_initial_loaded_extensions().empty()) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    NWebExtensionManagerDispatcher::OnExtensionInitLoadEndCallBack();
#endif
    g_load_phase = kNormalLoad;
  }
}

}  // namespace extensions
