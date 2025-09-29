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

#include "arkweb/chromium_ext/services/network/prp_preload/src/page_res_parallel_preload_mgr_impl.h"
#include "base/test/bind.h"
#include "base/test/task_environment.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_builder.h"
#include "services/network/public/mojom/network_context.mojom.h"

namespace ohos_prp_preload {

class PRParallelPreloadMgrImplTest : public testing::Test {
public:
 static void SetUpTestCase();
 static void TearDownTestCase();
 void SetUp();
 void TearDown();
protected:
 std::unique_ptr<base::test::TaskEnvironment> task_environment_;
};

void PRParallelPreloadMgrImplTest::SetUpTestCase() {}

void PRParallelPreloadMgrImplTest::TearDownTestCase() {}

void PRParallelPreloadMgrImplTest::SetUp() {
  task_environment_ = std::make_unique<base::test::TaskEnvironment>(
    base::test::TaskEnvironment::MainThreadType::DEFAULT);
}

void PRParallelPreloadMgrImplTest::TearDown() {
  task_environment_.reset();
}

TEST_F(PRParallelPreloadMgrImplTest, PRParallelPreloadMgrImplTest_001)
{
  PRParallelPreloadMgrImpl preload_mgr;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_nullptr = nullptr;
  preload_mgr.Init(task_runner_nullptr);
  EXPECT_FALSE(preload_mgr.is_inited_);
  auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  preload_mgr.Init(task_runner.get());
  EXPECT_TRUE(preload_mgr.is_inited_);
  preload_mgr.Init(task_runner.get());
}

TEST_F(PRParallelPreloadMgrImplTest, PRParallelPreloadMgrImplTest_002)
{
  PRParallelPreloadMgrImpl preload_mgr;
  net::URLRequestContextBuilder context_builder;
  std::unique_ptr<net::URLRequestContext> url_request_context = context_builder.Build();
  net::NetworkAnonymizationKey networkAnonymizationKey = net::NetworkAnonymizationKey();
  void* navigator = static_cast<void*>(new int(0));
  bool callback_called = false;
  std::string origin_callback;
  preload_mgr.StartPage(
      "page", networkAnonymizationKey, url_request_context->GetWeakPtr(),
      reinterpret_cast<uint64_t>(navigator),
      base::BindLambdaForTesting([&](const std::string& origin) {
        callback_called = true;
        origin_callback = origin;
        return;
      }),
      true);
  EXPECT_EQ(origin_callback, "origin.DEFAULT.Cancel");
  EXPECT_TRUE(callback_called);

  preload_mgr.is_inited_ = true;
  preload_mgr.StartPage("page", networkAnonymizationKey, url_request_context->GetWeakPtr(),
    reinterpret_cast<uint64_t>(navigator), base::BindOnce([](const std::string& origin) {
      return;}), false);
  EXPECT_TRUE(preload_mgr.prp_preload_info_map_["page"].prpp_req_loader_fac_ != nullptr);

  preload_mgr.prp_preload_info_map_["page"].start_page_ = true;
  preload_mgr.StartPage("page", networkAnonymizationKey, url_request_context->GetWeakPtr(),
    reinterpret_cast<uint64_t>(navigator), base::BindOnce([](const std::string& origin) {
      return;}), false);

  preload_mgr.StartPage("page_web_handle", networkAnonymizationKey, url_request_context->GetWeakPtr(),
    reinterpret_cast<uint64_t>(navigator), base::BindOnce([](const std::string& origin) {
      return;}), false);

  preload_mgr.web_handle_pages_map_.emplace(navigator, "page_web_handle");
  preload_mgr.prp_page_url_.emplace("page_web_handle", "page_web_handle_redirect");
  preload_mgr.StartPage("page_web_handle", networkAnonymizationKey, url_request_context->GetWeakPtr(),
    reinterpret_cast<uint64_t>(navigator), base::BindOnce([](const std::string& origin) {
      return;}), false);

  preload_mgr.web_handle_pages_map_.emplace(navigator, "page_web_handle");
  preload_mgr.prp_preload_info_map_["page_web_handle_redirect"].start_page_ = true;
  preload_mgr.prp_page_url_.emplace("page_web_handle_already_started", "page_web_handle_redirect");
  preload_mgr.StartPage("page_web_handle_already_started", networkAnonymizationKey, url_request_context->GetWeakPtr(),
    reinterpret_cast<uint64_t>(static_cast<void*>(new int(0))), base::BindOnce([](const std::string& origin) {
      return;}), false);

  preload_mgr.prp_preload_info_map_.clear();
  preload_mgr.web_handle_pages_map_.clear();
  preload_mgr.prp_page_url_.clear();
  preload_mgr.web_handle_pages_map_.emplace(navigator, "page");
  preload_mgr.prp_page_url_.emplace("page", "page_redirect");
  preload_mgr.prp_preload_info_map_["page_redirect"].start_page_ = true;
  preload_mgr.StartPage("page", networkAnonymizationKey, url_request_context->GetWeakPtr(),
    reinterpret_cast<uint64_t>(navigator), base::BindOnce([](const std::string& origin) {
      return;}), false);
}

TEST_F(PRParallelPreloadMgrImplTest, PRParallelPreloadMgrImplTest_003)
{
  PRParallelPreloadMgr& preloadMgr = PRParallelPreloadMgr::GetInstance();
  PRParallelPreloadMgrImpl preload_mgr;
  preload_mgr.is_inited_ = true;
  preload_mgr.StopPage(0);

  void* navigator = static_cast<void*>(new int(0));
  preload_mgr.StopPage(reinterpret_cast<uint64_t>(navigator));
  preload_mgr.web_handle_pages_map_.emplace(navigator, "page");
  EXPECT_EQ(preload_mgr.web_handle_pages_map_.size(), 1);
  preload_mgr.StopPage(reinterpret_cast<uint64_t>(navigator));
  EXPECT_EQ(preload_mgr.web_handle_pages_map_.size(), 0);
}

TEST_F(PRParallelPreloadMgrImplTest, PRParallelPreloadMgrImplTest_004)
{
  PRParallelPreloadMgrImpl preload_mgr;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  preload_mgr.UpdateResRequestInfo("page", info);
  preload_mgr.UpdateRedirectUrl("page", "page_redirect");
  url::Origin frame_origin = url::Origin::Create(GURL("frame_origin"));
  net::IsolationInfo isolation_info = net::IsolationInfo::Create(net::IsolationInfo::RequestType::kOther,
    frame_origin, frame_origin, net::SiteForCookies::FromOrigin(frame_origin));
  preload_mgr.SetPageOrigin("page", isolation_info);
  preload_mgr.OnPageOrigin("page", "page_origin");
  network::mojom::URLLoaderFactoryParamsPtr params = network::mojom::URLLoaderFactoryParams::New();
  preload_mgr.SetURLLoaderFactoryParam(std::move(params));
  EXPECT_EQ(preload_mgr.GetRequestLoaderFactory("page"), nullptr);
  preload_mgr.UpdateIdlePrerequestCount("page");
  base::Time start_time = base::Time::Now();
  base::Time end_time = base::Time::Now();
  preload_mgr.RemoveCache(start_time, end_time);
  preload_mgr.StopPageInternal("page");
  preload_mgr.OnRPPCtrlerTimeout("page");
  preload_mgr.DoRPPCtrlerTimeout("page");
  preload_mgr.DoRmPageUrl("page");

  preload_mgr.is_inited_ = true;
  preload_mgr.UpdateRedirectUrl("page", "page_redirect", true);
  EXPECT_EQ(preload_mgr.GetRequestLoaderFactory("page"), nullptr);

  preload_mgr.prp_preload_info_map_["page"].rp_preload_ctrler_ = base::WrapRefCounted(
    new (std::nothrow) ResParallelPreloadCtrler("page", net::NetworkAnonymizationKey(),
    base::SingleThreadTaskRunner::GetCurrentDefault(),
    base::BindRepeating([](const std::string& origin) {
      return;})));
  net::URLRequestContextBuilder context_builder;
  std::unique_ptr<net::URLRequestContext> url_request_context = context_builder.Build();
  std::shared_ptr<PRPPRequestLoaderFactory> factory =
    PRPPRequestLoaderFactory::CreatePRPPRequestLoaderFactory("page", url_request_context->GetWeakPtr());
  preload_mgr.prp_preload_info_map_["page"].prpp_req_loader_fac_ = factory;
  preload_mgr.StopPageInternal("page");

  preload_mgr.prp_preload_info_map_["page"].start_page_ = true;
  preload_mgr.prp_preload_info_map_["page"].rp_preload_ctrler_ = base::WrapRefCounted(
    new (std::nothrow) ResParallelPreloadCtrler("page", net::NetworkAnonymizationKey(),
    base::SingleThreadTaskRunner::GetCurrentDefault(),
    base::BindRepeating([](const std::string& origin) {
      return;})));
  preload_mgr.prp_preload_info_map_["page"].prpp_req_loader_fac_ = factory;

  preload_mgr.UpdateIdlePrerequestCount("page");
  preload_mgr.OnPageOrigin("page", "");
  preload_mgr.OnPageOrigin("page", "page_origin");
  preload_mgr.UpdateResRequestInfo("page", info);
  preload_mgr.SetPageOrigin("page", isolation_info);
  network::mojom::URLLoaderFactoryParamsPtr params_for_is_inited = network::mojom::URLLoaderFactoryParams::New();
  params_for_is_inited->main_url = "page";
  preload_mgr.prp_preload_info_map_["page"].page_origin_ready_ = false;
  preload_mgr.SetURLLoaderFactoryParam(std::move(params_for_is_inited));
  EXPECT_EQ(preload_mgr.GetRequestLoaderFactory("page").get(), factory->GetWeak().get());

  preload_mgr.prp_page_url_.emplace("page", "page_redirect");
  preload_mgr.prp_preload_info_map_["page_redirect"].start_page_ = true;
  preload_mgr.UpdateRedirectUrl("page", "page_redirect", true);

  preload_mgr.prp_page_url_.emplace("page", "page_redirect");
  preload_mgr.prp_preload_info_map_["page_redirect"].start_page_ = false;
  preload_mgr.UpdateRedirectUrl("page", "page_redirect", true);

  preload_mgr.prp_page_url_.emplace("page_another", "page_another_redirect");
  preload_mgr.DoRmPageUrl("page");
  preload_mgr.DoRmPageUrl("page_another_redirect");

  preload_mgr.net_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  preload_mgr.OnRPPCtrlerTimeout("page");

  preload_mgr.sth_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  preload_mgr.RemoveCache(start_time, end_time);
  preload_mgr.prp_preload_info_map_["page"].rp_preload_ctrler_ = base::WrapRefCounted(
    new (std::nothrow) ResParallelPreloadCtrler("page", net::NetworkAnonymizationKey(),
    base::SingleThreadTaskRunner::GetCurrentDefault(),
    base::BindRepeating([](const std::string& origin) {
      return;})));
  preload_mgr.prp_preload_info_map_["page"].start_page_ = true;
  preload_mgr.StopPageInternal("page");
}

TEST_F(PRParallelPreloadMgrImplTest, PRParallelPreloadMgrImplTest_005)
{
  PRParallelPreloadMgrImpl preload_mgr;
  preload_mgr.FindCurPreloadInfoAndRun("page", [&](PRParallelPreloadMgrImpl::PRParallelPreloadInfo&){});

  preload_mgr.prp_page_url_.emplace("page", "page_redirect");
  preload_mgr.FindCurPreloadInfoAndRun("page", [&](PRParallelPreloadMgrImpl::PRParallelPreloadInfo&){});

  preload_mgr.prp_preload_info_map_["page"].start_page_ = false;
  preload_mgr.FindCurPreloadInfoAndRun("page", [&](PRParallelPreloadMgrImpl::PRParallelPreloadInfo&){});

  preload_mgr.prp_preload_info_map_["page"].start_page_ = true;
  bool callback_called = false;
  preload_mgr.FindCurPreloadInfoAndRun("page", [&](PRParallelPreloadMgrImpl::PRParallelPreloadInfo& info){
    callback_called = true;
    info.start_page_ = false;
  });
  EXPECT_TRUE(callback_called);
  EXPECT_FALSE(preload_mgr.prp_preload_info_map_["page"].start_page_);
}

TEST_F(PRParallelPreloadMgrImplTest, PRParallelPreloadMgrImplTest_006)
{
  PRParallelPreloadMgrImpl preload_mgr;
  PageOriginCallback callback_empty;
  PageOriginCallback callback = base::BindOnce([](const std::string& origin){});
  bool is_started = preload_mgr.IsPageAlreadyStarted("page", callback_empty);
  EXPECT_FALSE(is_started);

  preload_mgr.prp_preload_info_map_["page"].start_page_ = true;
  is_started = preload_mgr.IsPageAlreadyStarted("page", callback);
  EXPECT_TRUE(is_started);
  preload_mgr.IsPageAlreadyStarted("page", callback_empty);
}

} // namespace ohos_prp_preload