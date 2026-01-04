// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_NETWORK_NETWORK_SERVICE_PROXY_DELEGATE_H_
#define SERVICES_NETWORK_NETWORK_SERVICE_PROXY_DELEGATE_H_

#include <deque>

#include "base/component_export.h"
#include "base/memory/raw_ptr.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "net/base/proxy_delegate.h"
#include "net/proxy_resolution/proxy_resolution_service.h"
#include "services/network/public/mojom/network_context.mojom.h"

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
#include "base/containers/lru_cache.h"
#endif

namespace net {
class HttpRequestHeaders;
class ProxyResolutionService;
}  // namespace net

namespace network {

// NetworkServiceProxyDelegate is used to support the custom proxy
// configuration, which can be set in
// NetworkContextParams.custom_proxy_config_client_receiver.
class COMPONENT_EXPORT(NETWORK_SERVICE) NetworkServiceProxyDelegate
    : public net::ProxyDelegate,
      public mojom::CustomProxyConfigClient {
 public:
  NetworkServiceProxyDelegate(
      mojom::CustomProxyConfigPtr initial_config,
      mojo::PendingReceiver<mojom::CustomProxyConfigClient>
          config_client_receiver,
      mojo::PendingRemote<mojom::CustomProxyConnectionObserver>
          observer_remote);

  NetworkServiceProxyDelegate(const NetworkServiceProxyDelegate&) = delete;
  NetworkServiceProxyDelegate& operator=(const NetworkServiceProxyDelegate&) =
      delete;

  ~NetworkServiceProxyDelegate() override;

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
  net::Error OnBeforeTunnelRequest(
      const net::ProxyChain& proxy_chain,
      size_t chain_index,
      net::HttpRequestHeaders* extra_headers) override;
  net::Error OnTunnelHeadersReceived(
      const net::ProxyChain& proxy_chain,
      size_t chain_index,
      const net::HttpResponseHeaders& response_headers) override;
  void SetProxyResolutionService(
      net::ProxyResolutionService* proxy_resolution_service) override;

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
  void OnTunnelConnectResult(
      const net::ProxyChain& proxy_chain,
      const std::string& host,
      const net::HttpResponseHeaders& response_headers,
      const net::HttpRequestHeaders& request_headers) override;
  void OnProxyConnectResult(const net::ProxyChain& proxy_chain,
                            int error_code) override;
  void AddSuccessMainFrameHosts(const std::string& host) override;
  bool IsFallbackProxyFailedHost(const std::string& host) override;
  bool IsFallbackProxySuccessMainFrameHost(const std::string& host) override;
  void AddByPassRuleWithHost(const std::string& host) override;
  bool GetUrlMaliciousTypeAndHwCode(const std::vector<GURL>& url_chain,
                                    int* malicious_type,
                                    int* hw_code) override;
  bool IsFallbackProxyMaliciousType(int malicious_type) override;
  bool IsFallbackProxyHwCode(int hw_code) override;
  bool IsFallbackProxyRetryErrorCode(int net_error) override;
  bool IsFallbackProxyBlockHost(const std::string& host) override;
  int GetMaliciousUrlCheckWaitTime() override;
  int GetProxyConnectTimeout() override;
  int GetProxyTunnelTimeout() override;
  net::FallbackProxyStatus GetFallbackProxyStatus() override;
  bool IsFallbackProxyServer(const net::ProxyChain& proxy_chain) override;
#endif

 private:
  friend class NetworkServiceProxyDelegateTest;

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
  struct PageSafeBrowsingInfo {
    PageSafeBrowsingInfo(int type, int code);
    ~PageSafeBrowsingInfo();

    int malicious_type;
    int hw_code;
  };
#endif

  // Checks whether `proxy_chain` is present in the current proxy config.
  bool IsInProxyConfig(const net::ProxyChain& proxy_chain) const;

  // Whether the HTTP |method| with current |proxy_info| is eligible to be
  // proxied.
  bool EligibleForProxy(const net::ProxyInfo& proxy_info,
                        const std::string& method) const;

  void OnObserverDisconnect();

  // mojom::CustomProxyConfigClient implementation:
  void OnCustomProxyConfigUpdated(
      mojom::CustomProxyConfigPtr proxy_config,
      OnCustomProxyConfigUpdatedCallback callback) override;

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
  void SaveURLMaliciousTypeAndHwCode(const std::string& url,
                                     int type,
                                     int hw_code) override;
  void OnFallbackProxyInfoConfigUpdated(
      mojom::FallbackProxyInfoConfigPtr fallback_proxy_info_config) override;
  void UpdateFallbackProxyAuthHeaders(
      const net::HttpRequestHeaders& tunnel_headers) override;
  void UpdateFallbackProxyStatus(int status) override;
  void OnHostBlockListUpdated(
      const std::vector<std::string>& block_list) override;

  void SaveFallbackPrxoyBypassHosts(
      const std::vector<std::string>& bypass_hosts);
  void SaveUsingFallbackProxyErrorCodes(const std::vector<int>& error_codes);
  void SaveSafeBrowsingHwCodes(const std::vector<int>& hw_codes);
  void SaveSafeBrowsingMaliciousTypes(const std::vector<int>& types);
  void ProcessFailedResult(const std::string& host);
  void ProcessSuccessResult(const std::string& host);
#endif

  mojom::CustomProxyConfigPtr proxy_config_;
  mojo::Receiver<mojom::CustomProxyConfigClient> receiver_;
  mojo::Remote<mojom::CustomProxyConnectionObserver> observer_;

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
  // fallback代理状态，值为NORAML时可用.
  net::FallbackProxyStatus fallback_proxy_status_ =
      net::FallbackProxyStatus::NONE;
  // 存放使用fallback代理失败的域名.
  std::map<std::string, int> using_proxy_failed_hosts_map_;
  // 存放UI通知的主资源的恶意网址拦截信息，只存放最近使用的1w条数据.
  base::LRUCache<std::string, PageSafeBrowsingInfo> url_sb_info_map_;
  // 云控配置的二级域名黑名单.
  std::set<std::string> fallback_proxy_block_list_;
  // 云控配置的允许使用代理的错误码.
  std::set<int> using_proxy_error_codes_list_;
  // 云控配置的允许使用代理的hw_code.
  std::set<int> safe_browsing_hw_code_list_;
  // 云控配置的允许使用代理的malicious_type.
  std::set<int> safe_browsing_malicious_type_list_;
  // 存放使用代理成功的主资源域名.
  std::set<std::string> main_frame_using_proxy_success_list_;
  // 云控配置的和代理服务器建立隧道的超时时间，单位为millisecond.
  int proxy_tunnel_timeout_ = 10000;
  // 云控配置的和代理服务器建立socket的超时时间，单位为millisecond.
  int proxy_connect_timeout_ = 5000;
  // 云控配置的等待恶意网址查询时间，单位为millisecond.
  int malicious_url_check_wait_time_ = 3000;
#endif

  raw_ptr<net::ProxyResolutionService> proxy_resolution_service_ = nullptr;
};

}  // namespace network

#endif  // SERVICES_NETWORK_NETWORK_SERVICE_PROXY_DELEGATE_H_
