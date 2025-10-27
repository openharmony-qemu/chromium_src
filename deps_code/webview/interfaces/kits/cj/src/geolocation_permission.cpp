/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "geolocation_permission.h"

#include <cstdint>
#include <vector>

#include "nweb_data_base.h"
#include "nweb_helper.h"
#include "web_errors.h"
#include "securec.h"

namespace {
constexpr int32_t INTERFACE_OK = 0;
constexpr int32_t INTERFACE_ERROR = -1;
constexpr int32_t ALLOW_PERMISSION_OPERATION = 1;
constexpr int32_t DELETE_PERMISSION_OPERATION = 2;

} // namespace

namespace OHOS {
namespace NWeb {

void GeolocationPermission::ProcessActionByType(std::string origin, bool incognitoMode,
    int32_t operationType, int32_t *errCode)
{
    std::shared_ptr<OHOS::NWeb::NWebDataBase> dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
    if (!dataBase) {
        return;
    }
    if (operationType == ALLOW_PERMISSION_OPERATION) {
        if (dataBase->SetPermissionByOrigin(origin, OHOS::NWeb::NWebDataBase::WebPermissionType::GEOLOCATION_TYPE, true,
            incognitoMode) == NWebError::INVALID_ORIGIN) {
            *errCode = NWebError::INVALID_ORIGIN;
            return;
        }
    } else if (operationType == DELETE_PERMISSION_OPERATION) {
        if (dataBase->ClearPermissionByOrigin(origin, OHOS::NWeb::NWebDataBase::WebPermissionType::GEOLOCATION_TYPE,
            incognitoMode) == NWebError::INVALID_ORIGIN) {
            *errCode =NWebError::INVALID_ORIGIN;
            return;
        }
    }
    return;
}

bool GeolocationPermission::ExecuteGetPermissionState(std::string origin, bool incognitoMode, int32_t *errCode)
{
    bool retValue = false;
    std::shared_ptr<OHOS::NWeb::NWebDataBase> dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
    if (!dataBase) {
        *errCode = INTERFACE_ERROR;
        return retValue;
    }
    if (dataBase->GetPermissionResultByOrigin(origin,
        OHOS::NWeb::NWebDataBase::WebPermissionType::GEOLOCATION_TYPE, retValue, incognitoMode)) {
        *errCode = INTERFACE_OK;
    } else {
        *errCode = NWebError::INVALID_ORIGIN;
    }
    return retValue;
}


std::vector<std::string> GeolocationPermission::ExecuteGetOrigins(bool incognitoMode, int32_t *errCode)
{
    std::vector<std::string> origins;
    std::shared_ptr<OHOS::NWeb::NWebDataBase> dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
    if (!dataBase) {
        *errCode = INTERFACE_ERROR;
        return origins;
    }
    origins = dataBase->GetOriginsByPermission(
        OHOS::NWeb::NWebDataBase::WebPermissionType::GEOLOCATION_TYPE, incognitoMode);
    *errCode = INTERFACE_OK;
    return origins;
}

void GeolocationPermission::CjAllowGeolocation(std::string origin, bool incognitoMode, int32_t *errCode)
{
    return ProcessActionByType(origin, incognitoMode, ALLOW_PERMISSION_OPERATION, errCode);
}

void GeolocationPermission::CjDeleteGeolocation(std::string origin, bool incognitoMode, int32_t *errCode)
{
    return ProcessActionByType(origin, incognitoMode, DELETE_PERMISSION_OPERATION, errCode);
}

bool GeolocationPermission::CjGetAccessibleGeolocation(std::string origin, bool incognitoMode, int32_t *errCode)
{
    return ExecuteGetPermissionState(origin, incognitoMode, errCode);
}

std::vector<std::string> GeolocationPermission::CjGetStoredGeolocation(bool incognitoMode, int32_t *errCode)
{
    return ExecuteGetOrigins(incognitoMode, errCode);
}

void GeolocationPermission::CjDeleteAllGeolocation(bool incognitoMode, int32_t *errCode)
{
    std::shared_ptr<OHOS::NWeb::NWebDataBase> dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
    if (dataBase != nullptr) {
        dataBase->ClearAllPermission(OHOS::NWeb::NWebDataBase::WebPermissionType::GEOLOCATION_TYPE, incognitoMode);
    }
    return;
}

} // namespace NWeb
} // namespace OHOS
