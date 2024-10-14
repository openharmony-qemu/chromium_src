// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <utility>
#include "build/build_config.h"
#if BUILDFLAG(IS_OHOS)
#define private public
#include "components/viz/service/frame_sinks/external_begin_frame_source_ohos.h"
#undef private
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/ohos/input_sync/input_vsync_sync_lock.h"
#include "external_begin_frame_source_ohos.cc"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace viz {
// test class for ExternalFrameSourceOhos
class MockSingleThreadTaskRunner : public base::SingleThreadTaskRunner{
 public:
  MOCK_METHOD(bool,
              DeleteOrReleaseSoonInternal,
              (const base::Location& from_here,
               void (*deleter)(const void*),
               const void* object),
              (override));
  MOCK_METHOD(bool, RunsTasksInCurrentSequence, (), (const, override));

  MOCK_METHOD(bool,
              PostDelayedTaskAt,
              (base::subtle::PostDelayedTaskPassKey,
               const base::Location& from_here,
               base::OnceClosure task,
               base::TimeTicks delayed_run_time,
               base::subtle::DelayPolicy delay_policy),
              (override));

  MOCK_METHOD(base::DelayedTaskHandle,
              PostCancelableDelayedTask,
              (base::subtle::PostDelayedTaskPassKey,
               const base::Location& from_here,
               base::OnceClosure task,
               base::TimeDelta delay),
              (override));

  MOCK_METHOD(bool,
              PostNonNestableDelayedTask,
              (const base::Location& from_here,
               base::OnceClosure task,
               base::TimeDelta delay),
              (override));
  MOCK_METHOD(bool,
              PostDelayedTask,
              (const base::Location& from_here,
               base::OnceClosure task,
               base::TimeDelta delay),
              (override));
};

class ExternalBeginFrameSourceOhosTest : public testing::Test {
 public:
  ExternalBeginFrameSourceOhosTest() {
    viz::FrameSinkManagerImpl* frame_sink_manager_impl =
        new viz::FrameSinkManagerImpl(viz::FrameSinkManagerImpl::InitParams());
    external_beginframesource_ohos_ =
        std::make_unique<viz::ExternalBeginFrameSourceOHOS>(
            1, frame_sink_manager_impl);
  }

  ~ExternalBeginFrameSourceOhosTest() override {}

  viz::ExternalBeginFrameSourceOHOS* begin_frame_source() {
    return external_beginframesource_ohos_.get();
  }

 private:
  std::unique_ptr<viz::ExternalBeginFrameSourceOHOS>
      external_beginframesource_ohos_;
};

TEST_F(ExternalBeginFrameSourceOhosTest, SetNeedWaitForInput1) {
  // SetNeedWaitForInput is true
  begin_frame_source()->SetNeedWaitForInput(true);
  EXPECT_EQ(true, base::ohos::InputSyncLock::GetInstance().NeedWaitForInput());
}

TEST_F(ExternalBeginFrameSourceOhosTest, SetNeedWaitForInput2) {
  // SetNeedWaitForInput is false
  begin_frame_source()->SetNeedWaitForInput(false);
  EXPECT_EQ(false, base::ohos::InputSyncLock::GetInstance().NeedWaitForInput());
}

TEST_F(ExternalBeginFrameSourceOhosTest, SetNeedWaitForInput3) {
  // SetNeedWaitForInput is 0
  begin_frame_source()->SetNeedWaitForInput(0);
  EXPECT_EQ(false, base::ohos::InputSyncLock::GetInstance().NeedWaitForInput());
}

TEST_F(ExternalBeginFrameSourceOhosTest, SetNeedWaitForInput4) {
  // SetNeedWaitForInput is 1
  begin_frame_source()->SetNeedWaitForInput(1);
  EXPECT_EQ(true, base::ohos::InputSyncLock::GetInstance().NeedWaitForInput());
}

TEST_F(ExternalBeginFrameSourceOhosTest, TriggerVsync) {
  // on_vsync_impl_task_queue_ is empty
  begin_frame_source()->TriggerVsync();
  EXPECT_EQ(true, begin_frame_source()->on_vsync_impl_task_queue_.empty());
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncEndCallback1) {
  // SetNeedWaitForInput is false
  begin_frame_source()->OnVSyncEndCallback();
  EXPECT_EQ(false, base::ohos::InputSyncLock::GetInstance().NeedWaitForInput());
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncEndCallback2) {
  // HandledTouchEvent is false
  begin_frame_source()->OnVSyncEndCallback();
  EXPECT_EQ(false,
            base::ohos::InputSyncLock::GetInstance().HandledTouchEvent());
}

TEST_F(ExternalBeginFrameSourceOhosTest, TriggerVsyncImpl) {
  // on_vsync_impl_task_queue_ is empty
  begin_frame_source()->TriggerVsyncImpl();
  EXPECT_EQ(true, begin_frame_source()->on_vsync_impl_task_queue_.empty());
}

TEST_F(ExternalBeginFrameSourceOhosTest, UpdateVSyncFrequency1) {
  // vsync_frequency_to_update_ is 30 and update_vsync_frequency_ is true
  begin_frame_source()->UpdateVSyncFrequency(10);
  EXPECT_EQ(30, begin_frame_source()->vsync_frequency_to_update_);
  EXPECT_EQ(true, begin_frame_source()->update_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, UpdateVSyncFrequency2) {
  // vsync_frequency_to_update_ is 30 and update_vsync_frequency_ is true
  begin_frame_source()->UpdateVSyncFrequency(30);
  EXPECT_EQ(30, begin_frame_source()->vsync_frequency_to_update_);
  EXPECT_EQ(true, begin_frame_source()->update_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, UpdateVSyncFrequency3) {
  // vsync_frequency_to_update_ is 50 and update_vsync_frequency_ is true
  begin_frame_source()->UpdateVSyncFrequency(50);
  EXPECT_EQ(50, begin_frame_source()->vsync_frequency_to_update_);
  EXPECT_EQ(true, begin_frame_source()->update_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, UpdateVSyncFrequency4) {
  // vsync_frequency_to_update_ is 60 and update_vsync_frequency_ is true
  begin_frame_source()->UpdateVSyncFrequency(60);
  EXPECT_EQ(60, begin_frame_source()->vsync_frequency_to_update_);
  EXPECT_EQ(true, begin_frame_source()->update_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, UpdateVSyncFrequency5) {
  // vsync_frequency_to_update_ is 60 and update_vsync_frequency_ is true
  begin_frame_source()->UpdateVSyncFrequency(70);
  EXPECT_EQ(60, begin_frame_source()->vsync_frequency_to_update_);
  EXPECT_EQ(true, begin_frame_source()->update_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, UpdateVSyncFrequency6) {
  // vsync_frequency_to_update_ is -1 and update_vsync_frequency_ is true
  begin_frame_source()->UpdateVSyncFrequency(-1);
  EXPECT_EQ(30, begin_frame_source()->vsync_frequency_to_update_);
  EXPECT_EQ(true, begin_frame_source()->update_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, UpdateVSyncFrequency7) {
  // vsync_frequency_to_update_ is 80 and update_vsync_frequency_ is true
  begin_frame_source()->UpdateVSyncFrequency(80);
  EXPECT_EQ(60, begin_frame_source()->vsync_frequency_to_update_);
  EXPECT_EQ(true, begin_frame_source()->update_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, UpdateVSyncFrequency8) {
  // vsync_frequency_to_update_ is 0 and update_vsync_frequency_ is true
  begin_frame_source()->UpdateVSyncFrequency(0);
  EXPECT_EQ(30, begin_frame_source()->vsync_frequency_to_update_);
  EXPECT_EQ(true, begin_frame_source()->update_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, UpdateVSyncFrequency9) {
  // vsync_frequency_to_update_ is -1000 and update_vsync_frequency_ is true
  begin_frame_source()->UpdateVSyncFrequency(-1000);
  EXPECT_EQ(30, begin_frame_source()->vsync_frequency_to_update_);
  EXPECT_EQ(true, begin_frame_source()->update_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, UpdateVSyncFrequency10) {
  // vsync_frequency_to_update_ is 1000 and update_vsync_frequency_ is true
  begin_frame_source()->UpdateVSyncFrequency(1000);
  EXPECT_EQ(60, begin_frame_source()->vsync_frequency_to_update_);
  EXPECT_EQ(true, begin_frame_source()->update_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, ResetVSyncFrequency1) {
  // reset_vsync_frequency_ true
  begin_frame_source()->ResetVSyncFrequency();
  EXPECT_EQ(true, begin_frame_source()->reset_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, ResetVSyncFrequency2) {
  // update_vsync_frequency_ is false
  begin_frame_source()->ResetVSyncFrequency();
  EXPECT_EQ(false, begin_frame_source()->update_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSync001) {
  void* data = nullptr;
  int64_t timestamp = 1000;
  testing::internal::CaptureStderr();
  begin_frame_source()->OnVSync(timestamp, data);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OnVSync data is nullptr"), std::string::npos);
}

TEST_F(ExternalBeginFrameSourceOhosTest, UpdateVSyncFrequency001) {
  int frame_rate = 30;
  begin_frame_source()->UpdateVSyncFrequency(frame_rate);
  EXPECT_EQ(30, begin_frame_source()->vsync_frequency_to_update_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, UpdateVSyncFrequency002) {
  int frame_rate = 20;
  begin_frame_source()->UpdateVSyncFrequency(frame_rate);
  EXPECT_EQ(30, begin_frame_source()->vsync_frequency_to_update_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, UpdateVSyncFrequency003) {
  int frame_rate = 54;
  begin_frame_source()->UpdateVSyncFrequency(frame_rate);
  EXPECT_EQ(50, begin_frame_source()->vsync_frequency_to_update_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, UpdateVSyncFrequency004) {
  int frame_rate = 66;
  begin_frame_source()->UpdateVSyncFrequency(frame_rate);
  EXPECT_EQ(60, begin_frame_source()->vsync_frequency_to_update_);
}

#if defined(OHOS_UNITTESTS)
TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl001) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->user_data_.reset(user_data);
  begin_frame_source()->last_vsync_period_ = timestamp;
  begin_frame_source()->vsync_period_ =
      begin_frame_source()->vsync_adapter_.GetVSyncPeriod();
  begin_frame_source()->first_vsync_since_notify_enabled_ = true;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data = nullptr);
  EXPECT_EQ(false, begin_frame_source()->first_vsync_since_notify_enabled_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl002) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->user_data_.reset(user_data);
  begin_frame_source()->last_vsync_period_ = timestamp;
  begin_frame_source()->vsync_period_ =
      begin_frame_source()->vsync_adapter_.GetVSyncPeriod();
  begin_frame_source()->first_vsync_since_notify_enabled_ = false;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data = nullptr);
  EXPECT_EQ(false, begin_frame_source()->first_vsync_since_notify_enabled_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl003) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->user_data_.reset(user_data);
  begin_frame_source()->last_vsync_period_ = timestamp;
  begin_frame_source()->vsync_period_ = 1;
  begin_frame_source()->first_vsync_since_notify_enabled_ = false;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data = nullptr);
  EXPECT_EQ(false, begin_frame_source()->first_vsync_since_notify_enabled_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl004) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->lower_frame_rate_enabled_ = true;
  g_skip_vsync = false;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(true, begin_frame_source()->lower_frame_rate_enabled_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl005) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->lower_frame_rate_enabled_ = false;
  g_skip_vsync = true;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(false, begin_frame_source()->lower_frame_rate_enabled_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl006) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->update_vsync_frequency_ = true;
  begin_frame_source()->vsync_frequency_to_update_ = 1;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(true, begin_frame_source()->update_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl007) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->update_vsync_frequency_ = true;
  begin_frame_source()->vsync_frequency_to_update_ = 0;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(true, begin_frame_source()->update_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl008) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->vsync_notification_enabled_ = true;
  begin_frame_source()->update_vsync_frequency_ = false;
  begin_frame_source()->vsync_frequency_to_update_ = 1;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(false, begin_frame_source()->update_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl009) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->update_vsync_frequency_ = false;
  begin_frame_source()->vsync_frequency_to_update_ = 0;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(false, begin_frame_source()->update_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl010) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->reset_vsync_frequency_ = false;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(false, begin_frame_source()->reset_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl011) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->reset_vsync_frequency_ = true;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(true, begin_frame_source()->reset_vsync_frequency_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl012) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->vsync_period_ =
      begin_frame_source()->vsync_adapter_.GetVSyncPeriod();
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(VSYNC_PERIOD_120HZ, begin_frame_source()->vsync_period_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl013) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->lower_frame_rate_enabled_ = true;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_NE(VSYNC_PERIOD_90HZ, begin_frame_source()->vsync_period_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl014) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->lower_frame_rate_enabled_ = false;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_NE(VSYNC_PERIOD_90HZ, begin_frame_source()->vsync_period_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl015) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->lower_frame_rate_enabled_ = false;
  g_skip_vsync = false;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(false, begin_frame_source()->lower_frame_rate_enabled_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl016) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->lower_frame_rate_enabled_ = true;
  g_skip_vsync = true;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(true, begin_frame_source()->lower_frame_rate_enabled_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl017) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->lower_frame_rate_enabled_ = true;
  g_skip_vsync = true;
  auto frame_sink_manager = const_cast<FrameSinkManagerImpl*>(
      begin_frame_source()->frame_sink_manager_.get());
  frame_sink_manager = nullptr;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(true, begin_frame_source()->lower_frame_rate_enabled_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl018) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->lower_frame_rate_enabled_ = true;
  g_skip_vsync = true;
  auto frame_sink_manager = const_cast<FrameSinkManagerImpl*>(
      begin_frame_source()->frame_sink_manager_.get());
  frame_sink_manager = const_cast<FrameSinkManagerImpl*>(
      begin_frame_source()->frame_sink_manager_.get());
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(true, begin_frame_source()->lower_frame_rate_enabled_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl019) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->vsync_notification_enabled_ = false;
  begin_frame_source()->user_data_ = nullptr;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(false, begin_frame_source()->vsync_notification_enabled_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl020) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  begin_frame_source()->vsync_notification_enabled_ = true;
  begin_frame_source()->user_data_ = nullptr;
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(true, begin_frame_source()->vsync_notification_enabled_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl021) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  std::unique_ptr<ExternalBeginFrameSourceOHOS::VSyncUserData> user_data_;
  begin_frame_source()->vsync_notification_enabled_ = true;
  begin_frame_source()->user_data_ =
      std::make_unique<ExternalBeginFrameSourceOHOS::VSyncUserData>(
          base::SingleThreadTaskRunner::GetCurrentDefault(),
          begin_frame_source()->weak_factory_.GetWeakPtr());
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(true, begin_frame_source()->vsync_notification_enabled_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl022) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 1000;
  std::unique_ptr<ExternalBeginFrameSourceOHOS::VSyncUserData> user_data_;
  begin_frame_source()->vsync_notification_enabled_ = false;
  begin_frame_source()->user_data_ =
      std::make_unique<ExternalBeginFrameSourceOHOS::VSyncUserData>(
          base::SingleThreadTaskRunner::GetCurrentDefault(),
          begin_frame_source()->weak_factory_.GetWeakPtr());
  begin_frame_source()->OnVSyncImpl(timestamp, user_data);
  EXPECT_EQ(false, begin_frame_source()->vsync_notification_enabled_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl023) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 55555555;
  begin_frame_source()->user_data_.reset(user_data);
  begin_frame_source()->last_vsync_period_ = timestamp;
  begin_frame_source()->pre_vsync_period_ = 22222222;
  begin_frame_source()->vsync_period_ =
      begin_frame_source()->vsync_adapter_.GetVSyncPeriod();
  begin_frame_source()->OnVSyncImpl(timestamp, user_data = nullptr);
  EXPECT_EQ(VSYNC_PERIOD_60HZ, begin_frame_source()->vsync_period_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl024) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 55555555;
  begin_frame_source()->user_data_.reset(user_data);
  begin_frame_source()->last_vsync_period_ = timestamp;
  begin_frame_source()->pre_vsync_period_ = 44444444;
  begin_frame_source()->vsync_period_ =
      begin_frame_source()->vsync_adapter_.GetVSyncPeriod();
  begin_frame_source()->OnVSyncImpl(timestamp, user_data = nullptr);
  EXPECT_EQ(VSYNC_PERIOD_90HZ, begin_frame_source()->vsync_period_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl025) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 55555555;
  begin_frame_source()->user_data_.reset(user_data);
  begin_frame_source()->last_vsync_period_ = timestamp;
  begin_frame_source()->pre_vsync_period_ = 50000000;
  begin_frame_source()->vsync_period_ =
      begin_frame_source()->vsync_adapter_.GetVSyncPeriod();
  begin_frame_source()->OnVSyncImpl(timestamp, user_data = nullptr);
  EXPECT_EQ(VSYNC_PERIOD_120HZ, begin_frame_source()->vsync_period_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, OnVSyncImpl026) {
  viz::ExternalBeginFrameSourceOHOS::VSyncUserData* user_data = nullptr;
  int64_t timestamp = 55555555;
  begin_frame_source()->user_data_.reset(user_data);
  begin_frame_source()->last_vsync_period_ = timestamp;
  begin_frame_source()->pre_vsync_period_ = 55555556;
  begin_frame_source()->vsync_period_ =
      begin_frame_source()->vsync_adapter_.GetVSyncPeriod();
  begin_frame_source()->OnVSyncImpl(timestamp, user_data = nullptr);
  EXPECT_EQ(VSYNC_PERIOD_60HZ, begin_frame_source()->vsync_period_);
}
#endif // OHOS_UNITTESTS

TEST_F(ExternalBeginFrameSourceOhosTest, SetEnabled001) {
  bool enabled = true;
  begin_frame_source()->vsync_notification_enabled_ = true;
  begin_frame_source()->user_data_ = nullptr;
  begin_frame_source()->SetEnabled(enabled);
  EXPECT_NE(false, begin_frame_source()->vsync_notification_enabled_);
}

TEST_F(ExternalBeginFrameSourceOhosTest, TriggerVsync_001) {
  scoped_refptr<base::SingleThreadTaskRunner> current =
      base::MakeRefCounted<MockSingleThreadTaskRunner>();
  auto mock_current = static_cast<MockSingleThreadTaskRunner*>(current.get());
  EXPECT_CALL(*mock_current, PostDelayedTask).WillOnce(testing::Return(false));
  base::WeakPtr<viz::ExternalBeginFrameSourceOHOS> weakPtr =
      begin_frame_source()->weak_factory_.GetWeakPtr();
  ExternalBeginFrameSourceOHOS::VSyncUserData* vsync_udata =
      new ExternalBeginFrameSourceOHOS::VSyncUserData(current, weakPtr);
  begin_frame_source()->on_vsync_impl_task_queue_.push_back(
      std::pair<int64_t, ExternalBeginFrameSourceOHOS::VSyncUserData*>(
          1111111, vsync_udata));
  begin_frame_source()->TriggerVsync();
  EXPECT_FALSE(base::ohos::InputSyncLock::GetInstance().HandledTouchEvent());
}

TEST_F(ExternalBeginFrameSourceOhosTest, TriggerVsync_002) {
  begin_frame_source()->TriggerVsync();
  EXPECT_TRUE(base::ohos::InputSyncLock::GetInstance().HandledTouchEvent());
}

TEST_F(ExternalBeginFrameSourceOhosTest, TriggerVsyncImpl_001) {
  scoped_refptr<base::SingleThreadTaskRunner> current =
      base::MakeRefCounted<MockSingleThreadTaskRunner>();
  auto mock_current = static_cast<MockSingleThreadTaskRunner*>(current.get());
  EXPECT_CALL(*mock_current, PostDelayedTask).WillOnce(testing::Return(false));
  base::WeakPtr<viz::ExternalBeginFrameSourceOHOS> weakPtr =
      begin_frame_source()->weak_factory_.GetWeakPtr();
  ExternalBeginFrameSourceOHOS::VSyncUserData* vsync_udata =
      new ExternalBeginFrameSourceOHOS::VSyncUserData(current, weakPtr);
  begin_frame_source()->on_vsync_impl_task_queue_.push_back(
      std::pair<int64_t, ExternalBeginFrameSourceOHOS::VSyncUserData*>(
          1111111, vsync_udata));
  begin_frame_source()->TriggerVsyncImpl();
  EXPECT_TRUE(begin_frame_source()->on_vsync_impl_task_queue_.empty());
}

TEST_F(ExternalBeginFrameSourceOhosTest, TriggerVsyncImpl_002) {
  begin_frame_source()->TriggerVsyncImpl();
  EXPECT_TRUE(begin_frame_source()->on_vsync_impl_task_queue_.empty());
}

TEST_F(ExternalBeginFrameSourceOhosTest, EmplaceVSyncImpl_001) {
  scoped_refptr<base::SingleThreadTaskRunner> current =
      base::MakeRefCounted<MockSingleThreadTaskRunner>();
  base::WeakPtr<viz::ExternalBeginFrameSourceOHOS> weakPtr =
      begin_frame_source()->weak_factory_.GetWeakPtr();
  ExternalBeginFrameSourceOHOS::VSyncUserData* vsync_udata =
      new ExternalBeginFrameSourceOHOS::VSyncUserData(current, weakPtr);
  begin_frame_source()->EmplaceVSyncImpl(111, vsync_udata);
  auto& [timestamp, userData] =
      begin_frame_source()->on_vsync_impl_task_queue_.front();
  EXPECT_EQ(timestamp, 111);
  EXPECT_EQ(userData, vsync_udata);
}

TEST_F(ExternalBeginFrameSourceOhosTest, EmplaceVSyncImpl_002) {
  scoped_refptr<base::SingleThreadTaskRunner> current =
      base::MakeRefCounted<MockSingleThreadTaskRunner>();

  base::WeakPtr<viz::ExternalBeginFrameSourceOHOS> weakPtr =
      begin_frame_source()->weak_factory_.GetWeakPtr();
  ExternalBeginFrameSourceOHOS::VSyncUserData* vsync_udata =
      new ExternalBeginFrameSourceOHOS::VSyncUserData(current, weakPtr);
  for (int i = 0; i < 21; i++) {
    begin_frame_source()->EmplaceVSyncImpl(111, vsync_udata);
  }
  EXPECT_LE(kMaxVsyncTaskQueueSize,
            begin_frame_source()->on_vsync_impl_task_queue_.size());
}

}  // namespace viz

#endif
