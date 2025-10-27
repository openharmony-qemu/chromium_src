/* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <memory>
#include <string>
#include <utility>

#include "base/files/file.h"
#include "base/test/metrics/histogram_tester.h"
#include "base/time/time.h"
#include "components/subresource_filter/content/renderer/unverified_ruleset_dealer.h"
#include "components/subresource_filter/content/renderer/user_unverified_ruleset_dealer.h"
#include "components/subresource_filter/core/common/memory_mapped_ruleset.h"
#include "components/subresource_filter/core/common/test_ruleset_creator.h"
#define private public
#define protected public
#include "components/subresource_filter/content/renderer/subresource_filter_agent.h"
#include "arkweb/chromium_ext/components/subresource_filter/core/common/document_subresource_filter_ext.h"
#include "components/subresource_filter/content/renderer/arkweb_subresource_filter_agent_ext.h"
#include "components/subresource_filter/content/renderer/user_subresource_filter_agent.h"
#include "components/subresource_filter/content/renderer/web_document_subresource_filter_impl.h"
#include "content/public/renderer/render_frame.h"
#undef protected
#undef private
#include "content/test/test_render_frame.h"
#include "url/gurl.h"
#include "user_subresource_filter_agent_test.h"

namespace subresource_filter {
class UserSubresourceFilterAgentTest : public ::testing::Test {
 protected:
  void SetUp() override {}
};

TEST_F(UserSubresourceFilterAgentTest, IsSubresourceFilterChild_MainFrame) {
  UserSubresourceFilterAgentTester tester;
  TestRenderFrameCondition cond;
  cond.is_main_frame = true;
  tester.CreateSubresourceFilterAgent(cond);
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);
  EXPECT_FALSE(agent->IsSubresourceFilterChild());
}

TEST_F(UserSubresourceFilterAgentTest, IsSubresourceFilterChild_NotMainFrame) {
  UserSubresourceFilterAgentTester tester;
  TestRenderFrameCondition cond;
  tester.CreateSubresourceFilterAgent(cond);
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);
  EXPECT_TRUE(agent->IsSubresourceFilterChild());
}

TEST_F(UserSubresourceFilterAgentTest, IsSubresourceFilterChild_FencedFrame) {
  UserSubresourceFilterAgentTester tester;
  TestRenderFrameCondition cond;
  cond.is_main_frame = false;
  cond.is_in_fenced_frame_tree = true;
  tester.CreateSubresourceFilterAgent(cond);
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);
  EXPECT_TRUE(agent->IsSubresourceFilterChild());
}

TEST_F(UserSubresourceFilterAgentTest, IsProvisional) {
  UserSubresourceFilterAgentTester tester;
  TestRenderFrameCondition cond;
  cond.is_main_frame = true;
  cond.local_frame_conditions[0].is_provisional = true;
  tester.CreateSubresourceFilterAgent(cond);
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);
  EXPECT_TRUE(agent->IsProvisional());
}

TEST_F(UserSubresourceFilterAgentTest, IsFrameCreatedByAdScript) {
  UserSubresourceFilterAgentTester tester;
  TestRenderFrameCondition cond;
  cond.is_main_frame = true;
  cond.local_frame_conditions[0].is_provisional = true;
  cond.local_frame_conditions[0].is_frame_created_by_ad_script = true;
  tester.CreateSubresourceFilterAgent(cond);
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);
  EXPECT_TRUE(agent->IsFrameCreatedByAdScript());
}

TEST_F(UserSubresourceFilterAgentTest, IsAdFrame) {
  UserSubresourceFilterAgentTester tester;
  TestRenderFrameCondition cond;
  cond.local_frame_conditions[0].is_ad_frame = true;
  tester.CreateSubresourceFilterAgent(cond);
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);
  EXPECT_TRUE(agent->IsAdFrame());
}

TEST_F(UserSubresourceFilterAgentTest,
       SetUserSubresourceFilterForCurrentDocument) {
  UserSubresourceFilterAgentTester tester;
  TestRenderFrameCondition cond;
  cond.is_main_frame = true;
  cond.is_in_fenced_frame_tree = true;
  cond.local_frame_conditions[1].is_ad_frame = true;
  cond.local_frame_conditions[0].is_frame_created_by_ad_script = true;
  tester.CreateSubresourceFilterAgent(cond);
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);
  auto filter = std::make_unique<MockWebDocumentSubresourceFilter>();
  EXPECT_NE(filter, nullptr);
  EXPECT_NO_FATAL_FAILURE(
      agent->SetUserSubresourceFilterForCurrentDocument(std::move(filter)));
}

TEST_F(UserSubresourceFilterAgentTest,
       SetAdEvidenceForInitialEmptySubframe_FrameCreatedByAdScript) {
  UserSubresourceFilterAgentTester tester;
  TestRenderFrameCondition cond;
  cond.is_main_frame = true;
  cond.local_frame_conditions[0].is_ad_frame = true;
  cond.is_in_fenced_frame_tree = true;
  cond.local_frame_conditions[0].is_frame_created_by_ad_script = true;
  tester.CreateSubresourceFilterAgent(cond, "http://example.com/test");
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);
  EXPECT_NO_FATAL_FAILURE(agent->SetAdEvidenceForInitialEmptySubframe());
}

TEST_F(UserSubresourceFilterAgentTest,
       SetAdEvidenceForInitialEmptySubframe_NoneFrameCreatedByAdScript) {
  UserSubresourceFilterAgentTester tester;
  TestRenderFrameCondition cond;
  cond.is_main_frame = true;
  cond.local_frame_conditions[0].is_ad_frame = true;
  cond.is_in_fenced_frame_tree = true;
  cond.local_frame_conditions[0].is_frame_created_by_ad_script = false;
  tester.CreateSubresourceFilterAgent(cond, "http://example.com/test");
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);
  EXPECT_NO_FATAL_FAILURE(agent->SetAdEvidenceForInitialEmptySubframe());
}

TEST_F(UserSubresourceFilterAgentTest,
       ActivateForNextCommittedLoad_NoneSubresourceFilterChild) {
  UserSubresourceFilterAgentTester tester;
  TestRenderFrameCondition cond;
  cond.is_main_frame = true;
  cond.is_in_fenced_frame_tree = false;
  tester.CreateSubresourceFilterAgent(cond, "http://example.com/test");
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);

  mojom::ActivationState state;
  state.activation_level = mojom::ActivationLevel::kEnabled;
  EXPECT_NO_FATAL_FAILURE(
      agent->ActivateForNextCommittedLoad(state.Clone(), std::nullopt));
}

#if BUILDFLAG(ARKWEB_ADBLOCK)
TEST_F(UserSubresourceFilterAgentTest, CalcElementHidingTypeOption_NullFrame) {
  UserSubresourceFilterAgentTester tester;
  TestRenderFrameCondition cond;
  cond.is_main_frame = true;
  cond.is_in_fenced_frame_tree = true;
  tester.CreateSubresourceFilterAgent(cond, "http://example.com/test");
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);
  EXPECT_NO_FATAL_FAILURE(agent->CalcElementHidingTypeOption(nullptr));
}

TEST_F(UserSubresourceFilterAgentTest,
       CalcElementHidingTypeOption_NullLocalFrame) {
  UserSubresourceFilterAgentTester tester;
  TestRenderFrameCondition cond;
  cond.is_main_frame = true;
  cond.is_in_fenced_frame_tree = true;
  cond.has_local_frame = false;
  tester.CreateSubresourceFilterAgent(cond, "http://example.com/test");
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);
  EXPECT_NO_FATAL_FAILURE(
      agent->CalcElementHidingTypeOption(tester.GetRenderFrame()));
}
#endif

TEST_F(UserSubresourceFilterAgentTest,
       GetInheritedActivationState_NullRenderFrame) {
  auto state = UserSubresourceFilterAgent::GetInheritedActivationState(nullptr);
  EXPECT_TRUE(state == mojom::ActivationState());
}

// RecordHistogramsOnFilterCreation
TEST_F(UserSubresourceFilterAgentTest,
       RecordHistogramsOnFilterCreation_kEnabled) {
  UserSubresourceFilterAgentTester tester;
  TestRenderFrameCondition cond;
  cond.is_main_frame = false;
  cond.is_in_fenced_frame_tree = true;
  tester.CreateSubresourceFilterAgent(cond, "http://example.com/test");
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);

  mojom::ActivationState state;
  state.activation_level = mojom::ActivationLevel::kEnabled;
  EXPECT_NO_FATAL_FAILURE(agent->RecordHistogramsOnFilterCreation(state));
}

TEST_F(UserSubresourceFilterAgentTest,
       RecordHistogramsOnFilterCreation_kDisabled) {
  UserSubresourceFilterAgentTester tester;
  TestRenderFrameCondition cond;
  cond.is_main_frame = false;
  cond.is_in_fenced_frame_tree = true;
  tester.CreateSubresourceFilterAgent(cond, "http://example.com/test");
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);

  mojom::ActivationState state;
  state.activation_level = mojom::ActivationLevel::kDisabled;
  EXPECT_NO_FATAL_FAILURE(agent->RecordHistogramsOnFilterCreation(state));
}

TEST_F(UserSubresourceFilterAgentTest, ConstructFilter_kDisabled) {
  UserSubresourceFilterAgentTester tester;
  TestRenderFrameCondition cond;
  cond.is_main_frame = false;
  cond.is_in_fenced_frame_tree = true;
  tester.CreateSubresourceFilterAgent(cond, "http://example.com/test");
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);

  mojom::ActivationState state;
  state.activation_level = mojom::ActivationLevel::kDisabled;
  const char url[] = "https://example.com/test";
  blink::WebURL test_url = {GURL(url)};
  EXPECT_NO_FATAL_FAILURE(agent->ConstructFilter(state, test_url));
}

TEST_F(UserSubresourceFilterAgentTest, DidSubresourceFiltered) {
  ArkWebSubresourceFilterAgentExtTester tester;
  tester.SetTestRulesetToDisallowURLs("test");
  TestRenderFrameCondition cond;
  cond.is_main_frame = false;
  cond.is_in_fenced_frame_tree = true;
  const char url[] = "http://example.com/test";
  tester.CreateMockArkSubresourceFilterAgentExt(cond, url);
  auto agent = tester.GetAgent();
  EXPECT_NE(agent, nullptr);
  // no filter_for_last_created_document_
  EXPECT_NO_FATAL_FAILURE(agent->DidSubresourceFiltered());

  // set filter_for_last_created_document_
  scoped_refptr<const MemoryMappedRuleset> ruleset =
      tester.GetUnverifiedRulesetDealer()->GetRuleset();
  EXPECT_NE(ruleset, nullptr);

  base::OnceClosure first_disallowed_load_callback(base::BindOnce([]() {}));
  mojom::ActivationState activation_state;
  activation_state.activation_level = mojom::ActivationLevel::kEnabled;
  auto filter = std::make_unique<WebDocumentSubresourceFilterImpl>(
      url::Origin::Create(GURL(url)), activation_state, std::move(ruleset),
      std::move(first_disallowed_load_callback));
  agent->filter_for_last_created_document_ = filter->AsWeakPtr();
  EXPECT_NO_FATAL_FAILURE(agent->DidSubresourceFiltered());

  // set did_load_finished_
  agent->did_load_finished_ = true;
  EXPECT_NO_FATAL_FAILURE(agent->DidSubresourceFiltered());
}
}  // namespace subresource_filter