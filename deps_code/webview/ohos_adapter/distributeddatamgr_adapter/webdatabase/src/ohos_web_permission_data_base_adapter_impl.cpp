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

#include "ohos_web_permission_data_base_adapter_impl.h"

#include <cinttypes>
#include <unistd.h>

#include "application_context.h"
#include "nweb_log.h"
#include "rdb_sql_utils.h"

using namespace OHOS::NativeRdb;
using namespace OHOS::NWeb;

namespace {
const int32_t RDB_VERSION = 1;
const std::string WEB_PERMISSION_DATABASE_FILE = "web_permission.db";

const std::string ID_COL = "_id";
const std::string GEOLOCATION_TABLE_NAME = "geolocation";
const std::string PERMISSION_ORIGIN_COL = "origin";
const std::string PERMISSION_RESULT_COL = "result";

const std::string CREATE_TABLE = "CREATE TABLE " + GEOLOCATION_TABLE_NAME
    + " (" + ID_COL + " INTEGER PRIMARY KEY, "
    + PERMISSION_ORIGIN_COL + " TEXT, " + PERMISSION_RESULT_COL + " INTEGER, "
    + " UNIQUE (" + PERMISSION_ORIGIN_COL + ") ON CONFLICT REPLACE);";

const std::string WEB_PATH = "/web";
}

int32_t PermissionDataBaseRdbOpenCallBack::OnCreate(OHOS::NativeRdb::RdbStore& store)
{
    WVLOG_I("web permission database opened, create table: %{public}s", CREATE_TABLE.c_str());
    return store.ExecuteSql(CREATE_TABLE);
}

int32_t PermissionDataBaseRdbOpenCallBack::OnUpgrade(OHOS::NativeRdb::RdbStore& rdbStore,
    int32_t currentVersion, int32_t targetVersion)
{
    WVLOG_I("web permission database upgrade");
    return OHOS::NativeRdb::E_OK;
}

OhosWebPermissionDataBaseAdapterImpl& OhosWebPermissionDataBaseAdapterImpl::GetInstance()
{
    WVLOG_I("webdatabase get permission data base instance");
    static OhosWebPermissionDataBaseAdapterImpl instance;
    return instance;
}

std::shared_ptr<OHOS::NativeRdb::RdbStore> OhosWebPermissionDataBaseAdapterImpl::CreateDataBase(
    const std::string& dataBeseName, OHOS::NativeRdb::RdbOpenCallback& callBack)
{
    WVLOG_I("web permission database create rdb store");
    std::shared_ptr<OHOS::NativeRdb::RdbStore> rdbStore = nullptr;
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        OHOS::AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (context == nullptr) {
        WVLOG_E("web permission database get context failed");
        return rdbStore;
    }

    std::string databaseDir = context->GetCacheDir() + WEB_PATH;
    if (access(databaseDir.c_str(), F_OK) != 0) {
        WVLOG_I("web permission database fail to access cache web dir:%{public}s", databaseDir.c_str());
        return rdbStore;
    }

    std::string bundleName = context->GetBundleName();
    std::string name = dataBeseName;
    int32_t errorCode = E_OK;
    std::string realPath = RdbSqlUtils::GetDefaultDatabasePath(databaseDir, name, errorCode);
    RdbStoreConfig config("");
    config.SetPath(std::move(realPath));
    config.SetBundleName(bundleName);
    config.SetName(std::move(name));
    config.SetArea(context->GetArea());
    WVLOG_I("web permission database databaseDir=%{public}s", databaseDir.c_str());
    WVLOG_I("web permission database bundleName=%{public}s", bundleName.c_str());

    int32_t errCode = NativeRdb::E_OK;
    rdbStore = RdbHelper::GetRdbStore(config, RDB_VERSION, callBack, errCode);
    WVLOG_I("web permission database create rdb store end, errCode=%{public}d", errCode);
    return rdbStore;
}

OhosWebPermissionDataBaseAdapterImpl::OhosWebPermissionDataBaseAdapterImpl()
{
    PermissionDataBaseRdbOpenCallBack callBack;
    rdbStore_ = OhosWebPermissionDataBaseAdapterImpl::CreateDataBase(WEB_PERMISSION_DATABASE_FILE, callBack);
}

std::string OhosWebPermissionDataBaseAdapterImpl::KeyToTableName(const WebPermissionType& key) const
{
    if (key == WebPermissionType::GEOLOCATION) {
        return GEOLOCATION_TABLE_NAME;
    }
    return "";
}

bool OhosWebPermissionDataBaseAdapterImpl::ExistPermissionByOrigin(
    const std::string& origin, const WebPermissionType& key)
{
    WVLOG_I("web permission database check exist permissions");
    if (rdbStore_ == nullptr || origin.empty()) {
        return false;
    }
    std::string tableName = KeyToTableName(key);
    if (tableName.empty()) {
        return false;
    }

    std::vector<std::string> columns;
    NativeRdb::AbsRdbPredicates dirAbsPred(tableName);
    dirAbsPred.EqualTo(PERMISSION_ORIGIN_COL, origin);
    auto resultSet = rdbStore_->Query(dirAbsPred, columns);
    if ((resultSet == nullptr) || (resultSet->GoToFirstRow() != NativeRdb::E_OK)) {
        WVLOG_E("web permissions database rdb store query failed");
        return false;
    }
    return true;
}

bool OhosWebPermissionDataBaseAdapterImpl::GetPermissionResultByOrigin(const std::string& origin,
    const WebPermissionType& key, bool& result)
{
    WVLOG_I("web permission database get permissions");
    if (rdbStore_ == nullptr || origin.empty()) {
        return false;
    }
    std::string tableName = KeyToTableName(key);
    if (tableName.empty()) {
        return false;
    }

    std::vector<std::string> columns;
    NativeRdb::AbsRdbPredicates dirAbsPred(tableName);
    dirAbsPred.EqualTo(PERMISSION_ORIGIN_COL, origin);
    auto resultSet = rdbStore_->Query(dirAbsPred, columns);
    if ((resultSet == nullptr) || (resultSet->GoToFirstRow() != NativeRdb::E_OK)) {
        WVLOG_E("web permissions database rdb store query failed");
        return false;
    }

    int32_t columnIndex;
    int32_t resultVal = 0;
    resultSet->GetColumnIndex(PERMISSION_RESULT_COL, columnIndex);
    resultSet->GetInt(columnIndex, resultVal);
    result = resultVal ? true : false;
    return true;
}

void OhosWebPermissionDataBaseAdapterImpl::SetPermissionByOrigin(const std::string& origin,
    const WebPermissionType& key, bool result)
{
    WVLOG_I("web permission database set info key:%{public}d", key);
    if (rdbStore_ == nullptr || origin.empty()) {
        return;
    }
    std::string tableName = KeyToTableName(key);
    if (tableName.empty()) {
        return;
    }
    int32_t errCode;
    int64_t outRowId;
    NativeRdb::ValuesBucket valuesBucket;
    valuesBucket.Clear();
    valuesBucket.PutString(PERMISSION_ORIGIN_COL, origin);
    valuesBucket.PutInt(PERMISSION_RESULT_COL, (int)result);
    errCode = rdbStore_->Insert(outRowId, tableName, valuesBucket);
    WVLOG_I("web permission database set info end, errCode=%{public}d", errCode);
}

void OhosWebPermissionDataBaseAdapterImpl::ClearPermissionByOrigin(const std::string& origin,
    const WebPermissionType& key)
{
    if (rdbStore_ == nullptr || origin.empty()) {
        return;
    }
    WVLOG_I("web permission database clear");
    std::string tableName = KeyToTableName(key);
    if (tableName.empty()) {
        return;
    }
    int32_t deletedRows = 0;
    NativeRdb::AbsRdbPredicates dirAbsPred(tableName);
    dirAbsPred.EqualTo(PERMISSION_ORIGIN_COL, origin);
    int32_t ret = rdbStore_->Delete(deletedRows, dirAbsPred);
    WVLOG_I("web permission database clear info: ret=%{public}d, deletedRows=%{public}d", ret, deletedRows);
}

void OhosWebPermissionDataBaseAdapterImpl::ClearAllPermission(const WebPermissionType& key)
{
    if (rdbStore_ == nullptr) {
        return;
    }
    std::string tableName = KeyToTableName(key);
    if (tableName.empty()) {
        return;
    }
    WVLOG_I("web permission database clear all permission:%{public}s info", tableName.c_str());

    int32_t deletedRows = 0;
    NativeRdb::AbsRdbPredicates dirAbsPred(tableName);
    int32_t ret = rdbStore_->Delete(deletedRows, dirAbsPred);
    WVLOG_I("web permission database clear all info: ret=%{public}d, deletedRows=%{public}d", ret, deletedRows);
}

void OhosWebPermissionDataBaseAdapterImpl::GetOriginsByPermission(const WebPermissionType& key,
    std::vector<std::string>& origins)
{
    if (rdbStore_ == nullptr) {
        return;
    }
    std::string tableName = KeyToTableName(key);
    if (tableName.empty()) {
        return;
    }
    std::vector<std::string> columns;
    NativeRdb::AbsRdbPredicates dirAbsPred(tableName);
    auto resultSet = rdbStore_->Query(dirAbsPred, columns);
    if ((resultSet == nullptr) || (resultSet->GoToFirstRow() != NativeRdb::E_OK)) {
        WVLOG_E("web permissions database rdb store query failed");
        return;
    }
    int32_t columnIndex;
    resultSet->GetColumnIndex(PERMISSION_ORIGIN_COL, columnIndex);
    WVLOG_I("web permission database origin columnIndex:%{public}d", columnIndex);
    do {
        std::string origin;
        resultSet->GetString(columnIndex, origin);
        origins.push_back(origin);
    } while (resultSet->GoToNextRow() == NativeRdb::E_OK);
}
