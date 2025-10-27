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

#include <thread>
#include <chrono>

#include "components/viz/common/frame_sinks/begin_frame_source.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#include "arkweb/chromium_ext/components/viz/service/frame_sinks/external_begin_frame_source_ohos.h"
#include "arkweb/chromium_ext/components/viz/service/frame_sinks/external_begin_frame_source_ohos.cc"
#undef private

namespace viz {
namespace {
    constexpr int64_t TIMEOUT = 200;
}

class ExternalBeginFrameSourceOhosSecondTest : public testing::Test {
public:
    ExternalBeginFrameSourceOhosSecondTest() = default;
    ~ExternalBeginFrameSourceOhosSecondTest() = default;
    void SetUp();
    void TearDown();

    viz::FrameSinkManagerImpl* frame_sink_manager_impl_ = nullptr;
    std::unique_ptr<viz::ExternalBeginFrameSourceOHOS> externalBeginFramesourceOHOS_;
};

void ExternalBeginFrameSourceOhosSecondTest::SetUp() {
    frame_sink_manager_impl_ = new viz::FrameSinkManagerImpl(viz::FrameSinkManagerImpl::InitParams());
    externalBeginFramesourceOHOS_ = std::make_unique<viz::ExternalBeginFrameSourceOHOS>(1, frame_sink_manager_impl_);
}

void ExternalBeginFrameSourceOhosSecondTest::TearDown() {
    externalBeginFramesourceOHOS_.reset();
    if (frame_sink_manager_impl_ != nullptr) {
        delete frame_sink_manager_impl_;
        frame_sink_manager_impl_ = nullptr;
    }
}

TEST_F(ExternalBeginFrameSourceOhosSecondTest, SendInternalBeginFrame) {
    ASSERT_NO_FATAL_FAILURE(externalBeginFramesourceOHOS_->SendInternalBeginFrame());
}

TEST_F(ExternalBeginFrameSourceOhosSecondTest, OnVSync){
    auto userData1 = std::make_unique<ExternalBeginFrameSourceOHOS::VSyncUserData>(
        nullptr, externalBeginFramesourceOHOS_->weak_factory_.GetWeakPtr());
    auto userData2 = std::make_unique<ExternalBeginFrameSourceOHOS::VSyncUserData>(
        base::SingleThreadTaskRunner::GetCurrentDefault(), externalBeginFramesourceOHOS_->weak_factory_.GetWeakPtr());

    ASSERT_NO_FATAL_FAILURE(externalBeginFramesourceOHOS_->OnVSync(1, nullptr));
    ASSERT_NO_FATAL_FAILURE(externalBeginFramesourceOHOS_->OnVSync(1, userData1.get()));
    ASSERT_NO_FATAL_FAILURE(externalBeginFramesourceOHOS_->OnVSync(1, userData2.get()));
}

TEST_F(ExternalBeginFrameSourceOhosSecondTest, OnVSyncImpl){
    auto userData = std::make_unique<ExternalBeginFrameSourceOHOS::VSyncUserData>(
        base::SingleThreadTaskRunner::GetCurrentDefault(), externalBeginFramesourceOHOS_->weak_factory_.GetWeakPtr());

    externalBeginFramesourceOHOS_->OnVSyncImpl(1, nullptr);
    EXPECT_EQ(externalBeginFramesourceOHOS_->user_data_, nullptr);
    externalBeginFramesourceOHOS_->vsync_notification_enabled_ = true;
    ASSERT_NO_FATAL_FAILURE(externalBeginFramesourceOHOS_->OnVSyncImpl(1, userData.get()));
    externalBeginFramesourceOHOS_->first_vsync_since_notify_enabled_ = true;
    ASSERT_NO_FATAL_FAILURE(externalBeginFramesourceOHOS_->OnVSyncImpl(1, userData.get()));
    externalBeginFramesourceOHOS_->first_vsync_since_notify_enabled_ = false;
    ASSERT_NO_FATAL_FAILURE(externalBeginFramesourceOHOS_->OnVSyncImpl(VSYNC_PERIOD_90120HZ_MID + 1, userData.get()));
    ASSERT_NO_FATAL_FAILURE(externalBeginFramesourceOHOS_->OnVSyncImpl(VSYNC_PERIOD_6090HZ_MID + 1, userData.get()));
    ASSERT_NO_FATAL_FAILURE(externalBeginFramesourceOHOS_->OnVSyncImpl(-1, userData.get()));
    externalBeginFramesourceOHOS_->lower_frame_rate_enabled_ = true;
    ASSERT_NO_FATAL_FAILURE(externalBeginFramesourceOHOS_->OnVSyncImpl(VSYNC_PERIOD_90120HZ_MID + 1, userData.get()));
    externalBeginFramesourceOHOS_->lower_frame_rate_enabled_ = false;
    externalBeginFramesourceOHOS_->half_frame_rate_enabled_ = false;
    ASSERT_NO_FATAL_FAILURE(externalBeginFramesourceOHOS_->OnVSyncImpl(VSYNC_PERIOD_90120HZ_MID + 1, userData.get()));
    externalBeginFramesourceOHOS_->lower_frame_rate_enabled_ = true;
    ASSERT_NO_FATAL_FAILURE(externalBeginFramesourceOHOS_->OnVSyncImpl(VSYNC_PERIOD_90120HZ_MID + 1, userData.get()));
    externalBeginFramesourceOHOS_->lower_frame_rate_enabled_ = false;
    externalBeginFramesourceOHOS_->half_frame_rate_enabled_ = true;
    ASSERT_NO_FATAL_FAILURE(externalBeginFramesourceOHOS_->OnVSyncImpl(130000001111, userData.get()));
    externalBeginFramesourceOHOS_->half_frame_rate_enabled_ = false;
    externalBeginFramesourceOHOS_->lower_frame_rate_enabled_ = false;
    ASSERT_NO_FATAL_FAILURE(externalBeginFramesourceOHOS_->OnVSyncImpl(VSYNC_PERIOD_90120HZ_MID + 1, userData.get()));
    externalBeginFramesourceOHOS_->lower_frame_rate_enabled_ = true;
    ASSERT_NO_FATAL_FAILURE(externalBeginFramesourceOHOS_->OnVSyncImpl(VSYNC_PERIOD_90120HZ_MID + 1, userData.get()));
    externalBeginFramesourceOHOS_->last_dvsync_state_ = true;
    externalBeginFramesourceOHOS_->OnVSyncImpl(VSYNC_PERIOD_90120HZ_MID + 1, userData.get());
    EXPECT_EQ(externalBeginFramesourceOHOS_->last_dvsync_state_, false);
    externalBeginFramesourceOHOS_->g_skip_vsync_ = true;
    externalBeginFramesourceOHOS_->OnVSyncImpl(VSYNC_PERIOD_90120HZ_MID + 1, userData.get());
    EXPECT_FALSE(externalBeginFramesourceOHOS_->g_skip_vsync_);
    externalBeginFramesourceOHOS_->update_vsync_frequency_ = true;
    externalBeginFramesourceOHOS_->vsync_frequency_to_update_ = 1;
    externalBeginFramesourceOHOS_->reset_vsync_frequency_ = true;
    externalBeginFramesourceOHOS_->OnVSyncImpl(VSYNC_PERIOD_90120HZ_MID + 1, userData.get());
    EXPECT_FALSE(externalBeginFramesourceOHOS_->reset_vsync_frequency_);
    std::this_thread::sleep_for(std::chrono::milliseconds(TIMEOUT));
}

TEST_F(ExternalBeginFrameSourceOhosSecondTest, UpdateVSyncFrequency) {
    externalBeginFramesourceOHOS_->UpdateVSyncFrequency(VSYNC_30HZ);
    EXPECT_EQ(externalBeginFramesourceOHOS_->vsync_frequency_to_update_, VSYNC_30HZ);

    externalBeginFramesourceOHOS_->UpdateVSyncFrequency(VSYNC_30HZ + 5);
    EXPECT_EQ(externalBeginFramesourceOHOS_->vsync_frequency_to_update_, VSYNC_30HZ);

    externalBeginFramesourceOHOS_->UpdateVSyncFrequency(VSYNC_60HZ);
    EXPECT_EQ(externalBeginFramesourceOHOS_->vsync_frequency_to_update_, VSYNC_60HZ);
}
} //namespace viz