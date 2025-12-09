// Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/chrome/browser/ssl/ohos_https_upgrades_interceptor.h"

#include "arkweb/chromium_ext/chrome/browser/ssl/ohos_https_upgrades_helper.h"


#include "base/functional/bind.h"
#include "base/metrics/histogram_functions.h"
#include "build/build_config.h"
#include "build/buildflag.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/browser/renderer_host/chrome_navigation_ui_data.h"
#include "chrome/browser/ssl/https_first_mode_settings_tracker.h"
#include "chrome/browser/ssl/https_only_mode_tab_helper.h"
#include "chrome/browser/ssl/https_upgrades_util.h"
#include "chrome/common/chrome_features.h"
#include "chrome/common/pref_names.h"
#include "components/captive_portal/core/buildflags.h"
#include "components/content_settings/core/browser/content_settings_registry.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/prefs/pref_service.h"
#include "components/security_interstitials/content/stateful_ssl_host_state_delegate.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/url_loader_request_interceptor.h"
#include "content/public/browser/web_contents.h"
#include "extensions/buildflags/buildflags.h"
#include "mojo/public/cpp/bindings/callback_helpers.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "mojo/public/cpp/system/data_pipe.h"
#include "net/base/url_util.h"
#include "net/http/http_status_code.h"
#include "net/http/http_util.h"
#include "net/url_request/redirect_info.h"
#include "services/network/public/cpp/is_potentially_trustworthy.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/cpp/url_loader_completion_status.h"
#include "services/network/public/mojom/network_context.mojom.h"
#include "services/network/public/mojom/url_loader.mojom.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "ui/base/page_transition_types.h"
#include "url/gurl.h"
#include "url/url_constants.h"

namespace {

// Updates a URL to HTTPS. URLs with the default port will result in the HTTPS
// URL using the default port 443. URLs with non-default ports won't have the
// port changed. For tests, the HTTPS port used can be overridden with
// OhosHttpsUpgradesInterceptor::SetHttpsPortForTesting().
GURL UpgradeUrlToHttps(const GURL& url) {
  DCHECK(!url.SchemeIsCryptographic());

  // Replace scheme with HTTPS.
  GURL::Replacements upgrade_url;
  upgrade_url.SetSchemeStr(url::kHttpsScheme);

  return url.ReplaceComponents(upgrade_url);
}

// Helper to configure an artificial redirect to `new_url`. This configures
// `response_head` and returns a computed RedirectInfo so both can be passed to
// URLLoaderClient::OnReceiveRedirect() to trigger the redirect.
net::RedirectInfo SetupRedirect(
    const network::ResourceRequest& request,
    const GURL& new_url,
    network::mojom::URLResponseHead* response_head) {
  response_head->encoded_data_length = 0;
  response_head->request_start = base::TimeTicks::Now();
  response_head->response_start = response_head->request_start;
  std::string header_string = base::StringPrintf(
      "HTTP/1.1 %i Temporary Redirect\n"
      "Location: %s\n"
      "Non-Authoritative-Reason: HttpsUpgrades\n",
      net::HTTP_TEMPORARY_REDIRECT, new_url.spec().c_str());
  response_head->headers = base::MakeRefCounted<net::HttpResponseHeaders>(
      net::HttpUtil::AssembleRawHeaders(header_string));
  net::RedirectInfo redirect_info = net::RedirectInfo::ComputeRedirectInfo(
      request.method, request.url, request.site_for_cookies,
      request.update_first_party_url_on_redirect
          ? net::RedirectInfo::FirstPartyURLPolicy::UPDATE_URL_ON_REDIRECT
          : net::RedirectInfo::FirstPartyURLPolicy::NEVER_CHANGE_URL,
      request.referrer_policy, request.referrer.spec(),
      net::HTTP_TEMPORARY_REDIRECT, new_url,
      /*referrer_policy_header=*/absl::nullopt,
      /*insecure_scheme_was_upgraded=*/false);
  return redirect_info;
}

}  // namespace

using RequestHandler = OhosHttpsUpgradesInterceptor::RequestHandler;

bool ShouldExcludeNavigationFromUpgrades(
    content::FrameTreeNodeId frame_id) {
  content::FrameTreeNode* frame_tree_node =
          content::FrameTreeNode::GloballyFindByID(frame_id);
  if (!frame_tree_node || !(frame_tree_node->navigation_request())) {
    return true;
  }
  content::NavigationRequest* request = frame_tree_node->navigation_request();
  // if is_browser_initiated == false, it means user tap a link to start navigation. 
  bool is_browser_initiated = request->browser_initiated();
  bool is_url_typed_with_http_scheme = request->is_url_typed_with_http_scheme();
  bool is_force_no_https_upgrade = request->is_force_no_https_upgrade();

  bool should_exclude_upgrade = (is_browser_initiated && is_url_typed_with_http_scheme) || is_force_no_https_upgrade;

  return should_exclude_upgrade;
}

// static
std::unique_ptr<OhosHttpsUpgradesInterceptor>
OhosHttpsUpgradesInterceptor::MaybeCreateInterceptor(content::FrameTreeNodeId frame_tree_node_id) {
  auto* web_contents =
      content::WebContents::FromFrameTreeNodeId(frame_tree_node_id);
  // Could be null if the FrameTreeNode's RenderFrameHost is shutting down.
  if (!web_contents) {
    return nullptr;
  }
  auto* https_helper = OhosHttpsUpgradesHelper::FromWebContents(web_contents);
  if (!https_helper || !(https_helper->should_upgrade_to_https())) {
    return nullptr;
  }
  return std::make_unique<OhosHttpsUpgradesInterceptor>(frame_tree_node_id);
}

OhosHttpsUpgradesInterceptor::OhosHttpsUpgradesInterceptor(
    content::FrameTreeNodeId frame_tree_node_id)
    : frame_tree_node_id_(frame_tree_node_id) {}

OhosHttpsUpgradesInterceptor::~OhosHttpsUpgradesInterceptor() = default;

void OhosHttpsUpgradesInterceptor::MaybeCreateLoader(
    const network::ResourceRequest& tentative_resource_request,
    content::BrowserContext* browser_context,
    content::URLLoaderRequestInterceptor::LoaderCallback callback) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  Profile* profile = Profile::FromBrowserContext(browser_context);
  if (!profile ||
      !g_browser_process->profile_manager()->IsValidProfile(profile)) {
    std::move(callback).Run({});
    return;
  }

  // Note: Redirects cause a restarted request with a new call to
  // MaybeCreateLoader().
  auto* web_contents =
      content::WebContents::FromFrameTreeNodeId(frame_tree_node_id_);
  // Could be null if the FrameTreeNode's RenderFrameHost is shutting down.
  if (!web_contents) {
    std::move(callback).Run({});
    return;
  }

  auto* tab_helper = HttpsOnlyModeTabHelper::FromWebContents(web_contents);
  if (!tab_helper) {
    HttpsOnlyModeTabHelper::CreateForWebContents(web_contents);
  }

  // Exclude HTTPS URLs.
  if (tentative_resource_request.url.SchemeIs(url::kHttpsScheme)) {
    std::move(callback).Run({});
    return;
  }

  // Exclude all other schemes other than HTTP.
  if (!tentative_resource_request.url.SchemeIs(url::kHttpScheme)) {
    std::move(callback).Run({});
    return;
  }
  // Exclude all IPHost URLs.
  if (tentative_resource_request.url.HostIsIPAddress()) {
    std::move(callback).Run({});
    return;
  }

  // Exclude "localhost" (and loopback addresses) as they do not expose traffic
  // over the network.
  // TODO(crbug.com/1394910): Extend the exemption list for HTTPS-Upgrades
  // beyond just localhost.
  if (net::IsLocalhost(tentative_resource_request.url)) {
    std::move(callback).Run({});
    return;
  }

  // Don't exclude local-network requests (for now) but record metrics for them.
  // TODO(crbug.com/1394910): Extend the exemption list for HTTPS-Upgrades
  // beyond just localhost.
  if (net::IsHostnameNonUnique(tentative_resource_request.url.host())) {
    // For HTTPS-Upgrades, skip attempting to upgrade non-unique hostnames
    // as they can't get publicly-trusted certificates.
    //
    // HTTPS-First Mode does not exempt these hosts in order to ensure that
    // Chrome shows the HTTP interstitial before navigation to them.
    // Potentially, these could fast-fail instead and skip directly to the
    // interstitial.
    auto* https_helper = OhosHttpsUpgradesHelper::FromWebContents(web_contents);
    if (!https_helper || !(https_helper->should_upgrade_to_https())) {
      std::move(callback).Run({});
      return;
    }
  }

  // Captive portals and manually-entered http:// navigations are excluded from
  // upgrades and we shouldn't warn on them when strict mode isn't enabled, so
  // allowlist those http:// connections instead.
  // TODO(crbug.com/363205521): Consider whether we want to allowlist captive
  // portal hostnames.
  StatefulSSLHostStateDelegate* state =
      static_cast<StatefulSSLHostStateDelegate*>(
          profile->GetSSLHostStateDelegate());
  auto* storage_partition =
      web_contents->GetPrimaryMainFrame()->GetStoragePartition();
  if (ShouldExcludeNavigationFromUpgrades(frame_tree_node_id_)) {
    if (state) {
      state->AllowHttpForHost(tentative_resource_request.url.host(),
                              storage_partition);
    }
    std::move(callback).Run({});
    return;
  }

  // Check whether this host would be upgraded to HTTPS by HSTS. This requires a
  // Mojo call to the network service, so set up a callback to continue the rest
  // of the MaybeCreateLoader() logic (passing along the necessary state). The
  // HSTS status will be passed as a boolean to
  // MaybeCreateLoaderOnHstsQueryCompleted(). If the Mojo call fails, this will
  // default to passing `false` and continuing as though the host does not have
  // HSTS (i.e., it will proceed with the HTTPS-First Mode logic).
  // TODO(crbug.com/1394910): Consider caching this result, at least within the
  // same navigation.
  auto query_complete_callback = base::BindOnce(
      &OhosHttpsUpgradesInterceptor::MaybeCreateLoaderOnHstsQueryCompleted,
      weak_factory_.GetWeakPtr(), tentative_resource_request,
      std::move(callback));
  network::mojom::NetworkContext* network_context =
      profile->GetDefaultStoragePartition()->GetNetworkContext();
  if (network_context) {
    network_context->IsHSTSActiveForHost(
                     tentative_resource_request.url.host(),
                     mojo::WrapCallbackWithDefaultInvokeIfNotRun(
                     std::move(query_complete_callback),
                     /*is_hsts_active_for_host=*/false));
  }
}

void OhosHttpsUpgradesInterceptor::MaybeCreateLoaderOnHstsQueryCompleted(
    const network::ResourceRequest& tentative_resource_request,
    content::URLLoaderRequestInterceptor::LoaderCallback callback,
    bool is_hsts_active_for_host) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  // Reconstruct objects here instead of binding them as parameters to this
  // callback method.
  //
  // It's possible for the WebContents to be destroyed during the
  // asynchronous HSTS query call, before this callback is run. If it no longer
  // exists, don't upgrade and return. (See crbug.com/1499515.)
  content::WebContents* web_contents =
      content::WebContents::FromFrameTreeNodeId(frame_tree_node_id_);
  if (!web_contents) {
    std::move(callback).Run({});
    return;
  }

  Profile* profile =
      Profile::FromBrowserContext(web_contents->GetBrowserContext());
  HttpsOnlyModeTabHelper* tab_helper =
      HttpsOnlyModeTabHelper::FromWebContents(web_contents);
  CHECK(profile);
  CHECK(tab_helper);

  // Only serve upgrade redirects for main frame, GET requests.
  if (!tentative_resource_request.is_outermost_main_frame ||
      tentative_resource_request.method != "GET") {
    std::move(callback).Run({});
    return;
  }
  
  // For non-strict modes, skip attempting to upgrade URLs with non-default
  // ports, as these are unlikely to succeed (the server needs to support HTTP
  // and HTTPS on the same port, or the URL needs to be incorrectly have an
  // HTTP scheme but with the server's HTTPS port).
  if (tentative_resource_request.url.has_port()) {
    std::move(callback).Run({});
    return;
  }

  auto* rfh = web_contents->GetPrimaryMainFrame();
  if (rfh == nullptr) {
    return;
  }

  StatefulSSLHostStateDelegate* state =
      static_cast<StatefulSSLHostStateDelegate*>(
          profile->GetSSLHostStateDelegate());
  auto* storage_partition = rfh->GetStoragePartition();
  if (state && state->IsHttpAllowedForHost(
                   tentative_resource_request.url.host(), storage_partition)) {
    // Renew the allowlist expiration for this host as the user is still
    // actively using it. This means that the allowlist entry will stay
    // valid until the user stops visiting this host for the entire
    // expiration period (one week).
    state->AllowHttpForHost(tentative_resource_request.url.host(),
                            storage_partition);

    std::move(callback).Run({});
    return;
  }

  // If this is a back/forward navigation to a failed upgrade, then don't
  // intercept to upgrade the navigation. Other forms of re-visiting a URL
  // that previously failed to be upgraded to HTTPS *should* be intercepted so
  // the upgrade can be attempted again (e.g., the user reloading the tab, the
  // user navigating around and ending back on this URL in the same tab, etc.).
  //
  // This effectively "caches" the HTTPS-First Mode interstitial for the
  // history entry of a failed upgrade for the lifetime of the tab. This means
  // that it is possible for a user to come back much later (say, a week later),
  // after a site has fixed its HTTPS configuration, and still see the
  // interstitial for that URL.
  //
  // Without this check, resetting the HTTPS-Upgrades flags in
  // HttpsOnlyModeTabHelper::DidStartNavigation() means the Interceptor would
  // fire on back/forward navigation to the interstitial, which causes an
  // "extra" interstitial entry to be added to the history list and lose other
  // entries.
  auto* entry = web_contents->GetController().GetPendingEntry();
  if (entry && (entry->GetTransitionType() & ui::PAGE_TRANSITION_FORWARD_BACK) &&
      tab_helper->has_failed_upgrade(tentative_resource_request.url)) {
    std::move(callback).Run({});
    return;
  }

  // Both HTTPS-First Mode and HTTPS-Upgrades are forms of upgrading all HTTP
  // navigations to HTTPS, with HTTPS-First Mode additionally enabling the
  // HTTP interstitial on fallback.
  auto* https_helper = OhosHttpsUpgradesHelper::FromWebContents(web_contents);
  if (!base::FeatureList::IsEnabled(features::kHttpsUpgrades) &&
      (!https_helper || !(https_helper->should_upgrade_to_https()))) {
    std::move(callback).Run({});
    return;
  }

  // If the request URL is in the set of URLs that HttpsUpgradesInterceptor has
  // already processed, skip upgrading and trigger fallback to HTTP to avoid a
  // redirect loop.
  if (base::Contains(urls_seen_, tentative_resource_request.url)) {
     if (state) {
        state->AllowHttpForHost(
            tab_helper->fallback_url().host(),
            rfh->GetStoragePartition());
      }
    tab_helper->set_is_navigation_upgraded(false);
    tab_helper->set_is_navigation_fallback(true);
    tab_helper->add_failed_upgrade(tab_helper->fallback_url());
    tab_helper->set_is_ssl_error(false);

    // Note: If `fallback_url` is the same as the request URL, this
    // could skip doing an additional redirect, but then the NavigationThrottle
    // doesn't have the ability to act on this navigation request and apply
    // the HTTPS-First Mode interstitial. If we add a better way to "fast fail"
    // navigations directly to the interstitial, then we could probably use that
    // here as well as an optimization.
    LOG(INFO) << "The URL was previously upgraded to HTTPS"
              <<  " but has been fallback to HTTP, "
              <<  "no further upgrade will be performed this time.";
    std::move(callback).Run(CreateRedirectHandler(tab_helper->fallback_url()));
    return;
  }

  // Not a redirect loop. Add the current request URL to the set of URLs seen.
  urls_seen_.insert(tentative_resource_request.url);

  // Mark navigation as upgraded.
  tab_helper->set_is_navigation_upgraded(true);
  tab_helper->set_fallback_url(tentative_resource_request.url);
  LOG(INFO) << "The URL will be upgrade to https";
  GURL https_url = UpgradeUrlToHttps(tentative_resource_request.url);
  std::move(callback).Run(CreateRedirectHandler(https_url));
}

bool OhosHttpsUpgradesInterceptor::MaybeCreateLoaderForResponse(
    const network::URLLoaderCompletionStatus& status,
    const network::ResourceRequest& request,
    network::mojom::URLResponseHeadPtr* response_head,
    mojo::ScopedDataPipeConsumerHandle* response_body,
    mojo::PendingRemote<network::mojom::URLLoader>* loader,
    mojo::PendingReceiver<network::mojom::URLLoaderClient>* client_receiver,
    blink::ThrottlingURLLoader* url_loader) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  // When an upgraded navigation fails, this method creates a loader to trigger
  // the fallback to HTTP.

  auto* web_contents =
      content::WebContents::FromFrameTreeNodeId(frame_tree_node_id_);
  if (!web_contents) {
    // `web_contents` can be null if the tab is being closed. Skip handling
    // failure in that case since the page is going away anyway.
    return false;
  }

  auto* tab_helper = HttpsOnlyModeTabHelper::FromWebContents(web_contents);
  if (!tab_helper || !tab_helper->is_navigation_upgraded()) {
    return false;
  }

  // Note: MaybeCreateLoaderForResponse() is called for all navigation
  // responses and failures, but not for things like a NavigationThrottle
  // cancelling or blocking the navigation.
  // Only intercept if the navigation failed.
  if (!tab_helper->is_ssl_error() && status.error_code == net::OK) {
    LOG(INFO) << "httpsUpgrades: this navigation status_code is OK and has no ssl error, "
              << "which will not fall back to http";
    return false;
  }

  if (!request.is_outermost_main_frame) {
    return false;
  }

  Profile* profile =
      Profile::FromBrowserContext(web_contents->GetBrowserContext());
  if (profile == nullptr) {
    return false;
  }
  StatefulSSLHostStateDelegate* state =
      static_cast<StatefulSSLHostStateDelegate*>(
          profile->GetSSLHostStateDelegate());
  auto* rfh = web_contents->GetPrimaryMainFrame();
  if (rfh == nullptr) {
    return false;
  }
  if (state) {
    state->AllowHttpForHost(
        tab_helper->fallback_url().host(),
        rfh->GetStoragePartition());
  }
  LOG(INFO) << "httpsUpgrades: the url has been fallback to http, status_code is " << status.error_code
            << ", and is_ssl_error is " << tab_helper->is_ssl_error();
  tab_helper->set_is_navigation_upgraded(false);
  tab_helper->set_is_navigation_fallback(true);
  tab_helper->add_failed_upgrade(tab_helper->fallback_url());
  tab_helper->set_is_ssl_error(false);
  // `client_` may have been previously bound from handling the initial upgrade
  // in MaybeCreateLoader(), so reset it before re-binding it to handle this
  // response.
  client_.reset();
  *client_receiver = client_.BindNewPipeAndPassReceiver();

  // Create an artificial redirect back to the fallback URL.
  auto new_response_head = network::mojom::URLResponseHead::New();
  net::RedirectInfo redirect_info = SetupRedirect(
      request, tab_helper->fallback_url(), new_response_head.get());
  client_->OnReceiveRedirect(redirect_info, std::move(new_response_head));
  return true;
}

RequestHandler OhosHttpsUpgradesInterceptor::CreateRedirectHandler(
    const GURL& new_url) {
  return base::BindOnce(&OhosHttpsUpgradesInterceptor::RedirectHandler,
                        weak_factory_.GetWeakPtr(), new_url);
}

void OhosHttpsUpgradesInterceptor::RedirectHandler(
    const GURL& new_url,
    const network::ResourceRequest& request,
    mojo::PendingReceiver<network::mojom::URLLoader> receiver,
    mojo::PendingRemote<network::mojom::URLLoaderClient> client) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  // Set up Mojo connection and initiate the redirect. `client_` and `receiver_`
  // may have been previously bound from handling a previous upgrade earlier in
  // the same navigation, so reset them before re-binding them to handle a new
  // redirect.
  receiver_.reset();
  client_.reset();
  receiver_.Bind(std::move(receiver));
  receiver_.set_disconnect_handler(
      base::BindOnce(&OhosHttpsUpgradesInterceptor::OnConnectionClosed,
                     weak_factory_.GetWeakPtr()));
  client_.Bind(std::move(client));

  // Create redirect.
  auto response_head = network::mojom::URLResponseHead::New();
  net::RedirectInfo redirect_info =
      SetupRedirect(request, new_url, response_head.get());

  client_->OnReceiveRedirect(redirect_info, std::move(response_head));
}

void OhosHttpsUpgradesInterceptor::OnConnectionClosed() {
  // This happens when content cancels the navigation. Reset the receiver and
  // client handle.
  receiver_.reset();
  client_.reset();
}
