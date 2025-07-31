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

#define private public
#include "base/ohos/blankless/blankless_controller.h"
#undef private
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace base {
namespace ohos {
class BlanklessControllerTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    controller.ResetForTest();
  }
  void TearDown() override {}

  BlanklessController& controller = BlanklessController::GetInstance();

  uint64_t blankless_key1 = 1024;
  uint64_t blankless_key2 = 1025;
  uint64_t blankless_key3 = 1026;

  int32_t lcp = 300;
  int32_t nweb_id1 = 1;
  int32_t nweb_id2 = 2;
};

TEST_F(BlanklessControllerTest, FrameRemoveCallback)
{
  EXPECT_EQ(controller.m_frame_remove_callback_map_.size(), 0);

  controller.RegisterFrameRemoveCallback(blankless_key1, [](){});
  EXPECT_EQ(controller.m_frame_remove_callback_map_.size(), 1);

  controller.RegisterFrameRemoveCallback(blankless_key2, [](){});
  EXPECT_EQ(controller.m_frame_remove_callback_map_.size(), 2);

  controller.FireFrameRemoveCallback(blankless_key2);
  EXPECT_EQ(controller.m_frame_remove_callback_map_.size(), 1);

  controller.FireFrameRemoveCallback(blankless_key2);
  EXPECT_EQ(controller.m_frame_remove_callback_map_.size(), 1);

  controller.FireFrameRemoveCallback(blankless_key1);
  EXPECT_EQ(controller.m_frame_remove_callback_map_.size(), 0);
}

TEST_F(BlanklessControllerTest, FrameInsertCallback)
{
  EXPECT_EQ(controller.m_frame_insert_callback_map_.size(), 0);

  controller.RegisterFrameInsertCallback(blankless_key1, [](){}, lcp);
  EXPECT_EQ(controller.m_frame_insert_callback_map_.size(), 1);

  controller.RegisterFrameInsertCallback(blankless_key2, [](){}, lcp);
  EXPECT_EQ(controller.m_frame_insert_callback_map_.size(), 2);

  EXPECT_EQ(controller.FireFrameInsertCallback(blankless_key1), lcp);
  EXPECT_EQ(controller.m_frame_insert_callback_map_.size(), 1);

  controller.CancelFrameInsertCallback(blankless_key2);
  EXPECT_EQ(controller.m_frame_insert_callback_map_.size(), 0);

  EXPECT_EQ(controller.FireFrameInsertCallback(blankless_key2), INT32_MAX);
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
    .blankless_key_dumped_history = {blankless_key1},
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

TEST_F(BlanklessControllerTest, Status_Dumped_ResetStatus_Allowed)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.ResetStatus(nweb_id1, true, false);
  EXPECT_EQ(status, BlanklessController::StatusCode::ALLOWED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::ALLOWED,
    .blankless_key_dumped_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_Dumped_RecordKey_DiffKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.RecordKey(nweb_id1, blankless_key2);
  EXPECT_EQ(status, BlanklessController::StatusCode::DUMPED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::DUMPED,
    .blankless_key = blankless_key2,
    .blankless_key_dumped_history = {blankless_key1, blankless_key2},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_Dumped_RecordKey_SameKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.RecordKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::CALL_MULTIPLED_TIMES);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::CALL_MULTIPLED_TIMES,
    .blankless_key = blankless_key1,
    .blankless_key_dumped_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_Dumped_MatchKey_SameKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::INSERTED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::INSERTED,
    .blankless_key_dumped_history = {blankless_key1},
    .blankless_key_inserted_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_Dumped_MatchKey_DiffKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.MatchKey(nweb_id1, blankless_key2);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::KEY_NOT_MATCH,
    .blankless_key = blankless_key1,
    .blankless_key_dumped_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_CallMultipledTimes_ResetStatus_Allowed)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.ResetStatus(nweb_id1, true, false);
  EXPECT_EQ(status, BlanklessController::StatusCode::ALLOWED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::ALLOWED,
    .blankless_key_dumped_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_CallMultipledTimes_RecordKey_DiffKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.RecordKey(nweb_id1, blankless_key2);
  EXPECT_EQ(status, BlanklessController::StatusCode::DUMPED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::DUMPED,
    .blankless_key = blankless_key2,
    .blankless_key_dumped_history = {blankless_key1, blankless_key2},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_CallMultipledTimes_RecordKey_SameKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.RecordKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::CALL_MULTIPLED_TIMES);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::CALL_MULTIPLED_TIMES,
    .blankless_key = blankless_key1,
    .blankless_key_dumped_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_CallMultipledTimes_MatchKey_SameKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::INSERTED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::INSERTED,
    .blankless_key_dumped_history = {blankless_key1},
    .blankless_key_inserted_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_CallMultipledTimes_MatchKey_DiffKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.MatchKey(nweb_id1, blankless_key2);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::KEY_NOT_MATCH,
    .blankless_key = blankless_key1,
    .blankless_key_dumped_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_CallMultipledTimes_MatchKey_Twice)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key1);
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::CALL_MULTIPLED_TIMES);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::CALL_MULTIPLED_TIMES,
    .blankless_key_dumped_history = {blankless_key1},
    .blankless_key_inserted_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_Inserted_ResetStatus_Allowed)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key1);
  auto status = controller.ResetStatus(nweb_id1, true, false);
  EXPECT_EQ(status, BlanklessController::StatusCode::ALLOWED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::ALLOWED,
    .blankless_key_dumped_history = {blankless_key1},
    .blankless_key_inserted_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_Inserted_RecordKey_DiffKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key1);
  auto status = controller.RecordKey(nweb_id1, blankless_key2);
  EXPECT_EQ(status, BlanklessController::StatusCode::DUMPED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::DUMPED,
    .blankless_key = blankless_key2,
    .blankless_key_dumped_history = {blankless_key1, blankless_key2},
    .blankless_key_inserted_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_Inserted_RecordKey_SameKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key1);
  auto status = controller.RecordKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::CALL_MULTIPLED_TIMES);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::CALL_MULTIPLED_TIMES,
    .blankless_key = blankless_key1,
    .blankless_key_dumped_history = {blankless_key1},
    .blankless_key_inserted_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_Inserted_MatchKey_DiffKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key1);
  auto status = controller.MatchKey(nweb_id1, blankless_key2);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::KEY_NOT_MATCH,
    .blankless_key_dumped_history = {blankless_key1},
    .blankless_key_inserted_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_Inserted_MatchKey_SameKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key1);
  auto status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::KEY_NOT_MATCH,
    .blankless_key_dumped_history = {blankless_key1},
    .blankless_key_inserted_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_KeyNotMatch_ResetStatus_Allowed)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key2);
  auto status = controller.ResetStatus(nweb_id1, true, false);
  EXPECT_EQ(status, BlanklessController::StatusCode::ALLOWED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::ALLOWED,
    .blankless_key_dumped_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_KeyNotMatch_RecordKey_DiffKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key2);
  auto status = controller.RecordKey(nweb_id1, blankless_key2);
  EXPECT_EQ(status, BlanklessController::StatusCode::DUMPED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::DUMPED,
    .blankless_key = blankless_key2,
    .blankless_key_dumped_history = {blankless_key1, blankless_key2},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_KeyNotMatch_RecordKey_SameKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key2);
  auto status = controller.RecordKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::CALL_MULTIPLED_TIMES);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::CALL_MULTIPLED_TIMES,
    .blankless_key = blankless_key1,
    .blankless_key_dumped_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_KeyNotMatch_MatchKey_DiffKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key2);
  auto status = controller.MatchKey(nweb_id1, blankless_key3);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::KEY_NOT_MATCH,
    .blankless_key = blankless_key1,
    .blankless_key_dumped_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_KeyNotMatch_MatchKey_SameKey)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key2);
  auto status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::INSERTED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::INSERTED,
    .blankless_key_dumped_history = {blankless_key1},
    .blankless_key_inserted_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
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

TEST_F(BlanklessControllerTest, Status_NotAllowed_Allowed_RecordKey)
{
  controller.ResetStatus(nweb_id1, false, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key1);
  controller.ResetStatus(nweb_id1, true, false);
  auto status = controller.RecordKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::DUMPED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::DUMPED,
    .blankless_key = blankless_key1,
    .blankless_key_dumped_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_NotAllowed_Allowed_Success)
{
  controller.ResetStatus(nweb_id1, false, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key1);
  controller.ResetStatus(nweb_id1, true, false);
  auto status = controller.RecordKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::DUMPED);
  BlanklessController::StatusInfo info1 = {
    .status_code = BlanklessController::StatusCode::DUMPED,
    .blankless_key = blankless_key1,
    .blankless_key_dumped_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info1));

  status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::INSERTED);
  BlanklessController::StatusInfo info2 = {
    .status_code = BlanklessController::StatusCode::INSERTED,
    .blankless_key_dumped_history = {blankless_key1},
    .blankless_key_inserted_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info2));
}

TEST_F(BlanklessControllerTest, Status_Allowed_RecordKey_MatchKey_Loop)
{
  controller.ResetStatus(nweb_id1, true, false);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::ALLOWED,
  };
  std::vector blankless_keys = {1, 2, 3, 4, 5};
  for (int i = 0; i < blankless_keys.size(); ++i) {
    auto status = controller.RecordKey(nweb_id1, blankless_keys[i]);
    EXPECT_EQ(status, BlanklessController::StatusCode::DUMPED);
    info.status_code = BlanklessController::StatusCode::DUMPED;
    info.blankless_key = blankless_keys[i];
    info.blankless_key_dumped_history.insert(blankless_keys[i]);
    EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
  }
  for (int i = 0; i < blankless_keys.size(); ++i) {
    auto status = controller.RecordKey(nweb_id1, blankless_keys[i]);
    EXPECT_EQ(status, BlanklessController::StatusCode::CALL_MULTIPLED_TIMES);
    info.status_code = BlanklessController::StatusCode::CALL_MULTIPLED_TIMES;
    info.blankless_key = blankless_keys[i];
    EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
  }
  for (int i = 0; i < blankless_keys.size(); ++i) {
    controller.RecordKey(nweb_id1, blankless_keys[i]);
    auto status = controller.MatchKey(nweb_id1, blankless_keys[i]);
    EXPECT_EQ(status, BlanklessController::StatusCode::INSERTED);
    info.blankless_key = BlanklessController::INVALID_BLANKLESS_KEY;
    info.status_code = BlanklessController::StatusCode::INSERTED;
    info.blankless_key_inserted_history.insert(blankless_keys[i]);
    EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
  }
  for (int i = 0; i < blankless_keys.size(); ++i) {
    controller.RecordKey(nweb_id1, blankless_keys[i]);
    info.blankless_key = blankless_keys[i];
    auto status = controller.MatchKey(nweb_id1, blankless_keys[i]);
    EXPECT_EQ(status, BlanklessController::StatusCode::CALL_MULTIPLED_TIMES);
    info.status_code = BlanklessController::StatusCode::CALL_MULTIPLED_TIMES;
    info.blankless_key = BlanklessController::INVALID_BLANKLESS_KEY;
    EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
  }
}

TEST_F(BlanklessControllerTest, Status_RemoveStatus)
{
  controller.ResetStatus(nweb_id1, true, false);
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key1);
  controller.RemoveStatus(nweb_id1);
  BlanklessController::StatusInfo info = {};
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info, false));
}

TEST_F(BlanklessControllerTest, Status_RecordKey_MatchKey_SameKey)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  auto status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::INSERTED);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::INSERTED,
    .blankless_key_dumped_history = {blankless_key1},
    .blankless_key_inserted_history = {blankless_key1},
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
    .blankless_key_dumped_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_RecordKey_Twice)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key1);
  auto status = controller.RecordKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::CALL_MULTIPLED_TIMES);
  BlanklessController::StatusInfo info = {
    .status_code = BlanklessController::StatusCode::CALL_MULTIPLED_TIMES,
    .blankless_key = blankless_key1,
    .blankless_key_dumped_history = {blankless_key1},
    .blankless_key_inserted_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info));
}

TEST_F(BlanklessControllerTest, Status_MatchKey_Twice)
{
  controller.RecordKey(nweb_id1, blankless_key1);
  controller.MatchKey(nweb_id1, blankless_key1);
  auto status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info1 = {
    .status_code = BlanklessController::StatusCode::KEY_NOT_MATCH,
    .blankless_key_dumped_history = {blankless_key1},
    .blankless_key_inserted_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info1));

  controller.RecordKey(nweb_id1, blankless_key1);
  status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::CALL_MULTIPLED_TIMES);
  BlanklessController::StatusInfo info2 = {
    .status_code = BlanklessController::StatusCode::CALL_MULTIPLED_TIMES,
    .blankless_key_dumped_history = {blankless_key1},
    .blankless_key_inserted_history = {blankless_key1},
  };
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info2));
}

TEST_F(BlanklessControllerTest, Status_MatchKey)
{
  auto status = controller.MatchKey(nweb_id1, blankless_key1);
  EXPECT_EQ(status, BlanklessController::StatusCode::KEY_NOT_MATCH);
  BlanklessController::StatusInfo info = {};
  EXPECT_TRUE(controller.CheckStatusForTest(nweb_id1, info, false));
}
}  // namespace ohos
}  // namespace base