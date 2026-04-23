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
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/base_export.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/chrome_switches.h"
#include "gtest/gtest.h"
#include "arkweb/chromium_ext/chrome/app/chrome_main_delegate_for_include.cc"

class SetUserDataDirForArkwebTest : public testing::Test {
protected:
  void SetUp() override {
    // Reset user data directory
    base::PathService::Override(chrome::DIR_USER_DATA, base::FilePath("/"));
  }

  void TearDown() override {
    if (base::PathExists(base::FilePath("/data/storage/el2"))) {
      base::DeletePathRecursively(base::FilePath("/data/storage/el2"));
    }
  }
};

TEST_F(SetUserDataDirForArkwebTest, TestCase001) {
  base::FilePath user_data_dir = base::FilePath("/data/storage/el2/base/files/__arkweb");
  bool result = SetUserDataDirForArkweb(user_data_dir);
  EXPECT_FALSE(result);

  base::FilePath target_user_data_dir;
  base::PathService::Get(chrome::DIR_USER_DATA, &target_user_data_dir);

  EXPECT_EQ(target_user_data_dir,
                base::FilePath("/data/storage/el2/base/files/__arkweb"));
}

TEST_F(SetUserDataDirForArkwebTest, TestCase002) {
  base::FilePath user_data_dir = base::FilePath("/data/storage/el2/base/files/__arkweb2");
  bool result = SetUserDataDirForArkweb(user_data_dir);
  EXPECT_FALSE(result);

  base::FilePath target_user_data_dir;
  base::PathService::Get(chrome::DIR_USER_DATA, &target_user_data_dir);

  EXPECT_EQ(target_user_data_dir,
                base::FilePath("/data/storage/el2/base/files/__arkweb2"));
}

TEST_F(SetUserDataDirForArkwebTest, TestCase003) {
  base::FilePath test_dir = base::FilePath("/data/storage/el2/base");
  base::FilePath test_file = test_dir.Append("tmp_test");
  base::CreateDirectory(test_dir);
  base::WriteFile(test_file, "test");

  base::FilePath user_data_dir = base::FilePath("/data/storage/el2/base/tmp_test");
  bool result = SetUserDataDirForArkweb(user_data_dir);
  EXPECT_TRUE(result);

  base::FilePath target_user_data_dir;
  base::PathService::Get(chrome::DIR_USER_DATA, &target_user_data_dir);

  EXPECT_EQ(target_user_data_dir, base::FilePath("/"));
}

TEST_F(SetUserDataDirForArkwebTest, TestCase004) {
  base::FilePath user_data_dir = base::FilePath("");
  bool result = SetUserDataDirForArkweb(user_data_dir);
  EXPECT_TRUE(result);

  base::FilePath target_user_data_dir;
  base::PathService::Get(chrome::DIR_USER_DATA, &target_user_data_dir);
  EXPECT_EQ(target_user_data_dir, base::FilePath("/"));
}