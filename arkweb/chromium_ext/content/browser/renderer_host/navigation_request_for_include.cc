/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "components/embedder_support/arkweb_version.h"

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
void NavigationRequest::StartNavigationExt() {
  bool is_incognito = false;
  WebContents* web_contents =
      WebContents::FromFrameTreeNodeId(frame_tree_node_->frame_tree_node_id());
  if (web_contents && web_contents->GetBrowserContext()) {
    is_incognito = web_contents->GetBrowserContext()->IsOffTheRecord();
  }
  LOG_FEEDBACK(INFO, kNavigation)
      << "NavigationRequestBeginNavigation isBrowserInitiated:"
      << commit_params_->is_browser_initiated
      << " isServerRedirect:" << was_redirected_
      << " isIncognito:" << is_incognito << " navigationId:" << navigation_id_
      << " navigationToken:" << devtools_navigation_token_.ToString() << " url:"
      << url::LogUtils::ConvertUrlWithMask(common_params_->url.spec());

  if (!is_incognito) {
    int32_t usage_scenario =
        web_contents->GetOrCreateWebPreferences().usage_scenario;
    LOG(URL) << "start a navigation url: "
             << common_params_->url.spec()
             << " is_browser_initiated_: "
             << commit_params_->is_browser_initiated
             << " was_redirected_: " << was_redirected_
             << " arkweb_version: " << ARKWEB_VERSION_FULL;
  }
}
#endif

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
bool NavigationRequest::NeedsReloadWithFallbackProxy() {
  return needs_reload_with_fallback_proxy_;
}

ErrorPageReloadReason  NavigationRequest::GetCurrentReloadReason() {
  return current_reload_reason_;
}

int NavigationRequest::GetOriginalNetErrorCode() {
  return original_error_code_;
}

bool NavigationRequest::HasBeenReloadedForThisReason(
    ErrorPageReloadReason  reason) {
  if (reload_reason_list_.find(reason) == reload_reason_list_.end()) {
    return false;
  }

  return true;
}
#endif  // BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
bool NavigationRequest::IsAutoReload() {
  return current_reload_reason_ > ErrorPageReloadReason::INVALID;
}

void NavigationRequest::OnReportNewNavigationInfo(
    const std::string& page_trace_id) {
  if (nav_request_utils_) {
    nav_request_utils_->OnReportNewNavigationInfo(page_trace_id);
  }
}
#endif
