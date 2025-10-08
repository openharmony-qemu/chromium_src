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

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "arkweb/chromium_ext/components/subresource_filter/content/shared/browser/ruleset_service_for_include.h"

namespace subresource_filter {
#if BUILDFLAG(ARKWEB_ADBLOCK)
class MockRulesetServiceClient : public RulesetServiceClient {
 public:
  void OnDeleteRulesetFile() override { delete_called_ = true; }
  bool IsDeleteCalled() { return delete_called_; }

 private:
  bool delete_called_ = false;
};

TEST(RulesetServiceIncludeTest, DeletesWhenFormatDiffers) {
  base::ScopedTempDir scoped_temp_dir;
  ASSERT_TRUE(scoped_temp_dir.CreateUniqueTempDir());
  bool has_different_format = false;
  base::FilePath format_dir =
      scoped_temp_dir.GetPath().AppendASCII("/tmp/obsolete");
  base::FilePath current_format_dir =
      scoped_temp_dir.GetPath().AppendASCII("/tmp/current");

  EXPECT_NO_FATAL_FAILURE(DeleteObsoleteRulesetsExt(
      has_different_format, format_dir, current_format_dir));
  EXPECT_TRUE(has_different_format);
}

TEST(RulesetServiceIncludeTest, DoesNotDeleteWhenFormatSame) {
  base::ScopedTempDir scoped_temp_dir;
  ASSERT_TRUE(scoped_temp_dir.CreateUniqueTempDir());
  bool has_different_format = false;
  base::FilePath format_dir =
      scoped_temp_dir.GetPath().AppendASCII("/tmp/same");
  base::FilePath current_format_dir =
      scoped_temp_dir.GetPath().AppendASCII("/tmp/same");

  EXPECT_NO_FATAL_FAILURE(DeleteObsoleteRulesetsExt(
      has_different_format, format_dir, current_format_dir));
  EXPECT_FALSE(has_different_format);
}

TEST(RulesetServiceIncludeTest, DeletesUnindexedWhenFormatOrVersionDiffers) {
  bool has_different_format = true;
  bool has_different_version = false;
  base::ScopedTempDir scoped_temp_dir;
  ASSERT_TRUE(scoped_temp_dir.CreateUniqueTempDir());
  base::FilePath unindexed_ruleset_base_dir =
      scoped_temp_dir.GetPath().AppendASCII("/unindexed");
  ::testing::StrictMock<MockRulesetServiceClient> client;
  EXPECT_NO_FATAL_FAILURE(DeleteObsoleteRulesetsClientExt(
      has_different_format, has_different_version, unindexed_ruleset_base_dir,
      &client));
  EXPECT_TRUE(client.IsDeleteCalled());
  EXPECT_FALSE(base::PathExists(unindexed_ruleset_base_dir));
}

TEST(RulesetServiceIncludeTest, DoesNotCallClientWhenFormatNotDiff) {
  bool has_different_format = false;
  bool has_different_version = true;
  base::ScopedTempDir scoped_temp_dir;
  ASSERT_TRUE(scoped_temp_dir.CreateUniqueTempDir());
  base::FilePath unindexed_ruleset_base_dir =
      scoped_temp_dir.GetPath().AppendASCII("/unindexed");
  ::testing::StrictMock<MockRulesetServiceClient> client;
  // Should not call OnDeleteRulesetFile
  EXPECT_NO_FATAL_FAILURE(DeleteObsoleteRulesetsClientExt(
      has_different_format, has_different_version, unindexed_ruleset_base_dir,
      &client));
  EXPECT_FALSE(client.IsDeleteCalled());
  EXPECT_FALSE(base::PathExists(unindexed_ruleset_base_dir));
}

TEST(RulesetServiceIncludeTest, DoesNotCallClientWhenFormatNotDiff2) {
  bool has_different_format = false;
  bool has_different_version = false;
  base::ScopedTempDir scoped_temp_dir;
  ASSERT_TRUE(scoped_temp_dir.CreateUniqueTempDir());
  base::FilePath unindexed_ruleset_base_dir =
      scoped_temp_dir.GetPath().AppendASCII("/unindexed");
  ASSERT_TRUE(base::CreateDirectory(unindexed_ruleset_base_dir));
  ::testing::StrictMock<MockRulesetServiceClient> client;
  // Should not call OnDeleteRulesetFile
  EXPECT_NO_FATAL_FAILURE(DeleteObsoleteRulesetsClientExt(
      has_different_format, has_different_version, unindexed_ruleset_base_dir,
      &client));
  EXPECT_FALSE(client.IsDeleteCalled());
  EXPECT_TRUE(base::PathExists(unindexed_ruleset_base_dir));
}

TEST(RulesetServiceIncludeTest, NoClient) {
  bool has_different_format = true;
  bool has_different_version = false;
  base::ScopedTempDir scoped_temp_dir;
  ASSERT_TRUE(scoped_temp_dir.CreateUniqueTempDir());
  base::FilePath unindexed_ruleset_base_dir =
      scoped_temp_dir.GetPath().AppendASCII("/unindexed");
  // Should not call OnDeleteRulesetFile
  EXPECT_NO_FATAL_FAILURE(DeleteObsoleteRulesetsClientExt(
      has_different_format, has_different_version, unindexed_ruleset_base_dir,
      nullptr));
  EXPECT_FALSE(base::PathExists(unindexed_ruleset_base_dir));
}
#endif
}  // namespace subresource_filter