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

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
#include "arkweb/chromium_ext/base/arkweb_report_statistics.h"
#include "arkweb/chromium_ext/base/ohos/nweb_engine_event_logger.h"
#include "arkweb/chromium_ext/base/ohos/nweb_engine_event_logger_code.h"
#include "base/json/json_writer.h"
#endif

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
#include "arkweb/chromium_ext/net/base/log_utils.h"
#endif

namespace net {

namespace {

#if BUILDFLAG(ARKWEB_EX_HTTP_DNS_FALLBACK)
const char* kSceneString[] = {"normal DNS", "ErrorRetry"};
const char* kDnsTransactionString[] = {"local DNS", "https DNS"};
const char* kSecureFallbackDnsEventType = "secure_dns_fallback_result";
const char* kScene = "scene";
const char* kHost = "host";
const char* kUdpDnsIpList = "udp_dns_ip_list";
const char* kSecureFallbackIpList = "ip_list";
const char* kLocalIp = "local_ip";
const char* kNetworkType = "network_type";
const char* kResult = "result";
const char* kDuration = "duration";
const char* kMccCode = "mcc";
const char* kMncCode = "mnc";
const char* kTimeStamp = "time_stamp";

std::string ToJsonStr(const std::vector<std::string>& content) {
  base::Value::List list;
  for (const auto& value : content) {
    list.Append(value);
  }
  auto json = base::WriteJson(list);
  return json.value_or("[]");
}

void IPListToString(const std::vector<IPEndPoint>& endpoints,
                    std::string& ipInfo,
                    bool anonymize) {
  ipInfo.append("[");
  for (size_t index = 0; index < endpoints.size(); index++) {
    if (anonymize) {
      ipInfo.append(net::LogUtils::AnonymizeIpAddress(endpoints[index]));
    } else {
      ipInfo.append(endpoints[index].address().ToString());
    }
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

void HostResolverManager::GetLocalAddress(IPEndPoint* address) {
  if (!dns_client_) {
    return;
  }
  dns_client_->GetLocalAddress(address);
}

void HostResolverManager::ReportSecureFallbackDnsResult(
    const std::optional<HostCache::Entry> insecure_results,
    const HostCache::Entry& secure_fallback_results,
    const std::string& host,
    const int index,
    const base::TimeDelta& duration) {
  std::string insecure_ip_info;
  std::string insecure_ip_info_mask;
  if (insecure_results && !insecure_results->ip_endpoints().empty()) {
    IPListToString(insecure_results->ip_endpoints(), insecure_ip_info, false);
    IPListToString(insecure_results->ip_endpoints(), insecure_ip_info_mask,
                   true);
  } else {
    insecure_ip_info.append("[]");
    insecure_ip_info_mask.append("[]");
  }

  std::string secure_ip_info;
  std::string secure_ip_info_mask;
  if (secure_fallback_results.error() == OK &&
      !secure_fallback_results.ip_endpoints().empty()) {
    IPListToString(secure_fallback_results.ip_endpoints(), secure_ip_info,
                   false);
    IPListToString(secure_fallback_results.ip_endpoints(), secure_ip_info_mask,
                   true);
  } else {
    secure_ip_info.append("[]");
    secure_ip_info_mask.append("[]");
  }

  std::vector<std::string> content;
  content.push_back(kScene);
  content.push_back(kSceneString[index]);
  content.push_back(kHost);
  content.push_back(host);
  content.push_back(kUdpDnsIpList);
  content.push_back(insecure_ip_info);
  content.push_back(kSecureFallbackIpList);
  content.push_back(secure_ip_info);
  IPEndPoint localIp;
  GetLocalAddress(&localIp);
  content.push_back(kLocalIp);
  content.push_back(localIp.address().ToString());
  content.push_back(kNetworkType);
  NetworkChangeNotifier::ConnectionType type =
      NetworkChangeNotifier::GetConnectionType();
  content.push_back(
      std::string(NetworkChangeNotifier::ConnectionTypeToString(type)));
  content.push_back(kResult);
  content.push_back(base::NumberToString(secure_fallback_results.error()));
  content.push_back(kDuration);
  content.push_back(base::NumberToString(duration.InMilliseconds()));
  auto time_stamp =
      static_cast<int64_t>(base::Time::Now().InSecondsFSinceUnixEpoch());
  content.push_back(kTimeStamp);
  content.push_back(base::NumberToString(time_stamp));
  std::vector<std::string> mcc_mnc(2, "");
  content.push_back(kMccCode);
  content.push_back(mcc_mnc[0]);
  content.push_back(kMncCode);
  content.push_back(mcc_mnc[1]);

  std::ostringstream ostr;
  ostr << "scene=" << kSceneString[index]
       << ", udp_dns_ip_list=" << insecure_ip_info
       << ", ip_list=" << secure_ip_info << ", network_type="
       << std::string(NetworkChangeNotifier::ConnectionTypeToString(type))
       << ", result=" << secure_fallback_results.error()
       << ", duration=" << duration.InMilliseconds()
       << ", time_stamp=" << time_stamp;

  // 经分打点数据上报
  base::ohos::OperationStatistics::Statistics(
      base::ohos::REGION_CHINA, base::ohos::PLATFORM_OPERATION_ANALYSIS,
      base::ohos::OperationStatistics::GROUP_BECE, kSecureFallbackDnsEventType,
      base::ohos::OperationStatistics::DEFAULT_DATA_VERSION, ToJsonStr(content),
      true, false, true, base::ohos::REPORT_DAILY);
  base::ohos::OperationStatistics::Statistics(
      base::ohos::REGION_OVERSEA, base::ohos::PLATFORM_BUSINESS_INTELLIGENCE,
      base::ohos::OperationStatistics::GROUP_BECE, kSecureFallbackDnsEventType,
      base::ohos::OperationStatistics::DEFAULT_DATA_VERSION, ToJsonStr(content),
      true, false, true, base::ohos::REPORT_DAILY);
  // 运维打点数据上报
  base::ohos::ReportEngineEvent(base::ohos::kModuleContentBrowser, host,
                                base::ohos::kSecureDnsFallbackResult,
                                ostr.str());

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO, kNetwork)
      << "HttpDNSResult scene:" << kSceneString[index]
      << " networkType:" << NetworkChangeNotifier::ConnectionTypeToString(type)
      << " udpDnsIpList:" << insecure_ip_info_mask
      << " ipList:" << secure_ip_info_mask
      << " result:" << secure_fallback_results.error()
      << " duration:" << duration.InMilliseconds()
      << " timeStamp:" << time_stamp
      << " host:" << url::LogUtils::ConvertUrlWithMask(host);
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
