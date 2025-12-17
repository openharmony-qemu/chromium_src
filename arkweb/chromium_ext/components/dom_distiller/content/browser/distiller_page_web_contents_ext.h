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

#ifndef COMPONENTS_DOM_DISTILLER_CONTENT_BROWSER_DISTILLER_PAGE_WEB_CONTENTS_EXT_H_
#define COMPONENTS_DOM_DISTILLER_CONTENT_BROWSER_DISTILLER_PAGE_WEB_CONTENTS_EXT_H_

#include <string>

#include "components/dom_distiller/content/browser/distiller_page_web_contents.h"
#include "components/dom_distiller/core/distiller_page.h"
#include "content/public/browser/render_frame_host.h"
#if BUILDFLAG(ARKWEB_READER_MODE)
#include "base/timer/timer.h"
#endif  // ARKWEB_READER_MODE

namespace dom_distiller {

class DistillerPageWebContents;
class DistillerPageWebContentsExt : public DistillerPageWebContents {
 public:
  DistillerPageWebContentsExt(content::BrowserContext* browser_context,
                              const gfx::Size& render_view_size,
                              std::unique_ptr<SourcePageHandleWebContents>
                                  optional_web_contents_handle);
  ~DistillerPageWebContentsExt() override;

  DistillerPageWebContentsExt* AsExt() override { return this; }

#if BUILDFLAG(ARKWEB_READER_MODE)
  void DistillPageImplExt(const GURL& url);
  void DistillPageImplEndExt();
  bool DOMContentLoadedExt(content::RenderFrameHost* render_frame_host);
  bool ExecuteJavaScriptExt();
  bool OnWebContentsDistillationDoneExt();
  
  void SetDistillOptions(const DistillOptions& options) override;
  const DistillOptions* GetDistillOptions() override;
  void DidFinishLoad(content::RenderFrameHost* render_frame_host,
                     const GURL& validated_url) override;
  void AbortDistill() override;
  void LoadProgressChanged(double progress) override;
  void DidFinishNavigation(
      content::NavigationHandle* navigation_handle) override;
  void OnWebContentsDistillationFailed(const std::string& error_message);
  void UpdateWebContentCreateParam(content::WebContents::CreateParams& param);

#if BUILDFLAG(ARKWEB_USERAGENT)
  void DidStartNavigation(content::NavigationHandle* navigation_handle) override;
  void DidRedirectNavigation(content::NavigationHandle* navigation_handle) override;
#endif // ARKWEB_USERAGENT

  static std::unique_ptr<content::WebContents> resident_web_contents_;
#endif  // ARKWEB_READER_MODE

 private:
#if BUILDFLAG(ARKWEB_READER_MODE)
  bool IsForDistillerPage() override;
  void DistillTimeout();

  DistillOptions distill_options_{};
  std::unique_ptr<base::OneShotTimer> distiller_timer_;
  bool distill_finished_{false};
  base::TimeDelta distiller_timeout_;
  bool is_source_webcontents_{false};
  std::string http_reason_phrase_;
  bool need_cancel_distill_for_http_error_{false};
#endif  // ARKWEB_READER_MODE
  base::WeakPtrFactory<DistillerPageWebContentsExt> weak_ptr_factory_{this};
};

}  // namespace dom_distiller
#endif  // COMPONENTS_DOM_DISTILLER_CONTENT_BROWSER_DISTILLER_PAGE_WEB_CONTENTS_EXT_H_