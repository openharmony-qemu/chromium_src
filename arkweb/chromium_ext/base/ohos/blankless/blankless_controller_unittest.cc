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

#include <string>
#include <string_view>
#define private public
#include "base/ohos/blankless/blankless_controller.h"
#undef private
#include "base/time/time.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "base/time/time_utils.h"
#include "base/files/file.h"
#include "base/files/file_util.cc"
#include "base/values.h"
#include "base/json/json_writer.h"

namespace base {
namespace ohos {
class BlanklessControllerTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    controller.Clear(0);
  }
  void TearDown() override {}

  BlanklessController& controller = BlanklessController::GetInstance();

  uint64_t blankless_key1 = 1024;
  uint64_t blankless_key2 = 1025;
  uint64_t blankless_key3 = 1026;

  int32_t nweb_id1 = 1;
};

#if BUILDFLAG(ARKWEB_TEST)
TEST_F(BlanklessControllerTest, LoadSysWhiteList001) {
  base::FilePath file_path = base::FilePath("/data/ut/blank_opt_white_list.json");
  ASSERT_TRUE(base::CreateDirectory(file_path.DirName()));

  base::File file(file_path, base::File::FLAG_CREATE | base::File::FLAG_WRITE);
  ASSERT_TRUE(file.IsValid());

  controller.m_white_list_.m_is_sys_loaded_ = false;
  controller.m_white_list_.LoadSysWhiteList();
  EXPECT_TRUE(controller.m_white_list_.m_is_sys_loaded_);
  base::DeleteFile(file_path);
}

TEST_F(BlanklessControllerTest, LoadSysWhiteList002) {
  base::FilePath file_path = base::FilePath("/data/ut/blank_opt_white_list.json");
  ASSERT_TRUE(base::CreateDirectory(file_path.DirName()));

  base::File file(file_path, base::File::FLAG_CREATE | base::File::FLAG_WRITE);
  ASSERT_TRUE(file.IsValid());

  std::string_view data = "text";
  base::WriteFile(file_path, data);

  controller.m_white_list_.m_is_sys_loaded_ = false;
  controller.m_white_list_.LoadSysWhiteList();
  EXPECT_TRUE(controller.m_white_list_.m_is_sys_loaded_);
  base::DeleteFile(file_path);
}

TEST_F(BlanklessControllerTest, LoadSysWhiteList003) {
  base::FilePath file_path = base::FilePath("/data/ut/blank_opt_white_list.json");
  ASSERT_TRUE(base::CreateDirectory(file_path.DirName()));

  base::File file(file_path, base::File::FLAG_CREATE | base::File::FLAG_WRITE);
  ASSERT_TRUE(file.IsValid());

  std::string_view data = R"json({
    "names": [
      {
        "key": "value",
      },
    ],
  })json";
  base::WriteFile(file_path, data);

  controller.m_white_list_.m_is_sys_loaded_ = false;
  controller.m_white_list_.LoadSysWhiteList();
  EXPECT_TRUE(controller.m_white_list_.m_is_sys_loaded_);
  base::DeleteFile(file_path);
}

TEST_F(BlanklessControllerTest, LoadSysWhiteList004) {
  base::FilePath file_path = base::FilePath("/data/ut/blank_opt_white_list.json");
  ASSERT_TRUE(base::CreateDirectory(file_path.DirName()));

  base::File file(file_path, base::File::FLAG_CREATE | base::File::FLAG_WRITE);
  ASSERT_TRUE(file.IsValid());

  std::string_view data = R"json({
    "exact-match": {
      "enabled": true,
      "patterns": ["example.com", "test.org"]
    },
  })json";
  base::WriteFile(file_path, data);

  controller.m_white_list_.m_is_sys_loaded_ = false;
  controller.m_white_list_.LoadSysWhiteList();
  EXPECT_TRUE(controller.m_white_list_.m_is_sys_loaded_);
  base::DeleteFile(file_path);
}

TEST_F(BlanklessControllerTest, LoadSysWhiteList005) {
  base::FilePath file_path = base::FilePath("/data/ut/blank_opt_white_list.json");
  ASSERT_TRUE(base::CreateDirectory(file_path.DirName()));

  base::File file(file_path, base::File::FLAG_CREATE | base::File::FLAG_WRITE);
  ASSERT_TRUE(file.IsValid());

  std::string_view data = R"json({
    "exact-match": {
      "enabled": true,
      "patterns": ["example.com", "test.org"]
    },
    "fuzzy-match": {
      "threshold": 0.85,
      "domains": ["*.example.com", "*.test.org"]
    }
  })json";
  base::WriteFile(file_path, data);

  controller.m_white_list_.m_is_sys_loaded_ = false;
  controller.m_white_list_.LoadSysWhiteList();
  EXPECT_TRUE(controller.m_white_list_.m_is_sys_loaded_);
  base::DeleteFile(file_path);
}

TEST_F(BlanklessControllerTest, LoadSysWhiteList006) {
  base::FilePath file_path = base::FilePath("/data/ut/blank_opt_white_list.json");
  ASSERT_TRUE(base::CreateDirectory(file_path.DirName()));

  base::File file(file_path, base::File::FLAG_CREATE | base::File::FLAG_WRITE);
  ASSERT_TRUE(file.IsValid());

  base::Value::Dict outer_dict;
  base::Value::List inner_list;
  inner_list.Append("item1");
  inner_list.Append("item2");
  inner_list.Append("item3");
  outer_dict.Set("exact-match", std::move(inner_list));
  outer_dict.Set("fuzzy-match", "file");
  std::string json_str;
  base::JSONWriter::Write(outer_dict, &json_str);
  base::WriteFile(file_path, json_str);

  controller.m_white_list_.m_is_sys_loaded_ = false;
  controller.m_white_list_.LoadSysWhiteList();
  EXPECT_TRUE(controller.m_white_list_.m_is_sys_loaded_);
  base::DeleteFile(file_path);
}

TEST_F(BlanklessControllerTest, LoadSysWhiteList007) {
  base::FilePath file_path = base::FilePath("/data/ut/blank_opt_white_list.json");
  ASSERT_TRUE(base::CreateDirectory(file_path.DirName()));

  base::File file(file_path, base::File::FLAG_CREATE | base::File::FLAG_WRITE);
  ASSERT_TRUE(file.IsValid());

  base::Value::Dict outer_dict;
  base::Value::List inner_list;
  outer_dict.Set("exact-match", std::move(inner_list));
  outer_dict.Set("fuzzy-match", std::move(inner_list));
  std::string json_str;
  base::JSONWriter::Write(outer_dict, &json_str);
  base::WriteFile(file_path, json_str);

  controller.m_white_list_.m_is_sys_loaded_ = false;
  controller.m_white_list_.LoadSysWhiteList();
  EXPECT_TRUE(controller.m_white_list_.m_is_sys_loaded_);
  base::DeleteFile(file_path);
}

TEST_F(BlanklessControllerTest, LoadSysWhiteList008) {
  base::FilePath file_path = base::FilePath("/data/ut/blank_opt_white_list.json");
  ASSERT_TRUE(base::CreateDirectory(file_path.DirName()));

  base::File file(file_path, base::File::FLAG_CREATE | base::File::FLAG_WRITE);
  ASSERT_TRUE(file.IsValid());

  base::Value::Dict outer_dict;
  base::Value::List inner_list_exact;
  inner_list_exact.Append("item1");
  inner_list_exact.Append("item2");
  inner_list_exact.Append("item3");
  base::Value::List inner_list_fuzzy;
  inner_list_fuzzy.Append("item1");
  inner_list_fuzzy.Append("item2");
  inner_list_fuzzy.Append("item3");
  outer_dict.Set("exact-match", std::move(inner_list_exact));
  outer_dict.Set("fuzzy-match", std::move(inner_list_fuzzy));
  std::string json_str;
  base::JSONWriter::Write(outer_dict, &json_str);
  base::WriteFile(file_path, json_str);

  controller.m_white_list_.m_is_sys_loaded_ = false;
  controller.m_white_list_.LoadSysWhiteList();
  EXPECT_TRUE(controller.m_white_list_.m_is_sys_loaded_);
  base::DeleteFile(file_path);
}
#endif

TEST_F(BlanklessControllerTest, CheckSysWhiteList01) {
  std::string url = "abc";
  uint64_t key = 0;
  EXPECT_FALSE(controller.m_white_list_.CheckSysWhiteList(url, key));
}

TEST_F(BlanklessControllerTest, CheckSysWhiteList02) {
  std::string url = "abc";
  controller.m_white_list_.m_is_sys_loaded_ = true;
  uint64_t key = 0;
  EXPECT_FALSE(controller.m_white_list_.CheckSysWhiteList(url, key));
}

TEST_F(BlanklessControllerTest, FrameRemoveCallback)
{
  int count = 0;
  controller.RegisterFrameRemoveCallback(nweb_id1, blankless_key1, [&count](){count++;});
  controller.FireFrameRemoveCallback(nweb_id1, blankless_key1);
  EXPECT_EQ(count, 1);

  controller.FireFrameRemoveCallback(nweb_id1, blankless_key1);
  EXPECT_EQ(count, 1);
}

TEST_F(BlanklessControllerTest, FrameInsertCallback)
{
  int32_t lcp = 300;
  int count = 0;
  controller.RegisterFrameInsertCallback(nweb_id1, blankless_key1, [&count](){count++;}, lcp);
  EXPECT_EQ(controller.FireFrameInsertCallback(nweb_id1, blankless_key1), lcp);
  EXPECT_EQ(count, 1);

  controller.RegisterFrameInsertCallback(nweb_id1, blankless_key1, [&count](){count++;}, lcp);
  controller.CancelFrameInsertCallback(nweb_id1, blankless_key1);
  EXPECT_EQ(controller.FireFrameInsertCallback(nweb_id1, blankless_key1), 0);
  EXPECT_EQ(count, 1);
}

TEST_F(BlanklessControllerTest, Status_Allowed_NotAllowed)
{
  controller.ResetStatus(nweb_id1, true, false);
  auto status = controller.ResetStatus(nweb_id1, false, false);
  EXPECT_EQ(status, BlanklessController::StatusCode::NOT_ALLOWED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::NOT_ALLOWED,
    .allowed = false,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_Allowed_RecordKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  auto status = controller.RecordKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::DUMPED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::DUMPED,
    .blankless_key = blankless_key1,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_Allowed_MatchKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  auto status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::KEY_NOT_MATCH,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_NotAllowed_Allowed)
{
  controller.ResetStatus(nweb_id1, false, false);
  auto status = controller.ResetStatus(nweb_id1, true, false);
  EXPECT_EQ(status, BlanklessController::StatusCode::ALLOWED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::ALLOWED,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_NotAllowed_RecordKey)
{
  controller.ResetStatus(nweb_id1, false, false);
  auto status = controller.RecordKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::NOT_ALLOWED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::NOT_ALLOWED,
    .blankless_key = blankless_key1,
    .allowed = false,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_NotAllowed_RecordKey_MatchKey_SameKey)
{
  controller.ResetStatus(nweb_id1, false, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::NOT_ALLOWED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::NOT_ALLOWED,
    .blankless_key = blankless_key1,
    .allowed = false,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_NotAllowed_RecordKey_MatchKey_DiffKey)
{
  controller.ResetStatus(nweb_id1, false, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.MatchKey(nweb_id1, blankless_key2);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::KEY_NOT_MATCH,
    .blankless_key = blankless_key1,
    .allowed = false,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_NotAllowed_MatchKey)
{
  controller.ResetStatus(nweb_id1, false, false);
  auto status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::KEY_NOT_MATCH,
    .allowed = false,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_Dumped_ResetStatus)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.ResetStatus(nweb_id1, true, false);
  EXPECT_EQ(status, BlanklessController::StatusCode::ALLOWED);
  BlanklessController::StatusInfo info1 = {
    .status_code = BlanklessController::StatusCode::ALLOWED,
    .blankless_key = blankless_key1,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info1));

  status = controller.ResetStatus(nweb_id1, false, false);
  EXPECT_EQ(status, BlanklessController::StatusCode::NOT_ALLOWED);
  BlanklessController::StatusInfo info2 = {
    .status_code = BlanklessController::StatusCode::NOT_ALLOWED,
    .allowed = false,
    .blankless_key = blankless_key1,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info2));
}

TEST_F(BlanklessControllerTest, Status_Dumped_RecordKey)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.RecordKey(nweb_id1, blankless_key2);
  EXPECT_EQ(status, BlanklessController::StatusCode::DUMPED);
  BlanklessController::StatusInfo info1 = {
    .status_code = BlanklessController::StatusCode::DUMPED,
    .blankless_key = blankless_key2,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info1));

  status = controller.RecordKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::DUMPED);
  BlanklessController::StatusInfo info2 = {
    .status_code = BlanklessController::StatusCode::DUMPED,
    .blankless_key = blankless_key1,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info2));
}

TEST_F(BlanklessControllerTest, Status_Dumped_MatchKey)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.MatchKey(nweb_id1, blankless_key2);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info1 = {
    .status_code = BlanklessController::StatusCode::KEY_NOT_MATCH,
    .blankless_key = blankless_key1,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info1));

  status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::INSERTED);
  BlanklessController::StatusInfo info2 = {
    .status_code = BlanklessController::StatusCode::INSERTED,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info2));
}

TEST_F(BlanklessControllerTest, Status_Inserted_ResetStatu)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key1);
  auto status = controller.ResetStatus(nweb_id1, true, false);
  EXPECT_EQ(status, BlanklessController::StatusCode::ALLOWED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::ALLOWED,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_Inserted_RecordKey)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key1);
  auto status = controller.RecordKey(nweb_id1, blankless_key2);
  EXPECT_EQ(status, BlanklessController::StatusCode::DUMPED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::DUMPED,
    .blankless_key = blankless_key2,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_Inserted_MatchKey)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key1);
  auto status = controller.MatchKey(nweb_id1, blankless_key2);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::KEY_NOT_MATCH,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_KeyNotMatch_ResetStatus_Allowed)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key2);
  auto status = controller.ResetStatus(nweb_id1, true, false);
  EXPECT_EQ(status, BlanklessController::StatusCode::ALLOWED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::ALLOWED,
    .blankless_key = blankless_key1,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_KeyNotMatch_RecordKey)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key2);
  auto status = controller.RecordKey(nweb_id1, blankless_key2);
  EXPECT_EQ(status, BlanklessController::StatusCode::DUMPED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::DUMPED,
    .blankless_key = blankless_key2,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_KeyNotMatch_MatchKey)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key2);
  auto status = controller.MatchKey(nweb_id1, blankless_key3);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info1 = {
    .status_code = BlanklessController::StatusCode::KEY_NOT_MATCH,
    .blankless_key = blankless_key1,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info1));

  status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::INSERTED);
  BlanklessController::StatusInfo info2 = {
    .status_code = BlanklessController::StatusCode::INSERTED,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info2));
}

TEST_F(BlanklessControllerTest, Status_NotAllowed_KeyNotMatch_RecordKey)
{
  controller.ResetStatus(nweb_id1, false, false);
  controller.MatchKey(nweb_id1, blankless_key1);
  auto status = controller.RecordKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::NOT_ALLOWED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::NOT_ALLOWED,
    .blankless_key = blankless_key1,
    .allowed = false,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_NotAllowed_Allowed_Success)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key1);
  controller.ResetStatus(nweb_id1, true, false);
  auto status = controller.RecordKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::DUMPED);
  BlanklessController::StatusInfo info1 = {
    .status_code = BlanklessController::StatusCode::DUMPED,
    .blankless_key = blankless_key1,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info1));

  status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::INSERTED);
  BlanklessController::StatusInfo info2 = {
    .status_code = BlanklessController::StatusCode::INSERTED,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info2));
}

TEST_F(BlanklessControllerTest, Status_Allowed_RecordKey_MatchKey_Loop)
{
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::ALLOWED,
  };
  std::vector blankless_keys = {1, 2, 3, 4, 5};
  for (int loop = 0; loop < 2; ++loop) {
    for (int i = 0; i < blankless_keys.size(); ++i) {
      auto status = controller.RecordKey(nweb_id1, blankless_keys[i]);
      EXPECT_EQ(status, BlanklessController::StatusCode::DUMPED);
      info.status_code = BlanklessController::StatusCode::DUMPED;
      info.blankless_key = blankless_keys[i];
      EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
    }
  }
  for (int loop = 0; loop < 2; ++loop) {
    for (int i = 0; i < blankless_keys.size(); ++i) {
      controller.RecordKey(nweb_id1, blankless_keys[i]);
      auto status = controller.MatchKey(nweb_id1, blankless_keys[i]);
      EXPECT_EQ(status, BlanklessController::StatusCode::INSERTED);
      info.status_code = BlanklessController::StatusCode::INSERTED;
      info.blankless_key = BlanklessController::INVALID_BLANKLESS_KEY;
      EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
    }
  }
}

TEST_F(BlanklessControllerTest, Status_RecordKey_MatchKey_SameKey)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::INSERTED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::INSERTED,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_RecordKey_MatchKey_DiffKey)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.MatchKey(nweb_id1, blankless_key2);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::KEY_NOT_MATCH,
    .blankless_key = blankless_key1,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_MatchKey)
{
  auto status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info = {};
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info, false));
}

TEST_F(BlanklessControllerTest, Status_RecordKey_NotAllowed_MatchKey)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.ResetStatus(nweb_id1, false, false);
  auto status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::NOT_ALLOWED);
  BlanklessController::StatusInfo info1 = {
    .status_code = BlanklessController::StatusCode::NOT_ALLOWED,
    .allowed = false,
    .blankless_key = blankless_key1,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info1));

  controller.ResetStatus(nweb_id1, true, false);
  status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::INSERTED);
  BlanklessController::StatusInfo info2 = {
    .status_code = BlanklessController::StatusCode::INSERTED,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info2));
}

TEST_F(BlanklessControllerTest, Status_RecordKey_Allowed_MatchKey)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.ResetStatus(nweb_id1, true, false);
  auto status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::INSERTED);
  BlanklessController::StatusInfo info1 = {
    .status_code = BlanklessController::StatusCode::INSERTED,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info1));

  controller.ResetStatus(nweb_id1, true, false);
  status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info2 = {
    .status_code = BlanklessController::StatusCode::KEY_NOT_MATCH,
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info2));
}

TEST_F(BlanklessControllerTest, Check_Record_Window_Id)
{
  uint32_t window_id = 1;
  controller.RecordWindowId(nweb_id1, window_id);
  auto window_id1 = controller.GetWindowIdByNWebId(nweb_id1);
  EXPECT_EQ(window_id1, window_id);

  uint32_t nweb_id2 = 2;
  auto window_id2 = controller.GetWindowIdByNWebId(nweb_id2);
  EXPECT_EQ(window_id2, 0);
}

TEST_F(BlanklessControllerTest, Check_Record_Dump_Time)
{
  auto dump_time = base::Time::Now().ToInternalValue() / base::Time::kMicrosecondsPerMillisecond;
  controller.RecordDumpTime(nweb_id1, blankless_key1, dump_time);
  auto record_time1 = controller.GetDumpTime(nweb_id1, blankless_key1);
  EXPECT_EQ(record_time1, dump_time);

  uint32_t nweb_id2 = 2;
  auto record_time2 = controller.GetDumpTime(nweb_id2, blankless_key1);
  EXPECT_EQ(record_time2, BlanklessController::INVALID_TIMESTAMP);
}
}  // namespace ohos
}  // namespace base