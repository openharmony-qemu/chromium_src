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

namespace content {
NavigationRequestUtils::NavigationRequestUtils(NavigationRequest* nav_request)
{
  this->nav_request_ = nav_request;
}

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
void NavigationRequestUtils::StartPage(const net::NetworkAnonymizationKey& networkAnonymizationKey,
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
  bool find_custom_scheme = false;
  std::string scheme = origin_and_debug_info.first.GetURL().scheme();
  for (size_t index = 0; index < url::GetCustomScheme().size(); index++) {
    if (scheme == url::GetCustomScheme()[index]) {
      find_custom_scheme = true;
    }
  }
  return find_custom_scheme;
}
#endif

}  // namespace content
