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

#ifndef ARK_OHOS_WEB_DATA_BASE_ADAPTER_WRAPPER_H
#define ARK_OHOS_WEB_DATA_BASE_ADAPTER_WRAPPER_H
#pragma once

#include "ohos_adapter/include/ark_ohos_web_data_base_adapter.h"
#include "ohos_web_data_base_adapter.h"

namespace OHOS::ArkWeb {

class ArkOhosWebDataBaseAdapterWrapper : public OHOS::NWeb::OhosWebDataBaseAdapter {
public:
    explicit ArkOhosWebDataBaseAdapterWrapper(ArkWebRefPtr<ArkOhosWebDataBaseAdapter>);

    bool ExistHttpAuthCredentials() override;

    void DeleteHttpAuthCredentials() override;

    void SaveHttpAuthCredentials(
        const std::string& host, const std::string& realm, const std::string& username, const char* password) override;

    void GetHttpAuthCredentials(const std::string& host, const std::string& realm, std::string& username,
        char* password, uint32_t passwordSize) override;

private:
    ArkWebRefPtr<ArkOhosWebDataBaseAdapter> ctocpp_;
};

class ArkOhosWebPermissionDataBaseAdapterWrapper : public OHOS::NWeb::OhosWebPermissionDataBaseAdapter {
public:
    explicit ArkOhosWebPermissionDataBaseAdapterWrapper(ArkWebRefPtr<ArkOhosWebPermissionDataBaseAdapter>);

    bool ExistPermissionByOrigin(const std::string& origin, const OHOS::NWeb::WebPermissionType& key) override;

    bool GetPermissionResultByOrigin(
        const std::string& origin, const OHOS::NWeb::WebPermissionType& key, bool& result) override;

    void SetPermissionByOrigin(
        const std::string& origin, const OHOS::NWeb::WebPermissionType& key, bool result) override;

    void ClearPermissionByOrigin(const std::string& origin, const OHOS::NWeb::WebPermissionType& key) override;

    void ClearAllPermission(const OHOS::NWeb::WebPermissionType& key) override;

    void GetOriginsByPermission(const OHOS::NWeb::WebPermissionType& key, std::vector<std::string>& origins) override;

private:
    ArkWebRefPtr<ArkOhosWebPermissionDataBaseAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_OHOS_WEB_DATA_BASE_ADAPTER_WRAPPER_H
