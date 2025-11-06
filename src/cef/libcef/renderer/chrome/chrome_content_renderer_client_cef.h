// Copyright 2020 The Chromium Embedded Framework Authors.
// Portions copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_RENDERER_CHROME_CHROME_CONTENT_RENDERER_CLIENT_CEF_
#define CEF_LIBCEF_RENDERER_CHROME_CHROME_CONTENT_RENDERER_CLIENT_CEF_

#include <memory>

#include "arkweb/build/features/features.h"
#include "base/memory/scoped_refptr.h"
#include "base/task/single_thread_task_runner.h"
#include "chrome/renderer/chrome_content_renderer_client.h"

namespace cef {
struct BrowserConfig;
}

class CefRenderManager;
class ArkWebContentRendererClientCefExt;

// CEF override of ChromeContentRendererClient.
#if BUILDFLAG(ARKWEB_ADBLOCK) || BUILDFLAG(ARKWEB_NETWORK_BASE) || \
    BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
class ChromeContentRendererClientCef
    : public ArkWebChromeContentRendererClientExt {
#else
class ChromeContentRendererClientCef : public ChromeContentRendererClient {
#endif
 public:
  ChromeContentRendererClientCef();

  ChromeContentRendererClientCef(const ChromeContentRendererClientCef&) =
      delete;
  ChromeContentRendererClientCef& operator=(
      const ChromeContentRendererClientCef&) = delete;

  ~ChromeContentRendererClientCef() override;

  virtual ArkWebContentRendererClientCefExt*
  AsArkWebContentRendererClientCef() {
    return nullptr;
  }

  // Render thread task runner.
  base::SingleThreadTaskRunner* render_task_runner() const {
    return render_task_runner_.get();
  }

  // Returns the task runner for the current thread. Returns NULL if the current
  // thread is not the main render process thread.
  scoped_refptr<base::SingleThreadTaskRunner> GetCurrentTaskRunner();

  // ChromeContentRendererClient overrides.
  void RenderThreadStarted() override;
  void RenderThreadConnected() override;
  void RenderFrameCreated(content::RenderFrame* render_frame) override;
  void WebViewCreated(blink::WebView* web_view,
                      bool was_created_by_renderer,
                      const url::Origin* outermost_origin) override;
  void DevToolsAgentAttached() override;
  void DevToolsAgentDetached() override;
  void ExposeInterfacesToBrowser(mojo::BinderMap* binders) override;

 private:
  friend class ArkWebContentRendererClientCefExt;
  void OnBrowserCreated(blink::WebView* web_view,
                        const cef::BrowserConfig& config);

  std::unique_ptr<CefRenderManager> render_manager_;

  scoped_refptr<base::SingleThreadTaskRunner> render_task_runner_;
};

#include "cef/ohos_cef_ext/libcef/renderer/arkweb_content_renderer_client_cef_ext.h"

#endif  // CEF_LIBCEF_RENDERER_CHROME_CHROME_CONTENT_RENDERER_CLIENT_CEF_
