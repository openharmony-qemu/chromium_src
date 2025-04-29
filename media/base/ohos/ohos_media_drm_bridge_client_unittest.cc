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

#include "media/base/ohos/ohos_media_drm_bridge_client.h"
#include "media/base/ohos/ohos_media_drm_bridge_delegate.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace media {
class SetMediaDrmBridgeClientTest : public ::testing::Test {
 protected:
  SetMediaDrmBridgeClientTest() = default;
  ~SetMediaDrmBridgeClientTest() override = default;

  void SetUp() override {
    media_drm_bridge_client_ = std::make_unique<OHOSMediaDrmBridgeClient>();
  }

  void TearDown() override { media_drm_bridge_client_.reset(); }
  std::unique_ptr<OHOSMediaDrmBridgeClient> media_drm_bridge_client_;
};

TEST_F(SetMediaDrmBridgeClientTest, SetMediaDrmBridgeClient) {
  SetMediaDrmBridgeClient(media_drm_bridge_client_.get());
  EXPECT_NE(GetMediaDrmBridgeClient(), nullptr);
}

TEST_F(SetMediaDrmBridgeClientTest, GetMediaDrmBridgeDelegate) {
  std::vector<uint8_t> scheme_uuid;
  UUID test_scheme_uuid(scheme_uuid);
  OHOSMediaDrmBridgeDelegate* result =
      media_drm_bridge_client_->GetMediaDrmBridgeDelegate(test_scheme_uuid);
  EXPECT_EQ(result, nullptr);
}

TEST_F(SetMediaDrmBridgeClientTest, AddKeySystemUUIDMappings) {
  OHOSMediaDrmBridgeClient::KeySystemUuidMap key_system_uuid_map;
  media_drm_bridge_client_->AddKeySystemUUIDMappings(&key_system_uuid_map);
  EXPECT_TRUE(key_system_uuid_map.empty());
}
}  // namespace media
