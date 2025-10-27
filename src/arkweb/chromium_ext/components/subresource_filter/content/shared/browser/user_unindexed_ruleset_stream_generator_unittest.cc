// Copyright (c) 2025 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <cstddef>
#include <fstream>
#include <memory>
#include <set>
#include <string>

#include "base/files/file.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "base/path_service.h"
#include "base/test/task_environment.h"
#include "components/subresource_filter/core/browser/user_ruleset_version.h"
#include "components/subresource_filter/core/common/test_ruleset_creator.h"
#include "components/subresource_filter/core/common/unindexed_ruleset.h"
#include "components/url_pattern_index/proto/rules.pb.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/protobuf/src/google/protobuf/io/zero_copy_stream.h"
#include "ui/base/resource/mock_resource_bundle_delegate.h"
#include "ui/base/resource/resource_bundle.h"
#define private public
#include "components/subresource_filter/content/shared/browser/user_unindexed_ruleset_stream_generator.h"
#undef private

namespace subresource_filter {
namespace proto = url_pattern_index::proto;
namespace {
const std::set<std::string_view> kTestSuffixs = {"test1", "test2"};
constexpr int kTestResourceId = 12345;
}  // namespace

class UserUnindexedRulesetStreamGeneratorTest : public ::testing::Test {
 public:
  UserUnindexedRulesetStreamGeneratorTest() = default;
  ~UserUnindexedRulesetStreamGeneratorTest() override = default;

 protected:
  void SetUp() override {}

  void TearDown() override {}

  void CreateUserUnindexedRulesetWthSuffix() {
    std::vector<proto::CssRule> rules;
    for (const auto& suffix : kTestSuffixs) {
      auto rule = test_ruleset_creator_.MakeCssRule(suffix);
      auto* domain_item = rule.add_domains();
      EXPECT_TRUE(domain_item != nullptr);
      domain_item->set_domain("example.com");
      domain_item->set_exclude(false);
      rules.push_back(rule);
    }
    CreateUserUnindexedRuleset(rules);
  }

  bool ReadAndCheck(UserUnindexedRulesetStreamGenerator* generator) {
    UnindexedRulesetReader reader(generator->ruleset_stream());

    size_t count = 0;
    proto::FilteringRules ruleset_chunk;
    while (reader.ReadNextChunk(&ruleset_chunk)) {
      for (const auto& rule : ruleset_chunk.css_rules()) {
        if (kTestSuffixs.find(rule.css_selector()) != kTestSuffixs.end()) {
          count++;
        }
      }
    }
    return count == kTestSuffixs.size();
  }

  void CreateUserUnindexedRuleset(const std::vector<proto::CssRule>& rules) {
    ASSERT_NO_FATAL_FAILURE(
        test_ruleset_creator_.CreateUnindexedRulesetWithCssRules(
            rules, &test_unindexed_ruleset_));
  }

  testing::TestRuleset test_unindexed_ruleset_;
  testing::TestRulesetCreator test_ruleset_creator_;
};

TEST_F(UserUnindexedRulesetStreamGeneratorTest, FromFile) {
  CreateUserUnindexedRulesetWthSuffix();
  UserUnindexedRulesetInfo ruleset_info{};
  ruleset_info.content_version = "test_version";
  ruleset_info.ruleset_path = test_unindexed_ruleset_.path;
  UserUnindexedRulesetStreamGenerator generator(ruleset_info);
  google::protobuf::io::ZeroCopyInputStream* stream =
      generator.ruleset_stream();
  EXPECT_TRUE(stream != nullptr);
  EXPECT_EQ(generator.ruleset_size(), test_unindexed_ruleset_.contents.size());
  EXPECT_TRUE(ReadAndCheck(&generator));
}

TEST_F(UserUnindexedRulesetStreamGeneratorTest, FromResource) {
  CreateUserUnindexedRulesetWthSuffix();
  UserUnindexedRulesetInfo ruleset_info{};
  ruleset_info.content_version = "test_version";
  ruleset_info.resource_id = kTestResourceId;
  // save resource

  std::string tmp(
      reinterpret_cast<const char*>(test_unindexed_ruleset_.contents.data()),
      test_unindexed_ruleset_.contents.size());
  raw_ptr<ui::ResourceBundle> original_resource_bundle =
      ui::ResourceBundle::SwapSharedInstanceForTesting(nullptr);
  ::testing::NiceMock<ui::MockResourceBundleDelegate> delegate;
  ui::ResourceBundle::InitSharedInstanceWithLocale(
      "en-US", &delegate, ui::ResourceBundle::DO_NOT_LOAD_COMMON_RESOURCES);

  ON_CALL(delegate, LoadDataResourceString(kTestResourceId))
      .WillByDefault(::testing::Return(tmp));

  UserUnindexedRulesetStreamGenerator generator(ruleset_info);
  google::protobuf::io::ZeroCopyInputStream* stream =
      generator.ruleset_stream();
  EXPECT_TRUE(stream != nullptr);
  EXPECT_EQ(generator.ruleset_size(), test_unindexed_ruleset_.contents.size());
  EXPECT_TRUE(ReadAndCheck(&generator));
  ui::ResourceBundle::CleanupSharedInstance();
  ui::ResourceBundle::SwapSharedInstanceForTesting(original_resource_bundle);
}

TEST_F(UserUnindexedRulesetStreamGeneratorTest, InvalidFile) {
  base::FilePath invalid_path(FILE_PATH_LITERAL("/path/to/nonexistent/file"));
  UserUnindexedRulesetInfo ruleset_info{};
  ruleset_info.content_version = "test_version";
  ruleset_info.ruleset_path = invalid_path;

  // invalid file
  UserUnindexedRulesetStreamGenerator generator(ruleset_info);
  EXPECT_EQ(generator.ruleset_size(), -1);

  // empty file path
  UserUnindexedRulesetInfo ruleset_info2{};
  UserUnindexedRulesetStreamGenerator generator2(ruleset_info2);
  EXPECT_EQ(generator2.ruleset_size(), 0);
}
}  // namespace subresource_filter