/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ohos_web_nweb_stats_adapter_impl.h"

#include <cinttypes>
#include <cstdlib>
#include <unistd.h>
#include <unordered_map>

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "third_party/bounds_checking_function/include/securec.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include <AbilityKit/ability_runtime/application_context.h>

using namespace OHOS::NWeb;

namespace {
static const int32_t RDB_VERSION = 1;
static const std::string NWEB_STATS_DATABASE_FILE = "nweb_stats.db";

static const std::string STATS_TABLE_NAME = "nweb_stats";
static const std::string KEY_COL = "key";
static const std::string VALUE_COL = "value";
static const std::string MAX_INSTANCE_COUNT_KEY = "max_instance_count";
static const std::string MAX_INSTANCE_COUNT_TIMESTAMP_KEY = "max_instance_count_timestamp";
static const int64_t DEFAULT_RESET_INTERVAL_SECONDS = 604800; // 7 days

static const std::string CREATE_TABLE = "CREATE TABLE " + STATS_TABLE_NAME
    + " (" + KEY_COL + " TEXT PRIMARY KEY, "
    + VALUE_COL + " INTEGER);";

const std::string WEB_PATH = "/web";

const std::unordered_map<AbilityRuntime_AreaMode, Rdb_SecurityArea> AREA_MODE_MAP = {
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL1, Rdb_SecurityArea::RDB_SECURITY_AREA_EL1 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL2, Rdb_SecurityArea::RDB_SECURITY_AREA_EL2 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL3, Rdb_SecurityArea::RDB_SECURITY_AREA_EL3 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL4, Rdb_SecurityArea::RDB_SECURITY_AREA_EL4 },
    { AbilityRuntime_AreaMode::ABILITY_RUNTIME_AREA_MODE_EL5, Rdb_SecurityArea::RDB_SECURITY_AREA_EL5 }
};
}

OhosWebNWebStatsAdapterImpl& OhosWebNWebStatsAdapterImpl::GetInstance()
{
    WVLOG_D("nweb_stats get instance");
    static OhosWebNWebStatsAdapterImpl instance;
    return instance;
}

Rdb_SecurityArea OhosWebNWebStatsAdapterImpl::GetAreaMode(AbilityRuntime_AreaMode areaMode)
{
    auto mode = AREA_MODE_MAP.find(areaMode);
    if (mode == AREA_MODE_MAP.end()) {
        WVLOG_E("nweb_stats no find areaMode in map");
        return RDB_SECURITY_AREA_EL2;
    }
    return mode->second;
}

bool OhosWebNWebStatsAdapterImpl::InitDatabase()
{
    AbilityRuntime_ErrorCode code = ABILITY_RUNTIME_ERROR_CODE_PARAM_INVALID;
    constexpr int32_t NATIVE_BUFFER_SIZE = 1024;
    char cacheDir[NATIVE_BUFFER_SIZE];
    int32_t cacheDirLength = 0;
    code = OH_AbilityRuntime_ApplicationContextGetCacheDir(cacheDir, NATIVE_BUFFER_SIZE, &cacheDirLength);
    if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR) {
        WVLOG_E("nweb_stats OH_AbilityRuntime_ApplicationContextGetCacheDir failed:err=%{public}d", code);
        return false;
    }
    std::string stringDir(cacheDir);
    std::string databaseDir = stringDir + WEB_PATH;

    if (access(databaseDir.c_str(), F_OK) != 0) {
        WVLOG_E("nweb_stats fail to access cache web dir:%{public}s", databaseDir.c_str());
        return false;
    }

    char bundleName[NATIVE_BUFFER_SIZE];
    int32_t bundleNameLength = 0;
    code = OH_AbilityRuntime_ApplicationContextGetBundleName(bundleName, NATIVE_BUFFER_SIZE, &bundleNameLength);
    if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR) {
        WVLOG_E("nweb_stats OH_AbilityRuntime_ApplicationContextGetBundleName failed:err=%{public}d", code);
        return false;
    }

    AbilityRuntime_AreaMode areaMode = ABILITY_RUNTIME_AREA_MODE_EL2;
    code = OH_AbilityRuntime_ApplicationContextGetAreaMode(&areaMode);
    if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR) {
        WVLOG_E("nweb_stats OH_AbilityRuntime_ApplicationContextGetAreaMode failed:err=%{public}d", code);
        return false;
    }

    std::string name = NWEB_STATS_DATABASE_FILE;
    OH_Rdb_Config config = {0};
    config.selfSize = sizeof(OH_Rdb_Config);
    config.dataBaseDir = databaseDir.c_str();
    config.bundleName = bundleName;
    config.storeName = name.c_str();
    config.area = GetAreaMode(areaMode);
    config.isEncrypt = false;
    config.securityLevel = OH_Rdb_SecurityLevel::S1;

    int errCode = static_cast<int>(RDB_OK);
    rdbStore_ = OH_Rdb_GetOrOpen(&config, &errCode);
    if (rdbStore_ == nullptr) {
        WVLOG_E("nweb_stats get rdb store failed, errCode=%{public}d", errCode);
        return false;
    }

    int version = RDB_VERSION;
    if (OH_Rdb_GetVersion(rdbStore_, &version) != RDB_OK) {
        WVLOG_E("nweb_stats get rdb version failed");
        return false;
    }

    if (version == 0) {
        if (OH_Rdb_Execute(rdbStore_, CREATE_TABLE.c_str()) != RDB_OK) {
            WVLOG_E("nweb_stats create table failed");
            return false;
        }
        if (OH_Rdb_SetVersion(rdbStore_, RDB_VERSION) != RDB_OK) {
            WVLOG_E("nweb_stats set version failed");
            return false;
        }
        WVLOG_I("nweb_stats create rdb succeed");
    } else {
        WVLOG_I("nweb_stats rdb already exists");
    }

    return true;
}

OhosWebNWebStatsAdapterImpl::OhosWebNWebStatsAdapterImpl()
{
    dbInitialized_ = InitDatabase();
}

OhosWebNWebStatsAdapterImpl::~OhosWebNWebStatsAdapterImpl()
{
    if (rdbStore_ != nullptr) {
        int errCode = OH_Rdb_CloseStore(rdbStore_);
        if (errCode == RDB_OK) {
            WVLOG_I("nweb_stats delete rdb succeed");
        } else {
            WVLOG_E("nweb_stats delete rdb failed");
        }
        rdbStore_ = nullptr;
    }
}

int64_t OhosWebNWebStatsAdapterImpl::QueryInt64Value(const std::string& key)
{
    if (rdbStore_ == nullptr) {
        WVLOG_E("nweb_stats rdbStore_ is null, query failed");
        return 0;
    }

    OH_Predicates *predicates = OH_Rdb_CreatePredicates(STATS_TABLE_NAME.c_str());
    if (predicates == nullptr) {
        WVLOG_E("nweb_stats create predicates failed");
        return 0;
    }

    OH_VObject *valueObject = OH_Rdb_CreateValueObject();
    if (valueObject == nullptr) {
        WVLOG_E("nweb_stats create value object failed");
        predicates->destroy(predicates);
        return 0;
    }
    valueObject->putText(valueObject, key.c_str());
    predicates->equalTo(predicates, KEY_COL.c_str(), valueObject);
    valueObject->destroy(valueObject);

    OH_Cursor *cursor = OH_Rdb_Query(rdbStore_, predicates, NULL, 0);
    predicates->destroy(predicates);

    if (cursor == nullptr || cursor->goToNextRow(cursor) != RDB_OK) {
        if (cursor != nullptr) {
            cursor->destroy(cursor);
        }
        return 0;
    }

    int64_t value = 0;
    int columnIndex = 0;
    cursor->getColumnIndex(cursor, VALUE_COL.c_str(), &columnIndex);
    cursor->getInt64(cursor, columnIndex, &value);
    cursor->destroy(cursor);

    WVLOG_I("nweb_stats query %{public}s=%{public}" PRId64, key.c_str(), value);
    return value;
}

void OhosWebNWebStatsAdapterImpl::StoreInt64Value(const std::string& key, int64_t value)
{
    if (rdbStore_ == nullptr) {
        WVLOG_E("nweb_stats rdbStore_ is null, store failed");
        return;
    }

    // Delete existing row first to avoid PRIMARY KEY conflict
    OH_Predicates *predicates = OH_Rdb_CreatePredicates(STATS_TABLE_NAME.c_str());
    if (predicates != nullptr) {
        OH_VObject *obj = OH_Rdb_CreateValueObject();
        if (obj != nullptr) {
            obj->putText(obj, key.c_str());
            predicates->equalTo(predicates, KEY_COL.c_str(), obj);
            OH_Rdb_Delete(rdbStore_, predicates);
            obj->destroy(obj);
        }
        predicates->destroy(predicates);
    }

    OH_VBucket* valueBucket = OH_Rdb_CreateValuesBucket();
    if (valueBucket == nullptr) {
        WVLOG_E("nweb_stats create values bucket failed");
        return;
    }
    valueBucket->clear(valueBucket);
    valueBucket->putText(valueBucket, KEY_COL.c_str(), key.c_str());
    valueBucket->putInt64(valueBucket, VALUE_COL.c_str(), value);
    int64_t rowId = OH_Rdb_Insert(rdbStore_, STATS_TABLE_NAME.c_str(), valueBucket);
    valueBucket->destroy(valueBucket);
    if (rowId <= 0) {
        WVLOG_E("nweb_stats store %{public}s failed, errCode=%{public}" PRId64, key.c_str(), rowId);
        return;
    }
    WVLOG_I("nweb_stats store %{public}s=%{public}" PRId64, key.c_str(), value);
}

int64_t OhosWebNWebStatsAdapterImpl::GetResetIntervalSeconds()
{
    std::string value = OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetSystemPropertiesInstance()
        .GetStringParameter("web.max_instance_reset_seconds", "604800");
    int64_t seconds = std::atoll(value.c_str());
    if (seconds <= 0) {
        WVLOG_W("nweb_stats invalid reset interval %{public}s, fallback to default", value.c_str());
        seconds = DEFAULT_RESET_INTERVAL_SECONDS;
    }
    WVLOG_D("nweb_stats reset interval=%{public}" PRId64 " seconds", seconds);
    return seconds;
}

uint32_t OhosWebNWebStatsAdapterImpl::GetMaxInstanceCount()
{
    if (!dbInitialized_ || rdbStore_ == nullptr) {
        WVLOG_E("nweb_stats db not initialized, return 0");
        return 0;
    }

    int64_t rawCount = QueryInt64Value(MAX_INSTANCE_COUNT_KEY);
    if (rawCount < 0) {
        WVLOG_W("nweb_stats invalid count=%{public}" PRId64 ", reset to 0", rawCount);
        return 0;
    }
    uint32_t count = static_cast<uint32_t>(rawCount);

    int64_t timestamp = QueryInt64Value(MAX_INSTANCE_COUNT_TIMESTAMP_KEY);
    if (timestamp == 0) {
        // Old version upgrade: no timestamp, write current timestamp and keep count
        StoreInt64Value(MAX_INSTANCE_COUNT_TIMESTAMP_KEY, static_cast<int64_t>(time(nullptr)));
        WVLOG_I("nweb_stats upgrade: set timestamp, keep count=%{public}u", count);
        return count;
    }

    int64_t intervalSeconds = GetResetIntervalSeconds();
    int64_t now = static_cast<int64_t>(time(nullptr));
    if (now - timestamp > intervalSeconds) {
        WVLOG_I("nweb_stats expired: now=%{public}" PRId64 " ts=%{public}" PRId64
                " interval=%{public}" PRId64 ", reset to 0",
                now, timestamp, intervalSeconds);
        return 0;
    }

    WVLOG_I("nweb_stats valid: count=%{public}u ts=%{public}" PRId64, count, timestamp);
    return count;
}

void OhosWebNWebStatsAdapterImpl::UpdateMaxInstanceCount(uint32_t count)
{
    if (!dbInitialized_ || rdbStore_ == nullptr) {
        WVLOG_E("nweb_stats db not initialized, skip update");
        return;
    }

    StoreInt64Value(MAX_INSTANCE_COUNT_KEY, static_cast<int64_t>(count));
    StoreInt64Value(MAX_INSTANCE_COUNT_TIMESTAMP_KEY, static_cast<int64_t>(time(nullptr)));
    WVLOG_I("nweb_stats update max_instance_count=%{public}u", count);
}
