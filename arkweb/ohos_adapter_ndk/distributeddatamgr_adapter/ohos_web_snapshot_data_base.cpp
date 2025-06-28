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

#include "ohos_web_snapshot_data_base.h"

#include <cinttypes>
#include <unistd.h>

#include <AbilityKit/ability_runtime/ability_runtime_common.h>
#include <AbilityKit/ability_runtime/application_context.h>
#include <AbilityKit/ability_runtime/context_constant.h>
#include <database/rdb/oh_predicates.h>
#include <database/rdb/oh_value_object.h>
#include <database/rdb/relational_store_error_code.h>

#include "arkweb/ohos_nweb/src/nweb_hilog.h"

using namespace OHOS::NWeb;

namespace OHOS::NWeb {
namespace {
const int32_t RDB_VERSION = 2;
const std::string WEB_SNAPSHOT_DATABASE_FILE = "web_snapshot.db";

const int DEFAULT_CAPACITY = 30; // in MB
const int MIN_CAPACITY = 0; // in MB
const int MAX_CAPACITY = 100; // in MB
const int BYTE_PER_MB = 1024 * 1024;

const std::string ID_COL = "_id";
const std::string SNAPSHOT_TABLE_NAME = "snapshot";
const std::string SNAPSHOT_KEY_COL = "key";
const std::string WHOLE_SNAPSHOT_COL = "wholeSnapshotpath";
const std::string STATIC_SNAPSHOT_COL = "staticSnapshotpath";
const std::string SNAPSHOT_TIME_COL = "snapshotTime";
const std::string SIMILARITY_COL = "similarity";
const std::string LCP_TIME_COL = "lcpTime";
const std::string SNAPSHOT_FILE_SIZE_COL = "snapshotFileSize";
const std::string SNAPSHOT_FILE_TIME_COL = "snapshotFileTime";
const std::string PREFERENCE_HASH_COL = "preferenceHash";
const uint32_t MAXIMUM_SNAPSHOT_NUMBER = 30;
const int64_t MAXIMUM_TIME_LIMIT_MICRO_SECONDS = 1000LL * 1000 * 3600 * 24 * 7;

const std::string CREATE_TABLE = "CREATE TABLE " + SNAPSHOT_TABLE_NAME
    + " (" + ID_COL + " INTEGER PRIMARY KEY, "
    + SNAPSHOT_KEY_COL + " INTEGER, " + WHOLE_SNAPSHOT_COL + " TEXT, "
    + STATIC_SNAPSHOT_COL + " TEXT, " + SNAPSHOT_TIME_COL + " INTEGER, "
    + SIMILARITY_COL + " REAL, " + LCP_TIME_COL + " INTEGER, "
    + SNAPSHOT_FILE_SIZE_COL + " INTEGER, " + SNAPSHOT_FILE_TIME_COL + " INTEGER, "
    + PREFERENCE_HASH_COL + " INTEGER, "
    + " UNIQUE (" + SNAPSHOT_KEY_COL + ") ON CONFLICT REPLACE);";

const std::string DELETE_TABLE = "DROP TABLE " + SNAPSHOT_TABLE_NAME + ";";

const std::string WEB_PATH = "/web";

const std::unordered_map<AbilityRuntime_AreaMode, Rdb_SecurityArea> AREA_MODE_MAP = {
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL1, Rdb_SecurityArea::RDB_SECURITY_AREA_EL1 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL2, Rdb_SecurityArea::RDB_SECURITY_AREA_EL2 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL3, Rdb_SecurityArea::RDB_SECURITY_AREA_EL3 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL4, Rdb_SecurityArea::RDB_SECURITY_AREA_EL4 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL5, Rdb_SecurityArea::RDB_SECURITY_AREA_EL5 }
};
}

OhosWebSnapshotDataBase& OhosWebSnapshotDataBase::GetInstance()
{
    static OhosWebSnapshotDataBase instance;
    return instance;
}

void OhosWebSnapshotDataBase::GetOrOpen(const OH_Rdb_Config& config)
{
    int32_t errCode = RDB_OK;
    rdbStore_ = OH_Rdb_GetOrOpen(&config, &errCode);
    if (rdbStore_ == nullptr) {
        WVLOG_E("web snapshot database get rdb store failed, errCode=%{public}d", errCode);
        return;
    }

    int version = 0;
    if (OH_Rdb_GetVersion(rdbStore_, &version) != RDB_OK) {
        WVLOG_E("web snapshot database get rdb version failed");
        return;
    }
    WVLOG_I("web snapshot database rdb version = %{public}d, current version = %{public}d", RDB_VERSION, version);

    if (version == RDB_VERSION) {
        return;
    }

    if (version != 0) {
        if (OH_Rdb_Execute(rdbStore_, DELETE_TABLE.c_str()) != RDB_OK) {
            WVLOG_E("web snapshot database delete table failed");
            return;
        }
    }

    if (OH_Rdb_Execute(rdbStore_, CREATE_TABLE.c_str()) != RDB_OK) {
        WVLOG_E("web snapshot database create table failed");
        return;
    }

    if (OH_Rdb_SetVersion(rdbStore_, RDB_VERSION) != RDB_OK) {
        WVLOG_E("web snapshot database set version failed");
        return;
    }
}

OhosWebSnapshotDataBase::OhosWebSnapshotDataBase()
    : totalSnapShotFileBytes_(0)
    , capacityInByte_(DEFAULT_CAPACITY * BYTE_PER_MB)
{
    AbilityRuntime_ErrorCode code = ABILITY_RUNTIME_ERROR_CODE_PARAM_INVALID;
    constexpr int32_t NATIVE_BUFFER_SIZE = 1024;
    char cacheDir[NATIVE_BUFFER_SIZE];
    int32_t cacheDirLength = 0;
    code = OH_AbilityRuntime_ApplicationContextGetCacheDir(cacheDir, NATIVE_BUFFER_SIZE, &cacheDirLength);
    if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR) {
        WVLOG_E("OH_AbilityRuntime_ApplicationContextGetCacheDir failed:err=%{public}d", code);
        return;
    }
    std::string stringDir(cacheDir);
    std::string databaseDir = stringDir + WEB_PATH;

    if (access(databaseDir.c_str(), F_OK) != 0) {
        WVLOG_E("web snapshot fail to access cache web dir:%{public}s", databaseDir.c_str());
        return;
    }

    char bundleName[NATIVE_BUFFER_SIZE];
    int32_t bundleNameLength = 0;
    code = OH_AbilityRuntime_ApplicationContextGetBundleName(bundleName, NATIVE_BUFFER_SIZE, &bundleNameLength);
    if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR) {
        WVLOG_E("OH_AbilityRuntime_ApplicationContextGetBundleName failed:err=%{public}d", code);
        return;
    }

    AbilityRuntime_AreaMode areaMode = ABILITY_RUNTIME_AREA_MODE_EL2;
    code = OH_AbilityRuntime_ApplicationContextGetAreaMode(&areaMode);
    auto it = AREA_MODE_MAP.find(areaMode);
    if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR || it == AREA_MODE_MAP.end()) {
        WVLOG_E("OH_AbilityRuntime_ApplicationContextGetAreaMode failed:err=%{public}d", code);
        return;
    }

    OH_Rdb_Config config = {0};
    config.selfSize = sizeof(OH_Rdb_Config);
    config.dataBaseDir = databaseDir.c_str();
    config.bundleName = bundleName;
    config.storeName = WEB_SNAPSHOT_DATABASE_FILE.c_str();
    config.area = it->second;
    config.securityLevel = OH_Rdb_SecurityLevel::S3;

    GetOrOpen(config);
    GetAllInfo();
}

OhosWebSnapshotDataBase::~OhosWebSnapshotDataBase()
{
    if (rdbStore_ == nullptr) {
        WVLOG_E("web snapshot database delete rdb is null");
        return;
    }
    int errCode = OH_Rdb_CloseStore(rdbStore_);
    if (errCode == RDB_OK) {
        WVLOG_I("web snapshot database delete rdb succeed");
    } else {
        WVLOG_E("web snapshot database delete rdb failed");
    }
}

int32_t OhosWebSnapshotDataBase::SetBlanklessLoadingCacheCapacity(int32_t capacity)
{
    int calCapacity = capacity;
    if (capacity < MIN_CAPACITY) {
        calCapacity = MIN_CAPACITY;
    } else if (capacity > MAX_CAPACITY) {
        calCapacity = MAX_CAPACITY;
    }
    capacityInByte_.store(calCapacity * BYTE_PER_MB);
    return calCapacity;
}

void OhosWebSnapshotDataBase::ClearSnapshotDataItem(const std::vector<int64_t>& blankless_keys)
{
    if (rdbStore_ == nullptr) {
        WVLOG_E("web snapshot database clear data rdb is null");
        return;
    }

    int32_t errCode = RDB_OK;
    if (blankless_keys.size() == 0) {
        DataMapClear();
        errCode = DataClear();
        WVLOG_I("web snapshot database clear all data, errCode:%{public}d", errCode);
        return;
    }

    for (int64_t blankless_key : blankless_keys) {
        DataMapErase(blankless_key);
        errCode = DataDelete(blankless_key);
        WVLOG_I("web snapshot database clear data url=%{public}ld, errCode:%{public}d", blankless_key, errCode);
    }
}

void OhosWebSnapshotDataBase::InsertSnapshotDataItem(int64_t blankless_key, const SnapshotDataItem& data)
{
    if (rdbStore_ == nullptr) {
        return;
    }

    InsertDataBaseDataItem(blankless_key, {data, GetCurrentTime()});
}

SnapshotDataItem OhosWebSnapshotDataBase::GetSnapshotDataItem(int64_t blankless_key)
{
    std::lock_guard<std::mutex> lock(dataBaseMapMtx_);
    auto it = dataBaseMap_.find(blankless_key);
    if (it == dataBaseMap_.end()) {
        WVLOG_E("web snapshot database get data failed");
        return SnapshotDataItem{};
    }

    if (GetCurrentTime() - MAXIMUM_TIME_LIMIT_MICRO_SECONDS >= it->second.time) {
        WVLOG_E("web snapshot database get data timeout");
        return SnapshotDataItem{};
    }
    return it->second.snapshotData;
}

void OhosWebSnapshotDataBase::RegisterDataBaseCallback(std::shared_ptr<OhosWebSnapshotDataBaseCallback> callback)
{
    if (callback == nullptr) {
        WVLOG_E("web snapshot database register callback is null");
        return;
    }
    dataBaseDeleteCallbacks_.push_back(callback);
    std::unordered_set<std::string> existPaths;
    {
        std::lock_guard<std::mutex> lock(dataBaseMapMtx_);
        for (const auto& pair : dataBaseMap_) {
            const std::string& wholePath = pair.second.snapshotData.wholePath;
            const std::string& staticPath = pair.second.snapshotData.staticPath;
            if (!wholePath.empty()) {
                existPaths.insert(wholePath);
            }
            if (!staticPath.empty()) {
                existPaths.insert(staticPath);
            }
        }
    }
    callback->OnDataExist(existPaths);
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void OhosWebSnapshotDataBase::GetAllInfo()
{
    DataMapClear();
    if (rdbStore_ == nullptr) {
        return;
    }

    OH_Predicates* rdbPredicate = OH_Rdb_CreatePredicates(SNAPSHOT_TABLE_NAME.c_str());
    rdbPredicate->clear(rdbPredicate);
    OH_Cursor* cursor = OH_Rdb_Query(rdbStore_, rdbPredicate, NULL, 0);
    rdbPredicate->destroy(rdbPredicate);
    if (cursor == nullptr) {
        return;
    }
    if (cursor->goToNextRow(cursor) != RDB_OK) {
        cursor->destroy(cursor);
        return;
    }
    int32_t urlColumnIndex;
    cursor->getColumnIndex(cursor, SNAPSHOT_KEY_COL.c_str(), &urlColumnIndex);
    int32_t wholePathColumnIndex;
    cursor->getColumnIndex(cursor, WHOLE_SNAPSHOT_COL.c_str(), &wholePathColumnIndex);
    int32_t staticPathColumnIndex;
    cursor->getColumnIndex(cursor, STATIC_SNAPSHOT_COL.c_str(), &staticPathColumnIndex);
    int32_t snapshotTimeColumnIndex;
    cursor->getColumnIndex(cursor, SNAPSHOT_TIME_COL.c_str(), &snapshotTimeColumnIndex);
    int32_t similarityColumnIndex;
    cursor->getColumnIndex(cursor, SIMILARITY_COL.c_str(), &similarityColumnIndex);
    int32_t lcpTimeColumnIndex;
    cursor->getColumnIndex(cursor, LCP_TIME_COL.c_str(), &lcpTimeColumnIndex);
    int32_t snapShotFileSizeColumnIndex;
    cursor->getColumnIndex(cursor, SNAPSHOT_FILE_SIZE_COL.c_str(), &snapShotFileSizeColumnIndex);
    int32_t snapShotFileTimeColumnIndex;
    cursor->getColumnIndex(cursor, SNAPSHOT_FILE_TIME_COL.c_str(), &snapShotFileTimeColumnIndex);
    int32_t preferenceHashColumnIndex;
    cursor->getColumnIndex(cursor, PREFERENCE_HASH_COL.c_str(), &preferenceHashColumnIndex);
    std::vector<uint64_t> timeoutColumns;
    int64_t allowedSnapshotTime = GetCurrentTime() - MAXIMUM_TIME_LIMIT_MICRO_SECONDS;
    size_t size = 0;
    do {
        int64_t snapshotTime;
        cursor->getInt64(cursor, snapshotTimeColumnIndex, &snapshotTime);
        int64_t blankless_key = 0LL;
        cursor->getInt64(cursor, urlColumnIndex, &blankless_key);
        if (allowedSnapshotTime >= snapshotTime) {
            timeoutColumns.push_back(blankless_key);
            continue;
        }

        SnapshotDataItem dataItem;
        cursor->getSize(cursor, wholePathColumnIndex, &size);
        auto wholePath = std::make_unique<char[]>(size + 1);
        cursor->getText(cursor, wholePathColumnIndex, wholePath.get(), size + 1);
        dataItem.wholePath = wholePath.get();

        cursor->getSize(cursor, staticPathColumnIndex, &size);
        auto staticPath = std::make_unique<char[]>(size + 1);
        cursor->getText(cursor, staticPathColumnIndex, staticPath.get(), size + 1);
        dataItem.staticPath = staticPath.get();

        cursor->getReal(cursor, similarityColumnIndex, &dataItem.historySimilarity);
        int64_t lcpTime;
        cursor->getInt64(cursor, lcpTimeColumnIndex, &lcpTime);
        dataItem.lcpTime = static_cast<int32_t>(lcpTime);
        cursor->getInt64(cursor, snapShotFileSizeColumnIndex, &dataItem.snapShotFileSize);
        cursor->getInt64(cursor, snapShotFileTimeColumnIndex, &dataItem.snapShotFileTime);
        cursor->getInt64(cursor, preferenceHashColumnIndex, &dataItem.preferenceHash);
        auto oldest_blankless_keys = DataMapInsert(blankless_key, {dataItem, snapshotTime});
        for (auto oldest_blankless_key: oldest_blankless_keys) {
            DataDelete(oldest_blankless_key);
        }
    } while (cursor->goToNextRow(cursor) == RDB_OK);
    cursor->destroy(cursor);

    for (int64_t timeoutKey : timeoutColumns) {
        int32_t errCode = DataDelete(timeoutKey);
        WVLOG_I("web snapshot database timeout url=%{public}lu, errCode:%{public}d", timeoutKey, errCode);
    }
}

int64_t OhosWebSnapshotDataBase::GetCurrentTime()
{
    auto currentTimeStamp = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(currentTimeStamp).count();
}

void OhosWebSnapshotDataBase::InsertDataBaseDataItem(int64_t blankless_key, const DataBaseDataItem& data)
{
    int32_t errCode = RDB_OK;
    if (DataMapUpdate(blankless_key, data)) {
        errCode = DataInsert(blankless_key, data);
        WVLOG_I("web snapshot database insert snapshotPath end, errCode:%{public}d", errCode);
        return;
    }

    auto oldest_blankless_keys = DataMapInsert(blankless_key, data);
    for (auto oldest_blankless_key: oldest_blankless_keys) {
        DataDelete(oldest_blankless_key);
    }
    errCode = DataInsert(blankless_key, data);
    WVLOG_I("web snapshot database insert snapshotPath end, errCode:%{public}d", errCode);
}

void OhosWebSnapshotDataBase::NotifyDataBaseDeletePath(const std::string& path)
{
    if (path.empty()) {
        WVLOG_W("web snapshot database notify callback path is empty");
        return;
    }
    for (const auto& callback : dataBaseDeleteCallbacks_) {
        callback->OnDataDelete(path);
        WVLOG_I("web snapshot database notify callback path:%{public}s", path.c_str());
    }
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) int32_t OhosWebSnapshotDataBase::DataClear()
{
    for (const auto& callback : dataBaseDeleteCallbacks_) {
        callback->OnDataClear();
    }

    OH_Predicates* rdbPredicate = OH_Rdb_CreatePredicates(SNAPSHOT_TABLE_NAME.c_str());
    int32_t errCode = OH_Rdb_Delete(rdbStore_, rdbPredicate);
    rdbPredicate->destroy(rdbPredicate);
    return errCode;
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) int32_t OhosWebSnapshotDataBase::DataDelete(int64_t blankless_key)
{
    OH_VObject* object = OH_Rdb_CreateValueObject();
    OH_Predicates* rdbPredicate = OH_Rdb_CreatePredicates(SNAPSHOT_TABLE_NAME.c_str());
    object->putInt64(object, &blankless_key, 1);
    rdbPredicate->clear(rdbPredicate);
    rdbPredicate->equalTo(rdbPredicate, SNAPSHOT_KEY_COL.c_str(), object);
    int32_t errCode = OH_Rdb_Delete(rdbStore_, rdbPredicate);
    rdbPredicate->destroy(rdbPredicate);
    object->destroy(object);
    return errCode;
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) int32_t OhosWebSnapshotDataBase::DataInsert(int64_t blankless_key,
    const DataBaseDataItem& data)
{
    OH_VBucket* valueBucket = OH_Rdb_CreateValuesBucket();
    valueBucket->clear(valueBucket);
    valueBucket->putInt64(valueBucket, SNAPSHOT_KEY_COL.c_str(), blankless_key);
    valueBucket->putText(valueBucket, WHOLE_SNAPSHOT_COL.c_str(), data.snapshotData.wholePath.c_str());
    valueBucket->putText(valueBucket, STATIC_SNAPSHOT_COL.c_str(), data.snapshotData.staticPath.c_str());
    valueBucket->putReal(valueBucket, SIMILARITY_COL.c_str(), data.snapshotData.historySimilarity);
    valueBucket->putInt64(valueBucket, LCP_TIME_COL.c_str(), data.snapshotData.lcpTime);
    valueBucket->putInt64(valueBucket, SNAPSHOT_FILE_SIZE_COL.c_str(), data.snapshotData.snapShotFileSize);
    valueBucket->putInt64(valueBucket, SNAPSHOT_FILE_TIME_COL.c_str(), data.snapshotData.snapShotFileTime);
    valueBucket->putInt64(valueBucket, PREFERENCE_HASH_COL.c_str(), data.snapshotData.preferenceHash);
    valueBucket->putInt64(valueBucket, SNAPSHOT_TIME_COL.c_str(), data.time);
    int32_t errCode = OH_Rdb_Insert(rdbStore_, SNAPSHOT_TABLE_NAME.c_str(), valueBucket);
    valueBucket->destroy(valueBucket);
    return errCode;
}

void OhosWebSnapshotDataBase::DataMapClear()
{
    std::lock_guard<std::mutex> lock(dataBaseMapMtx_);
    dataBaseMap_.clear();
    totalSnapShotFileBytes_ = 0;
}

void OhosWebSnapshotDataBase::DataMapErase(int64_t blankless_key)
{
    std::lock_guard<std::mutex> lock(dataBaseMapMtx_);
    auto it = dataBaseMap_.find(blankless_key);
    if (it == dataBaseMap_.end()) {
        WVLOG_E("web snapshot database delete can not find blankless_key:%{public}lu", blankless_key);
        return;
    }
    NotifyDataBaseDeletePath(it->second.snapshotData.wholePath);
    NotifyDataBaseDeletePath(it->second.snapshotData.staticPath);
    if (totalSnapShotFileBytes_ >= it->second.snapshotData.snapShotFileSize) {
        totalSnapShotFileBytes_ -= it->second.snapshotData.snapShotFileSize;
    } else {
        totalSnapShotFileBytes_ = 0;
    }
    dataBaseMap_.erase(blankless_key);
}

std::vector<int64_t> OhosWebSnapshotDataBase::DataMapInsert(int64_t blankless_key, const DataBaseDataItem& data)
{
    std::vector<int64_t> oldest_blankless_keys;
    std::lock_guard<std::mutex> lock(dataBaseMapMtx_);
    while (dataBaseMap_.size() >= MAXIMUM_SNAPSHOT_NUMBER ||
        UINT64_MAX - totalSnapShotFileBytes_ < data.snapshotData.snapShotFileSize ||
        totalSnapShotFileBytes_ + data.snapshotData.snapShotFileSize > capacityInByte_.load()) {
        int64_t oldestTime = INT64_MAX;
        int64_t oldest_blankless_key = INT64_MAX;
        uint64_t oldest_blankless_size = 0;
        for (const auto& pair : dataBaseMap_) {
            if (oldestTime > pair.second.time) {
                oldestTime = pair.second.time;
                oldest_blankless_key = pair.first;
                oldest_blankless_size = pair.second.snapshotData.snapShotFileSize;
            }
        }
        if (oldest_blankless_key != INT64_MAX) {
            oldest_blankless_keys.push_back(oldest_blankless_key);
            dataBaseMap_.erase(oldest_blankless_key);
            if (totalSnapShotFileBytes_ >= oldest_blankless_size) {
                totalSnapShotFileBytes_ -= oldest_blankless_size;
            } else {
                totalSnapShotFileBytes_ = 0;
            }
        }
        WVLOG_I("web snapshot database delete key=%{public}ld", oldest_blankless_key);
    }

    dataBaseMap_.insert({blankless_key, data});
    totalSnapShotFileBytes_ += data.snapshotData.snapShotFileSize;
    return oldest_blankless_keys;
}

bool OhosWebSnapshotDataBase::DataMapUpdate(int64_t blankless_key, const DataBaseDataItem& newData)
{
    std::lock_guard<std::mutex> lock(dataBaseMapMtx_);
    auto it = dataBaseMap_.find(blankless_key);
    if (it == dataBaseMap_.end()) {
        return false;
    }
    if (it->second.snapshotData.wholePath != newData.snapshotData.wholePath) {
        NotifyDataBaseDeletePath(it->second.snapshotData.wholePath);
    }
    if (it->second.snapshotData.staticPath != newData.snapshotData.staticPath) {
        NotifyDataBaseDeletePath(it->second.snapshotData.staticPath);
    }
    if (totalSnapShotFileBytes_ >= it->second.snapshotData.snapShotFileSize) {
        totalSnapShotFileBytes_ -= it->second.snapshotData.snapShotFileSize;
    } else {
        totalSnapShotFileBytes_ = 0;
    }
    totalSnapShotFileBytes_ += newData.snapshotData.snapShotFileSize;
    it->second = newData;
    return true;
}
} // namespace OHOS::NWeb