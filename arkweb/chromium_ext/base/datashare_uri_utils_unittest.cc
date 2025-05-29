/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "base/datashare_uri_utils.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "base/files/file.h"
#include "base/strings/utf_string_conversions.h"
#include "ohos_adapter_helper.h"

namespace base {
namespace testing {

class DatashareUriUtilsTest : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(DatashareUriUtilsTest, OpenDatashareUriForRead001) {
  FilePath invalid_uri("invalid_datashare_uri");
  File file = OpenDatashareUriForRead(invalid_uri);
  EXPECT_FALSE(file.IsValid());
}

TEST_F(DatashareUriUtilsTest, GetFileDisplayName001) {
  FilePath uri("valid_uri");
  std::u16string result = GetFileDisplayName(uri);
  EXPECT_FALSE(result.empty());
}

TEST_F(DatashareUriUtilsTest, GetRealPath001) {
  FilePath uri("invalid_datashare_uri");
  std::string result = GetRealPath(uri);
  EXPECT_FALSE(result.empty());
}

}  // namespace testing
}  // namespace base
