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

#include <algorithm>
#include <cstdint>
#include <memory>
#include <utility>
#include "base/test/test_mock_time_task_runner.h"
#include "content/public/browser/web_contents.h"
#include "media/audio/audio_io.h"
#include "media/audio/audio_thread.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#define protected public
#include "content/browser/media/session/media_session_impl.h"
#include "ohos_audio_output_stream.h"
#undef protected
#undef private

using namespace testing;

namespace media {

class AudioRendererOptionsTest : public testing::Test {
 public:
  void SetUp() override {
    options_ = std::make_unique<AudioRendererOptions>();
    ASSERT_NE(options_, nullptr);
  }
  void TearDown() override { options_ = nullptr; }

 protected:
  std::unique_ptr<AudioRendererOptions> options_;
};

class AudioRendererCallbackTest : public ::testing::Test {
 public:
  AudioRendererCallbackTest() = default;
  void SetUp() override {
    render_callback_ =
        std::make_unique<AudioRendererCallback>(nullptr, nullptr);
    ASSERT_NE(render_callback_, nullptr);
  }
  void TearDown() override { render_callback_ = nullptr; }
  std::unique_ptr<AudioRendererCallback> render_callback_;
};

class AudioOutputChangeCallbackTest : public ::testing::Test {
 public:
  AudioOutputChangeCallbackTest() = default;
  void SetUp() override {
    change_callback_ =
        std::make_unique<AudioOutputChangeCallback>(nullptr, params_, false);
    ASSERT_NE(change_callback_, nullptr);
  }
  void TearDown() override { change_callback_ = nullptr; }
  AudioParameters params_;
  std::unique_ptr<AudioOutputChangeCallback> change_callback_;
};

TEST_F(AudioOutputChangeCallbackTest, OnOutputDeviceChange001) {
  change_callback_->isCommunication_ = false;
  auto reason = 0;
  reason = (int32_t)AudioAdapterDeviceChangeReason::OLD_DEVICE_UNAVALIABLE;
  change_callback_->OnOutputDeviceChange(8);
  EXPECT_EQ(change_callback_->isCommunication_, false);
}

TEST_F(AudioOutputChangeCallbackTest, OnOutputDeviceChange002) {
  change_callback_->isCommunication_ = true;
  auto reason = 0;
  reason = (int32_t)AudioAdapterDeviceChangeReason::OLD_DEVICE_UNAVALIABLE;
  change_callback_->OnOutputDeviceChange(8);
  EXPECT_EQ(reason,
            (int32_t)AudioAdapterDeviceChangeReason::OLD_DEVICE_UNAVALIABLE);
}

TEST_F(AudioOutputChangeCallbackTest, OnOutputDeviceChange003) {
  change_callback_->isCommunication_ = true;
  auto reason = 0;
  reason = (int32_t)AudioAdapterDeviceChangeReason::UNKNOWN;
  change_callback_->OnOutputDeviceChange(8);
  EXPECT_EQ(change_callback_->isCommunication_, true);
}

TEST_F(AudioOutputChangeCallbackTest, OnOutputDeviceChange004) {
  change_callback_->isCommunication_ = false;
  auto reason = 0;
  reason = (int32_t)AudioAdapterDeviceChangeReason::UNKNOWN;
  change_callback_->OnOutputDeviceChange(8);
  EXPECT_EQ(reason, (int32_t)AudioAdapterDeviceChangeReason::UNKNOWN);
}

TEST_F(AudioOutputChangeCallbackTest, OnOutputDeviceChange005) {
  change_callback_->OnOutputDeviceChange(2);
  change_callback_->isCommunication_ = true;
  ASSERT_EQ(change_callback_->main_task_runner_, nullptr);
}

TEST_F(AudioRendererOptionsTest, AudioRendererOptionsTest_GetSamplingRateTest) {
  AudioAdapterSamplingRate rate = AudioAdapterSamplingRate::SAMPLE_RATE_8000;
  options_->rate_ = rate;
  auto ret = options_->GetSamplingRate();
  EXPECT_EQ(ret, rate);
}

TEST_F(AudioRendererOptionsTest, GetEncodingTypeTest) {
  AudioAdapterEncodingType encoding = AudioAdapterEncodingType::ENCODING_PCM;
  options_->encoding_ = encoding;
  auto ret = options_->GetEncodingType();
  EXPECT_EQ(ret, encoding);
}

TEST_F(AudioRendererOptionsTest, GetSampleFormatTest) {
  AudioAdapterSampleFormat forma = AudioAdapterSampleFormat::SAMPLE_S16LE;
  options_->format_ = forma;
  auto ret = options_->GetSampleFormat();
  EXPECT_EQ(ret, forma);
}

TEST_F(AudioRendererOptionsTest, GetChannelTest) {
  AudioAdapterChannel channels = AudioAdapterChannel::CHANNEL_3;
  options_->channels_ = channels;
  auto ret = options_->GetChannel();
  EXPECT_EQ(ret, channels);
}

TEST_F(AudioRendererOptionsTest, GetStreamUsageTest) {
  AudioAdapterStreamUsage stream_usage =
      AudioAdapterStreamUsage::STREAM_USAGE_VOICE_COMMUNICATION;
  options_->stream_usage_ = stream_usage;
  auto ret = options_->GetStreamUsage();
  EXPECT_EQ(ret, stream_usage);
}

TEST_F(AudioRendererOptionsTest, GetRenderFlagsTest) {
  int32_t renderer_flags = 0;
  options_->renderer_flags_ = renderer_flags;
  auto ret = options_->GetRenderFlags();
  EXPECT_EQ(ret, renderer_flags);
}

TEST_F(AudioRendererCallbackTest, OnSuspend001) {
  auto old_intterval = render_callback_->intervalSinceLastSuspend_;
  render_callback_->media_session_ = nullptr;
  render_callback_->OnSuspend();
  EXPECT_EQ(render_callback_->intervalSinceLastSuspend_, old_intterval);
}

TEST_F(AudioRendererCallbackTest, OnResume001) {
  render_callback_->media_session_ = nullptr;
  render_callback_->OnResume();
  EXPECT_EQ(render_callback_->media_session_, nullptr);
}

TEST_F(AudioRendererCallbackTest, GetSuspendFlag) {
  auto ret = render_callback_->GetSuspendFlag();
  EXPECT_EQ(ret, false);
}

TEST_F(AudioRendererCallbackTest, SetSuspendFlag) {
  render_callback_->SetSuspendFlag(false);
  EXPECT_EQ(render_callback_->suspendFlag_, false);
}

TEST_F(AudioRendererCallbackTest, OnSuspend01) {
  testing::internal::CaptureStderr();
  std::string log_output1 = testing::internal::GetCapturedStderr();
  render_callback_->OnSuspend();
  EXPECT_EQ(log_output1.find("AudioRendererCallback::OnSuspend"),
            std::string::npos);
}
TEST_F(AudioRendererCallbackTest, OnSuspend02) {
  testing::internal::CaptureStderr();
  std::string log_output1 = testing::internal::GetCapturedStderr();
  render_callback_->OnSuspend();
  EXPECT_EQ(log_output1.find(
                "AudioRendererCallback::OnSuspend media_session_ is null."),
            std::string::npos);
}

TEST_F(AudioRendererCallbackTest, OnResume01) {
  testing::internal::CaptureStderr();
  std::string log_output1 = testing::internal::GetCapturedStderr();
  render_callback_->OnResume();
  EXPECT_EQ(log_output1.find(
                "AudioRendererCallback::OnResume audioResumeInterval is"),
            std::string::npos);
}

TEST_F(AudioRendererCallbackTest, OnResume02) {
  testing::internal::CaptureStderr();
  std::string log_output1 = testing::internal::GetCapturedStderr();
  render_callback_->OnResume();
  EXPECT_EQ(log_output1.find(
                "AudioRendererCallback::OnResume media_session_ is null."),
            std::string::npos);
}

}  // namespace media
