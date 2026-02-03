/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/services/network/public/cpp/navigation_info_mojom_traits.h"

#include "mojo/public/cpp/bindings/struct_traits.h"

namespace mojo {

// static
bool StructTraits<network::mojom::ResolveInfoDataView, net::ResolveInfo>::Read(
    network::mojom::ResolveInfoDataView data,
    net::ResolveInfo* out) {
  if (!data.ReadQueryHost(&out->query_host) ||
      !data.ReadIpEndpoints(&out->ip_endpoints) ||
      !data.ReadTruncationIps(&out->truncation_ips)) {
    return false;
  }
  out->error_code = data.error_code();
  out->dns_status = data.dns_status();
  out->dns_transition = data.dns_transition();
  return true;
}

// static
bool StructTraits<network::mojom::DnsInfoDataView, net::DnsInfo>::Read(
    network::mojom::DnsInfoDataView data,
    net::DnsInfo* out) {
  if (!data.ReadHost(&out->host) || !data.ReadAddressList(&out->address_list) ||
      !data.ReadTruncationIps(&out->truncation_ips)) {
    return false;
  }
  out->result = data.result();
  out->dns_transition = data.dns_transition();
  out->dns_status = data.dns_status();
  return true;
}

// static
bool StructTraits<network::mojom::ArkWebSSLInfoDataView, net::ArkWebSSLInfo>::
    Read(network::mojom::ArkWebSSLInfoDataView data, net::ArkWebSSLInfo* out) {
  if (!data.ReadHost(&out->host) || !data.ReadIssuer(&out->issuer) ||
      !data.ReadExpiredDate(&out->expired_date)) {
    return false;
  }
  out->result = data.result();
  out->is_fatal_cert_error = data.is_fatal_cert_error();
  return true;
}

// static
bool StructTraits<network::mojom::SocketInfoDataView, net::SocketInfo>::Read(
    network::mojom::SocketInfoDataView data,
    net::SocketInfo* out) {
  if (!data.ReadHost(&out->host) || !data.ReadAddressList(&out->address_list)) {
    return false;
  }
  out->result = data.result();
  return true;
}

// static
bool StructTraits<network::mojom::RequestAttemptDataView, net::RequestAttempt>::
    Read(network::mojom::RequestAttemptDataView data,
         net::RequestAttempt* out) {
  if (!data.ReadRequestTraceId(&out->request_trace_id) ||
      !data.ReadDnsInfo(&out->dns_info) || !data.ReadSslInfo(&out->ssl_info) ||
      !data.ReadSocketInfo(&out->socket_info)) {
    return false;
  }
  out->attempt_type = data.attempt_type();
  out->request_result = data.request_result();
  out->was_fetched_via_proxy = data.was_fetched_via_proxy();
  return true;
}

// static
bool StructTraits<network::mojom::NavigationInfoDataView, net::NavigationInfo>::
    Read(network::mojom::NavigationInfoDataView data,
         net::NavigationInfo* out) {
  if (!data.ReadRequestUrl(&out->request_url) ||
      !data.ReadRequestUuid(&out->request_uuid) ||
      !data.ReadTimeStamp(&out->time_stamp) ||
      !data.ReadDnsNameServers(&out->dns_name_servers) ||
      !data.ReadLocalAddress(&out->local_address) ||
      !data.ReadResolveInfo(&out->resolve_info) ||
      !data.ReadRequestAttempts(&out->request_attempts)) {
    return false;
  }
  out->original_error_code = data.original_error_code();
  out->error_code = data.error_code();
  out->can_use_secure_dns = data.can_use_secure_dns();
  out->is_fallback_proxy_enabled = data.is_fallback_proxy_enabled();
  return true;
}

}  // namespace mojo