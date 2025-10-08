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
#include "net/url_request/url_request_job.h"
#include "net/url_request/url_request_test_util.h"

#include "arkweb/chromium_ext/services/network/prp_preload/src/preload_runner/prpp_request_loader_impl.h"
#include "arkweb/chromium_ext/services/network/url_loader_utils.h"

namespace ohos_prp_preload {

constexpr int CACHE_BLOCK_SIZE_TEST = 64 * 1024;
constexpr int MAX_PRPP_BODY_CACHE_SIZE_TEST = 32;
constexpr int MIN_PRPP_BODY_CACHE_SIZE_TEST = 4;
constexpr int STATE_ENUM_TEST = 8;

class PRPPRequestLoaderImplTest : public testing::Test {
public:
  static void SetUpTestCase();
  static void TearDownTestCase();
  void SetUp() override;
  void TearDown() override;
protected:
  std::unique_ptr<base::test::TaskEnvironment> task_environment_;
};

void PRPPRequestLoaderImplTest::SetUpTestCase() {}

void PRPPRequestLoaderImplTest::TearDownTestCase() {}

void PRPPRequestLoaderImplTest::SetUp()
{
  task_environment_ = std::make_unique<base::test::TaskEnvironment>(
    base::test::TaskEnvironment::MainThreadType::DEFAULT);
}

void PRPPRequestLoaderImplTest::TearDown()
{
  task_environment_.reset();
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_CreatePRPPRequestLoader)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  net::IsolationInfo isolationInfo;
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  std::shared_ptr<PRPPRequestLoader> loader = PRPPRequestLoader::CreatePRPPRequestLoader(
    main_url, info, isolationInfo, url_request_context->GetWeakPtr(), ResPreloadedCB());
  EXPECT_NE(loader, nullptr);

  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());
  impl.prpp_req_info_ = nullptr;
  impl.Init(isolationInfo);

  loader = PRPPRequestLoader::CreatePRPPRequestLoader(
    main_url, info, isolationInfo, nullptr, ResPreloadedCB());
  EXPECT_EQ(loader, nullptr);
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_InitAndStartUrlRequest)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  net::IsolationInfo isolationInfo;
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());

  std::shared_ptr<PRRequestInfo> info1 = std::make_shared<PRRequestInfo>();
  info1->set_url(GURL("http://test.test:111"));
  impl.InitAndStartUrlRequest(info1, isolationInfo, true);
  EXPECT_EQ(info1->type(), PRRequestInfoType::TYPE_DEFAULT);
  info1->set_type(PRRequestInfoType::TYPE_PAGE_PREFLIGHT);
  impl.InitAndStartUrlRequest(info1, isolationInfo, true);
  EXPECT_EQ(info1->type(), PRRequestInfoType::TYPE_PAGE_PREFLIGHT);
  impl.url_request_context_ = nullptr;
  impl.InitAndStartUrlRequest(info1, isolationInfo, true);
  impl.url_request_context_ = url_request_context->GetWeakPtr();

  isolationInfo.top_frame_origin_ = std::make_optional<url::Origin>();
  EXPECT_EQ(isolationInfo.IsEmpty(), false);
  impl.InitAndStartUrlRequest(info1, isolationInfo, false);
  base::flat_set<net::SourceStream::SourceType> set;
  set.insert(net::SourceStream::SourceType::TYPE_DEFLATE);
  info1->set_accepted_stream_types(std::make_optional<base::flat_set<net::SourceStream::SourceType>>(set));
  EXPECT_EQ(info1->accepted_stream_types().has_value(), true);
  impl.InitAndStartUrlRequest(info1, isolationInfo, false);
  info1->set_initiator(std::make_optional<url::Origin>());
  EXPECT_EQ(info1->initiator().has_value(), true);
  impl.InitAndStartUrlRequest(info1, isolationInfo, false);

  info1->set_allow_credentials(true);
  EXPECT_EQ(info1->allow_credentials_, true);
  impl.InitAndStartUrlRequest(info1, isolationInfo, false);
  net::TestDelegate* delegate = new net::TestDelegate();
  impl.SetResetUrlRequestCallback(base::BindOnce([](const std::shared_ptr<net::URLRequest>& url_request) {}));
  EXPECT_EQ(impl.reset_url_request_callback_.is_null(), false);
  impl.InitAndStartUrlRequest(info1, isolationInfo, true);
  impl.SetRequestDelegate(delegate);
  EXPECT_NE(impl.delegate_, nullptr);
  impl.InitAndStartUrlRequest(info1, isolationInfo, true);
  delete delegate;
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_ReInitAndStartUrlRequest)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  net::IsolationInfo isolationInfo;
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());
  impl.InitAndStartUrlRequest(info, isolationInfo, true);
  impl.ReInitAndStartUrlRequest();
  EXPECT_EQ(impl.preload_state_, PRPPRequestLoaderImpl::STATE_IDLE);
  impl.ProcessOutboundTrustTokenInterceptor();
  impl.ProcessOutboundAttributionInterceptor();
  impl.ProcessOutboundSharedStorageInterceptor();
  impl.ScheduleStart();
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_SetRawRequestHeadersAndNotify)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());
  impl.SetRawRequestHeadersAndNotify(net::HttpRawRequestHeaders());
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_REQ_SENT;
  impl.SetRawRequestHeadersAndNotify(net::HttpRawRequestHeaders());
  impl.need_do_replay_self_ = true;
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_IDLE;
  impl.SetRawRequestHeadersAndNotify(net::HttpRawRequestHeaders());
  EXPECT_EQ(impl.preload_state_, PRPPRequestLoaderImpl::STATE_REQ_SENT);
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_NotifyEarlyResponse)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());
  scoped_refptr<const net::HttpResponseHeaders> headersRefptr;
  impl.NotifyEarlyResponse(headersRefptr);
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_ERALY_HEAD_RESPONSED;
  impl.NotifyEarlyResponse(headersRefptr);
  impl.need_do_replay_self_ = true;
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_IDLE;
  impl.NotifyEarlyResponse(headersRefptr);
  EXPECT_EQ(impl.preload_state_, PRPPRequestLoaderImpl::STATE_ERALY_HEAD_RESPONSED);
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_SetRawResponseHeaders)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());
  scoped_refptr<const net::HttpResponseHeaders> headersRefptr;
  impl.SetRawResponseHeaders(headersRefptr);
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_HEAD_RESPONSED;
  impl.SetRawResponseHeaders(headersRefptr);
  impl.need_do_replay_self_ = true;
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_IDLE;
  impl.SetRawResponseHeaders(headersRefptr);
  EXPECT_EQ(impl.preload_state_, PRPPRequestLoaderImpl::STATE_HEAD_RESPONSED);
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_OnConnected)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());
  net::TransportInfo transportInfo;
  EXPECT_EQ(impl.OnConnected(nullptr, transportInfo, net::CompletionOnceCallback()), net::OK);
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_CONNECTED;
  EXPECT_EQ(impl.OnConnected(nullptr, transportInfo, net::CompletionOnceCallback()), net::OK);
  impl.need_do_replay_self_ = true;
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_IDLE;
  EXPECT_EQ(impl.OnConnected(nullptr, transportInfo, net::CompletionOnceCallback()), net::OK);
  EXPECT_EQ(impl.preload_state_, PRPPRequestLoaderImpl::STATE_CONNECTED);
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_OnReceivedRedirect)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());

  net::RedirectInfo redirect_info;
  bool defer_redirect = true;
  net::AuthChallengeInfo authInfo;
  net::SSLCertRequestInfo requestInfo;
  net::SSLInfo SSLInfo;
  EXPECT_EQ(impl.res_loaded_cb_.is_null(), true);
  impl.OnReceivedRedirect(nullptr, redirect_info, &defer_redirect);
  impl.OnAuthRequired(nullptr, authInfo);
  impl.OnCertificateRequested(nullptr, &requestInfo);
  impl.OnSSLCertificateError(nullptr, 1, SSLInfo, true);

  impl.res_loaded_cb_ = base::BindOnce([](PRPPRequestLoader* prpp_loader) {});
  EXPECT_EQ(impl.res_loaded_cb_.is_null(), false);
  impl.OnReceivedRedirect(nullptr, redirect_info, &defer_redirect);
  impl.OnAuthRequired(nullptr, authInfo);
  impl.OnCertificateRequested(nullptr, &requestInfo);
  impl.OnSSLCertificateError(nullptr, 1, SSLInfo, true);
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_OnResponseStarted)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  net::IsolationInfo isolationInfo;
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());

  std::shared_ptr<PRRequestInfo> info1 = std::make_shared<PRRequestInfo>();
  info1->set_url(GURL("http://test.test:111"));
  impl.InitAndStartUrlRequest(info1, isolationInfo, true);
  impl.OnResponseStarted(nullptr, net::OK);
  EXPECT_EQ(impl.preload_state_, PRPPRequestLoaderImpl::STATE_RESPONSE_STARTED);
  impl.OnResponseStarted(nullptr, net::ERR_CACHE_MISS);
  impl.real_load_flags_ = net::LOAD_ONLY_FROM_CACHE;
  impl.OnResponseStarted(nullptr, net::OK);
  impl.real_load_flags_ = net::OK;
  impl.OnResponseStarted(nullptr, net::OK);
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_IDLE;
  impl.OnResponseStarted(nullptr, net::LOAD_DISABLE_CACHE);
  EXPECT_EQ(impl.preload_state_, PRPPRequestLoaderImpl::STATE_UNSUPPORT);
  impl.need_do_replay_self_ = true;
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_IDLE;
  impl.OnResponseStarted(nullptr, net::OK);
  EXPECT_EQ(impl.preload_state_, PRPPRequestLoaderImpl::STATE_RESPONSE_STARTED);
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_ReadMore)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  net::IsolationInfo isolationInfo;
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());

  std::shared_ptr<PRRequestInfo> info1 = std::make_shared<PRRequestInfo>();
  info1->set_url(GURL("http://test.test:111"));
  impl.InitAndStartUrlRequest(info1, isolationInfo, true);
  impl.ReadMore();
  EXPECT_NE(impl.cur_write_block_, nullptr);
  EXPECT_NE(impl.url_request_->CanReadFromURLRequestJob(), false);
  impl.cur_write_block_->SetCapacity(0);
  impl.ReadMore();
  impl.cur_write_block_->SetCapacity(1);
  impl.ReadMore();
  impl.url_request_->job_ = nullptr;
  impl.ReadMore();
  EXPECT_EQ(impl.url_request_->CanReadFromURLRequestJob(), false);
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_DidRead)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  net::IsolationInfo isolationInfo;
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());

  std::shared_ptr<PRRequestInfo> info1 = std::make_shared<PRRequestInfo>();
  info1->set_url(GURL("http://test.test:111"));
  impl.InitAndStartUrlRequest(info1, isolationInfo, true);
  impl.ReadMore();
  EXPECT_NE(impl.cur_write_block_, nullptr);
  impl.DidRead(CACHE_BLOCK_SIZE_TEST + 1, false);
  impl.DidRead(-1, false);
  impl.DidRead(0, false);
  EXPECT_EQ(impl.total_size_, 0);
  impl.total_size_ = 1;
  impl.DidRead(0, false);

  impl.res_loaded_cb_ = base::BindOnce([](PRPPRequestLoader* prpp_loader) {});
  EXPECT_EQ(impl.res_loaded_cb_.is_null(), false);
  impl.ReadMore();
  impl.DidRead(0, false);
  impl.ReadMore();
  impl.DidRead(1, false);
  impl.DidRead(1, true);
  for (int i = 0; i < MAX_PRPP_BODY_CACHE_SIZE_TEST + 1; i++) {
    impl.body_cache_.push(scoped_refptr<net::GrowableIOBuffer>());
  }
  impl.DidRead(1, true);
  EXPECT_EQ(impl.need_continue_read_, true);

  impl.out_buf_ = scoped_refptr<net::IOBuffer>();
  impl.DidRead(0, false);
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_OnReadCompleted)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  net::IsolationInfo isolationInfo;
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());

  impl.OnReadCompleted(nullptr, 0);
  std::shared_ptr<net::URLRequest> url_request = impl.url_request_context_.get()->CreateRequestForPrpp(
    GURL("http://test.test:222"), net::MEDIUM, &impl, MISSING_TRAFFIC_ANNOTATION, false);
  EXPECT_NE(url_request, nullptr);
  impl.OnReadCompleted(url_request.get(), 0);
  std::shared_ptr<PRRequestInfo> info1 = std::make_shared<PRRequestInfo>();
  info1->set_url(GURL("http://test.test:111"));
  impl.InitAndStartUrlRequest(info1, isolationInfo, true);
  EXPECT_NE(impl.url_request_, nullptr);
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_UNSUPPORT;
  impl.OnReadCompleted(impl.url_request_.get(), 0);
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_ERROR;
  impl.OnReadCompleted(impl.url_request_.get(), 0);
  impl.ReadMore();
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_IDLE;
  impl.OnReadCompleted(impl.url_request_.get(), 0);

  impl.SetRequestDelegate(nullptr);
  EXPECT_EQ(impl.delegate_, nullptr);
  impl.SetRequestHeadersCallback(base::RepeatingCallback<void(net::HttpRawRequestHeaders)>());
  impl.SetResponseHeadersCallback(base::RepeatingCallback<void(scoped_refptr<const net::HttpResponseHeaders>)>());
  impl.SetEarlyResponseHeadersCallback(base::RepeatingCallback<void(scoped_refptr<const net::HttpResponseHeaders>)>());
  impl.SetResetUrlRequestCallback(base::BindOnce([](const std::shared_ptr<net::URLRequest>& url_request) {}));
  impl.SetUpdateResRequestInfoCallback(base::BindOnce([](const std::string& key,
    const std::shared_ptr<ohos_prp_preload::PRRequestInfo>& info) {}));
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_StartAndDoReplay)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  net::IsolationInfo isolationInfo;
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());

  impl.SetRequestDelegate(&impl);
  EXPECT_NE(impl.delegate_, nullptr);
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_UNSUPPORT;
  EXPECT_EQ(impl.StartReplay(), false);
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_ERROR;
  EXPECT_EQ(impl.StartReplay(), false);
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_IDLE;
  EXPECT_EQ(impl.StartReplay(), true);
  impl.need_update_req_info_ = true;
  EXPECT_EQ(impl.StartReplay(), true);
  impl.SetUpdateResRequestInfoCallback(base::BindOnce([](const std::string& key,
    const std::shared_ptr<ohos_prp_preload::PRRequestInfo>& info) {}));
  EXPECT_EQ(impl.update_res_request_info_callback_.is_null(), false);
  EXPECT_EQ(impl.StartReplay(), true);

  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_RESPONSED;
  impl.DoReplay();
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_UNSUPPORT;
  impl.DoReplay();
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_ERROR;
  impl.DoReplay();
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_IDLE;
  impl.DoReplay();
  EXPECT_EQ(impl.need_do_replay_self_, true);
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_CONNECTED);
  EXPECT_NE(impl.rec_msg_list_.empty(), true);
  impl.DoReplay();
  impl.delegate_ = nullptr;
  EXPECT_EQ(impl.StartReplay(), false);
  impl.DoReplay();
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_ProcessMessages)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());
  impl.SetRequestDelegate(&impl);
  EXPECT_NE(impl.delegate_, nullptr);

  impl.rec_msg_list_.push(static_cast<PRPPRequestLoaderImpl::PRPPRecorderMsg>(STATE_ENUM_TEST));
  impl.ProcessMessages();
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_CONNECTED);
  impl.ProcessMessages();
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_RAW_REQ_HDR);
  impl.ProcessMessages();
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_EARLY_RESP_HDR);
  impl.ProcessMessages();
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_RAW_RESP_HDR);
  impl.ProcessMessages();

  impl.SetRequestHeadersCallback(base::BindRepeating([&](net::HttpRawRequestHeaders headers) {}));
  EXPECT_NE(impl.request_headers_callback_.is_null(), true);
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_RAW_REQ_HDR);
  impl.ProcessMessages();
  impl.SetEarlyResponseHeadersCallback(
    base::BindRepeating([&](scoped_refptr<const net::HttpResponseHeaders> headers) {}));
  EXPECT_NE(impl.early_response_headers_callback_.is_null(), true);
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_EARLY_RESP_HDR);
  impl.ProcessMessages();
  impl.SetResponseHeadersCallback(base::BindRepeating([&](scoped_refptr<const net::HttpResponseHeaders> headers) {}));
  EXPECT_NE(impl.response_headers_callback_.is_null(), true);
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_RAW_RESP_HDR);
  impl.ProcessMessages();

  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_RESPONSE_BODY);
  impl.ProcessMessages();
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_ERROR);
  impl.ProcessMessages();
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_IDLE;
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_RESPONSE_STARTED);
  impl.ProcessMessages();
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_RESPONSE_STARTED;
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_RESPONSE_STARTED);
  impl.ProcessMessages();
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_RESPONSED;
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_RESPONSE_STARTED);
  impl.ProcessMessages();
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_RESPONSE_STARTED);
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_RESPONSE_BODY);
  impl.ProcessMessages();
  EXPECT_EQ(impl.need_do_replay_self_, false);
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_ProcessMessages2)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  net::IsolationInfo isolationInfo;
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());
  impl.SetRequestDelegate(&impl);
  EXPECT_NE(impl.delegate_, nullptr);

  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_IDLE;
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_RAW_REQ_HDR);
  impl.ProcessMessages();
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_RESPONSE_STARTED;
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_RAW_REQ_HDR);
  impl.ProcessMessages();
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_RESPONSED;
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_RAW_REQ_HDR);
  impl.ProcessMessages();
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_RAW_REQ_HDR);
  impl.rec_msg_list_.push(PRPPRequestLoaderImpl::PRPPRecorderMsg::MSG_RAW_REQ_HDR);
  impl.ProcessMessages();
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_Read)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());

  impl.Read(nullptr, 0);
  impl.need_continue_read_ = true;
  impl.Read(nullptr, 0);
  EXPECT_EQ(impl.need_continue_read_, false);
  impl.Read(nullptr, 1);
  EXPECT_EQ(impl.out_max_bytes_, 1);
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_RESPONSED;
  impl.Read(nullptr, 1);
  EXPECT_EQ(impl.out_max_bytes_, 0);
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_UNSUPPORT;
  impl.Read(nullptr, 1);
  EXPECT_EQ(impl.out_max_bytes_, 0);
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_ERROR;
  impl.Read(nullptr, 1);
  EXPECT_EQ(impl.out_max_bytes_, 0);

  impl.body_cache_.push(nullptr);
  impl.Read(nullptr, 1);
  impl.body_cache_.push(base::MakeRefCounted<net::GrowableIOBuffer>());
  EXPECT_NE(impl.body_cache_.front(), nullptr);
  impl.Read(nullptr, 1);
  impl.cur_read_offset_ = -1;
  impl.Read(nullptr, 1);
  impl.body_cache_.push(base::MakeRefCounted<net::GrowableIOBuffer>());
  impl.body_cache_.front()->offset_ = MAX_PRPP_BODY_CACHE_SIZE_TEST;
  net::IOBuffer* buffer = new net::IOBuffer();
  impl.Read(buffer, 1);
  delete buffer;

  for (int i = 0; i < MIN_PRPP_BODY_CACHE_SIZE_TEST + 1; i++) {
    impl.body_cache_.push(base::MakeRefCounted<net::GrowableIOBuffer>());
  }
  impl.Read(nullptr, 0);
  EXPECT_EQ(impl.out_max_bytes_, 0);
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_UpdateResRequestInfo)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());

  std::string key = "test";
  impl.UpdateResRequestInfo(key, info);
  EXPECT_EQ(impl.need_update_req_info_, true);
  impl.SetRequestDelegate(&impl);
  EXPECT_NE(impl.delegate_, nullptr);
  impl.UpdateResRequestInfo(key, info);
  impl.SetUpdateResRequestInfoCallback(base::BindOnce([](const std::string& key,
    const std::shared_ptr<ohos_prp_preload::PRRequestInfo>& info) {}));
  EXPECT_EQ(impl.update_res_request_info_callback_.is_null(), false);
  impl.UpdateResRequestInfo(key, info);

  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_UNSUPPORT;
  impl.PushFailure(PRPPRequestLoaderImpl::STATE_UNSUPPORT, false);
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_ERROR;
  impl.PushFailure(PRPPRequestLoaderImpl::STATE_ERROR, false);
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_IDLE;
  impl.PushFailure(PRPPRequestLoaderImpl::STATE_UNSUPPORT, false);
  EXPECT_EQ(impl.preload_state_, PRPPRequestLoaderImpl::STATE_UNSUPPORT);
  impl.preload_state_ = PRPPRequestLoaderImpl::STATE_IDLE;
  impl.PushFailure(PRPPRequestLoaderImpl::STATE_IDLE, true);

  impl.ClearMsgList();
  impl.ClearBodyCache();
  impl.ClearLoader();
  EXPECT_EQ(impl.out_max_bytes_, 0);
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_GetLoadTimingInfo)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  net::IsolationInfo isolationInfo;
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());

  impl.GetLoadTimingInfo(nullptr);
  net::LoadTimingInfo load_timing_info;
  impl.GetLoadTimingInfo(&load_timing_info);
  std::shared_ptr<PRRequestInfo> info1 = std::make_shared<PRRequestInfo>();
  info1->set_url(GURL("http://test.test:111"));
  impl.InitAndStartUrlRequest(info1, isolationInfo, true);
  EXPECT_NE(impl.url_request_, nullptr);
  impl.GetLoadTimingInfo(&load_timing_info);

  impl.url_request_->load_timing_info_.proxy_resolve_start = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.proxy_resolve_end = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.connect_timing.domain_lookup_start = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.connect_timing.domain_lookup_end = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.connect_timing.connect_start = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.connect_timing.connect_end = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.connect_timing.ssl_start = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.connect_timing.ssl_end = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);

  impl.url_request_->load_timing_info_.send_start = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.send_end = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.receive_headers_start = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.receive_headers_end = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.receive_non_informational_headers_start = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.first_early_hints_time = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.push_start = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.push_end = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_GetLoadTimingInfo2)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  net::IsolationInfo isolationInfo;
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());

  net::LoadTimingInfo load_timing_info;
  impl.GetLoadTimingInfo(&load_timing_info);
  std::shared_ptr<PRRequestInfo> info1 = std::make_shared<PRRequestInfo>();
  info1->set_url(GURL("http://test.test:111"));
  impl.InitAndStartUrlRequest(info1, isolationInfo, true);
  EXPECT_NE(impl.url_request_, nullptr);

  impl.load_timing_info_.request_start = base::TimeTicks::Now() + base::Milliseconds(1000);
  impl.url_request_->load_timing_info_.request_start = base::TimeTicks::Now() + base::Milliseconds(1000);
  impl.url_request_->load_timing_info_.send_start = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.send_end = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.receive_headers_start = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.receive_headers_end = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.receive_non_informational_headers_start = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.first_early_hints_time = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.push_start = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
  impl.url_request_->load_timing_info_.push_end = base::TimeTicks::Now();
  impl.GetLoadTimingInfo(&load_timing_info);
}

TEST_F(PRPPRequestLoaderImplTest, PRPPRequestLoaderImplTest_ClearLoaderCallback)
{
  std::string main_url;
  std::shared_ptr<PRRequestInfo> info = std::make_shared<PRRequestInfo>();
  std::unique_ptr<net::URLRequestContext> url_request_context = net::CreateTestURLRequestContextBuilder()->Build();
  EXPECT_NE(url_request_context, nullptr);
  PRPPRequestLoaderImpl impl(main_url, info, url_request_context->GetWeakPtr(), ResPreloadedCB());

  impl.ClearLoaderCallback(true);
  impl.ClearLoaderCallback(false);
  EXPECT_EQ(impl.delegate_, nullptr);
}

} // namespace ohos_prp_preload