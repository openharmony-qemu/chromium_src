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

#include "arkweb/build/features/features.h"
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "cef/ohos_cef_ext/libcef/browser/net_service/net_helpers.h"
#include "content/public/browser/shared_cors_origin_access_list.h"

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
#include "chrome/browser/preloading/prefetch/no_state_prefetch/chrome_no_state_prefetch_contents_delegate.h"
#include "chrome/browser/preloading/prefetch/no_state_prefetch/chrome_no_state_prefetch_processor_impl_delegate.h"
#include "chrome/browser/preloading/prefetch/no_state_prefetch/no_state_prefetch_manager_factory.h"
#include "components/no_state_prefetch/browser/no_state_prefetch_contents.h"
#include "components/no_state_prefetch/browser/no_state_prefetch_manager.h"
#include "components/no_state_prefetch/browser/no_state_prefetch_processor_impl.h"
#include "ohos_cef_ext/libcef/browser/predictors/loading_predictor.h"
#include "ohos_cef_ext/libcef/browser/predictors/loading_predictor_factory.h"
#include "ohos_cef_ext/libcef/browser/predictors/predictor_database.h"
#endif  // BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "components/subresource_filter/content/browser/content_subresource_filter_throttle_manager.h"
#include "ohos_cef_ext/libcef/browser/subresource_filter/adblock_content_subresource_filter_web_contents_helper_factory.h"
#endif  // ARKWEB_ADBLOCK

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/content_settings/page_specific_content_settings_delegate.h"
#include "components/content_settings/browser/page_specific_content_settings.h"
#include "ipc/ipc_channel_proxy.h"
#include "ohos_cef_ext/libcef/browser/content_settings/oh_host_content_settings_observer.h"
#include "ohos_cef_ext/libcef/browser/content_settings/oh_host_content_settings_observer_factory.h"
#endif

#if BUILDFLAG(ARKWEB_ZOOM)
#include "cef/ohos_cef_ext/libcef/browser/osr/arkweb_render_widget_host_view_osr_ext.h"
#include "content/public/browser/render_widget_host.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_HTTP_DNS_FALLBACK)
#include "arkweb/chromium_ext/servieces/network/public/mojom/network_config_ohos.mojom.h"
#endif

#if BUILDFLAG(ARKWEB_FCP)
#include "cef/ohos_cef_ext/libcef/browser/page_load_metrics/arkweb_page_load_metrics_initialize.h"
#include "components/page_load_metrics/browser/metrics_navigation_throttle.h"
#include "components/page_load_metrics/browser/metrics_web_contents_observer.h"
#include "components/page_load_metrics/browser/page_load_metrics_embedder_base.h"
#include "components/page_load_metrics/browser/page_load_metrics_memory_tracker.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
#include "cef/ohos_cef_ext/libcef/browser/net/ohos_applink_throttle.h"
#endif  // BUILDFLAG(IS_ARKWEB)

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#include "cef/libcef/browser/browser_host_base.h"
#endif

enum AppLoadedInTabSource {
  // A platform app page tried to load one of its own URLs in a tab.
  APP_LOADED_IN_TAB_SOURCE_APP = 0,

  // A platform app background page tried to load one of its own URLs in a tab.
  APP_LOADED_IN_TAB_SOURCE_BACKGROUND_PAGE,

  // An extension or app tried to load a resource of a different platform app in
  // a tab.
  APP_LOADED_IN_TAB_SOURCE_OTHER_EXTENSION,

  // A non-app and non-extension page tried to load a platform app in a tab.
  APP_LOADED_IN_TAB_SOURCE_OTHER,

  APP_LOADED_IN_TAB_SOURCE_MAX
};

#if BUILDFLAG(ENABLE_EXTENSIONS_CORE)
AppLoadedInTabSource ClassifyAppLoadedInTabSource(
    const GURL& opener_url,
    const extensions::Extension* target_platform_app) {
  if (!opener_url.SchemeIs(extensions::kExtensionScheme)
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
      && !opener_url.SchemeIs(extensions::kArkwebExtensionScheme)
#endif
  ) {
    // The forbidden app URL was being opened by a non-extension page (e.g.
    // http).
    return APP_LOADED_IN_TAB_SOURCE_OTHER;
  }

  if (opener_url.host_piece() != target_platform_app->id()) {
    // The forbidden app URL was being opened by a different app or extension.
    return APP_LOADED_IN_TAB_SOURCE_OTHER_EXTENSION;
  }

  // This platform app was trying to window.open() one of its own URLs.
  if (opener_url ==
      extensions::BackgroundInfo::GetBackgroundURL(target_platform_app)) {
    // Source was the background page.
    return APP_LOADED_IN_TAB_SOURCE_BACKGROUND_PAGE;
  }

  // Source was a different page inside the app.
  return APP_LOADED_IN_TAB_SOURCE_APP;
}
#endif  // BUILDFLAG(ENABLE_EXTENSIONS_CORE)

class ChromeContentBrowserClientUtils {
 public:
#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
  static void RenderProcessWillLaunch(content::RenderProcessHost* host,
                                      raw_ptr<ChromeContentBrowserClient> obj) {
    if (base::CommandLine::ForCurrentProcess()->HasSwitch(
            switches::kEnableNwebEx)) {
      auto renderer_configuration = obj->GetRendererConfiguration(host);
      RendererContentSettingRules rules;
      content_settings::GetRendererContentSettingRules(
          HostContentSettingsMapFactory::GetForProfile(
              host->GetBrowserContext()),
          &rules);
      renderer_configuration->SetContentSettingRules(rules);
    }
  }
#endif  // BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)

#if BUILDFLAG(ARKWEB_ZOOM)
  static CefRefPtr<CefBrowserHostBase> OverrideWebkitPrefsGetBrowserHost(
      WebContents* web_contents,
      raw_ptr<ChromeContentBrowserClient> obj) {
    auto rvh = web_contents->GetRenderViewHost();
    CefRenderWidgetHostViewOSR* rwhvb =
        static_cast<CefRenderWidgetHostViewOSR*>(rvh->GetWidget()->GetView());
    CefRefPtr<CefBrowserHostBase> browser_host =
        CefBrowserHostBase::GetBrowserForHost(rvh);
    if (rwhvb && rwhvb->IsRenderWidgetHostViewChildFrame() && browser_host) {
      return;
    }

    if (rwhvb && browser_host && rwhvb->AsArkWebRenderWidgetHostViewOSRExt()) {
      rwhvb->AsArkWebRenderWidgetHostViewOSRExt()->SetDoubleTapSupportEnabled(
          browser_host->settings().supports_double_tap_zoom);
      rwhvb->AsArkWebRenderWidgetHostViewOSRExt()
          ->SetMultiTouchZoomSupportEnabled(
              browser_host->settings().supports_multi_touch_zoom);
    }
    return browser_host;
  }
#endif

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
  static void SetAutoplayPolicyExt(CefRefPtr<CefBrowserHostBase> browser_host,
                                   WebContents* web_contents,
                                   WebPreferences* web_prefs) {
    if (browser_host && browser_host->settings().user_gesture_required) {
      web_prefs->autoplay_policy =
          GetAutoplayPolicyForWebContents(web_contents);
    } else {
      web_prefs->autoplay_policy =
          blink::mojom::AutoplayPolicy::kNoUserGestureRequired;
    }
  }
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
  static void TrigAdBlockEnabledExt(content::NavigationHandle* handle) {
    CreateSubresourceFilterWebContentsHelper(handle);

    if (!handle->IsDownload() && handle->IsInMainFrame() &&
        handle->GetURL().SchemeIsHTTPOrHTTPS() && handle->GetWebContents()) {
      LOG(INFO) << "[AdBlock] Start to get adblock switch from UI";
      handle->GetWebContents()->TrigAdBlockEnabledForSiteFromUi(
          handle->GetURL().spec());
    }
  }
#endif  // ARKWEB_ADBLOCK

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  static void NoStatePrefetchContentsExt(
      content::BrowserContext* browser_context,
      const base::RepeatingCallback<content::WebContents*()>& wc_getter,
      std::vector<std::unique_ptr<blink::URLLoaderThrottle>>& result) {
    prerender::NoStatePrefetchManager* no_state_prefetch_manager =
        prerender::NoStatePrefetchManagerFactory::GetForBrowserContext(
            browser_context);
    if (no_state_prefetch_manager) {
      prerender::NoStatePrefetchContents* no_state_prefetch_contents =
          no_state_prefetch_manager->GetNoStatePrefetchContents(
              wc_getter.Run());
      if (no_state_prefetch_contents) {
        mojo::PendingRemote<prerender::mojom::NoStatePrefetchCanceler> canceler;
        no_state_prefetch_contents->AddNoStatePrefetchCancelerReceiver(
            canceler.InitWithNewPipeAndPassReceiver());
        result.push_back(
            std::make_unique<prerender::NoStatePrefetchURLLoaderThrottle>(
                prerender::NoStatePrefetchHistograms::GetHistogramPrefix(
                    no_state_prefetch_contents->origin()),
                std::move(canceler)));
      }
    }
  }
#endif  // ARKWEB_NO_STATE_PREFETCH

#if BUILDFLAG(IS_ARKWEB)
  static void AppLinkThrottleExt(
      const network::ResourceRequest& request,
      std::vector<std::unique_ptr<blink::URLLoaderThrottle>>& result,
      content::FrameTreeNodeId frame_tree_node_id) {
    if (request.destination == network::mojom::RequestDestination::kDocument &&
        request.url.SchemeIs(url::kHttpsScheme) &&
        request.transition_type !=
            static_cast<int32_t>(ui::PAGE_TRANSITION_RELOAD) &&
        (request.transition_type & ui::PAGE_TRANSITION_FROM_ADDRESS_BAR) == 0 &&
        (request.transition_type & ui::PAGE_TRANSITION_FORWARD_BACK) == 0) {
      bool is_same_host = true;
      if (request.request_initiator.has_value()) {
        if (request.request_initiator.value().host() != request.url.host()) {
          is_same_host = false;
        }
      } else {
        is_same_host = false;
      }

      if (!is_same_host) {
        result.push_back(std::make_unique<throttle::OhosAppLinkThrottle>(
            frame_tree_node_id, (request.transition_type &
                                 ui::PAGE_TRANSITION_CLIENT_REDIRECT) != 0));
      }
    }
  }
#endif

#if BUILDFLAG(ARKWEB_HTTP_DNS)
  static void DnsOverHttpsConfigExt(
      network::mojom::NetworkService* network_service) {
    if (net_service::NetHelpers::HasValidDnsOverHttpConfig()) {
      auto config = net::DnsOverHttpsServerConfig::FromString(
          net_service::NetHelpers::DnsOverHttpServerConfig());
      if (config.has_value()) {
        network_service->ConfigureStubHostResolver(
            true, net_service::NetHelpers::DnsOverHttpMode(),
            net::DnsOverHttpsConfig({{std::move(*config)}}), true);
      } else {
        LOG(INFO) << "doh server invalid";
      }
    }
  }
#endif

  static void SSLConfigAndRegisterObserverExt(
      content::BrowserContext* context,
      network::mojom::NetworkContextParams* network_context_params) {
#if BUILDFLAG(ARKWEB_SSL_AUTH_ALGO)
    network_context_params->initial_ssl_config =
        network::mojom::SSLConfig::New();
    network_context_params->initial_ssl_config->version_min =
        network::mojom::SSLVersion::kMinValue;
#endif

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
    if (base::CommandLine::ForCurrentProcess()->HasSwitch(
            switches::kEnableNwebEx)) {
      OhHostContentSettingsObserverFactory::GetInstance()->RegisterObserver(
          context);
      LOG(INFO) << "ExceptionList "
                   "ChromeContentBrowserClient::ConfigureNetworkContextParams ";
    }
#endif  // BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
  }
};

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
mojo::AssociatedRemote<chrome::mojom::RendererConfiguration>
ChromeContentBrowserClient::GetRendererConfiguration(
    content::RenderProcessHost* render_process_host) {
  IPC::ChannelProxy* channel = render_process_host->GetChannel();
  if (!channel) {
    return mojo::AssociatedRemote<chrome::mojom::RendererConfiguration>();
  }

  mojo::AssociatedRemote<chrome::mojom::RendererConfiguration>
      renderer_configuration;
  channel->GetRemoteAssociatedInterface(&renderer_configuration);
  return renderer_configuration;
}
#endif  // BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
bool ChromeContentBrowserClient::ShouldOverrideUrlLoading(
    content::FrameTreeNodeId frame_tree_node_id,
    bool browser_initiated,
    const GURL& gurl,
    const std::string& request_method,
    bool has_user_gesture,
    bool is_redirect,
    bool is_outermost_main_frame,
    bool is_prerendering,
    ui::PageTransition transition,
    bool* ignore_navigation) {
  *ignore_navigation = false;

  // Only GETs can be overridden.
  if (request_method != "GET") {
    return true;
  }

#if BUILDFLAG(ARKWEB_PDF)
  if (gurl.SchemeIs(extensions::kExtensionScheme)) {
    return true;
  }
#endif

  bool application_initiated =
      browser_initiated || transition & ui::PAGE_TRANSITION_FORWARD_BACK;

  // Don't offer application-initiated navigations unless it's a redirect.
  if (application_initiated && !is_redirect) {
    return true;
  }

  // For HTTP schemes, only top-level navigations can be overridden. Similarly,
  // WebView Classic lets app override only top level about:blank navigations.
  // So we filter out non-top about:blank navigations here.
  //
  // The uuid-in-package scheme is used for subframe navigation with WebBundles
  // (https://github.com/WICG/webpackage/blob/main/explainers/subresource-loading-opaque-origin-iframes.md),
  // so treat it in the same way as http(s).
  //
  // Note: about:blank navigations are not received in this path at the moment,
  // they use the old SYNC IPC path as they are not handled by network stack.
  // However, the old path should be removed in future.
  if (!is_outermost_main_frame &&
      (gurl.SchemeIs(url::kHttpScheme) || gurl.SchemeIs(url::kHttpsScheme) ||
       gurl.SchemeIs(url::kAboutScheme) ||
       gurl.SchemeIs(url::kUuidInPackageScheme))) {
    return true;
  }

  content::WebContents* web_contents =
      content::WebContents::FromFrameTreeNodeId(frame_tree_node_id);
  if (web_contents == nullptr) {
    return true;
  }

  CefRefPtr<CefBrowserHostBase> browser_host =
      CefBrowserHostBase::GetBrowserForContents(web_contents);
  if (browser_host == nullptr) {
    return true;
  }

  if (auto client = browser_host->GetClient()) {
    if (auto handler = client->GetRequestHandler()) {
      *ignore_navigation =
          handler->AsCefRequestHandlerExt()->ShouldOverrideUrlLoading(
              browser_host.get(), gurl.possibly_invalid_spec(), request_method,
              has_user_gesture, is_redirect, is_outermost_main_frame);
      return true;
    }
  }

  return true;
}
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
void ChromeContentBrowserClient::UpdateAdBlockEnabledForSite(
    content::RenderFrameHost* rfh,
    const GURL& gurl) {
  content::WebContents* web_contents =
      content::WebContents::FromRenderFrameHost(rfh);
  if (!web_contents) {
    return;
  }

  web_contents->TrigAdBlockEnabledForSiteFromUi(gurl.spec());
}
#endif