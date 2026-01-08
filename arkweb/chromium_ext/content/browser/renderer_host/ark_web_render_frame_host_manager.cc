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

#include "arkweb/chromium_ext/content/browser/renderer_host/ark_web_render_frame_host_manager.h"

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "content/browser/renderer_host/navigation_request.h"
#include "content/browser/renderer_host/render_frame_host_impl.h"
#include "content/public/browser/back_forward_cache.h"

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
#include "arkweb/chromium_ext/content/browser/renderer_host/arkweb_render_process_host_impl_utils.h"
#include "content/browser/renderer_host/render_process_host_impl.h"
#endif  // BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)

namespace content {

// LCOV_EXCL_START
void ArkWebUnloadOldFrame(RenderFrameHostImpl* old_render_frame_host,
                          const BackForwardCache& back_forward_cache,
                          const std::string& reason,
                          bool& can_store) {
#if BUILDFLAG(ARKWEB_BFCACHE)
  std::string arkweb_reason;
  if (back_forward_cache.ArkWebGetCacheSize() <= 0 ||
      back_forward_cache.ArkWebGetTimeToLive() <= 0) {
    can_store = false;
  }
#endif
#if BUILDFLAG(ARKWEB_USERAGENT)
  bool has_diff_useragent =
      old_render_frame_host &&
      old_render_frame_host->GetUserAgentDifferentFromNavigatingFrame();
  if (has_diff_useragent) {
    can_store = false;
    old_render_frame_host->SetUserAgentDifferentFromNavigatingFrame(false);
  }
#endif

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO, kNavigation)
      << "CanEnterBFCache canStore:" << can_store
      << " cacheSize:" << back_forward_cache.ArkWebGetCacheSize()
      << " timeToLive:" << back_forward_cache.ArkWebGetTimeToLive()
      << "s hasDiffUserAgent:" << has_diff_useragent << " reason:" << reason;
#endif
}
// LCOV_EXCL_STOP

void ArkWebGetFrameHostForNavigation(
    const std::string& shared_render_process_token,
    SiteInstanceImpl* dest_site_instance) {
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
  if (!shared_render_process_token.empty()) {
    RenderProcessHost* render_process =
        ArkwebRenderProcessHostImplUtils::GetProcessForSharedToken(
            shared_render_process_token);
    if (render_process) {
      dest_site_instance->ReuseExistingProcessIfPossible(render_process);
    } else {
      ArkwebRenderProcessHostImplUtils::RegisteProcessForSharedToken(
          shared_render_process_token, dest_site_instance->GetProcess());
    }
  }
#endif  // BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
}

}  // namespace content
