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
#include "media/base/ohos/ohos_cdm_factory.h"
#undef private
using ::testing::_;

namespace media {
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

  void SetUp() override {
    CreateFetcherCB create_fetcher_cb_;
    CreateStorageCB create_storage_cb_;
    cdm_factory = std::make_unique<media::OHOSCdmFactory>(create_fetcher_cb_,
                                                          create_storage_cb_);
  }

  void TearDown() override { cdm_factory.reset(); }
  std::unique_ptr<media::OHOSCdmFactory> cdm_factory;
};

TEST_F(OHOSCdmFactoryTest, Create) {
  CdmConfig cdm_config;
  SessionMessageCB session_message_cb;
  SessionClosedCB session_closed_cb;
  SessionKeysChangeCB session_keys_change_cb;
  SessionExpirationUpdateCB session_expiration_update_cb;
  scoped_refptr<ContentDecryptionModule> created_cdm;
  std::string error_message;
  CdmCreatedCB cdm_created_cb = base::BindOnce(
      [](scoped_refptr<ContentDecryptionModule>* out_cdm,
         std::string* out_error_message,
         const scoped_refptr<ContentDecryptionModule>& cdm,
         const std::string& error_message) {
        *out_cdm = cdm;
        *out_error_message = error_message;
      },
      &created_cdm, &error_message);
  scoped_refptr<base::SingleThreadTaskRunner> task_runner(
      base::MakeRefCounted<base::TestSimpleTaskRunner>());
  base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner);

  cdm_factory->Create(cdm_config, session_message_cb, session_closed_cb,
                      session_keys_change_cb, session_expiration_update_cb,
                      std::move(cdm_created_cb));
  EXPECT_TRUE(cdm_factory->pending_creations_.empty());
}

TEST_F(OHOSCdmFactoryTest, OnCdmCreated) {
  const uint32_t creation_id = 1;
  scoped_refptr<MockContentDecryptionModule> cdm(
      new MockContentDecryptionModule());
  EXPECT_CALL(*cdm, CreateSessionAndGenerateRequest(_, _, _, _)).Times(0);
  EXPECT_TRUE(cdm.get() != nullptr);
  std::string error_message = "Success";
  scoped_refptr<base::SingleThreadTaskRunner> task_runner(
      base::MakeRefCounted<base::TestSimpleTaskRunner>());
  base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner);
  auto cdm_created_cb_ = [](const scoped_refptr<ContentDecryptionModule>& cdm,
                            const std::string& error_message) {};
  CdmCreatedCB cdm_created_cb = base::BindOnce(cdm_created_cb_);
  cdm_factory->pending_creations_.emplace(
      creation_id, std::make_pair(nullptr, std::move(cdm_created_cb)));
  cdm_factory->OnCdmCreated(creation_id, cdm, error_message);
  ASSERT_NE(cdm_factory, nullptr);
  EXPECT_TRUE(cdm_factory->pending_creations_.empty());
}
}  // namespace media
