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
#include "media/base/ohos/ohos_media_drm_bridge_client.h"
#include "media/base/ohos/ohos_media_drm_bridge_factory.h"
#undef private
#include "base/functional/callback_helpers.h"
#include "base/test/test_simple_task_runner.h"
#include "media/base/cdm_config.h"
#include "media/base/content_decryption_module.h"
#include "media/base/media_drm_storage.h"
#include "media/base/ohos/ohos_media_drm_bridge.h"
#include "media/base/provision_fetcher.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/widevine/cdm/widevine_cdm_common.h"
using ::testing::_;

namespace media {
class MockMediaDrmStorage : public MediaDrmStorage {
 public:
  MOCK_METHOD(void, Initialize, (InitCB init_cb), (override));
  MOCK_METHOD(void, OnProvisioned, (ResultCB result_cb), (override));
  MOCK_METHOD(void,
              SavePersistentSession,
              (const std::string& session_id,
               const SessionData& session_data,
               ResultCB result_cb),
              (override));
  MOCK_METHOD(void,
              LoadPersistentSession,
              (const std::string& session_id,
               LoadPersistentSessionCB load_persistent_session_cb),
              (override));
  MOCK_METHOD(void,
              RemovePersistentSession,
              (const std::string& session_id, ResultCB result_cb),
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

class OHOSMediaDrmBridgeFactoryTest : public ::testing::Test {
 protected:
  OHOSMediaDrmBridgeFactoryTest() = default;
  ~OHOSMediaDrmBridgeFactoryTest() override = default;

  void SetUp() override {
    create_fetcher_cb_ =
        base::BindRepeating([]() -> std::unique_ptr<ProvisionFetcher> {
          return std::make_unique<MockProvisionFetcher>();
        });
    create_storage_cb_ =
        base::BindRepeating([]() -> std::unique_ptr<MediaDrmStorage> {
          return std::make_unique<MockMediaDrmStorage>();
        });
    factory_ = std::make_unique<OHOSMediaDrmBridgeFactory>(create_fetcher_cb_,
                                                           create_storage_cb_);
  }

  void TearDown() override {
    factory_.reset();
    create_storage_cb_.Reset();
    create_fetcher_cb_.Reset();
  }

  CdmConfig cdm_config_;
  CreateFetcherCB create_fetcher_cb_;
  CreateStorageCB create_storage_cb_;
  std::unique_ptr<OHOSMediaDrmBridgeFactory> factory_;
  CdmCreatedCB cdm_created_cb_;
};

TEST_F(OHOSMediaDrmBridgeFactoryTest, Create) {
  auto media_drm_bridge_client = std::make_unique<OHOSMediaDrmBridgeClient>();
  SetMediaDrmBridgeClient(media_drm_bridge_client.get());
  cdm_config_.key_system = "com.wiseplay.drm";
  bool isSupported =
      OHOSMediaDrmBridge::IsKeySystemSupported(cdm_config_.key_system);
  cdm_config_.use_hw_secure_codecs = true;
  SessionMessageCB session_message_cb;
  SessionClosedCB session_closed_cb;
  SessionKeysChangeCB session_keys_change_cb;
  SessionExpirationUpdateCB session_expiration_update_cb;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner(
      base::MakeRefCounted<base::TestSimpleTaskRunner>());
  base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd(task_runner);
  auto cdm_created_cb = [](const scoped_refptr<ContentDecryptionModule>& cdm,
                           const std::string& error_message) {};
  factory_->create_storage_cb_ =
      base::BindRepeating([]() -> std::unique_ptr<MediaDrmStorage> {
        auto storage = std::make_unique<MockMediaDrmStorage>();
        EXPECT_CALL(*storage, Initialize(_)).Times(testing::AtLeast(0));
        return storage;
      });
  factory_->cdm_created_cb_ = base::BindOnce(cdm_created_cb);
  if (isSupported) {
    factory_->Create(cdm_config_, session_message_cb, session_closed_cb,
                    session_keys_change_cb, session_expiration_update_cb,
                    std::move(cdm_created_cb_));
    ASSERT_NE(factory_->storage_, nullptr);
  }
}

TEST_F(OHOSMediaDrmBridgeFactoryTest, CreateMediaDrmBridge) {
  std::vector<uint8_t> scheme_uuid_ = {0x10, 0x31};
  std::string origin_id_ = "origin_id";
  scoped_refptr<base::SingleThreadTaskRunner> task_runner(
      base::MakeRefCounted<base::TestSimpleTaskRunner>());
  base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd(task_runner);
  auto cdm_created_cb = [](const scoped_refptr<ContentDecryptionModule>& cdm,
                           const std::string& error_message) {};
  factory_->cdm_created_cb_ = base::BindOnce(cdm_created_cb);
  factory_->CreateMediaDrmBridge(origin_id_);
  ASSERT_NE(factory_->ohos_media_drm_bridge_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeFactoryTest, OnStorageInitialized) {
  bool requires_secure_video_codec = true;
  std::vector<uint8_t> scheme_uuid_ = {0x10, 0x31};
  std::string origin_id_ = "origin_id";
  scoped_refptr<base::SingleThreadTaskRunner> task_runner(
      base::MakeRefCounted<base::TestSimpleTaskRunner>());
  base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd(task_runner);
  auto cdm_created_cb = [](const scoped_refptr<ContentDecryptionModule>& cdm,
                           const std::string& error_message) {};
  factory_->cdm_created_cb_ = base::BindOnce(cdm_created_cb);
  factory_->OnStorageInitialized(requires_secure_video_codec);
  ASSERT_EQ(factory_->storage_, nullptr);
}

TEST_F(OHOSMediaDrmBridgeFactoryTest, OnOHOSMediaCryptoReady) {
  cdm_config_.key_system = "nullptr";
  cdm_config_.use_hw_secure_codecs = true;
  void* session = nullptr;
  bool requires_secure_video_codec = true;
  auto cdm_created_cb = [](const scoped_refptr<ContentDecryptionModule>& cdm,
                           const std::string& error_message) {};
  factory_->cdm_created_cb_ = base::BindOnce(cdm_created_cb);
  factory_->OnOHOSMediaCryptoReady(session, requires_secure_video_codec);
  ASSERT_EQ(factory_->ohos_media_drm_bridge_, nullptr);
}
}  // namespace media
