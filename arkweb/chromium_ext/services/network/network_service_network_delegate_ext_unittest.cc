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

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/test/bind.h"
#include "base/test/task_environment.h"
#define private public
#include "services/network/network_context.h"
#include "services/network/network_quality_estimator_manager.h"
#include "services/network/network_service.h"
#undef private
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "net/base/net_errors.h"
#include "net/nqe/network_quality_estimator.h"
#include "net/reporting/reporting_service.h"
#include "net/url_request/url_request_context_builder.h"
#include "net/url_request/url_request_test_util.h"
#include "services/network/network_service_network_delegate.h"
#include "services/network/test/fake_test_cert_verifier_params_factory.h"
#include "services/network/test/test_utils.h"
#include "services/network/test/url_loader_context_for_tests.h"
#include "services/network/url_loader.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace network {

class NetworkServiceNetworkDelegateExtTest : public ::testing::Test {
 public:
  void SetUp() override {
    network_service_ = NetworkService::CreateForTesting();
    network_context_ =
        CreateContextWithParams(CreateNetworkContextParamsForTesting(), true);
    delegate_ext_ = std::make_unique<network::NetworkServiceNetworkDelegateExt>(
        true, false, mojo::PendingRemote<network::mojom::ProxyErrorClient>(),
        network_context_.get());
  }

  void TearDown() override {
    delegate_ext_.reset();
    network_context_.reset();
    network_service_.reset();
  }

 protected:
  std::unique_ptr<NetworkContext> CreateContextWithParams(
      mojom::NetworkContextParamsPtr context_params,
      bool is_strict_log_mode,
      std::unique_ptr<net::ReportingService> reporting_service = nullptr) {
    context_params->cert_verifier_params =
        FakeTestCertVerifierParamsFactory::GetCertVerifierParams();
    context_params->is_strict_log_mode = is_strict_log_mode;
    network_context_remote_.reset();
    return NetworkContext::CreateForTesting(
        network_service_.get(),
        network_context_remote_.BindNewPipeAndPassReceiver(),
        std::move(context_params),
        base::BindLambdaForTesting([&](net::URLRequestContextBuilder* builder) {
          builder->DisableHttpCache();
          builder->SetCookieStore(
              std::make_unique<net::CookieMonster>(nullptr, nullptr));
        }));
  }

  std::unique_ptr<net::URLRequest> MakeRequest(net::TestDelegate* delegate,
                                               const GURL& url) {
    auto builder = net::CreateTestURLRequestContextBuilder();
    builder->DisableHttpCache();
    auto ctx = builder->Build();
    net::URLRequestContext* raw = ctx.get();
    contexts_.push_back(std::move(ctx));
    return raw->CreateRequest(url, net::DEFAULT_PRIORITY, delegate);
  }

  base::test::TaskEnvironment task_env_;
  std::unique_ptr<NetworkContext> network_context_;
  std::unique_ptr<NetworkServiceNetworkDelegateExt> delegate_ext_;
  std::vector<std::unique_ptr<net::URLRequestContext>> contexts_;
  std::unique_ptr<NetworkService> network_service_;
  mojo::Remote<mojom::NetworkContext> network_context_remote_;
};

#if BUILDFLAG(ARKWEB_EXT_LOG_MESSAGE)

TEST_F(NetworkServiceNetworkDelegateExtTest,
       RecordErrorInfo_NonQuic_NoUrlLoader) {
  net::TestDelegate delegate;
  auto req = MakeRequest(&delegate, GURL("https://example.com/"));
  testing::internal::CaptureStderr();

  delegate_ext_->RecordErrorInfo(req.get(), net::ERR_TIMED_OUT);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("ERR_TIMED_OUT"), std::string::npos);
}

TEST_F(NetworkServiceNetworkDelegateExtTest, RecordErrorInfo_Quic_NoUrlLoader) {
  net::TestDelegate delegate;
  auto req = MakeRequest(&delegate, GURL("https://example.com/"));
  testing::internal::CaptureStderr();
  delegate_ext_->RecordErrorInfo(req.get(), net::ERR_QUIC_PROTOCOL_ERROR);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("ERR_QUIC_PROTOCOL_ERROR"), std::string::npos);
}

TEST_F(NetworkServiceNetworkDelegateExtTest,
       RecordErrorInfo_LoggerSwitch_On_NoCrash) {
  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      ::switches::kEnableLoggerReport);

  net::TestDelegate delegate;
  auto req = MakeRequest(&delegate, GURL("https://example.com/"));
  testing::internal::CaptureStderr();
  delegate_ext_->RecordErrorInfo(req.get(), net::ERR_FAILED);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("ERR_FAILED"), std::string::npos);
  base::CommandLine::ForCurrentProcess()->RemoveSwitch(
      switches::kEnableLoggerReport);
}

TEST_F(NetworkServiceNetworkDelegateExtTest,
       RecordErrorInfo_LoggerSwitch_StrictMode_False) {
  delegate_ext_.reset();

  auto context_params = CreateNetworkContextParamsForTesting();
  context_params->is_strict_log_mode = false;
  auto arkweb_network_context =
      CreateContextWithParams(CreateNetworkContextParamsForTesting(), false);
  delegate_ext_ = std::make_unique<NetworkServiceNetworkDelegateExt>(
      true, false, mojo::PendingRemote<network::mojom::ProxyErrorClient>(),
      arkweb_network_context.get());

  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      ::switches::kEnableLoggerReport);

  net::TestDelegate delegate;
  auto req = MakeRequest(&delegate, GURL("https://example.com/"));
  testing::internal::CaptureStderr();
  delegate_ext_->RecordErrorInfo(req.get(), net::ERR_FAILED);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("ERR_FAILED"), std::string::npos);
  base::CommandLine::ForCurrentProcess()->RemoveSwitch(
      switches::kEnableLoggerReport);
}

TEST_F(NetworkServiceNetworkDelegateExtTest,
       RecordErrorInfo_LoggerSwitch_Long_Url) {
  delegate_ext_.reset();

  auto context_params = CreateNetworkContextParamsForTesting();
  context_params->is_strict_log_mode = false;
  auto arkweb_network_context =
      CreateContextWithParams(CreateNetworkContextParamsForTesting(), false);

  delegate_ext_ = std::make_unique<NetworkServiceNetworkDelegateExt>(
      true, false, mojo::PendingRemote<network::mojom::ProxyErrorClient>(),
      arkweb_network_context.get());

  base::CommandLine::ForCurrentProcess()->AppendSwitch(
      ::switches::kEnableLoggerReport);

  std::string long_url = "https://example.com/";
  for (int32_t i = 0; i < 2000; ++i) {
    long_url += "a";
  }

  net::TestDelegate delegate;
  auto req = MakeRequest(&delegate, GURL(long_url));
  testing::internal::CaptureStderr();
  delegate_ext_->RecordErrorInfo(req.get(), net::ERR_FAILED);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("ERR_FAILED"), std::string::npos);
  base::CommandLine::ForCurrentProcess()->RemoveSwitch(
      switches::kEnableLoggerReport);
}

TEST_F(NetworkServiceNetworkDelegateExtTest,
       GetDownStreamThroughputKbps_NetworkService_Null) {
  auto context_params = CreateNetworkContextParamsForTesting();
  auto network_context =
      CreateContextWithParams(std::move(context_params), false);

  const_cast<raw_ptr<NetworkService>&>(network_context->network_service_) =
      nullptr;

  auto test_delegate = std::make_unique<NetworkServiceNetworkDelegateExt>(
      true, false, mojo::PendingRemote<network::mojom::ProxyErrorClient>(),
      network_context.get());
  EXPECT_EQ(test_delegate->GetDownStreamThroughputKbps(), 0);
}

TEST_F(NetworkServiceNetworkDelegateExtTest,
       GetDownStreamThroughputKbps_NetworkQualityEstimator_Null) {
  auto context_params = CreateNetworkContextParamsForTesting();
  auto network_context =
      CreateContextWithParams(std::move(context_params), false);
  auto* network_service = network_context->network_service();
  ASSERT_NE(network_service, nullptr);
  auto* mgr = network_service->network_quality_estimator_manager_.get();
  ASSERT_NE(nullptr, mgr);
  std::unique_ptr<net::NetworkQualityEstimator> saved =
      std::move(mgr->network_quality_estimator_);
  mgr->network_quality_estimator_.reset();

  auto test_delegate = std::make_unique<NetworkServiceNetworkDelegateExt>(
      true, false, mojo::PendingRemote<network::mojom::ProxyErrorClient>(),
      network_context.get());
  EXPECT_EQ(test_delegate->GetDownStreamThroughputKbps(), 0);
  mgr->network_quality_estimator_ = std::move(saved);
}

TEST_F(NetworkServiceNetworkDelegateExtTest, GetDownStreamThroughputKbps) {
  auto context_params = CreateNetworkContextParamsForTesting();
  auto network_context =
      CreateContextWithParams(std::move(context_params), false);

  auto test_delegate = std::make_unique<NetworkServiceNetworkDelegateExt>(
      true, false, mojo::PendingRemote<network::mojom::ProxyErrorClient>(),
      network_context.get());
  EXPECT_NE(test_delegate->GetDownStreamThroughputKbps(), 0);
}
#endif  // BUILDFLAG(ARKWEB_EXT_LOG_MESSAGE)

}  // namespace network