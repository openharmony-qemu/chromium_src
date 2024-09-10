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
#include <stdint.h>

#include <memory>

#include "base/strings/string_util.h"
#include "base/test/task_environment.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define private public
#define protected public
#include "ohos_media_player_bridge.h"
#undef protected
#undef private

using namespace media;
using namespace testing;
using namespace OHOS::NWeb;
using namespace base;

class MockGURL : public GURL {
 public:
  MOCK_METHOD(bool, SchemeIsBlob, (), (const));
};

class MockClient : public OHOSMediaPlayerBridge::Client {
 public:
  virtual ~MockClient() {}
  void OnFrameAvailable(int fd,
                        uint32_t size,
                        int32_t coded_width,
                        int32_t coded_height,
                        int32_t visible_width,
                        int32_t visible_height,
                        int32_t format) override {}
  void OnMediaDurationChanged(base::TimeDelta duration) override {}
  void OnPlaybackComplete() override {}
  void OnError(int error) override {}
  void OnVideoSizeChanged(int width, int height) override {}
  void OnPlayerInterruptEvent(int32_t value) override {}
  void OnAudioStateChanged(bool isAudible) override {}
};

class MockPlayerAdapter : public PlayerAdapter {
 public:
  int32_t SetPlayerCallback(
      std::shared_ptr<PlayerCallbackAdapter> callbackAdapter) override {
    return ret_;
  }
  int32_t SetSource(const std::string& url) override { return ret_; }
  int32_t SetSource(int32_t fd, int64_t offset, int64_t size) override {
    return ret_;
  }
  int32_t SetVideoSurface(
      std::shared_ptr<IConsumerSurfaceAdapter> cSurfaceAdapter) override {
    return ret_;
  }
  int32_t SetVolume(float leftVolume, float rightVolume) override {
    return ret_ = false;
  }
  int32_t Seek(int32_t mSeconds, PlayerSeekMode mode) override { return ret_; }
  int32_t Play() override { return ret_; }
  int32_t Pause() override { return ret_; }
  int32_t PrepareAsync() override { return ret_; }
  int32_t GetCurrentTime(int32_t& currentTime) override { return ret_; }
  int32_t GetDuration(int32_t& duration) override { return ret_; }
  int32_t SetPlaybackSpeed(PlaybackRateMode mode) override { return ret_; }

  int32_t ret_ = 0;
};

class MoxkSurfaceBufferAdapter : public SurfaceBufferAdapter {
 public:
  int32_t GetFileDescriptor() override { return ret_; }
  int32_t GetWidth() override { return ret_; }
  int32_t GetHeight() override { return height_; }
  int32_t GetStride() override { return ret_; }
  int32_t GetFormat() override { return format_; }
  uint32_t GetSize() override { return size_; }
  void* GetVirAddr() override { return &ret_; }

  int32_t ret_ = 0;
  int32_t format_ = 0;
  int32_t height_ = 0;
  int32_t size_ = 0;
  uint32_t uin_ret_ = 0;
};

class MoxkIConsumerSurfaceAdapter : public IConsumerSurfaceAdapter {
 public:
  int32_t RegisterConsumerListener(
      std::shared_ptr<IBufferConsumerListenerAdapter> listener) override {
    return ic_ret;
  }
  int32_t ReleaseBuffer(std::shared_ptr<SurfaceBufferAdapter> buffer,
                        int32_t fence) override {
    return ic_ret;
  }
  int32_t SetUserData(const std::string& key, const std::string& val) override {
    return ic_ret;
  }
  int32_t SetQueueSize(uint32_t queueSize) override { return ic_ret; }
  int32_t ic_ret;
};

constexpr int player_init_ok = 0;
constexpr int milliseconds = 1000;
constexpr base::TimeDelta time_delta = base::Milliseconds(milliseconds);

class OHOSMediaPlayerBridgeTests : public ::testing::Test {
 public:
  void SetUp() override {
    gurl_ = std::make_shared<MockGURL>();
    mock_client_ = std::make_shared<MockClient>();

    bridge = new OHOSMediaPlayerBridge(
        *gurl_, site_for_cookies, top_frame_origin, user_agent, hide_url_log,
        mock_client_.get(), allow_credentials, is_hls);
  }

  void TearDown() override {
    delete bridge;
    bridge = nullptr;
    mock_client_.reset();
    gurl_.reset();
  }

  OHOSMediaPlayerBridge* bridge;
  net::SiteForCookies site_for_cookies;
  url::Origin top_frame_origin;
  std::string user_agent{"test"};
  bool hide_url_log = false;
  bool allow_credentials = true;
  bool is_hls = false;
  std::shared_ptr<MockClient> mock_client_;
  std::shared_ptr<MockGURL> gurl_;
  base::test::TaskEnvironment task_environment_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
};

TEST_F(OHOSMediaPlayerBridgeTests, Initialize_001) {
  int32_t result = bridge->Initialize();
  EXPECT_EQ(result, player_init_ok);
}

TEST_F(OHOSMediaPlayerBridgeTests, Initialize_002) {
  EXPECT_CALL(*gurl_, SchemeIsBlob()).WillRepeatedly(Return(true));
  int32_t result = bridge->Initialize();
  EXPECT_EQ(result, MockPlayerAdapter::PLAYER_STATE_ERROR);
}

TEST_F(OHOSMediaPlayerBridgeTests, Start_001) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->pending_play_ = false;
  bridge->Start();
  EXPECT_TRUE(bridge->pending_play_);
}

TEST_F(OHOSMediaPlayerBridgeTests, Start_002) {
  bridge->player_ = nullptr;
  bridge->prepared_ = true;
  bridge->Start();
}

TEST_F(OHOSMediaPlayerBridgeTests, Start_003) {
  bridge->player_ = nullptr;
  bridge->prepared_ = false;
  bridge->Start();
}

TEST_F(OHOSMediaPlayerBridgeTests, Prepare_001) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->Prepare();
}

TEST_F(OHOSMediaPlayerBridgeTests, Prepare_002) {
  bridge->player_ = nullptr;
  EXPECT_CALL(*gurl_, SchemeIsBlob()).WillRepeatedly(Return(true));
  bridge->Prepare();
}

TEST_F(OHOSMediaPlayerBridgeTests, Prepare_003) {
  bridge->player_ = nullptr;
  EXPECT_CALL(*gurl_, SchemeIsBlob()).WillRepeatedly(Return(false));
  bridge->Prepare();
}

TEST_F(OHOSMediaPlayerBridgeTests, StartInternal_001) {
  bridge->prepared_ = true;
  bridge->player_ = std::make_unique<MockPlayerAdapter>();
  bridge->StartInternal();
}

TEST_F(OHOSMediaPlayerBridgeTests, StartInternal_002) {
  bridge->prepared_ = false;
  bridge->player_ = nullptr;
  bridge->StartInternal();
}

TEST_F(OHOSMediaPlayerBridgeTests, StartInternal_003) {
  bridge->prepared_ = false;
  bridge->player_ = std::make_unique<MockPlayerAdapter>();
  bridge->StartInternal();
  bridge->prepared_ = false;
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause_001) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_IDLE;
  bridge->Pause();
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause_002) {
  bridge->player_ = nullptr;
  bridge->Pause();
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause__003) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  mock_player_adapter->ret_ = 1;
  bridge->player_ = std::move(mock_player_adapter);
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_STARTED;
  bridge->Pause();
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause__004) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  mock_player_adapter->ret_ = 0;
  bridge->current_volume_ = 0;
  bridge->player_ = std::move(mock_player_adapter);
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_STARTED;
  bridge->Pause();
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause__005) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  mock_player_adapter->ret_ = 0;
  bridge->current_volume_ = 1;
  bridge->player_ = std::move(mock_player_adapter);
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_STARTED;
  auto mock_client = std::make_unique<MockClient>();
  bridge->client_ = mock_client.get();
  bridge->Pause();
}

TEST_F(OHOSMediaPlayerBridgeTests, SeekTo_001) {
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_PLAYBACK_COMPLETE;
  bridge->seeking_on_playback_complete_ = false;
  bridge->SeekTo(time_delta);
}

TEST_F(OHOSMediaPlayerBridgeTests, SeekTo_002) {
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_STOPPED;
  bridge->seeking_on_playback_complete_ = false;
  bridge->SeekTo(time_delta);
}

TEST_F(OHOSMediaPlayerBridgeTests, SeekTo_003) {
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_STOPPED;
  bridge->seeking_on_playback_complete_ = true;
  bridge->SeekTo(time_delta);
}

TEST_F(OHOSMediaPlayerBridgeTests, SeekInternal_001) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  mock_player_adapter_->ret_ = 0;
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->SeekInternal(time_delta);
}

TEST_F(OHOSMediaPlayerBridgeTests, SeekInternal_002) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  mock_player_adapter_->ret_ = 1;
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->SeekInternal(time_delta);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVolume_001) {
  int volume = 0;
  bool is_muted = false;
  bridge->prepared_ = false;
  bridge->SetVolume(volume, is_muted);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVolume_002) {
  int volume = 0;
  bool is_muted = false;
  bridge->prepared_ = true;
  bridge->player_ = nullptr;
  bridge->SetVolume(volume, is_muted);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVolume_003) {
  int volume = 0;
  bool is_muted = false;
  bridge->prepared_ = true;
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  mock_player_adapter->ret_ = 1;
  bridge->player_ = std::move(mock_player_adapter);
  bridge->SetVolume(volume, is_muted);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVolume_004) {
  int volume = 1;
  bool is_muted = false;
  bridge->prepared_ = true;
  bridge->current_volume_ = 0;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED;
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  mock_player_adapter->ret_ = 0;
  bridge->player_ = std::move(mock_player_adapter);
  bridge->SetVolume(volume, is_muted);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVolume_005) {
  int volume = 0;
  bool is_muted = false;
  bridge->prepared_ = true;
  bridge->current_volume_ = 1;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED;
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  mock_player_adapter->ret_ = 0;
  bridge->player_ = std::move(mock_player_adapter);
  bridge->SetVolume(volume, is_muted);
}

TEST_F(OHOSMediaPlayerBridgeTests, PropagateDuration) {
  auto mock_client = std::make_shared<MockClient>();
  bridge->client_ = mock_client.get();
  bridge->PropagateDuration(time_delta);
  EXPECT_EQ(bridge->duration_, time_delta);
}

TEST_F(OHOSMediaPlayerBridgeTests, GetDuration_001) {
  std::unique_ptr<MockPlayerAdapter> mock_player_adapter_ =
      std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->prepared_ = true;
  bridge->GetDuration();
}

TEST_F(OHOSMediaPlayerBridgeTests, GetDuration_002) {
  std::unique_ptr<MockPlayerAdapter> mock_player_adapter_ =
      std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->prepared_ = false;
  bridge->GetDuration();
}

TEST_F(OHOSMediaPlayerBridgeTests, GetMediaTime_001) {
  bridge->pending_seek_ = time_delta;
  bridge->prepared_ = false;
  base::TimeDelta time = bridge->GetMediaTime();
  EXPECT_EQ(time, time_delta);
}

TEST_F(OHOSMediaPlayerBridgeTests, GetMediaTime_002) {
  bridge->pending_seek_ = time_delta;
  std::unique_ptr<MockPlayerAdapter> mock_player_adapter_ =
      std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->prepared_ = true;
  bridge->seeking_on_playback_complete_ = false;
  bridge->seek_complete_ = true;
  bridge->GetMediaTime();
}

TEST_F(OHOSMediaPlayerBridgeTests, SeekDone) {
  bridge->seek_complete_ = false;
  bridge->SeekDone();
  EXPECT_TRUE(bridge->seek_complete_);
}
