/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#include "extensions/browser/pref_names.h"
#include "extensions/browser/renderer_startup_helper.h"
#include "extensions/common/extension_urls.h"
 
namespace extensions {
 
namespace util {
 
void SetWebStoreConfig(Profile* profile,
                       const extension_urls::WebStoreConfig& config) {
  if (!extension_urls::SetWebStoreConfig(config)) {
    return;
  }
 
  LOG(INFO) << "save webstore config to prefs";
 
  base::Value::Dict dict;
  dict.Set(pref_names::kWebStoreConfigApiUrl, config.api_url);
  dict.Set(pref_names::kWebStoreConfigLaunchUrl, config.launch_url);
  dict.Set(pref_names::kWebStoreConfigUpdateUrl, config.update_url);
  dict.Set(pref_names::kWebStoreConfigVerifyUrl, config.verify_url);
  dict.Set(pref_names::kWebStoreConfigHomePageUrl, config.home_page_url);
  profile->GetPrefs()->SetDict(pref_names::kWebStoreConfig, std::move(dict));
 
  RendererStartupHelperFactory::GetForBrowserContext(profile)
      ->OnWebStoreConfigChanged(config);
}
 
void LoadWebStoreConfig(Profile* profile) {
  const base::Value::Dict& dict =
      profile->GetPrefs()->GetDict(pref_names::kWebStoreConfig);
 
  LOG(INFO) << "load webstore config from prefs";
 
  extension_urls::WebStoreConfig config;
  auto api_url = dict.FindString(pref_names::kWebStoreConfigApiUrl);
  if (api_url && !api_url->empty()) {
    config.api_url = *api_url;
  }
 
  auto launch_url = dict.FindString(pref_names::kWebStoreConfigLaunchUrl);
  if (launch_url && !launch_url->empty()) {
    config.launch_url = *launch_url;
  }
 
  auto update_url = dict.FindString(pref_names::kWebStoreConfigUpdateUrl);
  if (update_url && !update_url->empty()) {
    config.update_url = *update_url;
  }
 
  auto verify_url = dict.FindString(pref_names::kWebStoreConfigVerifyUrl);
  if (verify_url && !verify_url->empty()) {
    config.verify_url = *verify_url;
  }
 
  auto home_page_url = dict.FindString(pref_names::kWebStoreConfigHomePageUrl);
  if (home_page_url && !home_page_url->empty()) {
    config.home_page_url = *home_page_url;
  }
 
  SetWebStoreConfig(profile, config);
}
 
}  // namespace util
 
}  // namespace extensions