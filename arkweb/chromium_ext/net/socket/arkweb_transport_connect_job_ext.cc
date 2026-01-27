// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "net/socket/transport_connect_job.h"
#include "arkweb/chromium_ext/net/socket/arkweb_transport_connect_job_ext.h"

#include <memory>
#include <utility>

#include "arkweb/chromium_ext/url/ohos/log_utils.h"
#include "base/check_op.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/metrics/histogram_macros.h"
#include "base/notreached.h"
#include "base/task/single_thread_task_runner.h"
#include "base/time/time.h"
#include "net/base/features.h"
#include "net/base/host_port_pair.h"
#include "net/base/ip_endpoint.h"
#include "net/base/net_errors.h"
#include "net/base/trace_constants.h"
#include "net/base/tracing.h"
#include "net/dns/public/host_resolver_results.h"
#include "net/dns/public/secure_dns_policy.h"
#include "net/log/net_log_event_type.h"
#include "net/socket/socket_tag.h"
#include "net/socket/transport_connect_sub_job.h"
#include "third_party/abseil-cpp/absl/types/variant.h"
#include "url/scheme_host_port.h"
#include "url/url_constants.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
#include "base/ohos/nweb_engine_event_logger.h"
#include "base/ohos/nweb_engine_event_logger_code.h"
#endif

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
#include "url/ohos/log_utils.h"
#endif

namespace net {

#if BUILDFLAG(ARKWEB_MULTI_IP_CONNECT)
constexpr size_t kMinRequiredIpEndpoints = 2;
#endif

#if BUILDFLAG(ARKWEB_EX_NETWORK_CONNECTION)
void ArkWebTransportConnectJobExt::SetConnectTimeout(int timeout_override) {
  timeout_override_ = base::Seconds(timeout_override);
}
#endif

ArkWebTransportConnectJobExt::ArkWebTransportConnectJobExt(
    RequestPriority priority,
    const SocketTag& socket_tag,
    const CommonConnectJobParams* common_connect_job_params,
    const scoped_refptr<TransportSocketParams>& params,
    Delegate* delegate,
    const NetLogWithSource* net_log,
    std::optional<EndpointResultOverride> endpoint_result_override)
    : TransportConnectJob(priority,
                          socket_tag,
                          common_connect_job_params,
                          params,
                          delegate,
                          net_log,
                          endpoint_result_override) {}

ArkWebTransportConnectJobExt::~ArkWebTransportConnectJobExt() = default;

#if BUILDFLAG(ARKWEB_MULTI_IP_CONNECT)
void ArkWebTransportConnectJobExt::AddIpv4AddressLimit(
    int& ip_addresses_count, std::vector<IPEndPoint>& ipv4_addresses_limit, const IPEndPoint& ip_endpoint) {
  if (ip_addresses_count < kMaxMultiSocketNum) {
    ipv4_addresses_limit.push_back(ip_endpoint);
  }
  ip_addresses_count++;
}

void ArkWebTransportConnectJobExt::AddIpv6AddressLimit(
    int& ip_addresses_count, std::vector<IPEndPoint>& ipv6_addresses_limit, const IPEndPoint& ip_endpoint) {
  if (ip_addresses_count < kMaxMultiSocketNum) {
    ipv6_addresses_limit.push_back(ip_endpoint);
  }
  ip_addresses_count++;
}

void ArkWebTransportConnectJobExt::ClearMultiJobsAndStopTimers() {
  if (!websocket_endpoint_lock_manager()) {
    if (multi_ip_enabled_) {
      multi_connect_timer_.Stop();
      multi_connect_fallback_timer_.Stop();
      multi_connect_jobs_.clear();
      multi_connect_fallback_jobs_.clear();
    }
  }
}

void ArkWebTransportConnectJobExt::WillDoMultiConnect() {
  if (multi_ip_enabled_ && !websocket_endpoint_lock_manager() &&
      multi_connect_ip_addresses_.size() > 1) {
    multi_connect_timer_.Start(
        FROM_HERE, multi_connect_interval_time_,
        base::BindRepeating(&ArkWebTransportConnectJobExt::StartMultiConnectJobs,
                            weak_ptr_factory_ext_.GetWeakPtr()));
  }
}

void ArkWebTransportConnectJobExt::StartMultiConnectJobs() {
  // The timer should only fire while we're waiting for the main connect to
  // succeed.
  if (next_state_ != STATE_TRANSPORT_CONNECT_COMPLETE) {
    LOG(ERROR) << "StartMultiConnectJobs ret for next_state_[" << next_state_
               << "] is not expected";
    return;
  }

  net_log().AddEvent(NetLogEventType::TRANSPORT_MULTI_CONNECT_JOB);
  multi_connect_ip_addresses_.erase(multi_connect_ip_addresses_.begin());
  std::unique_ptr<TransportConnectSubJob> ip_job =
      std::make_unique<TransportConnectSubJob>(multi_connect_ip_addresses_,
                                               this, SUB_MULTI_JOB);
  int result = ip_job->Start();
  if (multi_connect_ip_addresses_.size() <= 1) {
    multi_connect_timer_.Stop();
  }
  if (result != ERR_IO_PENDING) {
    OnSubJobComplete(result, ip_job.get());
    return;
  }
  multi_connect_jobs_.push_back(std::move(ip_job));
}

void ArkWebTransportConnectJobExt::WillDoMultiConnectFallback() {
  if (multi_ip_enabled_ && !websocket_endpoint_lock_manager() &&
      multi_connect_fallback_ip_addresses_.size() > 1) {
    multi_connect_fallback_timer_.Start(
        FROM_HERE, multi_connect_interval_time_,
        base::BindRepeating(&ArkWebTransportConnectJobExt::StartMultiConnectFallbackJobs,
                            weak_ptr_factory_ext_.GetWeakPtr()));
  }
}

void ArkWebTransportConnectJobExt::EraseFromMultiJobs(TransportConnectSubJob* job) {
  for (auto it = multi_connect_jobs_.begin();
       it != multi_connect_jobs_.end(); it++) {
    if (it->get() == job) {
      multi_connect_jobs_.erase(it);
      break;
    }
  }
}

void ArkWebTransportConnectJobExt::EraseFromMultiFallbackJobs(TransportConnectSubJob* job) {
  for (auto it = multi_connect_fallback_jobs_.begin();
       it != multi_connect_fallback_jobs_.end(); it++) {
    if (it->get() == job) {
      multi_connect_fallback_jobs_.erase(it);
      break;
    }
  }
}

void ArkWebTransportConnectJobExt::StartMultiConnectFallbackJobs() {
  // The timer should only fire while we're waiting for the main connect to
  // succeed.
  if (next_state_ != STATE_TRANSPORT_CONNECT_COMPLETE) {
    LOG(ERROR) << "StartMultiConnectFallbackJobs ret for next_state_["
               << next_state_ << "] is not expected";
    return;
  }

  net_log().AddEvent(NetLogEventType::TRANSPORT_MULTI_CONNECT_JOB_FALLBACK);
  multi_connect_fallback_ip_addresses_.erase(
      multi_connect_fallback_ip_addresses_.begin());
  std::unique_ptr<TransportConnectSubJob> ip_job =
      std::make_unique<TransportConnectSubJob>(
          multi_connect_fallback_ip_addresses_, this, SUB_MULTI_FALLBACK_JOB);
  int result = ip_job->Start();
  if (multi_connect_fallback_ip_addresses_.size() <= 1) {
    multi_connect_fallback_timer_.Stop();
  }
  if (result != ERR_IO_PENDING) {
    OnSubJobComplete(result, ip_job.get());
    return;
  }
  multi_connect_fallback_jobs_.push_back(std::move(ip_job));
}

void ArkWebTransportConnectJobExt::MultiIpSubJobReport(TransportConnectSubJob* job) {
  if (multi_ip_enabled_ && !websocket_endpoint_lock_manager() &&
      job->Socket() && job->type() > SUB_JOB_IPV6) {
    IPEndPoint address;
    job->Socket()->GetPeerAddress(&address);
    NeedReportSuccessIp(address, job->type());
  }
}

void ArkWebTransportConnectJobExt::NeedReportSuccessIp(const IPEndPoint& address,
                                              SubJobType type) {
  const HostResolverEndpointResult& endpoint =
      GetEndpointResultForCurrentSubJobs();
  if (endpoint.ip_endpoints.size() < kMinRequiredIpEndpoints) {
    return;
  }

  int success_index = -1;
  for (int i = 0; i < endpoint.ip_endpoints.size(); i++) {
    if (endpoint.ip_endpoints[i] == address) {
      success_index = i + 1;
      break;
    }
  }

  // Todo: The check for success_index = 1 should be deleted
  // when the report is supported.
  if (success_index == 1) {
    return;
  }

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  if (type == SUB_MULTI_JOB || type == SUB_MULTI_FALLBACK_JOB) {
    LOG_FEEDBACK(INFO) << "success ip = " << address.ToString()
                       << ", and success index = " << success_index
                       << ", job_type " << (int)type;
  }
#endif

  ReportSuccessIp(success_index, endpoint.ip_endpoints.size(), type);
}

void ArkWebTransportConnectJobExt::ReportSuccessIp(int success_index,
                                          int ip_addresses_num,
                                          SubJobType type) {
  std::ostringstream ostr;
  ostr << "ip_counter=" << ip_addresses_num
       << ", succeed_number=" << success_index << ", job_type=" << (int)type;
  std::string host = ToLegacyDestinationEndpoint(params_->destination()).host();
  LOG(DEBUG) << "event_message: " << ostr.str() << ", resource: ***";

#if BUILDFLAG(IS_ARKWEB)
  // 打点
  base::ohos::ReportEngineEvent(
    base::ohos::kModuleNet,
    base::ohos::kDefaultUrl,
    base::ohos::kMultiIpConnect,
    ostr.str());
#endif
}
#endif  // BUILDFLAG(ARKWEB_MULTI_IP_CONNECT)

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
void ArkWebTransportConnectJobExt::SetFromPreload(bool from_preload)
{
  ConnectJob::SetFromPreload(from_preload);
  if (ipv4_job_) {
    ipv4_job_->SetFromPreload(from_preload);
  }
  if (ipv6_job_) {
    ipv6_job_->SetFromPreload(from_preload);
  }
}
#endif
}  // namespace net
