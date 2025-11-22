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
#include <memory>
#include <string>

#include "base/files/file_path.h"
#include "base/strings/strcat.h"
#include "base/trace_event/traced_value.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/testing_pref_service.h"
#include "components/subresource_filter/core/common/indexed_ruleset.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "components/subresource_filter/core/browser/user_ruleset_version.h"

namespace subresource_filter {
namespace {
static constexpr std::string_view kTestFilterTag = "test_filter";
static constexpr std::string_view kAnotherFilterTag = "another_filter";
static constexpr int kResourceId = 100;
}

class UserRulesetVersionTest : public testing::Test {
 public:
  void SetUp() override {
    // Register preferences for testing
    UserIndexedRulesetVersion::RegisterPrefs(test_prefs_.registry(), kTestFilterTag);
    UserIndexedRulesetVersion::RegisterPrefs(test_prefs_.registry(), kAnotherFilterTag);
  }

  void TearDown() override {}

 protected:
  TestingPrefServiceSimple test_prefs_;
};

// Test preference name generation functions
TEST_F(UserRulesetVersionTest, PrefNameGeneration_CreatesCorrectNames) {
  std::string content_pref = base::StrCat({kTestFilterTag, kUserRulesetContentVersion});
  std::string format_pref = base::StrCat({kTestFilterTag, kUserRulesetFormatVersion});
  std::string checksum_pref = base::StrCat({kTestFilterTag, kUserRulesetChecksum});
  std::string easylist_path_pref = base::StrCat({kTestFilterTag, kUserEasylistPath});
  std::string easylist_replace_pref = base::StrCat({kTestFilterTag, kUserEasylistReplace});

  // Verify that preferences are correctly registered
  EXPECT_TRUE(test_prefs_.FindPreference(content_pref));
  EXPECT_TRUE(test_prefs_.FindPreference(format_pref));
  EXPECT_TRUE(test_prefs_.FindPreference(checksum_pref));
  EXPECT_TRUE(test_prefs_.FindPreference(easylist_path_pref));
  EXPECT_TRUE(test_prefs_.FindPreference(easylist_replace_pref));
}

TEST_F(UserRulesetVersionTest, UserUnindexedRulesetInfo_Assignment) {
  UserUnindexedRulesetInfo info1;
  info1.content_version = "version_1.0";
  info1.ruleset_path = base::FilePath(FILE_PATH_LITERAL("/path/to/ruleset"));
  info1.resource_id = kResourceId;
  info1.license_path = base::FilePath(FILE_PATH_LITERAL("/path/to/license"));

  // Test copy construction
  UserUnindexedRulesetInfo info2(info1);
  EXPECT_EQ(info2.content_version, "version_1.0");
  EXPECT_EQ(info2.ruleset_path.value(), FILE_PATH_LITERAL("/path/to/ruleset"));
  EXPECT_EQ(info2.resource_id, kResourceId);
  EXPECT_EQ(info2.license_path.value(), FILE_PATH_LITERAL("/path/to/license"));

  // Test assignment operator
  UserUnindexedRulesetInfo info3 = info1;
  EXPECT_EQ(info3.content_version, "version_1.0");
  EXPECT_EQ(info3.ruleset_path.value(), FILE_PATH_LITERAL("/path/to/ruleset"));
  EXPECT_EQ(info3.resource_id, kResourceId);
  EXPECT_EQ(info3.license_path.value(), FILE_PATH_LITERAL("/path/to/license"));
}

// Test UserIndexedRulesetVersion construction
TEST_F(UserRulesetVersionTest, UserIndexedRulesetVersion_DefaultConstruction) {
  UserIndexedRulesetVersion version(kTestFilterTag);
  // default value
  EXPECT_EQ(version.filter_tag, kTestFilterTag);
  EXPECT_TRUE(version.content_version.empty());
  EXPECT_EQ(version.format_version, 0);
  EXPECT_EQ(version.checksum, 0);
  EXPECT_FALSE(version.user_easylist_replace);
  EXPECT_TRUE(version.user_easylist_path.empty());
  EXPECT_FALSE(version.IsValid());
}

// Test UserIndexedRulesetVersion validity checks
TEST_F(UserRulesetVersionTest, UserIndexedRulesetVersion_ValidityChecks) {
  UserIndexedRulesetVersion version(kTestFilterTag);

  // Initially invalid
  EXPECT_FALSE(version.IsValid());
  version.format_version = 1;
  EXPECT_FALSE(version.IsValid());

  // Set content version only (format_version = 0) - still invalid
  version.format_version = 0;
  version.content_version = "test_version";
  EXPECT_FALSE(version.IsValid());

  // Set both - now valid
  version.format_version = 1;
  version.content_version = "test_version";
  EXPECT_TRUE(version.IsValid());

  // Empty content version - invalid again
  version.content_version = "";
  EXPECT_FALSE(version.IsValid());
}

TEST_F(UserRulesetVersionTest, UserIndexedRulesetVersion_CurrentFormatVersionCheck) {
  const int current_format = UserIndexedRulesetVersion::CurrentFormatVersion();
  EXPECT_GT(current_format, 0);  // Should be a positive number

  UserIndexedRulesetVersion version("test_content", current_format, kTestFilterTag);
  EXPECT_TRUE(version.IsCurrentFormatVersion());

  UserIndexedRulesetVersion old_version("test_content", current_format - 1, kTestFilterTag);
  EXPECT_FALSE(old_version.IsCurrentFormatVersion());

  UserIndexedRulesetVersion future_version("test_content", current_format + 1, kTestFilterTag);
  EXPECT_FALSE(future_version.IsCurrentFormatVersion());
}

// Test preferences operations
TEST_F(UserRulesetVersionTest, UserIndexedRulesetVersion_PreferencesRoundTrip) {
  const std::string content_ver = "roundtrip_content_1.5";
  const int format_ver = 25;
  const int checksum_val = 98765;

  UserIndexedRulesetVersion original_version(content_ver, format_ver, kTestFilterTag);
  original_version.checksum = checksum_val;

  // Save to preferences
  original_version.SaveToPrefs(&test_prefs_);

  // Create new version and read from preferences
  UserIndexedRulesetVersion loaded_version(kTestFilterTag);
  loaded_version.ReadFromPrefs(&test_prefs_);

  // Verify data was correctly saved and loaded
  EXPECT_EQ(loaded_version.content_version, content_ver);
  EXPECT_EQ(loaded_version.format_version, format_ver);
  EXPECT_EQ(loaded_version.checksum, checksum_val);
  EXPECT_EQ(loaded_version.filter_tag, kTestFilterTag);
  EXPECT_TRUE(loaded_version.IsValid());
}

// Test Easylist preferences operations
TEST_F(UserRulesetVersionTest, UserIndexedRulesetVersion_EasylistPreferences) {
  UserIndexedRulesetVersion version(kTestFilterTag);
  std::string easylist_path = "/path/to/easylist.txt";
  bool replace_value = true;

  // Save easylist info to preferences
  version.SaveUserEasylistInfoToPrefs(&test_prefs_, easylist_path, replace_value);
  // Read back easylist info
  version.ReadUserEasylistInfoFromPrefs(&test_prefs_);
  EXPECT_EQ(version.user_easylist_path, easylist_path);
  EXPECT_EQ(version.user_easylist_replace, replace_value);
}

TEST_F(UserRulesetVersionTest, UserIndexedRulesetVersion_EasylistPreferences_NullPrefService) {
  UserIndexedRulesetVersion version(kTestFilterTag);
  std::string easylist_path = "/path/to/easylist.txt";
  // Test with null PrefService - should not crash and log error
  EXPECT_NO_FATAL_FAILURE(
      version.SaveUserEasylistInfoToPrefs(nullptr, easylist_path, true));
  EXPECT_NO_FATAL_FAILURE(
      version.ReadUserEasylistInfoFromPrefs(nullptr));
}

TEST_F(UserRulesetVersionTest, UserIndexedRulesetVersion_EasylistPreferences_RoundTrip) {
  const std::string easylist_path = "/custom/path/to/easylist.txt";
  const bool replace_value = false;

  UserIndexedRulesetVersion version(kTestFilterTag);
  version.SaveUserEasylistInfoToPrefs(&test_prefs_, 
                                     const_cast<std::string&>(easylist_path), 
                                     replace_value);

  UserIndexedRulesetVersion loaded_version(kTestFilterTag);
  loaded_version.ReadUserEasylistInfoFromPrefs(&test_prefs_);
  EXPECT_EQ(loaded_version.user_easylist_path, easylist_path);
  EXPECT_EQ(loaded_version.user_easylist_replace, replace_value);
}

// Test TracedValue generation
TEST_F(UserRulesetVersionTest, UserIndexedRulesetVersion_ToTracedValue) {
  const std::string content_ver = "traced_content_1.2";
  const int format_ver = 15;
  UserIndexedRulesetVersion version(content_ver, format_ver, kTestFilterTag);
  auto traced_value = version.ToTracedValue();
  EXPECT_NE(traced_value, nullptr);
}

// Test preference registration
TEST_F(UserRulesetVersionTest, RegisterPrefs_CreatesAllRequiredPreferences) {
  TestingPrefServiceSimple fresh_prefs;
  const std::string test_tag = "registration_test";

  // Before registration, preferences shouldn't exist
  std::string content_pref = base::StrCat({test_tag, kUserRulesetContentVersion});
  EXPECT_FALSE(fresh_prefs.FindPreference(content_pref));

  // Register preferences
  UserIndexedRulesetVersion::RegisterPrefs(fresh_prefs.registry(), test_tag);

  // After registration, all preferences should exist with correct default values
  EXPECT_TRUE(fresh_prefs.FindPreference(content_pref));
  EXPECT_EQ(fresh_prefs.GetString(content_pref), "");

  std::string format_pref = base::StrCat({test_tag, kUserRulesetFormatVersion});
  EXPECT_TRUE(fresh_prefs.FindPreference(format_pref));
  EXPECT_EQ(fresh_prefs.GetInteger(format_pref), 0);

  std::string checksum_pref = base::StrCat({test_tag, kUserRulesetChecksum});
  EXPECT_TRUE(fresh_prefs.FindPreference(checksum_pref));
  EXPECT_EQ(fresh_prefs.GetInteger(checksum_pref), 0);

  std::string easylist_path_pref = base::StrCat({test_tag, kUserEasylistPath});
  EXPECT_TRUE(fresh_prefs.FindPreference(easylist_path_pref));
  EXPECT_EQ(fresh_prefs.GetString(easylist_path_pref), "");

  std::string easylist_replace_pref = base::StrCat({test_tag, kUserEasylistReplace});
  EXPECT_TRUE(fresh_prefs.FindPreference(easylist_replace_pref));
  EXPECT_FALSE(fresh_prefs.GetBoolean(easylist_replace_pref));
}
}  // namespace subresource_filter