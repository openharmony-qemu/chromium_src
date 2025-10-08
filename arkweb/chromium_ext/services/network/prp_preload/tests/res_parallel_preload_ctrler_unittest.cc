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

#include <string>

#include "base/test/task_environment.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "net/url_request/url_request_context_builder.h"
#include "net/url_request/url_request_test_util.h"

#define private public
#include "arkweb/chromium_ext/services/network/prp_preload/src/res_parallel_preload_ctrler.h"
#undef private

namespace ohos_prp_preload {

class ResParallelPreloadCtrlerTest : public ::testing::Test {
 public:
  static void SetUpTestCase();
  static void TearDownTestCase();
  void SetUp();
  void TearDown();

 protected:
  base::test::SingleThreadTaskEnvironment task_environment_{
      base::test::TaskEnvironment::MainThreadType::DEFAULT,
      base::test::TaskEnvironment::TimeSource::MOCK_TIME};
};

void ResParallelPreloadCtrlerTest::SetUpTestCase() {}

void ResParallelPreloadCtrlerTest::TearDownTestCase() {}

void ResParallelPreloadCtrlerTest::SetUp() {}

void ResParallelPreloadCtrlerTest::TearDown() {}

TEST_F(ResParallelPreloadCtrlerTest, ConstructorTest) {
  std::string url = "http://test.url";
  net::NetworkAnonymizationKey key;
  auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
  ResParallelPreloadCtrler ctrler(url, key, task_runner.get(), timeout_cb);
  EXPECT_TRUE(ctrler.sth_task_runner_ != nullptr);
  EXPECT_TRUE(ctrler.url_ == url);
}

TEST_F(ResParallelPreloadCtrlerTest, InitDiskCacheBackendFactoryTest01) {
  {
    std::string url = "http://test.url";
    net::NetworkAnonymizationKey key;
    PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
    ResParallelPreloadCtrler ctrler(url, key, nullptr, timeout_cb);
    net::URLRequestContextBuilder builder;
    builder.set_enable_brotli(false);
    builder.set_enable_zstd(false);
    auto url_request_context = builder.Build();
    ctrler.Init(nullptr, url_request_context->GetWeakPtr(), base::DoNothing());
    ctrler.Start();
    ctrler.Stop();
    EXPECT_TRUE(ctrler.url_ == url);
    base::RunLoop().RunUntilIdle();
  }
}
TEST_F(ResParallelPreloadCtrlerTest, InitDiskCacheBackendFactoryTest02) {
  {
    std::string url = "http://test.url";
    net::NetworkAnonymizationKey key;
    auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
    PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
    ResParallelPreloadCtrler ctrler(url, key, task_runner.get(), timeout_cb);
    net::URLRequestContextBuilder builder;
    builder.set_enable_brotli(false);
    builder.set_enable_zstd(false);
    auto url_request_context = builder.Build();
    ctrler.Init(task_runner, url_request_context->GetWeakPtr(),
                base::DoNothing());
    ctrler.Start();
    task_environment_.FastForwardBy(base::Seconds(5));
    base::RunLoop().RunUntilIdle();
    ctrler.Stop();
    EXPECT_TRUE(ctrler.url_ == url);
    base::RunLoop().RunUntilIdle();
  }
}

TEST_F(ResParallelPreloadCtrlerTest, InitDiskCacheBackendFactoryTest03) {
  {
    std::string url = "http://test.url";
    net::NetworkAnonymizationKey key;
    auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
    PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
    ResParallelPreloadCtrler ctrler(url, key, task_runner.get(), timeout_cb);
    net::URLRequestContextBuilder builder;
    builder.set_enable_brotli(false);
    builder.set_enable_zstd(false);
    auto url_request_context = builder.Build();
    ctrler.InitDiskCacheBackendFactoryForTest();
    ctrler.Init(task_runner, url_request_context->GetWeakPtr(),
                base::DoNothing());
    ctrler.Start();
    ctrler.Stop();
    EXPECT_TRUE(ctrler.url_ == url);
    base::RunLoop().RunUntilIdle();
    ctrler.ResetDiskCacheBackendFactoryForTest();
  }
}

TEST_F(ResParallelPreloadCtrlerTest, RemoveCacheTest) {
  std::string url = "http://test.url";
  net::NetworkAnonymizationKey key;
  auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
  ResParallelPreloadCtrler ctrler(url, key, task_runner.get(), timeout_cb);
  ctrler.InitDiskCacheBackendFactoryForTest();
  auto startTime = base::Time::Now();
  auto duration = base::Milliseconds(10);
  auto endTime = startTime + duration;
  ResParallelPreloadCtrler::RemoveCache(startTime, endTime);
  EXPECT_EQ(ctrler.url_, url);
  ctrler.ResetDiskCacheBackendFactoryForTest();
}

TEST_F(ResParallelPreloadCtrlerTest, UpdateResRequestInfoTest03) {
  {
    std::string url = "http://test.url";
    net::NetworkAnonymizationKey key;
    PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
    ResParallelPreloadCtrler ctrler(url, key, nullptr, timeout_cb);
    auto info = std::make_shared<PRRequestInfo>();
    ctrler.UpdateResRequestInfo(info);
    EXPECT_EQ(ctrler.url_, url);
  }

  {
    std::string url = "http://test.url";
    net::NetworkAnonymizationKey key;
    auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
    PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
    ResParallelPreloadCtrler ctrler(url, key, task_runner.get(), timeout_cb);
    auto info = std::make_shared<PRRequestInfo>();
    ctrler.UpdateResRequestInfo(info);
    base::RunLoop().RunUntilIdle();
    EXPECT_EQ(ctrler.filter_count_, 1);
  }
}

TEST_F(ResParallelPreloadCtrlerTest, UpdateResRequestInfoTest04) {
  std::string url = "http://test.url";
  net::NetworkAnonymizationKey key;
  auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
  ResParallelPreloadCtrler ctrler(url, key, task_runner.get(), timeout_cb);
  uint32_t greater_than_max_filter_count = 31;
  ctrler.filter_count_ = greater_than_max_filter_count;
  auto info = std::make_shared<PRRequestInfo>();
  ctrler.UpdateResRequestInfo(info);
  base::RunLoop().RunUntilIdle();
  EXPECT_EQ(ctrler.filter_count_, greater_than_max_filter_count);
}

TEST_F(ResParallelPreloadCtrlerTest, UpdateResRequestInfoTest05) {
  std::string url = "http://test.url";
  net::NetworkAnonymizationKey key;
  auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
  ResParallelPreloadCtrler ctrler(url, key, task_runner.get(), timeout_cb);
  uint32_t max_filter_count = 29;
  ctrler.filter_count_ = max_filter_count;
  auto discache_factory =
      base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  ResPreloadInfosCB test_cb = base::BindRepeating(
      [](const PRPPPreconnectInfoList& preconnect_info_list,
         const std::shared_ptr<PRPPReqInfoTreeNode>& preload_info_tree,
         const std::set<std::string>& need_record_header_urls) {});
  auto res_req_info_updater = base::MakeRefCounted<ResRequestInfoUpdater>(
      url, key, discache_factory, test_cb);
  ctrler.res_req_info_updater_ = res_req_info_updater;
  auto info = std::make_shared<PRRequestInfo>();
  ctrler.UpdateResRequestInfo(info);
  base::RunLoop().RunUntilIdle();
  EXPECT_EQ(ctrler.filter_count_, max_filter_count + 1);
}

TEST_F(ResParallelPreloadCtrlerTest, SetPageOriginTest01) {
  {
    std::string url = "http://test.url";
    net::NetworkAnonymizationKey key;
    PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
    ResParallelPreloadCtrler ctrler(url, key, nullptr, timeout_cb);
    ctrler.SetPageOrigin("test_origin");
    EXPECT_EQ(ctrler.url_, url);
  }
  {
    std::string url = "http://test.url";
    net::NetworkAnonymizationKey key;
    PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
    auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
    ResParallelPreloadCtrler ctrler(url, key, task_runner.get(), timeout_cb);
    ctrler.SetPageOrigin("test_origin");
    EXPECT_EQ(ctrler.res_req_info_updater_, nullptr);
    base::RunLoop().RunUntilIdle();
  }
}

TEST_F(ResParallelPreloadCtrlerTest, SetPageOriginTest02) {
  std::string url = "http://test.url";
  net::NetworkAnonymizationKey key;
  PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
  auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  ResParallelPreloadCtrler ctrler(url, key, task_runner.get(), timeout_cb);
  auto discache_factory =
      base::WrapRefCounted(new (std::nothrow) DiskCacheBackendFactory());
  ResPreloadInfosCB test_cb = base::BindRepeating(
      [](const PRPPPreconnectInfoList& preconnect_info_list,
         const std::shared_ptr<PRPPReqInfoTreeNode>& preload_info_tree,
         const std::set<std::string>& need_record_header_urls) {});
  auto res_req_info_updater = base::MakeRefCounted<ResRequestInfoUpdater>(
      url, key, discache_factory, test_cb);
  ctrler.res_req_info_updater_ = res_req_info_updater;
  ctrler.res_req_info_updater_->res_req_info_cache_mgr_->is_start_ = true;
  ctrler.SetPageOrigin("test_origin");
  base::RunLoop().RunUntilIdle();
  EXPECT_EQ(ctrler.res_req_info_updater_->res_req_info_cache_mgr_->page_origin_,
            "test_origin");
}

TEST_F(ResParallelPreloadCtrlerTest, SetPRPPReqLoaderFacTest01) {
  {
    std::string url = "http://test.url";
    net::NetworkAnonymizationKey key;
    PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
    ResParallelPreloadCtrler ctrler(url, key, nullptr, timeout_cb);
    ctrler.SetPRPPReqLoaderFac(nullptr);
    EXPECT_EQ(ctrler.url_, url);
  }
  {
    std::string url = "http://test.url";
    net::NetworkAnonymizationKey key;
    PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
    auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
    ResParallelPreloadCtrler ctrler(url, key, task_runner.get(), timeout_cb);
    ctrler.SetPRPPReqLoaderFac(nullptr);
    EXPECT_EQ(ctrler.res_preload_scheduler_, nullptr);
    base::RunLoop().RunUntilIdle();
  }
}

TEST_F(ResParallelPreloadCtrlerTest, SetPRPPReqLoaderFacTest02) {
  std::string url = "http://test.url";
  net::NetworkAnonymizationKey key;
  PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
  auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  ResParallelPreloadCtrler ctrler(url, key, task_runner.get(), timeout_cb);
  net::URLRequestContextBuilder builder;
  std::unique_ptr<net::URLRequestContext> url_request_context(builder.Build());
  EXPECT_NE(url_request_context, nullptr);
  PRPPOnPageOriginCB on_page_origin_cb;
  auto res_preload_scheduler = base::MakeRefCounted<ResPreloadScheduler>(
      url, task_runner.get(), url_request_context->GetWeakPtr(),
      on_page_origin_cb);
  ctrler.res_preload_scheduler_ = res_preload_scheduler;
  std::shared_ptr<PRPPRequestLoaderFactory> factory =
      PRPPRequestLoaderFactory::CreatePRPPRequestLoaderFactory(
          "page", url_request_context->GetWeakPtr());
  EXPECT_EQ(ctrler.res_preload_scheduler_->loader_fac_weak_, nullptr);
  ctrler.SetPRPPReqLoaderFac(factory->GetWeak());
  base::RunLoop().RunUntilIdle();
  EXPECT_NE(ctrler.res_preload_scheduler_->loader_fac_weak_, nullptr);
}

TEST_F(ResParallelPreloadCtrlerTest, UpdateIdlePrerequestCountTest01) {
  {
    std::string url = "http://test.url";
    net::NetworkAnonymizationKey key;
    PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
    ResParallelPreloadCtrler ctrler(url, key, nullptr, timeout_cb);
    ctrler.UpdateIdlePrerequestCount();
    EXPECT_EQ(ctrler.url_, url);
  }

  {
    std::string url = "http://test.url";
    net::NetworkAnonymizationKey key;
    PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
    auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
    ResParallelPreloadCtrler ctrler(url, key, task_runner.get(), timeout_cb);
    ctrler.UpdateIdlePrerequestCount();
    EXPECT_EQ(ctrler.res_preload_scheduler_, nullptr);
    base::RunLoop().RunUntilIdle();
  }
}

TEST_F(ResParallelPreloadCtrlerTest, UpdateIdlePrerequestCountTest02) {
  std::string url = "http://test.url";
  net::NetworkAnonymizationKey key;
  PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
  auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
  ResParallelPreloadCtrler ctrler(url, key, task_runner.get(), timeout_cb);
  net::URLRequestContextBuilder builder;
  std::unique_ptr<net::URLRequestContext> url_request_context(builder.Build());
  EXPECT_NE(url_request_context, nullptr);
  PRPPOnPageOriginCB on_page_origin_cb;
  auto res_preload_scheduler = base::MakeRefCounted<ResPreloadScheduler>(
      url, task_runner.get(), url_request_context->GetWeakPtr(),
      on_page_origin_cb);
  ctrler.res_preload_scheduler_ = res_preload_scheduler;
  EXPECT_EQ(ctrler.res_preload_scheduler_->idle_prerequest_count_, 0);
  ctrler.UpdateIdlePrerequestCount();
  base::RunLoop().RunUntilIdle();
  EXPECT_EQ(ctrler.res_preload_scheduler_->idle_prerequest_count_, 1);
}

TEST_F(ResParallelPreloadCtrlerTest, OnResPreloadInfosTest) {
  {
    std::string url = "http://test.url";
    net::NetworkAnonymizationKey key;
    PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
    auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
    ResParallelPreloadCtrler ctrler(url, key, task_runner.get(), timeout_cb);
    PRPPPreconnectInfoList list;
    std::shared_ptr<PRPPReqInfoTreeNode> node =
        std::make_shared<PRPPReqInfoTreeNode>();
    std::set<std::string> urls;
    ctrler.OnResPreloadInfos(list, node, urls);
    EXPECT_EQ(ctrler.res_preload_scheduler_, nullptr);
  }
  {
    std::string url = "http://test.url";
    net::NetworkAnonymizationKey key;
    PRPPCtrlerTimeoutCB timeout_cb = base::DoNothing();
    auto task_runner = base::SingleThreadTaskRunner::GetCurrentDefault();
    ResParallelPreloadCtrler ctrler(url, key, task_runner.get(), timeout_cb);
    net::URLRequestContextBuilder builder;
    std::unique_ptr<net::URLRequestContext> url_request_context(
        builder.Build());
    EXPECT_NE(url_request_context, nullptr);
    PRPPOnPageOriginCB on_page_origin_cb;
    auto res_preload_scheduler = base::MakeRefCounted<ResPreloadScheduler>(
        url, nullptr, url_request_context->GetWeakPtr(), on_page_origin_cb);
    ctrler.res_preload_scheduler_ = res_preload_scheduler;
    PRPPPreconnectInfoList list;
    std::shared_ptr<PRPPReqInfoTreeNode> node =
        std::make_shared<PRPPReqInfoTreeNode>();
    std::set<std::string> urls;
    ctrler.OnResPreloadInfos(list, node, urls);
    EXPECT_FALSE(ctrler.res_preload_scheduler_->preload_triggered_);
  }
}

} // namespace ohos_prp_preload