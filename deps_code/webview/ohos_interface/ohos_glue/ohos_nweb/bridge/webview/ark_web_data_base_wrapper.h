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

#ifndef ARK_WEB_DATA_BASE_WRAPPER_H_
#define ARK_WEB_DATA_BASE_WRAPPER_H_
#pragma once

#include "include/nweb_data_base.h"
#include "ohos_nweb/include/ark_web_data_base.h"

namespace OHOS::ArkWeb {

class ArkWebDataBaseWrapper : public OHOS::NWeb::NWebDataBase {
public:
    ArkWebDataBaseWrapper(ArkWebRefPtr<ArkWebDataBase> ark_web_data_base);
    ~ArkWebDataBaseWrapper() = default;

    /**
     * @brief delete all specifies permission type.
     *
     * @param type specifies permission type.
     * @param incognito true if web is in the incognito mode, flase otherwise.
     */
    void ClearAllPermission(int type, bool incognito) override;

    /**
     * @brief get username and password.
     *
     * @param host the host to which the credentials apply.
     * @param realm the realm to which the credentials apply.
     * @param user_name the username.
     * @param password the password.
     * @param password_size the password array size.
     */
    void GetHttpAuthCredentials(const std::string& host, const std::string& realm, std::string& user_name,
        char* password, uint32_t password_size) override;

    /**
     * @brief save http authentication credentials.
     *
     * @param host the host to which the credentials apply.
     * @param realm the realm to which the credentials apply.
     * @param username the user_name.
     * @param password the password.
     */
    void SaveHttpAuthCredentials(
        const std::string& host, const std::string& realm, const std::string& user_name, const char* password) override;

    /**
     * @brief Get whether instances holds any http authentication credentials.
     *
     * @return true if instances saved any http authentication credentials.
     */
    bool ExistHttpAuthCredentials() override;

    /**
     * @brief clear all saved authentication credentials.
     *
     */
    void DeleteHttpAuthCredentials() override;

    /**
     * @brief obtains all origins of a specified permission type.
     *
     * @param type specifies permission type.
     * @param incognito true if web is in the incognito mode, flase otherwise.
     *
     * @return return all origin.
     */
    std::vector<std::string> GetOriginsByPermission(int type, bool incognito) override;

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
    bool GetPermissionResultByOrigin(const std::string& origin, int type, bool& result, bool incognito) override;

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
    int SetPermissionByOrigin(const std::string& origin, int type, bool result, bool incognito) override;

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
    bool ExistPermissionByOrigin(const std::string& origin, int type, bool incognito) override;

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
    int ClearPermissionByOrigin(const std::string& origin, int type, bool incognito) override;

private:
    ArkWebRefPtr<ArkWebDataBase> ark_web_data_base_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_DATA_BASE_WRAPPER_H_
