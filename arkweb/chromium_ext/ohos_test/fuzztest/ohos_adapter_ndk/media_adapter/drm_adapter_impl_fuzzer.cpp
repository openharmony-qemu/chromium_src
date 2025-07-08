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
#define private public
#include "arkweb/ohos_adapter_ndk/media_adapter/drm_adapter_impl.h"
#undef private

#include <cstdlib>
#include <ctime>
#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;

namespace OHOS {
#define MAX_KEY_SET_ID_LEN 64
#define MAX_STR_LEN 32
#define MAX_ARRAY_LEN 64
#define MAX_INT_RANGE_MAX 64
#define MAX_INT_RANGE_MIN 1
#define SECURITY_LEVEL_MIN 0
#define SECURITY_LEVEL_MAX 3
#define CERT_STATUS_MIN 0
#define CERT_STATUS_MAX 4

class DrmCallbackTest : public DrmCallbackAdapter {
public:
    DrmCallbackTest() = default;
    ~DrmCallbackTest() override = default;
    void OnSessionMessage(const std::string& sessionId, int32_t& type, const std::vector<uint8_t>& message) override {}

    void OnProvisionRequest(const std::string& defaultUrl, const std::string& requestData) override {}

    void OnProvisioningComplete(bool success) override {}

    void OnMediaKeySessionReady(void* session) override {}

    void OnPromiseRejected(uint32_t promiseId, const std::string& errorMessage) override {}

    void OnPromiseResolved(uint32_t promiseId) override {}

    void OnPromiseResolvedWithSession(uint32_t promiseId, const std::string& sessionId) override {}

    void OnSessionClosed(const std::string& sessionId) override {}

    void OnSessionKeysChange(const std::string& sessionId, const std::vector<std::string>& keyIdArray,
        const std::vector<uint32_t>& statusArray, bool hasAdditionalUsableKey, bool isKeyRelease) override {}

    void OnSessionExpirationUpdate(const std::string& sessionId, uint64_t expirationTime) override {}

    void OnStorageProvisioned() override {}

    void OnStorageSaveInfo(const std::vector<uint8_t>& ketSetId, const std::string& mimeType,
        const std::string& sessionId, int32_t keyType) override {}

    void OnStorageLoadInfo(const std::string& sessionId) override {}

    void OnStorageClearInfoForKeyRelease(const std::string& sessionId) override {}

    void OnStorageClearInfoForLoadFail(const std::string& sessionId) override {}

    void OnMediaLicenseReady(bool success) override {}
};

void DrmAdapterImpl__CreateSessionId(FuzzedDataProvider* fdp)
{
    auto session1 = SessionId::CreateSessionId("");

    std::string sessionId = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    auto session2 = SessionId::CreateSessionId(sessionId);
}

void DrmAdapterImpl__SessionIdMethods(FuzzedDataProvider* fdp)
{
    std::string emeId = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    size_t random_length = fdp->ConsumeIntegralInRange<size_t>(MAX_INT_RANGE_MIN, MAX_INT_RANGE_MAX);
    std::vector<uint8_t> keySetId(random_length);
    fdp->ConsumeData(keySetId.data(), random_length);
    auto session = NWeb::SessionId::CreateSessionId(emeId);

    session->EmeId();
    session->SetKeySetId(keySetId.data(), random_length);
    session->KeySetId();
    session->KeySetIdLen();
}

void DrmAdapterImpl__SessionInfoMethods(FuzzedDataProvider* fdp)
{
    std::string sessionId = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    std::string mimeType = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    int32_t keyType = fdp->ConsumeIntegralInRange<int32_t>(0, 2);
    auto session = SessionId::CreateSessionId(sessionId);
    auto sessionInfo = SessionInfo(session, mimeType, keyType);
    sessionInfo.MimeType();
    sessionInfo.KeyType();
    sessionInfo.IsRelease();
    sessionInfo.GetSessionId();

    sessionInfo.SetKeyType(keyType);
}

void GetMediaKeySession(MediaKeySession** keySession)
{
    // TODO: realize get media session
}

void DrmCallbackImpl__TestFunctionsOne(FuzzedDataProvider* fdp)
{
    std::shared_ptr<DrmCallbackAdapter>  cbTest = std::make_shared<DrmCallbackTest>();
    DrmCallbackImpl cb(cbTest);

    {
        std::string sessionId = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
        int32_t type = fdp->ConsumeIntegralInRange<int32_t>(0, 2);
        size_t random_length = fdp->ConsumeIntegralInRange<size_t>(MAX_INT_RANGE_MIN, MAX_INT_RANGE_MAX);
        std::vector<uint8_t> message(random_length);
        fdp->ConsumeData(message.data(), random_length);

        cb.OnSessionMessage(sessionId, type, message);
    }

    {
       std::string url = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
       std::string requestData = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
       cb.OnProvisionRequest(url, requestData);
    }

    {
        bool succeed = fdp->ConsumeBool();
        cb.OnProvisioningComplete(succeed);
    }

    {
        MediaKeySession* keySession = nullptr;
        GetMediaKeySession(&keySession);
        if (keySession == nullptr) {
            cb.OnMediaKeySessionReady(keySession);
        } else {
            cb.OnMediaKeySessionReady(reinterpret_cast<OHOSMediaKeySession>(keySession));
        }
    }
}

void DrmCallbackImpl__TestFunctionsTwo(FuzzedDataProvider* fdp)
{
    std::shared_ptr<DrmCallbackAdapter>  cbTest = std::make_shared<DrmCallbackTest>();
    DrmCallbackImpl cb(cbTest);
    std::string sessionId = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    uint32_t promiseId = fdp->ConsumeIntegral<uint32_t>();

    cb.OnStorageProvisioned();
    cb.OnStorageLoadInfo(sessionId);
    cb.OnSessionClosed(sessionId);
    cb.OnPromiseResolved(promiseId);
    cb.OnPromiseResolvedWithSession(promiseId, sessionId);

    {
        uint64_t expirationTime = fdp->ConsumeIntegral<uint64_t>();
        cb.OnSessionExpirationUpdate(sessionId, expirationTime);
    }
}

void DrmCallbackImpl__OnSessionKeysChange(FuzzedDataProvider* fdp)
{
    // TODO:test OnSessionKeysChange
}

void DrmCallbackImpl__OnStorageSaveInfo(FuzzedDataProvider* fdp)
{
    // TODO:test OnStorageSaveInfo
}

void DrmCallbackImpl__TestFunctionsThree(FuzzedDataProvider* fdp)
{
    // TODO:test OnStorageClearInfoForKeyRelease、OnStorageClearInfoForLoadFail、OnMediaLicenseReady
}

void DrmCallbackImpl__UpdateMediaKeySessionInfoMap(FuzzedDataProvider* fdp)
{
    std::shared_ptr<DrmCallbackAdapter> cbTest = std::make_shared<DrmCallbackTest>();
    DrmCallbackImpl cb(cbTest);

    MediaKeySession* keySession = nullptr;
    GetMediaKeySession(&keySession);
    std::string sessionId = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    std::string mimeType = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    int32_t keyType = fdp->ConsumeIntegralInRange<int32_t>(0, 2);
    auto session = SessionId::CreateSessionId(sessionId);

    std::shared_ptr<SessionInfo>  sessionInfo = make_shared<SessionInfo>(session, mimeType, keyType);

    cb.UpdateMediaKeySessionInfoMap(keySession, sessionInfo);
}

void DrmCallbackImpl__TestFunctionsFour(FuzzedDataProvider* fdp)
{
    std::shared_ptr<DrmCallbackAdapter> cbTest = std::make_shared<DrmCallbackTest>();
    DrmCallbackImpl cb(cbTest);

    MediaKeySession* keySession = nullptr;
    GetMediaKeySession(&keySession);

    cb.GetMediaKeySessionInfo(keySession);
    cb.RemoveMediaKeySessionInfo(keySession);
    cb.ClearMediaKeySessionInfo();
}

void GetMediaKeySystem(MediaKeySystem** keySession)
{
    // TODO: realize get media key system
}

void DrmAdapterImpl__SystemCallBackWithObj(FuzzedDataProvider* fdp)
{
    MediaKeySystem* mediaKeySystem = nullptr;
    GetMediaKeySystem(&mediaKeySystem);
    int32_t tmpInt = fdp->ConsumeIntegralInRange<int32_t>(200, 206);
    DRM_EventType eventType = static_cast<DRM_EventType>(tmpInt);

    size_t infoLen = fdp->ConsumeIntegralInRange<size_t>(MAX_INT_RANGE_MIN, MAX_INT_RANGE_MAX);
    std::vector<uint8_t> info(infoLen);
    fdp->ConsumeData(info.data(), infoLen);

    std::string extra = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    DrmAdapterImpl::SystemCallBackWithObj(mediaKeySystem, eventType, info.data(), infoLen, extra.data());
}

void DrmAdapterImpl__SessionEventCallBackWithObj(FuzzedDataProvider* fdp)
{
    MediaKeySession* mediaKeySession = nullptr;
    GetMediaKeySession(&mediaKeySession);

    int32_t tmpInt = fdp->ConsumeIntegralInRange<int32_t>(200, 206);
    DRM_EventType eventType = static_cast<DRM_EventType>(tmpInt);

    size_t infoLen = fdp->ConsumeIntegralInRange<size_t>(MAX_INT_RANGE_MIN, MAX_INT_RANGE_MAX);
    std::vector<uint8_t> info(infoLen);
    fdp->ConsumeData(info.data(), infoLen);

    std::string extra = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    DrmAdapterImpl::SessionEventCallBackWithObj(mediaKeySession, eventType, info.data(), infoLen, extra.data());
}

void DrmAdapterImpl__SessionKeyChangeCallBackWithObj(FuzzedDataProvider* fdp)
{
    MediaKeySession* mediaKeySession = nullptr;
    GetMediaKeySession(&mediaKeySession);

    DRM_KeysInfo keysInfo;
    keysInfo.keysInfoCount = fdp->ConsumeIntegralInRange<uint32_t>(MAX_INT_RANGE_MIN, MAX_INT_RANGE_MAX);

    for (uint32_t i = 0; i < keysInfo.keysInfoCount; ++i) {
        fdp->ConsumeData(keysInfo.keyId[i], MAX_KEY_ID_LEN);
    }

    for (uint32_t i = 0; i < keysInfo.keysInfoCount; ++i) {
        std::string status = fdp->ConsumeRandomLengthString(MAX_STR_LEN - 1);
        strncpy(keysInfo.statusValue[i], status.c_str(), MAX_STR_LEN);
    }

    bool newKeysAvailable = fdp->ConsumeBool();


    DrmAdapterImpl::SessionKeyChangeCallBackWithObj(mediaKeySession, &keysInfo, newKeysAvailable);
}

void DrmCallbackImpl__TestFunctionsFive(FuzzedDataProvider* fdp)
{
    DrmAdapterImpl drmAdapter;
    std::string name = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    std::string mimeType = fdp->ConsumeRandomLengthString(16);
    int32_t level = fdp->ConsumeIntegral<int32_t>();
    drmAdapter.IsSupported(name);
    drmAdapter.IsSupported2(name, mimeType);
    drmAdapter.IsSupported3(name, mimeType, level);
    (void)drmAdapter.GetUUID(name);

    {
        bool result = fdp->ConsumeBool();
        drmAdapter.StorageProvisionedResult(result);
    }

    {
        bool result = fdp->ConsumeBool();
        int32_t type = fdp->ConsumeIntegral<int32_t>();
        drmAdapter.StorageSaveInfoResult(result, type);
    }

    {
        bool result = fdp->ConsumeBool();
        int32_t type = fdp->ConsumeIntegral<int32_t>();
        drmAdapter.StorageClearInfoResult(result, type);
    }

    {
        std::string response = fdp->ConsumeRandomLengthString(32);
        bool isResponseReceived = fdp->ConsumeBool();
        drmAdapter.ProcessKeySystemResponse(response, isResponseReceived);
    }
}

void DrmAdapterImpl__StorageLoadInfoResult(FuzzedDataProvider* fdp)
{
    std::string sessionId = fdp->ConsumeRandomLengthString(16);
    std::vector<uint8_t> keySetId = fdp->ConsumeBytes<uint8_t>(16);
    std::string mimeType = fdp->ConsumeRandomLengthString(16);
    uint32_t keyType = fdp->ConsumeIntegralInRange<uint32_t>(0, 2);

    DrmAdapterImpl drmAdapter;
    drmAdapter.StorageLoadInfoResult(sessionId, keySetId, mimeType, keyType);
}


void DrmAdapterImpl__CreateKeySystem(FuzzedDataProvider* fdp)
{
    std::string name = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    std::string origin = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    int32_t securityLevel = fdp->ConsumeIntegralInRange<int32_t>(SECURITY_LEVEL_MIN, SECURITY_LEVEL_MAX);
    DrmAdapterImpl drmAdapter;
    drmAdapter.CreateKeySystem(name, origin, securityLevel);
}

void DrmAdapterImpl__GenerateMediaKeyRequest(FuzzedDataProvider* fdp)
{
    std::string sessionId = fdp->ConsumeRandomLengthString(16);
    int32_t type = fdp->ConsumeIntegral<int32_t>();
    int32_t initDataLen = fdp->ConsumeIntegralInRange<int32_t>(1, 64);
    std::vector<uint8_t> initData(initDataLen);
    fdp->ConsumeData(initData.data(), initDataLen);

    std::string mimeType = fdp->ConsumeRandomLengthString(16);
    uint32_t promiseId = fdp->ConsumeIntegral<uint32_t>();
    DrmAdapterImpl drmAdapter;
    drmAdapter.GenerateMediaKeyRequest(sessionId, type, initDataLen, initData, mimeType, promiseId);
}

void DrmCallbackImpl__TestFunctionsSix(FuzzedDataProvider* fdp)
{
    uint32_t promiseId = fdp->ConsumeIntegral<uint32_t>();
    std::string sessionId = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    std::string emeId = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    size_t responseLen = fdp->ConsumeIntegralInRange<size_t>(MAX_INT_RANGE_MIN, MAX_INT_RANGE_MAX);
    std::vector<uint8_t> response(responseLen);
    fdp->ConsumeData(response.data(), responseLen);

    {
        DrmAdapterImpl drmAdapter;
        drmAdapter.UpdateSession(promiseId, sessionId, response);
    }

    {
        DrmAdapterImpl drmAdapter;
        drmAdapter.CloseSession(promiseId, emeId);
    }

    {
        DrmAdapterImpl drmAdapter;
        drmAdapter.RemoveSession(promiseId, emeId);
    }

    {
        DrmAdapterImpl drmAdapter;
        drmAdapter.LoadSession(promiseId, sessionId);
    }
}


void DrmCallbackImpl__TestFunctionsSeven(FuzzedDataProvider* fdp)
{
    DrmAdapterImpl drmAdapter;
    (void)drmAdapter.ReleaseMediaKeySystem();
    (void)drmAdapter.ReleaseMediaKeySession();

    {
        std::string configName = fdp->ConsumeRandomLengthString(16);
        std::string value = fdp->ConsumeRandomLengthString(32);
        (void)drmAdapter.SetConfigurationString(configName, value);

        char valueGet[128];
        uint8_t valueGetArray[128];
        int32_t valueLen = 128;
        (void)drmAdapter.GetConfigurationString(configName, valueGet, valueLen);
        (void)drmAdapter.GetConfigurationByteArray(configName, valueGetArray, &valueLen);
    }

    {
        int32_t level = 0;
        int32_t certStatus = 0;
        drmAdapter.GetMaxContentProtectionLevel(level);
        drmAdapter.GetCertificateStatus(certStatus);
    }

    {
        auto callbackAdapter = std::make_shared<DrmCallbackTest>();
        drmAdapter.RegistDrmCallback(callbackAdapter);
    }

    (void)drmAdapter.ClearMediaKeys();
    (void)drmAdapter.GetSecurityLevel();

    {
        std::string mimeType = fdp->ConsumeRandomLengthString(16);
        bool status = fdp->ConsumeBool();
        drmAdapter.RequireSecureDecoderModule(mimeType, status);
    }

    {
        std::string emeId = fdp->ConsumeRandomLengthString(16);
        drmAdapter.CreateMediaKeySession(emeId);
        drmAdapter.GetMediaKeySession(emeId);
    }

    {
        MediaKeySession* drmKeySession = nullptr;
        GetMediaKeySession(&drmKeySession);
        drmAdapter.ReleaseMediaKeySession(drmKeySession);
    }
}

void DrmAdapterImpl__PutSessionInfo(FuzzedDataProvider* fdp)
{
    // TODO: test PutSessionInfo
}

void DrmAdapterImpl__GetSessionInfo(FuzzedDataProvider* fdp)
{
    // TODO: test GetSessionInfo
}

void DrmAdapterImpl__RemoveSessionInfo(FuzzedDataProvider* fdp)
{
    auto id = fdp->ConsumeRandomLengthString(32);
    std::shared_ptr<SessionId> sessionId = SessionId::CreateSessionId(id);
    DrmAdapterImpl drmAdapter;
    drmAdapter.RemoveSessionInfo(sessionId);
}

void DrmAdapterImpl__LoadSessionInfo(FuzzedDataProvider* fdp)
{
    std::string emeId = fdp->ConsumeRandomLengthString(16);
    DrmAdapterImpl drmAdapter;
    drmAdapter.LoadSessionInfo(emeId);
}

void DrmAdapterImpl__LoadSessionWithLoadedStorage(FuzzedDataProvider* fdp)
{
    auto id = fdp->ConsumeRandomLengthString(32);
    std::shared_ptr<SessionId> sessionId = SessionId::CreateSessionId(id);
    uint32_t promiseId = fdp->ConsumeIntegral<uint32_t>();
    DrmAdapterImpl drmAdapter;
    drmAdapter.LoadSessionWithLoadedStorage(sessionId, promiseId);
}

void DrmAdapterImpl__UpdateSessionResult(FuzzedDataProvider* fdp)
{
    bool isKeyRelease = fdp->ConsumeBool();
    auto id = fdp->ConsumeRandomLengthString(32);
    std::shared_ptr<SessionId> sessionId = SessionId::CreateSessionId(id);
    std::vector<uint8_t> mediaKeyId = fdp->ConsumeBytes<uint8_t>(16);
    int32_t mediaKeyIdLen = mediaKeyId.size();
    DrmAdapterImpl drmAdapter;
    drmAdapter.UpdateSessionResult(isKeyRelease, sessionId, mediaKeyId.data(), mediaKeyIdLen);
}

void DrmAdapterImpl__SetKeyType(FuzzedDataProvider* fdp)
{
    auto id = fdp->ConsumeRandomLengthString(32);
    std::shared_ptr<SessionId> sessionId = SessionId::CreateSessionId(id);
    int32_t keyType = fdp->ConsumeIntegralInRange<int32_t>(0, 2);
    DrmAdapterImpl drmAdapter;
    drmAdapter.SetKeyType(sessionId, keyType);
}

void DrmAdapterImpl__SetKeySetId(FuzzedDataProvider* fdp)
{
    auto id = fdp->ConsumeRandomLengthString(32);
    std::shared_ptr<SessionId> sessionId = SessionId::CreateSessionId(id);
    std::vector<uint8_t> mediaKeyId = fdp->ConsumeBytes<uint8_t>(16);
    int32_t mediaKeyIdLen = mediaKeyId.size();
    DrmAdapterImpl drmAdapter;
    drmAdapter.SetKeySetId(sessionId, mediaKeyId.data(), mediaKeyIdLen);
}

void DrmAdapterImpl__GetSessionIdByEmeId(FuzzedDataProvider* fdp)
{
    std::string emeId = fdp->ConsumeRandomLengthString(16);
    DrmAdapterImpl drmAdapter;
    drmAdapter.GetSessionIdByEmeId(emeId);
}

void DrmAdapterImpl__ClearPersistentSessionInfoFroKeyRelease(FuzzedDataProvider* fdp)
{
    auto id = fdp->ConsumeRandomLengthString(32);
    std::shared_ptr<SessionId> sessionId = SessionId::CreateSessionId(id);
    DrmAdapterImpl drmAdapter;
    drmAdapter.ClearPersistentSessionInfoFroKeyRelease(sessionId);
}

void DrmAdapterImpl__ClearPersistentSessionInfoForLoadFail(FuzzedDataProvider* fdp)
{
    auto id = fdp->ConsumeRandomLengthString(32);
    std::shared_ptr<SessionId> sessionId = SessionId::CreateSessionId(id);
    DrmAdapterImpl drmAdapter;
    drmAdapter.ClearPersistentSessionInfoForLoadFail(sessionId);
}

void DrmAdapterImpl__HandleKeyUpdatedCallback(FuzzedDataProvider* fdp)
{
    uint32_t promiseId = fdp->ConsumeIntegral<uint32_t>();
    bool result = fdp->ConsumeBool();
    DrmAdapterImpl drmAdapter;
    drmAdapter.HandleKeyUpdatedCallback(promiseId, result);
}

void DrmAdapterImpl__OnSessionExpirationUpdate(FuzzedDataProvider* fdp)
{
    MediaKeySession* drmKeySession = nullptr;
    std::vector<uint8_t> info = fdp->ConsumeBytes<uint8_t>(32);
    int32_t infoLen = info.size();
    DrmAdapterImpl::OnSessionExpirationUpdate(drmKeySession, info.data(), infoLen);
}

void DrmAdapterImpl__GetKeyRequest(FuzzedDataProvider* fdp)
{
    MediaKeySession* drmKeySession = nullptr;
    std::vector<uint8_t> info = fdp->ConsumeBytes<uint8_t>(32);
    int32_t infoLen = info.size();
    DrmAdapterImpl::GetKeyRequest(drmKeySession, info.data(), infoLen);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }

    FuzzedDataProvider fdp(data, size);
    OHOS::DrmAdapterImpl__CreateSessionId(&fdp);
    OHOS::DrmAdapterImpl__SessionIdMethods(&fdp);
    OHOS::DrmAdapterImpl__SessionInfoMethods(&fdp);
    OHOS::DrmCallbackImpl__TestFunctionsOne(&fdp);
    OHOS::DrmCallbackImpl__TestFunctionsTwo(&fdp);
    OHOS::DrmCallbackImpl__OnSessionKeysChange(&fdp);
    OHOS::DrmCallbackImpl__OnStorageSaveInfo(&fdp);
    OHOS::DrmCallbackImpl__TestFunctionsThree(&fdp);
    OHOS::DrmCallbackImpl__UpdateMediaKeySessionInfoMap(&fdp);
    OHOS::DrmCallbackImpl__TestFunctionsFour(&fdp);
    OHOS::DrmAdapterImpl__SystemCallBackWithObj(&fdp);
    OHOS::DrmAdapterImpl__SessionEventCallBackWithObj(&fdp);
    OHOS::DrmAdapterImpl__SessionKeyChangeCallBackWithObj(&fdp);
    OHOS::DrmCallbackImpl__TestFunctionsFive(&fdp);
    OHOS::DrmAdapterImpl__StorageLoadInfoResult(&fdp);
    OHOS::DrmAdapterImpl__CreateKeySystem(&fdp);
    OHOS::DrmAdapterImpl__GenerateMediaKeyRequest(&fdp);
    OHOS::DrmCallbackImpl__TestFunctionsSix(&fdp);
    OHOS::DrmCallbackImpl__TestFunctionsSeven(&fdp);
    OHOS::DrmAdapterImpl__PutSessionInfo(&fdp);
    OHOS::DrmAdapterImpl__GetSessionInfo(&fdp);
    OHOS::DrmAdapterImpl__RemoveSessionInfo(&fdp);
    OHOS::DrmAdapterImpl__LoadSessionInfo(&fdp);
    OHOS::DrmAdapterImpl__LoadSessionWithLoadedStorage(&fdp);
    OHOS::DrmAdapterImpl__UpdateSessionResult(&fdp);
    OHOS::DrmAdapterImpl__SetKeyType(&fdp);
    OHOS::DrmAdapterImpl__SetKeySetId(&fdp);
    OHOS::DrmAdapterImpl__GetSessionIdByEmeId(&fdp);
    OHOS::DrmAdapterImpl__ClearPersistentSessionInfoFroKeyRelease(&fdp);
    OHOS::DrmAdapterImpl__ClearPersistentSessionInfoForLoadFail(&fdp);
    OHOS::DrmAdapterImpl__HandleKeyUpdatedCallback(&fdp);
    OHOS::DrmAdapterImpl__OnSessionExpirationUpdate(&fdp);
    OHOS::DrmAdapterImpl__GetKeyRequest(&fdp);

    return 0;
}
