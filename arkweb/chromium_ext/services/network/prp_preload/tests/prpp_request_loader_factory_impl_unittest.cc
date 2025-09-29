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

#include "arkweb/chromium_ext/services/network/prp_preload/src/preload_runner/prpp_request_loader_factory_impl.h"
#include "base/test/task_environment.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_builder.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/test/url_loader_context_for_tests.h"
#include "net/proxy_resolution/configured_proxy_resolution_service.h"

namespace ohos_prp_preload {

class PRPPRequestLoaderFactoryImplTest : public testing::Test {
public:
 static void SetUpTestCase();
 static void TearDownTestCase();
 void SetUp();
 void TearDown();
protected:
 std::unique_ptr<base::test::TaskEnvironment> task_environment_;
};

void PRPPRequestLoaderFactoryImplTest::SetUpTestCase() {}

void PRPPRequestLoaderFactoryImplTest::TearDownTestCase() {}

void PRPPRequestLoaderFactoryImplTest::SetUp() {
  task_environment_ = std::make_unique<base::test::TaskEnvironment>(
    base::test::TaskEnvironment::MainThreadType::DEFAULT);
}

void PRPPRequestLoaderFactoryImplTest::TearDown() {
  task_environment_.reset();
}

TEST_F(PRPPRequestLoaderFactoryImplTest, PRPPRequestLoaderFactoryImplTest_001)
{
  net::URLRequestContextBuilder context_builder;
  std::unique_ptr<net::URLRequestContext> url_request_context = context_builder.Build();
  std::shared_ptr<PRPPRequestLoaderFactory> factory_shared =
    PRPPRequestLoaderFactory::CreatePRPPRequestLoaderFactory("page",
    url_request_context->GetWeakPtr());
  EXPECT_TRUE(factory_shared != nullptr);
  
  PRPPRequestLoaderFactoryImpl factory =
    PRPPRequestLoaderFactoryImpl("page", url_request_context->GetWeakPtr());
  std::shared_ptr<PRRequestInfo> info_nullptr = nullptr;
  std::set<std::string> need_record_header_urls;
  factory.CreateReqLoaderAndStart(info_nullptr, need_record_header_urls);

  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  info->set_url(GURL("https://some.other.origin/echoall"));
  info->set_preload_flag(PRPP_FLAGS_HDR_DYNAMIC);
  factory.CreateReqLoaderAndStart(info, need_record_header_urls);

  info->set_parent_for_dynamic_header(GURL("https://some.other.origin/echoall"));
  (void)factory.can_reuse_headers_map_.emplace("https://some.other.origin/echoall",
    net::HttpRequestHeaders());
  factory.CreateReqLoaderAndStart(info, need_record_header_urls);

  (void)need_record_header_urls.insert("https://some.other.origin/echoall");
  factory.CreateReqLoaderAndStart(info, need_record_header_urls);
  EXPECT_EQ(factory.need_record_header_urls_.count("https://some.other.origin/echoall"), 1);

  (void)factory.requests_already_start_set_.emplace("https://some.other.origin/echoall");
  factory.CreateReqLoaderAndStart(info, need_record_header_urls);
}

TEST_F(PRPPRequestLoaderFactoryImplTest, PRPPRequestLoaderFactoryImplTest_002)
{
  net::URLRequestContextBuilder context_builder;
  std::unique_ptr<net::URLRequestContext> url_request_context = context_builder.Build();
  PRPPRequestLoaderFactoryImpl factory =
    PRPPRequestLoaderFactoryImpl("page", url_request_context->GetWeakPtr());
  network::URLLoaderContextForTests url_loader_context_for_tests;
  network::ResourceRequest resource_request;
  resource_request.method = "GET";
  resource_request.url = GURL("https://some.other.origin/echoall");
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  info->set_method("GET");
  std::shared_ptr<PRRequestInfo> info_binding = std::make_shared<PRRequestInfo>();
  std::shared_ptr<PRPPRequestLoader> loader = PRPPRequestLoader::CreatePRPPRequestLoader(
    "page", info, net::IsolationInfo(), url_request_context->GetWeakPtr(),
    base::BindOnce(&PRPPRequestLoaderFactoryImpl::OnResPreloaded, factory.weak_factory_.GetWeakPtr()));
  (void)factory.prpp_req_loaders_.emplace("https://some.other.origin/echoall", loader);
  EXPECT_EQ(factory.GetPRPPReqLoader(url_loader_context_for_tests, resource_request,
    info_binding), loader);

  EXPECT_EQ(factory.GetPRPPReqLoader(url_loader_context_for_tests, resource_request,
    info_binding), nullptr);

  std::shared_ptr<PRPPRequestLoader> loader_nullptr = nullptr;
  (void)factory.prpp_req_loaders_.emplace("https://some.other.origin/echoall", loader_nullptr);
  EXPECT_EQ(factory.GetPRPPReqLoader(url_loader_context_for_tests, resource_request,
    info_binding), nullptr);

  resource_request.is_preflight = true;
  resource_request.method = "HEAD";
  (void)factory.prpp_req_loaders_.emplace("preflight+https://some.other.origin/echoall", loader);
  EXPECT_EQ(factory.GetPRPPReqLoader(url_loader_context_for_tests, resource_request,
    info_binding), nullptr);

  url::Origin frame_origin = url::Origin::Create(GURL("frame_origin"));
  net::IsolationInfo isolation_info = net::IsolationInfo::Create(net::IsolationInfo::RequestType::kOther,
    frame_origin, frame_origin, net::SiteForCookies::FromOrigin(frame_origin));
  factory.SetPRPPIsolation(isolation_info);
  url_loader_context_for_tests.mutable_factory_params().isolation_info = isolation_info;
  EXPECT_EQ(factory.GetPRPPReqLoader(url_loader_context_for_tests, resource_request,
    info_binding), nullptr);

  resource_request.method = "POST";
  EXPECT_EQ(factory.GetPRPPReqLoader(url_loader_context_for_tests, resource_request,
    info_binding), nullptr);
}

TEST_F(PRPPRequestLoaderFactoryImplTest, PRPPRequestLoaderFactoryImplTest_003)
{
  net::URLRequestContextBuilder context_builder;
  std::unique_ptr<net::URLRequestContext> url_request_context = context_builder.Build();
  PRPPRequestLoaderFactoryImpl factory =
    PRPPRequestLoaderFactoryImpl("page", url_request_context->GetWeakPtr());
  network::URLLoaderContextForTests url_loader_context_for_tests;
  network::ResourceRequest resource_request;
  std::shared_ptr<PRRequestInfo> info_nullptr = nullptr;
  std::shared_ptr<PRRequestInfo> info_binding_nullptr = nullptr;
  EXPECT_EQ(factory.IsInfoMatched(info_nullptr, url_loader_context_for_tests, resource_request,
    info_binding_nullptr), false);

  std::shared_ptr<PRRequestInfo> info_resource = std::make_shared<PRRequestInfo>();
  std::shared_ptr<PRRequestInfo> info_resource_binding = std::make_shared<PRRequestInfo>();
  resource_request.method = "GET";
  info_resource->set_method("GET");
  EXPECT_EQ(factory.IsInfoMatched(info_resource, url_loader_context_for_tests, resource_request,
    info_resource_binding), true);

  resource_request.trust_token_params = network::OptionalTrustTokenParams(
    network::mojom::TrustTokenParams::New());
  EXPECT_EQ(factory.IsInfoMatched(info_resource, url_loader_context_for_tests, resource_request,
    info_resource_binding), false);

  resource_request.request_body = scoped_refptr<network::ResourceRequestBody>(
    new network::ResourceRequestBody());
  EXPECT_EQ(factory.IsInfoMatched(info_resource, url_loader_context_for_tests, resource_request,
    info_resource_binding), false);

  info_resource->set_method("POST");
  EXPECT_EQ(factory.IsInfoMatched(info_resource, url_loader_context_for_tests, resource_request,
    info_resource_binding), false);
}

TEST_F(PRPPRequestLoaderFactoryImplTest, PRPPRequestLoaderFactoryImplTest_004)
{
  net::URLRequestContextBuilder context_builder;
  std::unique_ptr<net::URLRequestContext> url_request_context = context_builder.Build();
  PRPPRequestLoaderFactoryImpl factory =
    PRPPRequestLoaderFactoryImpl("page", url_request_context->GetWeakPtr());

  network::ResourceRequest resource_request;
  std::shared_ptr<PRRequestInfo> info_resource = std::make_shared<PRRequestInfo>();
  std::shared_ptr<PRRequestInfo> info_resource_binding = std::make_shared<PRRequestInfo>();
  EXPECT_EQ(factory.MatchRequestHeaders(info_resource, resource_request, info_resource_binding), true);

  net::HttpRequestHeaders resource_request_headers;
  resource_request_headers.SetHeader("header_key_1", "header_value_1");
  resource_request.headers = resource_request_headers;
  EXPECT_EQ(factory.MatchRequestHeaders(info_resource, resource_request, info_resource_binding), false);

  net::HttpRequestHeaders info_resource_headers;
  info_resource_headers.SetHeader("header_key_1", "header_value_1");
  info_resource->set_extra_request_headers(info_resource_headers);
  EXPECT_EQ(factory.MatchRequestHeaders(info_resource, resource_request, info_resource_binding), true);

  resource_request_headers.SetHeader("header_key_2", "header_value_2");
  resource_request.headers = resource_request_headers;
  info_resource_headers.SetHeader("header_key_2", "header_value_not_2");
  info_resource->set_extra_request_headers(info_resource_headers);
  EXPECT_EQ(factory.MatchRequestHeaders(info_resource, resource_request, info_resource_binding), false);
}

TEST_F(PRPPRequestLoaderFactoryImplTest, PRPPRequestLoaderFactoryImplTest_005)
{
  net::URLRequestContextBuilder context_builder;
  std::unique_ptr<net::URLRequestContext> url_request_context = context_builder.Build();
  PRPPRequestLoaderFactoryImpl factory =
    PRPPRequestLoaderFactoryImpl("page", url_request_context->GetWeakPtr());
  EXPECT_EQ(factory.GetMainUrl(), "page");

  PRPPRequestLoader* loader_nullptr = nullptr;
  factory.OnResPreloaded(loader_nullptr);

  std::shared_ptr<PRPPRequestLoader> loader = PRPPRequestLoader::CreatePRPPRequestLoader(
    "page", std::make_shared<PRRequestInfo>(), net::IsolationInfo(), url_request_context->GetWeakPtr(),
    base::BindOnce(&PRPPRequestLoaderFactoryImpl::OnResPreloaded, factory.weak_factory_.GetWeakPtr()));
  factory.OnResPreloaded(loader.get());

  net::IsolationInfo isolation_info;
  factory.SetPRPPIsolation(isolation_info);

  url::Origin frame_origin = url::Origin::Create(GURL("frame_origin"));
  factory.SetPRPPIsolation(net::IsolationInfo::Create(net::IsolationInfo::RequestType::kOther,
    frame_origin, frame_origin, net::SiteForCookies::FromOrigin(frame_origin)));

  factory.DoPendingCreateTask("https://some.other.origin/parent");
  std::shared_ptr<PRRequestInfo> info_resource = std::make_shared<PRRequestInfo>();
  info_resource->set_parent_for_dynamic_header(GURL("https://some.other.origin/parent"));
  factory.RecordPendingPRPPLoader(info_resource);
  factory.RecordPendingPRPPLoader(info_resource);
  factory.DoPendingCreateTask("https://some.other.origin/parent");

  info_resource->set_url(GURL("https://some.other.origin/echoall"));
  factory.CreatePendingReqLoaderAndStart(info_resource);

  (void)factory.can_reuse_headers_map_.emplace("https://some.other.origin/parent",
    net::HttpRequestHeaders());
  factory.CreatePendingReqLoaderAndStart(info_resource);

  (void)factory.requests_already_start_set_.emplace("https://some.other.origin/echoall");
  factory.CreatePendingReqLoaderAndStart(info_resource);

  factory.url_request_context_ = nullptr;
  factory.CreatePendingReqLoaderAndStart(info_resource);

  net::HttpRequestHeaders resource_request_headers;
  resource_request_headers.SetHeader("header_key_1", "header_value_1");
  info_resource->set_dynamic_header_keys(std::set<std::string>({"header_key_1"}));
  factory.ReplaceHeaders(resource_request_headers, info_resource);

  (void)factory.need_record_header_urls_.insert("https://some.other.origin/echoall");
  network::ResourceRequest resource_request;
  resource_request.url = GURL("https://some.other.origin/echoall");
  factory.RecordNeedRecordHeaderUrls(resource_request);
}

} // namespace ohos_prp_preload