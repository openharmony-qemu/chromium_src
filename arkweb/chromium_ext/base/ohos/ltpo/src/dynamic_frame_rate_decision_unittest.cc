// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <chrono>
#include <cmath>
#include "ohos_sdk/openharmony/native/llvm/bin/../include/libcxx-ohos/include/c++/v1/__ranges/lazy_split_view.h"
#include "base/features.h"
#include "base/test/test_simple_task_runner.h"
#define private public
#include "base/ohos/ltpo/include/dynamic_frame_rate_decision.h"
#undef private
#include "build/build_config.h"
#if BUILDFLAG(ARKWEB_SLIDE_LTPO)

#include "base/no_destructor.h"
#include "base/task/single_thread_task_executor.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/thread_pool.h"
#include "base/task/thread_pool/thread_pool_instance.h"
#include "base/test/bind.h"
#include "base/test/task_environment.h"
#include "base/trace_event/trace_event.h"
#include "build/build_config.h"
#include "content/public/browser/browser_thread.h"
#include "ohos_adapter_helper.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base {
namespace ohos {

class DynamicFrameRateDecisionTest : public DynamicFrameRateDecision {
 public:
  DynamicFrameRateDecisionTest() = default;
};

TEST(DynamicFrameRateDecisionTest, Init) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  EXPECT_EQ(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  // Assign a value to dynamixFrameRateDecision.curent_task_runner_
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
}

TEST(DynamicFrameRateDecisionTest, Init001) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  dynamixFrameRateDecision.curent_task_runner_ = task_runner_1;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate001) {
  // Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of false
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = false;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  EXPECT_EQ(dynamixFrameRateDecision.cur_frame_rate_, 0);
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate002) {
  // Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  if (dynamixFrameRateDecision.GetCurrentTimestampMS() -
          dynamixFrameRateDecision.touch_up_timestamp_ <
      3000) {
    EXPECT_EQ(dynamixFrameRateDecision.cur_frame_rate_, 120);
  } else {
    EXPECT_EQ(dynamixFrameRateDecision.cur_frame_rate_, 60);
  }
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate003) {
  // Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true;
  // Assign dynamixFrameRateDecision.sliding_frame_rate_ a value of 10
  dynamixFrameRateDecision.sliding_frame_rate_ = 10;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  // dynamixFrameRateDecision.cur_frame_rate_ is 10
  EXPECT_EQ(dynamixFrameRateDecision.cur_frame_rate_, 10);
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate004) {
  // Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true;
  // Assign dynamixFrameRateDecision.sliding_frame_rate_ a value of 100
  dynamixFrameRateDecision.sliding_frame_rate_ = 100;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  // dynamixFrameRateDecision.cur_frame_rate_ is 100
  EXPECT_EQ(dynamixFrameRateDecision.cur_frame_rate_, 100);
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate005) {
  // Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true;
  // Assign dynamixFrameRateDecision.sliding_frame_rate_ a value of -10
  dynamixFrameRateDecision.sliding_frame_rate_ = -10;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  if (dynamixFrameRateDecision.GetCurrentTimestampMS() -
          dynamixFrameRateDecision.touch_up_timestamp_ <
      3000) {
    EXPECT_EQ(dynamixFrameRateDecision.cur_frame_rate_, 120);
  } else {
    EXPECT_EQ(dynamixFrameRateDecision.cur_frame_rate_, 60);
  }
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate006) {
  // Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true;
  // Assign dynamixFrameRateDecision.video_frame_rate_ a value of 10
  dynamixFrameRateDecision.video_frame_rate_ = 10;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  if (dynamixFrameRateDecision.GetCurrentTimestampMS() -
          dynamixFrameRateDecision.touch_up_timestamp_ <
      3000) {
    EXPECT_EQ(dynamixFrameRateDecision.cur_frame_rate_, 120);
  } else {
    EXPECT_EQ(dynamixFrameRateDecision.cur_frame_rate_, 10);
  }
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate007) {
  // Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true;
  // Assign dynamixFrameRateDecision.video_frame_rate_ a value of 70
  dynamixFrameRateDecision.video_frame_rate_ = 70;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  if (dynamixFrameRateDecision.GetCurrentTimestampMS() -
          dynamixFrameRateDecision.touch_up_timestamp_ <
      3000) {
    EXPECT_EQ(dynamixFrameRateDecision.cur_frame_rate_, 120);
  } else {
    EXPECT_EQ(dynamixFrameRateDecision.cur_frame_rate_, 70);
  }
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate008) {
  // Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true;
  // Assign dynamixFrameRateDecision.video_frame_rate_ a value of 200
  dynamixFrameRateDecision.video_frame_rate_ = 200;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  // dynamixFrameRateDecision.cur_frame_rate_ is 200
  EXPECT_EQ(dynamixFrameRateDecision.cur_frame_rate_, 200);
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate009) {
  // Assign dynamixFrameRateDecision.frame_rate_linker_enable_ a value of true
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true;
  // Assign dynamixFrameRateDecision.video_frame_rate_ a value of 60
  dynamixFrameRateDecision.video_frame_rate_ = 60;
  dynamixFrameRateDecision.sliding_frame_rate_ = 10;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  // dynamixFrameRateDecision.cur_frame_rate_ is 60
  EXPECT_EQ(dynamixFrameRateDecision.cur_frame_rate_, 60);
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate010) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true;
  // Assign dynamixFrameRateDecision.sliding_frame_rate_ a value of 80
  dynamixFrameRateDecision.video_frame_rate_ = 10;
  dynamixFrameRateDecision.sliding_frame_rate_ = 80;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  // dynamixFrameRateDecision.cur_frame_rate_ is 80
  EXPECT_EQ(dynamixFrameRateDecision.cur_frame_rate_, 80);
}

TEST(DynamicFrameRateDecisionTest, UpdateFramePreferredRate011) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.frame_rate_linker_enable_ = true;
  dynamixFrameRateDecision.has_touch_point_  = true;
  dynamixFrameRateDecision.UpdateFramePreferredRate();
  EXPECT_EQ(dynamixFrameRateDecision.cur_frame_rate_, 120);
}

TEST(DynamicFrameRateDecisionTest, SetVisible001) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  // Assign dynamixFrameRateDecision.curent_task_runner_ a value of nullptr
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  bool measuring_param = true;
  dynamixFrameRateDecision.SetVisible(0, measuring_param);
  // dynamixFrameRateDecision.visible_ is false
  EXPECT_TRUE(dynamixFrameRateDecision.nwebVisibleSet_.empty());
}

TEST(DynamicFrameRateDecisionTest, SetVisible002) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  // Assign dynamixFrameRateDecision.curent_task_runner_ a value of nullptr
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  bool measuring_param = false;
  dynamixFrameRateDecision.SetVisible(0, measuring_param);
  // dynamixFrameRateDecision.visible_ is false
  EXPECT_TRUE(dynamixFrameRateDecision.nwebVisibleSet_.empty());
}

TEST(DynamicFrameRateDecisionTest, SetVisible003) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  // Assign a value to dynamixFrameRateDecision.curent_task_runner_
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = true;
  dynamixFrameRateDecision.SetVisible(0, measuring_param);
  // dynamixFrameRateDecision.visible_ is true
  EXPECT_TRUE(dynamixFrameRateDecision.nwebVisibleSet_.empty());
}

TEST(DynamicFrameRateDecisionTest, SetVisible004) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = false;
  dynamixFrameRateDecision.SetVisible(0, measuring_param);
  // dynamixFrameRateDecision.visible_ is false
  EXPECT_TRUE(dynamixFrameRateDecision.nwebVisibleSet_.empty());
}

TEST(DynamicFrameRateDecisionTest, SetVisibleImpl001) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.strategy_ = LTPOStrategy::HGM_FLING;
  dynamixFrameRateDecision.nwebVisibleSet_.insert(1);
  dynamixFrameRateDecision.SetVisibleImpl(1, false);
  EXPECT_TRUE(dynamixFrameRateDecision.nwebVisibleSet_.empty());
}

TEST(DynamicFrameRateDecisionTest, SetVisibleImpl002) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.strategy_ = LTPOStrategy::HGM_FLING;
  dynamixFrameRateDecision.SetVisibleImpl(1, true);
  EXPECT_FALSE(dynamixFrameRateDecision.nwebVisibleSet_.empty());
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate001) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  // Assign dynamixFrameRateDecision.curent_task_runner_ a value of nullptr
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = 10;
  // The incoming argument is 10
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);
  // dynamixFrameRateDecision.sliding_frame_rate_ is 0
  EXPECT_EQ(dynamixFrameRateDecision.sliding_frame_rate_, 0);
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate002) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = 0;
  // The incoming argument is 0
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);
  // dynamixFrameRateDecision.sliding_frame_rate_ is 0
  EXPECT_EQ(dynamixFrameRateDecision.sliding_frame_rate_, 0);
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate003) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = -10;
  // The incoming argument is -10
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);
  // dynamixFrameRateDecision.sliding_frame_rate_ is 0
  EXPECT_EQ(dynamixFrameRateDecision.sliding_frame_rate_, 0);
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate004) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  // Assign a value to dynamixFrameRateDecision.curent_task_runner_
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  // The incoming argument is 10
  int32_t measuring_param = 10;
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);
  // dynamixFrameRateDecision.sliding_frame_rate_ is 0
  EXPECT_NE(dynamixFrameRateDecision.sliding_frame_rate_, measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate005) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  // The incoming argument is 0
  int32_t measuring_param = 0;
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);

  // dynamixFrameRateDecision.sliding_frame_rate_ is 0
  EXPECT_EQ(dynamixFrameRateDecision.sliding_frame_rate_, measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate006) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  // The incoming argument is -10
  int32_t measuring_param = -10;
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);
  // dynamixFrameRateDecision.sliding_frame_rate_ is -10
  EXPECT_NE(dynamixFrameRateDecision.sliding_frame_rate_, measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate007) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  // The incoming argument is 1000
  int32_t measuring_param = 1000;
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);
  // dynamixFrameRateDecision.sliding_frame_rate_ is 1000
  EXPECT_NE(dynamixFrameRateDecision.sliding_frame_rate_, measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRate008) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  // The incoming argument is -1000
  int32_t measuring_param = -1000;
  dynamixFrameRateDecision.ReportSlidingFrameRate(measuring_param);
  // dynamixFrameRateDecision.sliding_frame_rate_ is -1000
  EXPECT_NE(dynamixFrameRateDecision.sliding_frame_rate_, measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate001) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  // Assign dynamixFrameRateDecision.curent_task_runner_ a value of nullptr
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = 0;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  // The incoming argument is 0, dynamixFrameRateDecision.video_frame_rate_ is 0
  EXPECT_EQ(dynamixFrameRateDecision.video_frame_rate_, 0);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate002) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = 10;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  // The incoming argument is 10, dynamixFrameRateDecision.video_frame_rate_ is
  // 0
  EXPECT_EQ(dynamixFrameRateDecision.video_frame_rate_, 0);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate003) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = -10;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  // The incoming argument is -10, dynamixFrameRateDecision.video_frame_rate_ is
  // 0
  EXPECT_EQ(dynamixFrameRateDecision.video_frame_rate_, 0);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate004) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  // Assign a value to dynamixFrameRateDecision.curent_task_runner_
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = -10;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  // The incoming argument is -10, dynamixFrameRateDecision.video_frame_rate_ is
  // -10
  EXPECT_NE(dynamixFrameRateDecision.video_frame_rate_, measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate005) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = 0;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  // The incoming argument is 0, dynamixFrameRateDecision.video_frame_rate_ is 0
  EXPECT_EQ(dynamixFrameRateDecision.video_frame_rate_, measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate006) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = 10;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  // The incoming argument is 10, dynamixFrameRateDecision.video_frame_rate_ is
  // 10
  EXPECT_NE(dynamixFrameRateDecision.video_frame_rate_, measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate007) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = 1000;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  // The incoming argument is 1000, dynamixFrameRateDecision.video_frame_rate_
  // is 1000
  EXPECT_NE(dynamixFrameRateDecision.video_frame_rate_, measuring_param);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRate008) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  int32_t measuring_param = -1000;
  dynamixFrameRateDecision.ReportVideoFrameRate(measuring_param);
  // The incoming argument is -1000, dynamixFrameRateDecision.video_frame_rate_
  // is -1000
  EXPECT_NE(dynamixFrameRateDecision.video_frame_rate_, measuring_param);
}

TEST(DynamicFrameRateDecisionTest, SetVsyncEnabled001) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  // Assign dynamixFrameRateDecision.curent_task_runner_ a value of nullptr
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = true;
  dynamixFrameRateDecision.SetVsyncEnabled(measuring_param);
  // dynamixFrameRateDecision.vsync_cnt_ is 0
  EXPECT_EQ(dynamixFrameRateDecision.vsync_cnt_, 0);
}

TEST(DynamicFrameRateDecisionTest, SetVsyncEnabled002) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = false;
  dynamixFrameRateDecision.SetVsyncEnabled(measuring_param);
  // dynamixFrameRateDecision.vsync_cnt_ is 0
  EXPECT_EQ(dynamixFrameRateDecision.vsync_cnt_, 0);
}

TEST(DynamicFrameRateDecisionTest, SetVsyncEnabled003) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  // Assign a value to dynamixFrameRateDecision.curent_task_runner_
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = true;
  dynamixFrameRateDecision.SetVsyncEnabled(measuring_param);
  // The incoming argument is true ,dynamixFrameRateDecision.vsync_cnt_ is 1
  EXPECT_EQ(dynamixFrameRateDecision.vsync_cnt_, 1);
}

TEST(DynamicFrameRateDecisionTest, SetVsyncEnabled004) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = false;
  dynamixFrameRateDecision.SetVsyncEnabled(measuring_param);
  // The incoming argument is false ,dynamixFrameRateDecision.vsync_cnt_ is 0
  EXPECT_EQ(dynamixFrameRateDecision.vsync_cnt_, 0);
}

TEST(DynamicFrameRateDecisionTest, SetHasTouchPoint001) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  // Assign dynamixFrameRateDecision.curent_task_runner_ a value of nullptr
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = true;
  dynamixFrameRateDecision.SetHasTouchPoint(measuring_param);
  // dynamixFrameRateDecision.has_touch_point_ is false
  EXPECT_FALSE(dynamixFrameRateDecision.has_touch_point_);
}

TEST(DynamicFrameRateDecisionTest, SetHasTouchPoint002) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  EXPECT_EQ(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = false;
  dynamixFrameRateDecision.SetHasTouchPoint(measuring_param);
  // dynamixFrameRateDecision.has_touch_point_ is false
  EXPECT_FALSE(dynamixFrameRateDecision.has_touch_point_);
}

TEST(DynamicFrameRateDecisionTest, SetHasTouchPoint003) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  // Assign a value to dynamixFrameRateDecision.curent_task_runner_
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = true;
  dynamixFrameRateDecision.SetHasTouchPoint(measuring_param);
  // The incoming argument is true ,dynamixFrameRateDecision.has_touch_point_ is
  // true
  EXPECT_FALSE(dynamixFrameRateDecision.has_touch_point_);
}

TEST(DynamicFrameRateDecisionTest, SetHasTouchPoint004) {
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.Init();
  EXPECT_NE(nullptr, dynamixFrameRateDecision.curent_task_runner_);
  bool measuring_param = false;
  dynamixFrameRateDecision.SetHasTouchPoint(measuring_param);
  // The incoming argument is false ,dynamixFrameRateDecision.has_touch_point_
  // is false
  EXPECT_TRUE(dynamixFrameRateDecision.has_touch_point_ == false);
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRateImpl001) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.ReportSlidingFrameRateImpl(1);
  EXPECT_EQ(dynamixFrameRateDecision.sliding_frame_rate_, 0);
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRateImpl002) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.strategy_ = LTPOStrategy::HGM_FLING;
  dynamixFrameRateDecision.ReportSlidingFrameRateImpl(0);
  EXPECT_EQ(dynamixFrameRateDecision.sliding_frame_rate_, 0);
}

TEST(DynamicFrameRateDecisionTest, ReportSlidingFrameRateImpl003) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.strategy_ = LTPOStrategy::HGM_FLING;
  dynamixFrameRateDecision.ReportSlidingFrameRateImpl(1);
  EXPECT_EQ(dynamixFrameRateDecision.sliding_frame_rate_, 1);
}

TEST(DynamicFrameRateDecisionTest, SetMaxFrameRateThreeSec001) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.sliding_frame_rate_ = 1;
  dynamixFrameRateDecision.SetMaxFrameRateThreeSec();
  EXPECT_EQ(dynamixFrameRateDecision.touch_up_timestamp_, 0);
}

TEST(DynamicFrameRateDecisionTest, SetMaxFrameRateThreeSec002) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.SetMaxFrameRateThreeSec();
  EXPECT_EQ(dynamixFrameRateDecision.touch_up_timestamp_, 0);
}

TEST(DynamicFrameRateDecisionTest, SetMaxFrameRateThreeSec003) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  dynamixFrameRateDecision.Init();
  dynamixFrameRateDecision.vsync_cnt_ = 1;
  dynamixFrameRateDecision.SetMaxFrameRateThreeSec();
  EXPECT_NE(dynamixFrameRateDecision.touch_up_timestamp_, 0);
}

TEST(DynamicFrameRateDecisionTest, SetLTPOStrategy001) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.curent_task_runner_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(dynamixFrameRateDecision.SetLTPOStrategy(1));
}

TEST(DynamicFrameRateDecisionTest, SetLTPOStrategy002) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  scoped_refptr<SingleThreadTaskRunner> task_runner_1(MakeRefCounted<TestSimpleTaskRunner>());
  SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1);
  dynamixFrameRateDecision.Init();
  dynamixFrameRateDecision.SetLTPOStrategy(1);
  EXPECT_EQ(dynamixFrameRateDecision.strategy_, LTPOStrategy::DISABLED);
}

TEST(DynamicFrameRateDecisionTest, SetLTPOStrategyImpl001) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.SetLTPOStrategyImpl(-1);
  EXPECT_EQ(dynamixFrameRateDecision.strategy_, LTPOStrategy::DISABLED);
}

TEST(DynamicFrameRateDecisionTest, SetLTPOStrategyImpl002) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.SetLTPOStrategyImpl(3);
  EXPECT_EQ(dynamixFrameRateDecision.strategy_, LTPOStrategy::ALL);
}

TEST(DynamicFrameRateDecisionTest, SetVsyncEnabledImpl001) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.SetVsyncEnabledImpl(true);
  EXPECT_EQ(dynamixFrameRateDecision.vsync_cnt_, 1);
}

TEST(DynamicFrameRateDecisionTest, SetVsyncEnabledImpl002) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.SetVsyncEnabledImpl(false);
  EXPECT_EQ(dynamixFrameRateDecision.vsync_cnt_, 0);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRateImpl001) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.ReportVideoFrameRateImpl(1);
  EXPECT_EQ(dynamixFrameRateDecision.video_frame_rate_, 0);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRateImpl002) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.strategy_ = LTPOStrategy::ALL;
  dynamixFrameRateDecision.ReportVideoFrameRateImpl(0);
  EXPECT_EQ(dynamixFrameRateDecision.video_frame_rate_, 0);
}

TEST(DynamicFrameRateDecisionTest, ReportVideoFrameRateImpl003) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.strategy_ = LTPOStrategy::ALL;
  dynamixFrameRateDecision.ReportVideoFrameRateImpl(1);
  EXPECT_EQ(dynamixFrameRateDecision.video_frame_rate_, 1);
}

TEST(DynamicFrameRateDecisionTest, SetHasTouchPointImpl001) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.SetHasTouchPointImpl(true);
  EXPECT_FALSE(dynamixFrameRateDecision.has_touch_point_);
}

TEST(DynamicFrameRateDecisionTest, SetHasTouchPointImpl002) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.strategy_ = LTPOStrategy::ALL;
  dynamixFrameRateDecision.SetHasTouchPointImpl(false);
  EXPECT_FALSE(dynamixFrameRateDecision.has_touch_point_);
}

TEST(DynamicFrameRateDecisionTest, SetHasTouchPointImpl003) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.strategy_ = LTPOStrategy::ALL;
  dynamixFrameRateDecision.SetHasTouchPointImpl(true);
  EXPECT_TRUE(dynamixFrameRateDecision.has_touch_point_);
}

TEST(DynamicFrameRateDecisionTest, SetHasTouchPointImpl004) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.strategy_ = LTPOStrategy::ALL;
  dynamixFrameRateDecision.has_touch_point_ = true;
  dynamixFrameRateDecision.SetHasTouchPointImpl(false);
  EXPECT_FALSE(dynamixFrameRateDecision.has_touch_point_);
}

TEST(DynamicFrameRateDecisionTest, SetFrameRateLinkerEnable001) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.SetFrameRateLinkerEnable(false);
  EXPECT_FALSE(dynamixFrameRateDecision.frame_rate_linker_enable_);
}

TEST(DynamicFrameRateDecisionTest, SetFrameRateLinkerEnable002) {
  DynamicFrameRateDecision dynamixFrameRateDecision;
  dynamixFrameRateDecision.SetFrameRateLinkerEnable(true);
  EXPECT_TRUE(dynamixFrameRateDecision.frame_rate_linker_enable_);
}

}  // namespace ohos
}  // namespace base
#endif
