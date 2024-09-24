// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#define private public

#include "build/build_config.h"
#if BUILDFLAG(IS_OHOS)
#include "components/viz/service/frame_sinks/external_begin_frame_source_ohos.h"

#include "base/memory/weak_ptr.h"
#include "base/ohos/input_sync/input_vsync_sync_lock.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace viz {
// test class for ExternalFrameSourceOhos
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

}  // namespace viz

#endif
