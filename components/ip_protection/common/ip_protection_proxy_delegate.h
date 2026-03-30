// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_IP_PROTECTION_COMMON_IP_PROTECTION_PROXY_DELEGATE_H_
#define COMPONENTS_IP_PROTECTION_COMMON_IP_PROTECTION_PROXY_DELEGATE_H_

#include <cstddef>
#include <string>

#include "base/gtest_prod_util.h"
#include "base/memory/raw_ref.h"
#include "base/memory/weak_ptr.h"
#include "base/types/expected.h"
#include "net/base/completion_once_callback.h"
#include "net/base/net_errors.h"
#include "net/base/network_anonymization_key.h"
#include "net/base/proxy_chain.h"
#include "net/base/proxy_delegate.h"
#include "net/proxy_resolution/proxy_retry_info.h"

class GURL;

namespace net {

class HttpRequestHeaders;
class ProxyResolutionService;
class ProxyList;
struct ProxyRetryInfo;

}  // namespace net

namespace ip_protection {

class IpProtectionCore;
enum class ProxyResolutionResult;

// IpProtectionProxyDelegate is used to support IP protection, by injecting
// proxies for requests where IP should be protected.
class IpProtectionProxyDelegate : public net::ProxyDelegate {
 public:
  // The `ip_protection_core` must be non-null.
  explicit IpProtectionProxyDelegate(IpProtectionCore* ip_protection_core);

  IpProtectionProxyDelegate(const IpProtectionProxyDelegate&) = delete;
  IpProtectionProxyDelegate& operator=(const IpProtectionProxyDelegate&) =
      delete;

  ~IpProtectionProxyDelegate() override;

  // net::ProxyDelegate implementation:
  void OnResolveProxy(
      const GURL& url,
      const net::NetworkAnonymizationKey& network_anonymization_key,
      const std::string& method,
      const net::ProxyRetryInfoMap& proxy_retry_info,
      net::ProxyInfo* result) override;
  void OnSuccessfulRequestAfterFailures(
      const net::ProxyRetryInfoMap& proxy_retry_info) override;
  void OnFallback(const net::ProxyChain& bad_chain, int net_error) override;
  base::expected<net::HttpRequestHeaders, net::Error> OnBeforeTunnelRequest(
      const net::ProxyChain& proxy_chain,
      size_t chain_index,
      OnBeforeTunnelRequestCallback callback) override;
  net::Error OnTunnelHeadersReceived(
      const net::ProxyChain& proxy_chain,
      size_t chain_index,
      const net::HttpResponseHeaders& response_headers,
      net::CompletionOnceCallback callback) override;
  void SetProxyResolutionService(
      net::ProxyResolutionService* proxy_resolution_service) override;
#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
  void OnTunnelConnectResult(
      const net::ProxyChain& proxy_chain,
      const std::string& host,
      const net::HttpResponseHeaders& response_headers,
      const net::HttpRequestHeaders& request_headers) override{};
  void OnProxyConnectResult(const net::ProxyChain& proxy_chain,
                            int error_code) override{};
  void AddSuccessMainFrameHosts(const std::string& host) override{};
  bool IsFallbackProxyFailedHost(const std::string& host) override {
    return false;
  };
  bool IsFallbackProxySuccessMainFrameHost(const std::string& host) override {
    return false;
  };
  void AddByPassRuleWithHost(const std::string& host) override{};
  bool GetUrlMaliciousTypeAndHwCode(const std::vector<GURL>& url_chain,
                                    int* malicious_type,
                                    int* hw_code) override {
    return false;
  };
  bool IsFallbackProxyMaliciousType(int malicious_type) override {
    return false;
  };
  bool IsFallbackProxyHwCode(int hw_code) override { return false; };
  bool IsFallbackProxyRetryErrorCode(int net_error) override { return false; };
  bool IsFallbackProxyBlockHost(const std::string& host) override {
    return false;
  };
  int GetMaliciousUrlCheckWaitTime() override { return 0; };
  int GetProxyConnectTimeout() override { return 0; };
  int GetProxyTunnelTimeout() override { return 0; };
  net::FallbackProxyStatus GetFallbackProxyStatus() override {
    return net::FallbackProxyStatus::NONE;
  };
  bool IsFallbackProxyServer(const net::ProxyChain& proxy_chain) override {
    return false;
  };
#endif

  bool AliasRequiresProxyOverride(
      const std::string scheme,
      const std::vector<std::string>& dns_aliases,
      const net::NetworkAnonymizationKey& network_anonymization_key) override;
  void OnStreamCreationAttempted(const net::ProxyChain& proxy_chain,
                                 base::TimeDelta duration,
                                 base::optional_ref<int> net_error) override;

 private:
  friend class IpProtectionProxyDelegateTest;
  FRIEND_TEST_ALL_PREFIXES(IpProtectionProxyDelegateTest, MergeProxyRules);

  // Note: the order of the return values must match the order of the enum
  // values in ProxyResolutionResult so that existing metric data is not
  // affected when we add a new enum value.
  // TODO(crbug.com/403156545): Refactor this so that we can make calls more
  // efficiently.
  ProxyResolutionResult ClassifyRequest(
      const GURL& url,
      const net::NetworkAnonymizationKey& network_anonymization_key,
      net::ProxyInfo* result);

  // Returns the equivalent of replacing all DIRECT proxies in
  // `existing_proxy_list` with the proxies in `custom_proxy_list`.
  static net::ProxyList MergeProxyRules(
      const net::ProxyList& existing_proxy_list,
      const net::ProxyList& custom_proxy_list);

  const raw_ref<IpProtectionCore> ip_protection_core_;

  base::WeakPtrFactory<IpProtectionProxyDelegate> weak_factory_{this};
};

}  // namespace ip_protection

#endif  // COMPONENTS_IP_PROTECTION_COMMON_IP_PROTECTION_PROXY_DELEGATE_H_
