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

#include "ohos_adapter/bridge/ark_drm_adapter_impl.h"

#include <string>

#include "ohos_adapter/bridge/ark_drm_callback_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkDrmAdapterImpl::ArkDrmAdapterImpl(std::shared_ptr<OHOS::NWeb::DrmAdapter> ref) : real_(ref) {}

bool ArkDrmAdapterImpl::IsSupported(const ArkWebString& name)
{
    bool ret = real_->IsSupported(ArkWebStringStructToClass(name));

    return ret;
}

bool ArkDrmAdapterImpl::IsSupported2(const ArkWebString& name, const ArkWebString& mimeType)
{
    bool ret = real_->IsSupported2(ArkWebStringStructToClass(name), ArkWebStringStructToClass(mimeType));

    return ret;
}

bool ArkDrmAdapterImpl::IsSupported3(const ArkWebString& name, const ArkWebString& mimeType, int32_t level)
{
    bool ret = real_->IsSupported3(ArkWebStringStructToClass(name), ArkWebStringStructToClass(mimeType), level);

    return ret;
}

ArkWebUint8Vector ArkDrmAdapterImpl::GetUUID(const ArkWebString& name)
{
    return ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>(real_->GetUUID(ArkWebStringStructToClass(name)));
}

void ArkDrmAdapterImpl::StorageProvisionedResult(bool result)
{
    real_->StorageProvisionedResult(result);
}

void ArkDrmAdapterImpl::StorageSaveInfoResult(bool result, int32_t type)
{
    real_->StorageSaveInfoResult(result, type);
}

void ArkDrmAdapterImpl::StorageLoadInfoResult(
    const ArkWebString& sessionId, const ArkWebUint8Vector& keySetId, const ArkWebString& mimeType, uint32_t keyType)
{
    real_->StorageLoadInfoResult(ArkWebStringStructToClass(sessionId),
        ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(keySetId), ArkWebStringStructToClass(mimeType),
        keyType);
}

void ArkDrmAdapterImpl::StorageClearInfoResult(bool result, int32_t type)
{
    real_->StorageClearInfoResult(result, type);
}

int32_t ArkDrmAdapterImpl::CreateKeySystem(const ArkWebString& name, const ArkWebString& origin, int32_t securityLevel)
{
    return real_->CreateKeySystem(ArkWebStringStructToClass(name), ArkWebStringStructToClass(origin), securityLevel);
}

int32_t ArkDrmAdapterImpl::ReleaseMediaKeySystem()
{
    return real_->ReleaseMediaKeySystem();
}

int32_t ArkDrmAdapterImpl::ReleaseMediaKeySession()
{
    return real_->ReleaseMediaKeySession();
}

int32_t ArkDrmAdapterImpl::SetConfigurationString(const ArkWebString& configName, const ArkWebString& value)
{
    return real_->SetConfigurationString(ArkWebStringStructToClass(configName), ArkWebStringStructToClass(value));
}

int32_t ArkDrmAdapterImpl::GetConfigurationString(const ArkWebString& configName, char* value, int32_t valueLen)
{
    return real_->GetConfigurationString(ArkWebStringStructToClass(configName), value, valueLen);
}

int32_t ArkDrmAdapterImpl::SetConfigurationByteArray(
    const ArkWebString& configName, const uint8_t* value, int32_t valueLen)
{
    return real_->SetConfigurationByteArray(ArkWebStringStructToClass(configName), value, valueLen);
}

int32_t ArkDrmAdapterImpl::GetConfigurationByteArray(const ArkWebString& configName, uint8_t* value, int32_t* valueLen)
{
    return real_->GetConfigurationByteArray(ArkWebStringStructToClass(configName), value, valueLen);
}

int32_t ArkDrmAdapterImpl::GetMaxContentProtectionLevel(int32_t& level)
{
    return real_->GetMaxContentProtectionLevel(level);
}

int32_t ArkDrmAdapterImpl::ProcessKeySystemResponse(const ArkWebString& response, bool isResponseReceived)
{
    return real_->ProcessKeySystemResponse(ArkWebStringStructToClass(response), isResponseReceived);
}

int32_t ArkDrmAdapterImpl::GetCertificateStatus(int32_t& certStatus)
{
    return real_->GetCertificateStatus(certStatus);
}

int32_t ArkDrmAdapterImpl::RegistDrmCallback(ArkWebRefPtr<ArkDrmCallbackAdapter> callbackAdapter)
{
    if (!(CHECK_REF_PTR_IS_NULL(callbackAdapter))) {
        return real_->RegistDrmCallback(std::make_shared<ArkDrmCallbackAdapterWrapper>(callbackAdapter));
    }
    return false;
}

int32_t ArkDrmAdapterImpl::UpdateSession(uint32_t promiseId, const ArkWebString& sessionId, ArkWebUint8Vector response)
{
    return real_->UpdateSession(promiseId, ArkWebStringStructToClass(sessionId),
        ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(response));
}

int32_t ArkDrmAdapterImpl::CloseSession(uint32_t promiseId, const ArkWebString& sessionId)
{
    return real_->CloseSession(promiseId, ArkWebStringStructToClass(sessionId));
}

int32_t ArkDrmAdapterImpl::RemoveSession(uint32_t promiseId, const ArkWebString& sessionId)
{
    return real_->RemoveSession(promiseId, ArkWebStringStructToClass(sessionId));
}

int32_t ArkDrmAdapterImpl::LoadSession(uint32_t promiseId, const ArkWebString& sessionId)
{
    return real_->LoadSession(promiseId, ArkWebStringStructToClass(sessionId));
}

int32_t ArkDrmAdapterImpl::ClearMediaKeys()
{
    return real_->ClearMediaKeys();
}

int32_t ArkDrmAdapterImpl::GetSecurityLevel()
{
    return real_->GetSecurityLevel();
}

int32_t ArkDrmAdapterImpl::RequireSecureDecoderModule(const ArkWebString& mimeType, bool& status)
{
    return real_->RequireSecureDecoderModule(ArkWebStringStructToClass(mimeType), status);
}

int32_t ArkDrmAdapterImpl::GenerateMediaKeyRequest(const ArkWebString& sessionId, int32_t type, int32_t initDataLen,
    const ArkWebUint8Vector& initData, const ArkWebString& mimeType, uint32_t promiseId)
{
    return real_->GenerateMediaKeyRequest(ArkWebStringStructToClass(sessionId), type, initDataLen,
        ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(initData), ArkWebStringStructToClass(mimeType),
        promiseId);
}
} // namespace OHOS::ArkWeb