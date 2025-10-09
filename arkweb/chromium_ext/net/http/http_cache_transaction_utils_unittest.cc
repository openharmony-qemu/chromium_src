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

#include "arkweb/chromium_ext/net/base/page_res_request_info.h"
#include "base/test/task_environment.h"
#include "net/http/http_response_headers.h"
#include "net/http/http_response_info.h"
#include "net/http/mock_http_cache.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "arkweb/chromium_ext/net/http/http_cache_transaction_utils.h"
#include "net/http/http_cache_transaction.h"
#undef private

namespace net {
class MockHttpTransaction : public HttpTransaction {
 public:
  MockHttpTransaction() {
    auto headers = base::MakeRefCounted<HttpResponseHeaders>(
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 0\r\n"
        "Content-Type: text/html\r\n"
        "\r\n");
    response_info_.headers = headers;
    response_info_.response_time = base::Time::Now();
    response_info_.request_time = base::Time::Now() - base::Seconds(1);
  }
  ~MockHttpTransaction() override = default;

  int RestartWithSecureDnsOnly(CompletionOnceCallback callback) {
    restart_secure_dns_called_ = true;
    callback_ = std::move(callback);
    return restart_result_;
  }

  const HttpResponseInfo* GetResponseInfo() const override {
    return &response_info_;
  }

  MOCK_METHOD3(Start,
               int(const HttpRequestInfo* request_info,
                   CompletionOnceCallback callback,
                   const NetLogWithSource& net_log));
  MOCK_METHOD1(RestartIgnoringLastError, int(CompletionOnceCallback callback));
  MOCK_METHOD3(RestartWithCertificate,
               int(scoped_refptr<X509Certificate> client_cert,
                   scoped_refptr<SSLPrivateKey> client_private_key,
                   CompletionOnceCallback callback));
  MOCK_METHOD2(RestartWithAuth,
               int(const AuthCredentials& credentials,
                   CompletionOnceCallback callback));
  MOCK_METHOD0(IsReadyToRestartForAuth, bool());
  MOCK_METHOD3(Read,
               int(IOBuffer* buf,
                   int buf_len,
                   CompletionOnceCallback callback));
  MOCK_METHOD0(StopCaching, void());
  MOCK_CONST_METHOD0(GetTotalReceivedBytes, int64_t());
  MOCK_CONST_METHOD0(GetTotalSentBytes, int64_t());
  MOCK_CONST_METHOD0(GetReceivedBodyBytes, int64_t());
  MOCK_METHOD0(DoneReading, void());
  MOCK_CONST_METHOD0(GetLoadState, LoadState());
  MOCK_METHOD1(SetQuicServerInfo, void(QuicServerInfo* quic_server_info));
  MOCK_CONST_METHOD1(GetLoadTimingInfo, bool(LoadTimingInfo* load_timing_info));
  MOCK_CONST_METHOD1(GetRemoteEndpoint, bool(IPEndPoint* endpoint));
  MOCK_CONST_METHOD1(PopulateNetErrorDetails, void(NetErrorDetails* details));
  MOCK_METHOD1(SetPriority, void(RequestPriority priority));
  MOCK_METHOD1(SetWebSocketHandshakeStreamCreateHelper,
               void(WebSocketHandshakeStreamBase::CreateHelper* create_helper));
  MOCK_METHOD1(SetBeforeNetworkStartCallback,
               void(BeforeNetworkStartCallback callback));
  MOCK_METHOD1(SetConnectedCallback, void(const ConnectedCallback& callback));
  MOCK_METHOD1(SetRequestHeadersCallback,
               void(RequestHeadersCallback callback));
  MOCK_METHOD1(SetEarlyResponseHeadersCallback,
               void(ResponseHeadersCallback callback));
  MOCK_METHOD1(SetResponseHeadersCallback,
               void(ResponseHeadersCallback callback));
  MOCK_METHOD1(
      SetModifyRequestHeadersCallback,
      void(base::RepeatingCallback<void(HttpRequestHeaders*)> callback));
  MOCK_METHOD1(SetIsSharedDictionaryReadAllowedCallback,
               void(base::RepeatingCallback<bool()> callback));
  MOCK_METHOD0(ResumeNetworkStart, int());
  MOCK_CONST_METHOD0(GetConnectionAttempts, ConnectionAttempts());
  MOCK_METHOD0(CloseConnectionOnDestruction, void());
  MOCK_CONST_METHOD0(IsMdlMatchForMetrics, bool());

  bool restart_secure_dns_called_ = false;
  int restart_result_ = OK;
  CompletionOnceCallback callback_;
  HttpResponseInfo response_info_;
};

class TestHttpCacheTransaction : public HttpCache::Transaction {
 public:
  TestHttpCacheTransaction(RequestPriority priority, HttpCache* cache)
      : HttpCache::Transaction(priority, cache) {}
  ~TestHttpCacheTransaction() override = default;

  Transaction::Mode mode() const override { return Transaction::READ_WRITE; }
  bool do_loop_called_ = false;
  int do_loop_result_ = 0;
};

class HttpTransactionUtilsTest : public testing::Test {
 public:
  void SetUp() override {
    mock_cache_ = std::make_unique<MockHttpCache>();
    mock_transaction_ = std::make_unique<TestHttpCacheTransaction>(
        DEFAULT_PRIORITY, mock_cache_->http_cache());

    utils_ = std::make_unique<HttpTransactionUtils>(mock_transaction_.get());
  }

  void TearDown() override {
    utils_.reset();
    mock_transaction_.reset();
    mock_cache_.reset();
  }

 protected:
  std::unique_ptr<MockHttpCache> mock_cache_;
  std::unique_ptr<TestHttpCacheTransaction> mock_transaction_;
  std::unique_ptr<HttpTransactionUtils> utils_;
  base::test::TaskEnvironment task_environment_;
};

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
TEST_F(HttpTransactionUtilsTest, UpdateCacheInfo_PreLoadInfo_IsNull) {
  mock_transaction_->preload_info_ = nullptr;
  auto response_headers = base::MakeRefCounted<HttpResponseHeaders>("test");
  response_headers->SetHeader("Cache-Control", "test");
  HttpResponseInfo response;
  response.headers = response_headers;
  response.response_time = base::Time::Now();
  response.request_time = base::Time::Now() - base::Seconds(1);
  utils_->UpdateCacheInfo(response);
  EXPECT_EQ(mock_transaction_->preload_info_, nullptr);
}

TEST_F(HttpTransactionUtilsTest, UpdateCacheInfo_Header_IsNull) {
  mock_transaction_->preload_info_ =
      std::make_shared<ohos_prp_preload::PRRequestInfo>();

  HttpResponseInfo response;
  response.headers = nullptr;
  response.response_time = base::Time::Now();
  response.request_time = base::Time::Now() - base::Seconds(1);

  utils_->UpdateCacheInfo(response);
  EXPECT_EQ(mock_transaction_->preload_info_->cache_type(),
            ohos_prp_preload::PRRequestCacheType::DISABLE_CACHE);
}

TEST_F(HttpTransactionUtilsTest, UpdateCacheInfo_FreshnessLifetimes_IsNotZero) {
  mock_transaction_->preload_info_ =
      std::make_shared<ohos_prp_preload::PRRequestInfo>();

  auto response_headers = base::MakeRefCounted<HttpResponseHeaders>("test");
  response_headers->SetHeader("Cache-Control", "max-age=3600");
  response_headers->SetHeader("ETag", "\"test-etag\"");
  response_headers->SetHeader("Last-Modified", "Sat, 20 Sep 2025 08:00:00 GMT");

  HttpResponseInfo response;
  response.headers = response_headers;
  response.response_time = base::Time::Now();
  response.request_time = base::Time::Now() - base::Seconds(1);

  utils_->UpdateCacheInfo(response);
  EXPECT_EQ(mock_transaction_->preload_info_->cache_type(),
            ohos_prp_preload::PRRequestCacheType::FORCE_CACHE);
  EXPECT_GT(mock_transaction_->preload_info_->freshness_life_times(), 0);
  EXPECT_EQ(mock_transaction_->preload_info_->e_tag(), "\"test-etag\"");
  EXPECT_EQ(mock_transaction_->preload_info_->last_modified(),
            "Sat, 20 Sep 2025 08:00:00 GMT");
}

TEST_F(HttpTransactionUtilsTest, UpdateCacheInfo_ETag_IsNotEmpty) {
  mock_transaction_->preload_info_ =
      std::make_shared<ohos_prp_preload::PRRequestInfo>();

  auto response_headers = base::MakeRefCounted<HttpResponseHeaders>("test");
  response_headers->SetHeader("ETag", "\"test-etag\"");
  response_headers->SetHeader("Cache-Control", "no-cache");

  HttpResponseInfo response;
  response.headers = response_headers;
  response.response_time = base::Time::Now();
  response.request_time = base::Time::Now() - base::Seconds(1);

  utils_->UpdateCacheInfo(response);
  EXPECT_EQ(mock_transaction_->preload_info_->cache_type(),
            ohos_prp_preload::PRRequestCacheType::NEGOTIATION_CACHE);
  EXPECT_EQ(mock_transaction_->preload_info_->freshness_life_times(), 0);
  EXPECT_EQ(mock_transaction_->preload_info_->e_tag(), "\"test-etag\"");
}

TEST_F(HttpTransactionUtilsTest, UpdateCacheInfo_LastModified_IsNotEmpty) {
  mock_transaction_->preload_info_ =
      std::make_shared<ohos_prp_preload::PRRequestInfo>();

  auto response_headers = base::MakeRefCounted<HttpResponseHeaders>("test");
  response_headers->SetHeader("Last-Modified", "Sat, 20 Sep 2025 08:00:00 GMT");
  response_headers->SetHeader("Cache-Control", "no-cache");

  HttpResponseInfo response;
  response.headers = response_headers;
  response.response_time = base::Time::Now();
  response.request_time = base::Time::Now() - base::Seconds(1);

  utils_->UpdateCacheInfo(response);

  EXPECT_EQ(mock_transaction_->preload_info_->cache_type(),
            ohos_prp_preload::PRRequestCacheType::NEGOTIATION_CACHE);
  EXPECT_EQ(mock_transaction_->preload_info_->freshness_life_times(), 0);
  EXPECT_EQ(mock_transaction_->preload_info_->last_modified(),
            "Sat, 20 Sep 2025 08:00:00 GMT");
}

TEST_F(HttpTransactionUtilsTest,
       UpdateCacheInfo_FreshnessLifetimes_And_ETag_And_LastModified_AreEmpty) {
  mock_transaction_->preload_info_ =
      std::make_shared<ohos_prp_preload::PRRequestInfo>();

  auto response_headers = base::MakeRefCounted<HttpResponseHeaders>("test");
  response_headers->SetHeader("Content-Type", "test");

  HttpResponseInfo response;
  response.headers = response_headers;
  response.response_time = base::Time::Now();
  response.request_time = base::Time::Now() - base::Seconds(1);

  utils_->UpdateCacheInfo(response);
  EXPECT_EQ(mock_transaction_->preload_info_->cache_type(),
            ohos_prp_preload::PRRequestCacheType::DISABLE_CACHE);
  EXPECT_EQ(mock_transaction_->preload_info_->freshness_life_times(), 0);
  EXPECT_TRUE(mock_transaction_->preload_info_->e_tag().empty());
  EXPECT_TRUE(mock_transaction_->preload_info_->last_modified().empty());
}

TEST_F(HttpTransactionUtilsTest, UpdateCacheInfo_FreshnessLifetimes_IsZero) {
  mock_transaction_->preload_info_ =
      std::make_shared<ohos_prp_preload::PRRequestInfo>();

  auto response_headers = base::MakeRefCounted<HttpResponseHeaders>("test");
  response_headers->SetHeader("ETag", "\"test-etag\"");
  response_headers->SetHeader("Last-Modified", "Sat, 20 Sep 2025 08:00:00 GMT");
  response_headers->SetHeader("Cache-Control", "no-cache");

  HttpResponseInfo response;
  response.headers = response_headers;
  response.response_time = base::Time::Now();
  response.request_time = base::Time::Now() - base::Seconds(1);

  utils_->UpdateCacheInfo(response);
  EXPECT_EQ(mock_transaction_->preload_info_->cache_type(),
            ohos_prp_preload::PRRequestCacheType::NEGOTIATION_CACHE);
  EXPECT_EQ(mock_transaction_->preload_info_->freshness_life_times(), 0);
  EXPECT_EQ(mock_transaction_->preload_info_->e_tag(), "\"test-etag\"");
  EXPECT_EQ(mock_transaction_->preload_info_->last_modified(),
            "Sat, 20 Sep 2025 08:00:00 GMT");
}

#endif  // ARKWEB_PRP_PRELOAD

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
TEST_F(HttpTransactionUtilsTest, RestartWithSecureDnsOnly_Cache_IsNull) {
  mock_transaction_->cache_ = nullptr;
  CompletionOnceCallback callback;
  int result = utils_->RestartWithSecureDnsOnly(callback);
  EXPECT_EQ(result, ERR_UNEXPECTED);
}

TEST_F(HttpTransactionUtilsTest, RestartWithSecureDnsOnly_Cache_IsNotNull) {
  mock_transaction_->cache_ = mock_cache_->http_cache()->GetWeakPtr();

  auto mock_network_trans = std::make_unique<MockHttpTransaction>();
  mock_network_trans->restart_result_ = ERR_IO_PENDING;
  mock_transaction_->network_trans_ = std::move(mock_network_trans);

  auto request = std::make_unique<HttpRequestInfo>();
  mock_transaction_->request_ = request.get();
  mock_transaction_->initial_request_ = nullptr;
  mock_transaction_->io_callback_ = base::DoNothing();

  CompletionOnceCallback callback = base::DoNothing();
  int result = utils_->RestartWithSecureDnsOnly(callback);
  EXPECT_EQ(result, ERR_IO_PENDING);
  EXPECT_FALSE(mock_transaction_->callback_.is_null());
}

TEST_F(HttpTransactionUtilsTest, RestartWithSecureDnsOnly_Normal) {
  auto request = std::make_unique<HttpRequestInfo>();
  request->load_flags = LOAD_NORMAL;
  mock_transaction_->request_ = request.get();

  auto initial_request = std::make_unique<HttpRequestInfo>();
  initial_request->load_flags = LOAD_NORMAL;
  mock_transaction_->initial_request_ = initial_request.get();

  auto custom_request = std::make_unique<HttpRequestInfo>();
  custom_request->load_flags = LOAD_NORMAL;
  custom_request->secure_dns_only = false;
  mock_transaction_->custom_request_ = std::move(custom_request);
  mock_transaction_->request_ = mock_transaction_->custom_request_.get();
  mock_transaction_->io_callback_ = base::DoNothing();

  auto mock_network_trans = std::make_unique<MockHttpTransaction>();
  mock_network_trans->restart_result_ = OK;
  mock_transaction_->network_trans_ = std::move(mock_network_trans);

  CompletionOnceCallback callback = base::DoNothing();
  int result = utils_->RestartWithSecureDnsOnly(callback);
  EXPECT_EQ(result, OK);
  EXPECT_TRUE(mock_transaction_->custom_request_->secure_dns_only);
  EXPECT_TRUE(mock_transaction_->callback_.is_null());
}

TEST_F(HttpTransactionUtilsTest, RestartNetworkRequestWithSecureDnsOnly_Async) {
  auto request = std::make_unique<HttpRequestInfo>();
  mock_transaction_->request_ = request.get();
  mock_transaction_->initial_request_ = nullptr;
  mock_transaction_->io_callback_ = base::DoNothing();

  mock_transaction_->next_state_ =
      HttpCache::Transaction::State::STATE_FINISH_HEADERS;

  auto mock_network_trans = std::make_unique<MockHttpTransaction>();
  mock_network_trans->restart_result_ = ERR_IO_PENDING;
  mock_transaction_->network_trans_ = std::move(mock_network_trans);
  int result = utils_->RestartNetworkRequestWithSecureDnsOnly();
  EXPECT_EQ(result, ERR_IO_PENDING);
  EXPECT_FALSE(mock_transaction_->do_loop_called_);
}

TEST_F(HttpTransactionUtilsTest,
       RestartNetworkRequestWithSecureDnsOnly_Request_Equals_InitialRequest) {
  auto request = std::make_unique<HttpRequestInfo>();
  mock_transaction_->request_ = request.get();
  mock_transaction_->initial_request_ = request.get();
  mock_transaction_->io_callback_ = base::DoNothing();

  mock_transaction_->next_state_ =
      HttpCache::Transaction::State::STATE_FINISH_HEADERS;

  auto custom_request = std::make_unique<HttpRequestInfo>();
  custom_request->secure_dns_only = false;
  mock_transaction_->custom_request_ = std::move(custom_request);

  auto mock_network_trans = std::make_unique<MockHttpTransaction>();
  mock_network_trans->restart_result_ = OK;
  mock_transaction_->network_trans_ = std::move(mock_network_trans);

  int result = utils_->RestartNetworkRequestWithSecureDnsOnly();
  EXPECT_EQ(result, OK);
  EXPECT_FALSE(mock_transaction_->custom_request_->secure_dns_only);
}

TEST_F(HttpTransactionUtilsTest,
       RestartNetworkRequestWithSecureDnsOnly_CustomRequest_IsSet) {
  auto request = std::make_unique<HttpRequestInfo>();
  mock_transaction_->request_ = request.get();
  mock_transaction_->initial_request_ = nullptr;

  auto custom_request = std::make_unique<HttpRequestInfo>();
  custom_request->secure_dns_only = false;
  mock_transaction_->custom_request_ = std::move(custom_request);
  mock_transaction_->request_ = mock_transaction_->custom_request_.get();
  mock_transaction_->initial_request_ = nullptr;

  mock_transaction_->io_callback_ = base::DoNothing();

  auto mock_network_trans = std::make_unique<MockHttpTransaction>();
  mock_network_trans->restart_result_ = ERR_IO_PENDING;
  mock_transaction_->network_trans_ = std::move(mock_network_trans);

  int result = utils_->RestartNetworkRequestWithSecureDnsOnly();
  EXPECT_EQ(result, ERR_IO_PENDING);
  EXPECT_TRUE(mock_transaction_->custom_request_->secure_dns_only);
}

#endif  // ARKWEB_EXT_HTTP_DNS_FALLBACK
}  // namespace net