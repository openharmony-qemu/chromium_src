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

#include "base/test/bind.h"
#define private public
#include "media/base/ohos/ohos_media_drm_bridge_client.h"
#include "media/base/ohos/ohos_media_drm_bridge.h"
#undef private
#include "base/functional/bind.h"
#include "base/test/test_simple_task_runner.h"
#include "media/base/cdm_promise.h"
#include "media/base/content_decryption_module.h"
#include "media/base/ohos/ohos_media_crypto_context_impl.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/drm_adapter.h"

namespace media {
class MockSimpleCdmPromise : public SimpleCdmPromise {
 public:
  MOCK_METHOD(void, resolve, (), (override));
  MOCK_METHOD(void,
              reject,
              (Exception exception_code,
               uint32_t system_code,
               const std::string& error_message),
              (override));
};

class MockNewSessionCdmPromise : public media::CdmPromiseTemplate<std::string> {
 public:
  MOCK_METHOD(void, resolve, (const std::string& result), (override));
  MOCK_METHOD(void,
              reject,
              (Exception exception_code,
               uint32_t system_code,
               const std::string& error_message),
              (override));
};

class MockOHOSMediaCryptoContext : public OHOSMediaCryptoContext {
 public:
  MOCK_METHOD(void,
              SetOHOSMediaCryptoReadyCB,
              (OHOSMediaCryptoReadyCB media_crypto_ready_cb),
              (override));
};

class MockProvisionFetcher : public ProvisionFetcher {
 public:
  MOCK_METHOD(void,
              Retrieve,
              (const GURL&, const std::string&, ResponseCB),
              (override));
  MOCK_METHOD(void,
              RetrieveWiseplayLicense,
              (const GURL&, const std::string&, ResponseCB),
              (override));
};

class OHOSMediaDrmBridgeTest : public testing::Test {
 protected:
  OHOSMediaDrmBridgeTest() = default;
  ~OHOSMediaDrmBridgeTest() override = default;

  void SetUp() override {
    std::vector<uint8_t> scheme_uuid = {0x10, 0x31};
    std::string origin_id = "example_origin_id";
    OHOSMediaDrmBridge::SecurityLevel security_level =
        OHOSMediaDrmBridge::SECURITY_LEVEL_DEFAULT;
    bool requires_media_crypto = true;
    scoped_refptr<base::SingleThreadTaskRunner> task_runner(
        base::MakeRefCounted<base::TestSimpleTaskRunner>());
    base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd(task_runner);
    std::unique_ptr<OHOSMediaDrmStorageBridge> storage =
        std::make_unique<OHOSMediaDrmStorageBridge>();
    media_drm_bridge_ = std::make_unique<OHOSMediaDrmBridge>(
        scheme_uuid, origin_id, security_level, requires_media_crypto,
        std::move(storage), create_fetcher_cb, session_message_cb,
        session_closed_cb, session_keys_change_cb,
        session_expiration_update_cb);
    cdm_promise_adapter_ = std::make_unique<CdmPromiseAdapter>();
    media_crypto_ready_cb_ = std::make_unique<MockOHOSMediaCryptoContext>();
    ohos_drm_callback_ =
        std::make_unique<OHOSDrmCallback>(task_runner,
                                          media_drm_bridge_->GetWeakPtr());
  }

  void TearDown() override {
    media_drm_bridge_.release();
    cdm_promise_adapter_.reset();
    media_crypto_ready_cb_.reset();
    ohos_drm_callback_.reset();
  }
  std::unique_ptr<OHOSMediaDrmBridge> media_drm_bridge_;
  std::unique_ptr<CdmPromiseAdapter> cdm_promise_adapter_;
  std::unique_ptr<MockOHOSMediaCryptoContext> media_crypto_ready_cb_;
  CreateFetcherCB create_fetcher_cb;
  std::unique_ptr<OHOSDrmCallback> ohos_drm_callback_;
  SessionMessageCB session_message_cb;
  SessionClosedCB session_closed_cb;
  SessionKeysChangeCB session_keys_change_cb;
  SessionExpirationUpdateCB session_expiration_update_cb;
};

TEST_F(OHOSMediaDrmBridgeTest, IsKeySystemSupported) {
  const std::string key_system = "com.test.drm";
  media_drm_bridge_->IsKeySystemSupported(key_system);
  EXPECT_FALSE(media_drm_bridge_->IsKeySystemSupported(key_system));
}

TEST_F(OHOSMediaDrmBridgeTest, IsPersistentLicenseTypeSupported) {
  const std::string key_system = "key_system";
  auto result = media_drm_bridge_->IsPersistentLicenseTypeSupported(key_system);
  EXPECT_TRUE(result);
}

TEST_F(OHOSMediaDrmBridgeTest, IsKeySystemSupportedWithType) {
  const std::string key_system = "key_system";
  const std::string container_mime_type = "container_mime_type_";
  auto result = media_drm_bridge_->IsKeySystemSupportedWithType(
      key_system, container_mime_type);
  EXPECT_FALSE(result);
}

TEST_F(OHOSMediaDrmBridgeTest, GetPlatformKeySystemNames) {
  auto result = media_drm_bridge_->GetPlatformKeySystemNames();
  EXPECT_FALSE(result.empty());
}

TEST_F(OHOSMediaDrmBridgeTest, GetUUID) {
  const std::string key_system = "key_system";
  auto result = media_drm_bridge_->GetUUID(key_system);
  EXPECT_TRUE(result.empty());
}

TEST_F(OHOSMediaDrmBridgeTest, CreateInternal) {
  std::vector<uint8_t> scheme_uuid_ = {0x10, 0x31};
  std::string origin_id_ = "example_origin_id";
  OHOSMediaDrmBridge::SecurityLevel security_level_ =
      OHOSMediaDrmBridge::SECURITY_LEVEL_DEFAULT;
  bool requires_media_crypto_ = true;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner(
      base::MakeRefCounted<base::TestSimpleTaskRunner>());
  base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd(task_runner);
  std::unique_ptr<OHOSMediaDrmStorageBridge> storage =
      std::make_unique<OHOSMediaDrmStorageBridge>();
  CreateFetcherCB create_fetcher_cb_;
  SessionMessageCB session_message_cb_;
  SessionClosedCB session_closed_cb_;
  SessionKeysChangeCB session_keys_change_cb_;
  SessionExpirationUpdateCB session_expiration_update_cb_;
  auto media_drm_bridge = OHOSMediaDrmBridge::CreateInternal(scheme_uuid_, origin_id_, security_level_,
                                    requires_media_crypto_, std::move(storage),
                                    create_fetcher_cb_, session_message_cb_,
                                    session_closed_cb_, session_keys_change_cb_,
                                    session_expiration_update_cb_);
  ASSERT_NE(media_drm_bridge, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, SetServerCertificate) {
  std::unique_ptr<MockSimpleCdmPromise> mock_promise =
      std::make_unique<MockSimpleCdmPromise>();
  EXPECT_CALL(*mock_promise, reject(testing::_, testing::_, testing::_))
      .Times(1);
  std::vector<uint8_t> certificate = {0x00, 0x01, 0x02, 0x03};
  media_drm_bridge_->SetServerCertificate(certificate, std::move(mock_promise));
  ASSERT_EQ(mock_promise, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, CreateSessionAndGenerateRequest001) {
  media::EmeInitDataType init_data_type = media::EmeInitDataType::WEBM;
  std::vector<uint8_t> init_data = {0x00, 0x01, 0x02, 0x03};
  CdmSessionType session_type = CdmSessionType::kTemporary;
  std::unique_ptr<MockNewSessionCdmPromise> mock_promise =
      std::make_unique<MockNewSessionCdmPromise>();
  EXPECT_CALL(*mock_promise, reject(testing::_, testing::_, testing::_))
      .Times(1);
  auto media_drm_bridge_client = std::make_unique<OHOSMediaDrmBridgeClient>();
  SetMediaDrmBridgeClient(media_drm_bridge_client.get());
  media_drm_bridge_->CreateSessionAndGenerateRequest(
      session_type, init_data_type, init_data, std::move(mock_promise));
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
  media_drm_bridge_.reset();
}

TEST_F(OHOSMediaDrmBridgeTest, CreateSessionAndGenerateRequest002) {
  media::EmeInitDataType init_data_type = media::EmeInitDataType::WEBM;
  std::vector<uint8_t> init_data = {0x00, 0x01, 0x02, 0x03};
  CdmSessionType session_type = CdmSessionType::kTemporary;
  std::unique_ptr<MockNewSessionCdmPromise> mock_promise =
      std::make_unique<MockNewSessionCdmPromise>();
  EXPECT_CALL(*mock_promise, reject(testing::_, testing::_, testing::_))
      .Times(1);
  media_drm_bridge_->ohos_drm_adapter_ = nullptr;
  auto media_drm_bridge_client = std::make_unique<OHOSMediaDrmBridgeClient>();
  SetMediaDrmBridgeClient(media_drm_bridge_client.get());
  media_drm_bridge_->CreateSessionAndGenerateRequest(
      session_type, init_data_type, init_data, std::move(mock_promise));
  ASSERT_EQ(media_drm_bridge_->ohos_drm_adapter_, nullptr);
  media_drm_bridge_.reset();
}

TEST_F(OHOSMediaDrmBridgeTest, LoadSession001) {
  const std::string session_id = "session_id";
  CdmSessionType session_type = CdmSessionType::kTemporary;
  std::unique_ptr<MockNewSessionCdmPromise> mock_promise =
      std::make_unique<MockNewSessionCdmPromise>();
  EXPECT_CALL(*mock_promise, reject(testing::_, testing::_, testing::_))
      .Times(1);
  media_drm_bridge_->LoadSession(session_type, session_id,
                                 std::move(mock_promise));
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, LoadSession002) {
  const std::string session_id = "session_id";
  CdmSessionType session_type = CdmSessionType::kPersistentLicense;
  std::unique_ptr<MockNewSessionCdmPromise> mock_promise =
      std::make_unique<MockNewSessionCdmPromise>();
  EXPECT_CALL(*mock_promise, reject(testing::_, testing::_, testing::_))
      .Times(1);
  media_drm_bridge_->ohos_drm_adapter_ = nullptr;
  media_drm_bridge_->LoadSession(session_type, session_id,
                                 std::move(mock_promise));
  ASSERT_EQ(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, UpdateSession001) {
  const std::string session_id = "session_id";
  const std::vector<uint8_t> response = {1};
  std::unique_ptr<MockSimpleCdmPromise> mock_promise =
      std::make_unique<MockSimpleCdmPromise>();
  EXPECT_CALL(*mock_promise, reject(testing::_, testing::_, testing::_))
      .Times(1);
  media_drm_bridge_->UpdateSession(session_id, response,
                                   std::move(mock_promise));
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
  media_drm_bridge_.reset();
}

TEST_F(OHOSMediaDrmBridgeTest, UpdateSession002) {
  const std::string session_id = "session_id";
  const std::vector<uint8_t> response = {1};
  std::unique_ptr<MockSimpleCdmPromise> mock_promise =
      std::make_unique<MockSimpleCdmPromise>();
  EXPECT_CALL(*mock_promise, reject(testing::_, testing::_, testing::_))
      .Times(1);
  media_drm_bridge_->ohos_drm_adapter_ = nullptr;
  media_drm_bridge_->UpdateSession(session_id, response,
                                   std::move(mock_promise));
  ASSERT_EQ(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, CloseSession001) {
  const std::string session_id = "session_id";
  std::unique_ptr<MockSimpleCdmPromise> mock_promise =
      std::make_unique<MockSimpleCdmPromise>();
  EXPECT_CALL(*mock_promise, reject(testing::_, testing::_, testing::_))
      .Times(1);
  media_drm_bridge_->CloseSession(session_id, std::move(mock_promise));
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
  media_drm_bridge_.reset();
}

TEST_F(OHOSMediaDrmBridgeTest, CloseSession002) {
  const std::string session_id = "session_id";
  std::unique_ptr<MockSimpleCdmPromise> mock_promise =
      std::make_unique<MockSimpleCdmPromise>();
  EXPECT_CALL(*mock_promise, reject(testing::_, testing::_, testing::_))
      .Times(1);
  media_drm_bridge_->ohos_drm_adapter_ = nullptr;
  media_drm_bridge_->CloseSession(session_id, std::move(mock_promise));
  ASSERT_EQ(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, RemoveSession001) {
  const std::string session_id = "session_id";
  std::unique_ptr<MockSimpleCdmPromise> mock_promise =
      std::make_unique<MockSimpleCdmPromise>();
  EXPECT_CALL(*mock_promise, reject(testing::_, testing::_, testing::_))
      .Times(1);
  media_drm_bridge_->RemoveSession(session_id, std::move(mock_promise));
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
  media_drm_bridge_.reset();
}

TEST_F(OHOSMediaDrmBridgeTest, RemoveSession002) {
  const std::string session_id = "session_id";
  std::unique_ptr<MockSimpleCdmPromise> mock_promise =
      std::make_unique<MockSimpleCdmPromise>();
  EXPECT_CALL(*mock_promise, reject(testing::_, testing::_, testing::_))
      .Times(1);
  media_drm_bridge_->ohos_drm_adapter_ = nullptr;
  media_drm_bridge_->RemoveSession(session_id, std::move(mock_promise));
  ASSERT_EQ(media_drm_bridge_->ohos_drm_adapter_, nullptr);
  media_drm_bridge_.reset();
}

TEST_F(OHOSMediaDrmBridgeTest, GetCdmContext) {
  auto result = media_drm_bridge_->GetCdmContext();
  ASSERT_NE(result, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, DeleteOnCorrectThread) {
  media_drm_bridge_->DeleteOnCorrectThread();
  ASSERT_NE(media_drm_bridge_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, RegisterEventCB) {
  base::RepeatingCallback<void(const media::CdmContext::Event)> event_cb =
      base::BindRepeating([](const media::CdmContext::Event event) -> void {});
  scoped_refptr<base::SingleThreadTaskRunner> task_runner(
      base::MakeRefCounted<base::TestSimpleTaskRunner>());
  base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd(task_runner);
  auto result = media_drm_bridge_->RegisterEventCB(event_cb);
  ASSERT_NE(result, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, GetOHOSMediaCryptoContext) {
  auto result = media_drm_bridge_->GetOHOSMediaCryptoContext();
  ASSERT_NE(result, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, IsSecureCodecRequired) {
  auto result = media_drm_bridge_->IsSecureCodecRequired();
  ASSERT_EQ(result, false);
}

TEST_F(OHOSMediaDrmBridgeTest, Provision) {
  base::OnceCallback<void(bool)> provisioning_complete_cb =
      base::BindOnce([](bool success) {});
  media_drm_bridge_->Provision(std::move(provisioning_complete_cb));
  ASSERT_NE(media_drm_bridge_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, Unprovision) {
  media_drm_bridge_->Unprovision();
  ASSERT_NE(media_drm_bridge_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, ResolvePromise) {
  uint32_t promise_id = 1;
  media_drm_bridge_->ResolvePromise(promise_id);
  ASSERT_NE(cdm_promise_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, ResolvePromiseWithSession) {
  uint32_t promise_id = 1;
  const std::string session_id = "session_id";
  media_drm_bridge_->ResolvePromiseWithSession(promise_id, session_id);
  ASSERT_NE(cdm_promise_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, RejectPromise) {
  uint32_t promise_id = 1;
  CdmPromise::Exception exception_code = {
      CdmPromise::Exception::NOT_SUPPORTED_ERROR};
  std::string error_message = "error_message";
  media_drm_bridge_->RejectPromise(promise_id, exception_code, error_message);
  ASSERT_NE(cdm_promise_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, SetOHOSMediaCryptoReadyCB) {
  MockOHOSMediaCryptoContext::OHOSMediaCryptoReadyCB media_crypto_ready_cb =
      base::BindOnce([](void* session, bool requires_secure_video_codec) {});
  media_drm_bridge_->SetOHOSMediaCryptoReadyCB(
      std::move(media_crypto_ready_cb));
  ASSERT_NE(media_crypto_ready_cb_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnOHOSMediaCryptoReady) {
  media_drm_bridge_->OnOHOSMediaCryptoReady(nullptr);
  ASSERT_NE(media_drm_bridge_->task_runner_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnProvisionRequest) {
  std::string defaultUrl = "defaultUrl";
  std::string requestData = "requestData";
  media_drm_bridge_->OnProvisionRequest(defaultUrl, requestData);
  ASSERT_NE(media_drm_bridge_->task_runner_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnProvisioningComplete) {
  bool success = true;
  media_drm_bridge_->provisioning_complete_cb_ =
      base::BindOnce([](bool success) {});
  scoped_refptr<base::SingleThreadTaskRunner> task_runner(
      base::MakeRefCounted<base::TestSimpleTaskRunner>());
  base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner);
  media_drm_bridge_->OnProvisioningComplete(success);
  ASSERT_NE(media_drm_bridge_->task_runner_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnPromiseResolved) {
  uint32_t promise_id = 1;
  media_drm_bridge_->OnPromiseResolved(promise_id);
  ASSERT_NE(media_drm_bridge_->task_runner_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnPromiseResolvedWithSession) {
  uint32_t promise_id = 1;
  std::string session_id = "session_id";
  media_drm_bridge_->OnPromiseResolvedWithSession(promise_id, session_id);
  ASSERT_NE(media_drm_bridge_->task_runner_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnPromiseRejected) {
  uint32_t promise_id = 1;
  std::string error_message = "error_message";
  media_drm_bridge_->OnPromiseRejected(promise_id, error_message);
  ASSERT_NE(media_drm_bridge_->task_runner_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageProvisionedResult) {
  bool result = true;
  media_drm_bridge_->OnStorageProvisionedResult(result);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageProvisioned) {
  media_drm_bridge_->storage_ = nullptr;
  media_drm_bridge_->OnStorageProvisioned();
  ASSERT_EQ(media_drm_bridge_->storage_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageSaveInfoUpdateResult001) {
  bool result = true;
  media_drm_bridge_->OnStorageSaveInfoUpdateResult(result);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageSaveInfoUpdateResult002) {
  bool result = true;
  media_drm_bridge_->ohos_drm_adapter_ = nullptr;
  media_drm_bridge_->OnStorageSaveInfoUpdateResult(result);
  ASSERT_EQ(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageSaveInfoRemoveResult001) {
  bool result = true;
  media_drm_bridge_->OnStorageSaveInfoRemoveResult(result);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageSaveInfoRemoveResult002) {
  bool result = true;
  media_drm_bridge_->ohos_drm_adapter_ = nullptr;
  media_drm_bridge_->OnStorageSaveInfoRemoveResult(result);
  ASSERT_EQ(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageSaveInfo) {
  std::vector<uint8_t> ket_set_id = {1};
  std::string mime_type = "mime_type";
  std::string session_id = "session_id";
  int32_t key_type = 1;
  media_drm_bridge_->storage_ = nullptr;
  media_drm_bridge_->OnStorageSaveInfo(ket_set_id, mime_type, session_id,
                                       key_type);
  ASSERT_EQ(media_drm_bridge_->storage_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageLoadInfoResult001) {
  std::vector<uint8_t> ket_set_id = {1};
  std::string mime = "mime_type";
  std::string session_id = "session_id";
  uint32_t key_type = 1;
  media_drm_bridge_->ohos_drm_adapter_ = nullptr;
  media_drm_bridge_->OnStorageLoadInfoResult(session_id, ket_set_id, mime,
                                             key_type);
  ASSERT_EQ(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageLoadInfoResult002) {
  std::vector<uint8_t> ket_set_id = {1};
  std::string mime = "mime_type";
  std::string session_id = "session_id";
  uint32_t key_type = 1;
  media_drm_bridge_->OnStorageLoadInfoResult(session_id, ket_set_id, mime,
                                             key_type);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageLoadInfo) {
  std::string session_id = "session_id";
  media_drm_bridge_->storage_ = nullptr;
  media_drm_bridge_->OnStorageLoadInfo(session_id);
  ASSERT_EQ(media_drm_bridge_->storage_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageClearInfoForKeyReleaseResult001) {
  bool result = true;
  media_drm_bridge_->ohos_drm_adapter_ = nullptr;
  media_drm_bridge_->OnStorageClearInfoForKeyReleaseResult(result);
  ASSERT_EQ(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageClearInfoForKeyReleaseResult002) {
  bool result = true;
  media_drm_bridge_->OnStorageClearInfoForKeyReleaseResult(result);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageClearInfoForKeyRelease) {
  std::string session_id = "session_id";
  media_drm_bridge_->storage_ = nullptr;
  media_drm_bridge_->OnStorageClearInfoForKeyRelease(session_id);
  ASSERT_EQ(media_drm_bridge_->storage_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageClearInfoForLoadFailResult001) {
  bool result = true;
  media_drm_bridge_->ohos_drm_adapter_ = nullptr;
  media_drm_bridge_->OnStorageClearInfoForLoadFailResult(result);
  ASSERT_EQ(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageClearInfoForLoadFailResult002) {
  bool result = true;
  media_drm_bridge_->OnStorageClearInfoForLoadFailResult(result);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageClearInfoForLoadFail) {
  std::string session_id = "session_id";
  media_drm_bridge_->storage_ = nullptr;
  media_drm_bridge_->OnStorageClearInfoForLoadFail(session_id);
  ASSERT_EQ(media_drm_bridge_->storage_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnSessionMessage) {
  std::string sessionId = "sessionId";
  int32_t type = 1;
  std::vector<uint8_t> message = {0x01, 0x02, 0x03, 0x04, 0x05};
  media_drm_bridge_->session_message_cb_ = base::BindRepeating(
      [](const std::string& session_id, CdmMessageType message_type,
         const std::vector<uint8_t>& message) {});
  ohos_drm_callback_->OnSessionMessage(sessionId, type, message);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
  media_drm_bridge_.reset();
}

TEST_F(OHOSMediaDrmBridgeTest, OnSessionClosed) {
  std::string session_id = "session_id";
  media_drm_bridge_->session_closed_cb_ = base::BindRepeating(
      [](const std::string& session_id, CdmSessionClosedReason reason) {});
  media_drm_bridge_->OnSessionClosed(session_id);
  ASSERT_NE(media_drm_bridge_->task_runner_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnSessionKeysChange) {
  std::string session_id = "session_id";
  std::vector<std::string> keyIdArray = {};
  std::vector<uint32_t> statusArray = {};
  bool has_additional_usable_key = false;
  SessionKeysChangeCB session_keys_change_cb;
  CdmKeysInfo cdm_keys_info;
  bool is_key_release = true;
  media_drm_bridge_->session_keys_change_cb_ = base::BindRepeating(
      [](const std::string& session_id, bool has_additional_usable_key,
         CdmKeysInfo keys_info) {});
  media_drm_bridge_->OnSessionKeysChange(session_id, keyIdArray, statusArray,
                                         has_additional_usable_key,
                                         is_key_release);
  ASSERT_EQ(keyIdArray.size(), statusArray.size());
}

TEST_F(OHOSMediaDrmBridgeTest, OnSessionExpirationUpdate) {
  std::string session_id = "session_id";
  uint64_t expiry_time_ms = 1;
  media_drm_bridge_->session_expiration_update_cb_ = base::BindRepeating(
      [](const std::string& session_id, base::Time new_expiry_time) {});
  media_drm_bridge_->OnSessionExpirationUpdate(session_id, expiry_time_ms);
  ASSERT_NE(media_drm_bridge_->task_runner_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, GetSecurityLevel001) {
  media_drm_bridge_->GetSecurityLevel();
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, GetSecurityLevel002) {
  media_drm_bridge_->ohos_drm_adapter_ = nullptr;
  media_drm_bridge_->GetSecurityLevel();
  ASSERT_EQ(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, NotifyMediaCryptoReady) {
  media_drm_bridge_->NotifyMediaCryptoReady();
  ASSERT_NE(media_crypto_ready_cb_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, ProcessProvisionResponse001) {
  bool success = false;
  const std::string response = "response";
  media_drm_bridge_->ProcessProvisionResponse(success, response);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, ProcessProvisionResponse002) {
  bool success = true;
  const std::string response = "response";
  media_drm_bridge_->ohos_drm_adapter_ = nullptr;
  media_drm_bridge_->ProcessProvisionResponse(success, response);
  ASSERT_EQ(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, ProcessProvisionResponse003) {
  bool success = true;
  const std::string response = "response";
  media_drm_bridge_->ProcessProvisionResponse(success, response);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnHasAdditionalUsableKey) {
  media_drm_bridge_->OnHasAdditionalUsableKey();
  ASSERT_NE(media_drm_bridge_->task_runner_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnProvisionRequestCallback) {
  std::string defaultUrl = "defaultUrl";
  std::string requestData = "requestData";
  ohos_drm_callback_->OnProvisionRequest(defaultUrl, requestData);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnProvisioningCompleteCallback) {
  bool success = true;
  media_drm_bridge_->provisioning_complete_cb_ =
      base::BindOnce([](bool success) {});
  ohos_drm_callback_->OnProvisioningComplete(success);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnMediaKeySessionReady) {
  void* session = nullptr;
  ohos_drm_callback_->OnMediaKeySessionReady(session);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnPromiseRejectedCallback) {
  uint32_t promiseId = 0;
  std::string errorMessage = "errorMessage";
  ohos_drm_callback_->OnPromiseRejected(promiseId, errorMessage);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnPromiseResolvedCallback) {
  uint32_t promiseId = 0;
  ohos_drm_callback_->OnPromiseResolved(promiseId);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnPromiseResolvedWithSessionCallback) {
  uint32_t promiseId = 0;
  std::string sessionId = "sessionId";
  ohos_drm_callback_->OnPromiseResolvedWithSession(promiseId, sessionId);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnSessionClosedCallback) {
  std::string sessionId = "sessionId";
  media_drm_bridge_->session_closed_cb_ = base::BindRepeating(
      [](const std::string& session_id, CdmSessionClosedReason reason) {});
  ohos_drm_callback_->OnSessionClosed(sessionId);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnSessionExpirationUpdateCallback) {
  std::string sessionId = "sessionId";
  uint64_t expiration_time = 1;
  media_drm_bridge_->session_expiration_update_cb_ = base::BindRepeating(
      [](const std::string& session_id, base::Time new_expiry_time) {});
  ohos_drm_callback_->OnSessionExpirationUpdate(sessionId, expiration_time);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnSessionKeysChangeCallback) {
  std::string sessionId = "sessionId";
  std::vector<std::string> keyIdArray = {};
  std::vector<uint32_t> statusArray = {};
  bool has_additional_usable_key = true;
  bool is_key_release = true;
  media_drm_bridge_->session_keys_change_cb_ = base::BindRepeating(
      [](const std::string& session_id, bool has_additional_usable_key,
         CdmKeysInfo keys_info) {});
  ohos_drm_callback_->OnSessionKeysChange(sessionId, keyIdArray, statusArray,
                                          has_additional_usable_key,
                                          is_key_release);
  ASSERT_NE(media_drm_bridge_->ohos_drm_adapter_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageClearInfoForKeyReleaseCallback) {
  std::string session_id = "session_id";
  media_drm_bridge_->storage_ = nullptr;
  ohos_drm_callback_->OnStorageClearInfoForKeyRelease(session_id);
  ASSERT_EQ(media_drm_bridge_->storage_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageProvisionedCallback) {
  media_drm_bridge_->storage_ = nullptr;
  ohos_drm_callback_->OnStorageProvisioned();
  ASSERT_EQ(media_drm_bridge_->storage_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageSaveInfoCallback) {
  std::vector<uint8_t> ket_set_id = {1};
  std::string mime_type = "mime_type";
  std::string session_id = "session_id";
  int32_t key_type = 1;
  media_drm_bridge_->storage_ = nullptr;
  ohos_drm_callback_->OnStorageSaveInfo(ket_set_id, mime_type, session_id,
                                        key_type);
  ASSERT_EQ(media_drm_bridge_->storage_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageLoadInfoCallback) {
  std::string session_id = "session_id";
  media_drm_bridge_->storage_ = nullptr;
  ohos_drm_callback_->OnStorageLoadInfo(session_id);
  ASSERT_EQ(media_drm_bridge_->storage_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeTest, SendProvisioningRequest) {
  const GURL default_url("default_url");
  const std::string request_data = "request_data";
  std::unique_ptr<MockProvisionFetcher> mock_provision_fetcher =
      std::make_unique<MockProvisionFetcher>();
  EXPECT_CALL(*mock_provision_fetcher,
              Retrieve(testing::_, testing::_, testing::_))
      .Times(testing::AnyNumber());
  media_drm_bridge_->create_fetcher_cb_ =
      base::BindLambdaForTesting([&]() -> std::unique_ptr<ProvisionFetcher> {
        return std::move(mock_provision_fetcher);
      });
  media_drm_bridge_->SendProvisioningRequest(default_url, request_data);
  ASSERT_NE(media_drm_bridge_->provision_fetcher_, nullptr);
  media_drm_bridge_.reset();
}

TEST_F(OHOSMediaDrmBridgeTest, OnStorageClearInfoForLoadFailCallback) {
  std::string session_id = "session_id";
  media_drm_bridge_->storage_ = nullptr;
  ohos_drm_callback_->OnStorageClearInfoForLoadFail(session_id);
  ASSERT_EQ(media_drm_bridge_->storage_, nullptr);
}
}  // namespace media
