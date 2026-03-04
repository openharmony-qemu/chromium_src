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

#include "arkweb/chromium_ext/content/browser/renderer_host/navigation_request_utils.h"
#include "content/public/common/content_switches.h"
#include "content/browser/renderer_host/frame_tree_node.h"
#include "mojo/public/cpp/bindings/callback_helpers.h"
#if BUILDFLAG(ARKWEB_READER_MODE)
#include "content/public/browser/web_contents_delegate.h"
#endif
#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
#include "arkweb/chromium_ext/content/browser/web_contents/web_contents_impl_ext.h"
#include "arkweb/chromium_ext/content/public/browser/error_page_reload_reason.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "arkweb/chromium_ext/net/base/navigation_info.h"
#include "arkweb/chromium_ext/net/base/request_attempt.h"
#include "arkweb/chromium_ext/services/network/public/mojom/navigation_info.mojom.h"
#include "services/network/public/mojom/url_loader_completion_status.mojom.h"
#endif

namespace content {
NavigationRequestUtils::NavigationRequestUtils(NavigationRequest* nav_request)
    : nav_request_(nav_request)
#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
      , enable_nweb_ex_(base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebEx))
#endif
{}

#if BUILDFLAG(ARKWEB_EXT_UA)
void NavigationRequestUtils::RemoveUserAgentHeaderForDevTools(
    bool devtools_useragent_override)
{
  if (devtools_useragent_override &&
      base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableNwebExUa)) {
    nav_request_->modified_request_headers_.RemoveHeader(net::HttpRequestHeaders::kUserAgent);
  }
}
#endif

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
network::mojom::NetworkContext* NavigationRequestUtils::GetNetworkContext() const
{
  if (!nav_request_->common_params_) {
    LOG(DEBUG) << "PRPPreload.NavigationRequestUtils::GetNetworkContext, no common_params";
    return nullptr;
  }
  if (nav_request_->common_params_->url.spec() == "about:blank") {
    LOG(DEBUG) << "PRPPreload.NavigationRequestUtils::GetNetworkContext, blank page not need";
    return nullptr;
  }
  if (!nav_request_->frame_tree_node_) {
    LOG(DEBUG) << "PRPPreload.NavigationRequestUtils::GetNetworkContext, no frame_tree_node";
    return nullptr;
  }
  if (!nav_request_->frame_tree_node_->IsMainFrame()) {
    LOG(DEBUG) << "PRPPreload.NavigationRequestUtils::GetNetworkContext, not main frame";
    return nullptr;
  }
  if (!nav_request_->frame_tree_node_->current_frame_host()) {
    LOG(DEBUG) << "PRPPreload.NavigationRequestUtils::GetNetworkContext, no current_frame_host";
    return nullptr;
  }
  if (!nav_request_->frame_tree_node_->current_frame_host()->GetStoragePartition()) {
    LOG(DEBUG) << "PRPPreload.NavigationRequestUtils::GetNetworkContext, no storage_partition";
    return nullptr;
  }

  return nav_request_->frame_tree_node_->current_frame_host()->GetStoragePartition()->GetNetworkContext();
}

using StartPageCallback__ = base::OnceCallback<void(const std::string&)>;
void NavigationRequestUtils::StartPage(const net::NetworkAnonymizationKey networkAnonymizationKey,
                                       uint64_t addr_web_handle)
{
  network::mojom::NetworkContext* network_context = GetNetworkContext();
  if (!network_context) {
    return;
  }

  addr_web_handle_ = addr_web_handle;
  network_context->StartPage(nav_request_->common_params_->url.spec(), networkAnonymizationKey, addr_web_handle_,
      mojo::WrapCallbackWithDefaultInvokeIfNotRun(base::BindOnce(&NavigationRequestUtils::OnGetIsolation,
          weak_factory_.GetWeakPtr()), ORIGIN));
}

void NavigationRequestUtils::OnGetIsolation(const std::string& origin)
{
  if (origin.starts_with(ORIGIN)) {
    LOG(DEBUG) << "PRPPreload.NavigationRequest::OnGetIsolation, canceled";
    return;
  }

  network::mojom::NetworkContext* network_context = GetNetworkContext();
  if (!network_context) {
    return;
  }

  url::Origin url_origin = url::Origin::Create(GURL(origin));
  net::IsolationInfo prp_isolation =
    nav_request_->frame_tree_node_->current_frame_host()->ComputeIsolationInfoForSubresourcesForPendingCommit(
        url_origin, nav_request_->is_credentialless(), nav_request_->ComputeFencedFrameNonce());
  network::mojom::URLLoaderFactoryParamsPtr params = network::mojom::URLLoaderFactoryParams::New();
  if (params) {
    params->isolation_info = prp_isolation;
    params->main_url = nav_request_->common_params_->url.spec();
    network_context->SetURLLoaderFactoryParam(std::move(params));
  }
}
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
void NavigationRequestUtils::SetAdblockEnabledStatus(
    blink::mojom::CommitNavigationParams* commit_params)
{
  LOG(DEBUG) << "[Adblock] CommitNavigation url : ***";
  // Cache the adblock enabled value for the download case.
  bool adblock_enabled = nav_request_->frame_tree_node_->is_adblock_enabled();
  nav_request_->frame_tree_node_->set_adblock_enabled_last_committed(adblock_enabled);

  if (nav_request_->frame_tree_node_->IsMainFrame()) {
    commit_params->site_adblock_enabled = adblock_enabled;
  }
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
bool NavigationRequestUtils::GetCustomScheme(const std::pair<url::Origin, std::string>& origin_and_debug_info)
{
  return base::Contains(url::GetCustomScheme(),
                        origin_and_debug_info.first.GetURL().scheme());
}
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
void NavigationRequestUtils::BeginNavigationImpl(
    bool& should_override_url_loading) {
  if (nav_request_->GetWebContents()->GetDelegate() &&
      nav_request_->GetWebContents()->GetDelegate()->IsForDistillerPage()) {
    should_override_url_loading =
        !nav_request_->common_params_->has_user_gesture &&
        !nav_request_->commit_params_->is_browser_initiated &&
        nav_request_->common_params_->referrer->url.host() !=
            nav_request_->common_params_->url.host();
  }
}
#endif

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
net::WebNavigationInfo& NavigationRequestUtils::GetWebNavigationInfo() {
  CHECK(nav_request_);
  return nav_request_->web_navigation_info_;
}

const net::WebNavigationInfo& NavigationRequestUtils::GetWebNavigationInfo()
    const {
  CHECK(nav_request_);
  return nav_request_->web_navigation_info_;
}

void NavigationRequestUtils::PopulateNavigationInfo(
    const std::optional<network::URLLoaderCompletionStatus>& status) {
  if (!enable_nweb_ex_) {
    return;
  }

  if (!GetContentClient()->browser()->ShouldReportNewNavigationInfo()) {
    return;
  }

  if (!status.has_value() || !status->navigation_info.has_value()) {
    return;
  }

  if (status->navigation_info->request_attempts.empty() ||
      status->navigation_info->request_url.empty() ||
      status->navigation_info->request_uuid.empty()) {
    return;
  }

  GetWebNavigationInfo().navigation_info = status->navigation_info.value();
  GetWebNavigationInfo().navigation_info.request_attempts.clear();
  GetWebNavigationInfo().navigation_info.error_code = status->error_code;

  GetWebNavigationInfo().is_auto_reload = IsAutoReload();
  GetWebNavigationInfo().auto_reload_reason =
      nav_request_->GetCurrentReloadReason();

  GetWebNavigationInfo().original_url = nav_request_->original_url_.spec();
  GetWebNavigationInfo().connection_type =
      static_cast<int>(net::NetworkChangeNotifier::GetConnectionType());

  GetWebNavigationInfo().did_use_fallback_proxy = status->used_fallback_proxy;
  GetWebNavigationInfo().is_captive_portal = false;

  const auto& nav_info = status->navigation_info.value();
  for (const auto& attempt : nav_info.request_attempts) {
    if (attempt.attempt_type == net::AttemptType::kContinueDespiteLastError) {
      GetWebNavigationInfo().has_ignore_certificate_error = true;
      continue;
    }

    if (attempt.attempt_type == net::AttemptType::kNormal ||
        attempt.attempt_type == net::AttemptType::kHttpDnsOnly ||
        attempt.attempt_type == net::AttemptType::kFallbackProxy) {
      GetWebNavigationInfo().navigation_info.request_attempts.push_back(
          attempt);
    }

    if (attempt.attempt_type == net::AttemptType::kFallbackProxy) {
      continue;
    }

    if (attempt.dns_info.UsedHttpDns()) {
      GetWebNavigationInfo().did_use_http_dns = true;
      GetWebNavigationInfo().secure_dns_records.insert(
          GetWebNavigationInfo().secure_dns_records.cend(),
          attempt.dns_info.address_list.cbegin(),
          attempt.dns_info.address_list.cend());
    } else if (attempt.dns_info.UsedInsecureDns()) {
      GetWebNavigationInfo().insecure_dns_records.insert(
          GetWebNavigationInfo().insecure_dns_records.cend(),
          attempt.dns_info.address_list.cbegin(),
          attempt.dns_info.address_list.cend());
    }
  }
}

bool NavigationRequestUtils::IsAutoReload() {
  return nav_request_->GetCurrentReloadReason() >
         ErrorPageReloadReason::INVALID;
}

void NavigationRequestUtils::OnReportNewNavigationInfo(
    const std::string& page_trace_id) {
  if (!enable_nweb_ex_) {
    return;
  }

  if (!GetContentClient()->browser()->ShouldReportNewNavigationInfo()) {
    return;
  }

  if (!(GURL(GetWebNavigationInfo().navigation_info.request_url)
            .SchemeIsHTTPOrHTTPS())) {
    GetWebNavigationInfo() = {};
    return;
  }

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  // Get safe browsing check detail from WebContents
  if (NavigatorDelegate* delegate = nav_request_->GetDelegate()) {
    content::WebContentsImpl* web_contents_impl =
        static_cast<content::WebContentsImpl*>(delegate);
    if (web_contents_impl && web_contents_impl->AsWebContentsImplExt()) {
      int code = 0;
      int threat_type = 0;
      GURL sa_url;
      web_contents_impl->AsWebContentsImplExt()->GetSafeBrowsingCheckDetail(
          code, threat_type, sa_url);
      if (sa_url == GURL(GetWebNavigationInfo().navigation_info.request_url)) {
        GetWebNavigationInfo().hw_code = code;
        GetWebNavigationInfo().threat_type = threat_type;
      }
    }
  }
#endif

  GetWebNavigationInfo().page_trace_id = page_trace_id;
  GetContentClient()->browser()->OnReportNewNavigationInfo(
      nav_request_->GetWebContents(), GetWebNavigationInfo());

  GetWebNavigationInfo() = {};
}
#endif

}  // namespace content
