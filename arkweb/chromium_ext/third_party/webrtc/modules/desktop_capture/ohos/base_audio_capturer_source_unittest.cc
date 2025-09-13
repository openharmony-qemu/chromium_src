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

#include "arkweb/ohos_adapter_ndk/interfaces/screen_capture_adapter.h"
#include "arkweb/ohos_adapter_ndk/screen_capture_adapter/screen_capture_adapter_impl.h"
#include "base/task/single_thread_task_runner.h"
#include "base/test/task_environment.h"
#include "media/base/audio_capturer_source.h"
#include "media/base/audio_parameters.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/audio_buffer_adapter_impl.h"
#define private public
#include "arkweb/chromium_ext/third_party/webrtc/modules/desktop_capture/ohos/base_audio_capturer_source.h"
#undef private

namespace webrtc {
class OHOSScreenCaptureCallbackMock : public ScreenCaptureCallbackAdapter {
 public:
  OHOSScreenCaptureCallbackMock() = default;
  ~OHOSScreenCaptureCallbackMock() override = default;
  void OnError(int32_t errorCode) override {}
  void OnAudioBufferAvailable(bool isReady,
                              AudioCaptureSourceTypeAdapter type) override {}
  void OnVideoBufferAvailable(bool isReady) override {}
  void OnStateChange(ScreenCaptureStateCodeAdapter stateCode) override {}
  void OnAudioBufferAvailableV2(bool isReady,
                                AudioCaptureSourceTypeAdapter type,
                                int32_t nweb_id) override {}
  void OnVideoBufferAvailableV2(bool isReady, int32_t nweb_id) override {}
  void OnStateChangeV2(ScreenCaptureStateCodeAdapter stateCode,
                       int32_t nweb_id) override {}
};

class MockCaptureCallback : public media::AudioCapturerSource::CaptureCallback {
 public:
  MOCK_METHOD(void, Capture, (const media::AudioBus* audio_bus, base::TimeTicks audio_capture_time,
              const media::AudioGlitchInfo& audio_glitch_info, double volume,
              bool key_pressed), (override));
  MOCK_METHOD(void, OnCaptureError, (media::AudioCapturerSource::ErrorCode code,
              const std::string& message), (override));
  MOCK_METHOD(void, OnCaptureMuted, (bool is_muted), (override));
  MOCK_METHOD(void, OnCaptureStarted, (), ());
};

class BaseAudioCapturerSourceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    capturer_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
    source_ =
        std::make_unique<BaseAudioCapturerSource>(capturer_task_runner_, 123);
  }

  void TearDown() override { source_.reset(); }

  void InitLowLatency() {
    media::AudioParameters params(
        media::AudioParameters::AUDIO_PCM_LOW_LATENCY,
        media::ChannelLayoutConfig(media::CHANNEL_LAYOUT_STEREO, 2), 48000,
        960);
    source_->Initialize(params, &mock_callback_);
  }

  void StartCaptureAndSource() {
    ASSERT_TRUE(BaseScreenCaptureSource::GetInstance().SetScreenCaptureConfig(
        source_->nwebId_));
    ASSERT_EQ(
        BaseScreenCaptureSource::GetInstance().StartCapture(source_->nwebId_),
        0);
    ASSERT_NO_FATAL_FAILURE(source_->Start());
  }

  scoped_refptr<base::SingleThreadTaskRunner> capturer_task_runner_;
  base::test::TaskEnvironment task_environment_;
  std::unique_ptr<BaseAudioCapturerSource> source_;
  testing::NiceMock<MockCaptureCallback> mock_callback_;
};

TEST_F(BaseAudioCapturerSourceTest, Initialize1) {
  media::AudioParameters params(
      media::AudioParameters::AUDIO_PCM_LOW_LATENCY,
      media::ChannelLayoutConfig(media::CHANNEL_LAYOUT_STEREO, 2), 48000, 960);
  ASSERT_NO_FATAL_FAILURE(source_->Initialize(params, &mock_callback_));
  EXPECT_FALSE(source_->portal_init_failed_);
  EXPECT_EQ(source_->params_.format(),
            media::AudioParameters::AUDIO_PCM_LOW_LATENCY);
  EXPECT_EQ(source_->base_callback_, &mock_callback_);
}

TEST_F(BaseAudioCapturerSourceTest, Initialize2) {
  media::AudioParameters params(
      media::AudioParameters::AUDIO_PCM_LINEAR,
      media::ChannelLayoutConfig(media::CHANNEL_LAYOUT_MONO, 1), 44100, 1024);
  ASSERT_NO_FATAL_FAILURE(source_->Initialize(params, &mock_callback_));

  EXPECT_TRUE(source_->portal_init_failed_);
  EXPECT_EQ(source_->params_.format(),
            media::AudioParameters::AUDIO_PCM_LINEAR);
  EXPECT_EQ(source_->base_callback_, &mock_callback_);
}

TEST_F(BaseAudioCapturerSourceTest, Start1) {
  media::AudioParameters params(
      media::AudioParameters::AUDIO_PCM_LINEAR,
      media::ChannelLayoutConfig(media::CHANNEL_LAYOUT_MONO, 1), 44100, 1024);
  source_->Initialize(params, &mock_callback_);
  EXPECT_TRUE(source_->portal_init_failed_);
  ASSERT_NO_FATAL_FAILURE(source_->Start());
  EXPECT_FALSE(source_->isStart_);
  EXPECT_EQ(source_->AudioScreenCapturerReadCallback_, nullptr);
}

TEST_F(BaseAudioCapturerSourceTest, Start2) {
  InitLowLatency();
  EXPECT_FALSE(source_->portal_init_failed_);
  source_->isStart_ = true;
  ASSERT_NO_FATAL_FAILURE(source_->Start());
  EXPECT_TRUE(source_->isStart_);
}

TEST_F(BaseAudioCapturerSourceTest, Start3) {
  InitLowLatency();
  EXPECT_FALSE(source_->portal_init_failed_);
  EXPECT_FALSE(source_->isStart_);
  ASSERT_NO_FATAL_FAILURE(source_->Start());
  EXPECT_TRUE(source_->isStart_);
  EXPECT_NE(source_->AudioScreenCapturerReadCallback_, nullptr);
}

TEST_F(BaseAudioCapturerSourceTest, Stop1) {
  InitLowLatency();
  source_->Start();
  EXPECT_NE(source_->base_callback_, nullptr);
  ASSERT_NO_FATAL_FAILURE(source_->Stop());
  EXPECT_EQ(source_->base_callback_, nullptr);
}

TEST_F(BaseAudioCapturerSourceTest, Stop2) {
  InitLowLatency();
  source_->base_callback_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(source_->Stop());
  EXPECT_FALSE(source_->isStart_);
}

TEST_F(BaseAudioCapturerSourceTest, HandleAudioBuffer1) {
  source_->portal_init_failed_ = true;
  ASSERT_NO_FATAL_FAILURE(source_->HandleAudioBuffer(
      OHOS::NWeb::AudioCaptureSourceTypeAdapter::MIC));
}

TEST_F(BaseAudioCapturerSourceTest, HandleAudioBuffer2) {
  InitLowLatency();
  source_->portal_init_failed_ = false;
  ASSERT_NO_FATAL_FAILURE(source_->HandleAudioBuffer(
      OHOS::NWeb::AudioCaptureSourceTypeAdapter::MIC));
}

TEST_F(BaseAudioCapturerSourceTest, HandleAudioBuffer3) {
  InitLowLatency();
  source_->portal_init_failed_ = false;
  StartCaptureAndSource();
  OH_AVBuffer* avBuffer = OH_AVBuffer_Create(1024);
  EXPECT_NE(avBuffer, nullptr);
  CallbackInfo cb;
  cb.callback = std::make_shared<OHOSScreenCaptureCallbackMock>();
  cb.nweb_id = source_->nwebId_;
  void* userData = static_cast<void*>(&cb);
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnBufferAvailable(
      nullptr, avBuffer,
      OH_AVScreenCaptureBufferType::OH_SCREEN_CAPTURE_BUFFERTYPE_AUDIO_INNER, 0,
      userData);
  ASSERT_NO_FATAL_FAILURE(source_->HandleAudioBuffer(
      OHOS::NWeb::AudioCaptureSourceTypeAdapter::MIC));
  ASSERT_NO_FATAL_FAILURE(source_->Stop());
  ASSERT_EQ(
      BaseScreenCaptureSource::GetInstance().StopCapture(source_->nwebId_), 0);
  OH_AVBuffer_Destroy(avBuffer);
}

TEST_F(BaseAudioCapturerSourceTest, HandleAudioBuffer4) {
  InitLowLatency();
  source_->portal_init_failed_ = false;
  StartCaptureAndSource();
  OH_AVBuffer* avBuffer = OH_AVBuffer_Create(1024);
  ASSERT_NE(avBuffer, nullptr);
  CallbackInfo cb;
  cb.callback = std::make_shared<OHOSScreenCaptureCallbackMock>();
  cb.nweb_id = source_->nwebId_;
  void* userData = static_cast<void*>(&cb);
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnBufferAvailable(
      nullptr, avBuffer,
      OH_AVScreenCaptureBufferType::OH_SCREEN_CAPTURE_BUFFERTYPE_AUDIO_INNER, 0,
      userData);
  ASSERT_NO_FATAL_FAILURE(source_->HandleAudioBuffer(
      OHOS::NWeb::AudioCaptureSourceTypeAdapter::ALL_PLAYBACK));
  ASSERT_NO_FATAL_FAILURE(source_->Stop());
  ASSERT_EQ(
      BaseScreenCaptureSource::GetInstance().StopCapture(source_->nwebId_), 0);
  OH_AVBuffer_Destroy(avBuffer);
}

TEST_F(BaseAudioCapturerSourceTest, HandleAudioBuffer5) {
  InitLowLatency();
  source_->portal_init_failed_ = false;
  StartCaptureAndSource();
  source_->base_callback_ = nullptr;
  OH_AVBuffer* avBuffer = OH_AVBuffer_Create(1024);
  ASSERT_NE(avBuffer, nullptr);
  CallbackInfo cb;
  cb.callback = std::make_shared<OHOSScreenCaptureCallbackMock>();
  cb.nweb_id = source_->nwebId_;
  void* userData = static_cast<void*>(&cb);
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnBufferAvailable(
      nullptr, avBuffer,
      OH_AVScreenCaptureBufferType::OH_SCREEN_CAPTURE_BUFFERTYPE_AUDIO_INNER, 0,
      userData);
  ASSERT_NO_FATAL_FAILURE(source_->HandleAudioBuffer(
      OHOS::NWeb::AudioCaptureSourceTypeAdapter::ALL_PLAYBACK));
  ASSERT_NO_FATAL_FAILURE(source_->Stop());
  ASSERT_EQ(
      BaseScreenCaptureSource::GetInstance().StopCapture(source_->nwebId_), 0);
  OH_AVBuffer_Destroy(avBuffer);
}

TEST_F(BaseAudioCapturerSourceTest, NotifyCaptureError1) {
  InitLowLatency();
  source_->base_callback_ = nullptr;
  EXPECT_CALL(mock_callback_,
              OnCaptureError(media::AudioCapturerSource::ErrorCode::kUnknown,
                             "Test error message"))
      .Times(0);
  EXPECT_CALL(mock_callback_, OnCaptureStarted()).Times(0);
  ASSERT_NO_FATAL_FAILURE(source_->NotifyCaptureError("Test error message"));
  ASSERT_NO_FATAL_FAILURE(source_->NotifyCaptureStarted());
}

TEST_F(BaseAudioCapturerSourceTest, NotifyCaptureError2) {
  InitLowLatency();
  EXPECT_CALL(mock_callback_,
              OnCaptureError(media::AudioCapturerSource::ErrorCode::kUnknown,
                             "Test error message"))
      .Times(1);
  EXPECT_CALL(mock_callback_, OnCaptureStarted()).Times(1);
  ASSERT_NO_FATAL_FAILURE(source_->NotifyCaptureError("Test error message"));
  ASSERT_NO_FATAL_FAILURE(source_->NotifyCaptureStarted());
}

}  // namespace webrtc