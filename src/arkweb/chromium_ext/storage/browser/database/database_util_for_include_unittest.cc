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
#include <string>

#include "base/files/file_path.h"
#include "gtest/gtest.h"
#include "storage/browser/database/database_util.h"

#if BUILDFLAG(ARKWEB_WEBSTORAGE)
namespace storage {
std::u16string ascii_to_u16string(const std::string& ascii_str) {
  std::u16string result;
  result.reserve(ascii_str.size());
  for (char c : ascii_str) {
    result.push_back(static_cast<char16_t>(c));
  }
  return result;
}

class DatabaseUtilTest : public ::testing::Test {
 public:
  std::u16string vfs_file_name;
  std::string origin_identifier;
  std::u16string database_name;
  std::u16string sqlite_suffix;
};

class MockDatabaseUtil : public DatabaseUtil {};
TEST_F(DatabaseUtilTest, CrackVfsFileNameExt_first_first_slash_index_npos) {
  DatabaseUtil databaseUtil;
  EXPECT_FALSE(databaseUtil.CrackVfsFileName(vfs_file_name, &origin_identifier,
                                             &database_name, &sqlite_suffix));
}
TEST_F(DatabaseUtilTest, CrackVfsFileNameExt_last_pound_index_npos) {
  DatabaseUtil databaseUtil;

  vfs_file_name = ascii_to_u16string("/a");
  EXPECT_FALSE(databaseUtil.CrackVfsFileName(vfs_file_name, &origin_identifier,
                                             &database_name, &sqlite_suffix));
}
TEST_F(DatabaseUtilTest, CrackVfsFileNameExt_IsSafeSuffix_false) {
  DatabaseUtil databaseUtil;

  vfs_file_name = ascii_to_u16string("/a.file@name");
  EXPECT_FALSE(databaseUtil.CrackVfsFileName(vfs_file_name, &origin_identifier,
                                             &database_name, &sqlite_suffix));
}
TEST_F(DatabaseUtilTest, CrackVfsFileNameExt_second_first_slash_index_npos) {
  DatabaseUtil databaseUtil;

  vfs_file_name = ascii_to_u16string("/a.db");
  EXPECT_FALSE(databaseUtil.CrackVfsFileName(vfs_file_name, &origin_identifier,
                                             &database_name, &sqlite_suffix));
}
TEST_F(DatabaseUtilTest, CrackVfsFileNameExt_IsValidOriginIdentifier_false) {
  DatabaseUtil databaseUtil;

  vfs_file_name = ascii_to_u16string("a/b/c.db");
  EXPECT_FALSE(databaseUtil.CrackVfsFileName(vfs_file_name, &origin_identifier,
                                             &database_name, &sqlite_suffix));
}
TEST_F(DatabaseUtilTest, CrackVfsFileNameExt_CrackVfsFileNameExt_success) {
  DatabaseUtil databaseUtil;

  vfs_file_name = ascii_to_u16string("://a/b/c.db");
  EXPECT_TRUE(databaseUtil.CrackVfsFileName(vfs_file_name, &origin_identifier,
                                            &database_name, &sqlite_suffix));
}
}  //  namespace storage
#endif  // BUILDFLAG(ARKWEB_WEBSTORAGE)