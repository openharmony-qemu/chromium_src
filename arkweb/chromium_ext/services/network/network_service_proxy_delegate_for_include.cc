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

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/command_line.h"
#include "net/http/http_status_code.h"

namespace network {

NetworkServiceProxyDelegate::PageSafeBrowsingInfo::PageSafeBrowsingInfo(
    int type,
    int code)
    : malicious_type(type), hw_code(code) {}

NetworkServiceProxyDelegate::PageSafeBrowsingInfo::~PageSafeBrowsingInfo() {}

void NetworkServiceProxyDelegate::OnTunnelConnectResult(
    const net::ProxyChain& proxy_chain,
    const std::string& host,
    const net::HttpResponseHeaders& response_headers,
    const net::HttpRequestHeaders& request_headers) {
  if (!IsInProxyConfig(proxy_chain)) {
    return;
  }

  int response_code = response_headers.response_code();
  std::string token;
  if (response_code == net::FallbackProxyResponseCode::TOKEN_AUTH_FAILED) {
    std::optional<std::string> token_result =
        request_headers.GetHeader(net::kFallbackProxyTunnelHeaderKey);
    if (token_result.has_value()) {
      token = token_result.value();
    }
  }
  if (observer_) {
    // Copy the response headers since mojo expects a ref counted object.
    observer_->OnTunnelHeadersReceivedWithToken(
        proxy_chain,
        base::MakeRefCounted<net::HttpResponseHeaders>(
            response_headers.raw_headers()),
        token);
  }

  if (response_code == net::FallbackProxyResponseCode::DEST_SERVER_TIME_OUT ||
      response_code == net::FallbackProxyResponseCode::DEST_SERVER_DISCONNECT ||
      response_code ==
          net::FallbackProxyResponseCode::DEST_HOST_RESOLVED_FAILED ||
      response_code ==
          net::FallbackProxyResponseCode::DEST_PORT_DENY_BY_PROXY) {
    ProcessFailedResult(host);
  } else if (response_code == net::HTTP_OK) {
    ProcessSuccessResult(host);
  }
}

bool NetworkServiceProxyDelegate::IsFallbackProxyServer(
    const net::ProxyChain& proxy_chain) {
  if (fallback_proxy_status_ != net::FallbackProxyStatus::NORMAL) {
    return false;
  }

  if (IsInProxyConfig(proxy_chain)) {
    return true;
  }
  return false;
}

void NetworkServiceProxyDelegate::AddSuccessMainFrameHosts(
    const std::string& host) {
  if (main_frame_using_proxy_success_list_.find(host) ==
      main_frame_using_proxy_success_list_.end()) {
    main_frame_using_proxy_success_list_.insert(host);
  }
}

void NetworkServiceProxyDelegate::ProcessFailedResult(const std::string& host) {
  if (main_frame_using_proxy_success_list_.find(host) !=
      main_frame_using_proxy_success_list_.end()) {
    main_frame_using_proxy_success_list_.erase(host);
  }

  auto it = using_proxy_failed_hosts_map_.find(host);
  if (it == using_proxy_failed_hosts_map_.end()) {
    using_proxy_failed_hosts_map_[host] = 1;
  } else {
    it->second++;
  }
}

void NetworkServiceProxyDelegate::ProcessSuccessResult(
    const std::string& host) {
  if (using_proxy_failed_hosts_map_.find(host) !=
      using_proxy_failed_hosts_map_.end()) {
    using_proxy_failed_hosts_map_.erase(host);
  }
  AddByPassRuleWithHost(host);
}

bool NetworkServiceProxyDelegate::IsFallbackProxyFailedHost(
    const std::string& host) {
  auto it = using_proxy_failed_hosts_map_.find(host);
  if (it == using_proxy_failed_hosts_map_.end() || it->second == 1) {
    return false;
  }
  return true;
}

bool NetworkServiceProxyDelegate::IsFallbackProxySuccessMainFrameHost(
    const std::string& host) {
  if (main_frame_using_proxy_success_list_.find(host) !=
      main_frame_using_proxy_success_list_.end()) {
    return true;
  }
  return false;
}

void NetworkServiceProxyDelegate::AddByPassRuleWithHost(
    const std::string& host) {
  proxy_config_->rules.bypass_rules.AddRuleFromString(host);
}

void NetworkServiceProxyDelegate::OnProxyConnectResult(
    const net::ProxyChain& proxy_chain,
    int error_code) {
  if (observer_) {
    observer_->OnProxyConnectResult(proxy_chain, error_code);
  }
}

void NetworkServiceProxyDelegate::SaveURLMaliciousTypeAndHwCode(
    const std::string& url,
    int type,
    int hw_code) {
  LOG(DEBUG) << "NetworkServiceProxyDelegate::SaveURLMaliciousTypeAndHwCode";
  PageSafeBrowsingInfo info(type, hw_code);
  url_sb_info_map_.Put(url, info);
}

bool NetworkServiceProxyDelegate::GetUrlMaliciousTypeAndHwCode(
    const std::vector<GURL>& url_chain,
    int* malicious_type,
    int* hw_code) {
  for (const GURL& url : url_chain) {
    auto it = url_sb_info_map_.Get(url.spec());
    if (it == url_sb_info_map_.end()) {
      continue;
    }
    *malicious_type = it->second.malicious_type;
    *hw_code = it->second.hw_code;
    return true;
  }

  return false;
}

bool NetworkServiceProxyDelegate::IsFallbackProxyMaliciousType(
    int malicious_type) {
  if (safe_browsing_malicious_type_list_.find(malicious_type) !=
      safe_browsing_malicious_type_list_.end()) {
    return true;
  }

  return false;
}

bool NetworkServiceProxyDelegate::IsFallbackProxyHwCode(int hw_code) {
  if (safe_browsing_hw_code_list_.find(hw_code) !=
      safe_browsing_hw_code_list_.end()) {
    return true;
  }

  return false;
}

void NetworkServiceProxyDelegate::SaveUsingFallbackProxyErrorCodes(
    const std::vector<int>& error_codes) {
  using_proxy_error_codes_list_ =
      std::set<int>(error_codes.begin(), error_codes.end());
}

void NetworkServiceProxyDelegate::SaveSafeBrowsingHwCodes(
    const std::vector<int>& hw_codes) {
  safe_browsing_hw_code_list_ = std::set<int>(hw_codes.begin(), hw_codes.end());
}

void NetworkServiceProxyDelegate::SaveSafeBrowsingMaliciousTypes(
    const std::vector<int>& types) {
  safe_browsing_malicious_type_list_ =
      std::set<int>(types.begin(), types.end());
}

void NetworkServiceProxyDelegate::OnFallbackProxyInfoConfigUpdated(
    mojom::FallbackProxyInfoConfigPtr fallback_proxy_info_config) {
  if (!fallback_proxy_info_config) {
    return;
  }

  SaveUsingFallbackProxyErrorCodes(
      fallback_proxy_info_config->using_proxy_error_codes);
  SaveSafeBrowsingHwCodes(fallback_proxy_info_config->safe_browsing_hw_codes);
  SaveSafeBrowsingMaliciousTypes(
      fallback_proxy_info_config->safe_browsing_malicious_types);
  proxy_tunnel_timeout_ = fallback_proxy_info_config->proxy_tunnel_timeout;
  proxy_connect_timeout_ = fallback_proxy_info_config->proxy_connect_timeout;
  malicious_url_check_wait_time_ =
      fallback_proxy_info_config->malicious_url_check_wait_time;

  LOG(DEBUG) << "NetworkServiceProxyDelegate::OnFallbackProxyInfoConfigUpdated"
             << ", error_codes.size "
             << fallback_proxy_info_config->using_proxy_error_codes.size()
             << ", safe_browsing_hw_codes.size "
             << fallback_proxy_info_config->safe_browsing_hw_codes.size()
             << ", safe_browsing_malicious_types.size "
             << fallback_proxy_info_config->safe_browsing_malicious_types.size()
             << ", malicious_url_check_wait_time "
             << malicious_url_check_wait_time_ << ", proxy_tunnel_timeout "
             << proxy_tunnel_timeout_ << ", proxy_connect_timeout "
             << proxy_connect_timeout_ << ", this " << this;
}

void NetworkServiceProxyDelegate::UpdateFallbackProxyAuthHeaders(
    const net::HttpRequestHeaders& tunnel_headers) {
  proxy_config_->connect_tunnel_headers = tunnel_headers;
}

void NetworkServiceProxyDelegate::UpdateFallbackProxyStatus(int status) {
  fallback_proxy_status_ = static_cast<net::FallbackProxyStatus>(status);
}

void NetworkServiceProxyDelegate::OnHostBlockListUpdated(
    const std::vector<std::string>& block_list) {
  LOG(DEBUG) << "OnHostBlockListUpdated, size " << block_list.size();
  fallback_proxy_block_list_ =
      std::set<std::string>(block_list.begin(), block_list.end());
}

bool NetworkServiceProxyDelegate::IsFallbackProxyRetryErrorCode(int net_error) {
  return using_proxy_error_codes_list_.find(net_error) !=
         using_proxy_error_codes_list_.end();
}

bool NetworkServiceProxyDelegate::IsFallbackProxyBlockHost(
    const std::string& host) {
  return fallback_proxy_block_list_.find(host) !=
         fallback_proxy_block_list_.end();
}

int NetworkServiceProxyDelegate::GetMaliciousUrlCheckWaitTime() {
  return malicious_url_check_wait_time_;
}

int NetworkServiceProxyDelegate::GetProxyConnectTimeout() {
  return proxy_connect_timeout_;
}

int NetworkServiceProxyDelegate::GetProxyTunnelTimeout() {
  return proxy_tunnel_timeout_;
}

net::FallbackProxyStatus NetworkServiceProxyDelegate::GetFallbackProxyStatus() {
  return fallback_proxy_status_;
}
#endif  // BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)

}
