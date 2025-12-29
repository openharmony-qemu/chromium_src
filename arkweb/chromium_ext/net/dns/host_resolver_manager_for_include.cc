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

#include "arkweb/chromium_ext/url/ohos/log_utils.h"

namespace net {

namespace {

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
const char* kSceneString[] = {"normal DNS", "ErrorRetry"};
const char* kDnsTransactionString[] = {"local DNS", "https DNS"};

void IPListToString(const std::vector<IPEndPoint>& endpoints,
                    std::string& ipInfo) {
  ipInfo.append("[");
  for (size_t index = 0; index < endpoints.size(); index++) {
    ipInfo.append(endpoints[index].address().ToString());
    if (index < endpoints.size() - 1) {
      ipInfo.append(", ");
    }
  }
  ipInfo.append("]");
}
#endif

}  // namespace

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
class HostResolverManager::WarmUpHttpDnsFallbackImpl
    : public ResolveContext::DohStatusObserver {
 public:
  WarmUpHttpDnsFallbackImpl(const std::string& server_template,
                            base::WeakPtr<ResolveContext> context,
                            base::WeakPtr<HostResolverManager> resolver)
      : doh_fallback_server_template_(server_template),
        context_(std::move(context)),
        resolver_(std::move(resolver)) {}

  WarmUpHttpDnsFallbackImpl(const WarmUpHttpDnsFallbackImpl&) = delete;
  WarmUpHttpDnsFallbackImpl& operator=(const WarmUpHttpDnsFallbackImpl&) =
      delete;

  ~WarmUpHttpDnsFallbackImpl() override {
    if (context_) {
      context_->UnregisterDohStatusObserver(this);
    }
  }

  void Start() {
    DCHECK(resolver_);
    DCHECK(context_);
    if (context_) {
      context_->RegisterDohStatusObserver(this);
    }
  }

  // ResolveContext::DohStatusObserver
  void OnSessionChanged() override { request_.reset(); }

  void OnDohServerUnavailable(bool network_change) override {
    base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
        FROM_HERE,
        base::BindOnce(&WarmUpHttpDnsFallbackImpl::PreDnsOfDohFallbackServer,
                       weak_ptr_factory_.GetWeakPtr()));
  }

 private:
  void PreDnsOfDohFallbackServer() {
    DCHECK(resolver_);
    DCHECK(context_);
    if (request_) {
      return;
    }

    GURL url(doh_fallback_server_template_);
    if (!url.is_valid()) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
      LOG_FEEDBACK(INFO, kNetwork)
          << "PreHttpDNSFailed message:dohFallbackServerTemplateInValid";
#endif
      return;
    }
    HostPortPair destination = HostPortPair::FromURL(url);
    HostResolver::ResolveHostParameters resolve_params;
    resolve_params.secure_dns_policy = SecureDnsPolicy::kBootstrap;
    request_ = resolver_->CreateRequest(destination, NetworkAnonymizationKey(),
                                        NetLogWithSource(), resolve_params,
                                        context_.get());
    auto result = request_->Start(base::BindOnce(
        &WarmUpHttpDnsFallbackImpl::PreDnsOfDohFallbackServerComplete,
        weak_ptr_factory_.GetWeakPtr()));
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO, kNetwork)
        << "HttpDNSWarmUp result:" << result
        << " url:" << url::LogUtils::ConvertUrlWithMask(url.spec());
#endif
    if (result != ERR_IO_PENDING) {
      request_.reset();
    }
  }

  void PreDnsOfDohFallbackServerComplete(int result) {
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
    LOG_FEEDBACK(INFO, kNetwork) << "PreHttpDNS result " << result;
#endif
    request_.reset();
  }

  std::string doh_fallback_server_template_;
  base::WeakPtr<ResolveContext> context_;
  base::WeakPtr<HostResolverManager> resolver_;
  std::unique_ptr<HostResolver::ResolveHostRequest> request_;

  base::WeakPtrFactory<WarmUpHttpDnsFallbackImpl> weak_ptr_factory_{this};
};
#endif  // BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)

//-----------------------------------------------------------------------------

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
bool HostResolverManager::CanUseSecureDnsFallback(
    ResolveContext* context) const {
  if (!dns_client_.get()) {
    return false;
  }

  return dns_client_->CanUseSecureDnsFallbackTransactions(context);
}

void HostResolverManager::WarmUpHttpsDnsFallback(ResolveContext* context) {
  auto warmup_httpdns_fallback = std::make_unique<WarmUpHttpDnsFallbackImpl>(
      doh_fallback_server_template_, context->GetWeakPtr(),
      weak_ptr_factory_.GetWeakPtr());
  warmup_httpdns_fallback->Start();
  warmup_httpdns_fallback_list_.push_back(std::move(warmup_httpdns_fallback));
}

void HostResolverManager::SetHttpsDnsFallbackData(
    bool enabled,
    const std::string& server_template) {
  https_dns_fallback_enabled_ = enabled;
  doh_fallback_server_template_ = server_template;
  for (auto& context : registered_contexts_) {
    context.SetHttpsDnsFallbackEnabled(enabled);
    warmup_httpdns_fallback_list_.clear();
    if (enabled) {
      WarmUpHttpsDnsFallback(&context);
    }
  }
}

void HostResolverManager::SetSuspectIpListAndSourceHostList(
    const std::vector<std::string>& ip_list,
    const std::vector<std::string>& host_list) {
  // Todo(huawei)
}

void HostResolverManager::ReportSecureFallbackDnsResult(
    const std::optional<HostCache::Entry> insecure_results,
    const HostCache::Entry& secure_fallback_results,
    const std::string& host,
    const int index,
    const base::TimeDelta& duration) {
  std::string insecure_ip_info;
  if (insecure_results && insecure_results->ip_endpoints().empty()) {
    IPListToString(insecure_results->ip_endpoints(), insecure_ip_info);
  } else {
    insecure_ip_info.append("[]");
  }

  std::string secure_ip_info;
  if (secure_fallback_results.error() == OK &&
      secure_fallback_results.ip_endpoints().empty()) {
    IPListToString(secure_fallback_results.ip_endpoints(), secure_ip_info);
  } else {
    secure_ip_info.append("[]");
  }

  std::ostringstream ostr;
  ostr << "scene=" << kSceneString[index]
       << ", udp_dns_ip_list=" << insecure_ip_info
       << ", ip_list=" << secure_ip_info
       << ", result=" << secure_fallback_results.error()
       << ", duration=" << duration.InMilliseconds();

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO, kNetwork) << "HttpDNSResult " << ostr.str();
#endif
}

void HostResolverManager::ReportDnsTransactionResult(int index,
                                                     const std::string& host,
                                                     int result_for_ipv4,
                                                     int result_for_ipv6) {
  std::ostringstream ostr;
  ostr << "dns_type=" << kDnsTransactionString[index]
       << ", v4result=" << result_for_ipv4 << ", v6result=" << result_for_ipv6;

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO, kNetwork) << "HttpDNSTransaction " << ostr.str();
#endif
}
#endif

}  // namespace net
