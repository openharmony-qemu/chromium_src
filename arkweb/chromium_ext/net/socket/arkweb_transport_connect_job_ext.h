// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef ARKWEB_NET_SOCKET_TRANSPORT_CONNECT_JOB_EXT_H_
#define ARKWEB_NET_SOCKET_TRANSPORT_CONNECT_JOB_EXT_H_

#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "base/containers/flat_set.h"
#include "base/containers/span.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "net/base/host_port_pair.h"
#include "net/base/net_export.h"
#include "net/base/network_anonymization_key.h"
#include "net/dns/host_resolver.h"
#include "net/dns/public/host_resolver_results.h"
#include "net/dns/public/resolve_error_info.h"
#include "net/dns/public/secure_dns_policy.h"
#include "net/socket/connect_job.h"
#include "net/socket/connection_attempts.h"
#include "third_party/abseil-cpp/absl/types/variant.h"
#include "url/scheme_host_port.h"

#include "net/socket/transport_connect_job.h"

namespace net {

class NetLogWithSource;
class SocketTag;

class NET_EXPORT_PRIVATE ArkWebTransportConnectJobExt : public TransportConnectJob {
 public:
  friend class TransportConnectJob;

  ArkWebTransportConnectJobExt(
      RequestPriority priority,
      const SocketTag& socket_tag,
      const CommonConnectJobParams* common_connect_job_params,
      const scoped_refptr<TransportSocketParams>& params,
      Delegate* delegate,
      const NetLogWithSource* net_log,
      std::optional<EndpointResultOverride> endpoint_result_override = std::nullopt);

  ArkWebTransportConnectJobExt(const ArkWebTransportConnectJobExt&) = delete;
  ArkWebTransportConnectJobExt& operator=(const ArkWebTransportConnectJobExt&) = delete;

  ~ArkWebTransportConnectJobExt() override;

  ArkWebTransportConnectJobExt *AsArkWebTransportConnectJobExt() override { return this; }

#if BUILDFLAG(ARKWEB_MULTI_IP_CONNECT)
  void AddIpv4AddressLimit(int& ip_addresses_count,
      std::vector<IPEndPoint>& ipv4_addresses_limi, const IPEndPoint& ip_endpointt);
  void AddIpv6AddressLimit(int& ip_addresses_count,
      std::vector<IPEndPoint>& ipv6_addresses_limi, const IPEndPoint& ip_endpointt);
#endif
#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
  void SetFromPreload(bool from_preload) override;
#endif
#if BUILDFLAG(ARKWEB_EXT_NETWORK_CONNECTION)
  void SetConnectTimeout(int timeout_override) override;
#endif

 private:
#if BUILDFLAG(ARKWEB_MULTI_IP_CONNECT)
  void ClearMultiJobsAndStopTimers();
  void WillDoMultiConnect();
  void StartMultiConnectJobs();
  void WillDoMultiConnectFallback();
  void EraseFromMultiJobs(TransportConnectSubJob* job);
  void EraseFromMultiFallbackJobs(TransportConnectSubJob* job);
  void StartMultiConnectFallbackJobs();
  void MultiIpSubJobReport(TransportConnectSubJob* job);
  void NeedReportSuccessIp(const IPEndPoint& address, SubJobType type);
  void ReportSuccessIp(int success_index,
                       int ip_addresses_num,
                       SubJobType type);
#endif  // BUILDFLAG(ARKWEB_MULTI_IP_CONNECT)

  base::WeakPtrFactory<ArkWebTransportConnectJobExt> weak_ptr_factory_ext_{this};
};

}  // namespace net

#endif  // ARKWEB_NET_SOCKET_TRANSPORT_CONNECT_JOB_EXT_H_
