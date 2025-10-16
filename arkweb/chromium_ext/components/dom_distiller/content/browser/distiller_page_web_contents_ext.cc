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

#include "arkweb/chromium_ext/components/dom_distiller/content/browser/distiller_page_web_contents_ext.h"

#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"
#include "url/gurl.h"
#if BUILDFLAG(ARKWEB_READER_MODE)
#include "arkweb/chromium_ext/url/ohos/log_utils.h"
#include "arkweb/ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_reader_mode_config.h"
#include "base/logging.h"
#include "content/public/browser/navigation_handle.h"
#include "net/http/http_response_headers.h"
#include "net/http/http_status_code.h"
#include "third_party/dom_distiller_js/dom_distiller_json_converter.h"

#if BUILDFLAG(ARKWEB_USERAGENT)
#include "cef/ohos_cef_ext/libcef/browser/useragent/arkweb_useragent_utils.h"
#endif // ARKWEB_USERAGENT

#endif  // ARKWEB_READER_MODE

namespace dom_distiller {

#if BUILDFLAG(ARKWEB_READER_MODE)
constexpr int kNetErrorCode = -1;
constexpr base::TimeDelta kDistillTimeout = base::Seconds(10);
constexpr base::TimeDelta kLoadAndDistillTimeout = base::Seconds(30);
constexpr std::string kSharedRenderProcessToken = "0xAAAAAA";

std::unique_ptr<content::WebContents>
    DistillerPageWebContentsExt::resident_web_contents_ = nullptr;
#endif  // ARKWEB_READER_MODE

DistillerPageWebContentsExt::DistillerPageWebContentsExt(
    content::BrowserContext* browser_context,
    const gfx::Size& render_view_size,
    std::unique_ptr<SourcePageHandleWebContents> optional_web_contents_handle)
    : DistillerPageWebContents(browser_context,
                               render_view_size,
                               std::move(optional_web_contents_handle)) {}

DistillerPageWebContentsExt::~DistillerPageWebContentsExt() {
#if BUILDFLAG(ARKWEB_READER_MODE)
  if (!is_source_webcontents_ && !resident_web_contents_ &&
      source_page_handle_ && source_page_handle_->web_contents()) {
    resident_web_contents_.reset(source_page_handle_->web_contents());
  }
#endif // ARKWEB_READER_MODE
}

#if BUILDFLAG(ARKWEB_READER_MODE)
void DistillerPageWebContentsExt::DistillPageImplExt(const GURL& url) {
  http_reason_phrase_.clear();
  distill_finished_ = false;
  need_cancel_distill_for_http_error_ = false;

  if (source_page_handle_ && source_page_handle_->web_contents() &&
      TargetRenderFrameHost().GetLastCommittedURL() == url) {
    is_source_webcontents_ = true;
    distiller_timeout_ = kDistillTimeout;
  } else {
    is_source_webcontents_ = false;
    distiller_timeout_ = kLoadAndDistillTimeout;
    if (const auto* config_data = nweb_ex::AlloyBrowserReaderModeConfig::GetInstance()->GetReaderModeConfigData()) {
      distiller_timeout_ =
          base::Seconds(config_data->content_config.meta_data.distill_timeout);
    }
  }
}

void DistillerPageWebContentsExt::DistillPageImplEndExt() {
  distiller_timer_ = std::make_unique<base::OneShotTimer>();
  distiller_timer_->Start(
      FROM_HERE, distiller_timeout_,
      base::BindOnce(&DistillerPageWebContentsExt::DistillTimeout,
                     weak_ptr_factory_.GetWeakPtr()));
}

bool DistillerPageWebContentsExt::DOMContentLoadedExt(
    content::RenderFrameHost* render_frame_host) {
  if (render_frame_host == &TargetRenderFrameHost()) {
    LOG(INFO) << __func__ << " [Distiller]";
    if (!http_reason_phrase_.empty() || render_frame_host->IsErrorDocument()) {
      std::string reason =
          http_reason_phrase_.empty()
              ? net::ErrorToShortString(render_frame_host->GetNetErrorCode())
              : http_reason_phrase_;
      OnWebContentsDistillationFailed(reason);
      return true;
    }
  }
  return false;
}

bool DistillerPageWebContentsExt::ExecuteJavaScriptExt() {
  if (distill_finished_) {
    return true;
  }
  if (!is_source_webcontents_) {
    content::WebContentsObserver::Observe(nullptr);
    // Stop any pending navigation since the intent is to distill the current
    // page.
    source_page_handle_->web_contents()->Stop();
  }
  LOG(INFO) << __func__ << " [Distiller]";
  CHECK_EQ(LOADING_PAGE, state_);
  state_ = EXECUTING_JAVASCRIPT;
  return false;
}

bool DistillerPageWebContentsExt::OnWebContentsDistillationDoneExt() {
  if (distiller_timer_ && distiller_timer_->IsRunning()) {
    distiller_timer_->Stop();
  }
  if (distill_finished_) {
    return true;
  }
  distill_finished_ = true;
  return false;
}

void DistillerPageWebContentsExt::SetDistillOptions(
    const DistillOptions& options) {
  distill_options_ = options;
}

const DistillOptions* DistillerPageWebContentsExt::GetDistillOptions() {
  return &distill_options_;
}

void DistillerPageWebContentsExt::DidFinishLoad(
    content::RenderFrameHost* render_frame_host,
    const GURL& validated_url) {
  if (render_frame_host == &TargetRenderFrameHost() && state_ == LOADING_PAGE) {
    if (need_cancel_distill_for_http_error_) {
      std::string reason =
          http_reason_phrase_.empty()
              ? net::ErrorToShortString(render_frame_host->GetNetErrorCode())
              : http_reason_phrase_;
      OnWebContentsDistillationFailed(reason);
      LOG(INFO) << __func__ << " [Distiller] cancel distill for http_error: " << reason;
      return;
    }
    LOG(INFO) << __func__ << " [Distiller]";
    ExecuteJavaScript();
  }
}

void DistillerPageWebContentsExt::AbortDistill() {
  LOG(INFO) << __func__ << " [Distiller]";
  content::WebContentsObserver::Observe(nullptr);
  if (!is_source_webcontents_ && source_page_handle_ && source_page_handle_->web_contents()) {
    source_page_handle_->web_contents()->Stop();
  }
}

void DistillerPageWebContentsExt::LoadProgressChanged(double progress) {
  LOG(INFO) << __func__ << " [Distiller] progress:" << progress;
}

void DistillerPageWebContentsExt::DistillTimeout() {
  LOG(INFO) << __func__ << " [Distiller] state:" << state_;
  std::string error_message("distill timeout");
  OnWebContentsDistillationFailed(error_message);
}

void DistillerPageWebContentsExt::OnWebContentsDistillationFailed(
    const std::string& error_message) {
  LOG(INFO) << __func__ << " [Distiller] error_message:" << error_message;
  proto::DomDistillerResult result;
  result.mutable_distilled_result()->set_code(kNetErrorCode);
  result.mutable_distilled_result()->set_message(error_message);
  OnWebContentsDistillationDone(
      GURL(), base::TimeTicks(),
      dom_distiller::proto::json::DomDistillerResult::WriteToValue(result));
}

void DistillerPageWebContentsExt::UpdateWebContentCreateParam(content::WebContents::CreateParams& param) {
  param.shared_render_process_token = kSharedRenderProcessToken;
}

void DistillerPageWebContentsExt::DidFinishNavigation(
    content::NavigationHandle* navigation_handle) {
  if (!navigation_handle->IsInPrimaryMainFrame()) {
    return;
  }
  http_reason_phrase_.clear();
  need_cancel_distill_for_http_error_ = false;
  net::HttpStatusCode response_code = net::HttpStatusCode::HTTP_OK;
  if (navigation_handle->GetResponseHeaders()) {
    response_code = static_cast<net::HttpStatusCode>(
        navigation_handle->GetResponseHeaders()->response_code());
  }
  if (response_code != net::HttpStatusCode::HTTP_OK) {
    need_cancel_distill_for_http_error_ = true;
    http_reason_phrase_ =
        base::StringPrintf("%d %s", static_cast<int>(response_code),
                           net::GetHttpReasonPhrase(response_code));
    LOG(INFO) << __func__ << " [Distiller] http_reason_phrase_: "
              << http_reason_phrase_;
  }
}

bool DistillerPageWebContentsExt::IsForDistillerPage() {
  return true;
}

#if BUILDFLAG(ARKWEB_USERAGENT)
void DistillerPageWebContentsExt::DidStartNavigation(content::NavigationHandle* navigation_handle) {
  arkweb_useragent_utils::MaybeOverrideUserAgentOnStartNavigation(navigation_handle);
}

void DistillerPageWebContentsExt::DidRedirectNavigation(content::NavigationHandle* navigation_handle) {
  arkweb_useragent_utils::MaybeOverrideUserAgentOnRedirectNavigation(navigation_handle);
}
#endif // ARKWEB_USERAGENT

#endif  // ARKWEB_READER_MODE
}  // namespace dom_distiller
