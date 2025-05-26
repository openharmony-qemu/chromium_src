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
#include "content/public/browser/back_forward_cache.h"

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
#include "arkweb/chromium_ext/content/browser/renderer_host/arkweb_render_process_host_impl_utils.h"
#include "content/browser/renderer_host/render_process_host_impl.h"
#endif  // BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)

namespace content {

void ArkWebUnloadOldFrame(const BackForwardCache& back_forward_cache,
                          const std::string reason,
                          bool& can_store) {
#if BUILDFLAG(ARKWEB_BFCACHE)
  if (back_forward_cache.ArkWebGetCacheSize() <= 0 ||
      back_forward_cache.ArkWebGetTimeToLive() <= 0) {
    can_store = false;
  }
  LOG(INFO) << "[BFCACHE] UnloadOldFrame can_store: " << can_store
            << " bfcache_eligibility.flattened_reasons:" << reason;
#endif
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO) << "RenderFrameHostManager::UnloadOldFrame the value of "
                        "bfcache_eligibility.flattened_reasons is: "
                     << reason;
#endif  // BUILDFLAG(ARKWEB_LOGGER_REPORT)
}

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
