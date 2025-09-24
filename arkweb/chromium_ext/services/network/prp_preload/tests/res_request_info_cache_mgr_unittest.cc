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

#include "testing/gtest/include/gtest/gtest.h"

#include "arkweb/chromium_ext/services/network/prp_preload/src/res_request_info_cache_mgr.h"
#include "url/gurl.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <memory>
#include <string>
#include <unordered_map>
#include "base/test/task_environment.h"
namespace ohos_prp_preload {

class ResRequestInfoCacheMgrTest: public testing::Test {
public:
  static void SetUpTestCase();
  static void TearDownTestCase();
  void SetUp();
  void TearDown();
protected:
  std::unique_ptr<base::test::TaskEnvironment> task_environment_;
};

void ResRequestInfoCacheMgrTest::SetUpTestCase() {}

void ResRequestInfoCacheMgrTest::TearDownTestCase() {}

void ResRequestInfoCacheMgrTest::SetUp() {
    task_environment_ = std::make_unique<base::test::TaskEnvironment>(
      base::test::TaskEnvironment::MainThreadType::DEFAULT);
}

void ResRequestInfoCacheMgrTest::TearDown(){
  task_environment_.reset();
}

TEST_F(ResRequestInfoCacheMgrTest, ResRequestInfoCacheMgrTest_001)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResRequestInfoCacheLoadedCB info_cache_cb = base::BindRepeating(
    [](const LinkedHashMap& load_info_list,
      const LinkedHashMap& preconnect_limit_info_list,
      const net::NetworkAnonymizationKey& networkAnonymizationKey,
      const std::string& page_seq_num){
        LOG(INFO) << "Cache loaded for page: " << page_seq_num;
  });

  ResReqInfoCacheMgr cache("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,info_cache_cb);
  cache.SetPageOrigin("test");
  EXPECT_NE(cache.page_origin_,"test");
  cache.Stop();
  cache.CheckFlush(false);
  cache.Start();
  cache.CheckFlush(false);
  cache.CheckFlush(true);
  cache.SetPageOrigin("test");
  EXPECT_EQ(cache.page_origin_,"test");
  cache.Start();

  GURL request_url("preflight+https://example.com/path/to/resource");
  std::shared_ptr<PRRequestInfo> request_info = std::make_shared<PRRequestInfo>(request_url, false);
  cache.new_preload_info_list_.insert({request_info->url().spec(),request_info});
  cache.last_flush_len_ = 0;
  EXPECT_NE(cache.new_preload_info_list_.size(),0);
  cache.Stop();
}

TEST_F(ResRequestInfoCacheMgrTest,ResRequestInfoCacheMgrTest_002)
{
  GURL origin_url("https://example.com");
  std::string key;
  ResReqInfoCacheMgr::GetPreconnectLimitListKey(origin_url, true, key);
  EXPECT_EQ(key, "ac/"+origin_url.spec());

  key.clear(); 
  ResReqInfoCacheMgr::GetPreconnectLimitListKey(origin_url, false, key);
  EXPECT_EQ(key, origin_url.spec());
}

TEST_F(ResRequestInfoCacheMgrTest,ResRequestInfoCacheMgrTest_003)
{
  GURL request_url1("https://example.com/path/to/resource");
  GURL request_url2("preflight+https://example.com/path/to/resource");
  GURL origin_url1;
  GURL origin_url2;
  std::shared_ptr<PRRequestInfo> request_info = std::make_shared<PRRequestInfo>(request_url1, false);
  ResReqInfoCacheMgr::GetPreconnectOriginUrl(request_info,origin_url1);
  std::string expect_url = "https://example.com/";
  EXPECT_EQ(expect_url,origin_url1.spec());

  request_info->set_type(PRRequestInfoType::TYPE_PAGE_PREFLIGHT);
  ResReqInfoCacheMgr::GetPreconnectOriginUrl(request_info,origin_url2);
  EXPECT_EQ(expect_url,origin_url2.spec());
}

TEST_F(ResRequestInfoCacheMgrTest,ResRequestInfoCacheMgrTest_004)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResRequestInfoCacheLoadedCB info_cache_cb = base::BindRepeating(
    [](const LinkedHashMap& load_info_list,
      const LinkedHashMap& preconnect_limit_info_list,
      const net::NetworkAnonymizationKey& networkAnonymizationKey,
      const std::string& page_seq_num){
        LOG(INFO) << "Cache loaded for page: " << page_seq_num;
  });

  ResReqInfoCacheMgr cache("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,info_cache_cb);
  EXPECT_NE(cache.disk_cache_, nullptr);
  cache.OnEntryLoadedCallback("");
  cache.is_start_ = true;
  cache.OnEntryLoadedCallback("");
  cache.page_index_ = 1;
  cache.OnEntryLoadedCallback("");
}

TEST_F(ResRequestInfoCacheMgrTest, UpdateResRequestInfoTest)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResRequestInfoCacheLoadedCB info_cache_cb = base::BindRepeating(
    [](const LinkedHashMap& load_info_list,
      const LinkedHashMap& preconnect_limit_info_list,
      const net::NetworkAnonymizationKey& networkAnonymizationKey,
      const std::string& page_seq_num){
        LOG(INFO) << "Cache loaded for page: " << page_seq_num;
  });

  ResReqInfoCacheMgr cache("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,info_cache_cb);

  GURL request_url("preflight+https://example.com/path/to/resource");
  std::shared_ptr<PRRequestInfo> request_info = std::make_shared<PRRequestInfo>(request_url, false);

  cache.UpdateResRequestInfo(request_info);
  cache.is_start_ = true;
  bool is_request_with_cookie = cache.IsRequestWithCookie(request_info);
  EXPECT_EQ(is_request_with_cookie,false);
  cache.UpdateResRequestInfo(request_info);
  cache.new_preload_info_list_.clear();
  cache.new_preload_info_list_.insert({request_info->url().spec(),request_info});
  cache.UpdateResRequestInfo(request_info);

  GURL url("http://user:pass@example.com");
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>(url, false);
  bool is_request_with_cookie2 = cache.IsRequestWithCookie(info);
  EXPECT_EQ(is_request_with_cookie2,false);
  bool has_username = info->url().has_username();
  EXPECT_EQ(has_username,true);
  info->set_type(PRRequestInfoType::TYPE_PAGE_PREFLIGHT);
  std::string key = info->url().spec();
  cache.UpdateResRequestInfo(info);
  EXPECT_NE(cache.new_preload_info_list_.size(),0);
  cache.UpdateResRequestInfo(info);
  cache.UpdatePreloadInfo(key,info);
  GURL request_url3("https://example.com/path/to/resource3");
  std::shared_ptr<PRRequestInfo> request_info3 = std::make_shared<PRRequestInfo>(request_url3, false);
  request_info3->set_preload_seq_num("12, 23, ");
  cache.load_preload_info_list_.insert({request_info3->url().spec(),request_info3});
  auto new_info = std::make_shared<PRRequestInfo>();
  cache.UpdatePreloadInfo(request_info3->url().spec(),new_info);
}

TEST_F(ResRequestInfoCacheMgrTest,UpdatePreconnectInfoTest)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResRequestInfoCacheLoadedCB info_cache_cb = base::BindRepeating(
    [](const LinkedHashMap& load_info_list,
      const LinkedHashMap& preconnect_limit_info_list,
      const net::NetworkAnonymizationKey& networkAnonymizationKey,
      const std::string& page_seq_num){
        LOG(INFO) << "Cache loaded for page: " << page_seq_num;
  });

  ResReqInfoCacheMgr cache("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,info_cache_cb);
  GURL request_url("preflight+https://example.com/path/to/resource");
  std::shared_ptr<PRRequestInfo> request_info = std::make_shared<PRRequestInfo>(request_url, false);
  request_info->set_is_preconnect(true);
  EXPECT_EQ(request_info->is_preconnect(),true);
  cache.UpdatePreconnectInfo(request_info);
  EXPECT_NE(cache.new_preconnect_limit_list_.size(),0);
  cache.UpdatePreconnectInfo(request_info);
}

TEST_F(ResRequestInfoCacheMgrTest,JsonToResReqPreloadInfoListTest)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResRequestInfoCacheLoadedCB info_cache_cb = base::BindRepeating(
    [](const LinkedHashMap& load_info_list,
      const LinkedHashMap& preconnect_limit_info_list,
      const net::NetworkAnonymizationKey& networkAnonymizationKey,
      const std::string& page_seq_num){
        LOG(INFO) << "Cache loaded for page: " << page_seq_num;
  });

  ResReqInfoCacheMgr cache("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,info_cache_cb);
  EXPECT_NE(cache.disk_cache_, nullptr);
  cache.is_start_=true;
  cache.OnEntryLoadedCallback("test");

  std::string content = R"json(
  [
    {
      "type": 1,
      "page_index": 0,
      "page_origin": "https://example.com"
    }
  ]
  )json";
  cache.OnEntryLoadedCallback(content);

  std::string contentNull = R"json(
  []
  )json";
  cache.OnEntryLoadedCallback(contentNull);

  std::string content2 = R"json([
  { "request_info_type": 1 }
  ])json";
  cache.OnEntryLoadedCallback(content2);
}

TEST_F(ResRequestInfoCacheMgrTest,JsonToResReqPreloadInfoListTest_002)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResRequestInfoCacheLoadedCB info_cache_cb = base::BindRepeating(
    [](const LinkedHashMap& load_info_list,
      const LinkedHashMap& preconnect_limit_info_list,
      const net::NetworkAnonymizationKey& networkAnonymizationKey,
      const std::string& page_seq_num){
        LOG(INFO) << "Cache loaded for page: " << page_seq_num;
  });

  ResReqInfoCacheMgr cache("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,info_cache_cb);
  EXPECT_NE(cache.disk_cache_, nullptr);
  cache.is_start_=true;
  std::string content4 = R"json([
    {
      "request_info_type": 1,
      "page_index": "0",
      "page_origin": "https://example.com",
      "url": "https://origin"
    },
    {"page_origin": "https://example.com"},
    {
      "request_info_type": 3,
      "allow_credentials": true,
      "limit_num": 10,
      "url": "https://example.com/limit"
    },
    {"request_info_type": 0 },
    {
      "request_info_type": 0,
      "allow_credentials": true,
      "limit_num": 10,
      "url": "https://example.com/limit"
    }
 ])json";
  cache.OnEntryLoadedCallback(content4);
}

TEST_F(ResRequestInfoCacheMgrTest,JsonToResReqPreloadInfoListTest_003)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResRequestInfoCacheLoadedCB info_cache_cb = base::BindRepeating(
    [](const LinkedHashMap& load_info_list,
      const LinkedHashMap& preconnect_limit_info_list,
      const net::NetworkAnonymizationKey& networkAnonymizationKey,
      const std::string& page_seq_num){
        LOG(INFO) << "Cache loaded for page: " << page_seq_num;
  });

  ResReqInfoCacheMgr cache("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,info_cache_cb);
  EXPECT_NE(cache.disk_cache_, nullptr);
  cache.is_start_=true;
  std::string content3 = R"json([
    {
      "request_info_type": 1,
      "page_index": "123"
    },
    {
      "request_info_type": 1,
      "page_index": "0",
      "page_origin": "https://example.com",
      "url": "https://origin"
    }
 ])json";
  cache.OnEntryLoadedCallback(content3);
}

TEST_F(ResRequestInfoCacheMgrTest,ResReqPreloadInfoListToJsonTest)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResRequestInfoCacheLoadedCB info_cache_cb = base::BindRepeating(
    [](const LinkedHashMap& load_info_list,
      const LinkedHashMap& preconnect_limit_info_list,
      const net::NetworkAnonymizationKey& networkAnonymizationKey,
      const std::string& page_seq_num){
        LOG(INFO) << "Cache loaded for page: " << page_seq_num;
  });

  ResReqInfoCacheMgr cache("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,info_cache_cb);
  EXPECT_NE(cache.disk_cache_, nullptr);
  cache.is_start_=true;
  cache.page_origin_ = "https://example.com";
  GURL request_url1("https://example.com/path/to/resource1");
  std::shared_ptr<PRRequestInfo> request_info1 = std::make_shared<PRRequestInfo>(request_url1, false);
  GURL request_url2("https://example.com/path/to/resource2");
  std::shared_ptr<PRRequestInfo> request_info2 = std::make_shared<PRRequestInfo>(request_url2, false);
  request_info2->set_limit_num(10);
  cache.new_preconnect_limit_list_.insert({request_info1->url().spec(),request_info1});
  cache.new_preconnect_limit_list_.insert({request_info2->url().spec(),request_info2});
  GURL request_url3("https://example.com/path/to/resource3");
  std::shared_ptr<PRRequestInfo> request_info3 = std::make_shared<PRRequestInfo>(request_url3, false);
  request_info3->set_preload_flag(PRPP_FLAGS_VISIBLE);
  cache.load_preload_info_list_.insert({request_info3->url().spec(),request_info3});
  cache.CheckFlush(true);
  cache.page_index_=5;
  GURL request_url4("https://example.com/path/to/resource4");
  std::shared_ptr<PRRequestInfo> request_info4 = std::make_shared<PRRequestInfo>(request_url4, false);
  request_info4->set_preload_flag(PRPP_FLAGS_VISIBLE);
  cache.load_preload_info_list_.insert({request_info4->url().spec(),request_info4});
  cache.CheckFlush(true);
}

TEST_F(ResRequestInfoCacheMgrTest,ResRequestInfoCacheMgrTest_005)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  scoped_refptr<DiskCacheBackendFactory> factory = nullptr;
  ResRequestInfoCacheLoadedCB info_cache_cb = base::BindRepeating(
    [](const LinkedHashMap& load_info_list,
      const LinkedHashMap& preconnect_limit_info_list,
      const net::NetworkAnonymizationKey& networkAnonymizationKey,
      const std::string& page_seq_num){
        LOG(INFO) << "Cache loaded for page: " << page_seq_num;
  });

  auto mgr = std::make_unique<ResReqInfoCacheMgr>("",networkAnonymizationKey,factory, info_cache_cb);
  EXPECT_NE(mgr->disk_cache_, nullptr);
}
} // namespace ohos_prp_preload

