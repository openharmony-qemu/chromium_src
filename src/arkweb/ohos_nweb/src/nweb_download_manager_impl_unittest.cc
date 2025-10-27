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
#include "ohos_nweb/src/nweb_download_manager_impl.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace OHOS::NWeb;
using namespace testing;

class MockNWebDOHConfig : public NWebDOHConfig {
 public:
  MOCK_METHOD(int, GetMode, (), (override));
  MOCK_METHOD(std::string, GetConfig, (), (override));
};

class NWebDownloadManagerImplTest : public testing::Test {
 protected:
  void SetUp() override {
    download_manager_ = std::make_shared<NWebDownloadManagerImpl>();
  }
  std::shared_ptr<NWebDownloadManagerImpl> download_manager_;
};

TEST_F(NWebDownloadManagerImplTest, SetHttpDns_AllBranchesCoverage) {
  std::shared_ptr<NWebDOHConfig> null_config = nullptr;
  EXPECT_NO_FATAL_FAILURE(download_manager_->SetHttpDns(null_config));

  auto mock_config = std::make_shared<MockNWebDOHConfig>();
  EXPECT_CALL(*mock_config, GetMode()).WillRepeatedly(Return(0));
  EXPECT_CALL(*mock_config, GetConfig()).WillRepeatedly(Return(""));

  EXPECT_NO_FATAL_FAILURE(download_manager_->SetHttpDns(mock_config));
}