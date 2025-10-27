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

#ifndef ARK_OHOS_WEB_DATA_BASE_ADAPTER_H
#define ARK_OHOS_WEB_DATA_BASE_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkOhosWebDataBaseAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual bool ExistHttpAuthCredentials() = 0;

    /*--ark web()--*/
    virtual void DeleteHttpAuthCredentials() = 0;

    /*--ark web()--*/
    virtual void SaveHttpAuthCredentials(
        const ArkWebString& host, const ArkWebString& realm, const ArkWebString& username, const char* password) = 0;

    /*--ark web()--*/
    virtual void GetHttpAuthCredentials(const ArkWebString& host, const ArkWebString& realm, ArkWebString& username,
        char* password, uint32_t passwordSize) = 0;
};

/*--ark web(source=webview)--*/
class ArkOhosWebPermissionDataBaseAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual bool ExistPermissionByOrigin(const ArkWebString& origin, const int32_t& key) = 0;

    /*--ark web()--*/
    virtual bool GetPermissionResultByOrigin(const ArkWebString& origin, const int32_t& key, bool& result) = 0;

    /*--ark web()--*/
    virtual void SetPermissionByOrigin(const ArkWebString& origin, const int32_t& key, bool result) = 0;

    /*--ark web()--*/
    virtual void ClearPermissionByOrigin(const ArkWebString& origin, const int32_t& key) = 0;

    /*--ark web()--*/
    virtual void ClearAllPermission(const int32_t& key) = 0;

    /*--ark web()--*/
    virtual void GetOriginsByPermission(const int32_t& key, ArkWebStringVector& origins) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_OHOS_WEB_DATA_BASE_ADAPTER_H
