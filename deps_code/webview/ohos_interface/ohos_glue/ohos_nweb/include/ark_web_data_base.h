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

#ifndef ARK_WEB_DATA_BASE_H_
#define ARK_WEB_DATA_BASE_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebDataBase : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @brief delete all specifies permission type.
     *
     * @param type specifies permission type.
     * @param incognito true if web is in the incognito mode, flase otherwise.
     */
    /*--ark web()--*/
    virtual void ClearAllPermission(int type, bool incognito) = 0;

    /**
     * @brief get username and password.
     *
     * @param host the host to which the credentials apply.
     * @param realm the realm to which the credentials apply.
     * @param user_name the username.
     * @param password the password.
     * @param password_size the password array size.
     */
    /*--ark web()--*/
    virtual void GetHttpAuthCredentials(const ArkWebString& host, const ArkWebString& realm, ArkWebString& user_name,
        char* password, uint32_t password_size) = 0;

    /**
     * @brief save http authentication credentials.
     *
     * @param host the host to which the credentials apply.
     * @param realm the realm to which the credentials apply.
     * @param username the user_name.
     * @param password the password.
     */
    /*--ark web()--*/
    virtual void SaveHttpAuthCredentials(
        const ArkWebString& host, const ArkWebString& realm, const ArkWebString& user_name, const char* password) = 0;

    /**
     * @brief Get whether instances holds any http authentication credentials.
     *
     * @return true if instances saved any http authentication credentials.
     */
    /*--ark web()--*/
    virtual bool ExistHttpAuthCredentials() = 0;

    /**
     * @brief clear all saved authentication credentials.
     *
     */
    /*--ark web()--*/
    virtual void DeleteHttpAuthCredentials() = 0;

    /**
     * @brief obtains all origins of a specified permission type.
     *
     * @param type specifies permission type.
     * @param incognito true if web is in the incognito mode, flase otherwise.
     *
     * @return return all origin.
     */
    /*--ark web()--*/
    virtual ArkWebStringVector GetOriginsByPermission(int type, bool incognito) = 0;

    /**
     * @brief get specifies permission type result by origin.
     *
     * @param origin url source.
     * @param type specifies permission type.
     * @param result saved result.
     * @param incognito true if web is in the incognito mode, flase otherwise.
     *
     * @return return Whether there is a saved result.
     */
    /*--ark web()--*/
    virtual bool GetPermissionByOrigin(const ArkWebString& origin, int type, bool& result, bool incognito) = 0;

    /**
     * @brief set specifies permission type result by origin.
     *
     * @param origin url source.
     * @param type specifies permission type.
     * @param result set result.
     * @param incognito true if web is in the incognito mode, flase otherwise.
     *
     * @return 0 if successfully set specifies permission type result by origin
     *         other return error id.
     */
    /*--ark web()--*/
    virtual int SetPermissionByOrigin(const ArkWebString& origin, int type, bool result, bool incognito) = 0;

    /**
     * @brief gets whether the instance holds the specified permissions for the
     *        specified source.
     *
     * @param origin url source.
     * @param type specifies permission type.
     * @param incognito true if web is in the incognito mode, flase otherwise.
     *
     * @return true if instances saved origin specified permissions.
     */
    /*--ark web()--*/
    virtual bool ExistPermissionByOrigin(const ArkWebString& origin, int type, bool incognito) = 0;

    /**
     * @brief delete specifies permission type by origin.
     *
     * @param origin url source.
     * @param type specifies permission type.
     * @param incognito true if web is in the incognito mode, flase otherwise.
     *
     * @return 0 if successfully delete specifies permission type result by origin
     *         other return error id.
     */
    /*--ark web()--*/
    virtual int ClearPermissionByOrigin(const ArkWebString& origin, int type, bool incognito) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_DATA_BASE_H_
