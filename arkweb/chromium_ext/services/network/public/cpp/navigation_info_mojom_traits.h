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

#ifndef SERVICES_NETWORK_PUBLIC_CPP_NAVIGATION_INFO_MOJOM_TRAITS_H_
#define SERVICES_NETWORK_PUBLIC_CPP_NAVIGATION_INFO_MOJOM_TRAITS_H_

#include "arkweb/chromium_ext/net/base/navigation_info.h"
#include "arkweb/chromium_ext/services/network/public/mojom/navigation_info.mojom-shared.h"
#include "base/component_export.h"
#include "mojo/public/cpp/bindings/struct_traits.h"
#include "net/base/net_errors.h"

namespace mojo {

template <>
class COMPONENT_EXPORT(NETWORK_CPP_NAVIGATION_INFO)
    StructTraits<network::mojom::ResolveInfoDataView, net::ResolveInfo> {
 public:
  static int error_code(const net::ResolveInfo& resolve_info) {
    return resolve_info.error_code;
  }

  static int dns_status(const net::ResolveInfo& resolve_info) {
    return resolve_info.dns_status;
  }

  static uint32_t dns_transition(const net::ResolveInfo& resolve_info) {
    return resolve_info.dns_transition;
  }

  static const std::string& query_host(const net::ResolveInfo& resolve_info) {
    return resolve_info.query_host;
  }

  static const std::vector<std::string> ip_endpoints(
      const net::ResolveInfo& resolve_info) {
    return resolve_info.ip_endpoints;
  }

  static const std::vector<std::string> truncation_ips(
      const net::ResolveInfo& resolve_info) {
    return resolve_info.truncation_ips;
  }

  static bool Read(network::mojom::ResolveInfoDataView data,
                   net::ResolveInfo* out);
};

template <>
class COMPONENT_EXPORT(NETWORK_CPP_NAVIGATION_INFO)
    StructTraits<network::mojom::DnsInfoDataView, net::DnsInfo> {
 public:
  static int result(const net::DnsInfo& dns_info) { return dns_info.result; }

  static uint32_t dns_transition(const net::DnsInfo& dns_info) {
    return dns_info.dns_transition;
  }

  static int dns_status(const net::DnsInfo& dns_info) {
    return dns_info.dns_status;
  }

  static const std::string& host(const net::DnsInfo& dns_info) {
    return dns_info.host;
  }

  static const std::vector<std::string> address_list(
      const net::DnsInfo& dns_info) {
    return dns_info.address_list;
  }

  static const std::vector<std::string> truncation_ips(
      const net::DnsInfo& dns_info) {
    return dns_info.truncation_ips;
  }

  static bool Read(network::mojom::DnsInfoDataView data, net::DnsInfo* out);
};

template <>
class COMPONENT_EXPORT(NETWORK_CPP_NAVIGATION_INFO)
    StructTraits<network::mojom::ArkWebSSLInfoDataView, net::ArkWebSSLInfo> {
 public:
  static const std::string& host(const net::ArkWebSSLInfo& arkweb_ssl_info) {
    return arkweb_ssl_info.host;
  }

  static int result(const net::ArkWebSSLInfo& arkweb_ssl_info) {
    return arkweb_ssl_info.result;
  }

  static const std::string& issuer(const net::ArkWebSSLInfo& arkweb_ssl_info) {
    return arkweb_ssl_info.issuer;
  }

  static bool is_fatal_cert_error(const net::ArkWebSSLInfo& arkweb_ssl_info) {
    return arkweb_ssl_info.is_fatal_cert_error;
  }

  static const std::string& expired_date(
      const net::ArkWebSSLInfo& arkweb_ssl_info) {
    return arkweb_ssl_info.expired_date;
  }

  static bool Read(network::mojom::ArkWebSSLInfoDataView data,
                   net::ArkWebSSLInfo* out);
};

template <>
class COMPONENT_EXPORT(NETWORK_CPP_NAVIGATION_INFO)
    StructTraits<network::mojom::SocketInfoDataView, net::SocketInfo> {
 public:
  static const std::string& host(const net::SocketInfo& socket_info) {
    return socket_info.host;
  }

  static int result(const net::SocketInfo& socket_info) {
    return socket_info.result;
  }

  static const std::vector<std::string> address_list(
      const net::SocketInfo& socket_info) {
    return socket_info.address_list;
  }

  static bool Read(network::mojom::SocketInfoDataView data,
                   net::SocketInfo* out);
};

template <>
class COMPONENT_EXPORT(NETWORK_CPP_NAVIGATION_INFO)
    StructTraits<network::mojom::RequestAttemptDataView, net::RequestAttempt> {
 public:
  static const std::string& request_trace_id(
      const net::RequestAttempt& request_attempt) {
    return request_attempt.request_trace_id;
  }

  static int attempt_type(const net::RequestAttempt& request_attempt) {
    return request_attempt.attempt_type;
  }

  static int request_result(const net::RequestAttempt& request_attempt) {
    return request_attempt.request_result;
  }

  static bool was_fetched_via_proxy(
      const net::RequestAttempt& request_attempt) {
    return request_attempt.was_fetched_via_proxy;
  }

  static const net::DnsInfo& dns_info(
      const net::RequestAttempt& request_attempt) {
    return request_attempt.dns_info;
  }

  static const net::ArkWebSSLInfo& ssl_info(
      const net::RequestAttempt& request_attempt) {
    return request_attempt.ssl_info;
  }

  static const net::SocketInfo& socket_info(
      const net::RequestAttempt& request_attempt) {
    return request_attempt.socket_info;
  }

  static bool Read(network::mojom::RequestAttemptDataView data,
                   net::RequestAttempt* out);
};

template <>
class COMPONENT_EXPORT(NETWORK_CPP_NAVIGATION_INFO)
    StructTraits<network::mojom::NavigationInfoDataView, net::NavigationInfo> {
 public:
  // String members
  static const std::string& request_url(const net::NavigationInfo& nav_info) {
    return nav_info.request_url;
  }

  static const std::string& request_uuid(const net::NavigationInfo& nav_info) {
    return nav_info.request_uuid;
  }

  static const std::string& time_stamp(const net::NavigationInfo& nav_info) {
    return nav_info.time_stamp;
  }

  static const std::string& dns_name_servers(
      const net::NavigationInfo& nav_info) {
    return nav_info.dns_name_servers;
  }

  static const std::string& local_address(const net::NavigationInfo& nav_info) {
    return nav_info.local_address;
  }

  // Integer members
  static int32_t original_error_code(const net::NavigationInfo& nav_info) {
    return nav_info.original_error_code;
  }

  static int32_t error_code(const net::NavigationInfo& nav_info) {
    return nav_info.error_code;
  }

  // Boolean members
  static bool can_use_secure_dns(const net::NavigationInfo& nav_info) {
    return nav_info.can_use_secure_dns;
  }

  static bool is_fallback_proxy_enabled(const net::NavigationInfo& nav_info) {
    return nav_info.is_fallback_proxy_enabled;
  }

  // Complex type members
  static const net::ResolveInfo& resolve_info(
      const net::NavigationInfo& nav_info) {
    return nav_info.resolve_info;
  }

  static const std::vector<net::RequestAttempt>& request_attempts(
      const net::NavigationInfo& nav_info) {
    return nav_info.request_attempts;
  }

  static bool Read(network::mojom::NavigationInfoDataView data,
                   net::NavigationInfo* out);
};

}  // namespace mojo

#endif  // SERVICES_NETWORK_PUBLIC_CPP_NAVIGATION_INFO_MOJOM_TRAITS_H_