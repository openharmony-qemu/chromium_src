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

#include "base/files/file_path.h"
#include "base/memory/scoped_refptr.h"
#include "base/strings/utf_string_conversions.h"
#include "base/types/pass_key.h"
#include "sql/database.h"
#include "storage/browser/database/database_tracker.h"
#include "storage/browser/database/databases_table.h"
#include "storage/browser/quota/quota_manager_proxy.h"
#include "third_party/googletest/src/googletest/include/gtest/gtest.h"

using namespace testing;

#if BUILDFLAG(ARKWEB_WEBSTORAGE)

namespace storage {
class MockDatabaseTracker : public DatabaseTracker {
 public:
  MockDatabaseTracker(const base::FilePath& path,
                      bool is_incognito,
                      scoped_refptr<QuotaManagerProxy> quota_manager_proxy)
      : DatabaseTracker(path,
                        is_incognito,
                        std::move(quota_manager_proxy),
                        DatabaseTracker::CreatePassKey()) {}

  bool LazyInitResult = true;
  bool LazyInit() { return LazyInitResult; }
  base::FilePath GetOriginDirectory(const std::string& origin_identifier) {
    return DatabaseTracker::GetOriginDirectory(origin_identifier);
  }
};

class DatabaseTrackerTest : public ::testing::Test {
 public:
  sql::Database database;
  DatabaseDetails databaseDetails;
  DatabasesTable databasesTable = DatabasesTable(&database);
  base::FilePath profile_path;
  scoped_refptr<storage::QuotaManagerProxy> quota_manager_proxy;
};

TEST_F(DatabaseTrackerTest, GetFullDBFilePath_LazyInitFalse) {
  databasesTable.Init();
  databasesTable.GetDatabaseDetails(databaseDetails.origin_identifier,
                                    databaseDetails.database_name,
                                    &databaseDetails);

  MockDatabaseTracker mockDatabaseTracker =
      MockDatabaseTracker(profile_path, false, quota_manager_proxy);

  mockDatabaseTracker.LazyInitResult = false;

  EXPECT_EQ(mockDatabaseTracker.GetFullDBFilePath(
                databaseDetails.origin_identifier,
                databaseDetails.database_name, false),
            base::FilePath());
}

TEST_F(DatabaseTrackerTest, GetFullDBFilePath_IdIncorrect) {
  MockDatabaseTracker mockDatabaseTracker =
      MockDatabaseTracker(profile_path, false, quota_manager_proxy);

  EXPECT_EQ(mockDatabaseTracker.GetFullDBFilePath(
                databaseDetails.origin_identifier,
                databaseDetails.database_name, false),
            base::FilePath());
}

TEST_F(DatabaseTrackerTest, GetFullDBFilePath_Success) {
  databasesTable.Init();
  databasesTable.GetDatabaseDetails(databaseDetails.origin_identifier,
                                    databaseDetails.database_name,
                                    &databaseDetails);

  MockDatabaseTracker mockDatabaseTracker =
      MockDatabaseTracker(profile_path, false, quota_manager_proxy);

  EXPECT_EQ(
      mockDatabaseTracker.GetFullDBFilePath(databaseDetails.origin_identifier,
                                            databaseDetails.database_name,
                                            false),
      mockDatabaseTracker.GetOriginDirectory(databaseDetails.origin_identifier)
          .AppendASCII(base::UTF16ToASCII(databaseDetails.database_name)));
}
}  // namespace storage
#endif  // BUILDFLAG(ARKWEB_WEBSTORAGE)
