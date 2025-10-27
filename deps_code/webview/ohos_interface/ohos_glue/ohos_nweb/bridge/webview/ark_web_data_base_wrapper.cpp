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

#include "ohos_nweb/bridge/ark_web_data_base_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebDataBaseWrapper::ArkWebDataBaseWrapper(ArkWebRefPtr<ArkWebDataBase> ark_web_data_base)
    : ark_web_data_base_(ark_web_data_base)
{}

void ArkWebDataBaseWrapper::ClearAllPermission(int type, bool incognito)
{
    ark_web_data_base_->ClearAllPermission(type, incognito);
}

void ArkWebDataBaseWrapper::GetHttpAuthCredentials(
    const std::string& host, const std::string& realm, std::string& user_name, char* password, uint32_t password_size)
{
    ArkWebString stHost = ArkWebStringClassToStruct(host);
    ArkWebString stRealm = ArkWebStringClassToStruct(realm);

    ArkWebString stUserName;
    ark_web_data_base_->GetHttpAuthCredentials(stHost, stRealm, stUserName, password, password_size);
    user_name = ArkWebStringStructToClass(stUserName);

    ArkWebStringStructRelease(stHost);
    ArkWebStringStructRelease(stRealm);
    ArkWebStringStructRelease(stUserName);
}

void ArkWebDataBaseWrapper::SaveHttpAuthCredentials(
    const std::string& host, const std::string& realm, const std::string& user_name, const char* password)
{
    ArkWebString stHost = ArkWebStringClassToStruct(host);
    ArkWebString stRealm = ArkWebStringClassToStruct(realm);
    ArkWebString stUserName = ArkWebStringClassToStruct(user_name);

    ark_web_data_base_->SaveHttpAuthCredentials(stHost, stRealm, stUserName, password);

    ArkWebStringStructRelease(stHost);
    ArkWebStringStructRelease(stRealm);
    ArkWebStringStructRelease(stUserName);
}

bool ArkWebDataBaseWrapper::ExistHttpAuthCredentials()
{
    return ark_web_data_base_->ExistHttpAuthCredentials();
}

void ArkWebDataBaseWrapper::DeleteHttpAuthCredentials()
{
    ark_web_data_base_->DeleteHttpAuthCredentials();
}

std::vector<std::string> ArkWebDataBaseWrapper::GetOriginsByPermission(int type, bool incognito)
{
    ArkWebStringVector stOrigins = ark_web_data_base_->GetOriginsByPermission(type, incognito);

    std::vector<std::string> objOrigins = ArkWebStringVectorStructToClass(stOrigins);

    ArkWebStringVectorStructRelease(stOrigins);
    return objOrigins;
}

bool ArkWebDataBaseWrapper::GetPermissionResultByOrigin(
    const std::string& origin, int type, bool& result, bool incognito)
{
    ArkWebString stOrigin = ArkWebStringClassToStruct(origin);

    bool flag = ark_web_data_base_->GetPermissionByOrigin(stOrigin, type, result, incognito);

    ArkWebStringStructRelease(stOrigin);
    return flag;
}

int ArkWebDataBaseWrapper::SetPermissionByOrigin(const std::string& origin, int type, bool result, bool incognito)
{
    ArkWebString stOrigin = ArkWebStringClassToStruct(origin);

    int code = ark_web_data_base_->SetPermissionByOrigin(stOrigin, type, result, incognito);

    ArkWebStringStructRelease(stOrigin);
    return code;
}

bool ArkWebDataBaseWrapper::ExistPermissionByOrigin(const std::string& origin, int type, bool incognito)
{
    ArkWebString stOrigin = ArkWebStringClassToStruct(origin);

    bool flag = ark_web_data_base_->ExistPermissionByOrigin(stOrigin, type, incognito);

    ArkWebStringStructRelease(stOrigin);
    return flag;
}

int ArkWebDataBaseWrapper::ClearPermissionByOrigin(const std::string& origin, int type, bool incognito)
{
    ArkWebString stOrigin = ArkWebStringClassToStruct(origin);

    int code = ark_web_data_base_->ClearPermissionByOrigin(stOrigin, type, incognito);

    ArkWebStringStructRelease(stOrigin);
    return code;
}

} // namespace OHOS::ArkWeb
