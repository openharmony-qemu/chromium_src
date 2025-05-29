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

#include "base/base_paths_ohos.h"

#include "base/base_paths.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/numerics/checked_math.h"
#include "base/process/process_metrics.h"
#include "base/strings/escape.h"
#include "base/test/gtest_util.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/modp_b64/modp_b64.h"

namespace base {

bool ParseAssetsOHOS(FilePath* result);
bool PathProviderOHOS(int key, FilePath* result);

class CommandLineMock : public CommandLine {
 public:
  CommandLineMock(const FilePath& program) : CommandLine(program) {}
  MOCK_METHOD(std::string,
              GetSwitchValueASCII,
              (std::string_view switch_string),
              (const));
  MOCK_METHOD(bool, HasSwitch, (const char switch_constant[]), (const));
};

class BasePathsOhosTest : public testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();
};

void BasePathsOhosTest::SetUpTestCase(void) {}

void BasePathsOhosTest::TearDownTestCase(void) {}

void BasePathsOhosTest::SetUp(void) {}

void BasePathsOhosTest::TearDown(void) {}

TEST_F(BasePathsOhosTest, ParseAssetsOHOS) {
  base::FilePath filePath;
  bool result = ParseAssetsOHOS(&filePath);
  EXPECT_EQ(result, true);
}

TEST_F(BasePathsOhosTest, ParseAssetsOHOS2) {
  CommandLineMock* mock =
      new CommandLineMock(FilePath(FILE_PATH_LITERAL("Program")));
  testing::Mock::AllowLeak(mock);
  EXPECT_CALL(*mock, GetSwitchValueASCII(testing::_))
      .Times(1)
      .WillOnce(testing::Return("foo"));
  EXPECT_CALL(*mock, HasSwitch(testing::_))
      .Times(1)
      .WillOnce(testing::Return(false));
  base::FilePath filePath;
  bool result = ParseAssetsOHOS(&filePath);
  EXPECT_EQ(result, true);
}

TEST_F(BasePathsOhosTest, ParseAssetsOHOS3) {
  CommandLineMock* mock =
      new CommandLineMock(FilePath(FILE_PATH_LITERAL("Program")));
  testing::Mock::AllowLeak(mock);
  EXPECT_CALL(*mock, GetSwitchValueASCII(testing::_))
      .Times(1)
      .WillOnce(testing::Return("foo"));
  EXPECT_CALL(*mock, HasSwitch(testing::_))
      .Times(1)
      .WillOnce(testing::Return(true));
  base::FilePath filePath;
  bool result = ParseAssetsOHOS(&filePath);
  EXPECT_EQ(result, true);
}

TEST_F(BasePathsOhosTest, PathProviderOHOS) {
  base::FilePath filePath;
  bool result = PathProviderOHOS(base::FILE_EXE, &filePath);
  EXPECT_EQ(result, true);

  result = PathProviderOHOS(base::FILE_MODULE, &filePath);
  EXPECT_EQ(result, false);

  result = PathProviderOHOS(base::DIR_MODULE, &filePath);
  EXPECT_EQ(result, true);

  result = PathProviderOHOS(base::DIR_SOURCE_ROOT, &filePath);
  EXPECT_EQ(result, true);

  result = PathProviderOHOS(base::DIR_USER_DESKTOP, &filePath);
  EXPECT_EQ(result, false);

  result = PathProviderOHOS(base::DIR_CACHE, &filePath);
  EXPECT_EQ(result, true);

  result = PathProviderOHOS(base::DIR_ASSETS, &filePath);
  EXPECT_EQ(result, true);

  result = PathProviderOHOS(base::DIR_OHOS_APP_DATA, &filePath);
  EXPECT_EQ(result, true);

  result = PathProviderOHOS(base::DIR_OHOS_EXTERNAL_STORAGE, &filePath);
  EXPECT_EQ(result, false);

  result = PathProviderOHOS(-1, &filePath);
  EXPECT_EQ(result, false);
}
}  // namespace base
                    