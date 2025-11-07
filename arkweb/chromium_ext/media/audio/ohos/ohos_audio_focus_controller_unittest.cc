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

#include "content/browser/media/session/media_session_impl.h"
#define private public
#include "media/audio/ohos/ohos_audio_focus_controller.h"
#undef private
#include "base/logging.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "arkweb/chromium_ext/media/base/ohos/ohos_media_player_bridge.h"
#include "content/public/test/browser_task_environment.h"
#include "content/public/test/test_renderer_host.h"
#include "content/test/test_web_contents.h"

using namespace media;
using namespace testing;
using namespace OHOS::NWeb;
using namespace base;

namespace media {
namespace {
constexpr int kDefaultSampleRate = 48000;
constexpr int kDefaultChannelCount = 2;
constexpr int kMinimumInputBufferSize = 2048;
}

class MockPlayerAdapter : public PlayerAdapter {
 public:
  MOCK_METHOD(int32_t,
              SetPlayerCallback,
              (std::shared_ptr<PlayerCallbackAdapter> callbackAdapter),
              (override));
  MOCK_METHOD(int32_t, SetSource, (const std::string& url), (override));
  MOCK_METHOD(int32_t,
              SetSource,
              (int32_t fd, int64_t offset, int64_t size),
              (override));
  MOCK_METHOD(int32_t,
              SetVideoSurface,
              (std::shared_ptr<IConsumerSurfaceAdapter> cSurfaceAdapter),
              (override));
  MOCK_METHOD(int32_t,
              SetVolume,
              (float leftVolume, float rightVolume),
              (override));
  MOCK_METHOD(int32_t,
              Seek,
              (int32_t mSeconds, PlayerSeekMode mode),
              (override));
  MOCK_METHOD(int32_t, Play, (), (override));
  MOCK_METHOD(int32_t, Pause, (), (override));
  MOCK_METHOD(int32_t, PrepareAsync, (), (override));
  MOCK_METHOD(int32_t, GetCurrentTime, (int32_t & currentTime), (override));
  MOCK_METHOD(int32_t, GetDuration, (int32_t & duration), (override));
  MOCK_METHOD(int32_t, SetPlaybackSpeed, (PlaybackRateMode mode), (override));
  MOCK_METHOD(int32_t, SetVideoSurfaceNew, (void* native_window), (override));
  using HeaderType = std::map<std::string, std::string>;
  MOCK_METHOD(int32_t,
              SetMediaSourceHeader,
              (const std::string& url, const HeaderType& header),
              (override));
};

class OHOSAudioFocusControllerTest : public content::RenderViewHostTestHarness {
 public:
  OHOSAudioFocusControllerTest() : content::RenderViewHostTestHarness() {}

  OHOSAudioFocusControllerTest(const OHOSAudioFocusControllerTest&) = delete;
  OHOSAudioFocusControllerTest& operator=(const OHOSAudioFocusControllerTest&) =
      delete;

  void SetUp() override {
    content::RenderViewHostTestHarness::SetUp();
  }

  void TearDown() override {
    content::RenderViewHostTestHarness::TearDown();
  }

  void SetAudioParametersId(AudioParameters &params) {
    auto frameHost = main_rfh();
    if (!frameHost) {
      return nullptr;
    }
    params.set_render_process_id(
        frameHost->GetProcess()->GetID());
    params.set_render_frame_id(frameHost->GetRoutingID());
  }
};

TEST_F(OHOSAudioFocusControllerTest, OnResume01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  testing::internal::CaptureStderr();
  OHOSAudioFocusController::OnResume(params);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OnResume RenderFrameHost not found for PID"), std::string::npos);
}

TEST_F(OHOSAudioFocusControllerTest, OnResume02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  SetAudioParametersId(params);
  testing::internal::CaptureStderr();
  OHOSAudioFocusController::OnResume(params);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("OHOSAudioFocusController OnResume get webContent failed"), std::string::npos);
  EXPECT_EQ(log_output.find("OHOSAudioFocusController OnResume get mediaSession failed"), std::string::npos);
}

TEST_F(OHOSAudioFocusControllerTest, OnSuspend01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  testing::internal::CaptureStderr();
  OHOSAudioFocusController::OnSuspend(params);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OnSuspend RenderFrameHost not found for PID"), std::string::npos);
}

TEST_F(OHOSAudioFocusControllerTest, OnSuspend02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  SetAudioParametersId(params);
  testing::internal::CaptureStderr();
  OHOSAudioFocusController::OnSuspend(params);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("OHOSAudioFocusController OnSuspend get webContent failed"), std::string::npos);
  EXPECT_EQ(log_output.find("OHOSAudioFocusController OnSuspend get mediaSession failed"), std::string::npos);
}

TEST_F(OHOSAudioFocusControllerTest, GetContentTypeOnUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  testing::internal::CaptureStderr();
  OHOSAudioFocusController::GetContentTypeOnUIThread(params);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("GetContentTypeOnUIThread RenderFrameHost not found for PID"), std::string::npos);
}

TEST_F(OHOSAudioFocusControllerTest, GetContentTypeOnUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  SetAudioParametersId(params);
  testing::internal::CaptureStderr();
  OHOSAudioFocusController::GetContentTypeOnUIThread(params);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("GetContentTypeOnUIThread WebContents not "
                            "associated with RenderFrameHost"), std::string::npos);
  EXPECT_EQ(log_output.find("GetContentTypeOnUIThread MediaSession not "
                            "initialized for WebContents"), std::string::npos);
}

TEST_F(OHOSAudioFocusControllerTest, CheckActiveOnUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckActiveOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSAudioFocusControllerTest, CheckActiveOnUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  SetAudioParametersId(params);
  bool res = OHOSAudioFocusController::CheckActiveOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSAudioFocusControllerTest, CheckOneShotPlayersOnUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckOneShotPlayersOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSAudioFocusControllerTest, CheckOneShotPlayersOnUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  SetAudioParametersId(params);
  bool res = OHOSAudioFocusController::CheckOneShotPlayersOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSAudioFocusControllerTest, CheckOneShotPlayersWhenSetMetadataOnUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckOneShotPlayersWhenSetMetadataOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSAudioFocusControllerTest, CheckOneShotPlayersWhenSetMetadataOnUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  SetAudioParametersId(params);
  bool res = OHOSAudioFocusController::CheckOneShotPlayersWhenSetMetadataOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSAudioFocusControllerTest, CheckGetSessionStateOnUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  auto res = OHOSAudioFocusController::CheckGetSessionStateOnUIThread(params);
  EXPECT_EQ(res, content::MediaSessionImpl::NWebMediaSessionState::NOINITIAL);
}

TEST_F(OHOSAudioFocusControllerTest, CheckGetSessionStateOnUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  SetAudioParametersId(params);
  auto res = OHOSAudioFocusController::CheckGetSessionStateOnUIThread(params);
  EXPECT_EQ(res, content::MediaSessionImpl::NWebMediaSessionState::NOINITIAL);
}

TEST_F(OHOSAudioFocusControllerTest, CheckGetPlayingStateOnUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckGetPlayingStateOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSAudioFocusControllerTest, CheckGetPlayingStateOnUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  SetAudioParametersId(params);
  bool res = OHOSAudioFocusController::CheckGetPlayingStateOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSAudioFocusControllerTest, CheckGetMuteStateOnUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckGetMuteStateOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSAudioFocusControllerTest, CheckGetMuteStateOnUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  SetAudioParametersId(params);
  bool res = OHOSAudioFocusController::CheckGetMuteStateOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSAudioFocusControllerTest, OneShotMediaPlayerStopped01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  testing::internal::CaptureStderr();
  OHOSAudioFocusController::OneShotMediaPlayerStopped(params);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OneShotMediaPlayerStopped OnSuspend get RenderFrameHost failed"), std::string::npos);
}

TEST_F(OHOSAudioFocusControllerTest, OneShotMediaPlayerStopped02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  SetAudioParametersId(params);
  testing::internal::CaptureStderr();
  OHOSAudioFocusController::OneShotMediaPlayerStopped(params);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("OneShotMediaPlayerStopped OnSuspend get RenderFrameHost failed"), std::string::npos);
  EXPECT_EQ(log_output.find("OneShotMediaPlayerStopped OnSuspend get webContent failed"), std::string::npos);
}

TEST_F(OHOSAudioFocusControllerTest, CheckGetAudioExclusiveUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckGetAudioExclusiveUIThread(params);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSAudioFocusControllerTest, CheckGetAudioExclusiveUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  SetAudioParametersId(params);
  bool res = OHOSAudioFocusController::CheckGetAudioExclusiveUIThread(params);
  EXPECT_EQ(res, true);
}

TEST_F(OHOSAudioFocusControllerTest, CheckGetAudioResumeIntervalUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  int res = OHOSAudioFocusController::CheckGetAudioResumeIntervalUIThread(params);
  EXPECT_EQ(res, 0);
}

TEST_F(OHOSAudioFocusControllerTest, CheckGetAudioResumeIntervalUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  SetAudioParametersId(params);
  int res = OHOSAudioFocusController::CheckGetAudioResumeIntervalUIThread(params);
  EXPECT_EQ(res, 0);
}

TEST_F(OHOSAudioFocusControllerTest, CheckGetAudioSessionTypeUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  int res = OHOSAudioFocusController::CheckGetAudioSessionTypeUIThread(params);
  EXPECT_EQ(res, 0);
}

TEST_F(OHOSAudioFocusControllerTest, CheckGetAudioSessionTypeUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  SetAudioParametersId(params);
  int res = OHOSAudioFocusController::CheckGetAudioSessionTypeUIThread(params);
  EXPECT_EQ(res, 0);
}

TEST_F(OHOSAudioFocusControllerTest, CheckIsSuspendedUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckIsSuspendedUIThread(params);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSAudioFocusControllerTest, CheckIsSuspendedUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  SetAudioParametersId(params);
  bool res = OHOSAudioFocusController::CheckIsSuspendedUIThread(params);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSAudioFocusControllerTest, CheckGetMediaPlayerMuteStateOnUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckGetMediaPlayerMuteStateOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSAudioFocusControllerTest, CheckGetMediaPlayerMuteStateOnUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  SetAudioParametersId(params);
  bool res = OHOSAudioFocusController::CheckGetMediaPlayerMuteStateOnUIThread(params);
  EXPECT_EQ(res, false);
}

} // namespace media