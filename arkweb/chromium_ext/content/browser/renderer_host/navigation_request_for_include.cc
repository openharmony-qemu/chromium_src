/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
void NavigationRequest::StartNavigationExt() {
  LOG_FEEDBACK(INFO) << "INFO: start a navigation url: "
                     << url::LogUtils::ConvertUrlWithMask(
                            common_params_->url.spec())
                     << " is_browser_initiated_: "
                     << commit_params_->is_browser_initiated
                     << " was_redirected_: " << was_redirected_;
  bool is_incognito = false;
  WebContents* web_contents =
      WebContents::FromFrameTreeNodeId(frame_tree_node_->frame_tree_node_id());
  if (web_contents && web_contents->GetBrowserContext()) {
    is_incognito = web_contents->GetBrowserContext()->IsOffTheRecord();
  }
  if (!is_incognito) {
    int32_t usage_scenario =
        web_contents->GetOrCreateWebPreferences().usage_scenario;
    LOG(URL) << "start a navigation url: "
             << common_params_->url.spec()
             << " is_browser_initiated_: "
             << commit_params_->is_browser_initiated
             << " was_redirected_: " << was_redirected_;
  }
}
#endif