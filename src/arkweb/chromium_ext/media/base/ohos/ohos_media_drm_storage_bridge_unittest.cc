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
#include "base/test/test_simple_task_runner.h"
#define private public
#include "arkweb/chromium_ext/media/base/ohos/ohos_media_drm_storage_bridge.h"
#include "arkweb/chromium_ext/media/base/ohos/ohos_media_drm_bridge_factory.h"
#undef private
#include "arkweb/chromium_ext/media/base/ohos/ohos_media_drm_bridge.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

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
  MOCK_METHOD(base::WeakPtr<MediaDrmStorage>,
              AsWeakPtr,
              (),
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

class OHOSMediaDrmStorageBridgeTest : public ::testing::Test {
 protected:
  OHOSMediaDrmStorageBridgeTest() = default;
  ~OHOSMediaDrmStorageBridgeTest() override = default;

  void SetUp() override {
    create_fetcher_cb_ =
        base::BindRepeating([]() -> std::unique_ptr<ProvisionFetcher> {
          return std::make_unique<MockProvisionFetcher>();
        });
    create_storage_cb_ =
        base::BindRepeating([]() -> std::unique_ptr<MediaDrmStorage> {
          return std::make_unique<MockMediaDrmStorage>();
        });
    scoped_refptr<base::SingleThreadTaskRunner> task_runner(
        base::MakeRefCounted<base::TestSimpleTaskRunner>());
    base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd(task_runner);
    factory = std::make_unique<OHOSMediaDrmStorageBridge>();
    factory_ = std::make_unique<OHOSMediaDrmBridgeFactory>(create_fetcher_cb_,
                                                           create_storage_cb_);
  }

  void TearDown() override {
    factory.reset();
    factory_.reset();
    create_storage_cb_.Reset();
    create_fetcher_cb_.Reset();
  }
  CreateStorageCB create_storage_cb_;
  CreateFetcherCB create_fetcher_cb_;
  std::unique_ptr<OHOSMediaDrmStorageBridge> factory;
  std::unique_ptr<OHOSMediaDrmBridgeFactory> factory_;
};

TEST_F(OHOSMediaDrmStorageBridgeTest, Initialize) {
  bool init_cb_ = false;
  auto init_cb = base::BindLambdaForTesting(
      [&init_cb_](bool result) { init_cb_ = result; });
  factory_->create_storage_cb_ =
      base::BindRepeating([]() -> std::unique_ptr<MediaDrmStorage> {
        std::unique_ptr storage = std::make_unique<MockMediaDrmStorage>();
        EXPECT_CALL(*storage, Initialize(testing::_))
            .Times(testing::AtLeast(0));
        return storage;
      });
  factory->Initialize(factory_->create_storage_cb_, std::move(init_cb));
  ASSERT_NE(factory->impl_, nullptr);
}

TEST_F(OHOSMediaDrmStorageBridgeTest, OnProvisioned) {
  bool result_cb = false;
  auto result_cb_ = base::BindLambdaForTesting(
      [&result_cb](bool result) { result_cb = result; });
  factory->impl_ = std::make_unique<MockMediaDrmStorage>();
  factory->OnProvisioned(std::move(result_cb_));
  ASSERT_NE(factory->impl_, nullptr);
}

TEST_F(OHOSMediaDrmStorageBridgeTest, OnLoadInfo) {
  std::string session_id = "session_id";
  auto result_cb_ = base::BindOnce(
      [](const std::string& id, const std::vector<uint8_t>& key,
         const std::string& init_data_type, uint32_t session_id) {});
  factory->impl_ = std::make_unique<MockMediaDrmStorage>();
  factory->OnLoadInfo(session_id, std::move(result_cb_));
  ASSERT_NE(factory->impl_, nullptr);
}

TEST_F(OHOSMediaDrmStorageBridgeTest, OnSaveInfo) {
  std::string session_id = "session_id";
  int32_t key_type = 1;
  std::vector<uint8_t> key_set_id = {1};
  std::string mime = "mime";
  bool result_cb = false;
  auto result_cb_ = base::BindLambdaForTesting(
      [&result_cb](bool result) { result_cb = result; });
  factory->impl_ = std::make_unique<MockMediaDrmStorage>();
  factory->OnSaveInfo(key_set_id, mime, session_id, key_type, result_cb_);
  ASSERT_NE(factory->impl_, nullptr);
}

TEST_F(OHOSMediaDrmStorageBridgeTest, OnClearInfo) {
  std::string session_id = "session_id";
  bool result_cb = false;
  auto result_cb_ = base::BindLambdaForTesting(
      [&result_cb](bool result) { result_cb = result; });
  factory->impl_ = std::make_unique<MockMediaDrmStorage>();
  factory->OnClearInfo(session_id, result_cb_);
  ASSERT_NE(factory->impl_, nullptr);
}

TEST_F(OHOSMediaDrmStorageBridgeTest, OnInitialized) {
  bool success = true;
  auto init_cb =
      base::BindLambdaForTesting([&success](bool result) { success = result; });
  MediaDrmStorage::MediaDrmOriginId origin_id(base::UnguessableToken::Create());
  factory->OnInitialized(init_cb, success, origin_id);
  ASSERT_TRUE(init_cb);

  MediaDrmStorage::MediaDrmOriginId null_id = std::nullopt;
  factory->OnInitialized(init_cb, false, null_id);
  ASSERT_FALSE(success);

  factory->OnInitialized(init_cb, true, null_id);
  ASSERT_TRUE(success);
}

TEST_F(OHOSMediaDrmStorageBridgeTest, OnSessionDataLoaded) {
  std::vector<uint8_t> key_set_id = {1};
  std::string mime_type = "mime-type";
  MediaDrmKeyType key_type = MediaDrmKeyType::OFFLINE;
  std::unique_ptr<MediaDrmStorage::SessionData> session_data_ =
      std::make_unique<MediaDrmStorage::SessionData>(key_set_id, mime_type,
                                                     key_type);
  std::string session_id = "session-id";
  auto load_result_cb = base::BindOnce(
      [](const std::string& sid, const std::vector<uint8_t>& key_set_id,
         const std::string& mime_type, uint32_t key_type) {});
  factory->OnSessionDataLoaded(std::move(load_result_cb), session_id,
                               std::move(session_data_));
  ASSERT_EQ(session_data_.get(), nullptr);

  auto cb = base::BindOnce(
      [](const std::string& sid, const std::vector<uint8_t>& key_set_id,
         const std::string& mime_type, uint32_t key_type) {
        ASSERT_EQ(mime_type, "");
        ASSERT_EQ(key_type, 0);
      });
  factory->OnSessionDataLoaded(std::move(cb), session_id, nullptr);
}
}  // namespace media
