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

#include "arkweb/chromium_ext/services/network/prp_preload/src/res_request_info_updater.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "base/test/task_environment.h"
namespace ohos_prp_preload {

class ResRequestInfoUpdaterTest : public testing::Test {
public:
  static void SetUpTestCase();
  static void TearDownTestCase();
  void SetUp();
  void TearDown();
protected:
  std::unique_ptr<base::test::TaskEnvironment> task_environment_;
};

void ResRequestInfoUpdaterTest::SetUpTestCase() {}

void ResRequestInfoUpdaterTest::TearDownTestCase() {}

void ResRequestInfoUpdaterTest::SetUp() {
    task_environment_ = std::make_unique<base::test::TaskEnvironment>(
      base::test::TaskEnvironment::MainThreadType::DEFAULT);
}

void ResRequestInfoUpdaterTest::TearDown(){
  task_environment_.reset();
}


TEST_F(ResRequestInfoUpdaterTest,ResRequestInfoUpdaterTest_001)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResPreloadInfosCB preload_infos_cb = base::BindRepeating(
    [](const PRPPPreconnectInfoList& preconnect_info_list,
      const std::shared_ptr<PRPPReqInfoTreeNode>& preload_info_tree,
      const std::set<std::string>& need_record_header_urls){
      LOG(INFO) << "callback";
  });
  ResRequestInfoUpdater updater("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,preload_infos_cb);
  EXPECT_NE(updater.res_req_info_cache_mgr_, nullptr);
  updater.Start();
  GURL request_url("preflight+https://example.com/path/to/resource");
  std::shared_ptr<PRRequestInfo> request_info = std::make_shared<PRRequestInfo>(request_url, false);
  updater.UpdateResRequestInfo(request_info);
  updater.Stop();
  updater.SetPageOrigin("test");
  updater.res_req_info_cache_mgr_ = nullptr;
  updater.Start();
  updater.UpdateResRequestInfo(request_info);
  updater.Stop();
  updater.SetPageOrigin("test");
}

TEST_F(ResRequestInfoUpdaterTest,OnResRequestInfoCacheLoadedTest_001)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResPreloadInfosCB preload_infos_cb;
  ResRequestInfoUpdater updater("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,preload_infos_cb);
  EXPECT_NE(updater.res_req_info_cache_mgr_, nullptr);
  LinkedHashMap load_info_list;
  LinkedHashMap preconnect_limit_info_list;
  updater.OnResRequestInfoCacheLoaded(load_info_list,preconnect_limit_info_list,networkAnonymizationKey,"test");
}

TEST_F(ResRequestInfoUpdaterTest,OnResRequestInfoCacheLoadedTest_002)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResPreloadInfosCB preload_infos_cb = base::BindRepeating(
    [](const PRPPPreconnectInfoList& preconnect_info_list,
      const std::shared_ptr<PRPPReqInfoTreeNode>& preload_info_tree,
      const std::set<std::string>& need_record_header_urls){
      LOG(INFO) << "callback";
  });
  ResRequestInfoUpdater updater("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,preload_infos_cb);
  EXPECT_NE(updater.res_req_info_cache_mgr_, nullptr);
  LinkedHashMap load_info_list;
  LinkedHashMap preconnect_limit_info_list;

  GURL request_url("preflight+https://example.com/path/to/resource");
  std::shared_ptr<PRRequestInfo> request_info = std::make_shared<PRRequestInfo>(request_url, false);
  load_info_list.insert({request_info->url().spec(),request_info});

  GURL request_url2("preflight+https://example.com/path/to/resource2");
  std::shared_ptr<PRRequestInfo> request_info2 = std::make_shared<PRRequestInfo>(request_url2, false);
  request_info2->set_type(PRRequestInfoType::TYPE_PAGE_ORIGIN);
  load_info_list.insert({request_info2->url().spec(),request_info2});

  GURL request_url3("preflight+https://example.com/path/to/resource3");
  std::shared_ptr<PRRequestInfo> request_info3 = std::make_shared<PRRequestInfo>(request_url3, false);
  request_info3->set_preload_seq_num("test");
  request_info3->set_request_end_time(100);
  load_info_list.insert({request_info3->url().spec(),request_info3});

  GURL request_url4("preflight+https://example.com/path/to/resource4");
  std::shared_ptr<PRRequestInfo> request_info4 = std::make_shared<PRRequestInfo>(request_url4, false);
  request_info4->set_preload_seq_num("test");
  request_info4->set_request_start_time(101);
  load_info_list.insert({request_info4->url().spec(),request_info4});

  GURL request_url5("preflight+https://example.com/path/to/resource5");
  std::shared_ptr<PRRequestInfo> request_info5 = std::make_shared<PRRequestInfo>(request_url5, false);
  request_info5->set_preload_seq_num("test");
  request_info5->set_request_start_time(101);
  request_info5->set_request_end_time(100);
  request_info5->set_preload_flag(PRRequestFlags::PRPP_FLAGS_HDR_DYNAMIC);
  load_info_list.insert({request_info5->url().spec(),request_info5});
  EXPECT_EQ(load_info_list.size(), 5);

  GURL request_url6("preflight+https://example.com/path/to/resource6");
  std::shared_ptr<PRRequestInfo> request_info6 = std::make_shared<PRRequestInfo>(request_url6, false);
  request_info6->set_request_end_time(100);
  load_info_list.insert({request_info6->url().spec(),request_info6});
  EXPECT_EQ(load_info_list.size(), 6);

  updater.OnResRequestInfoCacheLoaded(load_info_list,preconnect_limit_info_list,networkAnonymizationKey,"test");
}

TEST_F(ResRequestInfoUpdaterTest,BuildPreloadTreeTest_001)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResPreloadInfosCB preload_infos_cb = base::BindRepeating(
    [](const PRPPPreconnectInfoList& preconnect_info_list,
      const std::shared_ptr<PRPPReqInfoTreeNode>& preload_info_tree,
      const std::set<std::string>& need_record_header_urls){
      LOG(INFO) << "callback";
  });
  ResRequestInfoUpdater updater("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,preload_infos_cb);
  EXPECT_NE(updater.res_req_info_cache_mgr_, nullptr);

  GURL request_url2("preflight+https://example.com/path/to/resource2");
  std::shared_ptr<PRRequestInfo> request_info2 = std::make_shared<PRRequestInfo>(request_url2, false);
  request_info2->set_type(PRRequestInfoType::TYPE_PAGE_ORIGIN);

  std::shared_ptr<PRPPReqInfoTreeNode> cur_parent = std::make_shared<PRPPReqInfoTreeNode>();
  std::shared_ptr<PRPPReqInfoTreeNode> cur_first = std::make_shared<PRPPReqInfoTreeNode>();
  cur_first->req_info_ = request_info2;
  std::shared_ptr<PRPPReqInfoTreeNode> current = nullptr;

  GURL request_url3("preflight+https://example.com/path/to/resource3");
  std::shared_ptr<PRRequestInfo> request_info3 = std::make_shared<PRRequestInfo>(request_url3, false);
  request_info3->set_preload_seq_num("test");
  request_info3->set_request_end_time(100);
  LinkedHashMap load_info_list;
  load_info_list.insert({request_info3->url().spec(),request_info3});
}

TEST_F(ResRequestInfoUpdaterTest,BuildPreconnectListTest_001)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResPreloadInfosCB preload_infos_cb = base::BindRepeating(
    [](const PRPPPreconnectInfoList& preconnect_info_list,
      const std::shared_ptr<PRPPReqInfoTreeNode>& preload_info_tree,
      const std::set<std::string>& need_record_header_urls){
      LOG(INFO) << "callback";
  });
  ResRequestInfoUpdater updater("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,preload_infos_cb);
  EXPECT_NE(updater.res_req_info_cache_mgr_, nullptr);

  LinkedHashMap preconnect_limit_info_list;
  GURL request_url("https://example.com");
  std::shared_ptr<PRRequestInfo> request_info = std::make_shared<PRRequestInfo>(request_url, false);
  request_info->set_method("GET");
  request_info->set_cache_type(PRRequestCacheType::FORCE_CACHE);
  request_info->set_preconnect_num(5);
  GURL origin_url;
  std::string key;
  ResReqInfoCacheMgr::GetPreconnectOriginUrl(request_info, origin_url);
  ResReqInfoCacheMgr::GetPreconnectLimitListKey(origin_url, request_info->allow_credentials(),key);
  preconnect_limit_info_list.insert({key,request_info});
  updater.preconnect_org_url_map_[key] = 1;
  updater.BuildPreconnectList(request_info, preconnect_limit_info_list, networkAnonymizationKey, "test");
}

TEST_F(ResRequestInfoUpdaterTest,BuildPreloadTreeTest_01)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResPreloadInfosCB preload_infos_cb = base::BindRepeating(
    [](const PRPPPreconnectInfoList& preconnect_info_list,
      const std::shared_ptr<PRPPReqInfoTreeNode>& preload_info_tree,
      const std::set<std::string>& need_record_header_urls){
      LOG(INFO) << "callback";
  });
  ResRequestInfoUpdater updater("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,preload_infos_cb);
  EXPECT_NE(updater.res_req_info_cache_mgr_, nullptr);

  std::shared_ptr<PRPPReqInfoTreeNode> current = std::make_shared<PRPPReqInfoTreeNode>();
  std::shared_ptr<PRPPReqInfoTreeNode> cur_parent = std::make_shared<PRPPReqInfoTreeNode>();
  std::shared_ptr<PRPPReqInfoTreeNode> cur_first = std::make_shared<PRPPReqInfoTreeNode>();

  GURL request_url("https://example.com");
  std::shared_ptr<PRRequestInfo> request_info = std::make_shared<PRRequestInfo>(request_url, false);
  request_info->set_request_start_time(100000*2);
  request_info->set_preload_seq_num("test");
  request_info->set_preload_flag(PRRequestFlags::PRPP_FLAGS_HDR_DYNAMIC);
  current->req_info_ = request_info;

  GURL request_url2("preflight+https://example.com/path/to/resource2");
  std::shared_ptr<PRRequestInfo> request_info2 = std::make_shared<PRRequestInfo>(request_url2, false);
  cur_first->req_info_ = request_info2;

  EXPECT_EQ(current->req_info_->request_start_time(),100000*2);
  EXPECT_EQ(cur_first->req_info_->request_start_time()+100000,100000);

  updater.BuildPreloadTree(request_info,current,cur_first,cur_parent,1,"test");
}

TEST_F(ResRequestInfoUpdaterTest,BuildPreloadTreeTest_02)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResPreloadInfosCB preload_infos_cb = base::BindRepeating(
    [](const PRPPPreconnectInfoList& preconnect_info_list,
      const std::shared_ptr<PRPPReqInfoTreeNode>& preload_info_tree,
      const std::set<std::string>& need_record_header_urls){
      LOG(INFO) << "callback";
  });
  ResRequestInfoUpdater updater("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,preload_infos_cb);
  EXPECT_NE(updater.res_req_info_cache_mgr_, nullptr);

  std::shared_ptr<PRPPReqInfoTreeNode> current = std::make_shared<PRPPReqInfoTreeNode>();
  std::shared_ptr<PRPPReqInfoTreeNode> cur_parent = std::make_shared<PRPPReqInfoTreeNode>();
  std::shared_ptr<PRPPReqInfoTreeNode> cur_first = std::make_shared<PRPPReqInfoTreeNode>();

  GURL request_url("https://example.com");
  std::shared_ptr<PRRequestInfo> request_info = std::make_shared<PRRequestInfo>(request_url, false);
  request_info->set_request_start_time(0);
  request_info->set_preload_seq_num("test");
  request_info->set_request_end_time(10);
  current->req_info_ = request_info;

  GURL request_url2("preflight+https://example.com/path/to/resource2");
  std::shared_ptr<PRRequestInfo> request_info2 = std::make_shared<PRRequestInfo>(request_url2, false);
  cur_first->req_info_ = request_info2;

  updater.BuildPreloadTree(request_info,current,cur_first,cur_parent,0,"test");
}

TEST_F(ResRequestInfoUpdaterTest,UpdateResRequestInfoForDynamicHeaders_001)
{
  net::NetworkAnonymizationKey networkAnonymizationKey = new net::NetworkAnonymizationKey();
  DiskCacheBackendFactory* factory = new DiskCacheBackendFactory();
  scoped_refptr<DiskCacheBackendFactory> disk_cache_backend_factory(factory);
  ResPreloadInfosCB preload_infos_cb = base::BindRepeating(
    [](const PRPPPreconnectInfoList& preconnect_info_list,
      const std::shared_ptr<PRPPReqInfoTreeNode>& preload_info_tree,
      const std::set<std::string>& need_record_header_urls){
      LOG(INFO) << "callback";
  });
  ResRequestInfoUpdater updater("www.test.com",networkAnonymizationKey,disk_cache_backend_factory,preload_infos_cb);
  EXPECT_NE(updater.res_req_info_cache_mgr_, nullptr);

  std::shared_ptr<PRPPReqInfoTreeNode> parent = std::make_shared<PRPPReqInfoTreeNode>();
  std::shared_ptr<PRPPReqInfoTreeNode> grandPa = std::make_shared<PRPPReqInfoTreeNode>();

  std::list<std::shared_ptr<PRPPReqInfoTreeNode>> children;
  auto child1 = std::make_shared<PRPPReqInfoTreeNode>();
  auto child2 = std::make_shared<PRPPReqInfoTreeNode>();
  auto child3 = std::make_shared<PRPPReqInfoTreeNode>();

  GURL request_url("https://example.com");
  std::shared_ptr<PRRequestInfo> request_info = std::make_shared<PRRequestInfo>(request_url, false);
  request_info->set_cache_type(PRRequestCacheType::FORCE_CACHE);
  child1->req_info_ = request_info;

  GURL request_url2("https://example.com");
  std::shared_ptr<PRRequestInfo> request_info2 = std::make_shared<PRRequestInfo>(request_url2, false);
  child2->req_info_ = request_info2;

  GURL request_url3("https://example.com");
  std::shared_ptr<PRRequestInfo> request_info3 = std::make_shared<PRRequestInfo>(request_url3, false);
  child3->req_info_ = request_info3;

  children.push_back(child1);
  children.push_back(child2);
  children.push_back(child3);

  grandPa->children_ = children;
  parent->parent_ = grandPa.get();

  GURL request_url1("https://example.com");
  std::shared_ptr<PRRequestInfo> child_info = std::make_shared<PRRequestInfo>(request_url1, false);
  std::set<std::string> dynamic_header_keys;
  dynamic_header_keys.insert("hello");
  child_info->set_dynamic_header_keys(dynamic_header_keys);

  updater.UpdateResRequestInfoForDynamicHeaders(parent,child_info);
}


} // namespace ohos_prp_preload