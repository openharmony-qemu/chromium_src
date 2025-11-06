// Copyright 2020 The Chromium Embedded Framework Authors.
// Portions copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cef/libcef/renderer/chrome/chrome_content_renderer_client_cef.h"

#include "cef/libcef/renderer/blink_glue.h"
#include "cef/libcef/renderer/render_frame_observer.h"
#include "cef/libcef/renderer/render_manager.h"
#include "cef/libcef/renderer/thread_util.h"
#include "chrome/renderer/printing/chrome_print_render_frame_helper_delegate.h"
#include "content/public/renderer/render_frame.h"
#include "content/public/renderer/render_thread.h"
#include "third_party/blink/public/web/web_view.h"

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
#include "components/no_state_prefetch/renderer/no_state_prefetch_client.h"
#endif  // ARKWEB_NO_STATE_PREFETCH

#if BUILDFLAG(ARKWEB_MIXED_CONTENT)
#include "cef/ohos_cef_ext/libcef/renderer/arkweb_content_settings_client.h"
#endif

#if BUILDFLAG(ARKWEB_SAFEBROWSING) && !BUILDFLAG(ARKWEB_NWEB_EX)
#include "cef/ohos_cef_ext/libcef/renderer/chrome_safe_browsing_error_page_controller_delegate_impl.h"
#endif

ChromeContentRendererClientCef::ChromeContentRendererClientCef()
    : render_manager_(new CefRenderManager) {}

ChromeContentRendererClientCef::~ChromeContentRendererClientCef() = default;

scoped_refptr<base::SingleThreadTaskRunner>
ChromeContentRendererClientCef::GetCurrentTaskRunner() {
  // Check if currently on the render thread.
  if (CEF_CURRENTLY_ON_RT()) {
    return render_task_runner_;
  }
  return nullptr;
}

void ChromeContentRendererClientCef::RenderThreadStarted() {
  ChromeContentRendererClient::RenderThreadStarted();

  render_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
}

void ChromeContentRendererClientCef::RenderThreadConnected() {
  ChromeContentRendererClient::RenderThreadConnected();

  render_manager_->RenderThreadConnected();
}

void ChromeContentRendererClientCef::RenderFrameCreated(
    content::RenderFrame* render_frame) {
  // Will delete itself when no longer needed.
  CefRenderFrameObserver* render_frame_observer =
      new CefRenderFrameObserver(render_frame);

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  AsArkWebContentRendererClientCef()->NoStatePrefetchRenderFrame(render_frame);
#endif  // ARKWEB_NO_STATE_PREFETCH

  bool browser_created;
  std::optional<cef::BrowserConfig> config;
  render_manager_->RenderFrameCreated(render_frame, render_frame_observer,
                                      browser_created, config);
  if (browser_created) {
    CHECK(config.has_value());
    OnBrowserCreated(render_frame->GetWebView(), *config);
  }

  if (config.has_value()) {
    // This value will be used when the ChromeContentRendererClient
    // creates the new ChromePrintRenderFrameHelperDelegate below.
    ChromePrintRenderFrameHelperDelegate::SetNextPrintPreviewEnabled(
        (*config).print_preview_enabled);
  }

  ChromeContentRendererClient::RenderFrameCreated(render_frame);

#if BUILDFLAG(ARKWEB_MIXED_CONTENT)
  new ArkWebContentSettingsClient(render_frame);
#endif

#if BUILDFLAG(ARKWEB_SAFEBROWSING) && !BUILDFLAG(ARKWEB_NWEB_EX)
  new ChromeSafeBrowsingErrorPageControllerDelegateImpl(render_frame);
#endif
}

void ChromeContentRendererClientCef::WebViewCreated(
    blink::WebView* web_view,
    bool was_created_by_renderer,
    const url::Origin* outermost_origin) {
  ChromeContentRendererClient::WebViewCreated(web_view, was_created_by_renderer,
                                              outermost_origin);

  bool browser_created;
  std::optional<cef::BrowserConfig> config;
  render_manager_->WebViewCreated(web_view, browser_created, config);
  if (browser_created) {
    CHECK(config.has_value());
    OnBrowserCreated(web_view, *config);
  }

#if BUILDFLAG(ARKWEB_NO_STATE_PREFETCH)
  new prerender::NoStatePrefetchClient(web_view);
#endif  // ARKWEB_NO_STATE_PREFETCH
}

void ChromeContentRendererClientCef::DevToolsAgentAttached() {
  // WebWorkers may be creating agents on a different thread.
  if (!render_task_runner_->BelongsToCurrentThread()) {
    render_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&ChromeContentRendererClientCef::DevToolsAgentAttached,
                       base::Unretained(this)));
    return;
  }

  render_manager_->DevToolsAgentAttached();
}

void ChromeContentRendererClientCef::DevToolsAgentDetached() {
  // WebWorkers may be creating agents on a different thread.
  if (!render_task_runner_->BelongsToCurrentThread()) {
    render_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&ChromeContentRendererClientCef::DevToolsAgentDetached,
                       base::Unretained(this)));
    return;
  }

  render_manager_->DevToolsAgentDetached();
}

void ChromeContentRendererClientCef::ExposeInterfacesToBrowser(
    mojo::BinderMap* binders) {
  ChromeContentRendererClient::ExposeInterfacesToBrowser(binders);

  render_manager_->ExposeInterfacesToBrowser(binders);
}

void ChromeContentRendererClientCef::OnBrowserCreated(
    blink::WebView* web_view,
    const cef::BrowserConfig& config) {
#if BUILDFLAG(IS_MAC)
  blink_glue::SetUseExternalPopupMenus(web_view, !config.is_windowless);
#endif
  web_view->SetMovePictureInPictureEnabled(config.move_pip_enabled);
}
