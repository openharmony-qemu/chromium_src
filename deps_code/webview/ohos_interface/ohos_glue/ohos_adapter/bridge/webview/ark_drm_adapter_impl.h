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

#ifndef ARK_DRM_ADAPTER_IMPL_H
#define ARK_DRM_ADAPTER_IMPL_H
#pragma once
#include "drm_adapter.h"
#include "ohos_adapter/include/ark_drm_adapter.h"

namespace OHOS::ArkWeb {

class ArkDrmAdapterImpl : public ArkDrmAdapter {
public:
    explicit ArkDrmAdapterImpl(std::shared_ptr<OHOS::NWeb::DrmAdapter>);

    bool IsSupported(const ArkWebString& name) override;

    bool IsSupported2(const ArkWebString& name, const ArkWebString& mimeType) override;

    bool IsSupported3(const ArkWebString& name, const ArkWebString& mimeType, int32_t level) override;

    ArkWebUint8Vector GetUUID(const ArkWebString& name) override;

    void StorageProvisionedResult(bool result) override;

    void StorageSaveInfoResult(bool result, int32_t type) override;

    void StorageLoadInfoResult(const ArkWebString& sessionId, const ArkWebUint8Vector& keySetId,
        const ArkWebString& mimeType, uint32_t keyType) override;

    void StorageClearInfoResult(bool result, int32_t type) override;

    int32_t CreateKeySystem(const ArkWebString& name, const ArkWebString& origin, int32_t securityLevel) override;

    int32_t ReleaseMediaKeySystem() override;

    int32_t ReleaseMediaKeySession() override;

    int32_t SetConfigurationString(const ArkWebString& configName, const ArkWebString& value) override;

    int32_t GetConfigurationString(const ArkWebString& configName, char* value, int32_t valueLen) override;

    int32_t SetConfigurationByteArray(const ArkWebString& configName, const uint8_t* value, int32_t valueLen) override;

    int32_t GetConfigurationByteArray(const ArkWebString& configName, uint8_t* value, int32_t* valueLen) override;

    int32_t GetMaxContentProtectionLevel(int32_t& level) override;

    int32_t ProcessKeySystemResponse(const ArkWebString& response, bool isResponseReceived) override;

    int32_t GetCertificateStatus(int32_t& certStatus) override;

    int32_t RegistDrmCallback(ArkWebRefPtr<ArkDrmCallbackAdapter> callbackAdapter) override;

    int32_t UpdateSession(uint32_t promiseId, const ArkWebString& sessionId, ArkWebUint8Vector response) override;

    int32_t CloseSession(uint32_t promiseId, const ArkWebString& sessionId) override;

    int32_t RemoveSession(uint32_t promiseId, const ArkWebString& sessionId) override;

    int32_t LoadSession(uint32_t promiseId, const ArkWebString& sessionId) override;

    int32_t ClearMediaKeys() override;

    int32_t GetSecurityLevel() override;

    int32_t RequireSecureDecoderModule(const ArkWebString& mimeType, bool& status) override;

    int32_t GenerateMediaKeyRequest(const ArkWebString& sessionId, int32_t type, int32_t initDataLen,
        const ArkWebUint8Vector& initData, const ArkWebString& mimeType, uint32_t promiseId) override;

private:
    std::shared_ptr<OHOS::NWeb::DrmAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkDrmAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_DRM_ADAPTER_IMPL_H
