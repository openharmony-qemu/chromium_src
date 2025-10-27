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

#include "ohos_adapter/bridge/ark_ohos_web_data_base_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkOhosWebDataBaseAdapterImpl::ArkOhosWebDataBaseAdapterImpl(NWeb::OhosWebDataBaseAdapter& ref) : real_(ref) {}

bool ArkOhosWebDataBaseAdapterImpl::ExistHttpAuthCredentials()
{
    return real_.ExistHttpAuthCredentials();
}

void ArkOhosWebDataBaseAdapterImpl::DeleteHttpAuthCredentials()
{
    return real_.DeleteHttpAuthCredentials();
}

void ArkOhosWebDataBaseAdapterImpl::SaveHttpAuthCredentials(
    const ArkWebString& host, const ArkWebString& realm, const ArkWebString& username, const char* password)
{
    real_.SaveHttpAuthCredentials(ArkWebStringStructToClass(host), ArkWebStringStructToClass(realm),
        ArkWebStringStructToClass(username), password);
}

void ArkOhosWebDataBaseAdapterImpl::GetHttpAuthCredentials(
    const ArkWebString& host, const ArkWebString& realm, ArkWebString& username, char* password, uint32_t passwordSize)
{
    std::string s_username;
    real_.GetHttpAuthCredentials(
        ArkWebStringStructToClass(host), ArkWebStringStructToClass(realm), s_username, password, passwordSize);
    username = ArkWebStringClassToStruct(s_username);
}

ArkOhosWebPermissionDataBaseAdapterImpl::ArkOhosWebPermissionDataBaseAdapterImpl(
    NWeb::OhosWebPermissionDataBaseAdapter& ref)
    : real_(ref)
{}

bool ArkOhosWebPermissionDataBaseAdapterImpl::ExistPermissionByOrigin(const ArkWebString& origin, const int32_t& key)
{
    return real_.ExistPermissionByOrigin(ArkWebStringStructToClass(origin), (NWeb::WebPermissionType)key);
}

bool ArkOhosWebPermissionDataBaseAdapterImpl::GetPermissionResultByOrigin(
    const ArkWebString& origin, const int32_t& key, bool& result)
{
    return real_.GetPermissionResultByOrigin(ArkWebStringStructToClass(origin), (NWeb::WebPermissionType)key, result);
}

void ArkOhosWebPermissionDataBaseAdapterImpl::SetPermissionByOrigin(
    const ArkWebString& origin, const int32_t& key, bool result)
{
    real_.SetPermissionByOrigin(ArkWebStringStructToClass(origin), (NWeb::WebPermissionType)key, result);
}

void ArkOhosWebPermissionDataBaseAdapterImpl::ClearPermissionByOrigin(const ArkWebString& origin, const int32_t& key)
{
    real_.ClearPermissionByOrigin(ArkWebStringStructToClass(origin), (NWeb::WebPermissionType)key);
}

void ArkOhosWebPermissionDataBaseAdapterImpl::ClearAllPermission(const int32_t& key)
{
    real_.ClearAllPermission((NWeb::WebPermissionType)key);
}

void ArkOhosWebPermissionDataBaseAdapterImpl::GetOriginsByPermission(const int32_t& key, ArkWebStringVector& origins)
{
    std::vector<std::string> temp;
    real_.GetOriginsByPermission((NWeb::WebPermissionType)key, temp);
    origins = ArkWebStringVectorClassToStruct(temp);
}

} // namespace OHOS::ArkWeb
