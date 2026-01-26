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

#include "base/test/task_environment.h"
#include "net/url_request/url_request_context_builder.h"
#include "net/url_request/url_request_test_util.h"

#include "arkweb/chromium_ext/services/network/prp_preload/src/res_preload_scheduler.h"

namespace ohos_prp_preload {

constexpr int SOCKET_LIMIT_TEST = 30;
constexpr int MAX_REQUEST_COUNT_TEST = 12;

class ResPreloadSchedulerTest : public testing::Test {
public:
  static void SetUpTestCase();
  static void TearDownTestCase();
  void SetUp() override;
  void TearDown() override;
protected:
  std::unique_ptr<base::test::TaskEnvironment> task_environment_;
};

void ResPreloadSchedulerTest::SetUpTestCase() {}

void ResPreloadSchedulerTest::TearDownTestCase() {}

void ResPreloadSchedulerTest::SetUp()
{
  task_environment_ = std::make_unique<base::test::TaskEnvironment>(
    base::test::TaskEnvironment::MainThreadType::DEFAULT);
}

void ResPreloadSchedulerTest::TearDown()
{
  task_environment_.reset();
}

TEST_F(ResPreloadSchedulerTest, ResPreloadSchedulerTest_CreateReqLoaderAndStart)
{
  std::string url;
  scoped_refptr<base::SingleThreadTaskRunner> net_task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(net_task_runner, nullptr);
  EXPECT_NE(url_request_context, nullptr);
  PRPPOnPageOriginCB on_page_origin_cb;
  ResPreloadScheduler scheduler(url, net_task_runner, url_request_context->GetWeakPtr(), on_page_origin_cb);

  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  scheduler.CreateReqLoaderAndStart(info);
  EXPECT_NE(scheduler.prerequest_info_list_.size(), 0);
  scheduler.net_task_runner_ = nullptr;
  scheduler.CreateReqLoaderAndStart(info);
  scheduler.net_task_runner_ = net_task_runner;

  std::shared_ptr<PRPPRequestLoaderFactory> requestLoader =
    PRPPRequestLoaderFactory::CreatePRPPRequestLoaderFactory(url, url_request_context->GetWeakPtr());
  EXPECT_NE(requestLoader, nullptr);
  scheduler.SetPRPPReqLoaderFac(requestLoader->GetWeak(), (requestLoader->GetWeak().get() != nullptr));
  EXPECT_NE(scheduler.loader_fac_weak_, nullptr);
  scheduler.CreateReqLoaderAndStart(info);

  scheduler.need_record_header_urls_.insert(url);
  scheduler.CreateReqLoaderAndStart(info);
  EXPECT_EQ(scheduler.need_record_header_urls_.size(), 0);
}

TEST_F(ResPreloadSchedulerTest, ResPreloadSchedulerTest_StopPreload)
{
  std::string url;
  scoped_refptr<base::SingleThreadTaskRunner> net_task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(net_task_runner, nullptr);
  EXPECT_NE(url_request_context, nullptr);
  PRPPOnPageOriginCB on_page_origin_cb;
  ResPreloadScheduler scheduler(url, net_task_runner, url_request_context->GetWeakPtr(), on_page_origin_cb);
  scheduler.StopPreload();
  EXPECT_EQ(scheduler.preload_triggered_, false);
}

TEST_F(ResPreloadSchedulerTest, ResPreloadSchedulerTest_SetPRPPReqLoaderFac)
{
  std::string url;
  scoped_refptr<base::SingleThreadTaskRunner> net_task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(net_task_runner, nullptr);
  EXPECT_NE(url_request_context, nullptr);
  PRPPOnPageOriginCB on_page_origin_cb;
  ResPreloadScheduler scheduler(url, net_task_runner, url_request_context->GetWeakPtr(), on_page_origin_cb);
  scheduler.SetPRPPReqLoaderFac(nullptr, false);
  EXPECT_EQ(scheduler.loader_fac_weak_, nullptr);

  scheduler.preload_triggered_ = true;
  std::shared_ptr<PRPPRequestLoaderFactory> requestLoader =
    PRPPRequestLoaderFactory::CreatePRPPRequestLoaderFactory(url, url_request_context->GetWeakPtr());
  EXPECT_NE(requestLoader, nullptr);
  scheduler.SetPRPPReqLoaderFac(requestLoader->GetWeak(), (requestLoader->GetWeak().get() != nullptr));
  scheduler.preload_triggered_ = false;
  scheduler.SetPRPPReqLoaderFac(requestLoader->GetWeak(), (requestLoader->GetWeak().get() != nullptr));
}

TEST_F(ResPreloadSchedulerTest, ResPreloadSchedulerTest_SchedulePreloads)
{
  std::string url;
  scoped_refptr<base::SingleThreadTaskRunner> net_task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(net_task_runner, nullptr);
  EXPECT_NE(url_request_context, nullptr);
  PRPPOnPageOriginCB on_page_origin_cb;
  ResPreloadScheduler scheduler(url, net_task_runner, url_request_context->GetWeakPtr(), on_page_origin_cb);
  PRPPPreconnectInfoList list;
  std::shared_ptr<PRPPReqInfoTreeNode> node = std::make_shared<PRPPReqInfoTreeNode>();
  std::set<std::string> urls;
  scheduler.preload_triggered_ = true;
  scheduler.SchedulePreloads(list, node, urls);
  scheduler.preload_triggered_ = false;
  scheduler.net_task_runner_ = nullptr;
  scheduler.SchedulePreloads(list, node, urls);
  scheduler.net_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();

  scheduler.SchedulePreloads(list, nullptr, urls);
  EXPECT_EQ(scheduler.preload_triggered_, true);
  scheduler.preload_triggered_ = false;
  scheduler.SchedulePreloads(list, node, urls);
  EXPECT_EQ(scheduler.preload_info_tree_->req_info_, nullptr);
  node->req_info_ = std::make_shared<PRRequestInfo>();
  scheduler.preload_triggered_ = false;
  scheduler.SchedulePreloads(list, node, urls);
  EXPECT_NE(scheduler.preload_info_tree_->req_info_->type(), PRRequestInfoType::TYPE_PAGE_ORIGIN);
  scheduler.preload_triggered_ = false;
  node->req_info_->set_type(PRRequestInfoType::TYPE_PAGE_ORIGIN);
  scheduler.SchedulePreloads(list, node, urls);
  EXPECT_EQ(scheduler.preload_info_tree_->children_.size(), 0);
  std::list<std::shared_ptr<PRPPReqInfoTreeNode>> nodeList;
  std::shared_ptr<PRPPReqInfoTreeNode> node1 = std::make_shared<PRPPReqInfoTreeNode>();
  nodeList.push_back(node1);
  node->children_ = nodeList;
  scheduler.preload_triggered_ = false;
  scheduler.SchedulePreloads(list, node, urls);
  EXPECT_NE(scheduler.preload_info_tree_->children_.size(), 0);

  scheduler.on_page_origin_cb_ = base::BindRepeating([](const std::string& url,
    const std::string& page_origin) -> void {});
  scheduler.preload_triggered_ = false;
  scheduler.SchedulePreloads(list, node, urls);
  EXPECT_EQ(scheduler.cur_parent_, node);
  scheduler.preload_triggered_ = false;
  scheduler.SchedulePreloads(list, nullptr, urls);
}

TEST_F(ResPreloadSchedulerTest, ResPreloadSchedulerTest_SchedulePreconnects)
{
  std::string url;
  scoped_refptr<base::SingleThreadTaskRunner> net_task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(net_task_runner, nullptr);
  EXPECT_NE(url_request_context, nullptr);
  PRPPOnPageOriginCB on_page_origin_cb;
  ResPreloadScheduler scheduler(url, net_task_runner, url_request_context->GetWeakPtr(), on_page_origin_cb);
  scheduler.SchedulePreconnects();
  EXPECT_EQ(scheduler.prpp_preconnect_info_list_.size(), 0);

  PRPPPreconnectInfoList list;
  PRPPPreconnectInfo info;
  info.allow_credential_ = true;
  list.push_back(info);
  scheduler.prpp_preconnect_info_list_ = list;
  scheduler.SchedulePreconnects();
  EXPECT_EQ(scheduler.prpp_preconnect_info_list_.size(), 1);

  for (int i = 0; i < SOCKET_LIMIT_TEST; i++) {
    list.push_back(info);
  }
  scheduler.prpp_preconnect_info_list_ = list;
  scheduler.SchedulePreconnects();
  EXPECT_EQ(scheduler.prpp_preconnect_info_list_.size(), SOCKET_LIMIT_TEST + 1);
  scheduler.net_task_runner_ = nullptr;
  scheduler.SchedulePreconnects();
}

TEST_F(ResPreloadSchedulerTest, ResPreloadSchedulerTest_ContinueSchedulePreconnects)
{
  std::string url;
  scoped_refptr<base::SingleThreadTaskRunner> net_task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(net_task_runner, nullptr);
  EXPECT_NE(url_request_context, nullptr);
  PRPPOnPageOriginCB on_page_origin_cb;
  ResPreloadScheduler scheduler(url, net_task_runner, url_request_context->GetWeakPtr(), on_page_origin_cb);
  ResPreloadScheduler::PreconnectInfoListIter iter = scheduler.prpp_preconnect_info_list_.end();
  scheduler.ContinueSchedulePreconnects(iter, 1);
  EXPECT_EQ(scheduler.info_list_version_, 0);
  scheduler.ContinueSchedulePreconnects(iter, 0);

  PRPPPreconnectInfo info;
  info.allow_credential_ = true;
  scheduler.prpp_preconnect_info_list_.push_back(info);
  ResPreloadScheduler::PreconnectInfoListIter iter1 = scheduler.prpp_preconnect_info_list_.begin();
  scheduler.ContinueSchedulePreconnects(iter1, 0);
  EXPECT_EQ(scheduler.preload_triggered_, false);
  scheduler.preload_triggered_ = true;
  scheduler.ContinueSchedulePreconnects(iter1, 0);
  scheduler.prpp_preconnect_info_list_.push_back(info);
  ResPreloadScheduler::PreconnectInfoListIter iter2 = scheduler.prpp_preconnect_info_list_.begin();
  scheduler.ContinueSchedulePreconnects(iter2, 0);
  EXPECT_NE(iter2, scheduler.prpp_preconnect_info_list_.end());
  scheduler.net_task_runner_ = nullptr;
  scheduler.ContinueSchedulePreconnects(iter2, 0);
}

TEST_F(ResPreloadSchedulerTest, ResPreloadSchedulerTest_SchedulePrerequests1)
{
  std::string url;
  scoped_refptr<base::SingleThreadTaskRunner> net_task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(net_task_runner, nullptr);
  EXPECT_NE(url_request_context, nullptr);
  PRPPOnPageOriginCB on_page_origin_cb;
  ResPreloadScheduler scheduler(url, net_task_runner, url_request_context->GetWeakPtr(), on_page_origin_cb);

  scheduler.SchedulePrerequests(0, 1);
  EXPECT_EQ(scheduler.info_list_version_, 0);
  scheduler.SchedulePrerequests(0, 0);
  EXPECT_EQ(scheduler.preload_triggered_, false);
  scheduler.preload_triggered_ = true;
  scheduler.SchedulePrerequests(0, 0);
  EXPECT_EQ(scheduler.cur_parent_, nullptr);
  scheduler.cur_parent_ = std::make_shared<PRPPReqInfoTreeNode>();
  scheduler.net_task_runner_ = nullptr;
  scheduler.SchedulePrerequests(0, 0);
  scheduler.net_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  scheduler.SchedulePrerequests(0, 0);
  EXPECT_EQ(scheduler.loader_fac_weak_, nullptr);
  std::shared_ptr<PRPPRequestLoaderFactory> requestLoader =
    PRPPRequestLoaderFactory::CreatePRPPRequestLoaderFactory(url, url_request_context->GetWeakPtr());
  EXPECT_NE(requestLoader, nullptr);
  scheduler.loader_fac_weak_ = requestLoader->GetWeak();
  EXPECT_NE(scheduler.loader_fac_weak_, nullptr);
  scheduler.SchedulePrerequests(0, 0);
  scheduler.SchedulePrerequests(1, 0);
  scheduler.idle_prerequest_count_ = 1;
  scheduler.SchedulePrerequests(MAX_REQUEST_COUNT_TEST + 1, 0);
  scheduler.SchedulePrerequests(0, 0);

  std::shared_ptr<PRPPReqInfoTreeNode> node = std::make_shared<PRPPReqInfoTreeNode>();
  scheduler.cur_parent_->children_.push_back(node);
  scheduler.cur_node_iter_ = scheduler.cur_parent_->children_.begin();
  (*scheduler.cur_node_iter_)->req_info_ = std::make_shared<PRRequestInfo>();
  (*scheduler.cur_node_iter_)->req_info_->set_method("GET");
  scheduler.SchedulePrerequests(1, 0);
  scheduler.cur_parent_->children_.push_back(node);
  scheduler.cur_parent_->children_.push_back(node);
  scheduler.SchedulePrerequests(1, 0);

  scheduler.idle_prerequest_count_ = 0;
  scheduler.UpdateIdlePrerequestCount();
  EXPECT_EQ(scheduler.idle_prerequest_count_, 1);
}

TEST_F(ResPreloadSchedulerTest, ResPreloadSchedulerTest_SchedulePrerequests2)
{
  std::string url;
  scoped_refptr<base::SingleThreadTaskRunner> net_task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(net_task_runner, nullptr);
  EXPECT_NE(url_request_context, nullptr);
  PRPPOnPageOriginCB on_page_origin_cb;
  ResPreloadScheduler scheduler(url, net_task_runner, url_request_context->GetWeakPtr(), on_page_origin_cb);

  scheduler.preload_triggered_ = true;
  std::shared_ptr<PRPPRequestLoaderFactory> requestLoader =
    PRPPRequestLoaderFactory::CreatePRPPRequestLoaderFactory(url, url_request_context->GetWeakPtr());
  EXPECT_NE(requestLoader, nullptr);
  scheduler.loader_fac_weak_ = requestLoader->GetWeak();
  EXPECT_NE(scheduler.loader_fac_weak_, nullptr);
  scheduler.cur_parent_ = std::make_shared<PRPPReqInfoTreeNode>();
  std::shared_ptr<PRPPReqInfoTreeNode> node = std::make_shared<PRPPReqInfoTreeNode>();
  scheduler.cur_parent_->children_.push_back(node);
  scheduler.cur_node_iter_ = scheduler.cur_parent_->children_.begin();
  (*scheduler.cur_node_iter_)->req_info_ = std::make_shared<PRRequestInfo>();
  (*scheduler.cur_node_iter_)->req_info_->set_method("ABC");
  EXPECT_EQ((*scheduler.cur_node_iter_)->req_info_->method_, "ABC");
  scheduler.SchedulePrerequests(1, 0);

  scheduler.cur_parent_ = std::make_shared<PRPPReqInfoTreeNode>();
  scheduler.cur_parent_->children_.push_back(node);
  scheduler.cur_node_iter_ = scheduler.cur_parent_->children_.begin();
  (*scheduler.cur_node_iter_)->req_info_ = std::make_shared<PRRequestInfo>();
  (*scheduler.cur_node_iter_)->req_info_->set_method("GET");
  (*scheduler.cur_node_iter_)->req_info_->set_preload_flag(PRRequestFlags::PRPP_FLAGS_HDR_NOT_MATCH);
  EXPECT_EQ((*scheduler.cur_node_iter_)->req_info_->preload_flags_, PRRequestFlags::PRPP_FLAGS_HDR_NOT_MATCH);
  scheduler.SchedulePrerequests(1, 0);

  scheduler.cur_parent_ = std::make_shared<PRPPReqInfoTreeNode>();
  scheduler.cur_parent_->children_.push_back(node);
  scheduler.cur_node_iter_ = scheduler.cur_parent_->children_.begin();
  (*scheduler.cur_node_iter_)->req_info_ = std::make_shared<PRRequestInfo>();
  (*scheduler.cur_node_iter_)->req_info_->set_method("GET");
  (*scheduler.cur_node_iter_)->req_info_->set_preload_flag(PRRequestFlags::PRPP_FLAGS_UNSUPPORT);
  EXPECT_EQ((*scheduler.cur_node_iter_)->req_info_->preload_flags_, PRRequestFlags::PRPP_FLAGS_UNSUPPORT);
  scheduler.SchedulePrerequests(1, 0);
}

} // namespace ohos_prp_preload