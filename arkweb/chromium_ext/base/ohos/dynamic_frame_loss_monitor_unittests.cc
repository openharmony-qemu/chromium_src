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

#define private public
#include "base/ohos/dynamic_frame_loss_monitor.h"
#undef private
#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/task/thread_pool.h"
#include "base/trace_event/trace_event.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "ohos_nweb/src/sysevent/event_reporter.h"

namespace base {
namespace ohos {

class DynamicFrameLossMonitorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    dynamic_frame = std::make_shared<DynamicFrameLossMonitor>();
  }

  void TearDown() override {}

  std::shared_ptr<DynamicFrameLossMonitor> dynamic_frame;
};

TEST_F(DynamicFrameLossMonitorTest, GetInstance) {
  dynamic_frame->prev_swap_buffer_time_ = 0;
  DynamicFrameLossMonitor& ret = dynamic_frame->GetInstance();
  EXPECT_EQ(ret.prev_swap_buffer_time_, 0);
}

TEST_F(DynamicFrameLossMonitorTest, StartMonitor_001) {
  int32_t nweb_id = 1;
  dynamic_frame->is_monitoring_ = false;
  dynamic_frame->StartMonitor(nweb_id);
  EXPECT_TRUE(dynamic_frame->is_monitoring_);
}

TEST_F(DynamicFrameLossMonitorTest, StartMonitor_002) {
  int32_t nweb_id = 1;
  dynamic_frame->is_monitoring_ = false;
  dynamic_frame->StartMonitor(nweb_id);
  EXPECT_EQ(dynamic_frame->current_nweb_id_, nweb_id);
}

TEST_F(DynamicFrameLossMonitorTest, StopMonitor_001) {
  dynamic_frame->is_monitoring_ = false;
  dynamic_frame->StopMonitor();
  EXPECT_FALSE(dynamic_frame->is_monitoring_);
}

TEST_F(DynamicFrameLossMonitorTest, StopMonitor_002) {
  dynamic_frame->stop_time_ = 0;
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->StopMonitor();
  EXPECT_EQ(dynamic_frame->stop_time_, 0);
}

TEST_F(DynamicFrameLossMonitorTest, OnVsync_001) {
  dynamic_frame->is_monitoring_ = false;
  dynamic_frame->OnVsync();
  EXPECT_FALSE(dynamic_frame->is_monitoring_);
}

TEST_F(DynamicFrameLossMonitorTest, OnVsync_002) {
  dynamic_frame->total_app_frames_ = 0;
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->received_first_frame_ = false;
  dynamic_frame->OnVsync();
  EXPECT_NE(dynamic_frame->total_app_frames_, 0);
}

TEST_F(DynamicFrameLossMonitorTest, OnVsync_003) {
  dynamic_frame->total_app_frames_ = 0;
  dynamic_frame->total_app_missed_frames_ = 0;
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->received_first_frame_ = true;
  dynamic_frame->cached_buffer_number_ = 1;
  dynamic_frame->app_seq_frames_ = 0;
  dynamic_frame->OnVsync();
  EXPECT_NE(dynamic_frame->app_seq_frames_, 0);
}

TEST_F(DynamicFrameLossMonitorTest, OnVsync_004) {
  dynamic_frame->total_app_frames_ = 0;
  dynamic_frame->total_app_missed_frames_ = 0;
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->received_first_frame_ = true;
  dynamic_frame->cached_buffer_number_ = 1;
  dynamic_frame->app_seq_frames_ = 2;
  dynamic_frame->OnVsync();
  EXPECT_NE(dynamic_frame->app_seq_frames_, 2);
}

TEST_F(DynamicFrameLossMonitorTest, OnVsync_005) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->received_first_frame_ = true;
  dynamic_frame->cached_buffer_number_ = 0;
  dynamic_frame->total_app_missed_frames_ = 0;
  dynamic_frame->app_seq_frames_ = 2;
  dynamic_frame->OnVsync();
  EXPECT_EQ(dynamic_frame->total_app_missed_frames_, 1);
}

TEST_F(DynamicFrameLossMonitorTest, OnVsync_006) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->received_first_frame_ = true;
  dynamic_frame->cached_buffer_number_ = 0;
  dynamic_frame->total_app_missed_frames_ = 0;
  dynamic_frame->app_seq_frames_ = 1;
  dynamic_frame->OnVsync();
  EXPECT_EQ(dynamic_frame->app_seq_frames_, 0);
}

TEST_F(DynamicFrameLossMonitorTest, OnSwapBuffer_001) {
  auto initial_received_first_frame = dynamic_frame->received_first_frame_;
  dynamic_frame->is_monitoring_ = false;
  dynamic_frame->OnSwapBuffer();
  EXPECT_EQ(dynamic_frame->received_first_frame_, initial_received_first_frame);
}

TEST_F(DynamicFrameLossMonitorTest, OnSwapBuffer_002) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->OnSwapBuffer();
  auto received_first_frame = dynamic_frame->received_first_frame_;
  EXPECT_TRUE(received_first_frame);
  EXPECT_GT(dynamic_frame->prev_swap_buffer_time_, 0);
  EXPECT_EQ(0, dynamic_frame->max_app_frametime_);
}

TEST_F(DynamicFrameLossMonitorTest, OnSwapBuffer_003) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->OnSwapBuffer();
  int64_t initialTime = dynamic_frame->prev_swap_buffer_time_;
  dynamic_frame->prev_swap_buffer_time_ = initialTime + 100;
  dynamic_frame->OnSwapBuffer();
  EXPECT_TRUE(dynamic_frame->received_first_frame_);
  EXPECT_EQ(2, dynamic_frame->cached_buffer_number_);
}

TEST_F(DynamicFrameLossMonitorTest, OnSwapBuffer_004) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->OnSwapBuffer();
  int64_t initialTime = dynamic_frame->prev_swap_buffer_time_;
  dynamic_frame->prev_swap_buffer_time_ = initialTime + 50;
  dynamic_frame->OnSwapBuffer();
  dynamic_frame->prev_swap_buffer_time_ = initialTime + 150;
  dynamic_frame->OnSwapBuffer();
  EXPECT_NE(24, dynamic_frame->max_app_frametime_);
}

TEST_F(DynamicFrameLossMonitorTest, ReportToHiAppEvent_001) {
  dynamic_frame->max_app_frametime_ = 0;
  dynamic_frame->ReportToHiAppEvent();
  EXPECT_EQ(0, dynamic_frame->max_app_frametime_);
}

TEST_F(DynamicFrameLossMonitorTest, ReportToHiSysEvent_001) {
  dynamic_frame->total_app_missed_frames_ = 0;
  dynamic_frame->ReportToHiSysEvent();
  EXPECT_EQ(0, dynamic_frame->total_app_missed_frames_);
}

TEST_F(DynamicFrameLossMonitorTest, ResetStatus_001) {
  dynamic_frame->ResetStatus();
  EXPECT_FALSE(dynamic_frame->is_monitoring_);
  EXPECT_FALSE(dynamic_frame->received_first_frame_);
  EXPECT_EQ(0, dynamic_frame->start_time_);
  EXPECT_EQ(0, dynamic_frame->stop_time_);
  EXPECT_EQ(0, dynamic_frame->total_app_frames_);
  EXPECT_EQ(0, dynamic_frame->total_app_missed_frames_);
  EXPECT_EQ(0, dynamic_frame->max_app_seq_missed_frames_);
  EXPECT_EQ(0, dynamic_frame->app_seq_missed_frames_);
  EXPECT_EQ(0, dynamic_frame->app_seq_frames_);
  EXPECT_EQ(0, dynamic_frame->prev_swap_buffer_time_);
  EXPECT_EQ(0, dynamic_frame->max_app_frametime_);
  EXPECT_EQ(0, dynamic_frame->cached_buffer_number_);
}

TEST_F(DynamicFrameLossMonitorTest, StartMonitor_003) {
  int32_t nweb_id = 1;
  dynamic_frame->is_monitoring_ = false;
  dynamic_frame->start_time_ = 0;
  dynamic_frame->StartMonitor(nweb_id);
  EXPECT_GT(dynamic_frame->start_time_, 0);
}

TEST_F(DynamicFrameLossMonitorTest, StartMonitor_004) {
  int32_t nweb_id = 2;
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->current_nweb_id_ = 1;
  dynamic_frame->StartMonitor(nweb_id);
  EXPECT_EQ(dynamic_frame->current_nweb_id_, nweb_id);
}

TEST_F(DynamicFrameLossMonitorTest, StartMonitor_005) {
  int32_t nweb_id = 0;
  dynamic_frame->is_monitoring_ = false;
  dynamic_frame->StartMonitor(nweb_id);
  EXPECT_TRUE(dynamic_frame->is_monitoring_);
  EXPECT_EQ(dynamic_frame->current_nweb_id_, 0);
}

TEST_F(DynamicFrameLossMonitorTest, StartMonitor_006) {
  int32_t nweb_id = -1;
  dynamic_frame->is_monitoring_ = false;
  dynamic_frame->StartMonitor(nweb_id);
  EXPECT_TRUE(dynamic_frame->is_monitoring_);
  EXPECT_EQ(dynamic_frame->current_nweb_id_, -1);
}

TEST_F(DynamicFrameLossMonitorTest, StartMonitor_007) {
  int32_t nweb_id = 100;
  dynamic_frame->is_monitoring_ = false;
  dynamic_frame->StartMonitor(nweb_id);
  EXPECT_TRUE(dynamic_frame->is_monitoring_);
  EXPECT_EQ(dynamic_frame->current_nweb_id_, 100);
}

TEST_F(DynamicFrameLossMonitorTest, StopMonitor_003) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->stop_time_ = 0;
  dynamic_frame->StopMonitor();
  EXPECT_EQ(dynamic_frame->stop_time_, 0);
}

TEST_F(DynamicFrameLossMonitorTest, OnVsync_007) {
  dynamic_frame->total_app_frames_ = 0;
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->received_first_frame_ = true;
  dynamic_frame->cached_buffer_number_ = 5;
  dynamic_frame->app_seq_missed_frames_ = 3;
  dynamic_frame->app_seq_frames_ = 3;
  dynamic_frame->OnVsync();
  EXPECT_EQ(dynamic_frame->cached_buffer_number_, 4);
  EXPECT_EQ(dynamic_frame->app_seq_frames_, 4);
  EXPECT_EQ(dynamic_frame->app_seq_missed_frames_, 0);
}

TEST_F(DynamicFrameLossMonitorTest, OnVsync_008) {
  dynamic_frame->total_app_frames_ = 0;
  dynamic_frame->total_app_missed_frames_ = 0;
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->received_first_frame_ = true;
  dynamic_frame->cached_buffer_number_ = 2;
  dynamic_frame->app_seq_frames_ = 1;
  dynamic_frame->app_seq_missed_frames_ = 0;
  dynamic_frame->OnVsync();
  EXPECT_EQ(dynamic_frame->app_seq_frames_, 2);
  EXPECT_EQ(dynamic_frame->cached_buffer_number_, 1);
  EXPECT_EQ(dynamic_frame->total_app_missed_frames_, 0);
}

TEST_F(DynamicFrameLossMonitorTest, OnVsync_009) {
  dynamic_frame->max_app_seq_missed_frames_ = 0;
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->received_first_frame_ = true;
  dynamic_frame->cached_buffer_number_ = 0;
  dynamic_frame->total_app_missed_frames_ = 0;
  dynamic_frame->app_seq_missed_frames_ = 0;
  dynamic_frame->app_seq_frames_ = 0;
  dynamic_frame->OnVsync();
  EXPECT_EQ(dynamic_frame->max_app_seq_missed_frames_, 1);
}

TEST_F(DynamicFrameLossMonitorTest, OnVsync_010) {
  dynamic_frame->max_app_seq_missed_frames_ = 5;
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->received_first_frame_ = true;
  dynamic_frame->cached_buffer_number_ = 0;
  dynamic_frame->total_app_missed_frames_ = 0;
  dynamic_frame->app_seq_missed_frames_ = 3;
  dynamic_frame->app_seq_frames_ = 0;
  dynamic_frame->OnVsync();
  EXPECT_EQ(dynamic_frame->max_app_seq_missed_frames_, 5);
}

TEST_F(DynamicFrameLossMonitorTest, OnVsync_011) {
  dynamic_frame->max_app_seq_missed_frames_ = 3;
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->received_first_frame_ = true;
  dynamic_frame->cached_buffer_number_ = 0;
  dynamic_frame->total_app_missed_frames_ = 0;
  dynamic_frame->app_seq_missed_frames_ = 10;
  dynamic_frame->app_seq_frames_ = 0;
  dynamic_frame->OnVsync();
  EXPECT_EQ(dynamic_frame->max_app_seq_missed_frames_, 11);
}

TEST_F(DynamicFrameLossMonitorTest, OnVsync_012) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->received_first_frame_ = true;
  dynamic_frame->cached_buffer_number_ = 10;
  dynamic_frame->total_app_missed_frames_ = 0;
  dynamic_frame->app_seq_missed_frames_ = 5;
  dynamic_frame->app_seq_frames_ = 5;
  dynamic_frame->OnVsync();
  EXPECT_EQ(dynamic_frame->app_seq_missed_frames_, 0);
}

TEST_F(DynamicFrameLossMonitorTest, OnVsync_013) {
  dynamic_frame->total_app_frames_ = 100;
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->received_first_frame_ = false;
  dynamic_frame->OnVsync();
  EXPECT_EQ(dynamic_frame->total_app_frames_, 101);
}

TEST_F(DynamicFrameLossMonitorTest, OnSwapBuffer_005) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->cached_buffer_number_ = 0;
  dynamic_frame->OnSwapBuffer();
  EXPECT_EQ(dynamic_frame->cached_buffer_number_, 1);
}

TEST_F(DynamicFrameLossMonitorTest, OnSwapBuffer_006) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->cached_buffer_number_ = 5;
  dynamic_frame->OnSwapBuffer();
  EXPECT_EQ(dynamic_frame->cached_buffer_number_, 6);
}

TEST_F(DynamicFrameLossMonitorTest, OnSwapBuffer_007) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->max_app_frametime_ = 0;
  dynamic_frame->prev_swap_buffer_time_ = 100;
  dynamic_frame->OnSwapBuffer();
  EXPECT_GT(dynamic_frame->max_app_frametime_, 0);
}

TEST_F(DynamicFrameLossMonitorTest, OnSwapBuffer_008) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->max_app_frametime_ = 200;
  dynamic_frame->prev_swap_buffer_time_ = 100;
  dynamic_frame->OnSwapBuffer();
  EXPECT_GE(dynamic_frame->max_app_frametime_, 200);
}

TEST_F(DynamicFrameLossMonitorTest, OnSwapBuffer_009) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->received_first_frame_ = false;
  dynamic_frame->OnSwapBuffer();
  EXPECT_TRUE(dynamic_frame->received_first_frame_);
}

TEST_F(DynamicFrameLossMonitorTest, OnSwapBuffer_010) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->cached_buffer_number_ = -1;
  dynamic_frame->OnSwapBuffer();
  EXPECT_EQ(dynamic_frame->cached_buffer_number_, 0);
}

TEST_F(DynamicFrameLossMonitorTest, OnSwapBuffer_011) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->cached_buffer_number_ = 0;
  for (int i = 0; i < 10; i++) {
    dynamic_frame->OnSwapBuffer();
  }
  EXPECT_EQ(dynamic_frame->cached_buffer_number_, 10);
}

TEST_F(DynamicFrameLossMonitorTest, ResetStatus_002) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->received_first_frame_ = true;
  dynamic_frame->start_time_ = 1000;
  dynamic_frame->stop_time_ = 2000;
  dynamic_frame->total_app_frames_ = 100;
  dynamic_frame->total_app_missed_frames_ = 10;
  dynamic_frame->max_app_seq_missed_frames_ = 5;
  dynamic_frame->app_seq_missed_frames_ = 3;
  dynamic_frame->app_seq_frames_ = 7;
  dynamic_frame->prev_swap_buffer_time_ = 1500;
  dynamic_frame->max_app_frametime_ = 200;
  dynamic_frame->cached_buffer_number_ = 3;
  dynamic_frame->ResetStatus();
  EXPECT_FALSE(dynamic_frame->is_monitoring_);
  EXPECT_FALSE(dynamic_frame->received_first_frame_);
  EXPECT_EQ(0, dynamic_frame->start_time_);
  EXPECT_EQ(0, dynamic_frame->stop_time_);
  EXPECT_EQ(0, dynamic_frame->total_app_frames_);
  EXPECT_EQ(0, dynamic_frame->total_app_missed_frames_);
  EXPECT_EQ(0, dynamic_frame->max_app_seq_missed_frames_);
  EXPECT_EQ(0, dynamic_frame->app_seq_missed_frames_);
  EXPECT_EQ(0, dynamic_frame->app_seq_frames_);
  EXPECT_EQ(0, dynamic_frame->prev_swap_buffer_time_);
  EXPECT_EQ(0, dynamic_frame->max_app_frametime_);
  EXPECT_EQ(0, dynamic_frame->cached_buffer_number_);
}

TEST_F(DynamicFrameLossMonitorTest, ResetStatus_003) {
  dynamic_frame->is_monitoring_ = false;
  dynamic_frame->received_first_frame_ = false;
  dynamic_frame->ResetStatus();
  EXPECT_FALSE(dynamic_frame->is_monitoring_);
  EXPECT_FALSE(dynamic_frame->received_first_frame_);
}

TEST_F(DynamicFrameLossMonitorTest, GetCurrentTimestampMS_001) {
  int64_t timestamp1 = dynamic_frame->GetCurrentTimestampMS();
  EXPECT_GT(timestamp1, 0);
}

TEST_F(DynamicFrameLossMonitorTest, GetCurrentTimestampMS_002) {
  int64_t timestamp1 = dynamic_frame->GetCurrentTimestampMS();
  int64_t timestamp2 = dynamic_frame->GetCurrentTimestampMS();
  EXPECT_GE(timestamp2, timestamp1);
}

TEST_F(DynamicFrameLossMonitorTest, FullCycle_001) {
  int32_t nweb_id = 1;
  dynamic_frame->StartMonitor(nweb_id);
  EXPECT_TRUE(dynamic_frame->is_monitoring_);
  dynamic_frame->OnSwapBuffer();
  EXPECT_TRUE(dynamic_frame->received_first_frame_);
  dynamic_frame->OnVsync();
  EXPECT_GT(dynamic_frame->total_app_frames_, 0);
  dynamic_frame->StopMonitor();
  EXPECT_FALSE(dynamic_frame->is_monitoring_);
}

TEST_F(DynamicFrameLossMonitorTest, FullCycle_002) {
  int32_t nweb_id = 5;
  dynamic_frame->StartMonitor(nweb_id);
  for (int i = 0; i < 5; i++) {
    dynamic_frame->OnSwapBuffer();
    dynamic_frame->OnVsync();
  }
  EXPECT_EQ(dynamic_frame->cached_buffer_number_, 0);
  EXPECT_EQ(dynamic_frame->total_app_frames_, 5);
  dynamic_frame->StopMonitor();
  EXPECT_FALSE(dynamic_frame->is_monitoring_);
}

TEST_F(DynamicFrameLossMonitorTest, FullCycle_003) {
  int32_t nweb_id = 10;
  dynamic_frame->StartMonitor(nweb_id);
  dynamic_frame->OnVsync();
  dynamic_frame->OnVsync();
  dynamic_frame->OnVsync();
  EXPECT_EQ(dynamic_frame->total_app_frames_, 3);
  EXPECT_FALSE(dynamic_frame->received_first_frame_);
  dynamic_frame->StopMonitor();
}

}  // namespace ohos
}  // namespace base
