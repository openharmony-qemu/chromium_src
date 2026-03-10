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

#include <vector>
#include <map>

#include "arkweb/chromium_ext/third_party/blink/renderer/platform/exported/we_cache_utils.h"
#include "base/test/task_environment.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/platform/web_cache.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"

#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
#include "third_party/blink/renderer/platform/loader/fetch/memory_cache.h"
#endif

namespace blink {

class WebCacheUtilsTest : public testing::Test {
public:
  static void SetUpTestCase();
  static void TearDownTestCase();
  void SetUp() override;
  void TearDown() override;
protected:
  std::unique_ptr<base::test::TaskEnvironment> task_environment_;

  Resource* GetResourceFromCache(const std::string& url) {
    KURL kurl((String(url)));
    String cache_identifier = String(url.c_str());
    return MemoryCache::Get()->ResourceForURL(kurl, cache_identifier);
  }
};

void WebCacheUtilsTest::SetUpTestCase() {}

void WebCacheUtilsTest::TearDownTestCase() {}

void WebCacheUtilsTest::SetUp()
{
  task_environment_ = std::make_unique<base::test::TaskEnvironment>(
      base::test::TaskEnvironment::MainThreadType::DEFAULT);
}

void WebCacheUtilsTest::TearDown()
{
  task_environment_.reset();
}

/**
 * @tc.name: WebCacheUtilsTest_Constructor_001
 * @tc.desc: Test WebCacheUtils constructor.
 * @tc.type: FUNC
 */
TEST_F(WebCacheUtilsTest, WebCacheUtilsTest_Constructor_001)
{
    WebCache* web_cache = nullptr;
    WebCacheUtils utils(web_cache);
    EXPECT_EQ(utils.web_cache_, web_cache);
}

#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
/**
 * @tc.name: WebCacheUtilsTest_AddResourceToCache_001
 * @tc.desc: Test AddResourceToCache with CSS type.
 * @tc.type: FUNC
 */
TEST_F(WebCacheUtilsTest, WebCacheUtilsTest_AddResourceToCache_001)
{
    std::string url = "https://example.com/test.css";
    std::string origin = "https://example.com";
    std::vector<uint8_t> resource = {0x62, 0x6F, 0x64, 0x79};
    base::flat_map<std::string, std::string> response_headers;
    response_headers["Content-Type"] = "text/css";
    uint64_t type = static_cast<uint64_t>(OfflineResourceType::CSS);

    WebCacheUtils::AddResourceToCache(url, origin, resource, response_headers, type);
    EXPECT_EQ(GetResourceFromCache(url), nullptr);
}

/**
 * @tc.name: WebCacheUtilsTest_AddResourceToCache_002
 * @tc.desc: Test AddResourceToCache with CLASSIC_JS type.
 * @tc.type: FUNC
 */
TEST_F(WebCacheUtilsTest, WebCacheUtilsTest_AddResourceToCache_002)
{
    std::string url = "https://example.com/test.js";
    std::string origin = "https://example.com";
    std::vector<uint8_t> resource = {0x63, 0x6F, 0x6E, 0x73, 0x74};
    base::flat_map<std::string, std::string> response_headers;
    response_headers["Content-Type"] = "application/javascript";
    uint64_t type = static_cast<uint64_t>(OfflineResourceType::CLASSIC_JS);

    WebCacheUtils::AddResourceToCache(url, origin, resource, response_headers, type);
    EXPECT_EQ(GetResourceFromCache(url), nullptr);
}

/**
 * @tc.name: WebCacheUtilsTest_AddResourceToCache_003
 * @tc.desc: Test AddResourceToCache with MODULE_JS type.
 * @tc.type: FUNC
 */
TEST_F(WebCacheUtilsTest, WebCacheUtilsTest_AddResourceToCache_003)
{
    std::string url = "https://example.com/test.mjs";
    std::string origin = "https://example.com";
    std::vector<uint8_t> resource = {0x65, 0x78, 0x70, 0x6F, 0x72, 0x74};
    base::flat_map<std::string, std::string> response_headers;
    response_headers["Content-Type"] = "application/javascript";
    uint64_t type = static_cast<uint64_t>(OfflineResourceType::MODULE_JS);

    WebCacheUtils::AddResourceToCache(url, origin, resource, response_headers, type);
    EXPECT_EQ(GetResourceFromCache(url), nullptr);
}

/**
 * @tc.name: WebCacheUtilsTest_AddResourceToCache_004
 * @tc.desc: Test AddResourceToCache with invalid type.
 * @tc.type: FUNC
 */
TEST_F(WebCacheUtilsTest, WebCacheUtilsTest_AddResourceToCache_004)
{
    std::string url = "https://example.com/test.txt";
    std::string origin = "https://example.com";
    std::vector<uint8_t> resource = {0x74, 0x65, 0x78, 0x74};
    base::flat_map<std::string, std::string> response_headers;
    response_headers["Content-Type"] = "text/plain";
    uint64_t type = 999;

    WebCacheUtils::AddResourceToCache(url, origin, resource, response_headers, type);
    EXPECT_EQ(GetResourceFromCache(url), nullptr);
}

/**
 * @tc.name: WebCacheUtilsTest_AddResourceToCache_005
 * @tc.desc: Test AddResourceToCache with empty resource.
 * @tc.type: FUNC
 */
TEST_F(WebCacheUtilsTest, WebCacheUtilsTest_AddResourceToCache_005)
{
    std::string url = "https://example.com/empty.js";
    std::string origin = "https://example.com";
    std::vector<uint8_t> resource;
    base::flat_map<std::string, std::string> response_headers;
    response_headers["Content-Type"] = "application/javascript";
    uint64_t type = static_cast<uint64_t>(OfflineResourceType::CLASSIC_JS);

    WebCacheUtils::AddResourceToCache(url, origin, resource, response_headers, type);
    EXPECT_EQ(GetResourceFromCache(url), nullptr);
}

/**
 * @tc.name: WebCacheUtilsTest_AddResourceToCache_006
 * @tc.desc: Test AddResourceToCache with Cache-Control headers.
 * @tc.type: FUNC
 */
TEST_F(WebCacheUtilsTest, WebCacheUtilsTest_AddResourceToCache_006)
{
    std::string url = "https://example.com/cached.js";
    std::string origin = "https://example.com";
    std::vector<uint8_t> resource = {0x63, 0x61, 0x63, 0x68, 0x65, 0x64};
    base::flat_map<std::string, std::string> response_headers;
    response_headers["Content-Type"] = "application/javascript";
    response_headers["Cache-Control"] = "max-age=3600";
    uint64_t type = static_cast<uint64_t>(OfflineResourceType::CLASSIC_JS);

    WebCacheUtils::AddResourceToCache(url, origin, resource, response_headers, type);
    EXPECT_EQ(GetResourceFromCache(url), nullptr);
}
#endif

}  // namespace blink
