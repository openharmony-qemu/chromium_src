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

#ifndef ARK_DRM_ADAPTER_WRAPPER_H
#define ARK_DRM_ADAPTER_WRAPPER_H
#pragma once

#include "drm_adapter.h"
#include "ohos_adapter/include/ark_drm_adapter.h"

namespace OHOS::ArkWeb {

class ArkDrmAdapterWrapper : public OHOS::NWeb::DrmAdapter {
public:
    explicit ArkDrmAdapterWrapper(ArkWebRefPtr<ArkDrmAdapter>);

    bool IsSupported(const std::string& name) override;

    bool IsSupported2(const std::string& name, const std::string& mimeType) override;

    bool IsSupported3(const std::string& name, const std::string& mimeType, int32_t level) override;

    std::vector<uint8_t> GetUUID(const std::string& name) override;

    void StorageProvisionedResult(bool result) override;

    void StorageSaveInfoResult(bool result, int32_t type) override;

    void StorageLoadInfoResult(const std::string& sessionId, const std::vector<uint8_t>& keySetId,
        const std::string& mimeType, uint32_t keyType) override;

    void StorageClearInfoResult(bool result, int32_t type) override;

    int32_t CreateKeySystem(const std::string& name, const std::string& origin, int32_t securityLevel) override;

    int32_t ReleaseMediaKeySystem() override;

    int32_t ReleaseMediaKeySession() override;

    int32_t SetConfigurationString(const std::string& configName, const std::string& value) override;

    int32_t GetConfigurationString(const std::string& configName, char* value, int32_t valueLen) override;

    int32_t SetConfigurationByteArray(const std::string& configName, const uint8_t* value, int32_t valueLen) override;

    int32_t GetConfigurationByteArray(const std::string& configName, uint8_t* value, int32_t* valueLen) override;

    int32_t GetMaxContentProtectionLevel(int32_t& level) override;

    int32_t ProcessKeySystemResponse(const std::string& response, bool isResponseReceived) override;

    int32_t GetCertificateStatus(int32_t& certStatus) override;

    int32_t RegistDrmCallback(std::shared_ptr<NWeb::DrmCallbackAdapter> callbackAdapter) override;

    int32_t UpdateSession(uint32_t promiseId, const std::string& sessionId, std::vector<uint8_t> response) override;

    int32_t CloseSession(uint32_t promiseId, const std::string& sessionId) override;

    int32_t RemoveSession(uint32_t promiseId, const std::string& sessionId) override;

    int32_t LoadSession(uint32_t promiseId, const std::string& sessionId) override;

    int32_t ClearMediaKeys() override;

    int32_t GetSecurityLevel() override;

    int32_t RequireSecureDecoderModule(const std::string& mimeType, bool& status) override;

    int32_t GenerateMediaKeyRequest(const std::string& sessionId, int32_t type, int32_t initDataLen,
        const std::vector<uint8_t>& initData, const std::string& mimeType, uint32_t promiseId) override;

private:
    ArkWebRefPtr<ArkDrmAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_DRM_ADAPTER_WRAPPER_H
