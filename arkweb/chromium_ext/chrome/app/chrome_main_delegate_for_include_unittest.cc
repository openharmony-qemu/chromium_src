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
  // Initialize command line arguments
  base::CommandLine::Init(0, nullptr);
  base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();
  // Turn off the switch
  cmd_line->RemoveSwitch(switches::kCacheDirExists);
  cmd_line->RemoveSwitch(switches::kUserDataDirExists);
  cmd_line->RemoveSwitch(switches::kArkwebAppDataDir);
  cmd_line->RemoveSwitch(switches::kUserDataDir);
  // Reset user data directory
  base::PathService::Override(chrome::DIR_USER_DATA, base::FilePath());
}
  void TearDown() override {
    if (base::PathExists(base::FilePath("/data/storage/el2"))) {
      base::DeletePathRecursively(base::FilePath("/data/storage/el2"));
    }
  }
};

TEST_F(SetUserDataDirForArkwebTest, TestCase001) {
  // 1. user_data_dir is empty
  //    cache/web not exist
  //    files/__arkweb not exist
  base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();

  base::FilePath arkweb_app_base_dir = base::FilePath("/data/storage/el2/base");
  base::FilePath user_data_dir = base::FilePath();
  cmd_line->AppendSwitchPath(switches::kArkwebAppDataDir, arkweb_app_base_dir);
  cmd_line->AppendSwitchPath(switches::kUserDataDir, user_data_dir);

  bool result = SetUserDataDirForArkweb(cmd_line);
  EXPECT_FALSE(result);

  base::FilePath target_user_data_dir;
  base::PathService::Get(chrome::DIR_USER_DATA, &target_user_data_dir);

  EXPECT_EQ(target_user_data_dir,
                base::FilePath("/data/storage/el2/base/files/__arkweb"));
}

TEST_F(SetUserDataDirForArkwebTest, TestCase002) {
  // 2. user_data_dir is empty
  //    cache/web exist
  //    files/__arkweb exist
  base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();

  base::FilePath arkweb_app_base_dir = base::FilePath("/data/storage/el2/base");
  base::FilePath user_data_dir = base::FilePath();
  cmd_line->AppendSwitch(switches::kCacheDirExists);
  cmd_line->AppendSwitch(switches::kUserDataDirExists);
  cmd_line->AppendSwitchPath(switches::kArkwebAppDataDir, arkweb_app_base_dir);
  cmd_line->AppendSwitchPath(switches::kUserDataDir, user_data_dir);

  bool result = SetUserDataDirForArkweb(cmd_line);
  EXPECT_FALSE(result);

  base::FilePath target_user_data_dir;
  base::PathService::Get(chrome::DIR_USER_DATA, &target_user_data_dir);

  EXPECT_EQ(target_user_data_dir,
                base::FilePath("/data/storage/el2/base/files/__arkweb"));
}

TEST_F(SetUserDataDirForArkwebTest, TestCase003) {
  // 3. user_data_dir is empty
  //    cache/web exist
  //    files/__arkweb not exist
  base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();

  base::FilePath arkweb_app_base_dir = base::FilePath("/data/storage/el2/base");
  base::FilePath user_data_dir = base::FilePath();
  cmd_line->AppendSwitch(switches::kCacheDirExists);
  cmd_line->AppendSwitchPath(switches::kArkwebAppDataDir, arkweb_app_base_dir);
  cmd_line->AppendSwitchPath(switches::kUserDataDir, user_data_dir);

  bool result = SetUserDataDirForArkweb(cmd_line);
  EXPECT_FALSE(result);

  base::FilePath target_user_data_dir;
  base::PathService::Get(chrome::DIR_USER_DATA, &target_user_data_dir);

  EXPECT_EQ(target_user_data_dir,
                base::FilePath("/data/storage/el2/base/cache/web"));
}

TEST_F(SetUserDataDirForArkwebTest, TestCase004) {
  // 4. user_data_dir invalid
  //    cache/web not exists
  base::FilePath test_dir = base::FilePath("/data/storage/el2/base");
  base::FilePath test_file = test_dir.Append("tmp_test");
  base::CreateDirectory(test_dir);
  base::WriteFile(test_file, "test");

  base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();
  base::FilePath arkweb_app_base_dir = base::FilePath("/data/storage/el2/base");
  base::FilePath user_data_dir = base::FilePath("tmp_test");
  cmd_line->AppendSwitchPath(switches::kArkwebAppDataDir, arkweb_app_base_dir);
  cmd_line->AppendSwitchPath(switches::kUserDataDir, user_data_dir);

  bool result = SetUserDataDirForArkweb(cmd_line);
  EXPECT_TRUE(result);

  base::FilePath target_user_data_dir;
  base::PathService::Get(chrome::DIR_USER_DATA, &target_user_data_dir);
  if (result) {
    target_user_data_dir.clear();
  }

  EXPECT_TRUE(target_user_data_dir.empty());
}

TEST_F(SetUserDataDirForArkwebTest, TestCase005) {
  // 5. user_data_dir invalid
  //    cache/web exists
  //    user_data_dir exists
  base::FilePath test_dir = base::FilePath("/data/storage/el2/base");
  base::FilePath test_file = test_dir.Append("tmp_test");
  base::CreateDirectory(test_dir);
  base::WriteFile(test_file, "test");

  base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();
  base::FilePath arkweb_app_base_dir = base::FilePath("/data/storage/el2/base");
  base::FilePath user_data_dir = base::FilePath("tmp_test");
  cmd_line->AppendSwitch(switches::kCacheDirExists);
  cmd_line->AppendSwitch(switches::kUserDataDirExists);
  cmd_line->AppendSwitchPath(switches::kArkwebAppDataDir, arkweb_app_base_dir);
  cmd_line->AppendSwitchPath(switches::kUserDataDir, user_data_dir);

  bool result = SetUserDataDirForArkweb(cmd_line);
  EXPECT_TRUE(result);

  base::FilePath target_user_data_dir;
  base::PathService::Get(chrome::DIR_USER_DATA, &target_user_data_dir);
  if (result) {
    target_user_data_dir.clear();
  }

  EXPECT_TRUE(target_user_data_dir.empty());
}

TEST_F(SetUserDataDirForArkwebTest, TestCase006) {
  // 6. user_data_dir invalid
  //    cache/web exists
  //    user_data_dir not exists
  base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();

  base::FilePath arkweb_app_base_dir = base::FilePath("/data/storage/el2/base");
  base::FilePath user_data_dir = base::FilePath("tmp_test");
  cmd_line->AppendSwitch(switches::kCacheDirExists);
  cmd_line->AppendSwitchPath(switches::kArkwebAppDataDir, arkweb_app_base_dir);
  cmd_line->AppendSwitchPath(switches::kUserDataDir, user_data_dir);

  bool result = SetUserDataDirForArkweb(cmd_line);
  EXPECT_FALSE(result);

  base::FilePath target_user_data_dir;
  base::PathService::Get(chrome::DIR_USER_DATA, &target_user_data_dir);

  EXPECT_EQ(target_user_data_dir,
                base::FilePath("/data/storage/el2/base/cache/web"));
}

TEST_F(SetUserDataDirForArkwebTest, TestCase007) {
  // 7. user_data_dir valid
  //    cache/web not exists
  base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();

  base::FilePath arkweb_app_base_dir = base::FilePath("/data/storage/el2/base");
  base::FilePath user_data_dir = base::FilePath("files/__arkweb2");
  cmd_line->AppendSwitchPath(switches::kArkwebAppDataDir, arkweb_app_base_dir);
  cmd_line->AppendSwitchPath(switches::kUserDataDir, user_data_dir);

  bool result = SetUserDataDirForArkweb(cmd_line);
  EXPECT_FALSE(result);

  base::FilePath target_user_data_dir;
  base::PathService::Get(chrome::DIR_USER_DATA, &target_user_data_dir);

  EXPECT_EQ(target_user_data_dir,
                base::FilePath("/data/storage/el2/base/files/__arkweb2"));
}

TEST_F(SetUserDataDirForArkwebTest, TestCase008) {
  // 8. user_data_dir valid
  //    cache/web exists
  //    user_data_dir exists
  base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();

  base::FilePath arkweb_app_base_dir = base::FilePath("/data/storage/el2/base");
  base::FilePath user_data_dir = base::FilePath("files/__arkweb2");
  cmd_line->AppendSwitch(switches::kCacheDirExists);
  cmd_line->AppendSwitch(switches::kUserDataDirExists);
  cmd_line->AppendSwitchPath(switches::kArkwebAppDataDir, arkweb_app_base_dir);
  cmd_line->AppendSwitchPath(switches::kUserDataDir, user_data_dir);

  bool result = SetUserDataDirForArkweb(cmd_line);
  EXPECT_FALSE(result);

  base::FilePath target_user_data_dir;
  base::PathService::Get(chrome::DIR_USER_DATA, &target_user_data_dir);

  EXPECT_EQ(target_user_data_dir,
                base::FilePath("/data/storage/el2/base/files/__arkweb2"));
}

TEST_F(SetUserDataDirForArkwebTest, TestCase009) {
  // 9. user_data_dir valid
  //    cache/web exists
  //    user_data_dir not exists
  base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();

  base::FilePath arkweb_app_base_dir = base::FilePath("/data/storage/el2/base");
  base::FilePath user_data_dir = base::FilePath("files/__arkweb2");
  cmd_line->AppendSwitch(switches::kCacheDirExists);
  cmd_line->AppendSwitchPath(switches::kArkwebAppDataDir, arkweb_app_base_dir);
  cmd_line->AppendSwitchPath(switches::kUserDataDir, user_data_dir);

  bool result = SetUserDataDirForArkweb(cmd_line);
  EXPECT_FALSE(result);

  base::FilePath target_user_data_dir;
  base::PathService::Get(chrome::DIR_USER_DATA, &target_user_data_dir);

  EXPECT_EQ(target_user_data_dir,
                base::FilePath("/data/storage/el2/base/cache/web"));
}