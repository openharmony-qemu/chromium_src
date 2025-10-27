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

#ifndef ARK_OHOS_WEB_DATA_BASE_ADAPTER_IMPL_H
#define ARK_OHOS_WEB_DATA_BASE_ADAPTER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_ohos_web_data_base_adapter.h"
#include "ohos_web_data_base_adapter.h"

namespace OHOS::ArkWeb {

class ArkOhosWebDataBaseAdapterImpl : public ArkOhosWebDataBaseAdapter {
public:
    explicit ArkOhosWebDataBaseAdapterImpl(NWeb::OhosWebDataBaseAdapter&);

    bool ExistHttpAuthCredentials() override;

    void DeleteHttpAuthCredentials() override;

    void SaveHttpAuthCredentials(const ArkWebString& host, const ArkWebString& realm, const ArkWebString& username,
        const char* password) override;

    void GetHttpAuthCredentials(const ArkWebString& host, const ArkWebString& realm, ArkWebString& username,
        char* password, uint32_t passwordSize) override;

private:
    NWeb::OhosWebDataBaseAdapter& real_;

    IMPLEMENT_REFCOUNTING(ArkOhosWebDataBaseAdapterImpl);
};

class ArkOhosWebPermissionDataBaseAdapterImpl : public ArkOhosWebPermissionDataBaseAdapter {
public:
    explicit ArkOhosWebPermissionDataBaseAdapterImpl(NWeb::OhosWebPermissionDataBaseAdapter&);

    bool ExistPermissionByOrigin(const ArkWebString& origin, const int32_t& key) override;

    bool GetPermissionResultByOrigin(const ArkWebString& origin, const int32_t& key, bool& result) override;

    void SetPermissionByOrigin(const ArkWebString& origin, const int32_t& key, bool result) override;

    void ClearPermissionByOrigin(const ArkWebString& origin, const int32_t& key) override;

    void ClearAllPermission(const int32_t& key) override;

    void GetOriginsByPermission(const int32_t& key, ArkWebStringVector& origins) override;

private:
    NWeb::OhosWebPermissionDataBaseAdapter& real_;

    IMPLEMENT_REFCOUNTING(ArkOhosWebPermissionDataBaseAdapterImpl);
};

} // namespace OHOS::ArkWeb
#endif // ARK_OHOS_WEB_DATA_BASE_ADAPTER_IMPL_H
