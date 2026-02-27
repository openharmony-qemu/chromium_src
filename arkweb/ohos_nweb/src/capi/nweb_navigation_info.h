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

#ifndef OHOS_NWEB_WEB_NAVIGATION_INFO_H_
#define OHOS_NWEB_WEB_NAVIGATION_INFO_H_

#include <string>
#include <vector>

struct WebDnsInfo {
  std::string host;
  int result;
  std::string address_list;
  int dns_transition;
  std::string dns_record_truncation;
};

struct WebSocketInfo {
  std::string host;
  int result;
  std::string address_list;
};

struct WebSSLInfo {
  std::string host;
  int result;
  std::string issuer;
  bool is_fatal_error;
  std::string expired_date;
};

struct WebRequestAttempt {
  std::string request_trace_id;
  int attempt_type;
  int request_attempt_result;
  bool was_fetched_via_proxy;
  WebDnsInfo dns_info;
  WebSocketInfo socket_info;
  WebSSLInfo ssl_info;
};

struct NWebNavigationInfo {
  std::string url;
  std::string original_url;
  std::string request_trace_id;
  std::string page_trace_id;
  std::string time_stamp;
  std::string name_servers;
  std::string local_ip;
  int original_error_code;
  int error_code;
  bool is_https_dns_enabled;
  bool did_use_https_dns;
  bool is_fallback_proxy_enabled;
  bool did_use_fallback_proxy;
  int website_policy;
  int hw_code;
  int connection_type;
  std::string insecure_dns_records;
  std::string secure_dns_records;
  bool is_captive_portal;
  std::vector<WebRequestAttempt> request_attempts;
  bool is_auto_reload;
  int auto_reload_reason;
  bool has_ignore_certificate_error;
};

#endif  //  OHOS_NWEB_WEB_NAVIGATION_INFO_H_
