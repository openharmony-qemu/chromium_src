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
 
#include "base/logging.h"
#include "extensions/common/extension_urls.h"
#include "extensions/common/extensions_client.h"
#include "extensions/common/features/feature_provider.h"
 
namespace extension_urls {
 
namespace {
 
bool g_web_store_enable = false;
WebStoreConfig g_web_store_config;
 
}  // namespace
 
const char kCheckCrxPath[] = "/hw/update2json";
const char kDownloadCrxPath[] = "/hw/detail";
 
bool IsWebStoreEnable() {
  return g_web_store_enable;
}
 
WebStoreConfig GetWebStoreConfig() {
  return g_web_store_config;
}
 
bool SetWebStoreConfig(const WebStoreConfig& web_store_config) {
  bool is_changed = false;
  if (web_store_config.api_url != g_web_store_config.api_url) {
    is_changed = true;
    g_web_store_config.api_url = web_store_config.api_url;
  }
 
  if (web_store_config.launch_url != g_web_store_config.launch_url) {
    is_changed = true;
    g_web_store_config.launch_url = web_store_config.launch_url;
  }
 
  if (web_store_config.update_url != g_web_store_config.update_url) {
    is_changed = true;
    g_web_store_config.update_url = web_store_config.update_url;
  }
 
  if (web_store_config.verify_url != g_web_store_config.verify_url) {
    is_changed = true;
    g_web_store_config.verify_url = web_store_config.verify_url;
  }
 
  if (web_store_config.home_page_url != g_web_store_config.home_page_url) {
    is_changed = true;
    g_web_store_config.home_page_url = web_store_config.home_page_url;
  }
 
  if (!is_changed) {
    LOG(INFO) << "webstore config is the same and does not need to be updated";
    return false;
  }
 
  extensions::ExtensionsClient* client = extensions::ExtensionsClient::Get();
  if (client) {
    client->SetWebstoreBaseURL(GetDefaultWebstoreLaunchURL());
    client->SetNewWebstoreBaseURL(GetDefaultNewWebstoreLaunchURL());
    client->SetWebstoreUpdateURL(GetDefaultWebstoreUpdateUrl());
  }
 
  LOG(INFO) << "webstore config has been update";
 
  g_web_store_enable = true;
  extensions::FeatureProvider::UpdateFeatures();
  return true;
}
 
int GetWebStoreTypeByUrl(bool flag, const GURL& url) {
  std::string domain;
  if (flag) {
    domain = GURL(g_web_store_config.launch_url).host();
  } else {
    domain = GURL(g_web_store_config.update_url).host();
  }
 
  if (url.DomainIs(domain)) {
    return extensions::kWebStoreType360;
  }
 
  return extensions::kWebStoreTypeDefault;
}
 
GURL GetWebstoreUpdateUrl(int webstore_type) {
  if (webstore_type != extensions::kWebStoreTypeDefault) {
    return GetDefaultWebstoreUpdateUrl();
  }
 
  return GURL(kChromeWebstoreUpdateURL);
}
 
GURL GetWebStoreHomePageUrl() {
  return GURL(g_web_store_config.home_page_url);
}
 
std::vector<std::string> GetWebstoreUpdateDomains() {
  std::vector<std::string> domains;
  GURL url(g_web_store_config.update_url);
  domains.push_back(url.host());
  return domains;
}
 
GURL GetWebStoreCheckUpdateUrl() {
  /* https://update.googleapis.com/service/update2/json */
  return GURL(g_web_store_config.update_url + kCheckCrxPath);
}
 
GURL GetDefaultWebstoreApiUrl() {
  /* https://chromewebstore.googleapis.com */
  return GURL(g_web_store_config.api_url);
}
 
GURL GetDefaultWebstoreUpdateUrl() {
  /* https://clients2.google.com/service/update2/crx */
  return GURL(g_web_store_config.update_url + kDownloadCrxPath);
}
 
GURL GetDefaultWebstoreLaunchURL() {
  /* https://chrome.google.com/webstore */
  return GURL(g_web_store_config.launch_url);
}
 
GURL GetDefaultWebstoreVerifyURL() {
  /* https://www.googleapis.com/chromewebstore/v1.1/items/verify */
  return GURL(g_web_store_config.verify_url);
}
 
GURL GetDefaultNewWebstoreLaunchURL() {
  /* https://chromewebstore.google.com/ */
  return GURL(g_web_store_config.launch_url + "/");
}
 
GURL GetDefaultWebstoreLaunchURLPattern() {
  return GURL(g_web_store_config.launch_url + "/*");
}
 
}  // namespace extension_urls