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

#include "media/base/ohos/ohos_media_crypto_context_impl.h"
#include "base/test/test_simple_task_runner.h"
#include "media/base/cdm_context.h"
#include "media/base/ohos/ohos_media_crypto_context.h"
#define private public
#include "base/task/single_thread_task_runner.h"
#include "media/base/ohos/ohos_media_drm_bridge.h"
#undef private
#include "media/base/content_decryption_module.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
using ::testing::_;

namespace media {
class MockOHOSMediaCryptoContext : public OHOSMediaCryptoContext {
 public:
  MOCK_METHOD(void,
              SetOHOSMediaCryptoReadyCB,
              (OHOSMediaCryptoReadyCB media_crypto_ready_cb),
              (override));
};

class OHOSMediaCryptoContextImplTest : public ::testing::Test {
 protected:
  OHOSMediaCryptoContextImplTest() = default;
  ~OHOSMediaCryptoContextImplTest() override = default;

  void SetUp() override {
    media_crypto_ready_cb_ = std::make_unique<MockOHOSMediaCryptoContext>();
  }

  void TearDown() override {
    media_crypto_ready_cb_.reset();
    media_crypto_ready.reset();
    storage.reset();
  }
  std::unique_ptr<MockOHOSMediaCryptoContext> media_crypto_ready_cb_;
  std::unique_ptr<OHOSMediaCryptoContextImpl> media_crypto_ready;
  std::unique_ptr<OHOSMediaDrmStorageBridge> storage;
};

TEST_F(OHOSMediaCryptoContextImplTest, SetOHOSMediaCryptoReadyCB) {
  EXPECT_CALL(*media_crypto_ready_cb_, SetOHOSMediaCryptoReadyCB(_)).Times(0);
  MockOHOSMediaCryptoContext::OHOSMediaCryptoReadyCB callback =
      base::BindOnce([](void* session, bool requires_secure_video_codec) {});
  std::vector<uint8_t> scheme_uuid_ = {0x10, 0x31};
  std::string origin_id_ = "example_origin_id";
  OHOSMediaDrmBridge::SecurityLevel security_level_ =
      OHOSMediaDrmBridge::SECURITY_LEVEL_DEFAULT;
  bool requires_media_crypto_ = true;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner(
      base::MakeRefCounted<base::TestSimpleTaskRunner>());
  base::SingleThreadTaskRunner::CurrentDefaultHandle sttcd(task_runner);
  auto storage = std::make_unique<OHOSMediaDrmStorageBridge>();
  CreateFetcherCB create_fetcher_cb_;
  SessionMessageCB session_message_cb_;
  SessionClosedCB session_closed_cb_;
  SessionKeysChangeCB session_keys_change_cb_;
  SessionExpirationUpdateCB session_expiration_update_cb_;
  OHOSMediaDrmBridge* media_drm_bridge = new OHOSMediaDrmBridge(
      scheme_uuid_, origin_id_, security_level_, requires_media_crypto_,
      std::move(storage), create_fetcher_cb_, session_message_cb_,
      session_closed_cb_, session_keys_change_cb_,
      session_expiration_update_cb_);
  ASSERT_NE(media_drm_bridge, nullptr);
  media_crypto_ready =
      std::make_unique<OHOSMediaCryptoContextImpl>(media_drm_bridge);
  media_crypto_ready->SetOHOSMediaCryptoReadyCB(std::move(callback));
  ASSERT_NE(media_crypto_ready, nullptr);
}
}  // namespace media
