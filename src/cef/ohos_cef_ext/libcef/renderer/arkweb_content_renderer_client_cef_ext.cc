// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cef/libcef/renderer/chrome/chrome_content_renderer_client_cef.h"
#include "cef/ohos_cef_ext/libcef/renderer/arkweb_content_renderer_client_cef_ext.h"

#include "arkweb/build/features/features.h"
#include "cef/libcef/renderer/render_frame_observer.h"
#include "cef/libcef/renderer/render_manager.h"
#include "cef/libcef/renderer/thread_util.h"
#include "chrome/renderer/printing/chrome_print_render_frame_helper_delegate.h"
#include "content/public/renderer/render_frame.h"
#include "content/public/renderer/render_thread.h"
#include "third_party/blink/public/web/web_view.h"

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
#include "components/network_hints/renderer/web_prescient_networking_impl.h"
#include "components/no_state_prefetch/renderer/no_state_prefetch_client.h"
#include "components/no_state_prefetch/renderer/no_state_prefetch_helper.h"
#include "components/no_state_prefetch/renderer/no_state_prefetch_render_frame_observer.h"
#endif  // ARKWEB_NO_STATE_PREFETCH

#if BUILDFLAG(ARKWEB_PRINT)
#include "libcef/renderer/extensions/ohos_print_render_frame_helper_delegate.h"
#endif  // BUILDFLAG(ARKWEB_PRINT)

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
#include "libcef/renderer/chrome_safe_browsing_error_page_controller_delegate_impl.h"
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#include "third_party/blink/public/web/web_local_frame.h"
#endif

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
#include "components/js_injection/renderer/js_communication.h"
#endif

#if BUILDFLAG(ARKWEB_MIXED_CONTENT)
#include "cef/ohos_cef_ext/libcef/renderer/arkweb_content_settings_client.h"
#endif

#include "arkweb/chromium_ext/components/js_injection/renderer/js_communication_utils.h"

ArkWebContentRendererClientCefExt::ArkWebContentRendererClientCefExt()
    : ChromeContentRendererClientCef() {}

ArkWebContentRendererClientCefExt::~ArkWebContentRendererClientCefExt() =
    default;

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
void ArkWebContentRendererClientCefExt::NoStatePrefetchRenderFrame(
    content::RenderFrame* render_frame) {
  new prerender::NoStatePrefetchRenderFrameObserver(render_frame);
  if (!render_frame->IsMainFrame()) {
    auto* main_frame_no_state_prefetch_helper =
        prerender::NoStatePrefetchHelper::Get(
            render_frame->GetMainRenderFrame());
    if (main_frame_no_state_prefetch_helper) {
      // Avoid any race conditions from having the browser tell subframes that
      // they're no-state prefetching.
      new prerender::NoStatePrefetchHelper(
          render_frame,
          main_frame_no_state_prefetch_helper->histogram_prefix());
    }
  }
}
#endif  // ARKWEB_NO_STATE_PREFETCH

#if BUILDFLAG(ARKWEB_SAFEBROWSING) && !BUILDFLAG(ARKWEB_NWEB_EX)
void ArkWebContentRendererClientCefExt::PrepareErrorPage(
    content::RenderFrame* render_frame,
    const blink::WebURLError& error,
    const std::string& http_method,
    content::mojom::AlternativeErrorPageOverrideInfoPtr
        alternative_error_page_info,
    std::string* error_html) {
  ChromeSafeBrowsingErrorPageControllerDelegateImpl::Get(render_frame)
      ->PrepareForErrorPage();
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
bool ArkWebContentRendererClientCefExt::HandleNavigation(
    content::RenderFrame* render_frame,
    blink::WebFrame* frame,
    const blink::WebURLRequest& request,
    blink::WebNavigationType type,
    blink::WebNavigationPolicy default_policy,
    bool is_redirect) {
  // Only GETs can be overridden.
  if (!request.HttpMethod().Equals("GET")) {
    return false;
  }

  // Any navigation from loadUrl, and goBack/Forward are considered application-
  // initiated and hence will not yield a shouldOverrideUrlLoading() callback.
  // Webview classic does not consider reload application-initiated so we
  // continue the same behavior.
  bool application_initiated = type == blink::kWebNavigationTypeBackForward;

  // Don't offer application-initiated navigations unless it's a redirect.
  if (application_initiated && !is_redirect) {
    return false;
  }

  bool is_outermost_main_frame = frame->IsOutermostMainFrame();
  const GURL& gurl = request.Url();
  // For HTTP schemes, only top-level navigations can be overridden. Similarly,
  // WebView Classic lets app override only top level about:blank navigations.
  // So we filter out non-top about:blank navigations here.
  if (!is_outermost_main_frame &&
      (gurl.SchemeIs(url::kHttpScheme) || gurl.SchemeIs(url::kHttpsScheme) ||
       gurl.SchemeIs(url::kAboutScheme))) {
    return false;
  }

  blink::WebView* web_view = render_frame->GetWebView();

  bool browser_created;
  std::optional<bool> is_windowless;
  std::optional<bool> print_preview_enabled;
  render_manager_->WebViewCreated(web_view, browser_created, is_windowless,
                                  print_preview_enabled);
  if (!browser_created) {
    return false;
  }

  blink::WebLocalFrame* local_frame = render_frame->GetWebFrame();
  CefRefPtr<CefBrowserImpl> browserPtr =
      CefBrowserImpl::GetBrowserForMainFrame(local_frame->Top());

  if (!browserPtr) {
    return false;
  }

  bool ignore_navigation = false;
  bool has_user_gesture = request.HasUserGesture();
  CefRefPtr<CefFrameImpl> framePtr = browserPtr->GetWebFrameImpl(local_frame);

  ignore_navigation = framePtr->ShouldOverrideUrlLoading(
      gurl.possibly_invalid_spec(), request.HttpMethod().Utf8(),
      has_user_gesture, is_redirect, is_outermost_main_frame);

  return ignore_navigation;
}
#endif

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
std::unique_ptr<blink::WebPrescientNetworking>
ArkWebContentRendererClientCefExt::CreatePrescientNetworking(
    content::RenderFrame* render_frame) {
  return std::make_unique<network_hints::WebPrescientNetworkingImpl>(
      render_frame);
}

bool ArkWebContentRendererClientCefExt::IsPrefetchOnly(
    content::RenderFrame* render_frame) {
  return prerender::NoStatePrefetchHelper::IsPrefetching(render_frame);
}
#endif  // BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)

void ArkWebContentRendererClientCefExt::RunScriptsAtDocumentStart(
    content::RenderFrame* render_frame) {
  ChromeContentRendererClient::RunScriptsAtDocumentStart(render_frame);
#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
  js_injection::JsCommunication* communication =
      js_injection::JsCommunication::Get(render_frame);
  if (communication) {
    communication->RunScriptsAtDocumentStart();
  }
#endif
}

void ArkWebContentRendererClientCefExt::RunScriptsAtDocumentEnd(
    content::RenderFrame* render_frame) {
  ChromeContentRendererClient::RunScriptsAtDocumentEnd(render_frame);
#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
  js_injection::JsCommunication* communication =
      js_injection::JsCommunication::Get(render_frame);
  if (communication) {
    communication->implUtils_->RunScriptsAtDocumentEnd();
  }
#endif  // ARKWEB_JS_ON_DOCUMENT_END
}

#if BUILDFLAG(ARKWEB_JSPROXY)
void ArkWebContentRendererClientCefExt::RunScriptsAtHeadReady(
    content::RenderFrame* render_frame) {
  js_injection::JsCommunication* communication =
      js_injection::JsCommunication::Get(render_frame);

  if (communication) {
    communication->implUtils_->RunScriptsAtHeadReady();
  }
}
#endif
