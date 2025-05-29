// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "components/subresource_filter/content/browser/content_subresource_filter_throttle_manager.h"
#include "arkweb/chromium_ext/components/subresource_filter/content/browser/arkweb_content_subresource_filter_throttle_manager_ext.h"

#include <utility>

#include "base/check.h"
#include "base/check_op.h"
#include "base/containers/contains.h"
#include "base/debug/crash_logging.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/metrics/histogram_functions.h"
#include "base/metrics/histogram_macros.h"
#include "base/not_fatal_until.h"
#include "base/strings/stringprintf.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/traced_value.h"
#include "components/subresource_filter/content/browser/ad_tagging_utils.h"
#include "components/subresource_filter/content/browser/content_subresource_filter_web_contents_helper.h"
#include "components/subresource_filter/content/browser/profile_interaction_manager.h"
#include "components/subresource_filter/content/browser/safe_browsing_child_navigation_throttle.h"
#include "components/subresource_filter/content/browser/safe_browsing_page_activation_throttle.h"
#include "components/subresource_filter/content/mojom/subresource_filter.mojom.h"
#include "components/subresource_filter/content/shared/browser/activation_state_computing_navigation_throttle.h"
#include "components/subresource_filter/content/shared/browser/page_load_statistics.h"
#include "components/subresource_filter/content/shared/browser/utils.h"
#include "components/subresource_filter/content/shared/common/utils.h"
#include "components/subresource_filter/core/browser/async_document_subresource_filter.h"
#include "components/subresource_filter/core/browser/subresource_filter_constants.h"
#include "components/subresource_filter/core/browser/subresource_filter_features.h"
#include "components/subresource_filter/core/common/common_features.h"
#include "components/subresource_filter/core/common/constants.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/frame_tree_node_id.h"
#include "content/public/browser/global_routing_id.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/navigation_throttle.h"
#include "content/public/browser/page.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/url_utils.h"
#include "net/base/net_errors.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_provider.h"
#include "third_party/blink/public/common/features.h"
#include "third_party/blink/public/common/tokens/tokens.h"
#include "third_party/blink/public/mojom/devtools/console_message.mojom.h"
#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "components/subresource_filter/content/browser/ohos_adblock_config.h"
#endif

namespace subresource_filter {

ArkWebContentSubresourceFilterThrottleManagerExt::
    ArkWebContentSubresourceFilterThrottleManagerExt(
        SubresourceFilterProfileContext* profile_context,
        scoped_refptr<safe_browsing::SafeBrowsingDatabaseManager> database_manager,
        VerifiedRulesetDealer::Handle* dealer_handle,
        ContentSubresourceFilterWebContentsHelper& web_contents_helper,
        content::NavigationHandle& initiating_navigation_handle)
    : ContentSubresourceFilterThrottleManager(
        profile_context,
        database_manager,
        dealer_handle,
        web_contents_helper,
        initiating_navigation_handle) {}
  ArkWebContentSubresourceFilterThrottleManagerExt::~ArkWebContentSubresourceFilterThrottleManagerExt() = default;

#if BUILDFLAG(ARKWEB_ADBLOCK)
mojom::ActivationState ArkWebContentSubresourceFilterThrottleManagerExt::AdBlockActivationStateForNextCommittedLoad(
    content::NavigationHandle* navigation_handle,
    ActivationStateComputingNavigationThrottle* throttle) {
  mojom::ActivationLevel activation_level = mojom::ActivationLevel::kDisabled;
  LOG(DEBUG) << "[Adblock] navigation url : ***";

  subresource_filter::ActivationDecision decision;
  mojom::ActivationState state;
  state.activation_level =
      profile_interaction_manager_->OnPageActivationComputed(
          navigation_handle, activation_level, &decision);

  OHOS::adblock::AdBlockConfig::GetInstance()->ReadFromPrefService();
  state.user_subresource_filter_replace =
      OHOS::adblock::AdBlockConfig::GetInstance()
          ->GetUserEasylistReplaceSwitch();
  throttle->NotifyPageActivationWithRuleset(EnsureRulesetHandle(), state);

  if (state.activation_level == mojom::ActivationLevel::kDisabled) {
    return mojom::ActivationState();
  }
  throttle->WillSendActivationToRenderer();
  return state;
}

void ArkWebContentSubresourceFilterThrottleManagerExt::StatisticActivationState(
    content::NavigationHandle* navigation_handle) {
  if (!statistics_) {
    mojom::ActivationState state;
    state.activation_level = mojom::ActivationLevel::kEnabled;
    OHOS::adblock::AdBlockConfig::GetInstance()->ReadFromPrefService();
    state.user_subresource_filter_replace =
        OHOS::adblock::AdBlockConfig::GetInstance()
            ->GetUserEasylistReplaceSwitch();
    statistics_ = std::make_unique<PageLoadStatisticsExt>(
        state, navigation_handle->GetWebContents());
  }
}

void ArkWebContentSubresourceFilterThrottleManagerExt::ReportSubresourceMap(
    content::RenderFrameHost* render_frame_host, const GURL& validated_url) {
  // 数据上报 loads_disallowed_url_map 来mojom文件； 需要转换为std:map
  std::map<std::string, int32_t> subresource_map;
  for (auto& pair : statistics_->AsPageLoadStatisticsExt()->getAggregatedDocumentStatistics()
                        .loads_disallowed_url_map) {
    subresource_map.insert(pair);
  }

  for (auto& pair : statistics_->AsPageLoadStatisticsExt()->getUserAggregatedDocumentStatistics()
                        .loads_disallowed_url_map) {
    subresource_map.insert(pair);
  }

  LOG(INFO) << "[AdBlock] subresource map.size():" << subresource_map.size();

  if (subresource_map.size() > 0) {
    content::WebContents::FromRenderFrameHost(render_frame_host)
        ->OnAdsBlocked(validated_url.spec(), subresource_map,
                       statistics_->AsPageLoadStatisticsExt()->IsFirstReport());

    if (statistics_->AsPageLoadStatisticsExt()->IsFirstReport()) {
      statistics_->AsPageLoadStatisticsExt()->SetReported();
    }
  }
}

void ArkWebContentSubresourceFilterThrottleManagerExt::OnPageAdBlockActivationState(
    content::NavigationHandle* navigation_handle) {
  mojom::ActivationLevel activation_level = mojom::ActivationLevel::kDisabled;
  LOG(DEBUG) << "[Adblock] navigation url : ***";

  subresource_filter::ActivationDecision decision;
  mojom::ActivationState state;
  state.activation_level =
      profile_interaction_manager_->OnPageActivationComputed(
          navigation_handle, activation_level, &decision);

  OHOS::adblock::AdBlockConfig::GetInstance()->ReadFromPrefService();
  state.user_subresource_filter_replace =
      OHOS::adblock::AdBlockConfig::GetInstance()
          ->GetUserEasylistReplaceSwitch();

  auto it =
      ongoing_activation_throttles_.find(navigation_handle->GetNavigationId());
  if (it == ongoing_activation_throttles_.end())
    return;
  if (state.activation_level == mojom::ActivationLevel::kDisabled) {
    ongoing_activation_throttles_.erase(it);
    return;
  }

  it->second->NotifyPageActivationWithRuleset(EnsureRulesetHandle(), state);
  return;
}

void ArkWebContentSubresourceFilterThrottleManagerExt::AdBlockMaybeCreateActivationState(
    content::NavigationHandle* navigation_handle,
    std::unique_ptr<ActivationStateComputingNavigationThrottle>& throttle) {
  if (navigation_handle && !navigation_handle->IsDownload() &&
      navigation_handle->IsInMainFrame() &&
      navigation_handle->GetURL().SchemeIsHTTPOrHTTPS() &&
      navigation_handle->GetWebContents()) {
    LOG(DEBUG) << "[Adblock] navigation url: ***";

    mojom::ActivationState state;
    state.activation_level = mojom::ActivationLevel::kEnabled;

    OHOS::adblock::AdBlockConfig::GetInstance()->ReadFromPrefService();
    state.user_subresource_filter_replace =
        OHOS::adblock::AdBlockConfig::GetInstance()
            ->GetUserEasylistReplaceSwitch();

    throttle->NotifyPageActivationWithRuleset(EnsureRulesetHandle(), state);
  }
}

// static
void ArkWebContentSubresourceFilterThrottleManagerExt::BindUserReceiver(
    mojo::PendingAssociatedReceiver<mojom::UserSubresourceFilterHost>
        pending_receiver,
    content::RenderFrameHost* render_frame_host) {
  if (auto* manager = FromPage(render_frame_host->GetPage())) {
    manager->user_receiver_.Bind(render_frame_host,
                                 std::move(pending_receiver));
  }
}

void ArkWebContentSubresourceFilterThrottleManagerExt::
    DidDisallowFirstUserSubresource() {
  MaybeShowNotification(user_receiver_.GetCurrentTargetFrame());
}

void ArkWebContentSubresourceFilterThrottleManagerExt::FrameIsUserAd() {
  // `FrameIsAd()` can only be called for an initial empty document. As it won't
  // pass through `ReadyToCommitNavigation()` (and has not yet passed through
  // `DidFinishNavigation()`), we know it won't be updated further.
  //
  // The fenced frame root will not report this for the initial empty document
  // as the fenced frame is isolated from the embedder cannot be scripted. In
  // that case we do can rely on passing through ReadyToCommitNavigation and we
  // do so since fenced frame ad tagging varies significantly from regular
  // iframes, see `AdScriptDidCreateFencedFrame`.
  content::RenderFrameHost* render_frame_host =
      user_receiver_.GetCurrentTargetFrame();
  CHECK(!render_frame_host->IsFencedFrameRoot(), base::NotFatalUntil::M129);
  OnFrameIsAd(user_receiver_.GetCurrentTargetFrame());
}

void ArkWebContentSubresourceFilterThrottleManagerExt::FrameWasCreatedByUserAdScript() {
  OnChildFrameWasCreatedByAdScript(user_receiver_.GetCurrentTargetFrame());
}

void ArkWebContentSubresourceFilterThrottleManagerExt::UserAdScriptDidCreateFencedFrame(
    const blink::RemoteFrameToken& placeholder_token) {
  if (!blink::features::IsFencedFramesEnabled()) {
    mojo::ReportBadMessage(
        "AdScriptDidCreateFencedFrame can only be called when fenced frames "
        "are enabled.");
    return;
  }

  // This method is called from the renderer when it creates a new MPArch-based
  // fenced frame while an ad script was on the v8 stack.
  //
  // Normal frames compute this when the new RenderFrame initializes since that
  // happens synchronously during the CreateFrame call. At that time,
  // SubresourceFilterAgent calls FrameWasCreatedByAdScript if needed.  However,
  // creating an MPArch-based FencedFrame doesn't create a RenderFrame in the
  // calling process; it creates a RenderFrame in another renderer via IPC at
  // which point we cannot inspect the v8 stack so we use this special path for
  // fenced frames.

  content::RenderFrameHost* owner_frame =
      user_receiver_.GetCurrentTargetFrame();
  CHECK(owner_frame, base::NotFatalUntil::M129);

  auto* fenced_frame_root = content::RenderFrameHost::FromPlaceholderToken(
      owner_frame->GetProcess()->GetID(), placeholder_token);

  if (!fenced_frame_root) {
    return;
  }

  if (!fenced_frame_root->IsFencedFrameRoot()) {
    mojo::ReportBadMessage(
        "AdScriptDidCreateFencedFrame received token for frame that isn't a "
        "fenced frame root.");
    return;
  }

  if (fenced_frame_root->GetParentOrOuterDocument() != owner_frame) {
    mojo::ReportBadMessage(
        "AdScriptDidCreateFencedFrame called from non-embedder of fenced "
        "frame.");
    return;
  }

  CHECK(!base::Contains(tracked_ad_evidence_,
                        fenced_frame_root->GetFrameTreeNodeId()),
        base::NotFatalUntil::M129);
  OnChildFrameWasCreatedByAdScript(fenced_frame_root);
}

void ArkWebContentSubresourceFilterThrottleManagerExt::SetStatisticsAfterDocumentLoad(
    mojom::DocumentLoadStatisticsPtr statistics) {
  if (statistics_) {
    statistics_->AsPageLoadStatisticsExt()->OnStatisticsAfterDocumentLoad(*statistics);
  }
}

void ArkWebContentSubresourceFilterThrottleManagerExt::
    UserSetStatisticsAfterDocumentLoad(
        mojom::DocumentLoadStatisticsPtr statistics) {
  if (statistics_) {
    statistics_->AsPageLoadStatisticsExt()->OnUserStatisticsAfterDocumentLoad(*statistics);
  }
}

void ArkWebContentSubresourceFilterThrottleManagerExt::UserSetDocumentLoadStatistics(
    mojom::DocumentLoadStatisticsPtr statistics) {
  if (statistics_) {
    statistics_->AsPageLoadStatisticsExt()->OnUserDocumentLoadStatistics(*statistics);
  }
}
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)

}  // namespace subresource_filter
