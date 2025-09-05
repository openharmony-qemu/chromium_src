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

#include <vector>
#include "components/cdm/common/widevine_drm_delegate_ohos.h"
#include "media/base/eme_constants.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cdm {

class WidevineDrmDelegateOHOSTest : public testing::Test {
 public:
  void SetUp() override {
    delegate_ = std::make_unique<WidevineDrmDelegateOHOS>();
  }

  void TearDown() override {
    delegate_.reset();
  }

  std::unique_ptr<WidevineDrmDelegateOHOS> delegate_;
};

TEST_F(WidevineDrmDelegateOHOSTest, OnCreateSessionNonCencDataType) {
  const std::vector<uint8_t> init_data = {1, 2};
  std::vector<uint8_t> init_data_out {};
  std::vector<std::string> param_out {};

  bool result = delegate_->OnCreateSession(
      media::EmeInitDataType::KEYIDS, init_data, &init_data_out,
      &param_out);
  EXPECT_TRUE(result);
}

TEST_F(WidevineDrmDelegateOHOSTest, OnCreateSessionCencDataType) {
  const std::vector<uint8_t> init_data = {1, 2};
  std::vector<uint8_t> init_data_out {};
  std::vector<std::string> param_out {};

  bool result = delegate_->OnCreateSession(
      media::EmeInitDataType::CENC, init_data, &init_data_out,
      &param_out);

  EXPECT_FALSE(result);
}
} // namespace cdm