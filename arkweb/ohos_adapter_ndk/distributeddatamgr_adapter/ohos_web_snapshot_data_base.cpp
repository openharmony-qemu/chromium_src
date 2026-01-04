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
const int32_t RDB_VERSION = 5;
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
const std::string EXPIRATION_TIME_COL = "expirationTime";
const std::string SNAPSHOT_WIDTH = "width";
const std::string SNAPSHOT_HEIGHT = "height";
const std::string SIMILARITY_COL = "similarity";
const std::string LCP_TIME_COL = "lcpTime";
const std::string SNAPSHOT_FILE_SIZE_COL = "snapshotFileSize";
const std::string SNAPSHOT_FILE_TIME_COL = "snapshotFileTime";
const std::string PREFERENCE_HASH_COL = "preferenceHash";
const uint32_t MAXIMUM_SNAPSHOT_NUMBER = 1000;
const int64_t MAXIMUM_TIME_LIMIT_MICRO_SECONDS = 1000LL * 1000 * 3600 * 24 * 7;

const std::string DATABASE_INFO_TABLE_NAME = "databaseInfo";
const std::string UNIQUE_KEY_COL = "uniqueKey";
const int64_t DATABASE_INFO_UNIQUE_ID = 0LL;
const std::string CAPACITY_KEY_COL = "capacity";

const std::string CREATE_TABLE = "CREATE TABLE " + SNAPSHOT_TABLE_NAME
    + " (" + ID_COL + " INTEGER PRIMARY KEY, "
    + SNAPSHOT_KEY_COL + " INTEGER, " + WHOLE_SNAPSHOT_COL + " TEXT, "
    + STATIC_SNAPSHOT_COL + " TEXT, " + EXPIRATION_TIME_COL + " INTEGER, "
    + SNAPSHOT_WIDTH + " INTEGER, " + SNAPSHOT_HEIGHT + " INTEGER, "
    + SIMILARITY_COL + " REAL, " + LCP_TIME_COL + " INTEGER, "
    + SNAPSHOT_FILE_SIZE_COL + " INTEGER, " + SNAPSHOT_FILE_TIME_COL + " INTEGER, "
    + PREFERENCE_HASH_COL + " INTEGER, "
    + " UNIQUE (" + SNAPSHOT_KEY_COL + ") ON CONFLICT REPLACE);";

const std::string DELETE_TABLE = "DROP TABLE " + SNAPSHOT_TABLE_NAME + ";";

const std::string CREATE_DATABAS_INFO_TABLE = "CREATE TABLE " + DATABASE_INFO_TABLE_NAME
    + " (" + UNIQUE_KEY_COL + " INTEGER, " + CAPACITY_KEY_COL + " INTEGER, "
    + " UNIQUE (" + UNIQUE_KEY_COL + ") ON CONFLICT REPLACE);";

const std::string DELETE_DATABAS_INFO_TABLE = "DROP TABLE " + DATABASE_INFO_TABLE_NAME + ";";

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
        WVLOG_E("blankless GetOrOpen failed, errCode:%{public}d", errCode);
        return;
    }

    int version = 0;
    if (OH_Rdb_GetVersion(rdbStore_, &version) != RDB_OK) {
        WVLOG_E("blankless GetOrOpen get rdb version failed");
        return;
    }
    WVLOG_I("blankless GetOrOpen rdb version:%{public}d, current version:%{public}d", RDB_VERSION, version);

    if (version == RDB_VERSION) {
        return;
    }

    if (version != 0) {
        // The table may not exist in the old version, and delete table may be failed. should not return.
        if (OH_Rdb_Execute(rdbStore_, DELETE_TABLE.c_str()) != RDB_OK) {
            WVLOG_E("blankless GetOrOpen delete table failed");
        }
        if (OH_Rdb_Execute(rdbStore_, DELETE_DATABAS_INFO_TABLE.c_str()) != RDB_OK) {
            WVLOG_E("blankless GetOrOpen delete database info table failed");
        }
    }

    if (OH_Rdb_Execute(rdbStore_, CREATE_TABLE.c_str()) != RDB_OK) {
        WVLOG_E("blankless GetOrOpen create table failed");
        return;
    }
    if (OH_Rdb_Execute(rdbStore_, CREATE_DATABAS_INFO_TABLE.c_str()) != RDB_OK) {
        WVLOG_E("blankless GetOrOpen create database info table failed");
        return;
    }

    if (OH_Rdb_SetVersion(rdbStore_, RDB_VERSION) != RDB_OK) {
        WVLOG_E("blankless GetOrOpen set version failed");
        return;
    }
}

OhosWebSnapshotDataBase::OhosWebSnapshotDataBase() : capacityInByte_(DEFAULT_CAPACITY * BYTE_PER_MB)
{
    WVLOG_I("blankless OhosWebSnapshotDataBase create rdb store");

    AbilityRuntime_ErrorCode code = ABILITY_RUNTIME_ERROR_CODE_PARAM_INVALID;
    constexpr int32_t NATIVE_BUFFER_SIZE = 1024;
    char cacheDir[NATIVE_BUFFER_SIZE];
    int32_t cacheDirLength = 0;
    code = OH_AbilityRuntime_ApplicationContextGetCacheDir(cacheDir, NATIVE_BUFFER_SIZE, &cacheDirLength);
    if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR) {
        WVLOG_E("blankless OhosWebSnapshotDataBase context get cache dir failed, errCode:%{public}d", code);
        return;
    }
    std::string stringDir(cacheDir);
    std::string databaseDir = stringDir + WEB_PATH;

    if (access(databaseDir.c_str(), F_OK) != 0) {
        WVLOG_E("blankless OhosWebSnapshotDataBase access cache web dir failed, dir:%{public}s", databaseDir.c_str());
        return;
    }

    char bundleName[NATIVE_BUFFER_SIZE];
    int32_t bundleNameLength = 0;
    code = OH_AbilityRuntime_ApplicationContextGetBundleName(bundleName, NATIVE_BUFFER_SIZE, &bundleNameLength);
    if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR) {
        WVLOG_E("blankless OhosWebSnapshotDataBase context get bundle name failed, errCode:%{public}d", code);
        return;
    }

    AbilityRuntime_AreaMode areaMode = ABILITY_RUNTIME_AREA_MODE_EL2;
    code = OH_AbilityRuntime_ApplicationContextGetAreaMode(&areaMode);
    auto it = AREA_MODE_MAP.find(areaMode);
    if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR || it == AREA_MODE_MAP.end()) {
        WVLOG_E("blankless OhosWebSnapshotDataBase context get area mode failed, errCode:%{public}d", code);
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
        WVLOG_E("blankless ~OhosWebSnapshotDataBase rdb is null");
        return;
    }
    int errCode = OH_Rdb_CloseStore(rdbStore_);
    if (errCode == RDB_OK) {
        WVLOG_I("blankless ~OhosWebSnapshotDataBase close rdb succeed");
    } else {
        WVLOG_E("blankless ~OhosWebSnapshotDataBase close rdb failed");
    }
}

int32_t OhosWebSnapshotDataBase::GetCapacityInByte()
{
    if (rdbStore_ == nullptr) {
        WVLOG_E("blankless GetCapacityInByte rdb is null");
        return 0;
    }

    std::lock_guard<std::mutex> lock(dataBaseMapMtx_);
    WVLOG_I("blankless GetCapacityInByte capacityInByte_:%{public}d", capacityInByte_);
    return capacityInByte_;
}

int32_t OhosWebSnapshotDataBase::SetBlanklessLoadingCacheCapacity(int32_t capacity)
{
    if (rdbStore_ == nullptr) {
        WVLOG_E("blankless SetBlanklessLoadingCacheCapacity rdb is null");
        return 0;
    }

    std::lock_guard<std::mutex> lock(dataBaseMapMtx_);
    if (capacity < MIN_CAPACITY) {
        capacity = MIN_CAPACITY;
    } else if (capacity > MAX_CAPACITY) {
        capacity = MAX_CAPACITY;
    }
    UpdateCapacityData(capacity);
    capacityInByte_ = capacity * BYTE_PER_MB;

    if (capacity == MIN_CAPACITY) {
        if (dataBaseMap_.size() != 0) {
            ClearInner();
        }
        WVLOG_I("blankless SetBlanklessLoadingCacheCapacity capacity:%{public}d", capacity);
        return capacity;
    }

    while (totalSnapShotFileBytes_ > capacityInByte_) {
        DeleteInner(GetOldestKey());
    }
    WVLOG_I("blankless SetBlanklessLoadingCacheCapacity capacity:%{public}d", capacity);
    return capacity;
}

void OhosWebSnapshotDataBase::ClearSnapshotDataItem(const std::vector<int64_t>& blankless_keys)
{
    if (rdbStore_ == nullptr) {
        WVLOG_E("blankless ClearSnapshotDataItem rdb is null");
        return;
    }

    std::lock_guard<std::mutex> lock(dataBaseMapMtx_);
    if (blankless_keys.size() == 0) {
        if (dataBaseMap_.size() != 0) {
            ClearInner();
        }
        return;
    }

    for (int64_t blankless_key : blankless_keys) {
        if (dataBaseMap_.find(blankless_key) != dataBaseMap_.end()) {
            DeleteInner(blankless_key);
        }
    }
}

bool OhosWebSnapshotDataBase::InsertSnapshotDataItem(int64_t blankless_key, const SnapshotDataItem& data)
{
    return InsertSnapshotDataItem(blankless_key, data, GetCurrentTime() + MAXIMUM_TIME_LIMIT_MICRO_SECONDS);
}

bool OhosWebSnapshotDataBase::InsertSnapshotDataItem(int64_t blankless_key, const SnapshotDataItem& data,
    int64_t expirationTime)
{
    if (rdbStore_ == nullptr) {
        WVLOG_E("blankless InsertSnapshotDataItem rdb is null");
        return false;
    }

    std::lock_guard<std::mutex> lock(dataBaseMapMtx_);
    if (data.snapShotFileSize <= 0 || data.snapShotFileSize > capacityInByte_) {
        WVLOG_E("blankless InsertSnapshotDataItem failed, fileSize:%{public}ld, capacityInByte_:%{public}d",
            data.snapShotFileSize, capacityInByte_);
        return false;
    }

    if (dataBaseMap_.find(blankless_key) != dataBaseMap_.end()) {
        // The database key is unique, InsertInner will delete old data and create new data, no need to delete here.
        DeleteNotify(blankless_key);
        DeleteMap(blankless_key);
    }

    while (dataBaseMap_.size() >= MAXIMUM_SNAPSHOT_NUMBER ||
        totalSnapShotFileBytes_ + data.snapShotFileSize > capacityInByte_) {
        DeleteInner(GetOldestKey());
    }

    InsertInner(blankless_key, {data, expirationTime});
    return true;
}

SnapshotDataItem OhosWebSnapshotDataBase::GetSnapshotDataItem(int64_t blankless_key)
{
    if (rdbStore_ == nullptr) {
        WVLOG_E("blankless GetSnapshotDataItem rdb is null");
        return SnapshotDataItem{};
    }

    std::lock_guard<std::mutex> lock(dataBaseMapMtx_);
    auto it = dataBaseMap_.find(blankless_key);
    if (it == dataBaseMap_.end()) {
        WVLOG_E("blankless GetSnapshotDataItem get data failed");
        return SnapshotDataItem{};
    }

    if (GetCurrentTime() >= it->second.expirationTime) {
        WVLOG_E("blankless GetSnapshotDataItem the data has expired");
        return SnapshotDataItem{};
    }
    WVLOG_I("blankless GetSnapshotDataItem success key:%{public}ld", blankless_key);
    return it->second.snapshotData;
}

void OhosWebSnapshotDataBase::RegisterDataBaseCallback(std::shared_ptr<OhosWebSnapshotDataBaseCallback> callback)
{
    if (callback == nullptr) {
        WVLOG_E("blankless RegisterDataBaseCallback callback is null");
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
    WVLOG_I("blankless RegisterDataBaseCallback success");
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void OhosWebSnapshotDataBase::GetDatabaseInfo()
{
    OH_Predicates* rdbPredicate = OH_Rdb_CreatePredicates(DATABASE_INFO_TABLE_NAME.c_str());
    OH_VObject* valueObject = OH_Rdb_CreateValueObject();
    int64_t uniqueId = DATABASE_INFO_UNIQUE_ID;
    valueObject->putInt64(valueObject, &uniqueId, 1);
    rdbPredicate->equalTo(rdbPredicate, UNIQUE_KEY_COL.c_str(), valueObject);
    valueObject->destroy(valueObject);
    OH_Cursor* cursor = OH_Rdb_Query(rdbStore_, rdbPredicate, NULL, 0);
    rdbPredicate->destroy(rdbPredicate);
    if (cursor == nullptr) {
        return;
    }
    if (cursor->goToNextRow(cursor) != RDB_OK) {
        cursor->destroy(cursor);
        return;
    }
    int32_t capacityColumnIndex;
    int32_t errCode = cursor->getColumnIndex(cursor, CAPACITY_KEY_COL.c_str(), &capacityColumnIndex);
    if (errCode != RDB_OK) {
        WVLOG_E("blankless GetDatabaseInfo get capacity column index failed, errCode:%{public}d", errCode);
        cursor->destroy(cursor);
        return;
    }
    int64_t capacity;
    errCode = cursor->getInt64(cursor, capacityColumnIndex, &capacity);
    if (errCode != RDB_OK) {
        WVLOG_E("blankless GetDatabaseInfo get capacity value failed, errCode:%{public}d", errCode);
        cursor->destroy(cursor);
        return;
    }
    if (capacity < MIN_CAPACITY || capacity > MAX_CAPACITY) {
        WVLOG_E("blankless GetDatabaseInfo get capacity invalid, capacity:%{public}ld", capacity);
        cursor->destroy(cursor);
        return;
    }
    capacityInByte_ = capacity * BYTE_PER_MB;
    cursor->destroy(cursor);
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void OhosWebSnapshotDataBase::GetAllInfo()
{
    if (rdbStore_ == nullptr) {
        return;
    }

    GetDatabaseInfo();
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
    int32_t expirationTimeColumnIndex;
    cursor->getColumnIndex(cursor, EXPIRATION_TIME_COL.c_str(), &expirationTimeColumnIndex);
    int32_t widthIndex;
    cursor->getColumnIndex(cursor, SNAPSHOT_WIDTH.c_str(), &widthIndex);
    int32_t heightIndex;
    cursor->getColumnIndex(cursor, SNAPSHOT_HEIGHT.c_str(), &heightIndex);
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
    std::vector<int64_t> invalidKeys;
    int64_t currentTime = GetCurrentTime();
    size_t size = 0;
    do {
        if (dataBaseMap_.size() >= MAXIMUM_SNAPSHOT_NUMBER) {
            cursor->destroy(cursor);
            ClearData();
            ClearMap();
            WVLOG_E("blankless GetAllInfo too much data in the database");
            return;
        }
        int64_t expirationTime;
        cursor->getInt64(cursor, expirationTimeColumnIndex, &expirationTime);
        int64_t blankless_key = 0LL;
        cursor->getInt64(cursor, urlColumnIndex, &blankless_key);
        if (currentTime >= expirationTime) {
            invalidKeys.push_back(blankless_key);
            continue;
        }
        SnapshotDataItem dataItem;
        cursor->getInt64(cursor, snapShotFileSizeColumnIndex, &dataItem.snapShotFileSize);
        if (dataItem.snapShotFileSize <= 0 || totalSnapShotFileBytes_ + dataItem.snapShotFileSize > capacityInByte_) {
            cursor->destroy(cursor);
            ClearData();
            ClearMap();
            WVLOG_E("blankless GetAllInfo the file size is incorrect");
            return;
        }
        int64_t width;
        cursor->getInt64(cursor, widthIndex, &width);
        dataItem.width = static_cast<int32_t>(width);
        int64_t height;
        cursor->getInt64(cursor, heightIndex, &height);
        dataItem.height = static_cast<int32_t>(height);

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
        cursor->getInt64(cursor, snapShotFileTimeColumnIndex, &dataItem.snapShotFileTime);
        cursor->getInt64(cursor, preferenceHashColumnIndex, &dataItem.preferenceHash);
        InsertMap(blankless_key, {dataItem, expirationTime});
    } while (cursor->goToNextRow(cursor) == RDB_OK);
    cursor->destroy(cursor);

    for (int64_t key : invalidKeys) {
        DeleteData(key);
    }
}

int64_t OhosWebSnapshotDataBase::GetCurrentTime()
{
    auto currentTimeStamp = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(currentTimeStamp).count();
}

void OhosWebSnapshotDataBase::ClearInner()
{
    ClearData();
    ClearNotify();
    ClearMap();
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void OhosWebSnapshotDataBase::ClearData()
{
    OH_Predicates* rdbPredicate = OH_Rdb_CreatePredicates(SNAPSHOT_TABLE_NAME.c_str());
    int32_t errCode = OH_Rdb_Delete(rdbStore_, rdbPredicate);
    rdbPredicate->destroy(rdbPredicate);
    WVLOG_I("blankless ClearData errCode:%{public}d", errCode);
}

void OhosWebSnapshotDataBase::ClearNotify()
{
    for (const auto& callback : dataBaseDeleteCallbacks_) {
        callback->OnDataClear();
    }
}

void OhosWebSnapshotDataBase::ClearMap()
{
    totalSnapShotFileBytes_ = 0;
    dataBaseMap_.clear();
}

void OhosWebSnapshotDataBase::DeleteInner(int64_t blankless_key)
{
    DeleteData(blankless_key);
    DeleteNotify(blankless_key);
    DeleteMap(blankless_key);
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void OhosWebSnapshotDataBase::DeleteData(int64_t blankless_key)
{
    OH_VObject* object = OH_Rdb_CreateValueObject();
    OH_Predicates* rdbPredicate = OH_Rdb_CreatePredicates(SNAPSHOT_TABLE_NAME.c_str());
    object->putInt64(object, &blankless_key, 1);
    rdbPredicate->clear(rdbPredicate);
    rdbPredicate->equalTo(rdbPredicate, SNAPSHOT_KEY_COL.c_str(), object);
    int32_t errCode = OH_Rdb_Delete(rdbStore_, rdbPredicate);
    rdbPredicate->destroy(rdbPredicate);
    object->destroy(object);
    WVLOG_I("blankless DeleteData key:%{public}ld, errCode:%{public}d", blankless_key, errCode);
}

void OhosWebSnapshotDataBase::DeleteNotify(int64_t blankless_key)
{
    const std::string& wholePath = dataBaseMap_[blankless_key].snapshotData.wholePath;
    const std::string& staticPath = dataBaseMap_[blankless_key].snapshotData.staticPath;
    for (const auto& callback : dataBaseDeleteCallbacks_) {
        if (!wholePath.empty()) {
            callback->OnDataDelete(wholePath);
        }
        if (!staticPath.empty()) {
            callback->OnDataDelete(staticPath);
        }
    }
}

void OhosWebSnapshotDataBase::DeleteMap(int64_t blankless_key)
{
    totalSnapShotFileBytes_ -= dataBaseMap_[blankless_key].snapshotData.snapShotFileSize;
    dataBaseMap_.erase(blankless_key);
}

void OhosWebSnapshotDataBase::InsertInner(int64_t blankless_key, const DataBaseDataItem& data)
{
    InsertData(blankless_key, data);
    InsertMap(blankless_key, data);
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void OhosWebSnapshotDataBase::InsertData(int64_t blankless_key,
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
    valueBucket->putInt64(valueBucket, EXPIRATION_TIME_COL.c_str(), data.expirationTime);
    valueBucket->putInt64(valueBucket, SNAPSHOT_WIDTH.c_str(), data.snapshotData.width);
    valueBucket->putInt64(valueBucket, SNAPSHOT_HEIGHT.c_str(), data.snapshotData.height);
    int32_t errCode = OH_Rdb_Insert(rdbStore_, SNAPSHOT_TABLE_NAME.c_str(), valueBucket);
    valueBucket->destroy(valueBucket);
    WVLOG_I("blankless InsertData key:%{public}ld, errCode:%{public}d", blankless_key, errCode);
}

void OhosWebSnapshotDataBase::InsertMap(int64_t blankless_key, const DataBaseDataItem& data)
{
    totalSnapShotFileBytes_ += data.snapshotData.snapShotFileSize;
    dataBaseMap_.insert({blankless_key, data});
}

__attribute__((no_sanitize("cfi", "cfi-icall"))) void OhosWebSnapshotDataBase::UpdateCapacityData(int32_t capacity)
{
    OH_VBucket* valueBucket = OH_Rdb_CreateValuesBucket();
    valueBucket->putInt64(valueBucket, UNIQUE_KEY_COL.c_str(), DATABASE_INFO_UNIQUE_ID);
    valueBucket->putInt64(valueBucket, CAPACITY_KEY_COL.c_str(), capacity);
    int32_t errCode = OH_Rdb_Insert(rdbStore_, DATABASE_INFO_TABLE_NAME.c_str(), valueBucket);
    valueBucket->destroy(valueBucket);
    WVLOG_I("blankless UpdateCapacityData capacity:%{public}d, errCode:%{public}d", capacity, errCode);
}

int64_t OhosWebSnapshotDataBase::GetOldestKey()
{
    int64_t oldestTime = INT64_MAX;
    int64_t oldestKey = INT64_MAX;
    for (const auto& pair : dataBaseMap_) {
        if (oldestTime >= pair.second.expirationTime) {
            oldestTime = pair.second.expirationTime;
            oldestKey = pair.first;
        }
    }
    WVLOG_I("blankless GetOldestKey key:%{public}ld", oldestKey);
    return oldestKey;
}
} // namespace OHOS::NWeb