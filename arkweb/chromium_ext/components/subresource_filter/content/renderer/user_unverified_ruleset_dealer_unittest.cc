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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "base/files/file.h"
#include "base/files/file_util.h"
#define private public
#include "components/subresource_filter/content/renderer/user_unverified_ruleset_dealer.h"
#undef private
#include "third_party/blink/public/common/associated_interfaces/associated_interface_registry.h"

namespace subresource_filter {

class MockAssociatedInterfaceRegistry
    : public blink::AssociatedInterfaceRegistry {
 public:
  MOCK_METHOD(void, AddInterface, (const std::string&, const Binder&));
  MOCK_METHOD(void, RemoveInterface, (const std::string&));
};

class UserUnverifiedRulesetDealerTest : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

// Test RegisterMojoInterfaces calls AddInterface
TEST_F(UserUnverifiedRulesetDealerTest,
       RegisterMojoInterfaces_CallsAddInterface) {
  auto registry = std::make_unique<MockAssociatedInterfaceRegistry>();
  EXPECT_NE(registry, nullptr);
  UserUnverifiedRulesetDealer dealer;
  EXPECT_NO_FATAL_FAILURE(dealer.RegisterMojoInterfaces(registry.get()));
}

// Test UnregisterMojoInterfaces calls RemoveInterface
TEST_F(UserUnverifiedRulesetDealerTest,
       UnregisterMojoInterfaces_CallsRemoveInterface) {
  UserUnverifiedRulesetDealer dealer;
  auto registry = std::make_unique<MockAssociatedInterfaceRegistry>();
  EXPECT_NE(registry, nullptr);
  EXPECT_NO_FATAL_FAILURE(dealer.UnregisterMojoInterfaces(registry.get()));
}

TEST_F(UserUnverifiedRulesetDealerTest,
     OnRendererAssociatedRequest_ResetsAndBindsReceiver) {
  UserUnverifiedRulesetDealer dealer;
  mojo::PendingAssociatedReceiver<mojom::UserSubresourceFilterRulesetObserver>
      receiver;
  EXPECT_NO_FATAL_FAILURE(
      dealer.OnRendererAssociatedRequest(std::move(receiver)));
}
}  // namespace subresource_filter