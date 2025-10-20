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
#include "ohos_nweb/src/nweb_permission_status_query_manager.h"
#include "nweb_access_query.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace OHOS::NWeb;
using namespace testing;
namespace {
  static bool g_on_changed = false;
  void ResetTestState() {
    g_on_changed = false;
  }
}

struct TestNWebPermissionStatusQuery : public NWebPermissionStatusQuery {
  TestNWebPermissionStatusQuery() : access_query(nullptr) {}
  NWebAccessQuery* access_query;
};

class NWebPermissionStatusQueryManagerTest : public testing::Test {
 protected:
  void SetUp() override {
    ResetTestState();
    NWebPermissionStatusQueryManager::SetPermissionStatusQueryDelegate(nullptr);
  }
};

TEST_F(NWebPermissionStatusQueryManagerTest, QueryPermissionStatus_DelegateIsNotNull) {
  struct NWebPermissionStatusQueryDelegateCallback callback;
  callback.onPermissionStatusQuery = [](struct NWebPermissionStatusQuery* query) {
    g_on_changed = true;
  };
  NWebPermissionStatusQueryManager::SetPermissionStatusQueryDelegate(&callback);
  auto query = new TestNWebPermissionStatusQuery();
  query->access_query = nullptr;
  EXPECT_NO_FATAL_FAILURE(NWebPermissionStatusQueryManager::QueryPermissionStatus(query));
  EXPECT_TRUE(g_on_changed);
}

TEST_F(NWebPermissionStatusQueryManagerTest, QueryPermissionStatus_CallbackIsNull) {
  EXPECT_NO_FATAL_FAILURE(NWebPermissionStatusQueryManager::QueryPermissionStatus(
                          static_cast<NWebPermissionStatusQuery*>(nullptr)));
  EXPECT_FALSE(g_on_changed);
}

TEST_F(NWebPermissionStatusQueryManagerTest, QueryPermissionStatus_QueryNotNull) {
  auto query = new TestNWebPermissionStatusQuery();
  query->access_query = nullptr;
  EXPECT_NO_FATAL_FAILURE(NWebPermissionStatusQueryManager::QueryPermissionStatus(query));
  EXPECT_FALSE(g_on_changed);
}
