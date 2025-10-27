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
  dynamic_frame->is_monitoring_ = false;
  dynamic_frame->StartMonitor();
  EXPECT_TRUE(dynamic_frame->is_monitoring_);
}

TEST_F(DynamicFrameLossMonitorTest, StartMonitor_002) {
  dynamic_frame->is_monitoring_ = false;
  dynamic_frame->StartMonitor();
  EXPECT_TRUE(dynamic_frame->is_monitoring_);
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
  dynamic_frame->prev_swap_buffer_time_ = initialTime + 100; // 100ms
  dynamic_frame->OnSwapBuffer();
  EXPECT_TRUE(dynamic_frame->received_first_frame_);
  EXPECT_EQ(2, dynamic_frame->cached_buffer_number_);
}

TEST_F(DynamicFrameLossMonitorTest, OnSwapBuffer_004) {
  dynamic_frame->is_monitoring_ = true;
  dynamic_frame->OnSwapBuffer();
  int64_t initialTime = dynamic_frame->prev_swap_buffer_time_;
  dynamic_frame->prev_swap_buffer_time_ = initialTime + 50; // 50ms
  dynamic_frame->OnSwapBuffer();
  dynamic_frame->prev_swap_buffer_time_ = initialTime + 150; // 150ms
  dynamic_frame->OnSwapBuffer();
  EXPECT_NE(24, dynamic_frame->max_app_frametime_);
}

TEST_F(DynamicFrameLossMonitorTest, Report_001) {
  dynamic_frame->total_app_missed_frames_ = 0;
  dynamic_frame->Report();
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

}  // namespace ohos
}  // namespace base
