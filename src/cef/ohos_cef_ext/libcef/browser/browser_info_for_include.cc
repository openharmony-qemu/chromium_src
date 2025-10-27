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

#include "cef/ohos_cef_ext/libcef/browser/arkweb_frame_host_impl_ext.h"

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#include "content/browser/renderer_host/frame_tree.h"
#include "content/public/common/content_switches.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
content::GlobalRenderFrameHostId
CefBrowserInfo::GetLastDeleteSpeculativeRFHId() {
  return last_delete_speculative_rfh_id_;
}

content::GlobalRenderFrameHostToken
CefBrowserInfo::GetLastDeleteSpeculativeRFHToken() {
  return last_delete_speculative_rfh_token_;
}

bool CefBrowserInfo::IsPrerendering(content::RenderFrameHost* host) {
  // Prerendering is only supported when NwebEx is enabled.
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
      ::switches::kEnableNwebEx)) {
    return false;
  }

  if (!host) {
    return false;
  }

  return static_cast<content::RenderFrameHostImpl*>(host)
          ->frame_tree()->is_prerendering();
}

#endif