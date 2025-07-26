/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "components/web_cache/browser/web_cache_manager_utils.h"

#include "base/files/file_path.h"
#include "components/web_cache/browser/web_cache_manager.h"
#include "gtest/gtest.h"

#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
namespace web_cache {
class WebCacheManagerUtilsTest : public testing::Test {
 public:
  std::string url;
  std::string origin;
  std::vector<uint8_t> resource;
  std::map<std::string, std::string> response_headers;
  int type;
};

TEST_F(WebCacheManagerUtilsTest, AddResourceToCache) {
  WebCacheManagerUtils webCacheManagerUtils(WebCacheManager::GetInstance());
  webCacheManagerUtils.AddResourceToCache(url, origin, resource,
                                          response_headers, type);
}
}  // namespace web_cache
#endif