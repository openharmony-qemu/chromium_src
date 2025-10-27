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

#ifndef OHOS_WEB_DATA_BASE_ADAPTER_H
#define OHOS_WEB_DATA_BASE_ADAPTER_H

#include <string>
#include <vector>

namespace OHOS::NWeb {

class OhosWebDataBaseAdapter {
public:
    OhosWebDataBaseAdapter() = default;

    virtual ~OhosWebDataBaseAdapter() = default;

    virtual bool ExistHttpAuthCredentials() = 0;

    virtual void DeleteHttpAuthCredentials() = 0;

    virtual void SaveHttpAuthCredentials(
        const std::string& host, const std::string& realm, const std::string& username, const char* password) = 0;

    virtual void GetHttpAuthCredentials(const std::string& host, const std::string& realm, std::string& username,
        char* password, uint32_t passwordSize) = 0;
};

enum class WebPermissionType : int32_t { NONE_TYPE, GEOLOCATION };

class OhosWebPermissionDataBaseAdapter {
public:
    OhosWebPermissionDataBaseAdapter() = default;

    virtual ~OhosWebPermissionDataBaseAdapter() = default;

    virtual bool ExistPermissionByOrigin(const std::string& origin, const WebPermissionType& key) = 0;

    virtual bool GetPermissionResultByOrigin(const std::string& origin, const WebPermissionType& key, bool& result) = 0;

    virtual void SetPermissionByOrigin(const std::string& origin, const WebPermissionType& key, bool result) = 0;

    virtual void ClearPermissionByOrigin(const std::string& origin, const WebPermissionType& key) = 0;

    virtual void ClearAllPermission(const WebPermissionType& key) = 0;

    virtual void GetOriginsByPermission(const WebPermissionType& key, std::vector<std::string>& origins) = 0;
};

} // namespace OHOS::NWeb

#endif // OHOS_WEB_DATA_BASE_ADAPTER_H
