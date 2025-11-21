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

#include "base/test/task_environment.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#define protected public
#include "ohos_media_player_bridge.h"
#undef protected
#undef private

using namespace media;
using namespace testing;
using namespace OHOS::NWeb;
using namespace base;

static constexpr int DEFAULT_VALUE = -1;
static constexpr int DEFAULT_VALUE_ONE = 1;
static constexpr int DEFAULT_VALUE_TWO = 2;

class MockGURL : public GURL {
 public:
  MOCK_METHOD(bool, SchemeIsBlob, (), (const override));
  MOCK_METHOD(bool, SchemeIsFile, (), (const override));
  MOCK_METHOD(std::string, GetContent, (), (const override));
};

class MockClient : public OHOSMediaPlayerBridge::Client {
 public:
  virtual ~MockClient() {}
  MOCK_METHOD(void,
              OnFrameAvailable,
              (int, uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t),
              (override));
  MOCK_METHOD(void, OnMediaDurationChanged, (base::TimeDelta), (override));
  MOCK_METHOD(void, OnPlaybackComplete, (), (override));
  MOCK_METHOD(void, OnError, (int), (override));
  MOCK_METHOD(void, OnVideoSizeChanged, (int, int), (override));
  MOCK_METHOD(void, OnPlayerInterruptEvent, (int32_t), (override));
  MOCK_METHOD(void, OnAudioStateChanged, (bool), (override));
  MOCK_METHOD(void, OnPlayerSeekBack, (base::TimeDelta), (override));
  MOCK_METHOD(OHOSMediaResourceGetter*, GetMediaResourceGetter, (), (override));
};

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

class MockSurfaceBufferAdapter : public SurfaceBufferAdapter {
 public:
  MOCK_METHOD(int32_t, GetFileDescriptor, (), (override));
  MOCK_METHOD(int32_t, GetWidth, (), (override));
  MOCK_METHOD(int32_t, GetHeight, (), (override));
  MOCK_METHOD(int32_t, GetStride, (), (override));
  MOCK_METHOD(int32_t, GetFormat, (), (override));
  MOCK_METHOD(uint32_t, GetSize, (), (override));
  MOCK_METHOD(void*, GetVirAddr, (), (override));
};

class MockIConsumerSurfaceAdapter : public IConsumerSurfaceAdapter {
 public:
  MOCK_METHOD(int32_t,
              RegisterConsumerListener,
              (std::shared_ptr<IBufferConsumerListenerAdapter> listener),
              (override));
  MOCK_METHOD(int32_t,
              ReleaseBuffer,
              (std::shared_ptr<SurfaceBufferAdapter> buffer, int32_t fence),
              (override));
  MOCK_METHOD(int32_t,
              SetUserData,
              (const std::string& key, const std::string& val),
              (override));
  MOCK_METHOD(int32_t, SetQueueSize, (uint32_t queueSize), (override));
};

class MockMediaResourceGetter : public OHOSMediaResourceGetter {
 public:
  MOCK_METHOD(void,
              GetAuthCredentials,
              (const GURL& url, GetAuthCredentialsCB callback),
              (override));
  MOCK_METHOD(void,
              GetCookies,
              (const GURL& url,
               const net::SiteForCookies& site_for_cookies,
               const url::Origin& top_frame_origin,
               net::StorageAccessApiStatus storage_access_api_status,
               GetCookieCB callback),
              (override));
};

constexpr int player_init_ok = 0;
constexpr int milliseconds = 1000;
constexpr base::TimeDelta time_delta = base::Milliseconds(milliseconds);

class OHOSMediaPlayerBridgeTests : public ::testing::Test {
 public:
  void SetUp() override {
    gurl_ = std::make_unique<GURL>("http://example.com");
    mock_client_ = std::make_unique<MockClient>();

    bridge = new OHOSMediaPlayerBridge(
        *gurl_.get(), site_for_cookies, top_frame_origin, user_agent, storage_access_api_status_,
        hide_url_log, mock_client_.get(), allow_credentials, is_hls, headers, grantMediaFileAccessDirs);
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
  net::StorageAccessApiStatus storage_access_api_status_ = net::StorageAccessApiStatus::kAccessViaAPI;
  bool hide_url_log = false;
  bool allow_credentials = true;
  bool is_hls = false;
  base::flat_map<std::string, std::string> headers;
  std::vector<std::string> grantMediaFileAccessDirs;
  std::unique_ptr<MockClient> mock_client_;
  std::unique_ptr<GURL> gurl_;
  base::test::SingleThreadTaskEnvironment task_environment_{
      base::test::TaskEnvironment::TimeSource::MOCK_TIME};
};

TEST_F(OHOSMediaPlayerBridgeTests, Create) {
  std::vector<std::string> newDirs = {"test"};
  auto bridge_ = new OHOSMediaPlayerBridge(
      *gurl_.get(), site_for_cookies, top_frame_origin, user_agent,
      storage_access_api_status_, hide_url_log, mock_client_.get(),
      allow_credentials, is_hls, headers, newDirs);
  EXPECT_NE(bridge_, nullptr);
  delete bridge_;
  bridge_ = nullptr;
}

TEST_F(OHOSMediaPlayerBridgeTests, Initialize_001) {
  MockGURL url;
  bridge->url_ = url;
  EXPECT_CALL(url, SchemeIsBlob()).WillRepeatedly(Return(false));
  EXPECT_CALL(*mock_client_, GetMediaResourceGetter())
      .WillOnce(Return(nullptr));
  int32_t result = bridge->Initialize();
  task_environment_.RunUntilIdle();
  EXPECT_EQ(result, player_init_ok);
}

TEST_F(OHOSMediaPlayerBridgeTests, Initialize_002) {
  MockGURL url;
  bridge->url_ = url;
  EXPECT_CALL(url, SchemeIsBlob()).WillRepeatedly(Return(false));
  bridge->allow_credentials_ = false;
  int32_t result = bridge->Initialize();
  task_environment_.RunUntilIdle();
  EXPECT_EQ(result, player_init_ok);
}

TEST_F(OHOSMediaPlayerBridgeTests, Initialize_003) {
  MockGURL url;
  bridge->url_ = url;
  EXPECT_CALL(url, SchemeIsBlob()).WillRepeatedly(Return(false));
  bridge->client_ = nullptr;
  int32_t result = bridge->Initialize();
  task_environment_.RunUntilIdle();
  EXPECT_EQ(result, player_init_ok);
}

TEST_F(OHOSMediaPlayerBridgeTests, Initialize_004) {
  MockGURL url;
  bridge->url_ = url;
  EXPECT_CALL(url, SchemeIsBlob()).WillRepeatedly(Return(false));
  auto mock_resource_getter_ = new MockMediaResourceGetter();
  EXPECT_CALL(*mock_resource_getter_, GetCookies(_, _, _, _, _)).Times(1);
  EXPECT_CALL(*mock_client_, GetMediaResourceGetter())
      .WillOnce(Return(mock_resource_getter_));
  int32_t result = bridge->Initialize();
  task_environment_.RunUntilIdle();
  EXPECT_EQ(result, player_init_ok);
  EXPECT_TRUE(bridge->pending_retrieve_cookies_);
  delete mock_resource_getter_;
  mock_resource_getter_ = nullptr;
}

TEST_F(OHOSMediaPlayerBridgeTests, OnCookiesRetrieved_001) {
  std::string cookies = "test";
  bridge->client_ = nullptr;
  bridge->OnCookiesRetrieved(cookies);
  EXPECT_EQ(bridge->cookies_, cookies);
  EXPECT_FALSE(bridge->pending_retrieve_cookies_);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnCookiesRetrieved_002) {
  std::string cookies = "test";
  EXPECT_CALL(*mock_client_, GetMediaResourceGetter())
      .WillOnce(Return(nullptr));
  bridge->OnCookiesRetrieved(cookies);
  EXPECT_EQ(bridge->cookies_, cookies);
  EXPECT_FALSE(bridge->pending_retrieve_cookies_);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnCookiesRetrieved_003) {
  std::string cookies = "test";
  auto mock_resource_getter_ = new MockMediaResourceGetter();
  EXPECT_CALL(*mock_resource_getter_, GetAuthCredentials(_, _)).Times(1);
  EXPECT_CALL(*mock_client_, GetMediaResourceGetter())
      .WillRepeatedly(Return(mock_resource_getter_));
  bridge->OnCookiesRetrieved(cookies);
  delete mock_resource_getter_;
  mock_resource_getter_ = nullptr;
}

TEST_F(OHOSMediaPlayerBridgeTests, OnCookiesRetrieved_004) {
  std::string cookies = "test";
  auto mock_resource_getter_ = new MockMediaResourceGetter();
  EXPECT_CALL(*mock_resource_getter_, GetAuthCredentials(_, _)).Times(1);
  EXPECT_CALL(*mock_client_, GetMediaResourceGetter())
      .WillRepeatedly(Return(mock_resource_getter_));
  bridge->should_prepare_on_retrieved_cookies_ = true;
  bridge->OnCookiesRetrieved(cookies);
  EXPECT_FALSE(bridge->should_prepare_on_retrieved_cookies_);
  delete mock_resource_getter_;
  mock_resource_getter_ = nullptr;
}

TEST_F(OHOSMediaPlayerBridgeTests, OnCookiesRetrieved_005) {
  std::string cookies = "test";
  auto mock_resource_getter_ = new MockMediaResourceGetter();
  EXPECT_CALL(*mock_resource_getter_, GetAuthCredentials(_, _)).Times(1);
  EXPECT_CALL(*mock_client_, GetMediaResourceGetter())
      .WillRepeatedly(Return(mock_resource_getter_));
  bridge->should_prepare_on_retrieved_cookies_ = true;
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, SetMediaSourceHeader(_, _))
      .WillOnce(Return(1));
  EXPECT_CALL(*mock_player_adapter, SetSource(_)).WillOnce(Return(1));
  bridge->player_ = std::move(mock_player_adapter);
  testing::internal::CaptureStderr();
  bridge->OnCookiesRetrieved(cookies);
  std::string log_output = testing::internal::GetCapturedStderr();
  ASSERT_NE(log_output.find("SetPlayerSourceHeader error:ret="),
            std::string::npos);
  delete mock_resource_getter_;
  mock_resource_getter_ = nullptr;
}

TEST_F(OHOSMediaPlayerBridgeTests, OnCookiesRetrieved_006) {
  std::string cookies = "test";
  auto mock_resource_getter_ = new MockMediaResourceGetter();
  EXPECT_CALL(*mock_resource_getter_, GetAuthCredentials(_, _)).Times(1);
  EXPECT_CALL(*mock_client_, GetMediaResourceGetter())
      .WillRepeatedly(Return(mock_resource_getter_));
  bridge->should_prepare_on_retrieved_cookies_ = true;
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, SetMediaSourceHeader(_, _))
      .WillOnce(Return(0));
  EXPECT_CALL(*mock_player_adapter, SetSource(_)).Times(0);
  bridge->player_ = std::move(mock_player_adapter);
  bridge->OnCookiesRetrieved(cookies);
  delete mock_resource_getter_;
  mock_resource_getter_ = nullptr;
}

TEST_F(OHOSMediaPlayerBridgeTests, OnAuthCredentialsRetrieved_001) {
  std::u16string username = u"test";
  std::u16string password = u"test";
  bridge->OnAuthCredentialsRetrieved(username, password);
  EXPECT_NE(bridge->url_, *gurl_.get());
}

TEST_F(OHOSMediaPlayerBridgeTests, OnAuthCredentialsRetrieved_002) {
  std::u16string username = u"test";
  std::u16string password;
  bridge->OnAuthCredentialsRetrieved(username, password);
  EXPECT_NE(bridge->url_, *gurl_.get());
}

TEST_F(OHOSMediaPlayerBridgeTests, OnAuthCredentialsRetrieved_003) {
  std::u16string username;
  std::u16string password;
  bridge->OnAuthCredentialsRetrieved(username, password);
  EXPECT_EQ(bridge->url_, *gurl_.get());
}

TEST_F(OHOSMediaPlayerBridgeTests, GetPlayerHeadersInternal_001) {
  std::string cookies = "test";
  std::string user_agent_;
  auto bridge_ = new OHOSMediaPlayerBridge(
      *gurl_.get(), site_for_cookies, top_frame_origin, user_agent_,
      storage_access_api_status_, hide_url_log, mock_client_.get(),
      allow_credentials, is_hls, headers, grantMediaFileAccessDirs);
  bridge_->cookies_ = cookies;
  bridge_->headers_.emplace("test", "test");
  bridge_->GetPlayerHeadersInternal();
  delete bridge_;
  bridge_ = nullptr;
}

TEST_F(OHOSMediaPlayerBridgeTests, Start_001) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->prepared_ = false;
  bridge->pending_play_ = false;
  bridge->Start();
  EXPECT_TRUE(bridge->pending_play_);
}

TEST_F(OHOSMediaPlayerBridgeTests, Start_002) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(1);
  bridge->player_ = std::move(mock_player_adapter);
  bridge->prepared_ = true;
  bridge->Start();
  EXPECT_TRUE(bridge->prepared_);
  EXPECT_NE(bridge->player_, nullptr);
}

TEST_F(OHOSMediaPlayerBridgeTests, Start_003) {
  bridge->player_ = nullptr;
  bridge->pending_play_ = false;
  bridge->Start();
  EXPECT_TRUE(bridge->pending_play_);
}

TEST_F(OHOSMediaPlayerBridgeTests, Prepare_001) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->Prepare();
  EXPECT_NE(bridge->player_, nullptr);
}

TEST_F(OHOSMediaPlayerBridgeTests, Prepare_002) {
  bridge->player_ = nullptr;
  MockGURL url;
  bridge->url_ = url;
  EXPECT_CALL(url, SchemeIsBlob()).WillRepeatedly(Return(false));
  bridge->Prepare();
  EXPECT_TRUE(bridge->player_);
}

TEST_F(OHOSMediaPlayerBridgeTests, Prepare_003) {
  bridge->player_ = nullptr;
  MockGURL url;
  bridge->url_ = url;
  EXPECT_CALL(url, SchemeIsBlob()).WillRepeatedly(Return(true));
  bridge->Prepare();
}

TEST_F(OHOSMediaPlayerBridgeTests, SetPlayerSurface_001) {
  bridge->player_ = nullptr;
  testing::internal::CaptureStderr();
  bridge->SetPlayerSurface();
  std::string log_output = testing::internal::GetCapturedStderr();
  ASSERT_NE(
      log_output.find("OHOSMediaPlayerBridge SetPlayerSurface player is null"),
      std::string::npos);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetPlayerSurface_002) {
  bridge->native_window_origin_ = reinterpret_cast<void*>(0x1234);
  testing::internal::CaptureStderr();
  bridge->SetPlayerSurface();
  std::string log_output = testing::internal::GetCapturedStderr();
  ASSERT_EQ(log_output.find("OHOSMediaPlayerBridge GetNativeWindow is null"),
            std::string::npos);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetPlayerSurface_003) {
  bridge->native_window_origin_ = reinterpret_cast<void*>(0x1234);
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, PrepareAsync()).WillOnce(Return(1));
  EXPECT_CALL(*mock_player_adapter, SetVideoSurfaceNew(_)).WillOnce(Return(1));
  bridge->player_ = std::move(mock_player_adapter);
  testing::internal::CaptureStderr();
  bridge->SetPlayerSurface();
  std::string log_output = testing::internal::GetCapturedStderr();
  ASSERT_NE(log_output.find("Prepare error::ret="), std::string::npos);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetPlayerSurface_004) {
  bridge->native_window_origin_ = reinterpret_cast<void*>(0x1234);
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, PrepareAsync()).WillOnce(Return(0));
  EXPECT_CALL(*mock_player_adapter, SetVideoSurfaceNew(_)).WillOnce(Return(0));
  bridge->player_ = std::move(mock_player_adapter);
  testing::internal::CaptureStderr();
  bridge->SetPlayerSurface();
  std::string log_output = testing::internal::GetCapturedStderr();
  ASSERT_EQ(log_output.find("Prepare error::ret="), std::string::npos);
}

TEST_F(OHOSMediaPlayerBridgeTests, StartInternal_001) {
  bridge->prepared_ = true;
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Play()).Times(1);
  bridge->player_ = std::move(mock_player_adapter);
  bridge->StartInternal();
  EXPECT_NE(bridge->player_, nullptr);
  EXPECT_TRUE(bridge->player_);
}

TEST_F(OHOSMediaPlayerBridgeTests, StartInternal_002) {
  bridge->prepared_ = false;
  bridge->player_ = nullptr;
  bridge->StartInternal();
  EXPECT_FALSE(bridge->player_);
}

TEST_F(OHOSMediaPlayerBridgeTests, StartInternal_003) {
  bridge->prepared_ = false;
  bridge->player_ = std::make_unique<MockPlayerAdapter>();
  bridge->StartInternal();
  EXPECT_FALSE(bridge->prepared_);
}

TEST_F(OHOSMediaPlayerBridgeTests, StartInternal_004) {
  bridge->prepared_ = true;
  bridge->player_ = std::make_unique<MockPlayerAdapter>();
  bridge->pause_when_prepared_ = true;
  bridge->StartInternal();
  EXPECT_FALSE(bridge->pause_when_prepared_);
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause_001) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_IDLE;
  bridge->Pause();
  EXPECT_TRUE(bridge->player_);
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause_002) {
  bridge->player_ = nullptr;
  bridge->Pause();
  EXPECT_FALSE(bridge->player_);
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause__003) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Pause()).WillRepeatedly(Return(1));
  bridge->player_ = std::move(mock_player_adapter);
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_STARTED;
  bridge->Pause();
  EXPECT_EQ(bridge->player_state_, OHOS::NWeb::PlayerAdapter::PLAYER_STARTED);
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause__004) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Pause()).WillRepeatedly(Return(0));
  bridge->current_volume_ = 0;
  bridge->player_ = std::move(mock_player_adapter);
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_STARTED;
  bridge->Pause();
  auto ret = bridge->IsAudible(bridge->current_volume_);
  EXPECT_FALSE(ret);
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause__005) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, Pause()).WillRepeatedly(Return(0));
  bridge->current_volume_ = 1;
  bridge->player_ = std::move(mock_player_adapter);
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_STARTED;
  auto mock_client = std::make_unique<MockClient>();
  EXPECT_CALL(*mock_client, OnAudioStateChanged(false)).Times(1);
  bridge->client_ = mock_client.get();
  bridge->Pause();
  auto ret = bridge->IsAudible(bridge->current_volume_);
  EXPECT_TRUE(ret);
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause_006) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_PAUSED;
  testing::internal::CaptureStderr();
  bridge->Pause();
  std::string log_output = testing::internal::GetCapturedStderr();
  ASSERT_EQ(log_output.find(
                "OHOSMediaPlayerBridge Pause when perpared, player_state_ is:"),
            std::string::npos);
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause_007) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_STOPPED;
  testing::internal::CaptureStderr();
  bridge->Pause();
  std::string log_output = testing::internal::GetCapturedStderr();
  ASSERT_EQ(log_output.find(
                "OHOSMediaPlayerBridge Pause when perpared, player_state_ is:"),
            std::string::npos);
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause_008) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_PREPARED;
  testing::internal::CaptureStderr();
  bridge->Pause();
  std::string log_output = testing::internal::GetCapturedStderr();
  ASSERT_EQ(log_output.find(
                "OHOSMediaPlayerBridge Pause when perpared, player_state_ is:"),
            std::string::npos);
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause_009) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_PLAYBACK_COMPLETE;
  testing::internal::CaptureStderr();
  bridge->Pause();
  std::string log_output = testing::internal::GetCapturedStderr();
  ASSERT_EQ(log_output.find(
                "OHOSMediaPlayerBridge Pause when perpared, player_state_ is:"),
            std::string::npos);
}

TEST_F(OHOSMediaPlayerBridgeTests, Pause_010) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_PLAYBACK_COMPLETE;
  bridge->pending_play_ = true;
  testing::internal::CaptureStderr();
  bridge->Pause();
  std::string log_output = testing::internal::GetCapturedStderr();
  ASSERT_NE(log_output.find(
                "OHOSMediaPlayerBridge Pause when perpared, player_state_ is:"),
            std::string::npos);
}

TEST_F(OHOSMediaPlayerBridgeTests, SeekTo_002) {
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_STOPPED;
  bridge->prepared_ = true;
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter_,
              Seek(time_delta.InMilliseconds(),
                   OHOS::NWeb::PlayerSeekMode::SEEK_CLOSEST))
      .Times(1);
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->SeekTo(time_delta);
  EXPECT_FALSE(bridge->should_seek_on_prepare_);
}

TEST_F(OHOSMediaPlayerBridgeTests, SeekTo_003) {
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PLAYER_STOPPED;
  bridge->prepared_ = false;
  bridge->should_seek_on_prepare_ = false;
  bridge->SeekTo(time_delta);
  EXPECT_TRUE(bridge->should_seek_on_prepare_);
}

TEST_F(OHOSMediaPlayerBridgeTests, SeekInternal_001) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter_,
              Seek(time_delta.InMilliseconds(),
                   OHOS::NWeb::PlayerSeekMode::SEEK_CLOSEST))
      .WillRepeatedly(Return(1));
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->seek_complete_ = true;
  bridge->SeekInternal(time_delta);
  EXPECT_TRUE(bridge->seek_complete_);
}

TEST_F(OHOSMediaPlayerBridgeTests, SeekInternal_002) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter_,
              Seek(time_delta.InMilliseconds(),
                   OHOS::NWeb::PlayerSeekMode::SEEK_CLOSEST))
      .WillRepeatedly(Return(0));
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->seek_complete_ = true;
  bridge->SeekInternal(time_delta);
  EXPECT_FALSE(bridge->seek_complete_);
}

TEST_F(OHOSMediaPlayerBridgeTests, SeekInternal_003) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter_,
              Seek(time_delta.InMilliseconds(),
                   OHOS::NWeb::PlayerSeekMode::SEEK_PREVIOUS_SYNC))
      .WillRepeatedly(Return(0));
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->seeking_back_complete_ = true;
  testing::internal::CaptureStderr();
  bridge->SeekInternal(time_delta);
  EXPECT_FALSE(bridge->seek_complete_);
  std::string log_output = testing::internal::GetCapturedStderr();
  ASSERT_NE(
      log_output.find("OHOSMediaPlayerBridge::SeekTo mode=FAKE_SEEK"),
      std::string::npos);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVolume_001) {
  float volume_ = 0;
  bool is_muted = false;
  bridge->prepared_ = false;
  bridge->should_set_volume_on_prepare_ = false;
  bridge->SetVolume(volume_, is_muted);
  EXPECT_TRUE(bridge->should_set_volume_on_prepare_);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVolume_002) {
  float volume_ = 0;
  bool is_muted = false;
  bridge->prepared_ = true;
  bridge->player_ = nullptr;
  bridge->SetVolume(volume_, is_muted);
  EXPECT_FALSE(bridge->player_);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVolume_003) {
  float volume_ = 0;
  bridge->current_volume_ = 1;
  bool is_muted = false;
  bridge->prepared_ = true;
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, SetVolume(volume_, volume_))
      .WillRepeatedly(Return(1));
  bridge->player_ = std::move(mock_player_adapter);
  bridge->SetVolume(volume_, is_muted);
  EXPECT_NE(bridge->current_volume_, volume_);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVolume_004) {
  float volume_ = 0;
  bool is_muted = false;
  bridge->prepared_ = true;
  bridge->current_volume_ = 0;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED;
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, SetVolume(volume_, volume_))
      .WillRepeatedly(Return(0));
  bridge->player_ = std::move(mock_player_adapter);
  bridge->SetVolume(volume_, is_muted);
  EXPECT_EQ(bridge->player_state_,
            OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVolume_005) {
  float volume_ = 0;
  bool is_muted = false;
  bridge->prepared_ = true;
  bridge->current_volume_ = 1;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED;
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, SetVolume(volume_, volume_))
      .WillRepeatedly(Return(0));
  auto mock_client = std::make_unique<MockClient>();
  EXPECT_CALL(*mock_client, OnAudioStateChanged(false)).Times(1);
  bridge->client_ = mock_client.get();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->SetVolume(volume_, is_muted);
  EXPECT_EQ(bridge->player_state_,
            OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED);
  auto ret = bridge->IsAudible(bridge->current_volume_);
  EXPECT_FALSE(ret);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVolume_006) {
  float volume_ = 1;
  bool is_muted = true;
  bridge->prepared_ = true;
  bridge->current_volume_ = -1;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED;
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, SetVolume(0, 0)).WillRepeatedly(Return(0));
  auto mock_client = std::make_unique<MockClient>();
  EXPECT_CALL(*mock_client, OnAudioStateChanged(true)).Times(1);
  bridge->client_ = mock_client.get();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->SetVolume(volume_, is_muted);
  EXPECT_EQ(bridge->current_volume_, volume_);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVolume_007) {
  float volume_ = 1;
  bool is_muted = true;
  bridge->prepared_ = true;
  bridge->current_volume_ = -1;
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_IDLE;
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, SetVolume(0, 0)).WillRepeatedly(Return(0));
  auto mock_client = std::make_unique<MockClient>();
  EXPECT_CALL(*mock_client, OnAudioStateChanged(true)).Times(0);
  EXPECT_CALL(*mock_client, OnAudioStateChanged(false)).Times(0);
  bridge->client_ = mock_client.get();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->SetVolume(volume_, is_muted);
  EXPECT_EQ(bridge->current_volume_, volume_);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVolume_008) {
  float volume_ = 1;
  bool is_muted = true;
  bridge->prepared_ = true;
  bridge->current_volume_ = 1;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED;
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, SetVolume(0, 0)).WillRepeatedly(Return(0));
  auto mock_client = std::make_unique<MockClient>();
  EXPECT_CALL(*mock_client, OnAudioStateChanged(false)).Times(0);
  bridge->client_ = mock_client.get();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->SetVolume(volume_, is_muted);
  EXPECT_EQ(bridge->current_volume_, volume_);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVolume_009) {
  float volume_ = -1;
  bool is_muted = true;
  bridge->prepared_ = true;
  bridge->current_volume_ = 1;
  bridge->player_state_ = OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_IDLE;
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, SetVolume(0, 0)).WillRepeatedly(Return(0));
  auto mock_client = std::make_unique<MockClient>();
  EXPECT_CALL(*mock_client, OnAudioStateChanged(false)).Times(0);
  bridge->client_ = mock_client.get();
  bridge->player_ = std::move(mock_player_adapter);
  bridge->SetVolume(volume_, is_muted);
  EXPECT_EQ(bridge->current_volume_, volume_);
}

TEST_F(OHOSMediaPlayerBridgeTests, PropagateDuration) {
  auto mock_client = std::make_shared<MockClient>();
  EXPECT_CALL(*mock_client, OnMediaDurationChanged(time_delta)).Times(1);
  bridge->client_ = mock_client.get();
  bridge->PropagateDuration(time_delta);
  EXPECT_EQ(bridge->duration_, time_delta);
}

TEST_F(OHOSMediaPlayerBridgeTests, GetDuration_001) {
  int32_t value = -1;
  std::unique_ptr<MockPlayerAdapter> mock_player_adapter_ =
      std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter_, GetDuration(value)).Times(1);
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->prepared_ = true;
  bridge->GetDuration();
  EXPECT_NE(bridge->player_, nullptr);
  EXPECT_TRUE(bridge->prepared_);
}

TEST_F(OHOSMediaPlayerBridgeTests, GetDuration_002) {
  std::unique_ptr<MockPlayerAdapter> mock_player_adapter_ =
      std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->prepared_ = false;
  bridge->GetDuration();
  EXPECT_FALSE(bridge->prepared_);
}

TEST_F(OHOSMediaPlayerBridgeTests, GetDuration_003) {
  bridge->player_ = nullptr;
  bridge->GetDuration();
  EXPECT_FALSE(bridge->player_);
}

TEST_F(OHOSMediaPlayerBridgeTests, GetMediaTime_002) {
  bridge->pending_seek_ = time_delta;
  bridge->prepared_ = true;
  bridge->player_ = std::make_unique<MockPlayerAdapter>();
  bridge->seek_complete_ = false;
  bridge->fake_seek_complete_ = false;
  base::TimeDelta time = bridge->GetMediaTime();
  EXPECT_EQ(time, time_delta);
}

TEST_F(OHOSMediaPlayerBridgeTests, GetMediaTime_003) {
  bridge->pending_seek_ = time_delta;
  bridge->prepared_ = false;
  bridge->player_ = std::make_unique<MockPlayerAdapter>();
  base::TimeDelta time = bridge->GetMediaTime();
  EXPECT_EQ(time, time_delta);
}

TEST_F(OHOSMediaPlayerBridgeTests, GetMediaTime_004) {
  bridge->pending_seek_ = time_delta;
  bridge->player_ = nullptr;
  base::TimeDelta time = bridge->GetMediaTime();
  EXPECT_EQ(time, time_delta);
}

TEST_F(OHOSMediaPlayerBridgeTests, GetMediaTime_005) {
  int32_t value = -1;
  std::unique_ptr<MockPlayerAdapter> mock_player_adapter_ =
      std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter_, GetCurrentTime(value)).Times(1);
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->prepared_ = true;
  bridge->seek_complete_ = true;
  bridge->GetMediaTime();
  EXPECT_TRUE(bridge->prepared_);
  EXPECT_TRUE(bridge->seek_complete_);
}

TEST_F(OHOSMediaPlayerBridgeTests, GetMediaTime_006) {
  int32_t value = -1;
  std::unique_ptr<MockPlayerAdapter> mock_player_adapter_ =
      std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter_, GetCurrentTime(value)).WillOnce(Return(1));
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->prepared_ = true;
  bridge->fake_seek_complete_ = false;
  bridge->seek_complete_ = true;
  base::TimeDelta time = bridge->GetMediaTime();
  EXPECT_EQ(time, base::Milliseconds(value));
}

TEST_F(OHOSMediaPlayerBridgeTests, SeekDone) {
  bridge->seek_complete_ = false;
  bridge->SeekDone();
  EXPECT_TRUE(bridge->seek_complete_);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetPlaybackSpeed_001) {
  PlaybackRateMode mode = OHOS::NWeb::PlaybackRateMode::SPEED_FORWARD_1_00_X;
  std::unique_ptr<MockPlayerAdapter> mock_player_adapter_ =
      std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter_, SetPlaybackSpeed(mode))
      .WillOnce(Return(0));
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->SetPlaybackSpeed(mode);
  EXPECT_NE(bridge->player_, nullptr);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetPlaybackSpeed_002) {
  PlaybackRateMode mode = OHOS::NWeb::PlaybackRateMode::SPEED_FORWARD_1_00_X;
  std::unique_ptr<MockPlayerAdapter> mock_player_adapter_ =
      std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter_, SetPlaybackSpeed(mode))
      .WillOnce(Return(1));
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->SetPlaybackSpeed(mode);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetPlaybackSpeed_003) {
  PlaybackRateMode mode = OHOS::NWeb::PlaybackRateMode::SPEED_FORWARD_1_00_X;
  bridge->player_ = nullptr;
  bridge->SetPlaybackSpeed(mode);
  EXPECT_FALSE(bridge->player_);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnEnd) {
  auto mock_client = std::make_unique<MockClient>();
  EXPECT_CALL(*mock_client, OnPlaybackComplete()).Times(1);
  bridge->client_ = mock_client.get();
  bridge->OnEnd();
  EXPECT_NE(bridge->client_, nullptr);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnError) {
  int32_t error_code = 0;
  auto mock_client = std::make_unique<MockClient>();
  EXPECT_CALL(*mock_client, OnError(error_code)).Times(1);
  bridge->client_ = mock_client.get();
  bridge->OnError(error_code);
  EXPECT_NE(bridge->client_, nullptr);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_001) {
  bridge->player_ = nullptr;
  MockPlayerAdapter::PlayerStates PlayerStates = PlayerAdapter::PLAYER_PREPARED;
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_FALSE(bridge->player_);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_002) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates =
      PlayerAdapter::PLAYER_PLAYBACK_COMPLETE;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PLAYBACK_COMPLETE;
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_EQ(bridge->player_state_,
            OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PLAYBACK_COMPLETE);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_003) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates =
      PlayerAdapter::PLAYER_PLAYBACK_COMPLETE;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED;
  bridge->current_volume_ = 1;
  auto mock_client = std::make_unique<MockClient>();
  EXPECT_CALL(*mock_client, OnAudioStateChanged(false)).Times(1);
  bridge->client_ = mock_client.get();
  bridge->OnPlayerStateUpdate(PlayerStates);
  auto ret = bridge->IsAudible(bridge->current_volume_);
  EXPECT_TRUE(ret);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_004) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates =
      PlayerAdapter::PLAYER_PLAYBACK_COMPLETE;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED;
  bridge->current_volume_ = 0;
  bridge->OnPlayerStateUpdate(PlayerStates);
  auto ret = bridge->IsAudible(bridge->current_volume_);
  EXPECT_FALSE(ret);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_005) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates = PlayerAdapter::PLAYER_STARTED;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_RELEASED;
  bridge->current_volume_ = 1;
  auto mock_client = std::make_unique<MockClient>();
  EXPECT_CALL(*mock_client, OnAudioStateChanged(true)).Times(1);
  bridge->client_ = mock_client.get();
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_EQ(bridge->player_state_,
            OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_006) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates = PlayerAdapter::PLAYER_STARTED;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_RELEASED;
  bridge->current_volume_ = 0;
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_EQ(bridge->current_volume_, 0);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_07) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates =
      PlayerAdapter::PLAYER_STATE_ERROR;
  bridge->prepared_ = true;
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_FALSE(bridge->prepared_);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_08) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates = PlayerAdapter::PLAYER_IDLE;
  bridge->prepared_ = true;
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_FALSE(bridge->prepared_);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_09) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates =
      PlayerAdapter::PLAYER_INITIALIZED;
  bridge->prepared_ = true;
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_FALSE(bridge->prepared_);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_010) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates =
      PlayerAdapter::PLAYER_PREPARING;
  bridge->prepared_ = true;
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_FALSE(bridge->prepared_);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_011) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates =
      PlayerAdapter::PLAYER_PREPARING;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PLAYBACK_COMPLETE;
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_EQ(bridge->player_state_,
            OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PREPARING);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_012) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates = PlayerAdapter::PLAYER_PREPARED;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PREPARED;
  bridge->should_seek_on_prepare_ = true;
  EXPECT_EQ(bridge->player_state_,
            OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PREPARED);
  EXPECT_CALL(*static_cast<MockPlayerAdapter*>(bridge->player_.get()),
              GetDuration(testing::_))
      .Times(1)
      .WillOnce(Return(5));
  EXPECT_CALL(*mock_client_, OnMediaDurationChanged(testing::_)).Times(1);
  EXPECT_CALL(*static_cast<MockPlayerAdapter*>(bridge->player_.get()),
              Seek(testing::_, testing::_));
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_EQ(PlayerStates, bridge->player_state_);
  ASSERT_TRUE(bridge->prepared_);
  ASSERT_FALSE(bridge->should_seek_on_prepare_);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_013) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates = PlayerAdapter::PLAYER_PREPARED;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PREPARED;
  EXPECT_CALL(*static_cast<MockPlayerAdapter*>(bridge->player_.get()),
              GetDuration(testing::_))
      .Times(1)
      .WillOnce(Return(5));
  EXPECT_CALL(*mock_client_, OnMediaDurationChanged(testing::_)).Times(1);
  EXPECT_CALL(*static_cast<MockPlayerAdapter*>(bridge->player_.get()),
              SetVolume(testing::_, testing::_));
  bridge->should_seek_on_prepare_ = false;
  bridge->should_set_volume_on_prepare_ = true;
  ASSERT_TRUE(bridge->player_);
  EXPECT_EQ(bridge->player_state_,
            OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PREPARED);
  ASSERT_FALSE(bridge->should_seek_on_prepare_);
  ASSERT_TRUE(bridge->should_set_volume_on_prepare_);
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_EQ(PlayerStates, bridge->player_state_);
  ASSERT_TRUE(bridge->prepared_);
  ASSERT_FALSE(bridge->should_set_volume_on_prepare_);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_014) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates = PlayerAdapter::PLAYER_PREPARED;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PREPARED;
  EXPECT_CALL(*static_cast<MockPlayerAdapter*>(bridge->player_.get()),
              GetDuration(testing::_))
      .Times(1)
      .WillOnce(Return(5));
  EXPECT_CALL(*mock_client_, OnMediaDurationChanged(testing::_)).Times(1);
  EXPECT_CALL(*static_cast<MockPlayerAdapter*>(bridge->player_.get()), Play());
  bridge->should_seek_on_prepare_ = false;
  bridge->should_set_volume_on_prepare_ = false;
  bridge->pending_play_ = true;
  ASSERT_TRUE(bridge->player_);
  EXPECT_EQ(bridge->player_state_,
            OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PREPARED);
  ASSERT_FALSE(bridge->should_seek_on_prepare_);
  ASSERT_FALSE(bridge->should_set_volume_on_prepare_);
  ASSERT_TRUE(bridge->pending_play_);
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_EQ(PlayerStates, bridge->player_state_);
  ASSERT_FALSE(bridge->pending_play_);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_015) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates =
      PlayerAdapter::PLAYER_PREPARING;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_STARTED;
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_EQ(bridge->player_state_,
            OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PREPARING);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_016) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates = PlayerAdapter::PLAYER_PREPARED;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PREPARED;
  EXPECT_CALL(*static_cast<MockPlayerAdapter*>(bridge->player_.get()),
              GetDuration(testing::_))
      .Times(1)
      .WillOnce(Return(5));
  EXPECT_CALL(*mock_client_, OnMediaDurationChanged(testing::_)).Times(1);
  bridge->should_seek_on_prepare_ = false;
  bridge->should_set_volume_on_prepare_ = false;
  bridge->pending_play_ = false;
  bridge->pause_when_prepared_ = true;
  ASSERT_TRUE(bridge->player_);
  EXPECT_EQ(bridge->player_state_,
            OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_PREPARED);
  ASSERT_FALSE(bridge->should_seek_on_prepare_);
  ASSERT_FALSE(bridge->should_set_volume_on_prepare_);
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_EQ(PlayerStates, bridge->player_state_);
  ASSERT_FALSE(bridge->pause_when_prepared_);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerStateUpdate_017) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  MockPlayerAdapter::PlayerStates PlayerStates = PlayerAdapter::PLAYER_STARTED;
  bridge->player_state_ =
      OHOS::NWeb::PlayerAdapter::PlayerStates::PLAYER_RELEASED;
  bridge->current_volume_ = 0;
  bridge->OnPlayerStateUpdate(PlayerStates);
  EXPECT_EQ(bridge->current_volume_, 0);
}

TEST_F(OHOSMediaPlayerBridgeTests, CheckIsPathValid_001) {
  std::vector<std::string> newDirs = {"test"};
  auto bridge_ = new OHOSMediaPlayerBridge(
      *gurl_.get(), site_for_cookies, top_frame_origin, user_agent,
      storage_access_api_status_, hide_url_log, mock_client_.get(),
      allow_credentials, is_hls, headers, newDirs);
  EXPECT_NE(bridge_, nullptr);
  EXPECT_FALSE(bridge_->CheckIsPathValid("test"));
  delete bridge_;
  bridge_ = nullptr;
}

TEST_F(OHOSMediaPlayerBridgeTests, SetFdSource_001) {
  std::vector<std::string> newDirs = {"test"};
  auto bridge_ = new OHOSMediaPlayerBridge(
      *gurl_.get(), site_for_cookies, top_frame_origin, user_agent,
      storage_access_api_status_, hide_url_log, mock_client_.get(),
      allow_credentials, is_hls, headers, newDirs);
  EXPECT_NE(bridge_, nullptr);
  EXPECT_EQ(bridge_->SetFdSource("test"), -1);
  delete bridge_;
  bridge_ = nullptr;
}

TEST_F(OHOSMediaPlayerBridgeTests, OnVideoSizeChanged_001) {
  int32_t width = 1;
  int32_t height = 1;
  auto mock_client = std::make_unique<MockClient>();
  EXPECT_CALL(*mock_client, OnVideoSizeChanged(width, height)).Times(1);
  bridge->client_ = mock_client.get();
  bridge->OnVideoSizeChanged(width, height);
  EXPECT_NE(bridge->client_, nullptr);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnVideoSizeChanged_002) {
  int32_t width = 1;
  int32_t height = 1;
  bridge->client_ = nullptr;
  bridge->OnVideoSizeChanged(width, height);
  EXPECT_FALSE(bridge->client_);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerInterruptEvent_001) {
  int32_t value = 1;
  auto mock_client = std::make_unique<MockClient>();
  EXPECT_CALL(*mock_client, OnPlayerInterruptEvent(value)).Times(1);
  bridge->client_ = mock_client.get();
  bridge->OnPlayerInterruptEvent(value);
  EXPECT_NE(bridge->client_, nullptr);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnPlayerInterruptEvent_002) {
  int32_t value = 1;
  bridge->client_ = nullptr;
  bridge->OnPlayerInterruptEvent(value);
  EXPECT_FALSE(bridge->client_);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetFdSource) {
  std::string path = "ISONT";
  int32_t ret = bridge->SetFdSource(path);
  EXPECT_EQ(ret, -1);
}

TEST_F(OHOSMediaPlayerBridgeTests, IsAudible) {
  float volume_f = 1;
  bool ret = bridge->IsAudible(volume_f);
  EXPECT_TRUE(ret);
}

#if BUILDFLAG(ARKWEB_MEDIA)
TEST_F(OHOSMediaPlayerBridgeTests, uv__get_addr_tag01) {
  void* test_addr = reinterpret_cast<void*>(0x12345678);
  uint64_t ret = bridge->uv__get_addr_tag(test_addr);
  EXPECT_NE(ret, 0);
}

TEST_F(OHOSMediaPlayerBridgeTests, uv__get_addr_tag02) {
  uint64_t ret = bridge->uv__get_addr_tag(nullptr);
  EXPECT_EQ(ret, 0);
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
TEST_F(OHOSMediaPlayerBridgeTests, SetVideoSurfaceNew01) {
  int32_t surface_id = DEFAULT_VALUE;
  bridge->SetVideoSurfaceNew(surface_id);

  EXPECT_EQ(bridge->new_surface_id_, surface_id);
  EXPECT_EQ(bridge->pending_new_surface_id_, DEFAULT_VALUE);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVideoSurfaceNew02) {
  int32_t surface_id = DEFAULT_VALUE_ONE;
  bridge->new_surface_id_ = DEFAULT_VALUE_TWO;
  bridge->SetVideoSurfaceNew(surface_id);

  EXPECT_EQ(bridge->new_surface_id_, surface_id);
  EXPECT_EQ(bridge->pending_new_surface_id_, surface_id);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVideoSurfaceNew03) {
  int32_t surface_id = DEFAULT_VALUE_ONE;
  bridge->SetVideoSurfaceNew(surface_id);

  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);

  EXPECT_EQ(bridge->new_surface_id_, surface_id);
  EXPECT_EQ(bridge->pending_new_surface_id_, surface_id);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVideoSurfaceNew04) {
  int32_t surface_id = DEFAULT_VALUE_ONE;
  bridge->SetVideoSurfaceNew(surface_id);
  bridge->player_ = nullptr;

  EXPECT_EQ(bridge->new_surface_id_, surface_id);
  EXPECT_EQ(bridge->pending_new_surface_id_, surface_id);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVideoSurfaceOld01) {
  bridge->new_surface_id_ = DEFAULT_VALUE_TWO;
  bridge->player_ = nullptr;
  bridge->SetVideoSurfaceOld();

  EXPECT_EQ(bridge->new_surface_id_, DEFAULT_VALUE);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVideoSurfaceOld02) {
  bridge->player_ = nullptr;
  bridge->SetVideoSurfaceOld();

  EXPECT_EQ(bridge->new_surface_id_, DEFAULT_VALUE);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVideoSurfaceOld03) {
  bridge->new_surface_id_ = DEFAULT_VALUE_TWO;
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);

  bridge->SetVideoSurfaceOld();

  EXPECT_EQ(bridge->new_surface_id_, DEFAULT_VALUE);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVideoSurfaceOld04) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter);

  bridge->SetVideoSurfaceOld();

  EXPECT_EQ(bridge->new_surface_id_, DEFAULT_VALUE);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVideoSurfaceOld05) {
  auto mock_player_adapter = std::make_unique<MockPlayerAdapter>();
  EXPECT_CALL(*mock_player_adapter, SetVideoSurfaceNew(_))
      .WillRepeatedly(Return(-1));
  bridge->player_ = std::move(mock_player_adapter);
  bridge->SetVideoSurfaceOld();
  EXPECT_EQ(bridge->new_surface_id_, DEFAULT_VALUE);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVideoSurface01) {
  int32_t surface_id = DEFAULT_VALUE_ONE;
  bridge->SetVideoSurface(surface_id);

  EXPECT_EQ(bridge->new_surface_id_, surface_id);
}

TEST_F(OHOSMediaPlayerBridgeTests, SetVideoSurface02) {
  int32_t surface_id = DEFAULT_VALUE;
  bridge->SetVideoSurface(surface_id);

  EXPECT_EQ(bridge->new_surface_id_, DEFAULT_VALUE);
}
#endif

#if BUILDFLAG(ARKWEB_PIP)
TEST_F(OHOSMediaPlayerBridgeTests, PipEnable01) {
  bridge->PipEnable(true);

  EXPECT_EQ(bridge->new_surface_id_, DEFAULT_VALUE);
}

TEST_F(OHOSMediaPlayerBridgeTests, PipEnable02) {
  bridge->PipEnable(false);

  EXPECT_EQ(bridge->new_surface_id_, DEFAULT_VALUE);
}
#endif

TEST_F(OHOSMediaPlayerBridgeTests, OnSeekBack01) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->pending_seek_ = base::Milliseconds(0);
  bridge->recording_seek_ = base::Milliseconds(0);

  bridge->OnSeekBack(base::Milliseconds(0));
  EXPECT_EQ(bridge->recording_seek_, base::Milliseconds(0));
}

TEST_F(OHOSMediaPlayerBridgeTests, OnSeekBack02) {
  bridge->player_ = nullptr;
  bridge->pending_seek_ = base::Milliseconds(1);
  bridge->recording_seek_ = base::Milliseconds(0);

  bridge->OnSeekBack(base::Milliseconds(0));
  EXPECT_EQ(bridge->recording_seek_, base::Milliseconds(0));
}

TEST_F(OHOSMediaPlayerBridgeTests, OnSeekBack03) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->pending_seek_ = base::Milliseconds(1);
  bridge->recording_seek_ = base::Milliseconds(0);

  bridge->OnSeekBack(base::Milliseconds(0));
  EXPECT_EQ(bridge->recording_seek_, base::Milliseconds(0));
}

TEST_F(OHOSMediaPlayerBridgeTests, OnSeekBack04) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->pending_seek_ = base::Milliseconds(1);
  bridge->recording_seek_ = base::Milliseconds(0);
  bridge->seek_complete_ = false;

  bridge->OnSeekBack(base::Milliseconds(0));
  EXPECT_EQ(bridge->recording_seek_, base::Milliseconds(-1));
}

TEST_F(OHOSMediaPlayerBridgeTests, OnSeekBack05) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->pending_seek_ = base::Milliseconds(1);
  bridge->recording_seek_ = base::Milliseconds(0);
  bridge->seek_complete_ = true;
  bridge->seeking_back_complete_ = true;

  bridge->OnSeekBack(base::Milliseconds(0));
  EXPECT_EQ(bridge->recording_seek_, base::Milliseconds(0));
}

TEST_F(OHOSMediaPlayerBridgeTests, OnSeekBack06) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->pending_seek_ = base::Milliseconds(1);
  bridge->recording_seek_ = base::Milliseconds(400);
  bridge->seek_complete_ = true;
  bridge->seeking_back_complete_ = false;

  bridge->OnSeekBack(base::Milliseconds(0));
  EXPECT_EQ(bridge->seeking_back_complete_, true);
}

TEST_F(OHOSMediaPlayerBridgeTests, OnSeekBack07) {
  auto mock_player_adapter_ = std::make_unique<MockPlayerAdapter>();
  bridge->player_ = std::move(mock_player_adapter_);
  bridge->pending_seek_ = base::Milliseconds(1);
  bridge->recording_seek_ = base::Milliseconds(400);
  bridge->seek_complete_ = true;
  bridge->seeking_back_complete_ = false;
  bridge->client_ = nullptr;
  bridge->OnSeekBack(base::Milliseconds(0));
  EXPECT_FALSE(bridge->seeking_back_complete_);
}