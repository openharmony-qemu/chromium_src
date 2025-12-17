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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <fcntl.h>
#include <filemanagement/file_uri/oh_file_uri.h>
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_ndk_api.h"
#define private public
#include "datashare_adapter_impl.h"
#undef private

using namespace MockNdkApi;
using namespace OHOS::NWeb;

class DatashareAdapterImplTest : public ::testing::Test {
  protected:
   void SetAllMockType(bool type) {
      MockDatashareCommonEventSupport::bGetPath = type;
      MockDatashareCommonEventSupport::bGetFileNameMock = type;
   }
    void SetUp() override {
      adapter_ = std::make_unique<DatashareAdapterImpl>();
      SetAllMockType(true);
    }

    void TearDown() override {
      adapter_.reset();
      SetAllMockType(false);
    }

    std::unique_ptr<DatashareAdapterImpl> adapter_;
    const std::string validUri_ = "content://valid/uri";
};


TEST_F(DatashareAdapterImplTest, GetRealPath_Fail) {
  EXPECT_CALL(MockDatashareCommonEventSupport::GetInstance(),
              GetPathFromUriMock(testing::StrEq(validUri_.c_str()),
                                validUri_.length(), testing::_))
    .WillOnce(testing::Return(ERR_ENOMEM));

  std::string result = adapter_->GetRealPath(validUri_);
  EXPECT_TRUE(result.empty());
}

TEST_F(DatashareAdapterImplTest, GetRealPath_Success) {
  const std::string expectedPath = "/data/valid/path";

  EXPECT_CALL(MockDatashareCommonEventSupport::GetInstance(),
              GetPathFromUriMock(testing::StrEq(validUri_.c_str()),
                                validUri_.length(), testing::_))
    .WillOnce(testing::Invoke([&](const char*, unsigned int, char** out) {
      *out = strdup(expectedPath.c_str());
      return ERR_OK;
    }));

  std::string result = adapter_->GetRealPath(validUri_);
  EXPECT_EQ(result, expectedPath);
}

TEST_F(DatashareAdapterImplTest, GetFileDisplayName_Success) {
  const std::string expectedFileName = "valid_file.txt";

  EXPECT_CALL(MockDatashareCommonEventSupport::GetInstance(),
              GetFileNameMock(testing::StrEq(validUri_.c_str()),
                             testing::Eq(validUri_.length()), testing::_))
    .WillOnce(testing::Invoke([&](const char*, unsigned int, char** out) {
      *out = strdup(expectedFileName.c_str());
      return ERR_OK;
    }));

  std::string result = adapter_->GetFileDisplayName(validUri_);
  EXPECT_EQ(result, expectedFileName);
}

TEST_F(DatashareAdapterImplTest, GetFileDisplayName_Failure) {
  EXPECT_CALL(MockDatashareCommonEventSupport::GetInstance(),
              GetFileNameMock(testing::StrEq(validUri_.c_str()),
                             testing::Eq(validUri_.length()), testing::_))
    .WillOnce(testing::Return(ERR_ENOMEM));

  std::string result = adapter_->GetFileDisplayName(validUri_);
  EXPECT_TRUE(result.empty());
}

TEST_F(DatashareAdapterImplTest, OpenDataShareUriForRead_Failure_GetPath) {
  EXPECT_CALL(MockDatashareCommonEventSupport::GetInstance(),
              GetPathFromUriMock(testing::StrEq(validUri_.c_str()),
                                validUri_.length(), testing::_))
    .WillOnce(testing::Return(ERR_ENOMEM));

  int fd = adapter_->OpenDataShareUriForRead(validUri_);
  EXPECT_EQ(fd, -1);
}
