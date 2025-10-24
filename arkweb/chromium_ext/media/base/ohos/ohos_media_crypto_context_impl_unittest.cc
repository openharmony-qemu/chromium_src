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

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#include "base/test/test_simple_task_runner.h"
#include "base/test/task_environment.h"
#include "base/task/single_thread_task_runner.h"
#include "media/base/cdm_context.h"
#include "arkweb/chromium_ext/media/base/ohos/ohos_media_crypto_context.h"
#define private public
#include "arkweb/chromium_ext/media/base/ohos/ohos_media_drm_bridge.h"
#include "arkweb/chromium_ext/media/base/ohos/ohos_media_crypto_context_impl.h"
#undef private
#include "media/base/content_decryption_module.h"
#if BUILDFLAG(ARKWEB_ENABLE_WISEPLAY)
#include "media/cdm/wiseplay_cdm_common.h"
#endif

using ::testing::_;

namespace media {

namespace {
  CreateFetcherCB create_fetcher_cb;
  SessionMessageCB session_message_cb;
  SessionClosedCB session_closed_cb;
  SessionKeysChangeCB session_keys_change_cb;
  SessionExpirationUpdateCB session_expiration_update_cb;
  const uint8_t kWidevineUuid[16] = {0xED, 0xEF, 0x8B, 0xA9, 0x79, 0xD6,
                                    0x4A, 0xCE, 0xA3, 0xC8, 0x27, 0xDC,
                                    0xD5, 0x1D, 0x21, 0xED};
}

class MockOHOSMediaCryptoContext : public OHOSMediaCryptoContext {
 public:
  MOCK_METHOD(void,
              SetOHOSMediaCryptoReadyCB,
              (OHOSMediaCryptoReadyCB media_crypto_ready_cb),
              (override));
#if BUILDFLAG(ARKWEB_ENABLE_WISEPLAY)
  MOCK_METHOD(void,
              ReleaseInnerResource,
              (),
              (override));
#endif
  MOCK_METHOD(std::vector<uint8_t>,
              GetUUID,
              (),
              (override));
};

class OHOSMediaCryptoContextImplTest : public ::testing::Test {
 protected:
  OHOSMediaCryptoContextImplTest() = default;
  ~OHOSMediaCryptoContextImplTest() override = default;

  void SetUp() override {
    task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  }

  void TearDown() override {
    if (bridge_) {
      bridge_.reset();
    }
  }

  void CreateOHOSMediaDrmBridge(std::string system = "com.wiseplay.drm") {
    std::vector<uint8_t> scheme_uuid;
    if (system == "com.wiseplay.drm") {
      scheme_uuid = std::vector<uint8_t>(kWiseplayUuid, kWiseplayUuid + sizeof(kWiseplayUuid));
    } else if (system == "com.widevine.alpha") {
      scheme_uuid = std::vector<uint8_t>(kWidevineUuid, kWidevineUuid + sizeof(kWidevineUuid));
    } else {
      return;
    }
    std::string origin_id = "example_origin_id";
    OHOSMediaDrmBridge::SecurityLevel level = OHOSMediaDrmBridge::SECURITY_LEVEL_DEFAULT;
    bool require_media_crypto = true;
    bridge_ = std::make_unique<OHOSMediaDrmBridge>(
        scheme_uuid, origin_id, level, require_media_crypto, nullptr,
        create_fetcher_cb, session_message_cb, session_closed_cb,
        session_keys_change_cb, session_expiration_update_cb);
  }

  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::test::TaskEnvironment task_environment_;
  std::unique_ptr<OHOSMediaCryptoContextImpl> media_crypto_context_impl_;
  std::unique_ptr<OHOSMediaDrmBridge> bridge_;
};

TEST_F(OHOSMediaCryptoContextImplTest, SetOHOSMediaCryptoReadyCB_BridgeNotNull_widevine) {
  CreateOHOSMediaDrmBridge("com.widevine.alpha");
  MockOHOSMediaCryptoContext::OHOSMediaCryptoReadyCB callback =
      base::BindOnce([](void* session, bool requires_secure_video_codec) {});

  media_crypto_context_impl_ = std::make_unique<OHOSMediaCryptoContextImpl>(bridge_.get());
  EXPECT_NO_FATAL_FAILURE(media_crypto_context_impl_->SetOHOSMediaCryptoReadyCB(std::move(callback)));
  EXPECT_NE(media_crypto_context_impl_->media_drm_bridge_, nullptr);
}

TEST_F(OHOSMediaCryptoContextImplTest, SetOHOSMediaCryptoReadyCB_BridgeIsNull) {
  CreateOHOSMediaDrmBridge();
  MockOHOSMediaCryptoContext::OHOSMediaCryptoReadyCB callback =
      base::BindOnce([](void* session, bool requires_secure_video_codec) {});

  media_crypto_context_impl_ = std::make_unique<OHOSMediaCryptoContextImpl>(nullptr);
  EXPECT_NO_FATAL_FAILURE(media_crypto_context_impl_->SetOHOSMediaCryptoReadyCB(std::move(callback)));
  EXPECT_EQ(media_crypto_context_impl_->media_drm_bridge_, nullptr);
}

#if BUILDFLAG(ARKWEB_ENABLE_WISEPLAY)
TEST_F(OHOSMediaCryptoContextImplTest, SetOHOSMediaCryptoReadyCB_BridgeNotNull_wiseplay) {
  CreateOHOSMediaDrmBridge();
  MockOHOSMediaCryptoContext::OHOSMediaCryptoReadyCB callback =
      base::BindOnce([](void* session, bool requires_secure_video_codec) {});

  media_crypto_context_impl_ = std::make_unique<OHOSMediaCryptoContextImpl>(bridge_.get());
  EXPECT_NO_FATAL_FAILURE(media_crypto_context_impl_->SetOHOSMediaCryptoReadyCB(std::move(callback)));
  EXPECT_NE(media_crypto_context_impl_->media_drm_bridge_, nullptr);
}

TEST_F(OHOSMediaCryptoContextImplTest, ReleaseInnerResource_BridgeIsNull) {
  CreateOHOSMediaDrmBridge();
  media_crypto_context_impl_ = std::make_unique<OHOSMediaCryptoContextImpl>(nullptr);
  EXPECT_NO_FATAL_FAILURE(media_crypto_context_impl_->ReleaseInnerResource());
  EXPECT_EQ(media_crypto_context_impl_->media_drm_bridge_, nullptr);
}

TEST_F(OHOSMediaCryptoContextImplTest, ReleaseInnerResource_BridgeNotNull) {
  CreateOHOSMediaDrmBridge();
  media_crypto_context_impl_ = std::make_unique<OHOSMediaCryptoContextImpl>(bridge_.get());
  EXPECT_NO_FATAL_FAILURE(media_crypto_context_impl_->ReleaseInnerResource());
  EXPECT_NE(media_crypto_context_impl_->media_drm_bridge_, nullptr);
}
#endif

}  // namespace media
