/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "ohos_adapter/bridge/ark_drm_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_drm_callback_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkDrmAdapterWrapper::ArkDrmAdapterWrapper(ArkWebRefPtr<ArkDrmAdapter> ref) : ctocpp_(ref) {}

bool ArkDrmAdapterWrapper::IsSupported(const std::string& name)
{
    ArkWebString ark_name = ArkWebStringClassToStruct(name);
    bool ret = ctocpp_->IsSupported(ark_name);
    ArkWebStringStructRelease(ark_name);

    return ret;
}

bool ArkDrmAdapterWrapper::IsSupported2(const std::string& name, const std::string& mimeType)
{
    ArkWebString ark_name = ArkWebStringClassToStruct(name);
    ArkWebString ark_mimeType = ArkWebStringClassToStruct(mimeType);
    bool ret = ctocpp_->IsSupported2(ark_name, ark_mimeType);
    ArkWebStringStructRelease(ark_name);
    ArkWebStringStructRelease(ark_mimeType);

    return ret;
}

bool ArkDrmAdapterWrapper::IsSupported3(const std::string& name, const std::string& mimeType, int32_t level)
{
    ArkWebString ark_name = ArkWebStringClassToStruct(name);
    ArkWebString ark_mimeType = ArkWebStringClassToStruct(mimeType);
    bool ret = ctocpp_->IsSupported3(ark_name, ark_mimeType, level);
    ArkWebStringStructRelease(ark_name);
    ArkWebStringStructRelease(ark_mimeType);

    return ret;
}

std::vector<uint8_t> ArkDrmAdapterWrapper::GetUUID(const std::string& name)
{
    ArkWebString ark_name = ArkWebStringClassToStruct(name);
    ArkWebUint8Vector ark_uuid = ctocpp_->GetUUID(ark_name);
    std::vector<uint8_t> uuid = ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(ark_uuid);
    ArkWebBasicVectorStructRelease<ArkWebUint8Vector>(ark_uuid);
    ArkWebStringStructRelease(ark_name);
    return uuid;
}

void ArkDrmAdapterWrapper::StorageProvisionedResult(bool result)
{
    ctocpp_->StorageProvisionedResult(result);
}

void ArkDrmAdapterWrapper::StorageSaveInfoResult(bool result, int32_t type)
{
    ctocpp_->StorageSaveInfoResult(result, type);
}

void ArkDrmAdapterWrapper::StorageLoadInfoResult(
    const std::string& sessionId, const std::vector<uint8_t>& keySetId, const std::string& mimeType, uint32_t keyType)
{
    ArkWebString arkSessionId = ArkWebStringClassToStruct(sessionId);
    ArkWebUint8Vector arkKeySetId = ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>(keySetId);
    ArkWebString arkMimeType = ArkWebStringClassToStruct(mimeType);
    ctocpp_->StorageLoadInfoResult(arkSessionId, arkKeySetId, arkMimeType, keyType);
    ArkWebStringStructRelease(arkSessionId);
    ArkWebBasicVectorStructRelease(arkKeySetId);
    ArkWebStringStructRelease(arkMimeType);
}

void ArkDrmAdapterWrapper::StorageClearInfoResult(bool result, int32_t type)
{
    ctocpp_->StorageClearInfoResult(result, type);
}

int32_t ArkDrmAdapterWrapper::CreateKeySystem(const std::string& name, const std::string& origin, int32_t securityLevel)
{
    ArkWebString ark_name = ArkWebStringClassToStruct(name);
    ArkWebString ark_origin = ArkWebStringClassToStruct(origin);
    int32_t ret = ctocpp_->CreateKeySystem(ark_name, ark_origin, securityLevel);
    ArkWebStringStructRelease(ark_name);
    ArkWebStringStructRelease(ark_origin);
    return ret;
}

int32_t ArkDrmAdapterWrapper::ReleaseMediaKeySystem()
{
    return ctocpp_->ReleaseMediaKeySystem();
}

int32_t ArkDrmAdapterWrapper::ReleaseMediaKeySession()
{
    return ctocpp_->ReleaseMediaKeySession();
}

int32_t ArkDrmAdapterWrapper::SetConfigurationString(const std::string& configName, const std::string& value)
{
    ArkWebString config = ArkWebStringClassToStruct(configName);
    ArkWebString val = ArkWebStringClassToStruct(value);
    int32_t ret = ctocpp_->SetConfigurationString(config, val);
    ArkWebStringStructRelease(config);
    ArkWebStringStructRelease(val);

    return ret;
}

int32_t ArkDrmAdapterWrapper::GetConfigurationString(const std::string& configName, char* value, int32_t valueLen)
{
    ArkWebString config = ArkWebStringClassToStruct(configName);
    int32_t ret = ctocpp_->GetConfigurationString(config, value, valueLen);
    ArkWebStringStructRelease(config);

    return ret;
}

int32_t ArkDrmAdapterWrapper::SetConfigurationByteArray(
    const std::string& configName, const uint8_t* value, int32_t valueLen)
{
    ArkWebString config = ArkWebStringClassToStruct(configName);
    int32_t ret = ctocpp_->SetConfigurationByteArray(config, value, valueLen);
    ArkWebStringStructRelease(config);

    return ret;
}

int32_t ArkDrmAdapterWrapper::GetConfigurationByteArray(
    const std::string& configName, uint8_t* value, int32_t* valueLen)
{
    ArkWebString config = ArkWebStringClassToStruct(configName);
    int32_t ret = ctocpp_->GetConfigurationByteArray(config, value, valueLen);
    ArkWebStringStructRelease(config);

    return ret;
}

int32_t ArkDrmAdapterWrapper::GetMaxContentProtectionLevel(int32_t& level)
{
    return ctocpp_->GetMaxContentProtectionLevel(level);
}

int32_t ArkDrmAdapterWrapper::ProcessKeySystemResponse(const std::string& response, bool isResponseReceived)
{
    ArkWebString ark_response = ArkWebStringClassToStruct(response);
    int32_t ret = ctocpp_->ProcessKeySystemResponse(ark_response, isResponseReceived);
    ArkWebStringStructRelease(ark_response);
    return ret;
}

int32_t ArkDrmAdapterWrapper::GetCertificateStatus(int32_t& certStatus)
{
    return ctocpp_->GetCertificateStatus(certStatus);
}

int32_t ArkDrmAdapterWrapper::RegistDrmCallback(std::shared_ptr<NWeb::DrmCallbackAdapter> callbackAdapter)
{
    if (CHECK_SHARED_PTR_IS_NULL(callbackAdapter)) {
        return ctocpp_->RegistDrmCallback(nullptr);
    }

    return ctocpp_->RegistDrmCallback(new ArkDrmCallbackAdapterImpl(callbackAdapter));
}

int32_t ArkDrmAdapterWrapper::UpdateSession(
    uint32_t promiseId, const std::string& sessionId, std::vector<uint8_t> response)
{
    ArkWebString arkSessionId = ArkWebStringClassToStruct(sessionId);
    ArkWebUint8Vector arkResponse = ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>(response);
    int32_t ret = ctocpp_->UpdateSession(promiseId, arkSessionId, arkResponse);
    ArkWebStringStructRelease(arkSessionId);
    ArkWebBasicVectorStructRelease(arkResponse);
    return ret;
}

int32_t ArkDrmAdapterWrapper::CloseSession(uint32_t promiseId, const std::string& sessionId)
{
    ArkWebString arkSessionId = ArkWebStringClassToStruct(sessionId);
    int32_t ret = ctocpp_->CloseSession(promiseId, arkSessionId);
    ArkWebStringStructRelease(arkSessionId);
    return ret;
}

int32_t ArkDrmAdapterWrapper::RemoveSession(uint32_t promiseId, const std::string& sessionId)
{
    ArkWebString arkSessionId = ArkWebStringClassToStruct(sessionId);
    int32_t ret = ctocpp_->RemoveSession(promiseId, arkSessionId);
    ArkWebStringStructRelease(arkSessionId);
    return ret;
}

int32_t ArkDrmAdapterWrapper::LoadSession(uint32_t promiseId, const std::string& sessionId)
{
    ArkWebString arkSessionId = ArkWebStringClassToStruct(sessionId);
    int32_t ret = ctocpp_->LoadSession(promiseId, arkSessionId);
    ArkWebStringStructRelease(arkSessionId);
    return ret;
}

int32_t ArkDrmAdapterWrapper::ClearMediaKeys()
{
    return ctocpp_->ClearMediaKeys();
}

int32_t ArkDrmAdapterWrapper::GetSecurityLevel()
{
    return ctocpp_->GetSecurityLevel();
}

int32_t ArkDrmAdapterWrapper::RequireSecureDecoderModule(const std::string& mimeType, bool& status)
{
    ArkWebString ark_mimeType = ArkWebStringClassToStruct(mimeType);
    int32_t ret = ctocpp_->RequireSecureDecoderModule(ark_mimeType, status);
    ArkWebStringStructRelease(ark_mimeType);

    return ret;
}

int32_t ArkDrmAdapterWrapper::GenerateMediaKeyRequest(const std::string& sessionId, int32_t type, int32_t initDataLen,
    const std::vector<uint8_t>& initData, const std::string& mimeType, uint32_t promiseId)
{
    ArkWebString ark_mimeType = ArkWebStringClassToStruct(mimeType);
    ArkWebString ark_sessionId = ArkWebStringClassToStruct(sessionId);
    ArkWebUint8Vector ark_initData = ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>(initData);
    int32_t ret =
        ctocpp_->GenerateMediaKeyRequest(ark_sessionId, type, initDataLen, ark_initData, ark_mimeType, promiseId);
    ArkWebStringStructRelease(ark_mimeType);
    ArkWebStringStructRelease(ark_sessionId);
    ArkWebBasicVectorStructRelease(ark_initData);
    return ret;
}
} // namespace OHOS::ArkWeb
