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

#include "arkweb/chromium_ext/components/subresource_filter/content/browser/profile_interaction_manager_for_include.h"

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "content/browser/renderer_host/frame_tree_node.h"
#include "content/browser/renderer_host/render_frame_host_impl.h"
#endif

namespace subresource_filter {

mojom::ActivationLevel OnPageActivationComputedExt(
    content::NavigationHandle* navigation_handle,
    mojom::ActivationLevel initial_activation_level) {
  LOG(DEBUG) << "[Adblock] OnPageActivationComputed url : ***";
  CHECK(IsInSubresourceFilterRoot(navigation_handle),
        base::NotFatalUntil::M129);

  mojom::ActivationLevel effective_activation_level = initial_activation_level;
  const GURL& url(navigation_handle->GetURL());
  if (url.SchemeIsHTTPOrHTTPS()) {
    effective_activation_level =
        subresource_filter::mojom::ActivationLevel::kEnabled;
  }

  content::RenderFrameHost* rfh = navigation_handle->GetRenderFrameHost();

  if (rfh == NULL) {
    LOG(DEBUG) << "[Adblock] OnPageActivationComputed url22 : ***"
               << effective_activation_level;
    return subresource_filter::mojom::ActivationLevel::kDisabled;
  } else {
    content::FrameTreeNode* node =
        static_cast<content::RenderFrameHostImpl*>(rfh)->frame_tree_node();

    if (node == NULL || !node->is_adblock_enabled() ||
        !navigation_handle->GetWebContents()->IsAdsBlockEnabled()) {
      LOG(DEBUG) << "[Adblock] OnPageActivationComputed url11 : ***"
                 << effective_activation_level;
      return subresource_filter::mojom::ActivationLevel::kDisabled;
    }
  }
  LOG(DEBUG) << "[Adblock] OnPageActivationComputed url : ***"
             << effective_activation_level;
  return effective_activation_level;
}

bool ProfileInteractionManagerUtil::OnAdsViolationTriggeredExt(
    raw_ptr<ProfileInteractionManager> ProfileInteractionManagerObj,
    content::RenderFrameHost* rfh,
    mojom::AdsViolation& triggered_violation) {
  if (ProfileInteractionManagerObj->profile_context_) {
    const GURL& url = rfh->GetLastCommittedURL();
    std::optional<AdsInterventionManager::LastAdsIntervention>
        last_intervention = ProfileInteractionManagerObj->profile_context_
                                ->ads_intervention_manager()
                                ->GetLastAdsIntervention(url);
    // TODO(crbug.com/40721691): If a host triggers multiple times on a single
    // navigate and the durations don't match, we'll use the last duration
    // rather than the longest. The metadata should probably store the
    // activation with the longest duration.
    if (last_intervention &&
        last_intervention->duration_since <
            AdsInterventionManager::GetInterventionDuration(
                last_intervention->ads_violation)) {
      return false;
    }

    ProfileInteractionManagerObj->profile_context_->ads_intervention_manager()
        ->TriggerAdsInterventionForUrlOnSubsequentLoads(url,
                                                        triggered_violation);
  }
  return true;
}

}  // namespace subresource_filter
