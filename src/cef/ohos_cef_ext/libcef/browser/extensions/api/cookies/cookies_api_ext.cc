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

#include "chrome/browser/extensions/api/cookies/cookies_api.h"
#include "chrome/browser/extensions/api/cookies/cookies_helpers.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_tab_cef_delegate.h"
#include "chrome/browser/profiles/profile.h"

namespace extensions {
namespace {
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
}

ExtensionFunction::ResponseAction CookiesGetAllCookieStoresFunction::Run() {
  Profile* original_profile = Profile::FromBrowserContext(browser_context());
  DCHECK(original_profile);
  base::Value::List original_tab_ids;
  Profile* incognito_profile = nullptr;
  base::Value::List incognito_tab_ids;
  const bool include_incognito = include_incognito_information();
  if (include_incognito && original_profile->HasPrimaryOTRProfile()) {
    incognito_profile =
        original_profile->GetPrimaryOTRProfile(/*create_if_needed=*/true);
  }
  DCHECK(original_profile != incognito_profile);

  NWebExtensionTabQueryInfoV2 queryInfo;
  queryInfo.contextType = GetExtensionContextType(browser_context());
  queryInfo.includeIncognitoInfo = include_incognito_information();
  std::vector<NWebExtensionTab> tabs = OHOS::NWeb::NWebExtensionTabCefDelegate::QueryTab(queryInfo);
  for (const NWebExtensionTab& tab : tabs) {
    if (!tab.id.has_value()) {
      continue;
    }
    if (tab.incognito) {
      if (include_incognito) {
        incognito_tab_ids.Append(tab.id.value());
      }
      continue;
    }
    original_tab_ids.Append(tab.id.value());
  }
  // Return a list of all cookie stores with at least one open tab.
  std::vector<api::cookies::CookieStore> cookie_stores;
  if (!original_tab_ids.empty()) {
    cookie_stores.push_back(cookies_helpers::CreateCookieStore(
        original_profile, std::move(original_tab_ids)));
  }
  if (include_incognito && incognito_profile && !incognito_tab_ids.empty()) {
    cookie_stores.push_back(cookies_helpers::CreateCookieStore(
        incognito_profile, std::move(incognito_tab_ids)));
  }
  return RespondNow(ArgumentList(
      api::cookies::GetAllCookieStores::Results::Create(cookie_stores)));
}

}  // namespace extensions
