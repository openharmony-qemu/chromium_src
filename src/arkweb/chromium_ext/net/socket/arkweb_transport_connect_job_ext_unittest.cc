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

#include <memory>
#include <string>
#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "base/test/scoped_feature_list.h"
#include "base/test/task_environment.h"
#include "base/strings/utf_string_conversions.h"
#include "net/base/address_family.h"
#include "net/base/features.h"
#include "net/base/host_port_pair.h"
#include "net/base/ip_address.h"
#include "net/base/ip_endpoint.h"
#include "net/base/net_errors.h"
#include "net/cert/mock_cert_verifier.h"
#include "net/dns/mock_host_resolver.h"
#include "net/dns/public/secure_dns_policy.h"
#include "net/http/transport_security_state.h"
#include "net/log/net_log.h"
#include "net/ssl/ssl_config_service.h"
#include "net/ssl/test_ssl_config_service.h"

#define private public
#define protected public
#include "arkweb/chromium_ext/net/socket/arkweb_transport_connect_job_ext.h"
#include "net/socket/connect_job_test_util.h"
#include "net/socket/connection_attempts.h"
#include "net/socket/ssl_client_socket.h"
#include "net/socket/stream_socket.h"
#include "net/socket/transport_client_socket_pool_test_util.h"
#include "net/socket/transport_connect_sub_job.h"
#undef protected
#undef private

#include "net/spdy/spdy_test_util_common.h"
#include "net/test/gtest_util.h"
#include "net/test/test_with_task_environment.h"
#include "net/url_request/static_http_user_agent_settings.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/platform/wtf/std_lib_extras.h"
#include "url/scheme_host_port.h"
#include "url/url_constants.h"

namespace net {
namespace{
const constexpr char* HOST_NAME = "unresolvable.host.name";
constexpr int PORT_ONE = 80;
constexpr int PORT_TWO = 8441;
constexpr int PORT_THREE = 8442;
constexpr int NUM_A = 0;
constexpr int NUM_B = 1;
constexpr int NUM_C = 15;

IPAddress ParseIP(const std::string& ip) {
  IPAddress address;
  CHECK(address.AssignFromIPLiteral(ip));
  return address;
}

class MockStreamSocket : public StreamSocket {
 public:
  MockStreamSocket() = default;
  ~MockStreamSocket() override = default;
  int GetPeerAddress(IPEndPoint*) const override {
    std::cout << "enter GetPeerAddress";
    return NUM_A;
  }
  MOCK_METHOD(int, Connect, (CompletionOnceCallback), (override));
  MOCK_METHOD(int, ConfirmHandshake, (CompletionOnceCallback), (override));
  MOCK_METHOD(void, Disconnect, (), (override));
  MOCK_METHOD(bool, IsConnected, (), (const));
  MOCK_METHOD(bool, IsConnectedAndIdle, (), (const));
  MOCK_METHOD(int, GetLocalAddress, (IPEndPoint*), (const));
  MOCK_METHOD(const NetLogWithSource&, NetLog, (), (const));
  MOCK_METHOD(NextProto, GetNegotiatedProtocol, (), (const));
  MOCK_METHOD(bool, WasEverUsed, (), (const));
  MOCK_METHOD(bool, GetSSLInfo, (SSLInfo*), (override));
  MOCK_METHOD(std::optional<std::string_view>, GetPeerApplicationSettings, (), (const));
  MOCK_METHOD(void, GetSSLCertRequestInfo, (SSLCertRequestInfo*), (const));
  MOCK_METHOD(int64_t, GetTotalReceivedBytes, (), (const));
  MOCK_METHOD(void, ApplySocketTag, (const SocketTag&), (override));
  MOCK_METHOD(int, Read, (IOBuffer* buf, int buf_len, CompletionOnceCallback callback), (override));
  MOCK_METHOD(int, Write, (IOBuffer* buf, int buf_len, CompletionOnceCallback callback,
              const NetworkTrafficAnnotationTag& traffic_annotation), (override));
  MOCK_METHOD(int, SetReceiveBufferSize, (int32_t size), (override));
  MOCK_METHOD(int, SetSendBufferSize, (int32_t size), (override));
};

class ArkWebTransportConnectJobExtTest: public WithTaskEnvironment, public testing::Test {
 public:
  ArkWebTransportConnectJobExtTest()
      : WithTaskEnvironment(base::test::TaskEnvironment::TimeSource::MOCK_TIME),
        client_socket_factory_(NetLog::Get()),
        common_connect_job_params_(
            &client_socket_factory_,
            &host_resolver_,
            /*http_auth_cache=*/nullptr,
            /*http_auth_handler_factory=*/nullptr,
            /*spdy_session_pool=*/nullptr,
            /*quic_supported_versions=*/nullptr,
            /*quic_session_pool=*/nullptr,
            /*proxy_delegate=*/nullptr,
            &http_user_agent_settings_,
            &ssl_client_context_,
            /*socket_performance_watcher_factory=*/nullptr,
            /*network_quality_estimator=*/nullptr,
            NetLog::Get(),
            /*websocket_endpoint_lock_manager=*/nullptr,
            /*http_server_properties=*/nullptr,
            /*alpn_protos=*/nullptr,
            /*application_settings=*/nullptr,
            /*ignore_certificate_errors=*/nullptr,
            /*early_data_enabled=*/nullptr) {}

  ~ArkWebTransportConnectJobExtTest() override = default;

  static scoped_refptr<TransportSocketParams> DefaultParams() {
    return base::MakeRefCounted<TransportSocketParams>(
        url::SchemeHostPort(url::kHttpScheme, HOST_NAME, PORT_ONE),
        NetworkAnonymizationKey(), SecureDnsPolicy::kAllow,
        OnHostResolutionCallback(),
        /*supported_alpns=*/base::flat_set<std::string>());
  }

 protected:
  MockHostResolver host_resolver_{/*default_result=*/MockHostResolverBase::
                                      RuleResolver::GetLocalhostResult()};
  MockTransportClientSocketFactory client_socket_factory_;
  TestSSLConfigService ssl_config_service_{SSLContextConfig{}};
  MockCertVerifier cert_verifier_;
  TransportSecurityState transport_security_state_;
  const StaticHttpUserAgentSettings http_user_agent_settings_ = {"*",
                                                                 "test-ua"};
  SSLClientContext ssl_client_context_{&ssl_config_service_, &cert_verifier_,
                                       &transport_security_state_,
                                       /*ssl_client_session_cache=*/nullptr,
                                       /*sct_auditing_delegate=*/nullptr};
  CommonConnectJobParams common_connect_job_params_;
};


#if BUILDFLAG(ARKWEB_MULTI_IP_CONNECT)
TEST_F(ArkWebTransportConnectJobExtTest, AddIpv4AddressLimit) {
  host_resolver_.rules()->AddSimulatedTimeoutFailure(HOST_NAME);
  host_resolver_.set_synchronous_mode(false);
  TestConnectJobDelegate test_delegate;
  ArkWebTransportConnectJobExt transport_connect_job(
        DEFAULT_PRIORITY, SocketTag(), &common_connect_job_params_,
        DefaultParams(), &test_delegate, nullptr /* net_log */);
  test_delegate.StartJobExpectingResult(&transport_connect_job, ERR_NAME_NOT_RESOLVED, false);

  std::vector<IPEndPoint> list;
  IPAddress ip_address;
  IPEndPoint endpoint(ip_address, PORT_ONE);
  int ip_addresses_count = NUM_C;
  transport_connect_job.AddIpv4AddressLimit(ip_addresses_count, list, endpoint);
  EXPECT_EQ(list.size(), NUM_A);

  ip_addresses_count = NUM_B;
  transport_connect_job.AddIpv4AddressLimit(ip_addresses_count, list, endpoint);
  EXPECT_EQ(list.size(), NUM_B);
  EXPECT_EQ(list[NUM_A].port(), PORT_ONE);
}

TEST_F(ArkWebTransportConnectJobExtTest, AddIpv6AddressLimit) {
  host_resolver_.rules()->AddSimulatedTimeoutFailure(HOST_NAME);
  host_resolver_.set_synchronous_mode(false);
  TestConnectJobDelegate test_delegate;
  ArkWebTransportConnectJobExt transport_connect_job(
        DEFAULT_PRIORITY, SocketTag(), &common_connect_job_params_,
        DefaultParams(), &test_delegate, nullptr /* net_log */);
  test_delegate.StartJobExpectingResult(&transport_connect_job, ERR_NAME_NOT_RESOLVED, false);

  std::vector<IPEndPoint> list;
  IPAddress ip_address;
  IPEndPoint endpoint(ip_address, PORT_ONE);
  int ip_addresses_count = NUM_C;
  transport_connect_job.AddIpv6AddressLimit(ip_addresses_count, list, endpoint);
  EXPECT_EQ(list.size(), NUM_A);

  ip_addresses_count = NUM_B;
  transport_connect_job.AddIpv6AddressLimit(ip_addresses_count, list, endpoint);
  EXPECT_EQ(list.size(), NUM_B);
  EXPECT_EQ(list[NUM_A].port(), PORT_ONE);
}

TEST_F(ArkWebTransportConnectJobExtTest, ClearMultiJobsAndStopTimers) {
  host_resolver_.rules()->AddSimulatedTimeoutFailure(HOST_NAME);
  host_resolver_.set_synchronous_mode(false);
  TestConnectJobDelegate test_delegate;
  ArkWebTransportConnectJobExt transport_connect_job(
        DEFAULT_PRIORITY, SocketTag(), &common_connect_job_params_,
        DefaultParams(), &test_delegate, nullptr /* net_log */);
  test_delegate.StartJobExpectingResult(&transport_connect_job, ERR_NAME_NOT_RESOLVED, false);
  SpdySessionDependencies session_deps_;
  std::unique_ptr<HttpNetworkSession> network_session(SpdySessionDependencies::SpdyCreateSession(&session_deps_));
  common_connect_job_params_.websocket_endpoint_lock_manager = network_session->websocket_endpoint_lock_manager();
  std::vector<IPEndPoint> addresses;
  std::unique_ptr<TransportConnectSubJob> ip_job =
      std::make_unique<TransportConnectSubJob>(addresses, &transport_connect_job,
      TransportConnectJob::SUB_MULTI_JOB);
  TransportConnectSubJob* job = ip_job.get();
  transport_connect_job.multi_connect_jobs_.push_back(std::move(ip_job));
  transport_connect_job.ClearMultiJobsAndStopTimers();
  EXPECT_EQ(transport_connect_job.multi_connect_jobs_.size(), NUM_B);
  EXPECT_EQ(transport_connect_job.multi_connect_jobs_[NUM_A].get(), job);

  common_connect_job_params_.websocket_endpoint_lock_manager = nullptr;
  transport_connect_job.multi_ip_enabled_ = false;
  transport_connect_job.ClearMultiJobsAndStopTimers();
  EXPECT_EQ(transport_connect_job.multi_connect_jobs_.size(), NUM_B);
  EXPECT_EQ(transport_connect_job.multi_connect_jobs_[NUM_A].get(), job);

  transport_connect_job.multi_ip_enabled_ = true;
  transport_connect_job.ClearMultiJobsAndStopTimers();
  EXPECT_EQ(transport_connect_job.multi_connect_jobs_.size(), NUM_A);
}

TEST_F(ArkWebTransportConnectJobExtTest, WillDoMultiConnect) {
  host_resolver_.rules()->AddSimulatedTimeoutFailure(HOST_NAME);
  host_resolver_.set_synchronous_mode(false);
  TestConnectJobDelegate test_delegate;
  ArkWebTransportConnectJobExt transport_connect_job(
        DEFAULT_PRIORITY, SocketTag(), &common_connect_job_params_,
        DefaultParams(), &test_delegate, nullptr /* net_log */);
  test_delegate.StartJobExpectingResult(&transport_connect_job, ERR_NAME_NOT_RESOLVED, false);

  transport_connect_job.multi_ip_enabled_ = false;
  transport_connect_job.WillDoMultiConnect();
  EXPECT_NE(transport_connect_job.multi_connect_timer_.GetCurrentDelay(),
            transport_connect_job.multi_connect_interval_time_);

  transport_connect_job.multi_ip_enabled_ = true;
  SpdySessionDependencies session_deps_;
  std::unique_ptr<HttpNetworkSession> network_session(SpdySessionDependencies::SpdyCreateSession(&session_deps_));
  common_connect_job_params_.websocket_endpoint_lock_manager = network_session->websocket_endpoint_lock_manager();
  transport_connect_job.WillDoMultiConnect();
  EXPECT_NE(transport_connect_job.multi_connect_timer_.GetCurrentDelay(),
            transport_connect_job.multi_connect_interval_time_);

  transport_connect_job.multi_ip_enabled_ = true;
  common_connect_job_params_.websocket_endpoint_lock_manager = nullptr;
  transport_connect_job.multi_connect_ip_addresses_.clear();
  transport_connect_job.WillDoMultiConnect();
  EXPECT_NE(transport_connect_job.multi_connect_timer_.GetCurrentDelay(),
            transport_connect_job.multi_connect_interval_time_);

  transport_connect_job.multi_ip_enabled_ = true;
  common_connect_job_params_.websocket_endpoint_lock_manager = nullptr;
  transport_connect_job.multi_connect_ip_addresses_.push_back(IPEndPoint(ParseIP("192.168.1.11"), PORT_TWO));
  transport_connect_job.multi_connect_ip_addresses_.push_back(IPEndPoint(ParseIP("192.168.1.12"), PORT_THREE));
  transport_connect_job.WillDoMultiConnect();
  EXPECT_EQ(transport_connect_job.multi_connect_timer_.GetCurrentDelay(),
            transport_connect_job.multi_connect_interval_time_);
}

TEST_F(ArkWebTransportConnectJobExtTest, WillDoMultiConnectFallback) {
  host_resolver_.rules()->AddSimulatedTimeoutFailure(HOST_NAME);
  host_resolver_.set_synchronous_mode(false);
  TestConnectJobDelegate test_delegate;
  ArkWebTransportConnectJobExt transport_connect_job(
        DEFAULT_PRIORITY, SocketTag(), &common_connect_job_params_,
        DefaultParams(), &test_delegate, nullptr /* net_log */);
  test_delegate.StartJobExpectingResult(&transport_connect_job, ERR_NAME_NOT_RESOLVED, false);

  transport_connect_job.multi_ip_enabled_ = false;
  transport_connect_job.WillDoMultiConnectFallback();
  EXPECT_NE(transport_connect_job.multi_connect_fallback_timer_.GetCurrentDelay(),
            transport_connect_job.multi_connect_interval_time_);

  transport_connect_job.multi_ip_enabled_ = true;
  SpdySessionDependencies session_deps_;
  std::unique_ptr<HttpNetworkSession> network_session(SpdySessionDependencies::SpdyCreateSession(&session_deps_));
  common_connect_job_params_.websocket_endpoint_lock_manager = network_session->websocket_endpoint_lock_manager();
  transport_connect_job.WillDoMultiConnectFallback();
  EXPECT_NE(transport_connect_job.multi_connect_fallback_timer_.GetCurrentDelay(),
            transport_connect_job.multi_connect_interval_time_);

  transport_connect_job.multi_ip_enabled_ = true;
  common_connect_job_params_.websocket_endpoint_lock_manager = nullptr;
  transport_connect_job.multi_connect_fallback_ip_addresses_.clear();
  transport_connect_job.WillDoMultiConnectFallback();
  EXPECT_NE(transport_connect_job.multi_connect_fallback_timer_.GetCurrentDelay(),
            transport_connect_job.multi_connect_interval_time_);

  transport_connect_job.multi_ip_enabled_ = true;
  common_connect_job_params_.websocket_endpoint_lock_manager = nullptr;
  transport_connect_job.multi_connect_fallback_ip_addresses_.push_back(IPEndPoint(ParseIP("192.168.1.11"), PORT_TWO));
  transport_connect_job.multi_connect_fallback_ip_addresses_.push_back(IPEndPoint(ParseIP("192.168.1.12"), PORT_THREE));
  transport_connect_job.WillDoMultiConnectFallback();
  EXPECT_EQ(transport_connect_job.multi_connect_fallback_timer_.GetCurrentDelay(),
            transport_connect_job.multi_connect_interval_time_);
}

TEST_F(ArkWebTransportConnectJobExtTest, EraseFromMultiJobs) {
  host_resolver_.rules()->AddSimulatedTimeoutFailure(HOST_NAME);
  host_resolver_.set_synchronous_mode(false);
  TestConnectJobDelegate test_delegate;
  ArkWebTransportConnectJobExt transport_connect_job(
        DEFAULT_PRIORITY, SocketTag(), &common_connect_job_params_,
        DefaultParams(), &test_delegate, nullptr /* net_log */);
  test_delegate.StartJobExpectingResult(&transport_connect_job, ERR_NAME_NOT_RESOLVED, false);

  transport_connect_job.multi_connect_jobs_.clear();
  transport_connect_job.EraseFromMultiJobs(nullptr);

  std::vector<std::unique_ptr<TransportConnectSubJob>> multi_connect_jobs;
  std::vector<IPEndPoint> addresses;
  std::unique_ptr<TransportConnectSubJob> ip_job =
      std::make_unique<TransportConnectSubJob>(addresses, &transport_connect_job,
      TransportConnectJob::SUB_MULTI_JOB);
  TransportConnectSubJob* job = ip_job.get();
  transport_connect_job.multi_connect_jobs_.push_back(std::move(ip_job));
  transport_connect_job.EraseFromMultiJobs(nullptr);
  ASSERT_EQ(transport_connect_job.multi_connect_jobs_.size(), NUM_B);
  ASSERT_EQ(transport_connect_job.multi_connect_jobs_[NUM_A].get(), job);

  transport_connect_job.EraseFromMultiJobs(job);
  ASSERT_EQ(transport_connect_job.multi_connect_jobs_.size(), NUM_A);
}

TEST_F(ArkWebTransportConnectJobExtTest, EraseFromMultiFallbackJobs) {
  host_resolver_.rules()->AddSimulatedTimeoutFailure(HOST_NAME);
  host_resolver_.set_synchronous_mode(false);
  TestConnectJobDelegate test_delegate;
  ArkWebTransportConnectJobExt transport_connect_job(
        DEFAULT_PRIORITY, SocketTag(), &common_connect_job_params_,
        DefaultParams(), &test_delegate, nullptr /* net_log */);
  test_delegate.StartJobExpectingResult(&transport_connect_job, ERR_NAME_NOT_RESOLVED, false);

  transport_connect_job.multi_connect_fallback_jobs_.clear();
  transport_connect_job.EraseFromMultiFallbackJobs(nullptr);

  std::vector<std::unique_ptr<TransportConnectSubJob>> multi_connect_jobs;
  std::vector<IPEndPoint> addresses;
  std::unique_ptr<TransportConnectSubJob> ip_job =
      std::make_unique<TransportConnectSubJob>(addresses, &transport_connect_job,
      TransportConnectJob::SUB_MULTI_JOB);
  TransportConnectSubJob* job = ip_job.get();
  transport_connect_job.multi_connect_fallback_jobs_.push_back(std::move(ip_job));
  transport_connect_job.EraseFromMultiFallbackJobs(nullptr);
  ASSERT_EQ(transport_connect_job.multi_connect_fallback_jobs_.size(), NUM_B);
  ASSERT_EQ(transport_connect_job.multi_connect_fallback_jobs_[NUM_A].get(), job);

  transport_connect_job.EraseFromMultiFallbackJobs(job);
  ASSERT_EQ(transport_connect_job.multi_connect_fallback_jobs_.size(), NUM_A);
}

TEST_F(ArkWebTransportConnectJobExtTest, MultiIpSubJobReport) {
  host_resolver_.rules()->AddSimulatedTimeoutFailure(HOST_NAME);
  host_resolver_.set_synchronous_mode(false);
  TestConnectJobDelegate test_delegate;
  ArkWebTransportConnectJobExt transport_connect_job(
        DEFAULT_PRIORITY, SocketTag(), &common_connect_job_params_,
        DefaultParams(), &test_delegate, nullptr /* net_log */);
  test_delegate.StartJobExpectingResult(&transport_connect_job, ERR_NAME_NOT_RESOLVED, false);

  transport_connect_job.multi_ip_enabled_ = false;
  testing::internal::CaptureStderr();
  transport_connect_job.MultiIpSubJobReport(nullptr);
  std::string log_output = testing::internal::GetCapturedStderr();
  ASSERT_EQ(log_output.find("enter GetPeerAddress"), std::string::npos);

  transport_connect_job.multi_ip_enabled_ = true;
  SpdySessionDependencies session_deps_;
  std::unique_ptr<HttpNetworkSession> network_session(SpdySessionDependencies::SpdyCreateSession(&session_deps_));
  common_connect_job_params_.websocket_endpoint_lock_manager = network_session->websocket_endpoint_lock_manager();
  testing::internal::CaptureStderr();
  transport_connect_job.MultiIpSubJobReport(nullptr);
  log_output = testing::internal::GetCapturedStderr();
  ASSERT_EQ(log_output.find("enter GetPeerAddress"), std::string::npos);

  common_connect_job_params_.websocket_endpoint_lock_manager = nullptr;
  std::vector<std::unique_ptr<TransportConnectSubJob>> multi_connect_jobs;
  std::vector<IPEndPoint> addresses;
  std::unique_ptr<TransportConnectSubJob> ip_job =
      std::make_unique<TransportConnectSubJob>(addresses, &transport_connect_job,
      TransportConnectJob::SUB_JOB_IPV4);
  TransportConnectSubJob* job = ip_job.get();
  testing::internal::CaptureStderr();
  transport_connect_job.MultiIpSubJobReport(job);
  log_output = testing::internal::GetCapturedStderr();
  ASSERT_EQ(log_output.find("enter GetPeerAddress"), std::string::npos);

  MockStreamSocket* socket = new MockStreamSocket;
  job->transport_socket_.reset(socket);
  testing::internal::CaptureStderr();
  transport_connect_job.MultiIpSubJobReport(job);
  log_output = testing::internal::GetCapturedStderr();
  ASSERT_EQ(log_output.find("enter GetPeerAddress"), std::string::npos);
}

TEST_F(ArkWebTransportConnectJobExtTest, NeedReportSuccessIp) {
  host_resolver_.rules()->AddSimulatedTimeoutFailure(HOST_NAME);
  host_resolver_.set_synchronous_mode(false);
  TestConnectJobDelegate test_delegate;
  ArkWebTransportConnectJobExt transport_connect_job(
        DEFAULT_PRIORITY, SocketTag(), &common_connect_job_params_,
        DefaultParams(), &test_delegate, nullptr /* net_log */);
  test_delegate.StartJobExpectingResult(&transport_connect_job, ERR_NAME_NOT_RESOLVED, false);

  HostResolverEndpointResult endpoints;
  endpoints.ip_endpoints = {IPEndPoint(ParseIP("192.168.1.11"), PORT_TWO)};
  endpoints.metadata.supported_protocol_alpns = {"h3", "h2", "http/192.168"};
  transport_connect_job.endpoint_results_.push_back(endpoints);
  transport_connect_job.current_endpoint_result_ = NUM_A;
  IPEndPoint point(ParseIP("192.168.1.11"), PORT_TWO);
  transport_connect_job.NeedReportSuccessIp(point, TransportConnectJob::SUB_JOB_IPV4);

  endpoints.ip_endpoints.push_back(IPEndPoint(ParseIP("192.168.1.12"), PORT_THREE));
  transport_connect_job.endpoint_results_.clear();
  transport_connect_job.endpoint_results_.push_back(endpoints);
  testing::internal::CaptureStderr();
  transport_connect_job.NeedReportSuccessIp(point, TransportConnectJob::SUB_JOB_IPV4);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("ip_counter="), std::string::npos);

  transport_connect_job.endpoint_results_.clear();
  transport_connect_job.endpoint_results_.push_back(endpoints);
  testing::internal::CaptureStderr();
  transport_connect_job.NeedReportSuccessIp(IPEndPoint(ParseIP("192.168.1.13"), PORT_ONE),
                                            TransportConnectJob::SUB_JOB_IPV4);
  log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("ip_counter="), std::string::npos);

  transport_connect_job.endpoint_results_.clear();
  transport_connect_job.endpoint_results_.push_back(endpoints);
  testing::internal::CaptureStderr();
  transport_connect_job.NeedReportSuccessIp(IPEndPoint(ParseIP("192.168.1.13"), PORT_ONE),
                                            TransportConnectJob::SUB_MULTI_JOB);
  log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("ip_counter="), std::string::npos);

  transport_connect_job.endpoint_results_.clear();
  transport_connect_job.endpoint_results_.push_back(endpoints);
  testing::internal::CaptureStderr();
  transport_connect_job.NeedReportSuccessIp(IPEndPoint(ParseIP("192.168.1.13"), PORT_ONE),
                                            TransportConnectJob::SUB_MULTI_FALLBACK_JOB);
  log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("ip_counter="), std::string::npos);
}

#endif

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
TEST_F(ArkWebTransportConnectJobExtTest, SetFromPreload) {
  host_resolver_.rules()->AddSimulatedTimeoutFailure(HOST_NAME);
  host_resolver_.set_synchronous_mode(false);
  TestConnectJobDelegate test_delegate;
  ArkWebTransportConnectJobExt transport_connect_job(
        DEFAULT_PRIORITY, SocketTag(), &common_connect_job_params_,
        DefaultParams(), &test_delegate, nullptr /* net_log */);
  test_delegate.StartJobExpectingResult(&transport_connect_job, ERR_NAME_NOT_RESOLVED, false);

  transport_connect_job.SetFromPreload(true);
  ASSERT_TRUE(transport_connect_job.IsFromPreload());

  std::vector<std::unique_ptr<TransportConnectSubJob>> multi_connect_jobs;
  std::vector<IPEndPoint> addresses;
  std::unique_ptr<TransportConnectSubJob> ip_job_a =
      std::make_unique<TransportConnectSubJob>(addresses, &transport_connect_job,
      TransportConnectJob::SUB_MULTI_JOB);

  transport_connect_job.ipv4_job_.reset(ip_job_a.release());

  std::unique_ptr<TransportConnectSubJob> ip_job_b =
      std::make_unique<TransportConnectSubJob>(addresses, &transport_connect_job,
      TransportConnectJob::SUB_MULTI_JOB);
  transport_connect_job.ipv6_job_.reset(ip_job_b.release());
  transport_connect_job.SetFromPreload(true);
  ASSERT_TRUE(transport_connect_job.ipv4_job_->IsFromPreload());
  ASSERT_TRUE(transport_connect_job.ipv6_job_->IsFromPreload());
}

#endif
}
}