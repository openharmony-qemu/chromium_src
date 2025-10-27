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

#include "base/types/optional_util.h"
#include "base/task/bind_post_task.h"
#include "chrome/browser/extensions/api/tabs/tabs_constants.h"
#include "chrome/browser/extensions/api/tabs/tabs_api.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "chrome/browser/profiles/profile.h"
#include "components/sessions/content/session_tab_helper.h"
#include "components/zoom/zoom_controller.h"
#include "extensions/browser/extension_zoom_request_client.h"
#include "extensions/common/manifest_constants.h"
#include "extensions/common/mojom/api_permission_id.mojom-shared.h"
#include "extensions/common/permissions/permissions_data.h"

#include "libcef/browser/browser_info_manager.h"
#include "libcef/browser/extensions/extension_function_details.h"
#include "libcef/browser/extensions/tab_extensions_util.h"
#include "libcef/browser/extensions/window_extensions_util.h"
#include "third_party/blink/public/common/page/page_zoom.h"
#include "ohos_nweb/src/capi/web_extension_window_items.h"
#include "ohos_nweb/src/capi/web_extension_tab_items.h"
#include "ohos_nweb/src/cef_delegate/nweb_handler_delegate.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_tab_cef_delegate.h"
#include "ohos_nweb/src/cef_delegate/nweb_handler_delegate.h"
#include "extensions/browser/extension_zoom_request_client.h"
#include "base/types/optional_util.h"
#include "base/task/bind_post_task.h"
#include "third_party/blink/public/common/page/page_zoom.h"

using content::NavigationController;
using content::WebContents;
using zoom::ZoomController;

namespace extensions {

namespace tabs = api::tabs;

namespace {

const char kNotImplementedError[] = "Not implemented";
const char kJavaScriptUrlsNotAllowedInTabsUpdate[] =
    "JavaScript URLs are not allowed in chrome.tabs.update. Use "
    "chrome.tabs.executeScript instead.";
const char kSetOpenerError[] = "Cannot set a tab's opener to itself.";
constexpr char kCannotDetermineLanguageOfUnloadedTab[] =
    "Cannot determine language: tab not loaded";

std::optional<std::string> GetExtensionContextType(
    content::BrowserContext* browser_context) {
  if (!browser_context) {
    return std::nullopt;
  }

  if (browser_context->IsOffTheRecord()) {
    return "INCOGNITO";
  }

  Profile* profile = Profile::FromBrowserContext(browser_context);
  if (!profile) {
    return std::nullopt;
  }

  if (profile->IsRegularProfile()) {
    return "REGULAR";
  }
  return std::nullopt;
}

void SetQueryInfoCurrentWindowId(WebContents* webcontents,
                                 NWebExtensionTabQueryInfo& queryInfo) {
  if (!webcontents) {
    queryInfo.currentWindowId = extension_misc::kCurrentWindowId;
    return;
  }

  int32_t tab_id = webcontents->ExtensionGetTabId();
  if (tab_id > 0) {
    std::unique_ptr<NWebExtensionTab> tab =
        OHOS::NWeb::NWebExtensionTabCefDelegate::GetTab(tab_id);
    if (!tab || tab->nwebId < 0) {
      queryInfo.currentWindowId = extension_misc::kCurrentWindowId;
    } else {
      queryInfo.currentWindowId = tab->windowId;
    }
  } else {
    int nweb_id = webcontents->GetNWebId();
    int window_id =
        extensions::CefExtensionWindowIdManager::GetWindowId(nweb_id);
    if (window_id < 0) {
      queryInfo.currentWindowId = extension_misc::kCurrentWindowId;
    } else {
      queryInfo.currentWindowId = window_id;
    }
  }
}

std::optional<NWebExtensionTabStatus> GetQueryStatus(api::tabs::TabStatus status) {
  std::optional<NWebExtensionTabStatus> tabStatus = std::nullopt;
  switch (status) {
    case api::tabs::TabStatus::kUnloaded:
      tabStatus = NWebExtensionTabStatus::UNLOADED;
    break;
    case api::tabs::TabStatus::kLoading:
      tabStatus = NWebExtensionTabStatus::LOADING;
    break;
    case api::tabs::TabStatus::kComplete:
      tabStatus = NWebExtensionTabStatus::COMPLETE;
    break;
    default:
    break;
  }
  return tabStatus;
}

std::optional<WebExtensionWindowType> GetQueryWindowType(api::tabs::WindowType window_type) {
  std::optional<WebExtensionWindowType> queryWindowType;
  switch (window_type) {
    case api::tabs::WindowType::kNormal:
      queryWindowType = WebExtensionWindowType::NORMAL;
    break;
    case api::tabs::WindowType::kPopup:
      queryWindowType = WebExtensionWindowType::POPUP;
    break;
    case api::tabs::WindowType::kPanel:
      queryWindowType = WebExtensionWindowType::PANEL;
    break;
    case api::tabs::WindowType::kApp:
      queryWindowType = WebExtensionWindowType::APP;
    break;
    case api::tabs::WindowType::kDevtools:
      queryWindowType = WebExtensionWindowType::DEVTOOLS;
    break;
    default:
    break;
  }
  return queryWindowType;
}

void GetQueryParams(
    std::optional<tabs::Query::Params>& params,
    NWebExtensionTabQueryInfo& queryInfo) {
  queryInfo.active = params->query_info.active;
  queryInfo.audible = params->query_info.audible;
  queryInfo.autoDiscardable = params->query_info.auto_discardable;
  queryInfo.currentWindow = params->query_info.current_window;

  queryInfo.discarded = params->query_info.discarded;

  queryInfo.groupId = params->query_info.group_id;
  queryInfo.highlighted = params->query_info.highlighted;
  queryInfo.index = params->query_info.index;
  queryInfo.lastFocusedWindow = params->query_info.last_focused_window;
  queryInfo.muted = params->query_info.muted;
  queryInfo.windowId = params->query_info.window_id;
  queryInfo.pinned = params->query_info.pinned;
  queryInfo.status = GetQueryStatus(params->query_info.status);
  queryInfo.title = params->query_info.title;
  if (params->query_info.url) {
    std::vector<std::string> urls;
    if (params->query_info.url->as_string) {
      urls.push_back(*params->query_info.url->as_string);
    } else if (params->query_info.url->as_strings) {
      urls.swap(*params->query_info.url->as_strings);
    }
    queryInfo.url = urls;
  } else {
    queryInfo.url = std::nullopt;
  }
  queryInfo.windowType = GetQueryWindowType(params->query_info.window_type);
}

void GetCreateParams(
    std::optional<tabs::Create::Params>& params,
    NWebTabCreateInfo& create_info) {
  if (params->create_properties.active) {
    create_info.active = params->create_properties.active.value();
  }

  if (params->create_properties.index) {
    create_info.index = params->create_properties.index.value();
  }

  if (params->create_properties.opener_tab_id) {
    create_info.openerTabId = params->create_properties.opener_tab_id.value();
  }

  if (params->create_properties.pinned) {
    create_info.pinned = params->create_properties.pinned.value();
  }

  if (params->create_properties.window_id) {
    create_info.windowId = params->create_properties.window_id.value();
  }
}

int GetAnyWebContents(int windowId, ExtensionFunction* function) {
  NWebExtensionTabGetAnyTabParams params = {
    .windowId = windowId,
    .contextType = GetExtensionContextType(function->browser_context()),
    .includeIncognitoInfo = function->include_incognito_information(),
  };

  int current_tab_id =
      OHOS::NWeb::NWebExtensionTabCefDelegate::GetAnyTab(params);
  LOG(INFO) << "GetAnyWebContents return " << current_tab_id
            << "; by windowId=" << windowId;
  return current_tab_id;
}

int GetCurrentWebContents(ExtensionFunction* function) {
  WebContents* web_contents = function->GetSenderWebContents();
  // 1. normal extension page
  if (web_contents && web_contents->ExtensionGetTabId() > 0) {
    return web_contents->ExtensionGetTabId();
  }

  // 2. service worker
  if (!web_contents) {
    return GetAnyWebContents(extension_misc::kCurrentWindowId, function);
  }

  // 3. v2 background, window_id < 0
  // 4. popup or sidepanel, window_id > 0
  int nweb_id = web_contents->GetNWebId();
  int window_id = 
      extensions::CefExtensionWindowIdManager::GetWindowId(nweb_id);
  return GetAnyWebContents(window_id, function);
}

WebContents* GetTabsAPIDefaultWebContents(ExtensionFunction* function,
                                          int tab_id,
                                          std::string* error) {
  WebContents* contents = nullptr;
  int current = tab_id;
  if (current < 0) {
    current = GetCurrentWebContents(function);
    if (current < 0) {
      LOG(INFO) << "cannot find WebContents by GetCurrentWebContents";
      if (error) {
        *error = ErrorUtils::FormatErrorMessage(
            ExtensionTabUtil::kTabNotFoundError, base::NumberToString(tab_id));
      }
      return nullptr;
    }
  }

  if (!ExtensionTabUtil::GetTabById(
      current, function->browser_context(),
      function->include_incognito_information(), &contents)) {
    if (error) {
      *error = ErrorUtils::FormatErrorMessage(
          ExtensionTabUtil::kTabNotFoundError, base::NumberToString(tab_id));
    }
  }

  return contents;
}

NWebExtensionTabContextInfo GetNWebExtensionTabContextInfo(
    content::BrowserContext* browser_context,
    bool includeIncognitoInfo) {
  NWebExtensionTabContextInfo context_info;
  context_info.contextType = GetExtensionContextType(browser_context);
  context_info.includeIncognitoInfo = includeIncognitoInfo;
 
  return context_info;
}

}  // namespace

ExtensionFunction::ResponseAction TabsCaptureVisibleTabFunction::Run() {
  using api::extension_types::ImageDetails;

  EXTENSION_FUNCTION_VALIDATE(has_args());
  int window_id = extension_misc::kCurrentWindowId;

  if (args().size() > 0 && args()[0].is_int()) {
    window_id = args()[0].GetInt();
  }

  std::optional<ImageDetails> image_details;
  if (args().size() > 1) {
    image_details = ImageDetails::FromValue(args()[1]);
  }

  int current = GetAnyWebContents(window_id, this);
  if (current < 0) {
    LOG(INFO) << "TabsCaptureVisibleTabFunction cannot find WebContents by window_id:" << window_id;
    return RespondNow(Error("No active web contents to capture"));
  }

  WebContents* contents = nullptr;
  if (!ExtensionTabUtil::GetTabById(
      current, browser_context(),
      include_incognito_information(), &contents) || !contents) {
    LOG(INFO) << "TabsCaptureVisibleTabFunction cannot find WebContents by tab_id:" << current;
    return RespondNow(Error("No active web contents to capture"));
  }
  
  std::string error;
  if (!extension()->permissions_data()->CanCaptureVisiblePage(
      contents->GetLastCommittedURL(),
      current, &error,
      extensions::CaptureRequirement::kActiveTabOrAllUrls)) {
    return RespondNow(Error(std::move(error)));
  }

  // NOTE: CaptureAsync() may invoke its callback from a background thread,
  // hence the BindPostTask().
  const CaptureResult capture_result = CaptureAsync(
      contents, base::OptionalToPtr(image_details),
      base::BindPostTaskToCurrentDefault(base::BindOnce(
          &TabsCaptureVisibleTabFunction::CopyFromSurfaceComplete, this)));
  if (capture_result == OK) {
    // CopyFromSurfaceComplete might have already responded.
    return did_respond() ? AlreadyResponded() : RespondLater();
  }

  return RespondNow(Error(CaptureResultToErrorMessage(capture_result)));
}

void TabsCreateFunction::OnTabCreated(const base::WeakPtr<TabsCreateFunction>& function,
                                      const NWebExtensionTab& tab,
                                      std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "OnTabCreated is empty!!!!";
    return;
  }
  if (tab.nwebId <= 0 || error) {
    std::string errorMessage = error? error.value() : "create error";
    function->Respond(function->Error(errorMessage));
  } else {
    if (function->has_callback()) {
      NWebExtensionTab tab_t = tab;
      GURL gurl(tab_t.url.value_or(""));
      ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior = ExtensionTabUtil::GetScrubTabBehaviorExt(
          function->extension(), function->source_context_type(), gurl, tab_t.id.value_or(-1));
      function->Respond(function->WithArguments(base::Value(GetTabValue(tab_t, scrub_tab_behavior))));
    } else {
      function->Respond(function->NoArguments());
    }
  }

  if (!function->call_create_tab_) {
    LOG(INFO) << "TabsCreateFunction Release";
    function->Release();
  }
}

void TabsCreateFunction::CreateTabForExtension(
    std::string& url,
    content::BrowserContext* context) {
  NWebTabCreateInfoV2 create_info;
  create_info.createInfo.url = url;
  create_info.contextType = GetExtensionContextType(context);
}

ExtensionFunction::ResponseAction TabsCreateFunction::Run() {
  std::optional<tabs::Create::Params> params =
      tabs::Create::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);
  NWebTabCreateInfoV2 create_info;
  GetCreateParams(params, create_info.createInfo);

  if (params->create_properties.url) {
    auto result = ExtensionTabUtil::PrepareURLForNavigation(
        *params->create_properties.url, extension(), browser_context());
    if (!result.has_value()) {
      return RespondNow(Error(result.error()));
    }
    create_info.createInfo.url = (*result).spec();
  }
  create_info.contextType = GetExtensionContextType(browser_context());
  create_info.includeIncognitoInfo = include_incognito_information();

  call_create_tab_ =true;
  bool success = OHOS::NWeb::NWebExtensionTabCefDelegate::CreateTab(
      create_info, base::BindRepeating(&TabsCreateFunction::OnTabCreated,
                                       weak_ptr_factory_.GetWeakPtr()));
  call_create_tab_ =false;

  if (did_respond()) {
    LOG(INFO) << "TabsCreateFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "TabsCreateFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support tabs.create"));
  }
}

ExtensionFunction::ResponseAction TabsDetectLanguageFunction::Run() {
  std::optional<tabs::DetectLanguage::Params> params =
      tabs::DetectLanguage::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  int tab_id = params->tab_id ? *params->tab_id : -1;
  std::string error;
  WebContents* web_contents = GetTabsAPIDefaultWebContents(this, tab_id, &error);
  if (!web_contents) {
    return RespondNow(Error(std::move(error)));
  }

  if (web_contents->GetController().NeedsReload()) {
    // If the tab hasn't been loaded, don't wait for the tab to load.
    return RespondNow(
        Error(kCannotDetermineLanguageOfUnloadedTab));
  }

  CefRefPtr<AlloyBrowserHostImpl> alloy_browser_host =
      AlloyBrowserHostImpl::GetBrowserForContents(web_contents);
  
  std::string language = alloy_browser_host->GetCurrentLanguage();
  if (language.empty()) {
    return RespondNow(
        Error("failed to get language of tab"));
  }

  return RespondNow(WithArguments(language));
}

ExtensionFunction::ResponseAction TabsDiscardFunction::Run() {
  std::optional<tabs::Discard::Params> params =
      tabs::Discard::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);
  int tab_id = params->tab_id ? *params->tab_id : -1;
  call_discard_tab_ = true;
  NWebExtensionTabDiscardInfo discard_info = GetNWebExtensionTabContextInfo(
      browser_context(), include_incognito_information());
  bool success = OHOS::NWeb::NWebExtensionTabCefDelegate::DiscardTab(
      tab_id, discard_info, base::BindRepeating(&TabsDiscardFunction::OnTabDiscarded,
                                  weak_ptr_factory_.GetWeakPtr()));
  call_discard_tab_ = false;

  if (did_respond()) {
    LOG(INFO) << "TabsDiscardFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "TabsDiscardFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support tabs.discard"));
  }
}

void TabsDiscardFunction::OnTabDiscarded(
    const base::WeakPtr<TabsDiscardFunction>& function,
    NWebExtensionTab& tab,
    std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "OnTabDiscarded is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(error.value()));
  } else {
    if (function->has_callback()) {
      GURL gurl(tab.url.value_or(""));
      ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior =
          ExtensionTabUtil::GetScrubTabBehaviorExt(
              function->extension(), function->source_context_type(), gurl, tab.id.value_or(-1));
      function->Respond(function->WithArguments(base::Value(GetTabValue(tab, scrub_tab_behavior))));
    } else {
      function->Respond(function->NoArguments());
    }
  }

  if (!function->call_discard_tab_) {
    LOG(INFO) << "TabsDiscardFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction TabsDuplicateFunction::Run() {
  std::optional<tabs::Duplicate::Params> params =
      tabs::Duplicate::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);
  call_duplicate_tab_ = true;
  NWebExtensionTabDuplicateInfo duplicate_info = GetNWebExtensionTabContextInfo(
      browser_context(), include_incognito_information());
  bool success = OHOS::NWeb::NWebExtensionTabCefDelegate::DuplicateTab(
      params->tab_id, duplicate_info, base::BindRepeating(&TabsDuplicateFunction::OnTabDuplicated,
                                  weak_ptr_factory_.GetWeakPtr()));
  call_duplicate_tab_ = false;

  if (did_respond()) {
    LOG(INFO) << "TabsDuplicateFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "TabsDuplicateFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support tabs.duplicate"));
  } 
}

void TabsDuplicateFunction::OnTabDuplicated(
    const base::WeakPtr<TabsDuplicateFunction>& function,
    NWebExtensionTab& tab,
    std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "OnTabDuplicated is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(error.value()));
  } else {
    if (function->has_callback()) {
      GURL gurl(tab.url.value_or(""));
      ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior =
          ExtensionTabUtil::GetScrubTabBehaviorExt(
              function->extension(), function->source_context_type(), gurl, tab.id.value_or(-1));
      function->Respond(function->WithArguments(base::Value(GetTabValue(tab, scrub_tab_behavior))));
    } else {
      function->Respond(function->NoArguments());
    }
  }

  if (!function->call_duplicate_tab_) {
    LOG(INFO) << "TabsDuplicateFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction TabsGetFunction::Run() {
  LOG(DEBUG) << "TabsGetFunction Run";
  std::optional<tabs::Get::Params> params =
      tabs::Get::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);
  int tab_id = params->tab_id;
  std::unique_ptr<NWebExtensionTab> web_extension_tab =
      OHOS::NWeb::NWebExtensionTabCefDelegate::GetTab(
          tab_id, GetExtensionContextType(browser_context()),
          include_incognito_information());
  if (!web_extension_tab) {
    return RespondNow(Error(kNotImplementedError));
  }

  GURL gurl(web_extension_tab->url.value_or(""));
  ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior = ExtensionTabUtil::GetScrubTabBehaviorExt(
      extension(), source_context_type(), gurl, web_extension_tab->id.value_or(-1));

  base::Value::List get_results;
  get_results.reserve(1);
  get_results.Append(GetTabValue(*web_extension_tab, scrub_tab_behavior));

  return RespondNow(ArgumentList(std::move(get_results)));
}

ExtensionFunction::ResponseAction TabsGetCurrentFunction::Run() {
  // Return the caller, if it's a tab. If not the result isn't an error but an
  // empty tab (hence returning true).
  WebContents* caller_contents = GetSenderWebContents();
  if (caller_contents && caller_contents->ExtensionGetTabId() > 0) {
    std::unique_ptr<NWebExtensionTab> tab =
        OHOS::NWeb::NWebExtensionTabCefDelegate::GetTab(caller_contents->ExtensionGetTabId());
    ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior =
        ExtensionTabUtil::GetScrubTabBehavior(extension(), source_context_type(), caller_contents);
 
    return RespondNow(WithArguments(base::Value(GetTabValue(*tab, scrub_tab_behavior))));
  }
  return RespondNow(NoArguments());
}

ExtensionFunction::ResponseAction TabsGetZoomFunction::Run() {
  std::optional<tabs::GetZoom::Params> params =
      tabs::GetZoom::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  int tab_id = params->tab_id ? *params->tab_id : -1;
  std::string error;
  WebContents* web_contents =
      GetTabsAPIDefaultWebContents(this, tab_id, &error);
  if (!web_contents) {
    return RespondNow(Error(std::move(error)));
  }

  double zoom_level =
      ZoomController::FromWebContents(web_contents)->GetZoomLevel();
  double zoom_factor = blink::ZoomLevelToZoomFactor(zoom_level);

  return RespondNow(ArgumentList(tabs::GetZoom::Results::Create(zoom_factor)));
}

ExtensionFunction::ResponseAction TabsGetZoomSettingsFunction::Run() {
  std::optional<tabs::GetZoomSettings::Params> params =
      tabs::GetZoomSettings::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  int tab_id = params->tab_id ? *params->tab_id : -1;
  std::string error;
  WebContents* web_contents =
      GetTabsAPIDefaultWebContents(this, tab_id, &error);
  if (!web_contents) {
    return RespondNow(Error(std::move(error)));
  }
  ZoomController* zoom_controller =
      ZoomController::FromWebContents(web_contents);

  ZoomController::ZoomMode zoom_mode = zoom_controller->zoom_mode();
  api::tabs::ZoomSettings zoom_settings;
  ZoomModeToZoomSettings(zoom_mode, &zoom_settings);
  zoom_settings.default_zoom_factor =
      blink::ZoomLevelToZoomFactor(zoom_controller->GetDefaultZoomLevel());

  return RespondNow(
      ArgumentList(api::tabs::GetZoomSettings::Results::Create(zoom_settings)));
}

ExtensionFunction::ResponseAction TabsGoBackFunction::Run() {
  std::optional<tabs::GoBack::Params> params =
      tabs::GoBack::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  int tab_id = params->tab_id ? *params->tab_id : -1;
  std::string error;
  WebContents* web_contents =
      GetTabsAPIDefaultWebContents(this, tab_id, &error);
  if (!web_contents) {
    return RespondNow(Error(std::move(error)));
  }
  NavigationController& controller = web_contents->GetController();
  if (!controller.CanGoBack()) {
    return RespondNow(Error(tabs_constants::kNotFoundNextPageError));
  }

  controller.GoBack();
  return RespondNow(NoArguments());
}

ExtensionFunction::ResponseAction TabsGoForwardFunction::Run() {
  std::optional<tabs::GoForward::Params> params =
      tabs::GoForward::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  int tab_id = params->tab_id ? *params->tab_id : -1;
  std::string error;
  WebContents* web_contents =
      GetTabsAPIDefaultWebContents(this, tab_id, &error);
  if (!web_contents) {
    return RespondNow(Error(std::move(error)));
  }

  NavigationController& controller = web_contents->GetController();
  if (!controller.CanGoForward()) {
    return RespondNow(Error(tabs_constants::kNotFoundNextPageError));
  }

  controller.GoForward();
  return RespondNow(NoArguments());
}

ExtensionFunction::ResponseAction TabsGroupFunction::Run() {
  std::optional<tabs::Group::Params> params =
      tabs::Group::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  NWebExtensionTabGroupOptions options;
  if (params->options.group_id) {
    if (params->options.create_properties) {
      return RespondNow(Error(tabs_constants::kGroupParamsError));
    } else {
      options.groupId = params->options.group_id;
    }
  }

  NWebExtensionTabCreateProperties properties;
  if (params->options.create_properties && params->options.create_properties.value().window_id) {
    properties.windowId = params->options.create_properties.value().window_id.value();
  }
  properties.contextType = GetExtensionContextType(browser_context());
  properties.includeIncognitoInfo = include_incognito_information();
  options.createProperties = properties;

  // Get all tab IDs from parameters.
  if (params->options.tab_ids.as_integers) {
    options.tabIds = *params->options.tab_ids.as_integers;
    EXTENSION_FUNCTION_VALIDATE(!options.tabIds.empty());
  } else {
    EXTENSION_FUNCTION_VALIDATE(params->options.tab_ids.as_integer);
    options.tabIds.push_back(*params->options.tab_ids.as_integer);
  }

  call_group_tab_ = true;
  bool success = OHOS::NWeb::NWebExtensionTabCefDelegate::GroupTab(
      options, base::BindRepeating(&TabsGroupFunction::OnTabGrouped,
                                   weak_ptr_factory_.GetWeakPtr()));
  call_group_tab_ = false;

  if (did_respond()) {
    LOG(INFO) << "TabsGroupFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "TabsGroupFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support tabs.group"));
  }
}

void TabsGroupFunction::OnTabGrouped(
    const base::WeakPtr<TabsGroupFunction>& function,
    int group_id,
    std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "OnTabGrouped is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(error.value()));
  } else {
    function->Respond(function->has_callback()
        ? function->WithArguments(group_id)
        : function->NoArguments());
  }

  if (!function->call_group_tab_) {
    LOG(INFO) << "TabsGroupFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction TabsHighlightFunction::Run() {
  std::optional<tabs::Highlight::Params> params =
      tabs::Highlight::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  NWebExtensionTabHighlightInfo info;
  if (params->highlight_info.tabs.as_integers) {
    info.tabIds = *params->highlight_info.tabs.as_integers;
    EXTENSION_FUNCTION_VALIDATE(!info.tabIds.empty());
  } else {
    EXTENSION_FUNCTION_VALIDATE(params->highlight_info.tabs.as_integer);
    info.tabIds.push_back(*params->highlight_info.tabs.as_integer);
  }

  if (params->highlight_info.window_id) {
    info.windowId = params->highlight_info.window_id.value();
  }

  info.contextType = GetExtensionContextType(browser_context());
  info.includeIncognitoInfo = include_incognito_information();

  call_highlight_tab_ = true;
  bool success = OHOS::NWeb::NWebExtensionTabCefDelegate::HighlightTab(
      info, base::BindRepeating(&TabsHighlightFunction::OnTabHighlighted,
                                weak_ptr_factory_.GetWeakPtr()));
  call_highlight_tab_ = false;

  if (did_respond()) {
    LOG(INFO) << "TabsHighlightFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "TabsHighlightFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support tabs.highlight"));
  }
}

void TabsHighlightFunction::OnTabHighlighted(
    const base::WeakPtr<TabsHighlightFunction>& function,
    WebExtensionWindow& window,
    std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "OnTabHighlighted is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(error.value()));
  } else {
    std::vector<ExtensionTabUtil::ScrubTabBehavior> scrub_tab_behaviors;
    for (NWebExtensionTab tab : window.tabs) {
      GURL gurl(tab.url.value_or(""));
      ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior = ExtensionTabUtil::GetScrubTabBehaviorExt(
          function->extension(), function->source_context_type(), gurl, tab.id.value_or(-1));
      scrub_tab_behaviors.emplace_back(scrub_tab_behavior);
    }
    function->Respond(function->has_callback()
        ? function->WithArguments(GetWindowValue(window, scrub_tab_behaviors))
        : function->NoArguments());
  }

  if (!function->call_highlight_tab_) {
    LOG(INFO) << "TabsHighlightFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction TabsMoveFunction::Run() {
  std::optional<tabs::Move::Params> params =
      tabs::Move::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);
  std::vector<int> tab_ids;

  if (params->tab_ids.as_integers) {
    for (int tab_id : *params->tab_ids.as_integers) {
      tab_ids.emplace_back(tab_id);
    }
  } else {
    EXTENSION_FUNCTION_VALIDATE(params->tab_ids.as_integer);
    tab_ids.emplace_back(*params->tab_ids.as_integer);
  }

  NWebExtensionTabMovePropertiesV2 movePropertiesV2;
  movePropertiesV2.properties.index = params->move_properties.index;
  movePropertiesV2.properties.windowId = params->move_properties.window_id;
  movePropertiesV2.contextType = GetExtensionContextType(browser_context());
  movePropertiesV2.includeIncognitoInfo = include_incognito_information();

  call_move_tab_ = true;
  bool success = OHOS::NWeb::NWebExtensionTabCefDelegate::MoveTab(
      tab_ids, movePropertiesV2,
      base::BindRepeating(&TabsMoveFunction::OnTabMoved,
                          weak_ptr_factory_.GetWeakPtr()));
  call_move_tab_ = false;

  if (did_respond()) {
    LOG(INFO) << "TabsMoveFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "TabsMoveFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support tabs.move"));
  }
}

void TabsMoveFunction::OnTabMoved(
    const base::WeakPtr<TabsMoveFunction>& function,
    const std::vector<NWebExtensionTab>& tabs,
    std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "OnTabMoved is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(error.value()));
  } else if (!function->has_callback()) {
    function->Respond(function->NoArguments());
  } else if (tabs.size() == 1) {
    std::vector<NWebExtensionTab> tabs_t = tabs;
    GURL gurl(tabs_t[0].url.value_or(""));
    ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior =
        ExtensionTabUtil::GetScrubTabBehaviorExt(
            function->extension(), function->source_context_type(), gurl, tabs_t[0].id.value_or(-1));
    function->Respond(function->WithArguments(base::Value(GetTabValue(tabs_t[0], scrub_tab_behavior))));
  } else {
    std::vector<ExtensionTabUtil::ScrubTabBehavior> scrub_tab_behaviors;
    for (NWebExtensionTab tab : tabs) {
      GURL gurl(tab.url.value_or(""));
      ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior =
          ExtensionTabUtil::GetScrubTabBehaviorExt(
              function->extension(), function->source_context_type(), gurl, tab.id.value_or(-1));
      scrub_tab_behaviors.emplace_back(scrub_tab_behavior);
    }
    base::Value::List tab_list = GetTabValueList(tabs, scrub_tab_behaviors);
    function->Respond(function->WithArguments(std::move(tab_list)));
  }

  if (!function->call_move_tab_) {
    LOG(INFO) << "TabsMoveFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction TabsQueryFunction::Run() {
  std::optional<tabs::Query::Params> params =
      tabs::Query::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);
  LOG(DEBUG) << "TabsQueryFunction Run";

  NWebExtensionTabQueryInfoV2 queryInfo;
  WebContents* webcontents = GetSenderWebContents();
  SetQueryInfoCurrentWindowId(webcontents, queryInfo.query);
  GetQueryParams(params, queryInfo.query);

  URLPatternSet url_patterns;
  if (queryInfo.query.url) {
    std::string error;
    if (!url_patterns.Populate(*queryInfo.query.url, URLPattern::SCHEME_ALL,
                               true, &error)) {
      return RespondNow(Error(std::move(error)));
    }
  }

  queryInfo.contextType = GetExtensionContextType(browser_context());
  queryInfo.includeIncognitoInfo = include_incognito_information();

  std::vector<NWebExtensionTab> tabs =
      OHOS::NWeb::NWebExtensionTabCefDelegate::QueryTab(queryInfo);

  std::vector<ExtensionTabUtil::ScrubTabBehavior> scrub_tab_behaviors;
  for (NWebExtensionTab tab : tabs) {
    GURL gurl(tab.url.value_or(""));
    ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior =
        ExtensionTabUtil::GetScrubTabBehaviorExt(extension(), source_context_type(), gurl, tab.id.value_or(-1));
    scrub_tab_behaviors.emplace_back(scrub_tab_behavior);
  }
 
  base::Value::List tab_list = GetTabValueList(tabs, scrub_tab_behaviors);
  return RespondNow(WithArguments(std::move(tab_list)));
}

ExtensionFunction::ResponseAction TabsGetSelectedFunction::Run() {
  std::optional<tabs::GetSelected::Params> params =
      tabs::GetSelected::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  LOG(DEBUG) << "TabsGetSelectedFunction Run";

  NWebExtensionTabQueryInfoV2 query_info;
  query_info.contextType = GetExtensionContextType(browser_context());
  query_info.includeIncognitoInfo = include_incognito_information();

  WebContents* web_contents = GetSenderWebContents();
  query_info.query.active = true;
  query_info.query.windowId = params->window_id;
  if (!query_info.query.windowId) {
    SetQueryInfoCurrentWindowId(web_contents, query_info.query);
    query_info.query.currentWindow = true;
  }

  std::vector<NWebExtensionTab> tabs =
      OHOS::NWeb::NWebExtensionTabCefDelegate::QueryTab(query_info);
  if (tabs.size() != 1) {
    LOG(ERROR) << "invalid number of getSelected result tabs: " << tabs.size();
    return RespondNow(
        Error("getSelected tab failed: invalid number of result tabs"));
  }

  GURL gurl(tabs[0].url.value_or(""));
  ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior =
      ExtensionTabUtil::GetScrubTabBehaviorExt(extension(), source_context_type(), gurl, tabs[0].id.value_or(-1));
 
  return RespondNow(WithArguments(GetTabValue(tabs[0], scrub_tab_behavior)));
}

ExtensionFunction::ResponseAction TabsReloadFunction::Run() {
  std::optional<tabs::Reload::Params> params =
      tabs::Reload::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  bool bypass_cache = false;
  if (params->reload_properties && params->reload_properties->bypass_cache) {
    bypass_cache = *params->reload_properties->bypass_cache;
  }

  int tab_id = params->tab_id ? *params->tab_id : -1;
  std::string error;
  WebContents* web_contents =
      GetTabsAPIDefaultWebContents(this, tab_id, &error);
  if (!web_contents) {
    return RespondNow(Error(std::move(error)));
  }

  web_contents->GetController().Reload(
      bypass_cache ? content::ReloadType::BYPASSING_CACHE
                   : content::ReloadType::NORMAL,
      true);

  return RespondNow(NoArguments());
}

ExtensionFunction::ResponseAction TabsRemoveFunction::Run() {
  std::optional<tabs::Remove::Params> params =
      tabs::Remove::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);
  std::vector<int> tab_ids;

  if (params->tab_ids.as_integers) {
    for (int tab_id : *params->tab_ids.as_integers) {
      tab_ids.emplace_back(tab_id);
    }
  } else {
    EXTENSION_FUNCTION_VALIDATE(params->tab_ids.as_integer);
    tab_ids.emplace_back(*params->tab_ids.as_integer);
  }

  NWebExtensionTabRemoveParams nwebParams;
  nwebParams.tabIds = tab_ids;
  nwebParams.contextType = GetExtensionContextType(browser_context());
  nwebParams.includeIncognitoInfo = include_incognito_information();

  call_remove_tab_ = true;
  bool success = OHOS::NWeb::NWebExtensionTabCefDelegate::RemoveTab(
      nwebParams, base::BindRepeating(&TabsRemoveFunction::OnTabRemoved,
                                      weak_ptr_factory_.GetWeakPtr()));
  call_remove_tab_ = false;

  if (did_respond()) {
    LOG(INFO) << "TabsRemoveFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "TabsRemoveFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support tabs.remove"));
  }
}

void TabsRemoveFunction::OnTabRemoved(
    const base::WeakPtr<TabsRemoveFunction>& function,
    std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "OnTabRemoved is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(error.value()));
  } else {
    function->Respond(function->NoArguments());
  }

  if (!function->call_remove_tab_) {
    LOG(INFO) << "TabsRemoveFunction Release";
    function->Release();
  }
}

ExtensionFunction::ResponseAction TabsSetZoomFunction::Run() {
  std::optional<tabs::SetZoom::Params> params =
      tabs::SetZoom::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  int tab_id = params->tab_id ? *params->tab_id : -1;
  std::string error;
  WebContents* web_contents =
      GetTabsAPIDefaultWebContents(this, tab_id, &error);
  if (!web_contents) {
    return RespondNow(Error(std::move(error)));
  }

  GURL url(web_contents->GetVisibleURL());
  if (extension()->permissions_data()->IsRestrictedUrl(url, &error)) {
    return RespondNow(Error(std::move(error)));
  }

  ZoomController* zoom_controller =
      ZoomController::FromWebContents(web_contents);
  double zoom_level = params->zoom_factor > 0
                          ? blink::ZoomFactorToZoomLevel(params->zoom_factor)
                          : zoom_controller->GetDefaultZoomLevel();

  auto client = base::MakeRefCounted<ExtensionZoomRequestClient>(extension());
  if (!zoom_controller->SetZoomLevelByClient(zoom_level, client)) {
    // Tried to zoom a tab in disabled mode.
    return RespondNow(Error(tabs_constants::kCannotZoomDisabledTabError));
  }

  return RespondNow(NoArguments());
}

ExtensionFunction::ResponseAction TabsSetZoomSettingsFunction::Run() {
  using api::tabs::ZoomSettings;

  std::optional<tabs::SetZoomSettings::Params> params =
      tabs::SetZoomSettings::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  int tab_id = params->tab_id ? *params->tab_id : -1;
  std::string error;
  WebContents* web_contents =
      GetTabsAPIDefaultWebContents(this, tab_id, &error);
  if (!web_contents) {
    return RespondNow(Error(std::move(error)));
  }

  GURL url(web_contents->GetVisibleURL());
  if (extension()->permissions_data()->IsRestrictedUrl(url, &error)) {
    return RespondNow(Error(std::move(error)));
  }

  // "per-origin" scope is only available in "automatic" mode.
  if (params->zoom_settings.scope == tabs::ZoomSettingsScope::kPerOrigin &&
      params->zoom_settings.mode != tabs::ZoomSettingsMode::kAutomatic &&
      params->zoom_settings.mode != tabs::ZoomSettingsMode::kNone) {
    return RespondNow(Error(tabs_constants::kPerOriginOnlyInAutomaticError));
  }

  // Determine the correct internal zoom mode to set |web_contents| to from the
  // user-specified |zoom_settings|.
  ZoomController::ZoomMode zoom_mode = ZoomController::ZOOM_MODE_DEFAULT;
  switch (params->zoom_settings.mode) {
    case tabs::ZoomSettingsMode::kNone:
    case tabs::ZoomSettingsMode::kAutomatic:
      switch (params->zoom_settings.scope) {
        case tabs::ZoomSettingsScope::kNone:
        case tabs::ZoomSettingsScope::kPerOrigin:
          zoom_mode = ZoomController::ZOOM_MODE_DEFAULT;
          break;
        case tabs::ZoomSettingsScope::kPerTab:
          zoom_mode = ZoomController::ZOOM_MODE_ISOLATED;
      }
      break;
    case tabs::ZoomSettingsMode::kManual:
      zoom_mode = ZoomController::ZOOM_MODE_MANUAL;
      break;
    case tabs::ZoomSettingsMode::kDisabled:
      zoom_mode = ZoomController::ZOOM_MODE_DISABLED;
  }

  ZoomController::FromWebContents(web_contents)->SetZoomMode(zoom_mode);

  return RespondNow(NoArguments());
}

ExtensionFunction::ResponseAction TabsUngroupFunction::Run() {
  std::optional<tabs::Ungroup::Params> params =
      tabs::Ungroup::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  std::vector<int> tab_ids;
  if (params->tab_ids.as_integers) {
    tab_ids = *params->tab_ids.as_integers;
    EXTENSION_FUNCTION_VALIDATE(!tab_ids.empty());
  } else {
    EXTENSION_FUNCTION_VALIDATE(params->tab_ids.as_integer);
    tab_ids.push_back(*params->tab_ids.as_integer);
  }

  NWebExtensionTabUngroupParams nwebParams;
  nwebParams.tabIds = tab_ids;
  nwebParams.contextType = GetExtensionContextType(browser_context());
  nwebParams.includeIncognitoInfo = include_incognito_information();

  call_ungroup_tab_ = true;
  bool success = OHOS::NWeb::NWebExtensionTabCefDelegate::UngroupTab(
      nwebParams, base::BindRepeating(&TabsUngroupFunction::OnTabUngrouped,
                                      weak_ptr_factory_.GetWeakPtr()));
  call_ungroup_tab_ = false;

  if (did_respond()) {
    LOG(INFO) << "TabsUngroupFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "TabsUngroupFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support tabs.ungroup"));
  }
}

void TabsUngroupFunction::OnTabUngrouped(
    const base::WeakPtr<TabsUngroupFunction>& function,
    std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "OnTabUngrouped is empty!!!!";
    return;
  }

  if (error) {
    function->Respond(function->Error(error.value()));
  } else {
    function->Respond(function->NoArguments());
  }

  if (!function->call_ungroup_tab_) {
    LOG(INFO) << "TabsUngroupFunction Release";
    function->Release();
  }
}

void TabsUpdateFunction::OnTabUpdated(
    const base::WeakPtr<TabsUpdateFunction>& function,
    NWebExtensionTab& tab,
    std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "OnTabUpdated is empty!!!!";
    return;
  }

  if (tab.nwebId <= 0 || error) {
    std::string errorMessage = error ? error.value() : "update error";
    function->Respond(function->Error(errorMessage));
  } else {
    if (function->has_callback()) {
      GURL gurl(tab.url.value_or(""));
      ExtensionTabUtil::ScrubTabBehavior scrub_tab_behavior = ExtensionTabUtil::GetScrubTabBehaviorExt(
          function->extension(), function->source_context_type(), gurl, tab.id.value_or(-1));
      function->Respond(function->WithArguments(base::Value(GetTabValue(tab, scrub_tab_behavior))));
    } else {
      function->Respond(function->NoArguments());
    }
  }

  if (!function->call_update_tab_) {
    LOG(INFO) << "TabsUpdateFunction Release";
    function->Release();
  }
}

bool TabsUpdateFunction::GetUpdateParams(
    int tab_id,
    std::optional<api::tabs::Update::Params>& params,
    NWebExtensionTabUpdateProperties& update_properties) {
  if (params->update_properties.url.has_value()) {
    GURL url;
    auto url_expected = ExtensionTabUtil::PrepareURLForNavigation(
        *params->update_properties.url, extension(), browser_context());
    if (!url_expected.has_value()) {
      error_ = std::move(url_expected.error());
      return false;
    }
    url = *url_expected;

    const bool is_javascript_scheme = url.SchemeIs(url::kJavaScriptScheme);
    // JavaScript URLs are forbidden in chrome.tabs.update().
    if (is_javascript_scheme) {
      error_ = kJavaScriptUrlsNotAllowedInTabsUpdate;
      return false;
    }
    update_properties.url = url.spec();
  }

  if (params->update_properties.active.has_value()) {
    update_properties.active = *params->update_properties.active;
  }

  if (params->update_properties.highlighted.has_value()) {
    update_properties.highlighted = *params->update_properties.highlighted;
  }

  if (params->update_properties.pinned.has_value()) {
    update_properties.pinned = *params->update_properties.pinned;
  }

  if (params->update_properties.muted.has_value()) {
    update_properties.muted = *params->update_properties.muted;
  }

  if (params->update_properties.opener_tab_id.has_value()) {
    int opener_id = *params->update_properties.opener_tab_id;
    if (opener_id == tab_id) {
      error_ = kSetOpenerError;
      return false;
    }

    update_properties.openerTabId = *params->update_properties.opener_tab_id;
  }

  if (params->update_properties.auto_discardable.has_value()) {
    update_properties.autoDiscardable = *params->update_properties.auto_discardable;
  }

  return true;
}

ExtensionFunction::ResponseAction TabsUpdateFunction::Run() {
  std::optional<tabs::Update::Params> params =
      tabs::Update::Params::Create(args());
  EXTENSION_FUNCTION_VALIDATE(params);

  int tab_id = params->tab_id ? *params->tab_id : -1;
  std::string error;
  web_contents_ = GetTabsAPIDefaultWebContents(this, tab_id, &error);
  if (!web_contents_) {
    return RespondNow(Error(error));
  }

  // compatible with phone
  if (!OHOS::NWeb::NWebExtensionTabCefDelegate::HasExtensionListener()) {
    if (params->update_properties.url.has_value()) {
      std::string updated_url = *params->update_properties.url;
      if (!UpdateURL(updated_url, tab_id, &error_)) {
        return RespondNow(Error(error));
      }
    }
    return RespondNow(GetResult());
  }

  NWebExtensionTabUpdatePropertiesV2 update_properties;
  if (!GetUpdateParams(tab_id, params, update_properties.updateProperties)) {
    return RespondNow(Error(error_));
  }
  update_properties.contextType = GetExtensionContextType(browser_context());
  update_properties.includeIncognitoInfo = include_incognito_information();

  call_update_tab_ = true;
  bool success = OHOS::NWeb::NWebExtensionTabCefDelegate::UpdateTab(
      web_contents_->ExtensionGetTabId(), update_properties, base::BindRepeating(&TabsUpdateFunction::OnTabUpdated,
          weak_ptr_factory_.GetWeakPtr()));
  call_update_tab_ = false;

  if (did_respond()) {
    LOG(INFO) << "TabsUpdateFunction did_respond";
    return AlreadyResponded();
  }

  if (success) {
    AddRef();
    LOG(INFO) << "TabsUpdateFunction AddRef";
    return RespondLater();
  } else {
    return RespondNow(Error("not support tabs.update"));
  }
}

ExtensionFunction::ResponseValue TabsUpdateFunction::GetResult() {
  if (!has_callback()) {
    return NoArguments();
  }

  return ArgumentList(tabs::Get::Results::Create(
      CefExtensionFunctionDetails::CreateTabObject(
          AlloyBrowserHostImpl::GetBrowserForContents(web_contents_),
          -1, true, web_contents_->ExtensionGetTabId())));
}

ExecuteCodeFunction::InitResult ExecuteCodeInTabFunction::Init() {
  if (init_result_) {
    return init_result_.value();
  }

  if (args().size() < 2) {
    return set_init_result(VALIDATION_FAILURE);
  }

  const auto& tab_id_value = args()[0];
  // |tab_id| is optional so it's ok if it's not there.
  int tab_id = -1;
  if (tab_id_value.is_int()) {
    // But if it is present, it needs to be non-negative.
    tab_id = tab_id_value.GetInt();
    if (tab_id < 0) {
      return set_init_result(VALIDATION_FAILURE);
    }
  }

  // |details| are not optional.
  const base::Value& details_value = args()[1];
  if (!details_value.is_dict()) {
    return set_init_result(VALIDATION_FAILURE);
  }
  auto details =
      api::extension_types::InjectDetails::FromValue(details_value.GetDict());
  if (!details) {
    return set_init_result(VALIDATION_FAILURE);
  }

  // If the tab ID wasn't given then it needs to be converted to the
  // currently active tab's ID.
  if (tab_id == -1) {
    tab_id = GetCurrentWebContents(this);
  }

  execute_tab_id_ = tab_id;
  details_ = std::move(details);
  set_host_id(
      mojom::HostID(mojom::HostID::HostType::kExtensions, extension()->id()));
  return set_init_result(SUCCESS);
}

}  // namespace extensions
