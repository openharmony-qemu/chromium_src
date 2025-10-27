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

#include "components/subresource_filter/content/renderer/user_unverified_ruleset_dealer.h"

#include "third_party/blink/public/common/associated_interfaces/associated_interface_registry.h"

namespace subresource_filter {
UserUnverifiedRulesetDealer::UserUnverifiedRulesetDealer() = default;
UserUnverifiedRulesetDealer::~UserUnverifiedRulesetDealer() = default;

void UserUnverifiedRulesetDealer::RegisterMojoInterfaces(
    blink::AssociatedInterfaceRegistry* associated_interfaces) {
  // base::Unretained can be used here because the associated_interfaces
  // is owned by the RenderThread and will live for the duration of the
  // RenderThread.
  associated_interfaces
      ->AddInterface<mojom::UserSubresourceFilterRulesetObserver>(
          base::BindRepeating(
              &UserUnverifiedRulesetDealer::OnRendererAssociatedRequest,
              base::Unretained(this)));
}

void UserUnverifiedRulesetDealer::UnregisterMojoInterfaces(
    blink::AssociatedInterfaceRegistry* associated_interfaces) {
  associated_interfaces->RemoveInterface(
      mojom::UserSubresourceFilterRulesetObserver::Name_);
}

void UserUnverifiedRulesetDealer::UserSetRulesetForProcess(
    base::File ruleset_file) {
  SetRulesetFile(std::move(ruleset_file));
}

void UserUnverifiedRulesetDealer::OnRendererAssociatedRequest(
    mojo::PendingAssociatedReceiver<mojom::UserSubresourceFilterRulesetObserver>
        receiver) {
  receiver_.reset();
  receiver_.Bind(std::move(receiver));
}
}  // namespace subresource_filter
