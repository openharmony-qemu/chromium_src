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

#include <cstring>
#include <gtest/gtest.h>

#define private public
#include "arkweb/ohos_adapter_ndk/distributeddatamgr_adapter/ohos_web_snapshot_data_base.h"
#undef private

namespace OHOS::NWeb {

class OhosWebSnapshotDataBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

class OhosWebSnapshotDataBaseCallbackFuzz : public OhosWebSnapshotDataBaseCallback {
public:
    OhosWebSnapshotDataBaseCallbackFuzz() = default;

    ~OhosWebSnapshotDataBaseCallbackFuzz() override = default;

    void OnDataDelete(const std::string& path) override {}

    void OnDataExist(const std::unordered_set<std::string>& existPaths) override {}

    void OnDataClear() override {}
};

void OhosWebSnapshotDataBaseTest::SetUpTestCase() {}

void OhosWebSnapshotDataBaseTest::TearDownTestCase() {}

void OhosWebSnapshotDataBaseTest::SetUp() {}

void OhosWebSnapshotDataBaseTest::TearDown() {}

TEST_F(OhosWebSnapshotDataBaseTest, OhosWebSnapshotDataBaseTest_001)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();
    std::shared_ptr<OhosWebSnapshotDataBaseCallback> callback =
        std::make_shared<OhosWebSnapshotDataBaseCallbackFuzz>();
    std::string databaseDir = "/data/storage/el2/base/cache/web";
    dataBase.Init(databaseDir.c_str());
    OH_Rdb_Config config = {0};
    config.selfSize = sizeof(OH_Rdb_Config);
    config.dataBaseDir = databaseDir.c_str();
    config.bundleName = "com.example.myapplication";
    config.storeName = "web_snapshot.db";
    config.area = Rdb_SecurityArea::RDB_SECURITY_AREA_EL2;
    config.securityLevel = OH_Rdb_SecurityLevel::S3;

    dataBase.GetOrOpen(config);
    dataBase.GetAllInfo();
    EXPECT_NE(dataBase.rdbStore_, nullptr);
    dataBase.RegisterDataBaseCallback(callback);
    SnapshotDataItem dataItem = {
        "abc",
        "ab",
        0.5,
        1000,
        10,
        20,
        100
    };
    dataBase.InsertSnapshotDataItem(1, dataItem);
    EXPECT_NE(dataBase.dataBaseMap_.size(), 0);
    std::vector<int64_t> keys;
    keys.push_back(1);
    dataBase.ClearSnapshotDataItem(keys);
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 0);
}
} // namespace OHOS::NWeb