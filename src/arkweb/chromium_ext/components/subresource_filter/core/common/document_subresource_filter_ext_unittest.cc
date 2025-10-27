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

#include <string_view>

#include "base/files/file.h"
#include "base/test/metrics/histogram_tester.h"
#include "components/subresource_filter/core/common/constants.h"
#define private public
#include "components/subresource_filter/core/common/document_subresource_filter.h"
#undef private
#include "components/subresource_filter/core/common/memory_mapped_ruleset.h"
#include "components/subresource_filter/core/common/test_ruleset_creator.h"
#include "components/subresource_filter/core/common/test_ruleset_utils.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"
#include "url/origin.h"

namespace subresource_filter {

namespace proto = url_pattern_index::proto;
namespace {
constexpr const char kTestAlphaSuffix[] = "alpha";
constexpr const char kTestDomain[] = "example.com";
constexpr const char kTestUrl[] = "https://example.com/";
}  // namespace

class DocumentSubresourceFilterExtTest : public ::testing::Test {
 public:
  DocumentSubresourceFilterExtTest() = default;
  DocumentSubresourceFilterExtTest(const DocumentSubresourceFilterExtTest&) =
      delete;
  DocumentSubresourceFilterExtTest& operator=(
      const DocumentSubresourceFilterExtTest&) = delete;

 protected:
  void SetUp() override {
    EXPECT_NO_FATAL_FAILURE(CreateRuleset(kTestAlphaSuffix));
    document_url_ = GURL(kTestUrl);
  }
  void TearDown() override {}

  void CreateRuleset(std::string_view suffix) {
    testing::TestRulesetPair test_ruleset_pair;
    ASSERT_NO_FATAL_FAILURE(test_ruleset_creator_.CreateCssRulesetWithPathSuffix(
        suffix, &test_ruleset_pair));
    ruleset_ = MemoryMappedRuleset::CreateAndInitialize(
        testing::TestRuleset::Open(test_ruleset_pair.indexedCss));
  }

  void AddSimpleCssRules(const std::vector<proto::CssRule>& rules) {
    testing::TestRulesetPair test_ruleset_pair;
    ASSERT_NO_FATAL_FAILURE(test_ruleset_creator_.CreateRulesetWithCssRules(
        rules, &test_ruleset_pair));
    ruleset_ = MemoryMappedRuleset::CreateAndInitialize(
        testing::TestRuleset::Open(test_ruleset_pair.indexedCss));
  }

  const MemoryMappedRuleset* ruleset() { return ruleset_.get(); }

  // Helper to create DocumentSubresourceFilterExt instance
  std::unique_ptr<DocumentSubresourceFilterExt> CreateFilter(
      const url::Origin& origin = url::Origin(),
      scoped_refptr<const MemoryMappedRuleset> ruleset = nullptr) {
    url::Origin actual_origin =
        origin.opaque() ? url::Origin::Create(document_url_) : origin;
    if (!ruleset) {
      ruleset = ruleset_;
    }
    // Create default activation state
    mojom::ActivationState activation_state;
    activation_state.activation_level = mojom::ActivationLevel::kEnabled;
    activation_state.filtering_disabled_for_document = false;
    return std::make_unique<DocumentSubresourceFilterExt>(
        actual_origin, activation_state, ruleset, "test_uma_tag");
  }

 protected:
  GURL document_url_;
  testing::TestRulesetCreator test_ruleset_creator_;

 private:
  scoped_refptr<const MemoryMappedRuleset> ruleset_;
};

// Test construction
TEST_F(DocumentSubresourceFilterExtTest, Construction) {
  auto filter = CreateFilter();
  EXPECT_NE(filter, nullptr);
  EXPECT_EQ(filter->AsDocumentSubresourceFilterExt(), filter.get());
}

#if BUILDFLAG(ARKWEB_ADBLOCK)
// DidMatchCssRule
TEST_F(DocumentSubresourceFilterExtTest, DidMatchCssRule) {
  auto filter = CreateFilter();
  EXPECT_NE(filter, nullptr);
  // clear
  EXPECT_NO_FATAL_FAILURE(filter->ClearStatistics());

  // SetDidFinishLoad false
  std::string dom_path = "html > body > div.ad";
  filter->SetDidFinishLoad(false);
  EXPECT_NO_FATAL_FAILURE(
      filter->DidMatchCssRule(document_url_, dom_path, true));

  EXPECT_NO_FATAL_FAILURE(
      filter->DidMatchCssRule(document_url_, dom_path, false));
  // insert success
  EXPECT_EQ(1u, filter->statistics_.loads_disallowed_url_map.size());
  std::string tmp = document_url_.spec() + "##" + dom_path;
  auto data = filter->statistics_.loads_disallowed_url_map.find(tmp);
  EXPECT_NE(data, filter->statistics_.loads_disallowed_url_map.end());

  // include
  EXPECT_NO_FATAL_FAILURE(
      filter->DidMatchCssRule(document_url_, dom_path, false));
  EXPECT_EQ(1u, filter->statistics_.loads_disallowed_url_map.size());

  dom_path += "test";
  EXPECT_NO_FATAL_FAILURE(
      filter->DidMatchCssRule(document_url_, dom_path, false));
  EXPECT_EQ(1, filter->statistics_.loads_disallowed_url_map.size());

  EXPECT_NO_FATAL_FAILURE(filter->DidMatchCssRule(
      document_url_, "html > body > div.banner", false));
  EXPECT_EQ(2u, filter->statistics_.loads_disallowed_url_map.size());
  EXPECT_NO_FATAL_FAILURE(filter->ClearStatistics());
}

// Test FindMatchingCssRule with filtering disabled
TEST_F(DocumentSubresourceFilterExtTest, FindMatchingCssRule_Fail) {
  auto filter = CreateFilter(url::Origin());
  EXPECT_NE(filter, nullptr);
  filter->activation_state_.filtering_disabled_for_document = true;
  auto rules = filter->FindMatchingCssRule(document_url_, false);
  EXPECT_EQ(rules, nullptr);

  filter->activation_state_.filtering_disabled_for_document = false;
  GURL data_url("data:text/html,<html><body>test</body></html>");
  rules = filter->FindMatchingCssRule(data_url, false);
  EXPECT_EQ(rules, nullptr);
  // find fail
  auto result = filter->GetSelectors(data_url, false);
  EXPECT_EQ(result, nullptr);
  auto result1 = filter->GetUserDomPathSelectors(data_url, false);
  EXPECT_EQ(result1, nullptr);
}

TEST_F(DocumentSubresourceFilterExtTest, FindMatchingCssRule_NormalUrl) {
  // create ruleset
  std::vector<proto::CssRule> rules;
  auto rule = test_ruleset_creator_.MakeCssRule(
      "test1", proto::RULE_SEMANTICS_BLOCKLIST);
  auto* domain_item = rule.add_domains();
  domain_item->set_domain(kTestDomain);
  domain_item->set_exclude(false);
  rules.push_back(rule);
  rule = test_ruleset_creator_.MakeCssRule("test2",
                                           proto::RULE_SEMANTICS_BLOCKLIST);
  rules.push_back(rule);
  AddSimpleCssRules(rules);

  auto filter = CreateFilter();
  EXPECT_NE(filter, nullptr);
  auto result = filter->FindMatchingCssRule(document_url_, false);
  EXPECT_NE(result, nullptr);
  result = filter->FindMatchingCssRule(document_url_, true);
  EXPECT_NE(result, nullptr);
}

TEST_F(DocumentSubresourceFilterExtTest, GetSelectors) {
  // create ruleset
  std::vector<proto::CssRule> rules;
  auto rule = test_ruleset_creator_.MakeCssRule(
      "test1", proto::RULE_SEMANTICS_BLOCKLIST);
  auto* domain_item = rule.add_domains();
  domain_item->set_domain(kTestDomain);
  domain_item->set_exclude(false);
  rules.push_back(rule);
  rule = test_ruleset_creator_.MakeCssRule("test2",
                                           proto::RULE_SEMANTICS_BLOCKLIST);
  rules.push_back(rule);
  AddSimpleCssRules(rules);

  auto filter = CreateFilter();
  EXPECT_NE(filter, nullptr);
  auto result = filter->GetSelectors(document_url_, false);
  EXPECT_NE(result, nullptr);
  result = filter->GetSelectors(document_url_, true);
  EXPECT_NE(result, nullptr);
}

TEST_F(DocumentSubresourceFilterExtTest, GetUserDomPathSelectors) {
  // create ruleset
  std::vector<proto::CssRule> rules;
  auto rule = test_ruleset_creator_.MakeCssRule(
      "test1", proto::RULE_SEMANTICS_BLOCKLIST);
  auto* domain_item = rule.add_domains();
  domain_item->set_domain(kTestDomain);
  domain_item->set_exclude(false);
  rules.push_back(rule);
  rule = test_ruleset_creator_.MakeCssRule("test2",
                                           proto::RULE_SEMANTICS_BLOCKLIST);
  rules.push_back(rule);
  AddSimpleCssRules(rules);

  auto filter = CreateFilter();
  EXPECT_NE(filter, nullptr);
  auto result = filter->GetUserDomPathSelectors(document_url_, false);
  EXPECT_NE(result, nullptr);
  result = filter->GetUserDomPathSelectors(document_url_, true);
  EXPECT_NE(result, nullptr);
}
#endif  // BUILDFLAG(ARKWEB_ADBLOCK)
}  // namespace subresource_filter
