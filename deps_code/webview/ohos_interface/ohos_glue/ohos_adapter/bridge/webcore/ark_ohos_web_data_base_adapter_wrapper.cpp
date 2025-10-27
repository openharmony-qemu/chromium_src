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

#include "ohos_adapter/bridge/ark_ohos_web_data_base_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkOhosWebDataBaseAdapterWrapper::ArkOhosWebDataBaseAdapterWrapper(ArkWebRefPtr<ArkOhosWebDataBaseAdapter> ref)
    : ctocpp_(ref)
{}

bool ArkOhosWebDataBaseAdapterWrapper::ExistHttpAuthCredentials()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->ExistHttpAuthCredentials();
}

void ArkOhosWebDataBaseAdapterWrapper::DeleteHttpAuthCredentials()
{
    if (!ctocpp_) {
        return;
    }
    return ctocpp_->DeleteHttpAuthCredentials();
}

void ArkOhosWebDataBaseAdapterWrapper::SaveHttpAuthCredentials(
    const std::string& host, const std::string& realm, const std::string& username, const char* password)
{
    if (!ctocpp_) {
        return;
    }
    ArkWebString ark_host = ArkWebStringClassToStruct(host);
    ArkWebString ark_realm = ArkWebStringClassToStruct(realm);
    ArkWebString ark_username = ArkWebStringClassToStruct(username);
    ctocpp_->SaveHttpAuthCredentials(ark_host, ark_realm, ark_username, password);
    ArkWebStringStructRelease(ark_host);
    ArkWebStringStructRelease(ark_realm);
    ArkWebStringStructRelease(ark_username);
}

void ArkOhosWebDataBaseAdapterWrapper::GetHttpAuthCredentials(
    const std::string& host, const std::string& realm, std::string& username, char* password, uint32_t passwordSize)
{
    if (!ctocpp_) {
        return;
    }
    ArkWebString ark_host = ArkWebStringClassToStruct(host);
    ArkWebString ark_realm = ArkWebStringClassToStruct(realm);
    ArkWebString ark_username = ArkWebStringClassToStruct(username);
    ctocpp_->GetHttpAuthCredentials(ark_host, ark_realm, ark_username, password, passwordSize);
    username = ArkWebStringStructToClass(ark_username);
    ArkWebStringStructRelease(ark_host);
    ArkWebStringStructRelease(ark_realm);
    ArkWebStringStructRelease(ark_username);
}

ArkOhosWebPermissionDataBaseAdapterWrapper::ArkOhosWebPermissionDataBaseAdapterWrapper(
    ArkWebRefPtr<ArkOhosWebPermissionDataBaseAdapter> ref)
    : ctocpp_(ref)
{}

bool ArkOhosWebPermissionDataBaseAdapterWrapper::ExistPermissionByOrigin(
    const std::string& origin, const OHOS::NWeb::WebPermissionType& key)
{
    if (!ctocpp_) {
        return false;
    }

    ArkWebString ark_origin = ArkWebStringClassToStruct(origin);
    const int32_t ark_key = (int32_t)key;
    bool result = ctocpp_->ExistPermissionByOrigin(ark_origin, ark_key);
    ArkWebStringStructRelease(ark_origin);
    return result;
}

bool ArkOhosWebPermissionDataBaseAdapterWrapper::GetPermissionResultByOrigin(
    const std::string& origin, const OHOS::NWeb::WebPermissionType& key, bool& result)
{
    if (!ctocpp_) {
        return false;
    }

    ArkWebString ark_origin = ArkWebStringClassToStruct(origin);
    const int32_t ark_key = (int32_t)key;
    bool res = ctocpp_->GetPermissionResultByOrigin(ark_origin, ark_key, result);
    ArkWebStringStructRelease(ark_origin);
    return res;
}

void ArkOhosWebPermissionDataBaseAdapterWrapper::SetPermissionByOrigin(
    const std::string& origin, const OHOS::NWeb::WebPermissionType& key, bool result)
{
    if (!ctocpp_) {
        return;
    }
    ArkWebString ark_origin = ArkWebStringClassToStruct(origin);
    const int32_t ark_key = (int32_t)key;
    ctocpp_->SetPermissionByOrigin(ark_origin, ark_key, result);
    ArkWebStringStructRelease(ark_origin);
}

void ArkOhosWebPermissionDataBaseAdapterWrapper::ClearPermissionByOrigin(
    const std::string& origin, const OHOS::NWeb::WebPermissionType& key)
{
    if (!ctocpp_) {
        return;
    }
    ArkWebString ark_origin = ArkWebStringClassToStruct(origin);
    const int32_t ark_key = (int32_t)key;
    ctocpp_->ClearPermissionByOrigin(ark_origin, ark_key);
    ArkWebStringStructRelease(ark_origin);
}

void ArkOhosWebPermissionDataBaseAdapterWrapper::ClearAllPermission(const OHOS::NWeb::WebPermissionType& key)
{
    if (!ctocpp_) {
        return;
    }
    const int32_t ark_key = (int32_t)key;
    return ctocpp_->ClearAllPermission(ark_key);
}

void ArkOhosWebPermissionDataBaseAdapterWrapper::GetOriginsByPermission(
    const OHOS::NWeb::WebPermissionType& key, std::vector<std::string>& origins)
{
    if (!ctocpp_) {
        return;
    }
    const int32_t ark_key = (int32_t)key;
    ArkWebStringVector vec;
    ctocpp_->GetOriginsByPermission(ark_key, vec);
    origins = ArkWebStringVectorStructToClass(vec);
    ArkWebStringVectorStructRelease(vec);
}

} // namespace OHOS::ArkWeb
