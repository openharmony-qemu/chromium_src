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

#include "media/base/ohos/ohos_media_drm_bridge_delegate.h"
#include "media/base/ohos/ohos_media_drm_bridge_client.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace media {
class MockOHOSMediaDrmBridgeDelegate : public OHOSMediaDrmBridgeDelegate {
 public:
  MOCK_METHOD(const UUID, GetUUID, (), (const override));
};

class OHOSMediaDrmBridgeDelegateTest : public ::testing::Test {
 protected:
  OHOSMediaDrmBridgeDelegateTest() = default;
  ~OHOSMediaDrmBridgeDelegateTest() override = default;
  void SetUp() override {
    mock_delegate_ = std::make_unique<MockOHOSMediaDrmBridgeDelegate>();
  }

  void TearDown() override {
    mock_delegate_.reset();
    delegate_.reset();
  }
  std::unique_ptr<OHOSMediaDrmBridgeDelegate> delegate_;
  std::unique_ptr<MockOHOSMediaDrmBridgeDelegate> mock_delegate_;
};

TEST_F(OHOSMediaDrmBridgeDelegateTest, OnCreateSession) {
  delegate_ = std::move(mock_delegate_);
  EmeInitDataType init_data_type = EmeInitDataType::WEBM;
  std::vector<uint8_t> init_data = {0x01, 0x02, 0x03};
  std::vector<uint8_t> init_data_out;
  std::vector<std::string> optional_parameters_out;
  bool result = false;
  result = delegate_->OnCreateSession(init_data_type, init_data, &init_data_out,
                                      &optional_parameters_out);
  EXPECT_TRUE(result);
}
}  // namespace media
