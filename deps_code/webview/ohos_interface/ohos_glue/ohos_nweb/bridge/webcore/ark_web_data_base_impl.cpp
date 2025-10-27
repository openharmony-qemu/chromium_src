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

#include "ohos_nweb/bridge/ark_web_data_base_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebDataBaseImpl::ArkWebDataBaseImpl(std::shared_ptr<OHOS::NWeb::NWebDataBase> nweb_data_base)
    : nweb_data_base_(nweb_data_base)
{}

void ArkWebDataBaseImpl::ClearAllPermission(int type, bool incognito)
{
    nweb_data_base_->ClearAllPermission(type, incognito);
}

void ArkWebDataBaseImpl::GetHttpAuthCredentials(const ArkWebString& host, const ArkWebString& realm,
    ArkWebString& user_name, char* password, uint32_t password_size)
{
    std::string cls_user_name;
    nweb_data_base_->GetHttpAuthCredentials(
        ArkWebStringStructToClass(host), ArkWebStringStructToClass(realm), cls_user_name, password, password_size);
    user_name = ArkWebStringClassToStruct(cls_user_name);
}

void ArkWebDataBaseImpl::SaveHttpAuthCredentials(
    const ArkWebString& host, const ArkWebString& realm, const ArkWebString& user_name, const char* password)
{
    nweb_data_base_->SaveHttpAuthCredentials(ArkWebStringStructToClass(host), ArkWebStringStructToClass(realm),
        ArkWebStringStructToClass(user_name), password);
}

bool ArkWebDataBaseImpl::ExistHttpAuthCredentials()
{
    return nweb_data_base_->ExistHttpAuthCredentials();
}

void ArkWebDataBaseImpl::DeleteHttpAuthCredentials()
{
    nweb_data_base_->DeleteHttpAuthCredentials();
}

ArkWebStringVector ArkWebDataBaseImpl::GetOriginsByPermission(int type, bool incognito)
{
    return ArkWebStringVectorClassToStruct(nweb_data_base_->GetOriginsByPermission(type, incognito));
}

bool ArkWebDataBaseImpl::GetPermissionByOrigin(const ArkWebString& origin, int type, bool& result, bool incognito)
{
    return nweb_data_base_->GetPermissionResultByOrigin(ArkWebStringStructToClass(origin), type, result, incognito);
}

int ArkWebDataBaseImpl::SetPermissionByOrigin(const ArkWebString& origin, int type, bool result, bool incognito)
{
    return nweb_data_base_->SetPermissionByOrigin(ArkWebStringStructToClass(origin), type, result, incognito);
}

bool ArkWebDataBaseImpl::ExistPermissionByOrigin(const ArkWebString& origin, int type, bool incognito)
{
    return nweb_data_base_->ExistPermissionByOrigin(ArkWebStringStructToClass(origin), type, incognito);
}

int ArkWebDataBaseImpl::ClearPermissionByOrigin(const ArkWebString& origin, int type, bool incognito)
{
    return nweb_data_base_->ClearPermissionByOrigin(ArkWebStringStructToClass(origin), type, incognito);
}

} // namespace OHOS::ArkWeb
