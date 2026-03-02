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
#include "arkweb/chromium_ext/services/network/url_loader_utils.h"

#include "arkweb/chromium_ext/net/base/fallback_proxy_constants.h"
#include "arkweb/chromium_ext/services/network/url_loader_ext.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/trace_event/trace_event.h"
#include "net/base/ip_endpoint.h"
#include "net/base/load_timing_info.h"
#include "net/base/proxy_delegate.h"
#include "net/dns/host_resolver.h"
#include "net/url_request/url_request_context.h"
#include "services/network/public/cpp/header_util.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "services/network/sec_header_helpers.h"
#include "services/network/shared_dictionary/shared_dictionary_manager.h"
#include "services/network/throttling/scoped_throttling_token.h"

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
#include "arkweb/chromium_ext/services/network/prp_preload/include/page_res_parallel_preload_mgr.h"
#include "base/functional/callback.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
#include "arkweb/chromium_ext/net/base/navigation_info.h"
#include "arkweb/chromium_ext/net/url_request/url_request_context_ext.h"
#endif

namespace {

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
// Helper function to check if fallback proxy is enabled from any proxy context.
// This is decoupled from URLRequest and only depends on the context.
bool IsFallbackProxyEnabled(const net::URLRequestContext* context,
                            const net::URLRequest* request) {
  if (!context || !request) {
    return false;
  }

  auto* proxy_delegate = context->proxy_delegate();
  if (!proxy_delegate) {
    LOG_FEEDBACK(ERROR, kNetwork)
        << "FailGetFallbackProxyEnabled message:proxyDelegateIsNull";
    return false;
  }

  return proxy_delegate->GetFallbackProxyStatus() ==
         net::FallbackProxyStatus::NORMAL;
}
#endif

}  // namespace

namespace network {

URLLoaderUtils::URLLoaderUtils(URLLoader* url_loader,
                               const bool corb_detachable)
    : corb_detachable_(corb_detachable)
{
  this->url_loader_ = url_loader;
}

#if BUILDFLAG(ARKWEB_PERFORMANCE_NETWORK_TRACE)
std::string URLLoaderUtils::InMilliseconds(base::TimeTicks time)
{
  return std::to_string(time.since_origin().InMilliseconds());
}

void URLLoaderUtils::PrintNetworkTimingInfo()
{
  using namespace std;
  net::LoadTimingInfo metrics;
  url_loader_->url_request_->GetLoadTimingInfo(&metrics);
  TRACE_EVENT2("net", "URLLoader::PrintNetworkTimingInfo", "info",
               "socket_reused: " + to_string(metrics.socket_reused) +
               ";dns_start: " + InMilliseconds(metrics.connect_timing.domain_lookup_start) +
               ";dns_end: " + InMilliseconds(metrics.connect_timing.domain_lookup_end) +
               ";connect_start: " + InMilliseconds(metrics.connect_timing.connect_start) +
               ";connect_end: " + InMilliseconds(metrics.connect_timing.connect_end) +
               ";ssl_start: " + InMilliseconds(metrics.connect_timing.ssl_start) +
               ";ssl_end: " + InMilliseconds(metrics.connect_timing.ssl_end) +
               ";request_start: " + InMilliseconds(metrics.request_start) +
               ";send_start: " + InMilliseconds(metrics.send_start) +
               ";receive_headers_start: " + InMilliseconds(metrics.receive_headers_start) +
               ";request_end: " + InMilliseconds(base::TimeTicks::Now()) +
               ";decoded_size: " + to_string(url_loader_->total_written_bytes_) +
               ";encoded_size: " + to_string(url_loader_->url_request_->GetRawBodyBytes()) +
               ";idempotency: " + to_string(url_loader_->url_request_->GetIdempotency()),
               "id", url_loader_->request_id_);
}

void URLLoaderUtils::PrintNetworkCacheInfo()
{
  using namespace std;
  optional<base::TimeDelta> age = url_loader_->response_->headers->GetAgeValue();
  optional<base::Time> last_modified =
    url_loader_->response_->headers->GetLastModifiedValue();
  optional<base::Time> expires = url_loader_->response_->headers->GetExpiresValue();
  optional<string> cache_control =
      url_loader_->response_->headers->GetNormalizedHeader("Cache-Control");
  optional<string> etag = url_loader_->response_->headers->GetNormalizedHeader("ETag");
  TRACE_EVENT2(
      "net", "URLLoader::PrintNetworkCacheInfo", "info",
      "age: " + (age ? to_string(age.value().InMilliseconds()) : "unset") +
      ";last_modified: " + (last_modified ? base::TimeUtils::ToUTCTimeString(last_modified.value()) : "unset") +
      ";expires: " + (expires ? base::TimeUtils::ToUTCTimeString(expires.value()) : "unset") +
      ";cache_control: " + (cache_control ? cache_control.value() : "unset") +
      ";etag: " + (etag ? etag.value() : "unset") + ";is_zero: " +
      to_string(url_loader_->response_->headers->GetFreshnessLifetimes(url_loader_->response_->response_time)
                    .freshness.is_zero()) +
      ";was_fetched_via_cache: " + to_string(url_loader_->response_->was_fetched_via_cache) +
      ";load_flags: " + to_string(url_loader_->url_request_->load_flags()),
      "id", url_loader_->request_id_);
}

void URLLoaderUtils::PrintNetworkInfo()
{
  if (url_loader_->url_request_) {
#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
    if (url_loader_->url_request_->preload_info()) {
      url_loader_->url_request_->preload_info()->set_request_end_time(
          base::Time::Now().ToInternalValue());
    }
#endif
    if (url_loader_->url_request_->response_headers()) {
      const net::HttpResponseHeaders* response_headers =
      url_loader_->raw_response_headers_ && url_loader_->enable_reporting_raw_headers_
          ? url_loader_->raw_response_headers_.get()
          : url_loader_->url_request_->response_headers();
      TRACE_EVENT2("net", "URLLoader::NotifyCompleted", "response_code",
                   response_headers->response_code(), "id", url_loader_->request_id_);
    }
    PrintNetworkTimingInfo();
  }
  if (url_loader_->response_ && url_loader_->response_->headers) {
    PrintNetworkCacheInfo();
  }
}

std::string URLLoaderUtils::GetRequestHttpVersion()
{
  if (url_loader_->url_request_->was_fetched_via_spdy()) {
    return "http/2.0";
  } else {
    net::HttpVersion request_http_version =
      url_loader_->url_request_->response_headers()->GetHttpVersion();
    if (request_http_version == net::HttpVersion(0, 9)) {
      return "http/0.9";
    } else if (request_http_version == net::HttpVersion(1, 0)) {
      return "http/1.0";
    } else if (request_http_version == net::HttpVersion(1, 1)) {
      return "http/1.1";
    }
    return "http/2.0";
  }
}
#endif

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
void URLLoaderUtils::UpdateResRequestInfo(
    const std::string& key,
    const std::shared_ptr<ohos_prp_preload::PRRequestInfo>& info)
{
  if ((info->preload_flag() != ohos_prp_preload::PRPP_FLAGS_NONE) ||
      (url_loader_->resource_type_ == RESOURCE_TYPE_IMAGE)) {
    already_update_info_ = true;
    ohos_prp_preload::PRParallelPreloadMgr::GetInstance().UpdateResRequestInfo(
        key, info);
  }
}

void URLLoaderUtils::ConfigureRequestForRollback(
    const GURL& url,
    std::string_view method,
    const net::SiteForCookies& site_for_cookies,
    bool force_ignore_site_for_cookies,
    const std::vector<GURL>& url_chain,
    const GURL& referrer,
    net::ReferrerPolicy referrer_policy,
    bool upgrade_if_insecure,
    bool is_ad_tagged,
    std::optional<net::IsolationInfo> isolation_info,
    bool force_main_frame_for_same_site_cookies,
    net::SecureDnsPolicy secure_dns_policy,
    net::HttpRequestHeaders extra_request_headers,
    const std::optional<std::vector<net::SourceStream::SourceType>>&
        accepted_stream_types,
    const std::optional<url::Origin>& initiator,
    net::RedirectInfo::FirstPartyURLPolicy first_party_url_policy,
    int request_load_flags,
    bool priority_incremental,
    net::CookieSettingOverrides cookie_setting_overrides,
    std::optional<net::SharedDictionaryGetter> shared_dictionary_getter)
{
  url_request_rollback_->set_method(method);
  url_request_rollback_->set_site_for_cookies(site_for_cookies);
  url_request_rollback_->set_force_ignore_site_for_cookies(
      force_ignore_site_for_cookies);
  if (!url_chain.empty()) {
    url_request_rollback_->SetURLChain(url_chain);
  }
  url_request_rollback_->SetReferrer(referrer.GetAsReferrer().spec());
  url_request_rollback_->set_referrer_policy(referrer_policy);
  url_request_rollback_->set_upgrade_if_insecure(upgrade_if_insecure);
  url_request_rollback_->set_ad_tagged(is_ad_tagged);

  if (isolation_info) {
    url_request_rollback_->set_isolation_info(std::move(isolation_info).value());
  }

  url_request_rollback_->set_force_main_frame_for_same_site_cookies(
      force_main_frame_for_same_site_cookies);

  url_request_rollback_->SetSecureDnsPolicy(secure_dns_policy);

  url_request_rollback_->SetExtraRequestHeaders(std::move(extra_request_headers));

  url_request_rollback_->SetUserData(URLLoader::kUserDataKey,
      std::make_unique<URLLoader::UnownedPointer>(url_loader_));
  url_request_rollback_->set_accepted_stream_types(accepted_stream_types);

  url_request_rollback_->set_initiator(initiator);

  // Note: There are some ordering dependencies here. `SetRequestCredentials`
  // depends on `SetLoadFlags`; `CalculateStorageAccessStatus` depends on
  // `cookie_setting_overrides` and `SetRequestCredentials`.
  // `SetFetchMetadataHeaders` will depend on
  // `url_request_rollback_->storage_access_status()`, once https://crbug.com/366284840
  // is fixed.
  url_request_rollback_->cookie_setting_overrides() = cookie_setting_overrides;
  url_request_rollback_->SetLoadFlags(request_load_flags);
  url_loader_->SetRequestCredentials(url);
  url_request_rollback_->set_storage_access_status(
      url_request_rollback_->CalculateStorageAccessStatus());

  SetFetchMetadataHeaders(url_request_rollback_.get(), url_loader_->request_mode_,
                          url_loader_->has_user_activation_, url_loader_->request_destination_, nullptr,
                          *url_loader_->factory_params_, *url_loader_->origin_access_list_);

  url_request_rollback_->set_first_party_url_policy(first_party_url_policy);

  url_request_rollback_->SetPriorityIncremental(priority_incremental);

  url_request_rollback_->SetRequestHeadersCallback(base::BindRepeating(
      &URLLoader::SetRawRequestHeadersAndNotify, base::Unretained(url_loader_)));
  if (url_loader_->shared_dictionary_checker_) {
    url_request_rollback_->SetIsSharedDictionaryReadAllowedCallback(base::BindRepeating(
        &URLLoader::IsSharedDictionaryReadAllowed, base::Unretained(url_loader_)));
  }

  if (url_loader_->devtools_request_id()) {
    url_request_rollback_->SetResponseHeadersCallback(base::BindRepeating(
        &URLLoader::SetRawResponseHeaders, base::Unretained(url_loader_)));
  }

  url_request_rollback_->SetEarlyResponseHeadersCallback(base::BindRepeating(
      &URLLoader::NotifyEarlyResponse, base::Unretained(url_loader_)));

  if (shared_dictionary_getter) {
    url_request_rollback_->SetSharedDictionaryGetter(
        std::move(shared_dictionary_getter).value());
  }
}

void URLLoaderUtils::InitUrlRequestForRollback(
    URLLoaderContext& context,
    const ResourceRequest& request,
    const net::NetworkTrafficAnnotationTag& traffic_annotation,
    SharedDictionaryManager* shared_dictionary_manager,
    const std::string& org_main_url,
    std::shared_ptr<ohos_prp_preload::PRRequestInfo> preload_info)
{
  url_request_rollback_ = url_loader_->url_request_context_->CreateRequestForPrpp(
      request.url, request.priority, url_loader_, traffic_annotation,
      false, /*is_for_websockets=*/
      request.net_log_create_info);

  // |cors_exempt_headers| must be merged here to avoid breaking CORS checks.
  // They are non-empty when the values are given by the UA code, therefore
  // they should be ignored by CORS checks.
  net::HttpRequestHeaders merged_headers = request.headers;
  merged_headers.MergeFrom(ComputeAttributionReportingHeaders(request));
  merged_headers.MergeFrom(request.cors_exempt_headers);

  // This should be ensured by the CorsURLLoaderFactory(), which is called
  // before URLLoaders are created.
  DCHECK(AreRequestHeadersSafe(merged_headers));

  // When a service worker forwards a navigation request it uses the
  // service worker's IsolationInfo.  This causes the cookie code to fail
  // to send SameSite=Lax cookies for main-frame navigations passed through
  // a service worker.  To fix this we check to see if the original destination
  // of the request was a main frame document and then set a flag indicating
  // SameSite cookies should treat it as a main frame navigation.
  const bool force_main_frame_for_same_site_cookies =
      request.mode == mojom::RequestMode::kNavigate &&
      request.destination == mojom::RequestDestination::kEmpty &&
      request.original_destination == mojom::RequestDestination::kDocument;

  const net::SecureDnsPolicy secure_dns_policy =
      url_loader_->factory_params_->disable_secure_dns ||
              (request.trusted_params &&
               request.trusted_params->disable_secure_dns)
          ? net::SecureDnsPolicy::kDisable
          : net::SecureDnsPolicy::kAllow;

  const net::RedirectInfo::FirstPartyURLPolicy first_party_url_policy =
      request.update_first_party_url_on_redirect
          ? net::RedirectInfo::FirstPartyURLPolicy::UPDATE_URL_ON_REDIRECT
          : net::RedirectInfo::FirstPartyURLPolicy::NEVER_CHANGE_URL;

  if (!request.navigation_redirect_chain.empty()) {
    DCHECK_EQ(request.mode, mojom::RequestMode::kNavigate);
  }

  ConfigureRequestForRollback(
      request.url, request.method, request.site_for_cookies,
      /*force_ignore_site_for_cookies=*/
      url_loader_->ShouldForceIgnoreSiteForCookies(request),
      request.navigation_redirect_chain, request.referrer,
      request.referrer_policy,
      request.upgrade_if_insecure, /*upgrade_if_insecure=*/
      request.is_ad_tagged, /*is_ad_tagged=*/
      /*isolation_info=*/
      url_loader_->GetIsolationInfo(url_loader_->factory_params_->isolation_info,
          url_loader_->factory_params_->automatically_assign_isolation_info,
          request),
      /*force_main_frame_for_same_site_cookies=*/
      force_main_frame_for_same_site_cookies, secure_dns_policy,
      std::move(merged_headers), request.devtools_accepted_stream_types,
      request.request_initiator, first_party_url_policy,
      request.load_flags, /*request_load_flags=*/
      request.priority_incremental, /*priority_incremental=*/
      /*cookie_setting_overrides=*/
      url_loader_->CalculateCookieSettingOverrides(url_loader_->factory_params_->cookie_setting_overrides,
          request),
      /*shared_dictionary_getter=*/
      shared_dictionary_manager
          ? std::make_optional(
              shared_dictionary_manager->MaybeCreateSharedDictionaryGetter(
                  request.load_flags, url_loader_->request_destination_))
          : std::nullopt);

  if (context.ShouldRequireIsolationInfo()) {
    DCHECK(!url_request_rollback_->isolation_info().IsEmpty());
  }

  url_loader_->throttling_token_ = network::ScopedThrottlingToken::MaybeCreate(
      url_request_rollback_->net_log().source().id, request.throttling_profile_id);

  if (url_loader_->keepalive_ && url_loader_->keepalive_statistics_recorder_) {
    url_loader_->keepalive_statistics_recorder_->OnLoadStarted(
        *url_loader_->factory_params_->top_frame_id, url_loader_->keepalive_request_size_);
  }

  if (request.net_log_reference_info) {
    // Log source object that created the request, if available.
    url_request_rollback_->net_log().AddEventReferencingSource(
        net::NetLogEventType::CREATED_BY,
        request.net_log_reference_info.value());
  }

  SetUrlRequestForPRPP(request, url_request_rollback_, org_main_url, preload_info);
}

void URLLoaderUtils::SetUrlRequestForPRPP(
    const ResourceRequest& request,
    const std::shared_ptr<net::URLRequest>& url_request,
    const std::string& org_main_url,
    std::shared_ptr<ohos_prp_preload::PRRequestInfo>& preload_info)
{
  if (ohos_prp_preload::PRParallelPreloadMgr::GetInstance()
      .GetPRParallelPreloadMode() ==
      ohos_prp_preload::PRPPreloadMode::NONE) {
    return;
  }
  if (!request.main_url.spec().empty() &&
      request.main_url.spec() != request.url.spec()) {
    ohos_prp_preload::LoaderInfo loader_info {
        url_loader_->resource_type_,
        url_loader_->keepalive_,
        url_loader_->do_not_prompt_for_login_,
        static_cast<int>(url_loader_->request_mode_),
        static_cast<int>(url_loader_->request_credentials_mode_),
        static_cast<int>(url_loader_->request_destination_)};
    preload_info->set_loader_info(loader_info);
    url_request->set_update_res_request_info_callback(base::BindRepeating(
        &URLLoaderUtils::UpdateResRequestInfo, weak_ptr_factory_.GetWeakPtr()));
    if (request.allow_preload_record) {
      preload_info->or_preload_flag(ohos_prp_preload::PRPP_FLAGS_VISIBLE);
    }
    if (request.is_preflight) {
      preload_info->set_type(
          ohos_prp_preload::PRRequestInfoType::TYPE_PAGE_PREFLIGHT);
    }
    preload_info->set_request_start_time(base::Time::Now().ToInternalValue());
    url_request->set_preload_info(preload_info);
    url_request->set_allow_preload_record(request.allow_preload_record);
    if (!org_main_url.empty() && org_main_url != request.main_url.spec()) {
      url_request->set_main_url(GURL(org_main_url));
    } else {
          url_request->set_main_url(request.main_url);
    }
  }
}

void URLLoaderUtils::RollbackFromPPRP()
{
  url_loader_->url_request_ = url_request_rollback_;
  url_request_rollback_ = nullptr;
  url_loader_->ProcessOutboundAttributionInterceptor();
}

void URLLoaderUtils::ResetUrlRequest(const std::shared_ptr<net::URLRequest>& url_request)
{
  if (prpp_loader_.get()) {
    url_loader_->url_request_ = url_request;
  }
}

void URLLoaderUtils::SetPrppLoaderCallbacks(std::shared_ptr<ohos_prp_preload::PRPPRequestLoader>& prpp_loader)
{
  prpp_loader_ = prpp_loader;
  url_loader_->url_request_ = prpp_loader_->GetURLRequest();
  prpp_loader_->SetRequestDelegate(url_loader_);
  prpp_loader_->SetRequestHeadersCallback(base::BindRepeating(
      &URLLoader::SetRawRequestHeadersAndNotify, base::Unretained(url_loader_)));
  if (url_loader_->devtools_request_id()) {
    prpp_loader_->SetResponseHeadersCallback(base::BindRepeating(
        &URLLoader::SetRawResponseHeaders, base::Unretained(url_loader_)));
  }
  prpp_loader_->SetEarlyResponseHeadersCallback(base::BindRepeating(
      &URLLoader::NotifyEarlyResponse, base::Unretained(url_loader_)));
  prpp_loader_->SetResetUrlRequestCallback(base::BindOnce(
      &URLLoaderUtils::ResetUrlRequest, base::Unretained(this)));
}

void URLLoaderUtils::ConfigureAndCreateURLRequest(
    const ResourceRequest& request,
    const net::NetworkTrafficAnnotationTag& traffic_annotation,
    const bool shared_storage_writable_eligible,
    std::shared_ptr<ohos_prp_preload::PRRequestInfo>& preload_info)
{
  if (shared_storage_writable_eligible) {
    preload_info->or_preload_flag(ohos_prp_preload::PRPP_FLAGS_UNSUPPORT);
  }
  url_loader_->url_request_ = url_loader_->url_request_context_->CreateRequestForPrpp(
      request.url, request.priority, url_loader_, traffic_annotation,
      false, /*is_for_websockets=*/
      request.net_log_create_info);
}

void URLLoaderUtils::UpdatePreconnectInfo()
{
  net::LoadTimingInfo load_timing_info;
  if (url_loader_->url_request_ && url_loader_->url_request_->preload_info()) {
    url_loader_->url_request_->GetLoadTimingInfo(&load_timing_info);
    bool is_preconnect = load_timing_info.socket_from_preload;
    bool is_reused_sokcet = (load_timing_info.socket_reused || load_timing_info.connect_timing.connect_end.is_null());
    bool need_to_update = false;
    if (is_preconnect && !url_loader_->url_request_->preload_info()->is_preconnect()) {
      url_loader_->url_request_->preload_info()->set_is_preconnect(is_preconnect);
      need_to_update = true;
    }
    if (is_reused_sokcet && !url_loader_->url_request_->preload_info()->is_reused_sokcet()) {
      url_loader_->url_request_->preload_info()->set_is_reused_sokcet(is_reused_sokcet);
      need_to_update = true;
    }
    if (need_to_update && already_update_info_) {
      ohos_prp_preload::PRParallelPreloadMgr::GetInstance().UpdateResRequestInfo(
          url_loader_->url_request_->main_url().spec(), url_loader_->url_request_->preload_info());
    }
  }
}

void URLLoaderUtils::SetPreloadFlagUnSupport()
{
  if (url_loader_->url_request_ && url_loader_->url_request_->preload_info()) {
    url_loader_->url_request_->preload_info()->or_preload_flag(ohos_prp_preload::PRPP_FLAGS_UNSUPPORT);
    if (already_update_info_) {
      ohos_prp_preload::PRParallelPreloadMgr::GetInstance().UpdateResRequestInfo(
          url_loader_->url_request_->main_url().spec(), url_loader_->url_request_->preload_info());
    }
  }
}

void URLLoaderUtils::SetPreloadFlagVisible(const net::RequestPriority priority)
{
  if ((url_loader_->resource_type_ == RESOURCE_TYPE_IMAGE) &&
      (priority >= net::MEDIUM) && url_loader_->url_request_ &&
      url_loader_->url_request_->preload_info() &&
      (url_loader_->url_request_->preload_info()->preload_flag() ==
      ohos_prp_preload::PRPP_FLAGS_NONE)) {
    url_loader_->url_request_->preload_info()->or_preload_flag(
        ohos_prp_preload::PRPP_FLAGS_VISIBLE);
    if (already_update_info_) {
      ohos_prp_preload::PRParallelPreloadMgr::GetInstance().UpdateResRequestInfo(
          url_loader_->url_request_->main_url().spec(), url_loader_->url_request_->preload_info());
    }
  }
}

void URLLoaderUtils::UpdateResponseTimes(
    mojom::URLResponseHeadPtr& response,
    net::LoadTimingInfo& load_timing_info)
{
  if (prpp_loader_.get()) {
    prpp_loader_->GetLoadTimingInfo(&load_timing_info);
    if (url_loader_->url_request_->was_cached()) {
      response->request_time = url_loader_->url_request_->request_time();
      response->response_time = url_loader_->url_request_->response_time();
      response->original_response_time = url_loader_->url_request_->original_response_time();
    } else {
      response->request_time = load_timing_info.request_start_time;
      response->response_time = base::Time::Now();
      response->original_response_time = base::Time::Now();
    }
  } else {
    response->request_time = url_loader_->url_request_->request_time();
    response->response_time = url_loader_->url_request_->response_time();
    response->original_response_time = url_loader_->url_request_->original_response_time();
  }
}

void URLLoaderUtils::GetResponseLoadingTime(mojom::URLResponseHeadPtr& response)
{
  if (url_loader_->is_load_timing_enabled_) {
    if (prpp_loader_.get()) {
      prpp_loader_->GetLoadTimingInfo(&response->load_timing);
    } else {
      url_loader_->url_request_->GetLoadTimingInfo(&response->load_timing);
    }
  }
}

void URLLoaderUtils::SetRequestStartTime(
    mojom::URLResponseHeadPtr& response,
    net::LoadTimingInfo& load_timing_info)
{
  if (prpp_loader_.get()) {
    response->request_start = load_timing_info.request_start;
  } else {
    response->request_start = url_loader_->url_request_->creation_time();
  }
}

void URLLoaderUtils::HandleRedirectUrl(const net::RedirectInfo& redirect_info)
{
  if (url_loader_->url_request_->main_url().spec().empty() &&
      !redirect_info.new_url.spec().empty() &&
      redirect_info.new_url.spec() != url_loader_->url_request_->url().spec()) {
    ohos_prp_preload::PRParallelPreloadMgr::GetInstance().UpdateRedirectUrl(
        url_loader_->url_request_->url().spec(), redirect_info.new_url.spec(),
        redirect_updated_);
    redirect_updated_ = true;
  }
}

bool URLLoaderUtils::HandlePrppLoaderStartReplay(
    std::shared_ptr<ohos_prp_preload::PRPPRequestLoader>& prpp_loader,
    URLLoaderContext& context,
    const ResourceRequest& request,
    const net::NetworkTrafficAnnotationTag& traffic_annotation,
    SharedDictionaryManager* shared_dictionary_manager,
    const std::string& org_main_url,
    std::shared_ptr<ohos_prp_preload::PRRequestInfo> preload_info,
    const bool shared_storage_writable_eligible)
  {
    if (prpp_loader.get()) {
      SetPrppLoaderCallbacks(prpp_loader);
      if (prpp_loader_->StartReplay()) {
        InitUrlRequestForRollback(
            context, request, traffic_annotation, shared_dictionary_manager,
            org_main_url, preload_info);
        return true;
      }
      prpp_loader_->SetRequestDelegate(nullptr);
      prpp_loader_ = nullptr;
    }
    ConfigureAndCreateURLRequest(request, traffic_annotation,
        shared_storage_writable_eligible, preload_info);
    return false;
  }

void URLLoaderUtils::CleanupAndRollback()
{
  // roll-back
  LOG(DEBUG) << "PRPPreload.URLLoader::OnResponseStarted roll-back from prpp";
  prpp_loader_->ClearLoaderCallback(url_loader_->devtools_request_id().has_value());
  prpp_loader_ = nullptr;
  base::SequencedTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE, base::BindOnce(&URLLoaderUtils::RollbackFromPPRP,
                                weak_ptr_factory_.GetWeakPtr()));
}

void URLLoaderUtils::GetLoadTimeInfo(net::LoadTimingInfo* load_timing_info)
{
  if (prpp_loader_.get()) {
    prpp_loader_->GetLoadTimingInfo(load_timing_info);
  } else {
    url_loader_->url_request_->GetLoadTimingInfo(load_timing_info);
  }
}

int URLLoaderUtils::ReadDataFromLoaderOrRequest(scoped_refptr<NetToMojoIOBuffer> buf)
{
  int bytes_read = 0;
  if (prpp_loader_.get()) {
    bytes_read = prpp_loader_->Read(
        buf.get(), static_cast<int>(url_loader_->pending_write_buffer_size_ -
                                    url_loader_->pending_write_buffer_offset_));
  } else {
    bytes_read = url_loader_->url_request_->Read(
        buf.get(), static_cast<int>(url_loader_->pending_write_buffer_size_ -
                                    url_loader_->pending_write_buffer_offset_));
  }
  return bytes_read;
}
#endif

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
std::optional<network::URLLoaderCompletionStatus>
URLLoaderUtils::CreateURLLoaderCompletionStatus() {
  if (!url_loader_ || !url_loader_->url_request_ ||
      !url_loader_->url_request_->context()) {
    return std::nullopt;
  }

  auto* ctx =
      const_cast<net::URLRequestContext*>(url_loader_->url_request_->context());
  const auto* ctx_ext = ctx->AsURLRequestContextExt();

  if (!ctx_ext || !ctx_ext->ShouldReportNewNavigationInfo()) {
    return std::nullopt;
  }

  auto* url_request = url_loader_->url_request_.get();

  URLLoaderCompletionStatus status;
  status.resolve_error_info = url_request->response_info().resolve_error_info;
  PopulateURLLoaderCompletionStatus(status);

  return status;
}

void URLLoaderUtils::PopulateURLLoaderCompletionStatus(
    URLLoaderCompletionStatus& status) {
  if (!url_loader_ || !url_loader_->url_request_ ||
      !url_loader_->url_request_->context()) {
    return;
  }

  auto* ctx =
      const_cast<net::URLRequestContext*>(url_loader_->url_request_->context());
  const auto* ctx_ext = ctx->AsURLRequestContextExt();
  if (!ctx_ext || !ctx_ext->ShouldReportNewNavigationInfo()) {
    return;
  }

  auto* url_request = url_loader_->url_request_.get();

  net::NavigationInfo nav_info;
  nav_info.request_url = url_request->url().spec();
  nav_info.request_uuid = url_request->request_uuid();
  nav_info.time_stamp = base::NumberToString(
      (url_request->creation_time() - base::TimeTicks::UnixEpoch())
          .InMilliseconds());
  nav_info.error_code = url_request->GetStatus();
  nav_info.original_error_code = url_request->GetOriginalNetErrorCode();

  nav_info.is_fallback_proxy_enabled =
      IsFallbackProxyEnabled(url_loader_->url_request_context_, url_request);
  status.used_fallback_proxy = url_request->used_fallback_proxy();
  status.needs_reload_with_fallback_proxy =
      url_request->needs_reload_with_fallback_proxy();

  if (url_loader_->url_request_context_) {
    if (auto* host_resolver =
            url_loader_->url_request_context_->host_resolver()) {
      nav_info.dns_name_servers = host_resolver->GetDnsServersString();
#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
      nav_info.can_use_secure_dns = host_resolver->CanUseSecureDnsFallback();
      net::IPEndPoint local_address;
      host_resolver->GetLocalAddress(&local_address);
      nav_info.local_address = local_address.ToStringWithoutPort();
#endif
    }
  }

  nav_info.request_attempts = url_request->GetRequestAttempts();
  nav_info.resolve_info = url_request->response_info().resolve_info;

  status.navigation_info = std::move(nav_info);
}
#endif
}
