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

#include "base/logging.h"

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

class DrmCallbackAdapterMock : public DrmCallbackAdapter {
public:
    DrmCallbackAdapterMock() = default;
    ~DrmCallbackAdapterMock() override = default;
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

bool g_isSupportDrm = true;
auto mockCallback_ = std::make_shared<DrmCallbackAdapterMock>();

static const std::string GetKeySystemName()
{
    // TODO: realize get media session
    g_isSupportDrm = true;
    return "com.wiseplay.drm";
}

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
    std::shared_ptr<DrmCallbackAdapterMock>  cbTest = std::make_shared<DrmCallbackAdapterMock>();
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

    {
        uint32_t promiseId = fdp->ConsumeIntegral<uint32_t>();
        std::string errorMessage = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
        cb.OnPromiseRejected(promiseId, errorMessage);
    }
}

void DrmCallbackImpl__TestFunctionsTwo(FuzzedDataProvider* fdp)
{
    std::shared_ptr<DrmCallbackAdapter>  cbTest = std::make_shared<DrmCallbackAdapterMock>();
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
    std::shared_ptr<DrmCallbackAdapter> cbTest = std::make_shared<DrmCallbackAdapterMock>();
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
    std::shared_ptr<DrmCallbackAdapter> cbTest = std::make_shared<DrmCallbackAdapterMock>();
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

    bool newKeysAvailable = fdp->ConsumeBool();


    DrmAdapterImpl::SessionKeyChangeCallBackWithObj(mediaKeySession, &keysInfo, newKeysAvailable);
}

void DrmAdapterImpl__IsSupported(FuzzedDataProvider* fdp)
{
    DrmAdapterImpl drmAdapter;
    std::string name = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    std::string mimeType = fdp->ConsumeRandomLengthString(16);
    int32_t level = fdp->ConsumeIntegral<int32_t>();
    drmAdapter.IsSupported(name);
    drmAdapter.IsSupported2(name, mimeType);
    drmAdapter.IsSupported3(name, mimeType, level);
    drmAdapter.IsSupported("name");
    drmAdapter.IsSupported2("name", "mimeType");
    drmAdapter.IsSupported3("name", "mimeType", level);
    drmAdapter.GetUUID("name");
}

void DrmAdapterImpl__StorageProvisionedResult(FuzzedDataProvider* fdp)
{
    DrmAdapterImpl drmAdapter;
    bool result = fdp->ConsumeBool();
    drmAdapter.StorageProvisionedResult(result);
    drmAdapter.StorageProvisionedResult(false);
    if (g_isSupportDrm) {
        drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
        drmAdapter.RegistDrmCallback(mockCallback_);
        drmAdapter.contentProtectionLevel_ = CONTENT_PROTECTION_LEVEL_SW_CRYPTO;
        int32_t certStatus = -1;
        drmAdapter.GetCertificateStatus(certStatus);
        if (certStatus == 0) {
            drmAdapter.StorageProvisionedResult(true);
            drmAdapter.ReleaseMediaKeySession();
            drmAdapter.StorageProvisionedResult(true);
        }
        drmAdapter.ReleaseMediaKeySession();
        drmAdapter.ReleaseMediaKeySystem();
    }
}

void DrmAdapterImpl__StorageSaveInfoResult(FuzzedDataProvider* fdp)
{
    DrmAdapterImpl drmAdapter;
    bool result = fdp->ConsumeBool();
    int32_t type = fdp->ConsumeIntegral<int32_t>();
    drmAdapter.StorageSaveInfoResult(result, type);
    drmAdapter.StorageSaveInfoResult(true, 1);
    drmAdapter.RegistDrmCallback(mockCallback_);
    drmAdapter.StorageSaveInfoResult(true, 1);
}

void DrmAdapterImpl__StorageClearInfoResult(FuzzedDataProvider* fdp)
{
    DrmAdapterImpl drmAdapter;
    bool result = fdp->ConsumeBool();
    int32_t type = fdp->ConsumeIntegral<int32_t>();
    drmAdapter.StorageClearInfoResult(result, type);
    drmAdapter.RegistDrmCallback(mockCallback_);
    drmAdapter.StorageClearInfoResult(true, static_cast<int32_t>(ClearInfoType::KEY_RELEASE));
    drmAdapter.StorageClearInfoResult(false, static_cast<int32_t>(ClearInfoType::KEY_RELEASE));
    drmAdapter.StorageClearInfoResult(false, static_cast<int32_t>(ClearInfoType::LOAD_FAIL));
    drmAdapter.StorageClearInfoResult(true, static_cast<int32_t>(ClearInfoType::LOAD_FAIL));
    drmAdapter.callback_ = nullptr;
    drmAdapter.StorageClearInfoResult(true, static_cast<int32_t>(ClearInfoType::LOAD_FAIL));
}

void DrmAdapterImpl__ProcessKeySystemResponse(FuzzedDataProvider* fdp)
{
    DrmAdapterImpl drmAdapter;
    std::string response = fdp->ConsumeRandomLengthString(32);
    bool isResponseReceived = fdp->ConsumeBool();
    drmAdapter.ProcessKeySystemResponse(response, isResponseReceived);

    std::string KeySystemResponse = "response";
    drmAdapter.ProcessKeySystemResponse("", false);
    drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    drmAdapter.ProcessKeySystemResponse(KeySystemResponse, false);
    drmAdapter.RegistDrmCallback(mockCallback_);
    drmAdapter.ProcessKeySystemResponse(KeySystemResponse, false);
    drmAdapter.ReleaseMediaKeySystem();
}

void DrmAdapterImpl__StorageLoadInfoResult(FuzzedDataProvider* fdp)
{
    DrmAdapterImpl drmAdapter;
    std::string sessionId = fdp->ConsumeRandomLengthString(16);
    std::vector<uint8_t> keySetId = fdp->ConsumeBytes<uint8_t>(16);
    std::string mimeType = fdp->ConsumeRandomLengthString(16);
    uint32_t keyType = fdp->ConsumeIntegralInRange<uint32_t>(0, 2);
    drmAdapter.StorageLoadInfoResult(sessionId, keySetId, mimeType, keyType);
    keySetId.clear();
    drmAdapter.callback_ = nullptr;
    drmAdapter.StorageLoadInfoResult("", keySetId, "", 0);
    drmAdapter.RegistDrmCallback(mockCallback_);
    drmAdapter.StorageLoadInfoResult("", keySetId, "", 0);
    drmAdapter.callback_ = nullptr;
    std::vector<uint8_t> keySetId2 = { 0x01, 0x02 };
    drmAdapter.StorageLoadInfoResult("testEmeId063", keySetId2, "", 0);
    drmAdapter.StorageLoadInfoResult("testEmeId063", keySetId2, "", 0);
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
    DrmAdapterImpl drmAdapter;
    auto mockCallback = std::make_shared<DrmCallbackAdapterMock>();
    std::string sessionId = fdp->ConsumeRandomLengthString(16);
    int32_t type = fdp->ConsumeIntegral<int32_t>();
    int32_t initDataLen = fdp->ConsumeIntegralInRange<int32_t>(1, 64);
    std::vector<uint8_t> initData(initDataLen);
    fdp->ConsumeData(initData.data(), initDataLen);
    std::string mimeType = fdp->ConsumeRandomLengthString(16);
    uint32_t promiseId = fdp->ConsumeIntegral<uint32_t>();
    drmAdapter.GenerateMediaKeyRequest(sessionId, type, initDataLen, initData, mimeType, promiseId);
    std::string emeId = "invalidEmeId";
    type = 1;
    initDataLen = 128;
    initData.resize(128);
    uint32_t optionsCount = 1;
    drmAdapter.RegistDrmCallback(mockCallback);
    drmAdapter.GenerateMediaKeyRequest(emeId, type, initDataLen, initData, "video/avc", optionsCount);
    drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    drmAdapter.GenerateMediaKeyRequest(emeId, type, initDataLen, initData,
                                       "test_mime_type_long_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                                       optionsCount);
    drmAdapter.GenerateMediaKeyRequest(emeId, type, initDataLen, initData,
                                       "test_mime_type_long_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                                       optionsCount);
    drmAdapter.ReleaseMediaKeySystem();
}

void DrmAdapterImpl__UpdateSession(FuzzedDataProvider* fdp)
{
    std::string sessionId = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    int32_t len = fdp->ConsumeIntegralInRange<int32_t>(1, 64);
    std::vector<uint8_t> response(len);
    fdp->ConsumeData(response.data(), len);
    uint32_t promiseId = fdp->ConsumeIntegral<uint32_t>();
    DrmAdapterImpl drmAdapter;
    drmAdapter.UpdateSession(promiseId, sessionId, response);
    promiseId = 1;
    std::string emeId = "test_eme_id";
    response = { 0x01, 0x02, 0x03 };                         // Example response data
    auto validSessionId = std::make_shared<SessionId>("test_eme_id", nullptr, 0); // Assume this constructor exists
    std::string mimeType = "video/mp4";
    drmAdapter.UpdateSession(promiseId, emeId, response);
    drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    drmAdapter.UpdateSession(promiseId, emeId, response);
    drmAdapter.RegistDrmCallback(mockCallback_);
    drmAdapter.PutSessionInfo(validSessionId, mimeType, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_ONLINE));
    drmAdapter.UpdateSession(promiseId, emeId, response);
    drmAdapter.PutSessionInfo(validSessionId, mimeType, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_RELEASE));
    drmAdapter.UpdateSession(promiseId, emeId, response);
    drmAdapter.UpdateSession(promiseId, "invalid_eme_id", response);
    drmAdapter.RemoveSessionInfo(validSessionId);
    drmAdapter.ReleaseMediaKeySession();
    drmAdapter.ReleaseMediaKeySystem();
}

void DrmAdapterImpl__CloseSession(FuzzedDataProvider* fdp)
{
    std::string emeId = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    uint32_t promiseId = fdp->ConsumeIntegral<uint32_t>();
    DrmAdapterImpl drmAdapter;
    drmAdapter.CloseSession(promiseId, emeId);
    auto validSessionId = std::make_shared<SessionId>("test_eme_id", nullptr, 0);
    promiseId = 1;
    emeId = "test_eme_id";
    drmAdapter.CloseSession(promiseId, emeId);
    if (g_isSupportDrm) {
        drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
        drmAdapter.CloseSession(promiseId, emeId);
        std::string mimeType = "video/mp4";
        int32_t sessionType = 1;
        drmAdapter.PutSessionInfo(validSessionId, mimeType, sessionType);
        drmAdapter.CloseSession(promiseId, emeId);
        drmAdapter.RemoveSessionInfo(validSessionId);
        drmAdapter.PutSessionInfo(validSessionId, mimeType, sessionType);
        drmAdapter.keySystemType_ = KeySystemType::WIDEVINE;
        drmAdapter.CloseSession(promiseId, emeId);
        drmAdapter.ReleaseMediaKeySession();
        drmAdapter.ReleaseMediaKeySystem();
    }
}

void DrmAdapterImpl__RemoveSession(FuzzedDataProvider* fdp)
{
    std::string emeId = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    uint32_t promiseId = fdp->ConsumeIntegral<uint32_t>();
    DrmAdapterImpl drmAdapter;
    drmAdapter.CloseSession(promiseId, emeId);
    auto validSessionId = std::make_shared<SessionId>("test_eme_id", nullptr, 0);
    promiseId = 1;
    emeId = "test_eme_id";
    drmAdapter.CloseSession(promiseId, emeId);
    if (g_isSupportDrm) {
        drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
        drmAdapter.CloseSession(promiseId, emeId);
        std::string mimeType = "video/mp4";
        int32_t sessionType = 1;
        drmAdapter.PutSessionInfo(validSessionId, mimeType, sessionType);
        drmAdapter.CloseSession(promiseId, emeId);
        drmAdapter.RemoveSessionInfo(validSessionId);
        drmAdapter.PutSessionInfo(validSessionId, mimeType, sessionType);
        drmAdapter.keySystemType_ = KeySystemType::WIDEVINE;
        drmAdapter.CloseSession(promiseId, emeId);
        drmAdapter.ReleaseMediaKeySession();
        drmAdapter.ReleaseMediaKeySystem();
    }
}

void DrmAdapterImpl__LoadSession(FuzzedDataProvider* fdp)
{
    std::string sessionId = fdp->ConsumeRandomLengthString(MAX_STR_LEN);
    uint32_t promiseId = fdp->ConsumeIntegral<uint32_t>();
    DrmAdapterImpl drmAdapter;
    drmAdapter.LoadSession(promiseId, sessionId);
    promiseId = 1;
    sessionId = "sessionId";
    drmAdapter.LoadSession(promiseId, sessionId);
    drmAdapter.RegistDrmCallback(mockCallback_);
    drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    drmAdapter.LoadSession(promiseId, sessionId);
    drmAdapter.ReleaseMediaKeySession();
    drmAdapter.ReleaseMediaKeySystem();
}

void DrmAdapterImpl__SetConfigurationString(FuzzedDataProvider* fdp)
{
    DrmAdapterImpl drmAdapter;
    std::string configName = fdp->ConsumeRandomLengthString(16);
    std::string value = fdp->ConsumeRandomLengthString(32);
    drmAdapter.SetConfigurationString(configName, value);
    drmAdapter.SetConfigurationString("", "");
    drmAdapter.SetConfigurationString("version", "");
    drmAdapter.SetConfigurationString("version", "2.0");
    uint8_t description[4] = { 0x00, 0x00, 0x00, 0x00 };
    int32_t valueLen = sizeof(description);
    drmAdapter.SetConfigurationByteArray("", description, valueLen);
    drmAdapter.SetConfigurationByteArray("description", nullptr, valueLen);
    drmAdapter.SetConfigurationByteArray("description", description, valueLen);
}

void DrmAdapterImpl__GetConfigurationByteArray(FuzzedDataProvider* fdp)
{
    DrmAdapterImpl drmAdapter;
    char value[256];
    int32_t valueLen = sizeof(value);
    uint8_t valueGetArray[128];
    drmAdapter.GetConfigurationByteArray(value, valueGetArray, &valueLen);
    drmAdapter.GetConfigurationString("", value, valueLen);
    drmAdapter.GetConfigurationString("version", nullptr, valueLen);
    drmAdapter.GetConfigurationString("version", value, valueLen);
    if (g_isSupportDrm) {
        drmAdapter.RegistDrmCallback(mockCallback_);
        drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
        drmAdapter.SetConfigurationString("version", "2.0");
        drmAdapter.GetConfigurationString("testVersion", value, valueLen);
        drmAdapter.GetConfigurationString("version", value, valueLen);
        drmAdapter.ReleaseMediaKeySession();
        drmAdapter.ReleaseMediaKeySystem();
    }
}

void DrmAdapterImpl__RequireSecureDecoderModule(FuzzedDataProvider* fdp)
{
    DrmAdapterImpl drmAdapter;
    std::string mimeType = fdp->ConsumeRandomLengthString(16);
    bool status = fdp->ConsumeBool();
    drmAdapter.RequireSecureDecoderModule(mimeType, status);
    mimeType = "video/avc";
    status = false;
    drmAdapter.RequireSecureDecoderModule(mimeType, status);
    drmAdapter.RequireSecureDecoderModule("", status);
    if (g_isSupportDrm) {
        drmAdapter.RegistDrmCallback(mockCallback_);
        drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
        drmAdapter.RequireSecureDecoderModule(mimeType, status);
        int32_t certStatus = -1;
        drmAdapter.GetCertificateStatus(certStatus);
        drmAdapter.ReleaseMediaKeySession();
        drmAdapter.ReleaseMediaKeySystem();
    }
}

void DrmAdapterImpl__TestFunctionsSeven(FuzzedDataProvider* fdp)
{
    DrmAdapterImpl drmAdapter;
    {
        int32_t level = 0;
        int32_t certStatus = 0;
        drmAdapter.GetMaxContentProtectionLevel(level);
        drmAdapter.GetCertificateStatus(certStatus);
        if (g_isSupportDrm) {
            drmAdapter.RegistDrmCallback(mockCallback_);
            drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
            drmAdapter.GetMaxContentProtectionLevel(level);
            drmAdapter.ReleaseMediaKeySession();
            drmAdapter.ReleaseMediaKeySystem();
        }
    }

    {
        auto callbackAdapter = std::make_shared<DrmCallbackAdapterMock>();
        drmAdapter.RegistDrmCallback(callbackAdapter);
    }

    {
        drmAdapter.ClearMediaKeys();
        drmAdapter.GetSecurityLevel();
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
    auto validSessionId = std::make_shared<SessionId>("test_eme_id", nullptr, 0);
    std::string mimeType = "video/mp4";
    int32_t sessionType = 1;
    drmAdapter.PutSessionInfo(nullptr, mimeType, sessionType);
    drmAdapter.PutSessionInfo(validSessionId, mimeType, sessionType);
    drmAdapter.GetSessionInfo(validSessionId);
    drmAdapter.RemoveSessionInfo(nullptr);
    drmAdapter.RemoveSessionInfo(validSessionId);
    drmAdapter.GetSessionInfo(validSessionId);
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
    drmAdapter.LoadSessionWithLoadedStorage(nullptr, 1);
    sessionId = std::make_shared<SessionId>("sessionId_028", nullptr, 0);
    drmAdapter.LoadSessionWithLoadedStorage(sessionId, 1);
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
    drmAdapter.UpdateSessionResult(false, nullptr, nullptr, 0);
    sessionId = SessionId::CreateSessionId("tempSessionId068");
    drmAdapter.UpdateSessionResult(false, sessionId, nullptr, 0);

    std::string mimeType = "video/mp4";
    int32_t sessionType = 1;
    drmAdapter.callback_ = nullptr;
    drmAdapter.PutSessionInfo(sessionId, mimeType, sessionType);
    drmAdapter.UpdateSessionResult(false, sessionId, nullptr, 0);
    drmAdapter.UpdateSessionResult(true, sessionId, nullptr, 0);
    drmAdapter.RegistDrmCallback(mockCallback_);
    drmAdapter.UpdateSessionResult(false, sessionId, nullptr, 0);
}

void DrmAdapterImpl__SetKeyType(FuzzedDataProvider* fdp)
{
    auto id = fdp->ConsumeRandomLengthString(32);
    std::shared_ptr<SessionId> sessionId = SessionId::CreateSessionId(id);
    int32_t keyType = fdp->ConsumeIntegralInRange<int32_t>(0, 2);
    DrmAdapterImpl drmAdapter;
    drmAdapter.SetKeyType(sessionId, keyType);

    std::vector<uint8_t> keySetId = { 0x01, 0x02, 0x03 };
    sessionId = std::make_shared<SessionId>("sessionId", keySetId.data(), keySetId.size());
    auto sessionInfo = std::make_shared<SessionInfo>(
        sessionId, "mimeType", static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE));
    drmAdapter.SetKeyType(nullptr, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE));
    drmAdapter.RegistDrmCallback(mockCallback_);
    drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    drmAdapter.RegistDrmCallback(mockCallback_);
    drmAdapter.PutSessionInfo(sessionId, "mimeType", static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE));
    drmAdapter.SetKeyType(sessionId, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE));
    drmAdapter.RemoveSessionInfo(sessionId);
    drmAdapter.ReleaseMediaKeySession();
    drmAdapter.ReleaseMediaKeySystem();
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
    auto validSessionId = std::make_shared<SessionId>("test_eme_id", nullptr, 0);
    std::string mimeType = "video/mp4";
    int32_t sessionType = 1;
    drmAdapter.PutSessionInfo(validSessionId, mimeType, sessionType);
    drmAdapter.GetSessionIdByEmeId(validSessionId->EmeId());
    drmAdapter.GetSessionIdByEmeId("invalid_eme_id");
    drmAdapter.emeSessionInfoMap_["nullptr_eme_id"] = nullptr;
    drmAdapter.GetSessionIdByEmeId("nullptr_eme_id");

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

void DrmAdapterImpl__TestFunctionsEight(FuzzedDataProvider* fdp)
{
    DrmAdapterImpl drmAdapter;
    auto g_callback = std::make_shared<DrmCallbackImpl>(mockCallback_);
    drmAdapter.RegistDrmCallback(mockCallback_);
    drmAdapter.CreateKeySystem("", "", SECURITY_LEVEL_3);
    drmAdapter.CreateKeySystem("com.test.drm", "", SECURITY_LEVEL_3);
    drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    if (g_isSupportDrm) {
        int32_t certStatus = -1;
        drmAdapter.GetCertificateStatus(certStatus);
        if (certStatus == 0) {
            drmAdapter.ReleaseMediaKeySession();
            drmAdapter.ReleaseMediaKeySystem();

            drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_1);
            drmAdapter.ReleaseMediaKeySession();
            drmAdapter.ReleaseMediaKeySystem();
        }

        drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_UNKNOWN);
        drmAdapter.ReleaseMediaKeySession();
        drmAdapter.ReleaseMediaKeySystem();

        drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", -1);
        drmAdapter.ReleaseMediaKeySession();
        drmAdapter.ReleaseMediaKeySystem();
    }
}

void DrmAdapterImpl__CreateMediaKeySession(FuzzedDataProvider* fdp)
{
    DrmAdapterImpl drmAdapter;
    auto g_callback = std::make_shared<DrmCallbackImpl>(mockCallback_);
    drmAdapter.RegistDrmCallback(mockCallback_);
    drmAdapter.CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    if (g_isSupportDrm) {
        int32_t certStatus = -1;
        drmAdapter.GetCertificateStatus(certStatus);
        if (certStatus == 0) {
            drmAdapter.CreateMediaKeySession();
            drmAdapter.ReleaseMediaKeySession();

            drmAdapter.CreateMediaKeySession("emeId_006");
            drmAdapter.ReleaseMediaKeySession(drmAdapter.GetMediaKeySession("emeId_006"));

            drmAdapter.keySystemType_ = KeySystemType::WIDEVINE;
            drmAdapter.GetMediaKeySession("");

            drmAdapter.CreateMediaKeySession("emeId_006");
            drmAdapter.ReleaseMediaKeySession(drmAdapter.GetMediaKeySession("emeId_006"));
        }
    }
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
    OHOS::DrmAdapterImpl__IsSupported(&fdp);
    OHOS::DrmAdapterImpl__StorageProvisionedResult(&fdp);
    OHOS::DrmAdapterImpl__StorageSaveInfoResult(&fdp);
    OHOS::DrmAdapterImpl__StorageClearInfoResult(&fdp);
    OHOS::DrmAdapterImpl__ProcessKeySystemResponse(&fdp);
    OHOS::DrmAdapterImpl__StorageLoadInfoResult(&fdp);
    OHOS::DrmAdapterImpl__CreateKeySystem(&fdp);
    OHOS::DrmAdapterImpl__GenerateMediaKeyRequest(&fdp);
    OHOS::DrmAdapterImpl__UpdateSession(&fdp);
    OHOS::DrmAdapterImpl__CloseSession(&fdp);
    OHOS::DrmAdapterImpl__RemoveSession(&fdp);
    OHOS::DrmAdapterImpl__LoadSession(&fdp);
    OHOS::DrmAdapterImpl__SetConfigurationString(&fdp);
    OHOS::DrmAdapterImpl__GetConfigurationByteArray(&fdp);
    OHOS::DrmAdapterImpl__RequireSecureDecoderModule(&fdp);
    OHOS::DrmAdapterImpl__TestFunctionsSeven(&fdp);
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
    OHOS::DrmAdapterImpl__TestFunctionsEight(&fdp);
    OHOS::DrmAdapterImpl__CreateMediaKeySession(&fdp);

    return 0;
}
