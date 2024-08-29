// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#define private public

#include "build/build_config.h"
#if BUILDFLAG(IS_OHOS)
#include "base/ohos/ltpo/include/dynamic_frame_rate_decision.h"
#include "build/build_config.h"
#include "testing/gtest/include/gtest/gtest.h"
#include <chrono>
#include <cmath>
#include "base/task/single_thread_task_executor.h"
#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/thread_pool.h"
#include "base/trace_event/trace_event.h"
#include "content/public/browser/browser_thread.h"
#include "ohos_adapter_helper.h"
#include "base/task/thread_pool.h"
#include "base/task/thread_pool/thread_pool_instance.h"
#include "base/test/bind.h"
#include "base/test/task_environment.h"

namespace base {
namespace ohos {

class DynamicFrameRateDecisionTest : public DynamicFrameRateDecision {
  public:
  DynamicFrameRateDecisionTest() = default;
};

TEST(DynamicFrameRateDecisionTest, Init) 
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  EXPECT_EQ(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  //Assign a value to dynamixFrameRateDecision.curent_task_runner_
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate001)
{
  //Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of false
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = false;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  EXPECT_TRUE(dynamixFrameRateDecision.cur_frame_rate_ == 0);

}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate002)
{
  //Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true; 
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  if(dynamixFrameRateDecision.GetCurrentTimestampMS() - dynamixFrameRateDecision.touch_up_timestamp_< 3000) {
    EXPECT_TRUE(dynamixFrameRateDecision.cur_frame_rate_ == 120);
  }
  else {
    EXPECT_TRUE(dynamixFrameRateDecision.cur_frame_rate_ == 60);
  } 
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate003)
{
  //Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true; 
  //Assign dynamixFrameRateDecision.sliding_frame_rate_ a value of 10
  dynamixFrameRateDecision.sliding_frame_rate_ = 10;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  //dynamixFrameRateDecision.cur_frame_rate_ is 10
  EXPECT_TRUE(dynamixFrameRateDecision.cur_frame_rate_ == 10);
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate004)
{
  //Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true; 
  //Assign dynamixFrameRateDecision.sliding_frame_rate_ a value of 100
  dynamixFrameRateDecision.sliding_frame_rate_ = 100;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  //dynamixFrameRateDecision.cur_frame_rate_ is 100
  EXPECT_TRUE(dynamixFrameRateDecision.cur_frame_rate_ == 100);
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate005)
{
  //Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true; 
  //Assign dynamixFrameRateDecision.sliding_frame_rate_ a value of -10
  dynamixFrameRateDecision.sliding_frame_rate_ = -10;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  if(dynamixFrameRateDecision.GetCurrentTimestampMS() - dynamixFrameRateDecision.touch_up_timestamp_< 3000) {
    EXPECT_TRUE(dynamixFrameRateDecision.cur_frame_rate_ == 120);
  }
  else {
    EXPECT_TRUE(dynamixFrameRateDecision.cur_frame_rate_ == 60);
  } 
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate006)
{
  //Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true; 
  //Assign dynamixFrameRateDecision.video_frame_rate_ a value of 10
  dynamixFrameRateDecision.video_frame_rate_ = 10;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  if(dynamixFrameRateDecision.GetCurrentTimestampMS() - dynamixFrameRateDecision.touch_up_timestamp_< 3000) {
    EXPECT_TRUE(dynamixFrameRateDecision.cur_frame_rate_ == 120);
  }
  else {
    EXPECT_TRUE(dynamixFrameRateDecision.cur_frame_rate_ == 60);
  } 
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate007)
{
  //Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true; 
  //Assign dynamixFrameRateDecision.video_frame_rate_ a value of 70
  dynamixFrameRateDecision.video_frame_rate_ = 70;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  if(dynamixFrameRateDecision.GetCurrentTimestampMS() - dynamixFrameRateDecision.touch_up_timestamp_< 3000) {
    EXPECT_TRUE(dynamixFrameRateDecision.cur_frame_rate_ == 120);
  }
  else {
    EXPECT_TRUE(dynamixFrameRateDecision.cur_frame_rate_ == 70);
  } 
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate008)
{
  //Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true; 
  //Assign dynamixFrameRateDecision.video_frame_rate_ a value of 200
  dynamixFrameRateDecision.video_frame_rate_ = 200;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  //dynamixFrameRateDecision.cur_frame_rate_ is 200
  EXPECT_TRUE(dynamixFrameRateDecision.cur_frame_rate_ == 200);
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate009)
{
  //Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true; 
  //Assign dynamixFrameRateDecision.video_frame_rate_ a value of 60 
  dynamixFrameRateDecision.video_frame_rate_ = 60;
  dynamixFrameRateDecision.sliding_frame_rate_ = 10;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  //dynamixFrameRateDecision.cur_frame_rate_ is 60
  EXPECT_TRUE(dynamixFrameRateDecision.cur_frame_rate_ == 60);
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate010)
{
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true; 
  //Assign dynamixFrameRateDecision.sliding_frame_rate_ a value of 80
  dynamixFrameRateDecision.video_frame_rate_ = 10;
  dynamixFrameRateDecision.sliding_frame_rate_ = 80;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  //dynamixFrameRateDecision.cur_frame_rate_ is 80
  EXPECT_TRUE(dynamixFrameRateDecision.cur_frame_rate_ == 80);
}

TEST(DynamicFrameRateDecisionTest, SetVisible001)
{
  DynamicFrameRateDecision dynamixFrameRateDecision;
  //Assign dynamixFrameRateDecision.curent_task_runner_ a value of nullptr
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = true;
  dynamixFrameRateDecision.SetVisible(measuring_param);
  //dynamixFrameRateDecision.visible_ is false
  EXPECT_TRUE(dynamixFrameRateDecision.visible_ == false);
}

TEST(DynamicFrameRateDecisionTest, SetVisible002)
{
  DynamicFrameRateDecision dynamixFrameRateDecision;
  //Assign dynamixFrameRateDecision.curent_task_runner_ a value of nullptr
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = false;
  dynamixFrameRateDecision.SetVisible(measuring_param);
  //dynamixFrameRateDecision.visible_ is false
  EXPECT_TRUE(dynamixFrameRateDecision.visible_ == false);
}

TEST(DynamicFrameRateDecisionTest, SetVisible003)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  //Assign a value to dynamixFrameRateDecision.curent_task_runner_
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = true;
  dynamixFrameRateDecision.SetVisible(measuring_param);
  task_environment.RunUntilIdle();
  //dynamixFrameRateDecision.visible_ is true
  EXPECT_TRUE(dynamixFrameRateDecision.visible_ == true);
}

TEST(DynamicFrameRateDecisionTest, SetVisible004)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = false;
  dynamixFrameRateDecision.SetVisible(measuring_param);
  task_environment.RunUntilIdle();
  //dynamixFrameRateDecision.visible_ is false
  EXPECT_TRUE(dynamixFrameRateDecision.visible_ == false);
}


TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate001)
{
  DynamicFrameRateDecision dynamixFrameRateDecision;
  //Assign dynamixFrameRateDecision.curent_task_runner_ a value of nullptr
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = 10;
  //The incoming argument is 10
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);
  //dynamixFrameRateDecision.sliding_frame_rate_ is 0
  EXPECT_TRUE(dynamixFrameRateDecision.sliding_frame_rate_ == 0);

}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate002)
{
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = 0;
  //The incoming argument is 0
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);
  //dynamixFrameRateDecision.sliding_frame_rate_ is 0
  EXPECT_TRUE(dynamixFrameRateDecision.sliding_frame_rate_ == 0);
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate003)
{
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = -10;
  //The incoming argument is -10
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);
  //dynamixFrameRateDecision.sliding_frame_rate_ is 0
  EXPECT_TRUE(dynamixFrameRateDecision.sliding_frame_rate_ == 0);   
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate004)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  //Assign a value to dynamixFrameRateDecision.curent_task_runner_
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  //The incoming argument is 10
  int32_t measuring_param = 10;
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);
  task_environment.RunUntilIdle();
  //dynamixFrameRateDecision.sliding_frame_rate_ is 0
  EXPECT_TRUE(dynamixFrameRateDecision.sliding_frame_rate_ == measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate005)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  //The incoming argument is 0
  int32_t measuring_param = 0;
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);

  task_environment.RunUntilIdle();
  //dynamixFrameRateDecision.sliding_frame_rate_ is 0
  EXPECT_TRUE(dynamixFrameRateDecision.sliding_frame_rate_ == measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate006)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  //The incoming argument is -10
  int32_t measuring_param = -10;
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);
  task_environment.RunUntilIdle();
  //dynamixFrameRateDecision.sliding_frame_rate_ is -10
  EXPECT_TRUE(dynamixFrameRateDecision.sliding_frame_rate_ == measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate007)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  //The incoming argument is 1000
  int32_t measuring_param = 1000;
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);
  task_environment.RunUntilIdle();
  //dynamixFrameRateDecision.sliding_frame_rate_ is 1000
  EXPECT_TRUE(dynamixFrameRateDecision.sliding_frame_rate_ == measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate008)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  //The incoming argument is -1000
  int32_t measuring_param = -1000;
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);
  task_environment.RunUntilIdle();
  //dynamixFrameRateDecision.sliding_frame_rate_ is -1000
  EXPECT_TRUE(dynamixFrameRateDecision.sliding_frame_rate_ == measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate001)
{
  DynamicFrameRateDecision dynamixFrameRateDecision;
  //Assign dynamixFrameRateDecision.curent_task_runner_ a value of nullptr
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = 0;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  //The incoming argument is 0, dynamixFrameRateDecision.video_frame_rate_ is 0 
  EXPECT_TRUE(dynamixFrameRateDecision.video_frame_rate_ == 0);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate002)
{
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = 10;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  //The incoming argument is 10, dynamixFrameRateDecision.video_frame_rate_ is 0 
  EXPECT_TRUE(dynamixFrameRateDecision.video_frame_rate_ == 0);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate003)
{
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = -10;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  //The incoming argument is -10, dynamixFrameRateDecision.video_frame_rate_ is 0 
  EXPECT_TRUE(dynamixFrameRateDecision.video_frame_rate_ == 0);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate004)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  //Assign a value to dynamixFrameRateDecision.curent_task_runner_
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = -10;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  task_environment.RunUntilIdle();
  //The incoming argument is -10, dynamixFrameRateDecision.video_frame_rate_ is -10 
  EXPECT_TRUE(dynamixFrameRateDecision.video_frame_rate_ == measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate005)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = 0;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  task_environment.RunUntilIdle();
  //The incoming argument is 0, dynamixFrameRateDecision.video_frame_rate_ is 0 
  EXPECT_TRUE(dynamixFrameRateDecision.video_frame_rate_ == measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate006)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = 10;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  task_environment.RunUntilIdle();
  //The incoming argument is 10, dynamixFrameRateDecision.video_frame_rate_ is 10 
  EXPECT_TRUE(dynamixFrameRateDecision.video_frame_rate_ == measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate007)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = 1000;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  task_environment.RunUntilIdle();
  //The incoming argument is 1000, dynamixFrameRateDecision.video_frame_rate_ is 1000 
  EXPECT_TRUE(dynamixFrameRateDecision.video_frame_rate_ == measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate008)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = -1000;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  task_environment.RunUntilIdle();
  //The incoming argument is -1000, dynamixFrameRateDecision.video_frame_rate_ is -1000 
  EXPECT_TRUE(dynamixFrameRateDecision.video_frame_rate_ == measuring_param);
}

TEST(DynamicFrameRateDecisionTest, SetVsyncEnabled001)
{
  DynamicFrameRateDecision dynamixFrameRateDecision;
  //Assign dynamixFrameRateDecision.curent_task_runner_ a value of nullptr
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = true;
  dynamixFrameRateDecision.SetVsyncEnabled(measuring_param);
  //dynamixFrameRateDecision.vsync_cnt_ is 0
  EXPECT_TRUE(dynamixFrameRateDecision.vsync_cnt_ == 0);
}

TEST(DynamicFrameRateDecisionTest, SetVsyncEnabled002)
{
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = false;
  dynamixFrameRateDecision.SetVsyncEnabled(measuring_param);
  //dynamixFrameRateDecision.vsync_cnt_ is 0
  EXPECT_TRUE(dynamixFrameRateDecision.vsync_cnt_ == 0);
}
    

TEST(DynamicFrameRateDecisionTest, SetVsyncEnabled003)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  //Assign a value to dynamixFrameRateDecision.curent_task_runner_
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = true;
  dynamixFrameRateDecision.SetVsyncEnabled(measuring_param);
  task_environment.RunUntilIdle();
  //The incoming argument is true ,dynamixFrameRateDecision.vsync_cnt_ is 1
  EXPECT_TRUE(dynamixFrameRateDecision.vsync_cnt_ == 1);
}

TEST(DynamicFrameRateDecisionTest, SetVsyncEnabled004)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = false;
  dynamixFrameRateDecision.SetVsyncEnabled(measuring_param);
  task_environment.RunUntilIdle();
  //The incoming argument is false ,dynamixFrameRateDecision.vsync_cnt_ is 0
  EXPECT_TRUE(dynamixFrameRateDecision.vsync_cnt_ == 0);
}

TEST(DynamicFrameRateDecisionTest, SetHasTouchPoint001)
{
  DynamicFrameRateDecision dynamixFrameRateDecision;
  //Assign dynamixFrameRateDecision.curent_task_runner_ a value of nullptr
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = true;
  dynamixFrameRateDecision.SetHasTouchPoint(measuring_param);
  //dynamixFrameRateDecision.has_touch_point_ is false
  EXPECT_TRUE(dynamixFrameRateDecision.has_touch_point_ == false);
}

TEST(DynamicFrameRateDecisionTest, SetHasTouchPoint002)
{
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = false;
  dynamixFrameRateDecision.SetHasTouchPoint(measuring_param);
  //dynamixFrameRateDecision.has_touch_point_ is false
  EXPECT_TRUE(dynamixFrameRateDecision.has_touch_point_ == false);
}

TEST(DynamicFrameRateDecisionTest, SetHasTouchPoint003)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  //Assign a value to dynamixFrameRateDecision.curent_task_runner_
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = true;
  dynamixFrameRateDecision.SetHasTouchPoint(measuring_param);
  task_environment.RunUntilIdle();
  //The incoming argument is true ,dynamixFrameRateDecision.has_touch_point_ is true
  EXPECT_TRUE(dynamixFrameRateDecision.has_touch_point_ == true);
}

TEST(DynamicFrameRateDecisionTest, SetHasTouchPoint004)
{
  test::TaskEnvironment task_environment;
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr,dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = false;
  dynamixFrameRateDecision.SetHasTouchPoint(measuring_param);
  task_environment.RunUntilIdle();
  //The incoming argument is false ,dynamixFrameRateDecision.has_touch_point_ is false
  EXPECT_TRUE(dynamixFrameRateDecision.has_touch_point_ == false);
}


}
}  // namespace base
#endif
