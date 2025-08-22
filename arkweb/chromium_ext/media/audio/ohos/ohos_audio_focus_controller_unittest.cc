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

class OHOSAudioFocusControllerTest : public ::testing::Test {
 public:
  void SetUp() override {}

  void TearDown() override {}
};

TEST(OHOSAudioFocusControllerTest, OnResume01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  OHOSAudioFocusController::OnResume(params);
}

TEST(OHOSAudioFocusControllerTest, OnResume02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  OHOSAudioFocusController::OnResume(params);
}

TEST(OHOSAudioFocusControllerTest, OnSuspend01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  OHOSAudioFocusController::OnSuspend(params);
}

TEST(OHOSAudioFocusControllerTest, OnSuspend02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  OHOSAudioFocusController::OnSuspend(params);
}

TEST(OHOSAudioFocusControllerTest, GetContentTypeOnUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  OHOSAudioFocusController::GetContentTypeOnUIThread(params);
}

TEST(OHOSAudioFocusControllerTest, GetContentTypeOnUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  OHOSAudioFocusController::GetContentTypeOnUIThread(params);
}

TEST(OHOSAudioFocusControllerTest, CheckActiveOnUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckActiveOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST(OHOSAudioFocusControllerTest, CheckActiveOnUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckActiveOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST(OHOSAudioFocusControllerTest, CheckOneShotPlayersOnUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckOneShotPlayersOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST(OHOSAudioFocusControllerTest, CheckOneShotPlayersOnUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckOneShotPlayersOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST(OHOSAudioFocusControllerTest, CheckGetSessionStateOnUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  auto res = OHOSAudioFocusController::CheckGetSessionStateOnUIThread(params);
  EXPECT_EQ(res, content::MediaSessionImpl::NWebMediaSessionState::NOINITIAL);
}

TEST(OHOSAudioFocusControllerTest, CheckGetSessionStateOnUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  auto res = OHOSAudioFocusController::CheckGetSessionStateOnUIThread(params);
  EXPECT_EQ(res, content::MediaSessionImpl::NWebMediaSessionState::NOINITIAL);
}

TEST(OHOSAudioFocusControllerTest, CheckGetPlayingStateOnUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckGetPlayingStateOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST(OHOSAudioFocusControllerTest, CheckGetPlayingStateOnUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckGetPlayingStateOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST(OHOSAudioFocusControllerTest, CheckGetMuteStateOnUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckGetMuteStateOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST(OHOSAudioFocusControllerTest, CheckGetMuteStateOnUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckGetMuteStateOnUIThread(params);
  EXPECT_EQ(res, false);
}

TEST(OHOSAudioFocusControllerTest, OneShotMediaPlayerStopped01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  OHOSAudioFocusController::OneShotMediaPlayerStopped(params);
}

TEST(OHOSAudioFocusControllerTest, OneShotMediaPlayerStopped02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  OHOSAudioFocusController::OneShotMediaPlayerStopped(params);
}

TEST(OHOSAudioFocusControllerTest, CheckGetAudioExclusiveUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckGetAudioExclusiveUIThread(params);
  EXPECT_EQ(res, false);
}

TEST(OHOSAudioFocusControllerTest, CheckGetAudioExclusiveUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckGetAudioExclusiveUIThread(params);
  EXPECT_EQ(res, false);
}

TEST(OHOSAudioFocusControllerTest, CheckGetAudioResumeIntervalUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  int res = OHOSAudioFocusController::CheckGetAudioResumeIntervalUIThread(params);
  EXPECT_EQ(res, 0);
}

TEST(OHOSAudioFocusControllerTest, CheckGetAudioResumeIntervalUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  int res = OHOSAudioFocusController::CheckGetAudioResumeIntervalUIThread(params);
  EXPECT_EQ(res, 0);
}

TEST(OHOSAudioFocusControllerTest, CheckGetAudioSessionTypeUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  int res = OHOSAudioFocusController::CheckGetAudioSessionTypeUIThread(params);
  EXPECT_EQ(res, 0);
}

TEST(OHOSAudioFocusControllerTest, CheckGetAudioSessionTypeUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  int res = OHOSAudioFocusController::CheckGetAudioSessionTypeUIThread(params);
  EXPECT_EQ(res, 0);
}

TEST(OHOSAudioFocusControllerTest, CheckIsSuspendedUIThread01) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_PCM_LOW_LATENCY,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckIsSuspendedUIThread(params);
  EXPECT_EQ(res, false);
}

TEST(OHOSAudioFocusControllerTest, CheckIsSuspendedUIThread02) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(0);
  AudioParameters params(
      AudioParameters::AUDIO_FAKE,
      ChannelLayoutConfig::Guess(kDefaultChannelCount), kDefaultSampleRate,
      kMinimumInputBufferSize);
  bool res = OHOSAudioFocusController::CheckIsSuspendedUIThread(params);
  EXPECT_EQ(res, false);
}
} // namespace media