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

#include <algorithm>
#include <cstring>
#include <gtest/gtest.h>

#define private public
#include "arkweb/ohos_adapter_ndk/distributeddatamgr_adapter/ohos_web_snapshot_data_base.h"
#undef private

namespace OHOS::NWeb {
namespace {
OH_Rdb_Config g_RdbConfig = {0}; // for version test;
OH_Rdb_Store* g_TmpRdbStore = nullptr; // for nullptr test
const int64_t BYTE_PER_MB = 1024LL * 1024;
const int64_t MICRO_SECONDS_PER_DAY = 1000LL * 1000 * 3600 * 24;

#define STR(prefix, suffix) (#prefix + std::to_string(suffix))

int64_t GetCurrentTime()
{
    auto currentTimeStamp = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(currentTimeStamp).count();
}
}

class OhosWebSnapshotDataBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

class OhosWebSnapshotDataBaseCallbackUt : public OhosWebSnapshotDataBaseCallback {
public:
    OhosWebSnapshotDataBaseCallbackUt(
        const std::unordered_set<std::string>& existSet,
        const std::vector<std::string>& deleteVector,
        uint32_t clearTimes)
        : expectExistSet_(std::move(existSet)),
          expectDeleteVector_(std::move(deleteVector)),
          expectClearTimes_(clearTimes) {}

    ~OhosWebSnapshotDataBaseCallbackUt() override
    {
        EXPECT_EQ(0, expectDeleteVector_.size());
        EXPECT_EQ(realClearTimes_, expectClearTimes_);
        EXPECT_EQ(realExistTimes_, 1);
        EXPECT_EQ(TimesToDelete_, 0);
    }

    void OnDataDelete(const std::string& path) override
    {
        TimesToDelete_--;
        EXPECT_EQ(path, expectDeleteVector_[0]);
        expectDeleteVector_.erase(expectDeleteVector_.begin());
    }

    void OnDataExist(const std::unordered_set<std::string>& existPaths) override
    {
        EXPECT_EQ(existPaths, expectExistSet_);
        realExistTimes_++;
    }

    void OnDataClear() override
    {
        realClearTimes_++;
    }

    void StartToDelete(uint32_t times)
    {
        EXPECT_EQ(TimesToDelete_, 0);
        TimesToDelete_ = times;
    }

    void StopToDelete()
    {
        EXPECT_EQ(TimesToDelete_, 0);
        TimesToDelete_ = 0;
    }

private:
    std::unordered_set<std::string> expectExistSet_;
    std::vector<std::string> expectDeleteVector_;
    uint32_t expectClearTimes_;
    uint32_t realClearTimes_ = 0;
    uint32_t realExistTimes_ = 0;
    uint32_t TimesToDelete_ = 0;
};

void OhosWebSnapshotDataBaseTest::SetUpTestCase()
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();
    g_RdbConfig.selfSize = sizeof(OH_Rdb_Config);
    g_RdbConfig.dataBaseDir = "/data/storage/el2/base/cache/web";
    g_RdbConfig.bundleName = "com.example.myapplication";
    g_RdbConfig.storeName = "web_snapshot.db";
    g_RdbConfig.area = Rdb_SecurityArea::RDB_SECURITY_AREA_EL2;
    g_RdbConfig.securityLevel = OH_Rdb_SecurityLevel::S3;
    dataBase.GetOrOpen(g_RdbConfig);
    dataBase.GetAllInfo();
}

void OhosWebSnapshotDataBaseTest::TearDownTestCase() {}

void OhosWebSnapshotDataBaseTest::SetUp()
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();
    EXPECT_NE(dataBase.rdbStore_, nullptr);
}

void OhosWebSnapshotDataBaseTest::TearDown()
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();
    EXPECT_NE(dataBase.rdbStore_, nullptr);
    dataBase.dataBaseDeleteCallbacks_.clear();
    dataBase.ClearSnapshotDataItem({});
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
}

TEST_F(OhosWebSnapshotDataBaseTest, GetInstanceNoHookTest)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();
    std::swap(dataBase.rdbStore_, g_TmpRdbStore);

    EXPECT_EQ(dataBase.rdbStore_, nullptr);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(20), 0);

    std::swap(dataBase.rdbStore_, g_TmpRdbStore);
}

TEST_F(OhosWebSnapshotDataBaseTest, GetOrOpenLowVersionTest)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    EXPECT_EQ(dataBase.InsertSnapshotDataItem(1,
        {"w1", "s1", 0.8, 10, 3450000, GetCurrentTime(), 1234, 1080, 2280}), true);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, 10);
    int version = 0;
    EXPECT_EQ(OH_Rdb_GetVersion(dataBase.rdbStore_, &version), 0);
    if (version > 1) {
        EXPECT_EQ(OH_Rdb_SetVersion(dataBase.rdbStore_, version - 1), 0);
        EXPECT_EQ(OH_Rdb_CloseStore(dataBase.rdbStore_), 0);
        dataBase.rdbStore_ = nullptr;
        dataBase.GetOrOpen(g_RdbConfig);
        dataBase.ClearMap();
        dataBase.GetAllInfo();
        EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
        EXPECT_EQ(OH_Rdb_SetVersion(dataBase.rdbStore_, version), 0);
    }
}

TEST_F(OhosWebSnapshotDataBaseTest, GetOrOpenHighVersionTest)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    EXPECT_EQ(dataBase.InsertSnapshotDataItem(1,
        {"w1", "s1", 0.8, 10, 3450000, GetCurrentTime(), 1234, 1080, 2280}), true);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, 10);
    int version = 0;
    EXPECT_EQ(OH_Rdb_GetVersion(dataBase.rdbStore_, &version), 0);
    EXPECT_EQ(OH_Rdb_SetVersion(dataBase.rdbStore_, version + 1), 0);
    EXPECT_EQ(OH_Rdb_CloseStore(dataBase.rdbStore_), 0);
    dataBase.rdbStore_ = nullptr;
    dataBase.GetOrOpen(g_RdbConfig);
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(OH_Rdb_SetVersion(dataBase.rdbStore_, version), 0);
}

TEST_F(OhosWebSnapshotDataBaseTest, GetOrOpenMatchVersionTest)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    EXPECT_EQ(dataBase.InsertSnapshotDataItem(1,
        {"w1", "s1", 0.8, 10, 3450000, GetCurrentTime(), 1234, 1080, 2280}), true);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, 10);
    int version = 0;
    EXPECT_EQ(OH_Rdb_GetVersion(dataBase.rdbStore_, &version), 0);
    EXPECT_EQ(OH_Rdb_CloseStore(dataBase.rdbStore_), 0);
    dataBase.rdbStore_ = nullptr;
    dataBase.GetOrOpen(g_RdbConfig);
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, 10);
    EXPECT_EQ(OH_Rdb_SetVersion(dataBase.rdbStore_, version), 0);
}

TEST_F(OhosWebSnapshotDataBaseTest, RdbNullptrTest)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();
    std::swap(dataBase.rdbStore_, g_TmpRdbStore);

    EXPECT_EQ(dataBase.InsertSnapshotDataItem(1,
        {"w1", "s1", 0.8, 10, 3450000, GetCurrentTime(), 1234, 1080, 2280}), false);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    dataBase.ClearSnapshotDataItem({});
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(20), 0);
    EXPECT_EQ(dataBase.GetCapacityInByte(), 0);

    std::swap(dataBase.rdbStore_, g_TmpRdbStore);
}

TEST_F(OhosWebSnapshotDataBaseTest, InsertSnapshotDataItemOverCapacityTest)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(1,
        {"w1", "s1", 0.8, 10, 50 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), false);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
}

TEST_F(OhosWebSnapshotDataBaseTest, InsertSnapshotDataItemOverNumberTest)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(1,
        {"w1", "", 0.8, 10, 0.5 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(2,
        {"", "s2", 0.8, 20, 0.5 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    std::vector<std::string> deleteVector = {"w1", "s2"};
    for (int32_t key = 3; key <= 500; key++) {
        deleteVector.push_back(STR(w, key));
        deleteVector.push_back(STR(s, key));
    }
    std::unordered_set<std::string> existSet = {"w1", "s2"};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    for (int32_t key = 3; key <= 1010; key++) {
        if (key > 1000) {
            callback->StartToDelete(key <= 1002 ? 1 : 2);
        }
        EXPECT_EQ(dataBase.InsertSnapshotDataItem(key,
            {STR(w, key), STR(s, key), 0.8, key * 10, 0.02 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
        if (key > 1000) {
            callback->StopToDelete();
        }
    }
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(10).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(11).lcpTime, 110);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1010).lcpTime, 10100);
    callback->StartToDelete(980);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(1011,
        {"w1011", "s1011", 0.8, 10110, 19.8 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    callback->StopToDelete();
    EXPECT_EQ(dataBase.GetSnapshotDataItem(11).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(20).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(501).lcpTime, 5010);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1010).lcpTime, 10100);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1011).lcpTime, 10110);
}

TEST_F(OhosWebSnapshotDataBaseTest, InsertSnapshotDataItemAccumulationOverCapacityTest)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::vector<std::string> deleteVector = {"w5", "s5"};
    for (int32_t key = 1; key <= 10; key++) {
        if (key == 5) {
            continue;
        }
        deleteVector.push_back(STR(w, key));
        deleteVector.push_back(STR(s, key));
    }
    std::unordered_set<std::string> existSet = {};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    for (int32_t key = 1; key <= 20; key++) {
        dataBase.InsertSnapshotDataItem(key,
            {STR(w, key), STR(s, key), 0.8, key * 10, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280});
    }
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, 10);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(20).lcpTime, 200);
    callback->StartToDelete(20);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(5,
        {"w5", "s5", 0.8, 50, 20 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    callback->StopToDelete();
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(5).lcpTime, 50);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(10).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(11).lcpTime, 110);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(20).lcpTime, 200);
}

TEST_F(OhosWebSnapshotDataBaseTest, ClearSnapshotDateItemPartialDataTest)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::vector<std::string> deleteVector = {};
    for (int32_t key = 6; key <= 15; key++) {
        deleteVector.push_back(STR(w, key));
        deleteVector.push_back(STR(s, key));
    }
    std::unordered_set<std::string> existSet = {};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    for (int32_t key = 1; key <= 20; key++) {
        EXPECT_EQ(dataBase.InsertSnapshotDataItem(key,
            {STR(w, key), STR(s, key), 0.8, key * 10, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    }
    EXPECT_EQ(dataBase.GetSnapshotDataItem(6).lcpTime, 60);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(15).lcpTime, 150);
    callback->StartToDelete(20);
    dataBase.ClearSnapshotDataItem({6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
    callback->StopToDelete();
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, 10);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(5).lcpTime, 50);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(6).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(15).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(16).lcpTime, 160);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(20).lcpTime, 200);
}

TEST_F(OhosWebSnapshotDataBaseTest, ClearSnapshotDateItemAllDataTest)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::unordered_set<std::string> existSet = {};
    std::vector<std::string> deleteVector = {};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 1);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    dataBase.ClearSnapshotDataItem({});
    for (int32_t key = 1; key <= 20; key++) {
        EXPECT_EQ(dataBase.InsertSnapshotDataItem(key,
            {STR(w, key), STR(s, key), 0.8, key * 10, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    }
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, 10);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(20).lcpTime, 200);
    dataBase.ClearSnapshotDataItem({});
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(20).lcpTime, INT32_MAX);
}

TEST_F(OhosWebSnapshotDataBaseTest, SetBlanklessLoadingCacheCapacity1Test)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::unordered_set<std::string> existSet = {};
    std::vector<std::string> deleteVector = {};
    for (int32_t key = 1; key <= 19; key++) {
        deleteVector.push_back(STR(w, key));
        deleteVector.push_back(STR(s, key));
    }
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    for (int32_t key = 1; key <= 20; key++) {
        EXPECT_EQ(dataBase.InsertSnapshotDataItem(key,
            {STR(w, key), STR(s, key), 0.8, key * 10, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    }
    EXPECT_EQ(dataBase.GetSnapshotDataItem(19).lcpTime, 190);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(20).lcpTime, 200);
    callback->StartToDelete(38);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(1), 1);
    callback->StopToDelete();
    EXPECT_EQ(dataBase.GetSnapshotDataItem(19).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(20).lcpTime, 200);
    EXPECT_EQ(dataBase.GetCapacityInByte(), 1 * BYTE_PER_MB);
}

TEST_F(OhosWebSnapshotDataBaseTest, SetBlanklessLoadingCacheCapacity_1Test)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::unordered_set<std::string> existSet = {};
    std::vector<std::string> deleteVector = {};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 1);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    for (int32_t key = 1; key <= 20; key++) {
        EXPECT_EQ(dataBase.InsertSnapshotDataItem(key,
            {STR(w, key), STR(s, key), 0.8, key * 10, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    }
    EXPECT_EQ(dataBase.GetSnapshotDataItem(19).lcpTime, 190);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(20).lcpTime, 200);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(-1), 0);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(19).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(20).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetCapacityInByte(), 0);
}

TEST_F(OhosWebSnapshotDataBaseTest, SetBlanklessLoadingCacheCapacity1000Test)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::unordered_set<std::string> existSet = {};
    std::vector<std::string> deleteVector = {"w1", "s1"};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(1), 1);
    for (int32_t key = 1; key <= 2; key++) {
        if (key > 1) {
            callback->StartToDelete(2);
        }
        EXPECT_EQ(dataBase.InsertSnapshotDataItem(key,
            {STR(w, key), STR(s, key), 0.8, key * 10, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
        if (key > 1) {
            callback->StopToDelete();
        }
    }
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, 20);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(1000), 100);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(3,
        {"w3", "s3", 0.8, 30, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, 20);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(3).lcpTime, 30);
    EXPECT_EQ(dataBase.GetCapacityInByte(), 100 * BYTE_PER_MB);
}

TEST_F(OhosWebSnapshotDataBaseTest, GetSnapshotDataItemTest)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::unordered_set<std::string> existSet = {};
    std::vector<std::string> deleteVector = {"w1", "w2", "s2", "s5"};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(1,
        {"w1", "", 0.8, 10, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(2,
        {"w2", "s2", 0.8, 20, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(3,
        {"w3", "s3", 0.8, 30, 50 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), false);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(4,
        {"w4", "s4", 0.8, 40, 0, GetCurrentTime(), 1234, 1080, 2280}), false);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(5,
        {"", "s5", 0.8, 50, 20 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    callback->StartToDelete(1);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(6,
        {"w6", "s6", 0.8, 60, 9 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    callback->StopToDelete();
    callback->StartToDelete(3);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(7,
        {"", "s7", 0.8, 70, 10 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    callback->StopToDelete();
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(8,
        {"w8", "s8", 0.8, 80, 10 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 3);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(3).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(4).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(5).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(6).lcpTime, 60);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(7).lcpTime, 70);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(8).lcpTime, 80);
}

TEST_F(OhosWebSnapshotDataBaseTest, InsertSnapshotDataItemTest)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::unordered_set<std::string> existSet = {};
    std::vector<std::string> deleteVector = {"w1", "w4", "s7", "w3", "w6", "s6", "w9", "s9", "s5", "w2", "s2"};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    dataBase.InsertInner(1, {{"w1", "", 0.8, 10, 10 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() - 14 * MICRO_SECONDS_PER_DAY});
    dataBase.InsertInner(2, {{"w2", "s2", 0.8, 20, 5 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 14 * MICRO_SECONDS_PER_DAY});
    dataBase.InsertInner(3, {{"w3", "", 0.8, 30, 50 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime()});
    dataBase.InsertInner(4, {{"w4", "", 0.8, 40, 0, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() - 1 * MICRO_SECONDS_PER_DAY});
    dataBase.InsertInner(5, {{"", "s5", 0.8, 50, 0, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 8 * MICRO_SECONDS_PER_DAY});
    dataBase.InsertInner(6, {{"w6", "s6", 0.8, 60, 20 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 1 * MICRO_SECONDS_PER_DAY});
    dataBase.InsertInner(7, {{"", "s7", 0.8, 70, 10 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() - 1 * MICRO_SECONDS_PER_DAY});
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 7);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, 20);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(3).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(4).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(5).lcpTime, 50);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(6).lcpTime, 60);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(7).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(8,
        {"w8", "s8", 0.8, 80, 38 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), false);
    callback->StartToDelete(6);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(9,
        {"w9", "s9", 0.8, 90, 18 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    callback->StopToDelete();
    callback->StartToDelete(5);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(10,
        {"w10", "s10", 0.8, 100, 28 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    callback->StopToDelete();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 1);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(3).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(4).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(5).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(6).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(7).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(8).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(9).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(10).lcpTime, 100);
}

TEST_F(OhosWebSnapshotDataBaseTest, GetAllInfoTest001)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::unordered_set<std::string> existSet = {};
    std::vector<std::string> deleteVector = {};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    dataBase.InsertData(1, {{"w1", "s1", 0.8, 10, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() - 7 * MICRO_SECONDS_PER_DAY});
    dataBase.InsertData(2, {{"w2", "s2", 0.8, 20, 5 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 7 * MICRO_SECONDS_PER_DAY});
    dataBase.InsertData(3, {{"w3", "s3", 0.8, 30, 20 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime()});
    dataBase.InsertData(4, {{"w4", "s4", 0.8, 40, 20 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime()});
    dataBase.InsertData(5, {{"w5", "s5", 0.8, 50, 10 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 5 * MICRO_SECONDS_PER_DAY});
    dataBase.InsertData(6, {{"w6", "s6", 0.8, 60, 10 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 1 * MICRO_SECONDS_PER_DAY});
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 3);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, 20);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(3).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(4).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(5).lcpTime, 50);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(6).lcpTime, 60);
}

TEST_F(OhosWebSnapshotDataBaseTest, GetAllInfoTest002)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::unordered_set<std::string> existSet = {};
    std::vector<std::string> deleteVector = {};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    dataBase.InsertData(1, {{"w1", "s1", 0.8, 10, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() - 7 * MICRO_SECONDS_PER_DAY});
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 0);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    dataBase.ClearMap();
    dataBase.dataBaseDeleteCallbacks_.clear();

    callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    dataBase.InsertData(2, {{"w2", "s2", 0.8, 20, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 7 * MICRO_SECONDS_PER_DAY});
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 1);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, 20);
    dataBase.ClearMap();
    dataBase.dataBaseDeleteCallbacks_.clear();

    callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    dataBase.InsertData(3, {{"w3", "s3", 0.8, 30, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() - 7 * MICRO_SECONDS_PER_DAY});
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 1);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, 20);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(3).lcpTime, INT32_MAX);
}

TEST_F(OhosWebSnapshotDataBaseTest, GetAllInfoTest003)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::unordered_set<std::string> existSet = {};
    std::vector<std::string> deleteVector = {};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    dataBase.InsertData(1, {{"w1", "s1", 0.8, 10, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 14 * MICRO_SECONDS_PER_DAY});
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 1);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, 10);
    dataBase.ClearMap();
    dataBase.dataBaseDeleteCallbacks_.clear();

    callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    dataBase.InsertData(2, {{"w2", "s2", 0.8, 20, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime()});
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 1);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, 10);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, INT32_MAX);
    dataBase.ClearMap();
    dataBase.dataBaseDeleteCallbacks_.clear();

    callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    dataBase.InsertData(3, {{"w3", "s3", 0.8, 30, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 14 * MICRO_SECONDS_PER_DAY});
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 2);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, 10);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(3).lcpTime, 30);
}

TEST_F(OhosWebSnapshotDataBaseTest, GetAllInfoTest004)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::unordered_set<std::string> existSet = {};
    std::vector<std::string> deleteVector = {};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    dataBase.InsertData(1, {{"w1", "s1", 0.8, 10, 50 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 7 * MICRO_SECONDS_PER_DAY});
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 0);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    dataBase.ClearMap();
    dataBase.dataBaseDeleteCallbacks_.clear();

    callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    dataBase.InsertData(2, {{"w2", "s2", 0.8, 20, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 7 * MICRO_SECONDS_PER_DAY});
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 1);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, 20);
    dataBase.ClearMap();
    dataBase.dataBaseDeleteCallbacks_.clear();

    callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    dataBase.InsertData(3, {{"w3", "s3", 0.8, 30, 50 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 7 * MICRO_SECONDS_PER_DAY});
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 0);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(3).lcpTime, INT32_MAX);
}

TEST_F(OhosWebSnapshotDataBaseTest, GetAllInfoTest005)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::unordered_set<std::string> existSet = {};
    std::vector<std::string> deleteVector = {};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    dataBase.InsertData(1, {{"w1", "s1", 0.8, 10, 0, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 7 * MICRO_SECONDS_PER_DAY});
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 0);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    dataBase.ClearMap();
    dataBase.dataBaseDeleteCallbacks_.clear();

    callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    dataBase.InsertData(2, {{"w2", "s2", 0.8, 20, 1 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 7 * MICRO_SECONDS_PER_DAY});
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 1);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, 20);
    dataBase.ClearMap();
    dataBase.dataBaseDeleteCallbacks_.clear();

    callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    dataBase.InsertData(3, {{"w3", "s3", 0.8, 30, 0, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 7 * MICRO_SECONDS_PER_DAY});
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 0);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(3).lcpTime, INT32_MAX);
}

TEST_F(OhosWebSnapshotDataBaseTest, GetAllInfoTest006)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::unordered_set<std::string> existSet = {};
    std::vector<std::string> deleteVector = {};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    dataBase.InsertData(1, {{"w1", "s1", 0.8, 10, 20 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 7 * MICRO_SECONDS_PER_DAY});
    dataBase.InsertData(2, {{"w2", "s2", 0.8, 20, 20 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 6 * MICRO_SECONDS_PER_DAY});
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 0);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, INT32_MAX);
}

TEST_F(OhosWebSnapshotDataBaseTest, GetAllInfoTest007)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::unordered_set<std::string> existSet = {};
    std::vector<std::string> deleteVector = {};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    for (int32_t key = 1; key <= 1000; key++) {
        dataBase.InsertData(key,
            {{STR(w, key), STR(s, key), 0.8, key * 10, 0.01 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
            GetCurrentTime() + 7 * MICRO_SECONDS_PER_DAY});
    }
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 1000);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, 10);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1000).lcpTime, 10000);
    dataBase.ClearMap();
    dataBase.dataBaseDeleteCallbacks_.clear();

    callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    dataBase.InsertData(1001,
        {{"w1001", "s1001", 0.8, 10010, 0.01 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 7 * MICRO_SECONDS_PER_DAY});
    dataBase.ClearMap();
    dataBase.GetAllInfo();
    EXPECT_EQ(dataBase.dataBaseMap_.size(), 0);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1000).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1001).lcpTime, INT32_MAX);
}

TEST_F(OhosWebSnapshotDataBaseTest, InsertSnapshotDataItemOverNumberTest1)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(1,
        {"w1", "", 0.8, 10, 0.5 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(2,
        {"", "s2", 0.8, 20, 0.5 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 30 * MICRO_SECONDS_PER_DAY), true);
    std::vector<std::string> deleteVector = {"w1"};
    for (int32_t key = 3; key <= 1010; key++) {
        deleteVector.push_back(STR(w, key));
        deleteVector.push_back(STR(s, key));
    }
    deleteVector.push_back("s2");
    std::unordered_set<std::string> existSet = {"w1", "s2"};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    for (int32_t key = 3; key <= 1010; key++) {
        if (key > 1000) {
            callback->StartToDelete(key == 1001 ? 1 : 2);
        }
        EXPECT_EQ(dataBase.InsertSnapshotDataItem(key,
            {STR(w, key), STR(s, key), 0.8, key * 10, 0.02 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
        if (key > 1000) {
            callback->StopToDelete();
        }
    }
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, 20);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(3).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(10).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(11).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(12).lcpTime, 120);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1010).lcpTime, 10100);
    callback->StartToDelete(1999);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(1011,
        {"w1011", "s1011", 0.8, 10110, 29.8 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    callback->StopToDelete();
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(3).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(10).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(11).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(12).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1010).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1011).lcpTime, 10110);
}

TEST_F(OhosWebSnapshotDataBaseTest, InsertSnapshotDataItemAccumulationOverCapacityTest1)
{
    OhosWebSnapshotDataBase& dataBase = OhosWebSnapshotDataBase::GetInstance();

    std::vector<std::string> deleteVector = {"w1", "s1", "w3", "s3", "w4", "s4", "w2", "s2"};
    std::unordered_set<std::string> existSet = {};
    auto callback = std::make_shared<OhosWebSnapshotDataBaseCallbackUt>(existSet, deleteVector, 0);
    dataBase.RegisterDataBaseCallback(callback);
    EXPECT_EQ(dataBase.SetBlanklessLoadingCacheCapacity(30), 30);
    dataBase.InsertSnapshotDataItem(1,
        {"w1", "s1", 0.8, 10, 10 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280});
    dataBase.InsertSnapshotDataItem(2,
        {"w2", "s2", 0.8, 20, 10 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280},
        GetCurrentTime() + 30 * MICRO_SECONDS_PER_DAY);
    dataBase.InsertSnapshotDataItem(3,
        {"w3", "s3", 0.8, 30, 10 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280});
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, 10);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, 20);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(3).lcpTime, 30);
    callback->StartToDelete(4);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(4,
        {"w4", "s4", 0.8, 40, 15 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    callback->StopToDelete();
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, 20);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(3).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(4).lcpTime, 40);
    callback->StartToDelete(4);
    EXPECT_EQ(dataBase.InsertSnapshotDataItem(5,
        {"w5", "s5", 0.8, 50, 25 * BYTE_PER_MB, GetCurrentTime(), 1234, 1080, 2280}), true);
    callback->StopToDelete();
    EXPECT_EQ(dataBase.GetSnapshotDataItem(1).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(2).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(3).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(4).lcpTime, INT32_MAX);
    EXPECT_EQ(dataBase.GetSnapshotDataItem(5).lcpTime, 50);
}
} // namespace OHOS::NWeb