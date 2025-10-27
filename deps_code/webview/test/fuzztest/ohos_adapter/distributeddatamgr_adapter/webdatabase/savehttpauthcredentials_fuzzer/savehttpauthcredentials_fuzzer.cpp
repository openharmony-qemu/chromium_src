/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "savehttpauthcredentials_fuzzer.h"
#include <fuzzer/FuzzedDataProvider.h>

#include <cstring>
#include <securec.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <algorithm>

#include "ohos_web_data_base_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {

class MockRdbStore : public OHOS::NativeRdb::RdbStore {
  public:
    MockRdbStore() = default;
    ~MockRdbStore() override = default;
    MOCK_METHOD((std::pair<int, int64_t>), Insert, (const std::string &table, const Row &row, Resolution resolution),
    (override));
    MOCK_METHOD(int, Insert, (int64_t &outRowId, const std::string &table, const Row &row), (override));
    MOCK_METHOD(int, InsertWithConflictResolution,
        (int64_t &outRowId, const std::string &table, const Row &row, Resolution resolution), (override));
    MOCK_METHOD(int, Replace, (int64_t &outRowId, const std::string &table, const Row &row), (override));
    MOCK_METHOD(int, BatchInsert, (int64_t &outInsertNum, const std::string &table, const Rows &rows), (override));
    MOCK_METHOD((std::pair<int, int64_t>), BatchInsert,
        (const std::string &table, const RefRows &rows), (override));
    MOCK_METHOD((std::pair<int, int>), Update,
        (const std::string &table, const Row &row, const std::string &where,
        const Values &args, Resolution resolution),
        (override));
    MOCK_METHOD(int, Update,
        (int &changedRows, const std::string &table, const Row &row,
        const std::string &whereClause, const Values &args),
        (override));
    MOCK_METHOD(int, Update,
        (int &changedRows, const Row &row, const NativeRdb::AbsRdbPredicates &predicates), (override));
    MOCK_METHOD(int, UpdateWithConflictResolution,
        (int &changedRows, const std::string &table, const Row &row, const std::string &whereClause,
        const Values &args, Resolution resolution), (override));
    MOCK_METHOD(int, Delete,
        (int &deletedRows, const std::string &table, const std::string &whereClause, const Values &args),
        (override));
    MOCK_METHOD(int, Delete,
        (int &deletedRows, const NativeRdb::AbsRdbPredicates &predicates), (override));
    MOCK_METHOD(std::shared_ptr<NativeRdb::AbsSharedResultSet>, Query,
        (int &errCode, bool distinct, const std::string &table, const Fields &columns, const std::string &whereClause,
        const Values &args, const std::string &groupBy, const std::string &indexName, const std::string &orderBy,
        const int &limit, const int &offset), (override));
    MOCK_METHOD(std::shared_ptr<NativeRdb::AbsSharedResultSet>, Query,
        (const NativeRdb::AbsRdbPredicates &predicates, const Fields &columns), (override));
    MOCK_METHOD(std::shared_ptr<NativeRdb::AbsSharedResultSet>, QuerySql,
        (const std::string &sql, const Values &args), (override));
    MOCK_METHOD(std::shared_ptr<NativeRdb::ResultSet>, QueryByStep,
        (const std::string &sql, const Values &args, bool preCount), (override));
    MOCK_METHOD(std::shared_ptr<NativeRdb::ResultSet>, QueryByStep,
        (const NativeRdb::AbsRdbPredicates &predicates, const Fields &columns, bool preCount), (override));
    MOCK_METHOD(std::shared_ptr<NativeRdb::ResultSet>, RemoteQuery,
        (const std::string &device, const NativeRdb::AbsRdbPredicates &predicates, const Fields &columns, int &errCode),
        (override));
    MOCK_METHOD((std::pair<int32_t, std::shared_ptr<NativeRdb::ResultSet>>), QuerySharingResource,
        (const NativeRdb::AbsRdbPredicates &predicates, const Fields &columns), (override));
    MOCK_METHOD(int, ExecuteSql,
        (const std::string &sql, const Values &args), (override));
    MOCK_METHOD((std::pair<int32_t, NativeRdb::ValueObject>), Execute,
        (const std::string &sql, const Values &args, int64_t trxId), (override));
    MOCK_METHOD(int, ExecuteAndGetLong,
        (int64_t &outValue, const std::string &sql, const Values &args), (override));
    MOCK_METHOD(int, ExecuteAndGetString,
        (std::string &outValue, const std::string &sql, const Values &args), (override));
    MOCK_METHOD(int, ExecuteForLastInsertedRowId,
        (int64_t &outValue, const std::string &sql, const Values &args),
        (override));

    MOCK_METHOD(int, ExecuteForChangedRowCount,
        (int64_t &outValue, const std::string &sql, const Values &args),
        (override));

    MOCK_METHOD(int, Backup,
        (const std::string &databasePath, const std::vector<uint8_t> &encryptKey),
        (override));

    MOCK_METHOD(int, Attach,
        (const std::string &alias, const std::string &pathName, const std::vector<uint8_t> encryptKey),
        (override));

    MOCK_METHOD(int, Count,
        (int64_t &outValue, const NativeRdb::AbsRdbPredicates &predicates),
        (override));

    MOCK_METHOD(int, GetVersion, (int &version), (override));
    MOCK_METHOD(int, SetVersion, (int version), (override));

    MOCK_METHOD((std::pair<int32_t, std::shared_ptr<NativeRdb::Transaction>>), CreateTransaction, (int32_t type),
        (override));

    MOCK_METHOD(int, BeginTransaction, (), (override));
    MOCK_METHOD((std::pair<int, int64_t>), BeginTrans, (), (override));

    MOCK_METHOD(int, RollBack, (), (override));
    MOCK_METHOD(int, RollBack, (int64_t trxId), (override));

    MOCK_METHOD(int, Commit, (), (override));
    MOCK_METHOD(int, Commit, (int64_t trxId), (override));

    MOCK_METHOD(bool, IsInTransaction, (), (override));

    MOCK_METHOD(std::string, GetPath, (), (override));

    MOCK_METHOD(bool, IsHoldingConnection, (), (override));

    MOCK_METHOD(bool, IsOpen, (), (override, const));

    MOCK_METHOD(bool, IsReadOnly, (), (override, const));

    MOCK_METHOD(bool, IsMemoryRdb, (), (override, const));

    MOCK_METHOD(int, Restore,
        (const std::string &backupPath, const std::vector<uint8_t> &newKey),
        (override));

    MOCK_METHOD(int, SetDistributedTables,
        (const std::vector<std::string> &tables, int32_t type,
        const DistributedRdb::DistributedConfig &distributedConfig),
        (override));

    MOCK_METHOD(std::string, ObtainDistributedTableName,
        (const std::string &device, const std::string &table, int &errCode),
        (override));

    MOCK_METHOD(int, Sync,
        (const SyncOption &option, const NativeRdb::AbsRdbPredicates &predicate, const AsyncBrief &async),
        (override));

    MOCK_METHOD(int, Sync,
        (const SyncOption &option, const std::vector<std::string> &tables, const AsyncDetail &async),
        (override));

    MOCK_METHOD(int, Sync,
        (const SyncOption &option, const NativeRdb::AbsRdbPredicates &predicate, const AsyncDetail &async),
        (override));

    MOCK_METHOD(int, Subscribe,
        (const SubscribeOption &option, std::shared_ptr<RdbStoreObserver> observer),
        (override));

    MOCK_METHOD(int, UnSubscribe,
        (const SubscribeOption &option, std::shared_ptr<RdbStoreObserver> observer),
        (override));

    MOCK_METHOD(int, SubscribeObserver,
        (const SubscribeOption &option, const std::shared_ptr<RdbStoreObserver> &observer),
        (override));

    MOCK_METHOD(int, UnsubscribeObserver,
        (const SubscribeOption &option, const std::shared_ptr<RdbStoreObserver> &observer),
        (override));

    MOCK_METHOD(int, RegisterAutoSyncCallback,
        (std::shared_ptr<DetailProgressObserver> observer),
        (override));

    MOCK_METHOD(int, UnregisterAutoSyncCallback,
        (std::shared_ptr<DetailProgressObserver> observer),
        (override));

    MOCK_METHOD(int, Notify, (const std::string &event), (override));

    MOCK_METHOD(bool, IsSlaveDiffFromMaster, (), (override, const));

    MOCK_METHOD(int32_t, GetDbType, (), (override, const));

    MOCK_METHOD((std::pair<int32_t, uint32_t>), LockCloudContainer, (), (override));

    MOCK_METHOD(int32_t, UnlockCloudContainer, (), (override));

    MOCK_METHOD(int, InterruptBackup, (), (override));

    MOCK_METHOD(int32_t, GetBackupStatus, (), (override, const));

    MOCK_METHOD(ModifyTime, GetModifyTime, (const std::string &table, const std::string &column,
    std::vector<PRIKey> &keys), (override));

    MOCK_METHOD(int, CleanDirtyData, (const std::string &table, uint64_t cursor), (override));

    MOCK_METHOD(int, GetRebuilt, (NativeRdb::RebuiltType &rebuilt), (override));

    MOCK_METHOD((std::pair<int32_t, int32_t>), Attach,
    (const NativeRdb::RdbStoreConfig &config, const std::string &attachName, int32_t waitTime), (override));

    MOCK_METHOD((std::pair<int32_t, int32_t>), Detach,
    (const std::string &attachName, int32_t waitTime), (override));

    MOCK_METHOD(int, ModifyLockStatus, (const NativeRdb::AbsRdbPredicates &predicates, bool isLock), (override));

    MOCK_METHOD(int, SetSearchable, (bool isSearchable), (override));

    MOCK_METHOD(std::string, GetLogTableName, (const std::string &tableName), (override));
};

bool SaveHttpAuthCredentialsFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::string host = dataProvider.ConsumeRandomLengthString(10);
    std::string realm = dataProvider.ConsumeRandomLengthString(20);
    std::string name = dataProvider.ConsumeRandomLengthString(10);
    std::string password = dataProvider.ConsumeRandomLengthString(20);

    std::shared_ptr<MockRdbStore> mockRdbStore = std::make_shared<MockRdbStore>();
    EXPECT_CALL(*mockRdbStore, Insert(testing::Matcher<int64_t&>(testing::_),
                                      testing::Matcher<const std::string&>(testing::_),
                                      testing::Matcher<const MockRdbStore::Row&>(testing::_)))
                .Times(testing::AnyNumber());
    OhosWebDataBaseAdapterImpl(mockRdbStore).SaveHttpAuthCredentials(host, realm, name, password.c_str());
    return true;
}

bool GetHttpAuthCredentialsFuzzTest(const uint8_t* data, size_t size)
{
    constexpr int32_t maxLen = 256;
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::string host = dataProvider.ConsumeRandomLengthString(10);
    std::string realm = dataProvider.ConsumeRandomLengthString(20);
    std::string username;
    char str[maxLen + 1] = { 0 };
    std::shared_ptr<MockRdbStore>mockRdbStore=std::make_shared<MockRdbStore>();
    EXPECT_CALL(*mockRdbStore, QueryByStep(testing::Matcher<const NativeRdb::AbsRdbPredicates&>(testing::_),
                                          testing::Matcher<const MockRdbStore::Fields&>(testing::_),
                                          testing::Matcher<bool>(testing::_)))
                .Times(testing::AnyNumber());
    EXPECT_CALL(*mockRdbStore, Query(testing::_, testing::_)).Times(testing::AnyNumber());
    OhosWebDataBaseAdapterImpl(mockRdbStore).GetHttpAuthCredentials(host, realm, username, str, maxLen + 1);
    std::fill(str, str + maxLen + 1, 0);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::SaveHttpAuthCredentialsFuzzTest(data, size);
    OHOS::GetHttpAuthCredentialsFuzzTest(data, size);
    return 0;
}
