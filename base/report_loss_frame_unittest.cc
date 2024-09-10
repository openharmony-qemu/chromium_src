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

#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#define private public
#include "base/report_loss_frame.h"

namespace base {

class ReportLossFrameTest : public ::testing::Test {
 protected:
  ReportLossFrame* reportLossFrame;

  void SetUp() override { reportLossFrame = ReportLossFrame::GetInstance(); }

  void TearDown() override {
    delete reportLossFrame;
    ReportLossFrame::instance = nullptr;
  }
};

TEST_F(ReportLossFrameTest, GetInstanceTestNoInstance) {
  ReportLossFrame::instance = nullptr;
  ReportLossFrame* instance_ = ReportLossFrame::GetInstance();

  ASSERT_NE(instance_, nullptr);

  ReportLossFrame* secondInstance = ReportLossFrame::GetInstance();
  EXPECT_EQ(instance_, secondInstance);
}

TEST_F(ReportLossFrameTest, GetInstanceTestWithInstance) {
  auto instance = std::make_shared<ReportLossFrame>();
  ReportLossFrame::instance = instance.get();
  ReportLossFrame* getInstance = ReportLossFrame::GetInstance();

  ASSERT_EQ(getInstance, ReportLossFrame::instance);

  ReportLossFrame* secondInstance = ReportLossFrame::GetInstance();
  EXPECT_EQ(getInstance, secondInstance);
}

TEST_F(ReportLossFrameTest, Record001) {
  reportLossFrame->SetScrollState(ScrollMode::STOP);
  reportLossFrame->Record();
  EXPECT_FALSE(reportLossFrame->need_report_);
}

TEST_F(ReportLossFrameTest, Record002) {
  reportLossFrame->SetScrollState(ScrollMode::START);
  reportLossFrame->start_time_for_scroll_ = 0;
  reportLossFrame->Record();
  EXPECT_NE(reportLossFrame->start_time_for_scroll_, 0);
  EXPECT_FALSE(reportLossFrame->need_report_);
}

TEST_F(ReportLossFrameTest, Record003) {
  reportLossFrame->SetScrollState(ScrollMode::START);
  reportLossFrame->SetVsyncPeriod(16666667);
  reportLossFrame->start_time_for_scroll_ =
      reportLossFrame->GetCurrentTimestampMS();
  reportLossFrame->start_time_ = reportLossFrame->start_time_for_scroll_;
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  reportLossFrame->Record();
  EXPECT_FALSE(reportLossFrame->need_report_);
}

TEST_F(ReportLossFrameTest, Record004) {
  reportLossFrame->SetScrollState(ScrollMode::START);
  reportLossFrame->SetVsyncPeriod(16666667);
  reportLossFrame->start_time_for_scroll_ =
      reportLossFrame->GetCurrentTimestampMS();
  reportLossFrame->start_time_ = reportLossFrame->start_time_for_scroll_;
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  reportLossFrame->Record();
  EXPECT_TRUE(reportLossFrame->need_report_);
}

TEST_F(ReportLossFrameTest, Record005) {
  reportLossFrame->SetScrollState(ScrollMode::START);
  reportLossFrame->SetVsyncPeriod(10000000);
  reportLossFrame->start_time_for_scroll_ =
      reportLossFrame->GetCurrentTimestampMS();
  reportLossFrame->start_time_ = reportLossFrame->start_time_for_scroll_;
  std::this_thread::sleep_for(std::chrono::milliseconds(120));
  reportLossFrame->Record();
  EXPECT_TRUE(reportLossFrame->need_report_);
}

TEST_F(ReportLossFrameTest, Record006) {
  reportLossFrame->SetScrollState(ScrollMode::START);
  reportLossFrame->SetVsyncPeriod(10000000);
  reportLossFrame->start_time_for_scroll_ =
      reportLossFrame->GetCurrentTimestampMS();
  reportLossFrame->start_time_ = reportLossFrame->start_time_for_scroll_;
  std::this_thread::sleep_for(std::chrono::milliseconds(180));
  reportLossFrame->Record();
  EXPECT_TRUE(reportLossFrame->need_report_);
}

TEST_F(ReportLossFrameTest, Record007) {
  reportLossFrame->SetScrollState(ScrollMode::START);
  reportLossFrame->SetVsyncPeriod(10000000);
  reportLossFrame->start_time_for_scroll_ =
      reportLossFrame->GetCurrentTimestampMS();
  reportLossFrame->start_time_ = reportLossFrame->start_time_for_scroll_;
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  reportLossFrame->Record();
  EXPECT_TRUE(reportLossFrame->need_report_);
}

TEST_F(ReportLossFrameTest, Record008) {
  reportLossFrame->SetScrollState(ScrollMode::START);
  reportLossFrame->SetVsyncPeriod(10000000);
  reportLossFrame->start_time_for_scroll_ =
      reportLossFrame->GetCurrentTimestampMS();
  reportLossFrame->start_time_ = reportLossFrame->start_time_for_scroll_;
  std::this_thread::sleep_for(std::chrono::milliseconds(400));
  reportLossFrame->Record();
  EXPECT_TRUE(reportLossFrame->need_report_);
}

TEST_F(ReportLossFrameTest, Record009) {
  reportLossFrame->SetScrollState(ScrollMode::START);
  reportLossFrame->SetVsyncPeriod(10000000);
  reportLossFrame->start_time_for_scroll_ =
      reportLossFrame->GetCurrentTimestampMS();
  reportLossFrame->start_time_ = reportLossFrame->start_time_for_scroll_;
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  reportLossFrame->Record();
  EXPECT_TRUE(reportLossFrame->need_report_);
}

TEST_F(ReportLossFrameTest, Record010) {
  reportLossFrame->SetScrollState(ScrollMode::START);
  reportLossFrame->SetVsyncPeriod(10000000);
  reportLossFrame->start_time_for_scroll_ =
      reportLossFrame->GetCurrentTimestampMS();
  reportLossFrame->start_time_ = reportLossFrame->start_time_for_scroll_;
  std::this_thread::sleep_for(std::chrono::milliseconds(800));
  reportLossFrame->Record();
  EXPECT_TRUE(reportLossFrame->need_report_);
}

TEST_F(ReportLossFrameTest, Record011) {
  reportLossFrame->SetScrollState(ScrollMode::START);
  reportLossFrame->SetVsyncPeriod(10000000);
  reportLossFrame->start_time_for_scroll_ =
      reportLossFrame->GetCurrentTimestampMS();
  reportLossFrame->start_time_ = reportLossFrame->start_time_for_scroll_;
  std::this_thread::sleep_for(std::chrono::milliseconds(1200));
  reportLossFrame->Record();
  EXPECT_TRUE(reportLossFrame->need_report_);
}

TEST_F(ReportLossFrameTest, Reset001) {
  reportLossFrame->Reset();
  EXPECT_EQ(reportLossFrame->start_time_for_scroll_, 0);
  EXPECT_FALSE(reportLossFrame->need_report_);
  for (const auto& stat : reportLossFrame->jank_stats_) {
    EXPECT_EQ(stat, 0);
  }
}
}  // namespace base
