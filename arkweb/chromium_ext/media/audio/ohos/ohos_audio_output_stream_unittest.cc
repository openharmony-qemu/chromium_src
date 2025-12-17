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

#include "ohos_audio_output_stream.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <utility>

#include "base/test/task_environment.h"
#include "base/test/test_mock_time_task_runner.h"
#include "content/browser/media/session/media_session_impl.h"
#include "content/browser/media/session/mock_media_session_player_observer.h"
#include "content/browser/scheduler/browser_io_thread_delegate.h"
#include "content/browser/scheduler/browser_task_executor.h"
#include "content/browser/scheduler/browser_task_priority.h"
#include "content/browser/scheduler/browser_ui_thread_scheduler.h"
#include "content/public/browser/web_contents.h"
#include "content/public/test/browser_task_environment.h"
#include "content/public/test/test_renderer_host.h"
#include "content/test/test_web_contents.h"
#include "media/audio/audio_io.h"
#include "media/audio/audio_thread.h"
#include "media/base/audio_parameters.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/init/gl_factory.h"

using namespace testing;
using namespace content;

namespace media {

class MockAudioSourceCallback
    : public ::media::AudioOutputStream::AudioSourceCallback {
 public:
  MockAudioSourceCallback() {
    ON_CALL(*this, OnMoreData(_, _, _, _))
        .WillByDefault(Invoke(this, &MockAudioSourceCallback::OnMoreDataImpl));
  }

  MOCK_METHOD4(OnMoreData,
               int(base::TimeDelta,
                   base::TimeTicks,
                   const ::media::AudioGlitchInfo&,
                   ::media::AudioBus*));
  MOCK_METHOD1(OnError, void(ErrorType));

 private:
  int OnMoreDataImpl(base::TimeDelta /* delay */,
                     base::TimeTicks /* delay_timestamp */,
                     const ::media::AudioGlitchInfo& /* glitch_info */,
                     ::media::AudioBus* dest) {
    dest->Zero();
    return dest->frames();
  }
};

class OHOSAudioOutputStreamTest : public content::RenderViewHostTestHarness {
 public:
  OHOSAudioOutputStreamTest() : content::RenderViewHostTestHarness() {}
  OHOSAudioOutputStreamTest(const OHOSAudioOutputStreamTest&) = delete;
  OHOSAudioOutputStreamTest& operator=(const OHOSAudioOutputStreamTest&) =
      delete;
  void SetUp() override {
    CreateOutputStream();
    content::RenderViewHostTestHarness::SetUp();
  }

  void TearDown() override {
    stream_.reset();
    content::RenderViewHostTestHarness::TearDown();
  }

 public:
  void CreateOutputStream() {
    params_ = AudioParameters(
        AudioParameters::Format::AUDIO_PCM_LINEAR,
        ChannelLayoutConfig::FromLayout<CHANNEL_LAYOUT_MONO>(), 8000, 160);
    stream_ = std::make_unique<OHOSAudioOutputStream>(nullptr, params_, false);
    ASSERT_NE(stream_, nullptr);
  }

  content::MediaSessionImpl* GetMediaSession() {
    if (!stream_) {
      return nullptr;
    }
    auto frameHost = main_rfh();
    if (!frameHost) {
      return nullptr;
    }
    stream_->parameters_.set_render_process_id(
        frameHost->GetProcess()->GetID());
    stream_->parameters_.set_render_frame_id(frameHost->GetRoutingID());
    content::RenderFrameHost* renderFrameHost =
        content::RenderFrameHost::FromID(
            stream_->parameters_.render_process_id(),
            stream_->parameters_.render_frame_id());
    if (!renderFrameHost) {
      return nullptr;
    }
    content::WebContents* webContents =
        content::WebContents::FromRenderFrameHost(renderFrameHost);
    if (!webContents) {
      return nullptr;
    }
    content::MediaSessionImpl* mediaSession =
        content::MediaSessionImpl::Get(webContents);
    return mediaSession;
  }

  void SetSessionFocusState(
      content::MediaSessionImpl::State audio_focus_state) {
    content::MediaSessionImpl* mediaSession = GetMediaSession();
    ASSERT_NE(mediaSession, nullptr);
    mediaSession->SetAudioFocusState(audio_focus_state);
  }

  void SetSessionType(int audioSessionType) {
    content::MediaSessionImpl* mediaSession = GetMediaSession();
    ASSERT_NE(mediaSession, nullptr);
    mediaSession->audioSessionType_ = audioSessionType;
  }

  void SetContentType(media::MediaContentType media_content_type) {
    content::MediaSessionImpl* mediaSession = GetMediaSession();
    ASSERT_NE(mediaSession, nullptr);
    mediaSession->SetMediaContentType(media_content_type);
  }

  void SetPlayingState(bool playingState) {
    content::MediaSessionImpl* mediaSession = GetMediaSession();
    ASSERT_NE(mediaSession, nullptr);
    mediaSession->SetPlayingState(playingState);
  }

  void SetMuted(bool muted) {
    content::MediaSessionImpl* mediaSession = GetMediaSession();
    ASSERT_NE(mediaSession, nullptr);
    mediaSession->OnMediaMutedStatusChanged(muted);
  }

  void SetHasOneSHotPlayers(bool has_one_shot_players) {
    content::MediaSessionImpl* mediaSession = GetMediaSession();
    ASSERT_NE(mediaSession, nullptr);
    mediaSession->has_one_shot_players_ = has_one_shot_players;
  }

  void SetSessionState(
      content::MediaSessionImpl::NWebMediaSessionState sessionState) {
    content::MediaSessionImpl* mediaSession = GetMediaSession();
    ASSERT_NE(mediaSession, nullptr);
    mediaSession->SetSessionState(sessionState);
  }

 protected:
  AudioParameters params_;
  std::unique_ptr<OHOSAudioOutputStream> stream_;
};

TEST_F(OHOSAudioOutputStreamTest, Open001) {
  ASSERT_NE(stream_, nullptr);
  stream_->audio_stream_builder_ = nullptr;
  EXPECT_FALSE(stream_->Open());
}

TEST_F(OHOSAudioOutputStreamTest, Open002) {
  ASSERT_NE(stream_, nullptr);
  EXPECT_TRUE(stream_->Open());
}

TEST_F(OHOSAudioOutputStreamTest, AudioRendererOnWriteData001) {
  uint8_t* buffer = new uint8_t[10];
  buffer[0] = 97;
  std::shared_ptr<OHOSAudioOutputCallback> audioOutputCallback =
      std::make_shared<OHOSAudioOutputCallback>(
          stream_->weak_factory_.GetWeakPtr());

  stream_->callback_index_ =
      stream_->callback_wrapper_.AddCallback(audioOutputCallback);
  EXPECT_EQ(OHOSAudioOutputStream::AudioRendererOnWriteData(nullptr, nullptr,
                                                            nullptr, 0),
            0);
  EXPECT_EQ(OHOSAudioOutputStream::AudioRendererOnWriteData(
                nullptr, reinterpret_cast<void*>(stream_->callback_index_),
                nullptr, 0),
            0);

  EXPECT_EQ(OHOSAudioOutputStream::AudioRendererOnWriteData(nullptr, nullptr,
                                                            buffer, 0),
            0);
  EXPECT_EQ(OHOSAudioOutputStream::AudioRendererOnWriteData(
                nullptr, reinterpret_cast<void*>(stream_->callback_index_),
                buffer, 1),
            0);
  stream_->callback_wrapper_.Clear(stream_->callback_index_);
  delete[] buffer;
}

TEST_F(OHOSAudioOutputStreamTest, AudioRendererOnWriteData002) {
  uint8_t* buffer = new uint8_t[10];
  buffer[0] = 97;

  const size_t testCallbackIndex = 11;  // 11 for test,invalid index

  EXPECT_EQ(OHOSAudioOutputStream::AudioRendererOnWriteData(
                nullptr, reinterpret_cast<void*>(testCallbackIndex), buffer, 1),
            0);
  delete[] buffer;
}

TEST_F(OHOSAudioOutputStreamTest, AudioRendererOnError001) {
  const size_t testCallbackIndex = 11;  // 11 for test,invalid index
  EXPECT_EQ(OHOSAudioOutputStream::AudioRendererOnError(
                nullptr, reinterpret_cast<void*>(testCallbackIndex),
                OH_AudioStream_Result::AUDIOSTREAM_SUCCESS),
            0);
  EXPECT_EQ(OHOSAudioOutputStream::AudioRendererOnError(
                nullptr, nullptr, OH_AudioStream_Result::AUDIOSTREAM_SUCCESS),
            0);
}

TEST_F(OHOSAudioOutputStreamTest, AudioRendererOnError002) {
  uint8_t* buffer = new uint8_t[10];
  buffer[0] = 97;
  std::shared_ptr<OHOSAudioOutputCallback> audioOutputCallback =
      std::make_shared<OHOSAudioOutputCallback>(
          stream_->weak_factory_.GetWeakPtr());

  stream_->callback_index_ =
      stream_->callback_wrapper_.AddCallback(audioOutputCallback);
  EXPECT_EQ(OHOSAudioOutputStream::AudioRendererOnError(
                nullptr, reinterpret_cast<void*>(stream_->callback_index_),
                OH_AudioStream_Result::AUDIOSTREAM_SUCCESS),
            0);
  delete[] buffer;
  stream_->callback_wrapper_.Clear(stream_->callback_index_);
}

TEST_F(OHOSAudioOutputStreamTest, AudioRendererOnInterruptEvent001) {
  std::shared_ptr<OHOSAudioOutputCallback> audioOutputCallback =
      std::make_shared<OHOSAudioOutputCallback>(
          stream_->weak_factory_.GetWeakPtr());

  stream_->callback_index_ =
      stream_->callback_wrapper_.AddCallback(audioOutputCallback);
  EXPECT_EQ(OHOSAudioOutputStream::AudioRendererOnInterruptEvent(
                nullptr, reinterpret_cast<void*>(stream_->callback_index_),
                OH_AudioInterrupt_ForceType::AUDIOSTREAM_INTERRUPT_FORCE,
                OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_NONE),
            0);
  EXPECT_EQ(OHOSAudioOutputStream::AudioRendererOnInterruptEvent(
                nullptr, reinterpret_cast<void*>(stream_->callback_index_),
                OH_AudioInterrupt_ForceType::AUDIOSTREAM_INTERRUPT_FORCE,
                OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_RESUME),
            0);

  EXPECT_EQ(OHOSAudioOutputStream::AudioRendererOnInterruptEvent(
                nullptr, reinterpret_cast<void*>(stream_->callback_index_),
                OH_AudioInterrupt_ForceType::AUDIOSTREAM_INTERRUPT_FORCE,
                OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_STOP),
            0);

  EXPECT_EQ(OHOSAudioOutputStream::AudioRendererOnInterruptEvent(
                nullptr, reinterpret_cast<void*>(stream_->callback_index_),
                OH_AudioInterrupt_ForceType::AUDIOSTREAM_INTERRUPT_FORCE,
                OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_DUCK),
            0);

  EXPECT_EQ(OHOSAudioOutputStream::AudioRendererOnInterruptEvent(
                nullptr, reinterpret_cast<void*>(stream_->callback_index_),
                OH_AudioInterrupt_ForceType::AUDIOSTREAM_INTERRUPT_FORCE,
                OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_UNDUCK),
            0);
  stream_->callback_wrapper_.Clear(stream_->callback_index_);
}

TEST_F(OHOSAudioOutputStreamTest, AudioRendererOnInterruptEvent002) {
  EXPECT_EQ(OHOSAudioOutputStream::AudioRendererOnInterruptEvent(
                nullptr, nullptr,
                OH_AudioInterrupt_ForceType::AUDIOSTREAM_INTERRUPT_FORCE,
                OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_NONE),
            0);
}

TEST_F(OHOSAudioOutputStreamTest, AudioRendererOnInterruptEvent003) {
  const size_t testCallbackIndex = 11;  // 11 for test,invalid index
  EXPECT_EQ(OHOSAudioOutputStream::AudioRendererOnInterruptEvent(
                nullptr, reinterpret_cast<void*>(testCallbackIndex),
                OH_AudioInterrupt_ForceType::AUDIOSTREAM_INTERRUPT_FORCE,
                OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_NONE),
            0);
}

TEST_F(OHOSAudioOutputStreamTest, AudioRendererOutputDeviceChangeCallback001) {
  testing::internal::CaptureStderr();
  OHOSAudioOutputStream::AudioRendererOutputDeviceChangeCallback(
      nullptr, nullptr, OH_AudioStream_DeviceChangeReason::REASON_UNKNOWN);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find(
                "AudioRendererOutputDeviceChangeCallback callback is nullptr"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, AudioRendererOutputDeviceChangeCallback002) {
  testing::internal::CaptureStderr();
  std::shared_ptr<OHOSAudioOutputCallback> audioOutputCallback =
      std::make_shared<OHOSAudioOutputCallback>(
          stream_->weak_factory_.GetWeakPtr());

  stream_->callback_index_ =
      stream_->callback_wrapper_.AddCallback(audioOutputCallback);
  OHOSAudioOutputStream::AudioRendererOutputDeviceChangeCallback(
      nullptr, reinterpret_cast<void*>(stream_->callback_index_),
      OH_AudioStream_DeviceChangeReason::REASON_UNKNOWN);
  OHOSAudioOutputStream::AudioRendererOutputDeviceChangeCallback(
      nullptr, reinterpret_cast<void*>(stream_->callback_index_),
      OH_AudioStream_DeviceChangeReason::REASON_NEW_DEVICE_AVAILABLE);
  OHOSAudioOutputStream::AudioRendererOutputDeviceChangeCallback(
      nullptr, reinterpret_cast<void*>(stream_->callback_index_),
      OH_AudioStream_DeviceChangeReason::REASON_OVERRODE);
  OHOSAudioOutputStream::AudioRendererOutputDeviceChangeCallback(
      nullptr, reinterpret_cast<void*>(stream_->callback_index_),
      OH_AudioStream_DeviceChangeReason::REASON_OLD_DEVICE_UNAVAILABLE);
  std::string log_output = testing::internal::GetCapturedStderr();
  stream_->callback_wrapper_.Clear(stream_->callback_index_);
  EXPECT_EQ(log_output.find(
                "AudioRendererOutputDeviceChangeCallback callback is nullptr"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, AudioRendererOutputDeviceChangeCallback003) {
  testing::internal::CaptureStderr();
  const size_t testCallbackIndex = 11;  // 11 for test,invalid index
  OHOSAudioOutputStream::AudioRendererOutputDeviceChangeCallback(
      nullptr, reinterpret_cast<void*>(testCallbackIndex),
      OH_AudioStream_DeviceChangeReason::REASON_OLD_DEVICE_UNAVAILABLE);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find(
                "AudioRendererOutputDeviceChangeCallback callback is nullptr"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OnSuspend001) {
  testing::internal::CaptureStderr();
  ASSERT_NE(stream_, nullptr);
  stream_->Open();
  stream_->isDestroyed_ = true;
  stream_->OnSuspend();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(
      log_output.find("OHOSAudioOutputStream::OnSuspend during destroyed"),
      std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OnSuspend002) {
  testing::internal::CaptureStderr();
  ASSERT_NE(stream_, nullptr);
  stream_->Open();
  stream_->isDestroyed_ = false;
  stream_->parameters_.set_sample_rate(0);
  stream_->OnSuspend();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find(
                "OHOSAudioOutputStream::OnSuspend parameters_ is not valid."),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OnSuspend003) {
  testing::internal::CaptureStderr();
  ASSERT_NE(stream_, nullptr);
  stream_->Open();
  stream_->isDestroyed_ = false;
  stream_->running_ = false;
  stream_->OnSuspend();

  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("The playback is stopped. Exit OnSuspend."),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OnSuspend004) {
  ASSERT_NE(stream_, nullptr);
  stream_->Open();
  stream_->isDestroyed_ = false;
  stream_->running_ = true;
  SetSessionFocusState(content::MediaSessionImpl::State::ACTIVE);
  stream_->intervalSinceLastSuspend_ = 0;
  stream_->audioResumeInterval_ = 0;
  stream_->OnSuspend();
  EXPECT_EQ(stream_->intervalSinceLastSuspend_, 0);
  stream_->audioResumeInterval_ = 10;
  stream_->OnSuspend();
  EXPECT_NE(stream_->intervalSinceLastSuspend_, 0);
}

TEST_F(OHOSAudioOutputStreamTest, OnSuspend005) {
  ASSERT_NE(stream_, nullptr);
  stream_->Open();
  stream_->isDestroyed_ = false;
  stream_->running_ = true;
  SetSessionFocusState(content::MediaSessionImpl::State::ACTIVE);
  stream_->main_task_runner_ = nullptr;
  testing::internal::CaptureStderr();
  stream_->OnSuspend();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("main task runner is nullptr"), std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OnSuspend006) {
  testing::internal::CaptureStderr();
  ASSERT_NE(stream_, nullptr);
  stream_->Open();
  stream_->isDestroyed_ = false;
  stream_->running_ = true;
  SetSessionFocusState(content::MediaSessionImpl::State::SUSPENDED);
  auto observer =
      new MockMediaSessionPlayerObserver(media::MediaContentType::kPersistent);
  int player_id = 1;
  const content::MediaSessionImpl::PlayerIdentifier identifier(observer,
                                                               player_id);
  GetMediaSession()->one_shot_players_.insert(identifier);
  stream_->OnSuspend();
  GetMediaSession()->one_shot_players_.erase(identifier);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("media session is not active"), std::string::npos);
  delete observer;
}

TEST_F(OHOSAudioOutputStreamTest, OnSuspend007) {
  testing::internal::CaptureStderr();
  ASSERT_NE(stream_, nullptr);
  stream_->Open();
  stream_->isDestroyed_ = false;
  stream_->running_ = true;
  SetSessionFocusState(content::MediaSessionImpl::State::SUSPENDED);
  stream_->OnSuspend();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("media session is not active"), std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OneShotMediaPlayerStopped001) {
  testing::internal::CaptureStderr();
  auto params = AudioParameters(
    AudioParameters::Format::AUDIO_PCM_LINEAR,
    ChannelLayoutConfig::FromLayout<CHANNEL_LAYOUT_MONO>(), 8000, 160);
  stream_->OneShotMediaPlayerStopped(params);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("main_task_runner is nullptr"), std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OnResume001) {
  testing::internal::CaptureStderr();
  ASSERT_NE(stream_, nullptr);
  stream_->Open();
  stream_->isDestroyed_ = true;
  stream_->OnResume();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OHOSAudioOutputStream::OnResume during destroyed"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OnResume002) {
  testing::internal::CaptureStderr();
  stream_->Open();
  stream_->isDestroyed_ = false;
  stream_->parameters_.set_sample_rate(0);
  stream_->OnResume();

  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find(
                "OHOSAudioOutputStream::OnResume parameters_ is not valid"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OnResume003) {
  testing::internal::CaptureStderr();
  ASSERT_NE(stream_, nullptr);
  stream_->Open();
  stream_->isDestroyed_ = false;
  stream_->audioResumeInterval_ = -1;
  SetSessionFocusState(content::MediaSessionImpl::State::SUSPENDED);
  stream_->main_task_runner_ = nullptr;
  stream_->OnResume();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find(
                "OHOSAudioOutputStream::OnResume main task runner is nullptr"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OnResume004) {
  testing::internal::CaptureStderr();
  ASSERT_NE(stream_, nullptr);
  stream_->Open();
  stream_->isDestroyed_ = false;
  stream_->audioResumeInterval_ = -1;
  SetSessionFocusState(content::MediaSessionImpl::State::SUSPENDED);
  stream_->OnResume();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find(
                "OHOSAudioOutputStream::OnResume main task runner is nullptr"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OnResume005) {
  testing::internal::CaptureStderr();
  ASSERT_NE(stream_, nullptr);
  stream_->Open();
  stream_->isDestroyed_ = false;
  stream_->audioResumeInterval_ = -1;
  SetSessionFocusState(content::MediaSessionImpl::State::ACTIVE);
  stream_->OnResume();
  stream_->audioResumeInterval_ = 0;
  stream_->OnResume();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find(
                "OHOSAudioOutputStream::OnResume main task runner is nullptr"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OldDeviceUnavailable001) {
  testing::internal::CaptureStderr();
  stream_->isCommunication_ = true;
  stream_->OldDeviceUnavailable();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OldDeviceUnavailable communication"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OldDeviceUnavailable002) {
  testing::internal::CaptureStderr();
  stream_->isCommunication_ = false;
  stream_->main_task_runner_ = nullptr;
  stream_->OldDeviceUnavailable();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("main_task_runner is nullptr"), std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OldDeviceUnavailable003) {
  testing::internal::CaptureStderr();
  stream_->isCommunication_ = false;
  stream_->Open();
  stream_->OldDeviceUnavailable();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("main_task_runner is nullptr"), std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OldDeviceUnavailable004) {
  testing::internal::CaptureStderr();
  stream_->isCommunication_ = false;
  stream_->Open();
  SetSessionFocusState(content::MediaSessionImpl::State::ACTIVE);
  stream_->OldDeviceUnavailable();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("MediaSession is suspending the audio"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OldDeviceUnavailable005) {
  testing::internal::CaptureStderr();
  stream_->isCommunication_ = false;
  stream_->Open();
  SetSessionFocusState(content::MediaSessionImpl::State::SUSPENDED);
  stream_->OldDeviceUnavailable();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("MediaSession is suspended"), std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, isNeedResume001) {
  EXPECT_TRUE(stream_->isNeedResume(-1));
  EXPECT_FALSE(stream_->isNeedResume(0));
  stream_->intervalSinceLastSuspend_ = 0;
  EXPECT_FALSE(stream_->isNeedResume(1));
}

TEST_F(OHOSAudioOutputStreamTest, isNeedResume002) {
  EXPECT_TRUE(stream_->isNeedResume(-1));
  EXPECT_FALSE(stream_->isNeedResume(0));
  stream_->intervalSinceLastSuspend_ = std::time(nullptr);
  EXPECT_TRUE(stream_->isNeedResume(1000000));
}

TEST_F(OHOSAudioOutputStreamTest, SuspendOtherMediaSession001) {
  testing::internal::CaptureStderr();
  stream_->audioExclusive_ = false;
  stream_->SuspendOtherMediaSession();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(
      log_output.find("MediaSession is suspending the audio in other web."),
      std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, SuspendOtherMediaSession002) {
  testing::internal::CaptureStderr();
  stream_->audioExclusive_ = true;
  stream_->parameters_.set_sample_rate(0);
  stream_->SuspendOtherMediaSession();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(
      log_output.find("MediaSession is suspending the audio in other web."),
      std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, SuspendOtherMediaSession003) {
  stream_->audioExclusive_ = true;
  stream_->InitRender();
  SetSessionState(content::MediaSessionImpl::NWebMediaSessionState::NONEED);
  SetHasOneSHotPlayers(false);
  SetSessionFocusState(content::MediaSessionImpl::State::SUSPENDED);
  SetPlayingState(true);
  SetMuted(true);
  testing::internal::CaptureStderr();
  stream_->SuspendOtherMediaSession();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(
      log_output.find("MediaSession is suspending the audio in other web."),
      std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, Start001) {
  MockAudioSourceCallback callback;
  stream_->Start(&callback);
  EXPECT_EQ(stream_->running_, false);
  stream_->Open();
  SetSessionFocusState(content::MediaSessionImpl::State::SUSPENDED);
  stream_->Start(&callback);
  EXPECT_EQ(stream_->running_, true);
  stream_->Stop();
}

TEST_F(OHOSAudioOutputStreamTest, Stop001) {
  stream_->main_task_runner_ = nullptr;
  stream_->Stop();
  EXPECT_EQ(stream_->running_, false);
}

TEST_F(OHOSAudioOutputStreamTest, Stop002) {
  testing::internal::CaptureStderr();
  stream_->audio_renderer_ = nullptr;
  stream_->Stop();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find(
                "OHOSAudioOutputStream::Stop. audio_renderer_ is nullptr"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, Stop003) {
  testing::internal::CaptureStderr();
  stream_->reference_time_ = base::TimeTicks::Now();
  EXPECT_TRUE(stream_->Open());
  stream_->Stop();
  EXPECT_TRUE(stream_->reference_time_.is_null());
  stream_->isSuspended_ = false;
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find(
                "OHOSAudioOutputStream::Stop stream is suspended, cannot stop"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, Stop004) {
  testing::internal::CaptureStderr();
  stream_->reference_time_ = base::TimeTicks();
  EXPECT_TRUE(stream_->Open());
  stream_->isSuspended_ = true;
  stream_->Stop();
  EXPECT_TRUE(stream_->reference_time_.is_null());
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find(
                "OHOSAudioOutputStream::Stop stream is suspended, cannot stop"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, StopTimer001) {
  stream_->running_ = true;
  stream_->SchedulePumpSamples();
  stream_->StopTimer();
  EXPECT_FALSE(stream_->timer_.IsRunning());
}

TEST_F(OHOSAudioOutputStreamTest, StopTimer002) {
  stream_->StopTimer();
  EXPECT_FALSE(stream_->timer_.IsRunning());
}

TEST_F(OHOSAudioOutputStreamTest, Refresh001) {
  testing::internal::CaptureStderr();
  EXPECT_TRUE(stream_->Open());
  stream_->Refresh();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(
      log_output.find("OHOSAudioOutputStream::Refresh audio_renderer_ is null"),
      std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, Refresh002) {
  testing::internal::CaptureStderr();
  stream_->audio_renderer_ = nullptr;
  stream_->Refresh();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(
      log_output.find("OHOSAudioOutputStream::Refresh audio_renderer_ is null"),
      std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, SetInterruptMode001) {
  testing::internal::CaptureStderr();
  EXPECT_TRUE(stream_->Open());
  stream_->SetInterruptMode(true);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(
      log_output.find(
          "OHOSAudioOutputStream::SetInterruptMode audio_renderer_ is null."),
      std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, SetInterruptMode002) {
  testing::internal::CaptureStderr();
  stream_->audio_renderer_ = nullptr;
  stream_->SetInterruptMode(true);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(
      log_output.find(
          "OHOSAudioOutputStream::SetInterruptMode audio_renderer_ is null."),
      std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, SetVolume001) {
  std::cout << "SetVolume001" << std::endl;
  ASSERT_NE(stream_, nullptr);
  std::cout << "SetVolume 1" << std::endl;
  stream_->SetVolume(1);
  double volume = 0;
  std::cout << "GetVolume" << std::endl;
  stream_->GetVolume(&volume);
  EXPECT_EQ(volume, 1);
  stream_->SetVolume(-1);
  EXPECT_EQ(volume, 1);
  stream_->SetVolume(2);
  EXPECT_EQ(volume, 1);
}

TEST_F(OHOSAudioOutputStreamTest, InitRender001) {
  testing::internal::CaptureStderr();
  EXPECT_TRUE(stream_->InitRender());
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find(
                "OHOSAudioOutputStream::InitRender, parameters_ is not valid"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, InitRender002) {
  testing::internal::CaptureStderr();
  std::shared_ptr<OHOSAudioOutputCallback> audioOutputCallback =
      std::make_shared<OHOSAudioOutputCallback>(
          stream_->weak_factory_.GetWeakPtr());

  stream_->callback_index_ =
      stream_->callback_wrapper_.AddCallback(audioOutputCallback);

  EXPECT_TRUE(stream_->InitRender());
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find(
                "OHOSAudioOutputStream::InitRender, parameters_ is not valid"),
            std::string::npos);
  stream_->callback_wrapper_.Clear(stream_->callback_index_);
}

TEST_F(OHOSAudioOutputStreamTest, InitRender003) {
  testing::internal::CaptureStderr();
  stream_->parameters_.set_sample_rate(0);
  EXPECT_FALSE(stream_->InitRender());
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find(
                "OHOSAudioOutputStream::InitRender, parameters_ is not valid"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, InitRender004) {
  testing::internal::CaptureStderr();
  stream_->audio_stream_builder_ = nullptr;
  EXPECT_FALSE(stream_->InitRender());
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find(
                "OHOSAudioOutputStream::InitRender, parameters_ is not valid"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, SetStreamUsage001) {
  testing::internal::CaptureStderr();
  stream_->isCommunication_ = true;
  stream_->SetStreamUsage();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("OHOSAudioOutputStream AudioSessionType: "),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, SetStreamUsage002) {
  testing::internal::CaptureStderr();
  stream_->isCommunication_ = false;
  stream_->SetStreamUsage();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OHOSAudioOutputStream AudioSessionType: "),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, SetStreamUsage003) {
  testing::internal::CaptureStderr();
  stream_->isCommunication_ = false;
  SetSessionType(3);
  stream_->SetStreamUsage();

  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OHOSAudioOutputStream AudioSessionType: 3"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, SetStreamUsage004) {
  testing::internal::CaptureStderr();
  stream_->isCommunication_ = false;
  SetSessionType(0);
  SetContentType(media::MediaContentType::kTransient);
  stream_->SetStreamUsage();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OHOSAudioOutputStream AudioSessionType: 0"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, SetStreamUsage005) {
  testing::internal::CaptureStderr();
  stream_->isCommunication_ = false;
  stream_->parameters_.set_sample_rate(0);
  stream_->SetStreamUsage();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("OHOSAudioOutputStream AudioSessionType:"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, SetStreamUsage006) {
  testing::internal::CaptureStderr();
  stream_->isCommunication_ = false;
  SetSessionType(0);
  SetContentType(media::MediaContentType::kPersistent);
  stream_->SetStreamUsage();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OHOSAudioOutputStream AudioSessionType: 0"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, Prepare001) {
  testing::internal::CaptureStderr();
  auto params = AudioParameters(
      AudioParameters::Format::AUDIO_PCM_LINEAR,
      ChannelLayoutConfig::FromLayout<CHANNEL_LAYOUT_MONO>(), 8000, 160);
  stream_->parameters_.set_sample_rate(0);

  stream_->Prepare(params);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find(
                "OHOSAudioOutputStream::Prepare parameters_ is not valid"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, Prepare002) {
  testing::internal::CaptureStderr();
  auto params = AudioParameters(
      AudioParameters::Format::AUDIO_PCM_LINEAR,
      ChannelLayoutConfig::FromLayout<CHANNEL_LAYOUT_MONO>(), 8000, 160);

  stream_->Prepare(params);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(
      log_output.find(
          "OHOSAudioOutputStream::SetRendererInterruptMode audioExclusive:"),
      std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, StartRender001) {
  stream_->Open();
  SetSessionFocusState(content::MediaSessionImpl::State::SUSPENDED);

  EXPECT_TRUE(stream_->StartRender());
  stream_->Stop();
}

TEST_F(OHOSAudioOutputStreamTest, StartRender002) {
  stream_->InitRender();
  SetSessionState(content::MediaSessionImpl::NWebMediaSessionState::NONEED);
  SetHasOneSHotPlayers(false);
  SetSessionFocusState(content::MediaSessionImpl::State::SUSPENDED);
  SetPlayingState(true);
  SetMuted(true);
  stream_->audio_renderer_ = nullptr;
  EXPECT_FALSE(stream_->StartRender());
}

TEST_F(OHOSAudioOutputStreamTest, StartRender003) {
  testing::internal::CaptureStderr();
  stream_->Open();
  stream_->parameters_.set_sample_rate(0);
  stream_->audio_renderer_ = nullptr;
  EXPECT_FALSE(stream_->StartRender());
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("ohos audio render release failed"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, ReportError001) {
  testing::internal::CaptureStderr();
  stream_->isDestroyed_ = true;
  stream_->ReportError();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(
      log_output.find("OHOSAudioOutputStream::ReportError during destroyed"),
      std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, ReportError002) {
  testing::internal::CaptureStderr();
  stream_->isDestroyed_ = false;
  stream_->callback_ = nullptr;
  stream_->ReportError();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(
      log_output.find("OHOSAudioOutputStream::ReportError during destroyed"),
      std::string::npos);
  MockAudioSourceCallback callback;
  stream_->callback_ = &callback;
  EXPECT_CALL(callback, OnError).Times(1);
  stream_->ReportError();
}

TEST_F(OHOSAudioOutputStreamTest, GetDelay001) {
  testing::internal::CaptureStderr();
  stream_->audio_renderer_ = nullptr;
  base::TimeTicks delay_timestamp;
  stream_->GetDelay(delay_timestamp);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("Failed to get audio latency, result:"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, PumpSamples001) {
  testing::internal::CaptureStderr();
  stream_->running_ = false;
  stream_->PumpSamples();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("The playback is stopped. Exit PumpSamples."),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, PumpSamples002) {
  testing::internal::CaptureStderr();
  stream_->running_ = true;
  stream_->isSuspended_ = false;
  stream_->PumpSamples();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(
      log_output.find("Not a focus preemption scenario. Exit PumpSamples."),
      std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, PumpSamples003) {
  testing::internal::CaptureStderr();
  stream_->running_ = true;
  stream_->isSuspended_ = true;
  stream_->callback_ = nullptr;
  stream_->PumpSamples();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("PumpSample failed, callback_ is nullptr"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, PumpSamples004) {
  testing::internal::CaptureStderr();
  stream_->running_ = true;
  stream_->isSuspended_ = true;
  MockAudioSourceCallback callback;
  stream_->callback_ = &callback;
  stream_->audio_renderer_ = nullptr;
  stream_->PumpSamples();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("AudioRender is nullptr"), std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, PumpSamples005) {
  testing::internal::CaptureStderr();
  stream_->InitRender();
  stream_->running_ = true;
  stream_->isSuspended_ = true;
  MockAudioSourceCallback callback;
  stream_->callback_ = &callback;
  stream_->PumpSamples();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("AudioRender is nullptr"), std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OnWriteData001) {
  testing::internal::CaptureStderr();
  stream_->running_ = false;
  stream_->callback_ = nullptr;
  stream_->OnWriteData(nullptr, 0);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("OnWriteData failed, callback_ is nullptr"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OnWriteData002) {
  testing::internal::CaptureStderr();
  stream_->running_ = true;
  stream_->callback_ = nullptr;
  stream_->OnWriteData(nullptr, 0);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("OnWriteData failed, callback_ is nullptr"),
            std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, OnWriteData003) {
  uint8_t* buffer = new uint8_t[10];
  buffer[0] = 97;
  stream_->running_ = true;
  stream_->callback_ = nullptr;
  testing::internal::CaptureStderr();
  stream_->OnWriteData(buffer, 0);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OnWriteData failed, callback_ is nullptr"),
            std::string::npos);
  delete[] buffer;          
}

TEST_F(OHOSAudioOutputStreamTest, OnWriteData004) {
  uint8_t* buffer = new uint8_t[10];
  buffer[0] = 97;
  stream_->running_ = true;
  MockAudioSourceCallback callback;
  stream_->callback_ = &callback;
  stream_->reference_time_ = base::TimeTicks();
  testing::internal::CaptureStderr();
  stream_->OnWriteData(buffer, 1);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("OnWriteData failed, callback_ is nullptr"),
            std::string::npos);
  EXPECT_FALSE(stream_->reference_time_.is_null());
  delete[] buffer;
}

TEST_F(OHOSAudioOutputStreamTest, OnWriteData005) {
  uint8_t* buffer = new uint8_t[10];
  buffer[0] = 97;
  stream_->running_ = true;
  MockAudioSourceCallback callback;
  stream_->callback_ = &callback;
  stream_->reference_time_ = base::TimeTicks::Now();
  testing::internal::CaptureStderr();
  stream_->OnWriteData(buffer, 1);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("OnWriteData failed, callback_ is nullptr"),
            std::string::npos);
  EXPECT_FALSE(stream_->reference_time_.is_null());
  delete[] buffer;
}

TEST_F(OHOSAudioOutputStreamTest, SetUpAudioSilentState001) {
  testing::internal::CaptureStderr();
  stream_->isSilentMode_ = false;
  stream_->parameters_.set_sample_rate(0);
  stream_->SetUpAudioSilentState();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(
      log_output.find("OHOSAudioOutputStream: Try to set audio silent but get"),
      std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, SetUpAudioSilentState002) {
  testing::internal::CaptureStderr();
  stream_->isSilentMode_ = true;
  stream_->parameters_.set_sample_rate(0);
  stream_->SetUpAudioSilentState();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(
      log_output.find("OHOSAudioOutputStream: Try to set audio silent but get"),
      std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, SetUpAudioSilentState003) {
  testing::internal::CaptureStderr();
  stream_->isSilentMode_ = true;
  stream_->audio_renderer_ = nullptr;
  stream_->SetUpAudioSilentState();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(
      log_output.find("OHOSAudioOutputStream: Try to set audio silent but get"),
      std::string::npos);
}

TEST_F(OHOSAudioOutputStreamTest, SetUpAudioSilentState004) {
  stream_->InitRender();
  stream_->isSilentMode_ = true;
  stream_->SetUpAudioSilentState();
  EXPECT_NE(stream_->write_data_counts_, 0);
}

TEST_F(OHOSAudioOutputStreamTest, SetUpAudioSilentState005) {
  stream_->InitRender();
  stream_->isSilentMode_ = true;
  SetSessionFocusState(content::MediaSessionImpl::State::ACTIVE);
  stream_->SetUpAudioSilentState();
  EXPECT_EQ(stream_->write_data_counts_, 0);
}

TEST_F(OHOSAudioOutputStreamTest, SetUpAudioSilentState006) {
  stream_->InitRender();
  stream_->isSilentMode_ = true;
  SetSessionFocusState(content::MediaSessionImpl::State::SUSPENDED);
  SetPlayingState(true);
  stream_->SetUpAudioSilentState();
  EXPECT_EQ(stream_->write_data_counts_, 0);
}

TEST_F(OHOSAudioOutputStreamTest, SetUpAudioSilentState007) {
  stream_->InitRender();
  stream_->isSilentMode_ = true;
  SetSessionFocusState(content::MediaSessionImpl::State::SUSPENDED);
  SetPlayingState(false);
  stream_->write_data_counts_ = 1;
  stream_->SetUpAudioSilentState();
  EXPECT_EQ(stream_->write_data_counts_, 0);
}

TEST_F(OHOSAudioOutputStreamTest, SetUpAudioSilentState008) {
  stream_->InitRender();
  stream_->isSilentMode_ = true;
  SetSessionFocusState(content::MediaSessionImpl::State::SUSPENDED);
  SetPlayingState(false);
  stream_->write_data_counts_ = 0;
  stream_->SetUpAudioSilentState();
  EXPECT_NE(stream_->write_data_counts_, 0);
}

TEST_F(OHOSAudioOutputStreamTest, SetUpAudioSilentState009) {
  stream_->InitRender();
  stream_->isSilentMode_ = true;
  SetSessionFocusState(content::MediaSessionImpl::State::ACTIVE);
  SetMuted(false);
  stream_->SetUpAudioSilentState();
  EXPECT_EQ(stream_->write_data_counts_, 0);
}

TEST_F(OHOSAudioOutputStreamTest, SetUpAudioSilentState010) {
  stream_->InitRender();
  stream_->isSilentMode_ = true;
  SetSessionFocusState(content::MediaSessionImpl::State::ACTIVE);
  SetMuted(true);
  stream_->SetUpAudioSilentState();
  EXPECT_NE(stream_->write_data_counts_, 0);
}

TEST_F(OHOSAudioOutputStreamTest, IsPreloadOrMutedMediaMode001) {
  stream_->parameters_.set_sample_rate(0);
  EXPECT_EQ(stream_->IsPreloadOrMutedMediaMode(), false);
}

TEST_F(OHOSAudioOutputStreamTest, IsPreloadOrMutedMediaMode002) {
  stream_->InitRender();
  SetSessionState(content::MediaSessionImpl::NWebMediaSessionState::NOINITIAL);
  EXPECT_EQ(stream_->IsPreloadOrMutedMediaMode(), false);
}

TEST_F(OHOSAudioOutputStreamTest, IsPreloadOrMutedMediaMode003) {
  stream_->InitRender();
  SetSessionState(content::MediaSessionImpl::NWebMediaSessionState::NONEED);
  SetHasOneSHotPlayers(true);
  EXPECT_EQ(stream_->IsPreloadOrMutedMediaMode(), false);
}

TEST_F(OHOSAudioOutputStreamTest, IsPreloadOrMutedMediaMode004) {
  stream_->InitRender();
  SetSessionState(content::MediaSessionImpl::NWebMediaSessionState::NONEED);
  SetHasOneSHotPlayers(false);
  SetSessionFocusState(content::MediaSessionImpl::State::ACTIVE);
  SetMuted(true);
  EXPECT_EQ(stream_->IsPreloadOrMutedMediaMode(), true);
}

TEST_F(OHOSAudioOutputStreamTest, IsPreloadOrMutedMediaMode005) {
  stream_->InitRender();
  SetSessionState(content::MediaSessionImpl::NWebMediaSessionState::NONEED);
  SetHasOneSHotPlayers(false);
  SetSessionFocusState(content::MediaSessionImpl::State::SUSPENDED);
  SetPlayingState(true);
  SetMuted(true);
  EXPECT_EQ(stream_->IsPreloadOrMutedMediaMode(), true);
}

TEST_F(OHOSAudioOutputStreamTest, IsPreloadOrMutedMediaMode006) {
  stream_->InitRender();
  SetSessionState(content::MediaSessionImpl::NWebMediaSessionState::NONEED);
  SetHasOneSHotPlayers(false);
  SetSessionFocusState(content::MediaSessionImpl::State::SUSPENDED);
  SetPlayingState(true);
  SetMuted(false);
  EXPECT_EQ(stream_->IsPreloadOrMutedMediaMode(), false);
}

TEST_F(OHOSAudioOutputStreamTest, IsPreloadOrMutedMediaMode007) {
  stream_->InitRender();
  SetSessionState(content::MediaSessionImpl::NWebMediaSessionState::NONEED);
  SetHasOneSHotPlayers(false);
  SetSessionFocusState(content::MediaSessionImpl::State::SUSPENDED);
  SetPlayingState(false);
  SetMuted(true);
  EXPECT_EQ(stream_->IsPreloadOrMutedMediaMode(), true);
}

TEST_F(OHOSAudioOutputStreamTest, SchedulePumpSamples001) {
  stream_->running_ = true;
  stream_->SchedulePumpSamples();
  EXPECT_TRUE(stream_->timer_.IsRunning());
  stream_->SchedulePumpSamples();
  EXPECT_TRUE(stream_->timer_.IsRunning());
}

TEST_F(OHOSAudioOutputStreamTest, SchedulePumpSamples002) {
  stream_->running_ = false;
  stream_->SchedulePumpSamples();
  EXPECT_FALSE(stream_->timer_.IsRunning());
}
}  // namespace media