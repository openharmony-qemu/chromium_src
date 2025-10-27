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

#include "base/test/test_simple_task_runner.h"
#include "media/base/cdm_config.h"
#include "media/base/content_decryption_module.h"
#include "media/base/ohos/ohos_media_drm_bridge.h"
#include "media/base/provision_fetcher.h"
#include "media/cdm/aes_decryptor.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "arkweb/chromium_ext/media/base/ohos/ohos_cdm_factory.h"
#undef private
#include "media/cdm/clear_key_cdm_common.h"
#include "arkweb/chromium_ext/media/cdm/wiseplay_cdm_common.h"
#include "media/mojo/services/ohos_mojo_util.h"
#include "media/mojo/services/mojo_media_drm_storage.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "media/base/media_drm_storage.h"
#include "media/base/provision_fetcher.h"

using ::testing::_;

namespace media {

std::unique_ptr<ProvisionFetcher> CreateProvisionFetcher() {
  mojo::PendingRemote<mojom::ProvisionFetcher> provision_fetcher;
  return std::make_unique<MojoProvisionFetcher>(std::move(provision_fetcher));
}

std::unique_ptr<MediaDrmStorage> CreateMediaDrmStorage() {
  mojo::PendingRemote<mojom::MediaDrmStorage> media_drm_storage;
  return std::make_unique<MojoMediaDrmStorage>(std::move(media_drm_storage));
}

class MockContentDecryptionModule : public ContentDecryptionModule {
 public:
  MOCK_METHOD(void,
              SetServerCertificate,
              (const std::vector<uint8_t>& certificate,
               std::unique_ptr<SimpleCdmPromise> promise),
              (override));
  MOCK_METHOD(void,
              GetStatusForPolicy,
              (HdcpVersion min_hdcp_version,
               std::unique_ptr<KeyStatusCdmPromise> promise),
              ());
  MOCK_METHOD(void,
              CreateSessionAndGenerateRequest,
              (CdmSessionType session_type,
               EmeInitDataType init_data_type,
               const std::vector<uint8_t>& init_data,
               std::unique_ptr<NewSessionCdmPromise> promise),
              (override));
  MOCK_METHOD(void,
              LoadSession,
              (CdmSessionType session_type,
               const std::string& session_id,
               std::unique_ptr<NewSessionCdmPromise> promise),
              (override));
  MOCK_METHOD(void,
              UpdateSession,
              (const std::string& session_id,
               const std::vector<uint8_t>& response,
               std::unique_ptr<SimpleCdmPromise> promise),
              (override));
  MOCK_METHOD(void,
              CloseSession,
              (const std::string& session_id,
               std::unique_ptr<SimpleCdmPromise> promise),
              (override));
  MOCK_METHOD(void,
              RemoveSession,
              (const std::string& session_id,
               std::unique_ptr<SimpleCdmPromise> promise),
              (override));
  MOCK_METHOD(CdmContext*, GetCdmContext, (), (override));
};

class OHOSCdmFactoryTest : public ::testing::Test {
 protected:
  OHOSCdmFactoryTest() = default;
  ~OHOSCdmFactoryTest() override = default;

  std::unique_ptr<media::OHOSCdmFactory> cdm_factory_;

  void SetUp() override {
    cdm_factory_ = std::make_unique<media::OHOSCdmFactory>(base::BindRepeating(&CreateProvisionFetcher),
                                                           base::BindRepeating(&CreateMediaDrmStorage));
  }

  void TearDown() override { cdm_factory_.reset(); }
};

TEST_F(OHOSCdmFactoryTest, Create) {
  CdmConfig cdm_config;
  SessionMessageCB session_message_cb;
  SessionClosedCB session_closed_cb;
  SessionKeysChangeCB session_keys_change_cb;
  SessionExpirationUpdateCB session_expiration_update_cb;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner(
      base::MakeRefCounted<base::TestSimpleTaskRunner>());
  base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner);

  {
    auto cb = [](const scoped_refptr<ContentDecryptionModule>& cdm, CreateCdmStatus cdmStatus) {};
    CdmCreatedCB cdm_created_cb = base::BindOnce(cb);
    cdm_factory_->Create(cdm_config, session_message_cb, session_closed_cb,
                         session_keys_change_cb, session_expiration_update_cb,
                         std::move(cdm_created_cb));
    EXPECT_TRUE(cdm_factory_->pending_creations_.empty());
  }

  {
    cdm_config.key_system = media::kClearKeyKeySystem;
    auto cb = [](const scoped_refptr<ContentDecryptionModule>& cdm, CreateCdmStatus cdmStatus) {};
    CdmCreatedCB cdm_created_cb = base::BindOnce(cb);
    cdm_factory_->Create(cdm_config, session_message_cb, session_closed_cb,
                         session_keys_change_cb, session_expiration_update_cb,
                         std::move(cdm_created_cb));
    EXPECT_TRUE(cdm_factory_->pending_creations_.empty());
  }

  {
    cdm_config.key_system = "org.w3.clearkey.test";
    auto cb = [](const scoped_refptr<ContentDecryptionModule>& cdm, CreateCdmStatus cdmStatus) {};
    CdmCreatedCB cdm_created_cb = base::BindOnce(cb);
    cdm_factory_->Create(cdm_config, session_message_cb, session_closed_cb,
                         session_keys_change_cb, session_expiration_update_cb,
                         std::move(cdm_created_cb));
    EXPECT_TRUE(cdm_factory_->pending_creations_.empty());
  }

  {
    cdm_config.key_system = media::kWiseplayKeySystem;
    auto cb = [](const scoped_refptr<ContentDecryptionModule>& cdm, CreateCdmStatus cdmStatus) {};
    CdmCreatedCB cdm_created_cb = base::BindOnce(cb);
    cdm_factory_->Create(cdm_config, session_message_cb, session_closed_cb,
                         session_keys_change_cb, session_expiration_update_cb,
                         std::move(cdm_created_cb));
    EXPECT_TRUE(cdm_factory_->pending_creations_.empty());
  }
}

TEST_F(OHOSCdmFactoryTest, OnCdmCreated) {
  const uint32_t creation_id = 1;
  scoped_refptr<MockContentDecryptionModule> cdm(
      new MockContentDecryptionModule());
  EXPECT_CALL(*cdm, CreateSessionAndGenerateRequest(_, _, _, _)).Times(0);
  EXPECT_TRUE(cdm.get() != nullptr);

  CreateCdmStatus cdmStatus = CreateCdmStatus::kSuccess;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner(
      base::MakeRefCounted<base::TestSimpleTaskRunner>());
  base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner);
  auto cdm_created_cb_ = [](const scoped_refptr<ContentDecryptionModule>& cdm,
                            CreateCdmStatus status) {};
  CdmCreatedCB cdm_created_cb = base::BindOnce(cdm_created_cb_);
  cdm_factory_->pending_creations_.emplace(
      creation_id, std::make_pair(nullptr, std::move(cdm_created_cb)));
  cdm_factory_->OnCdmCreated(creation_id, cdm, cdmStatus);
  ASSERT_NE(cdm_factory_, nullptr);
  EXPECT_TRUE(cdm_factory_->pending_creations_.empty());
}
}  // namespace media
