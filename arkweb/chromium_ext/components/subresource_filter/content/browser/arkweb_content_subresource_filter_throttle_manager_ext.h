// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_ARKWEB_CONTENT_SUBRESOURCE_FILTER_THROTTLE_MANAGER_EXT_H_
#define COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_ARKWEB_CONTENT_SUBRESOURCE_FILTER_THROTTLE_MANAGER_EXT_H_

#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "base/containers/flat_set.h"
#include "base/gtest_prod_util.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/raw_ref.h"
#include "base/memory/weak_ptr.h"
#include "base/supports_user_data.h"
#include "components/safe_browsing/core/browser/db/database_manager.h"
#include "components/subresource_filter/content/browser/safe_browsing_child_navigation_throttle.h"
#include "components/subresource_filter/content/mojom/subresource_filter.mojom.h"
#include "components/subresource_filter/core/browser/verified_ruleset_dealer.h"
#include "components/subresource_filter/core/common/activation_decision.h"
#include "components/subresource_filter/core/mojom/subresource_filter.mojom.h"
#include "content/public/browser/frame_tree_node_id.h"
#include "content/public/browser/render_frame_host_receiver_set.h"
#include "third_party/blink/public/common/frame/frame_ad_evidence.h"

namespace content {
class NavigationHandle;
class NavigationThrottle;
class Page;
class RenderFrameHost;
}  // namespace content

namespace subresource_filter {

class ContentSubresourceFilterThrottleManager;
class ContentSubresourceFilterWebContentsHelper;
class SubresourceFilterProfileContext;

class ArkWebContentSubresourceFilterThrottleManagerExt : public ContentSubresourceFilterThrottleManager {
 public:
  ArkWebContentSubresourceFilterThrottleManagerExt(
      SubresourceFilterProfileContext* profile_context,
      scoped_refptr<safe_browsing::SafeBrowsingDatabaseManager>
          database_manager,
      VerifiedRulesetDealer::Handle* dealer_handle,
      ContentSubresourceFilterWebContentsHelper& web_contents_helper,
      content::NavigationHandle& initiating_navigation_handle);
  ~ArkWebContentSubresourceFilterThrottleManagerExt();

  // Disallow copy and assign.
  ArkWebContentSubresourceFilterThrottleManagerExt(
      const ArkWebContentSubresourceFilterThrottleManagerExt&) = delete;
  ArkWebContentSubresourceFilterThrottleManagerExt& operator=(
      const ArkWebContentSubresourceFilterThrottleManagerExt&) = delete;

  ArkWebContentSubresourceFilterThrottleManagerExt *AsArkWebContentSubresourceFilterThrottleManagerExt() override {
    return this;
  }

#if BUILDFLAG(ARKWEB_ADBLOCK)
  mojom::ActivationState AdBlockActivationStateForNextCommittedLoad(
      content::NavigationHandle* navigation_handle,
      ActivationStateComputingNavigationThrottle* throttle);

  void StatisticActivationState(content::NavigationHandle* navigation_handle);

  void ReportSubresourceMap(content::RenderFrameHost* render_frame_host, const GURL& validated_url);

  void OnPageAdBlockActivationState(content::NavigationHandle* navigation_handle);

  void AdBlockMaybeCreateActivationState(
    content::NavigationHandle* navigation_handle,
    std::unique_ptr<ActivationStateComputingNavigationThrottle>& throttle);

  static void BindUserReceiver(
      mojo::PendingAssociatedReceiver<mojom::UserSubresourceFilterHost>
          pending_receiver,
      content::RenderFrameHost* render_frame_host);

  void UserAdScriptDidCreateFencedFrame(
      const blink::RemoteFrameToken& placeholder_token) override;
  void FrameWasCreatedByUserAdScript() override;

  void FrameIsUserAd() override;

  void DidDisallowFirstUserSubresource() override;

  void SetStatisticsAfterDocumentLoad(
      mojom::DocumentLoadStatisticsPtr statistics) override;

  void UserSetStatisticsAfterDocumentLoad(
      mojom::DocumentLoadStatisticsPtr statistics) override;

  void UserSetDocumentLoadStatistics(
      mojom::DocumentLoadStatisticsPtr statistics) override;
#endif
};

}  // namespace subresource_filter

#endif  // COMPONENTS_SUBRESOURCE_FILTER_CONTENT_BROWSER_ARKWEB_CONTENT_SUBRESOURCE_FILTER_THROTTLE_MANAGER_EXT_H_
