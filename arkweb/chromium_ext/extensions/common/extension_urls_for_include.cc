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
 
namespace extensions {
 
enum : int {
  kWebStoreTypeDefault = 0,
 
  kWebStoreType360 = 1,
};
 
}  // namespace extensions
 
namespace extension_urls {
 
struct WebStoreConfig {
  std::string api_url;
  std::string update_url;
  std::string launch_url;
  std::string verify_url;
  std::string home_page_url;
};
 
bool IsWebStoreEnable();
 
WebStoreConfig GetWebStoreConfig();
 
bool SetWebStoreConfig(const WebStoreConfig& web_store_config);
 
int GetWebStoreTypeByUrl(bool flag, const GURL& url);
 
GURL GetWebstoreUpdateUrl(int webstore_type);
 
GURL GetWebStoreHomePageUrl();
 
std::vector<std::string> GetWebstoreUpdateDomains();
 
GURL GetWebStoreCheckUpdateUrl();
 
GURL GetDefaultWebstoreApiUrl();
 
GURL GetDefaultWebstoreLaunchURL();
 
GURL GetDefaultWebstoreVerifyURL();
 
GURL GetDefaultNewWebstoreLaunchURL();
 
GURL GetDefaultWebstoreLaunchURLPattern();
 
}  // namespace extension_urls