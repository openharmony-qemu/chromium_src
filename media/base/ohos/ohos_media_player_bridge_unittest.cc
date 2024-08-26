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
#include <memory>
#include <string>
#include "gtest/gtest.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "base/test/task_environment.h"
#include "gmock/gmock.h"
#define private public
#include "ohos_media_player_bridge.h"
#undef private

using namespace media;
using namespace testing;
using namespace OHOS::NWeb;
using namespace  base;

class MockGURL : public GURL {
public:
  MOCK_METHOD(bool, SchemeIsBlob, (), (const));
};

class MockClient : public OHOSMediaPlayerBridge::Client {
public:
  virtual ~MockClient() {} 
  MOCK_METHOD(void, OnFrameAvailable, (int, uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t), ());
  MOCK_METHOD(void, OnMediaDurationChanged, (base::TimeDelta), ());
  MOCK_METHOD(void, OnPlaybackComplete, (), ());
  MOCK_METHOD(void, OnError, (int), ());
  MOCK_METHOD(void, OnVideoSizeChanged, (int, int), ());
  MOCK_METHOD(void, OnPlayerInterruptEvent, (int32_t), ());
  MOCK_METHOD(void, OnAudioStateChanged, (bool), ());
};

class MockPlayerAdapter : public PlayerAdapter {
public:
  MOCK_METHOD(int32_t, SetPlayerCallback, (std::shared_ptr<PlayerCallbackAdapter> callbackAdapter), ());
  MOCK_METHOD(int32_t, SetSource, (const std::string& url), ());
  MOCK_METHOD(int32_t, SetSource, (int32_t fd, int64_t offset, int64_t size), ());
  MOCK_METHOD(int32_t, SetVideoSurface, (std::shared_ptr<IConsumerSurfaceAdapter> cSurfaceAdapter), ());
  MOCK_METHOD(int32_t, SetVolume, (float leftVolume, float rightVolume), ());
  MOCK_METHOD(int32_t, Seek, (int32_t mSeconds, PlayerSeekMode mode), ());
  MOCK_METHOD(int32_t, Play, (), ());
  MOCK_METHOD(int32_t, Pause, (), ());
  MOCK_METHOD(int32_t, PrepareAsync, (), ());
  MOCK_METHOD(int32_t, GetCurrentTime, (int32_t& currentTime), ());
  MOCK_METHOD(int32_t, GetDuration, (int32_t& duration), ());
  MOCK_METHOD(int32_t, SetPlaybackSpeed, (PlaybackRateMode mode), ());
};

class MoxkSurfaceBufferAdapter :public SurfaceBufferAdapter{
public:
  MOCK_METHOD(int32_t, GetFileDescriptor, (), ());
  MOCK_METHOD(int32_t, GetWidth, (), ());
  MOCK_METHOD(int32_t, GetHeight, (), ());
  MOCK_METHOD(int32_t, GetStride, (), ());
  MOCK_METHOD(int32_t, GetFormat, (), ());
  MOCK_METHOD(uint32_t, GetSize, (), ());
  MOCK_METHOD(void* , GetVirAddr, (), ());
};

class MoxkIConsumerSurfaceAdapter :public IConsumerSurfaceAdapter{
public:
  MOCK_METHOD(int32_t, RegisterConsumerListener, (std::shared_ptr<IBufferConsumerListenerAdapter> listener), ());
  MOCK_METHOD(int32_t, ReleaseBuffer, (std::shared_ptr<SurfaceBufferAdapter> buffer, int32_t fence), ());
  MOCK_METHOD(int32_t, SetUserData, (const std::string& key, const std::string& val), ());
  MOCK_METHOD(int32_t, SetQueueSize, (uint32_t queueSize), ());
};

constexpr int player_init_ok = 0;
constexpr int volume = 9;
constexpr int milliseconds = 1000;
constexpr base::TimeDelta time_delta = base::Milliseconds(milliseconds);

class OHOSMediaPlayerBridgeTests : public ::testing::Test {
public:
  void SetUp() override {
    gurl_ = std::make_unique<GURL>("http://example.com");
    mock_client_ = std::make_unique<MockClient>();

    bridge = new OHOSMediaPlayerBridge(
      *gurl_.get(),
      site_for_cookies,
      top_frame_origin,
      user_agent,
      hide_url_log,
      mock_client_.get(), 
      allow_credentials,
      is_hls);
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
  std::string user_agent {"test"};
  bool hide_url_log = false;
  bool allow_credentials = true;
  bool is_hls = false;
  std::unique_ptr<MockClient> mock_client_;
  std::unique_ptr<GURL> gurl_;
  base::test::TaskEnvironment task_environment_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
};

TEST_F(OHOSMediaPlayerBridgeTests, Initialize) {
  int32_t result = bridge->Initialize();
  EXPECT_EQ(result, player_init_ok);
}

TEST_F(OHOSMediaPlayerBridgeTests, Start) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->pending_play_ = false;
  bridge->Start();
  EXPECT_TRUE(bridge->pending_play_);
  bridge->pending_play_ = false;
  bridge->player_ = nullptr;
}

TEST_F(OHOSMediaPlayerBridgeTests, Prepare) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);
  auto mock_grul_ = std::make_unique<MockGURL>();
  EXPECT_CALL(*mock_grul_, SchemeIsBlob())
    .WillRepeatedly(Return(true));    
  bridge->Prepare();
}

TEST_F(OHOSMediaPlayerBridgeTests, StartInternal) {
  bridge->prepared_ = true;
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);
  auto mock_player = std::make_unique<MockPlayerAdapter>();  
  EXPECT_CALL(*mock_player, Play())
    .WillRepeatedly(Return(0));
  bridge->StartInternal();
  bridge->prepared_ = false;
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);
  auto mock_player = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player, Pause())
    .WillRepeatedly(Return(0));
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_STARTED;
  bridge->Pause();
}

TEST_F(OHOSMediaPlayerBridgeTests, SeekTo) {
  bridge->SeekTo(time_delta);
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_PLAYBACK_COMPLETE;
  EXPECT_EQ(bridge->pending_seek_,time_delta);
  bridge->seeking_on_playback_complete_ = false;
}

TEST_F(OHOSMediaPlayerBridgeTests, SeekInternal) {
  std::unique_ptr<MockPlayerAdapter> mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  std::unique_ptr<MockPlayerAdapter> mock_player_ = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_, Seek(time_delta.InMilliseconds(),  OHOS::NWeb::PlayerSeekMode::SEEK_CLOSEST))
    .WillRepeatedly(Return(0));
  bridge->SeekInternal(time_delta);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVolume) {
  bridge->SetVolume(volume,true);
  EXPECT_TRUE(bridge->should_set_volume_on_prepare_);
  bridge->should_set_volume_on_prepare_ = false;
}

TEST_F(OHOSMediaPlayerBridgeTests, PropagateDuration) {
  auto mock_client = std::make_unique<MockClient>();
  bridge->client_ = mock_client.get();

  EXPECT_CALL(*mock_client.get(), OnMediaDurationChanged(time_delta))
    .Times(1);
  bridge->PropagateDuration(time_delta);

  EXPECT_EQ(bridge->duration_,time_delta);
}

TEST_F(OHOSMediaPlayerBridgeTests, GetDuration) {
  std::unique_ptr<MockPlayerAdapter> mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  EXPECT_TRUE(bridge->player_ != nullptr);
  std::unique_ptr<MockPlayerAdapter> mock_player_ = std::make_unique<MockPlayerAdapter>();
  bridge->prepared_ = true;
  EXPECT_TRUE(bridge->prepared_ = true);
  int32_t duration = 0;
  EXPECT_CALL(*mock_player_, GetDuration(duration))
    .WillRepeatedly(Return(0));
  bridge->GetDuration();
  bridge->prepared_ = false;
}

TEST_F(OHOSMediaPlayerBridgeTests, GetMediaTime) {
  bridge->pending_seek_ = time_delta;
  bridge-> prepared_ = false;
  base::TimeDelta time =bridge->GetMediaTime();
  EXPECT_EQ(time,time_delta);
}

TEST_F(OHOSMediaPlayerBridgeTests, SeekDone) {
  bridge->seek_complete_ = false;
  bridge->SeekDone();
  EXPECT_TRUE(bridge->seek_complete_);
  bridge->seek_complete_ = true;
}

TEST_F(OHOSMediaPlayerBridgeTests, FinishPaint) {
  std::unique_ptr<MoxkIConsumerSurfaceAdapter> mock_consumer_surface_buffer_ = std::make_unique<MoxkIConsumerSurfaceAdapter>();
  std::shared_ptr<MoxkSurfaceBufferAdapter> mock_surface_buffer = std::make_shared<MoxkSurfaceBufferAdapter>();
  bridge->cached_buffers_.push_back(mock_surface_buffer);
  auto mock_surface_ = std::make_shared<MoxkSurfaceBufferAdapter>();
  bridge->task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  int32_t id = 0;
  EXPECT_CALL(*mock_surface_,GetFileDescriptor())
    .WillRepeatedly(Return(id));
  EXPECT_CALL(*mock_consumer_surface_buffer_, ReleaseBuffer(std::move(bridge->cached_buffers_.front()), -1))
    .WillRepeatedly(Return(0));
  bridge->FinishPaint(id);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetPlaybackSpeed) {
  PlaybackRateMode mode = OHOS::NWeb::PlaybackRateMode::SPEED_FORWARD_1_00_X;
  std::unique_ptr<MockPlayerAdapter> mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  EXPECT_TRUE(bridge->player_ != nullptr);
  std::unique_ptr<MockPlayerAdapter> mock_player_ = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_, SetPlaybackSpeed(mode))
    .WillRepeatedly(Return(1));    
  bridge->SetPlaybackSpeed(mode);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnEnd) {
  auto mock_client = std::make_unique<MockClient>();
  bridge->client_ = mock_client.get();
  EXPECT_CALL(*mock_client, OnPlaybackComplete())
    .Times(1);    
  bridge->OnEnd();
}

TEST_F(OHOSMediaPlayerBridgeTests, OnError) {
  int32_t error_code = 0;
  auto mock_client = std::make_unique<MockClient>();
  bridge->client_ = mock_client.get();
  EXPECT_CALL(*mock_client, OnError(error_code))
    .Times(1);   
  bridge->OnError(error_code);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);    
  MockPlayerAdapter::PlayerStates PlayerStates  = PlayerAdapter::PLAYER_PREPARED;
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PLAYBACK_COMPLETE;
  bridge->seeking_on_playback_complete_ = true;
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_FALSE(bridge->seeking_on_playback_complete_);
  bridge->seeking_on_playback_complete_ = false;
}

TEST_F(OHOSMediaPlayerBridgeTests, OnBufferAvailable) {
  std::shared_ptr<MoxkSurfaceBufferAdapter> mock_surface_buffer = std::make_shared<MoxkSurfaceBufferAdapter>();
  bridge->cached_buffers_.push_back(mock_surface_buffer);
  auto mock_surface_ = std::make_shared<MoxkSurfaceBufferAdapter>();
  bridge->task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  EXPECT_CALL(*mock_surface_,GetFileDescriptor)
    .WillRepeatedly(Return(1));
  bridge->OnBufferAvailable(mock_surface_);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnVideoSizeChanged) {
  int32_t  width = 1, height = 1;
  auto mock_client = std::make_unique<MockClient>();
  bridge->client_ = mock_client.get();
  EXPECT_CALL(*mock_client.get(), OnVideoSizeChanged(width,height))
    .Times(1);
  bridge->OnVideoSizeChanged(width,height);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerInterruptEvent) {
  int32_t value = 1;
  auto mock_client = std::make_unique<MockClient>();
  bridge->client_ = mock_client.get();
  EXPECT_CALL(*mock_client, OnPlayerInterruptEvent(value))
    .Times(1);    
  bridge->OnPlayerInterruptEvent(value);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetFdSource) {
  std::string path = "ISONT";
  int32_t ret = bridge->SetFdSource(path);
  EXPECT_EQ(ret,-1);
}

TEST_F(OHOSMediaPlayerBridgeTests, IsAudible) {
  float volume_f = 1;
  bool ret = bridge->IsAudible(volume_f);
  EXPECT_TRUE(ret);
}