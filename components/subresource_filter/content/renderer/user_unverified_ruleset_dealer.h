/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef COMPONENTS_SUBRESOURCE_FILTER_CONTENT_RENDERER_USER_UNVERIFIED_RULESET_DEALER_H_
#define COMPONENTS_SUBRESOURCE_FILTER_CONTENT_RENDERER_USER_UNVERIFIED_RULESET_DEALER_H_

#include "components/subresource_filter/content/common/ruleset_dealer.h"
#include "components/subresource_filter/core/mojom/subresource_filter.mojom.h"
#include "content/public/renderer/render_thread_observer.h"
#include "mojo/public/cpp/bindings/associated_receiver.h"

namespace subresource_filter {

class MemoryMappedRuleset;

// Memory maps the subresource filtering ruleset file received over IPC from the
// RulesetDistributor, and makes it available to all SubresourceFilterAgents
// within the current render process through GetRuleset() method. Does not make
// sure that the file is valid.
//
// See RulesetDealerBase for details on the lifetime of MemoryMappedRuleset, and
// the distribution pipeline diagram in content_ruleset_service.h.
class UserUnverifiedRulesetDealer
    : public RulesetDealer,
      public content::RenderThreadObserver,
      public mojom::UserSubresourceFilterRulesetObserver {
 public:
  UserUnverifiedRulesetDealer();
  ~UserUnverifiedRulesetDealer() override;

 private:
  // content::RenderThreadObserver overrides:
  void RegisterMojoInterfaces(
      blink::AssociatedInterfaceRegistry* associated_interfaces) override;
  void UnregisterMojoInterfaces(
      blink::AssociatedInterfaceRegistry* associated_interfaces) override;

  // mojom::UserSubresourceFilterRulesetObserver overrides:
  void UserSetRulesetForProcess(base::File ruleset_file) override;

  void OnRendererAssociatedRequest(
      mojo::PendingAssociatedReceiver<
          mojom::UserSubresourceFilterRulesetObserver> receiver);

  mojo::AssociatedReceiver<mojom::UserSubresourceFilterRulesetObserver>
      receiver_{this};
};

}  // namespace subresource_filter

#endif  // COMPONENTS_SUBRESOURCE_FILTER_CONTENT_RENDERER_USER_UNVERIFIED_RULESET_DEALER_H_
