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

#include "ohos_nweb/src/native_media_player/nweb_native_media_player_handler_impl.h"

#include <gmock/gmock.h>
#include "base/logging.h"
#include "gtest/gtest.h"

namespace OHOS {
using namespace OHOS::NWeb;
using ::testing::_;
using ::testing::Return;

class MockCefMediaPlayerListener : public CefMediaPlayerListener {
 public:
  MOCK_METHOD1(OnStatusChanged, void(uint32_t status));
  MOCK_METHOD1(OnVolumeChanged, void(double volume));
  MOCK_METHOD1(OnMutedChanged, void(bool muted));
  MOCK_METHOD1(OnPlaybackRateChanged, void(double playback_rate));
  MOCK_METHOD1(OnDurationChanged, void(double duration));
  MOCK_METHOD1(OnTimeUpdate, void(double current_time));
  MOCK_METHOD1(OnBufferedEndTimeChanged, void(double buffered_time));
  MOCK_METHOD0(OnEnded, void());
  MOCK_METHOD1(OnNetworkStateChanged, void(uint32_t state));
  MOCK_METHOD1(OnReadyStateChanged, void(uint32_t state));
  MOCK_METHOD1(OnFullscreenChanged, void(bool fullscreen));
  MOCK_METHOD0(OnSeeking, void());
  MOCK_METHOD0(OnSeekFinished, void());
  MOCK_METHOD2(OnError, void(uint32_t error_code, const CefString& error_msg));
  MOCK_METHOD2(OnVideoSizeChanged, void(int width, int height));
  void AddRef() const override {
    ref_count_++;
  }
    
  bool Release() const override {
    return --ref_count_ == 0;
  }
  
  bool HasOneRef() const override {
    return ref_count_ == 1;
  }
  
  bool HasAtLeastOneRef() const override {
    return ref_count_ >= 1;
  }

 private:
  mutable int ref_count_ = 1;
};

class NWebNativeMediaPlayerHandlerImplTest : public ::testing::Test {
 protected:
  void SetUp() override {
    mock_listener_ = new MockCefMediaPlayerListener();
  }

  void TearDown() override {
    if (listener_ptr_) {
      listener_ptr_.reset();
    }
  }

  MockCefMediaPlayerListener* mock_listener_;
  CefOwnPtr<CefMediaPlayerListener> listener_ptr_;
};

TEST_F(NWebNativeMediaPlayerHandlerImplTest, WithListener_AllMethodsCallListener01) {
  listener_ptr_ = CefOwnPtr<CefMediaPlayerListener>(mock_listener_);
  NWebNativeMediaPlayerHandlerImpl nptrhandler(nullptr);

  EXPECT_CALL(*mock_listener_, OnStatusChanged(_)).Times(1);
  EXPECT_CALL(*mock_listener_, OnVolumeChanged(_)).Times(1);
  EXPECT_CALL(*mock_listener_, OnMutedChanged(_)).Times(1);
  EXPECT_CALL(*mock_listener_, OnPlaybackRateChanged(_)).Times(1);
  EXPECT_CALL(*mock_listener_, OnDurationChanged(_)).Times(1);
  EXPECT_CALL(*mock_listener_, OnTimeUpdate(_)).Times(1);
  EXPECT_CALL(*mock_listener_, OnBufferedEndTimeChanged(_)).Times(1);

  nptrhandler.HandleStatusChanged(PlaybackStatus::PLAYING);
  nptrhandler.HandleVolumeChanged(0.8);
  nptrhandler.HandleMutedChanged(true);
  nptrhandler.HandlePlaybackRateChanged(1.0);
  nptrhandler.HandleDurationChanged(120.0);
  nptrhandler.HandleTimeUpdate(60.0);
  nptrhandler.HandleBufferedEndTimeChanged(90.0);

  NWebNativeMediaPlayerHandlerImpl handler(std::move(listener_ptr_));

  handler.HandleStatusChanged(PlaybackStatus::PLAYING);
  handler.HandleVolumeChanged(0.8);
  handler.HandleMutedChanged(true);
  handler.HandlePlaybackRateChanged(1.0);
  handler.HandleDurationChanged(120.0);
  handler.HandleTimeUpdate(60.0);
  handler.HandleBufferedEndTimeChanged(90.0);
}

TEST_F(NWebNativeMediaPlayerHandlerImplTest, WithListener_AllMethodsCallListener02) {
  listener_ptr_ = CefOwnPtr<CefMediaPlayerListener>(mock_listener_);
  NWebNativeMediaPlayerHandlerImpl nptrhandler(nullptr);

  EXPECT_CALL(*mock_listener_, OnEnded()).Times(1);
  EXPECT_CALL(*mock_listener_, OnNetworkStateChanged(_)).Times(1);
  EXPECT_CALL(*mock_listener_, OnReadyStateChanged(_)).Times(1);
  EXPECT_CALL(*mock_listener_, OnFullscreenChanged(_)).Times(1);
  EXPECT_CALL(*mock_listener_, OnSeeking()).Times(1);
  EXPECT_CALL(*mock_listener_, OnSeekFinished()).Times(1);
  EXPECT_CALL(*mock_listener_, OnError(_, _)).Times(1);
  EXPECT_CALL(*mock_listener_, OnVideoSizeChanged(_, _)).Times(1);

  nptrhandler.HandleEnded();
  nptrhandler.HandleNetworkStateChanged(NetworkState::IDLE);
  nptrhandler.HandleReadyStateChanged(ReadyState::HAVE_ENOUGH_DATA);
  nptrhandler.HandleFullScreenChanged(true);
  nptrhandler.HandleSeeking();
  nptrhandler.HandleSeekFinished();
  nptrhandler.HandleError(MediaError::DECODE_ERROR, "Decode error");
  nptrhandler.HandleVideoSizeChanged(1920.0, 1080.0);

  NWebNativeMediaPlayerHandlerImpl handler(std::move(listener_ptr_));

  handler.HandleEnded();
  handler.HandleNetworkStateChanged(NetworkState::IDLE);
  handler.HandleReadyStateChanged(ReadyState::HAVE_ENOUGH_DATA);
  handler.HandleFullScreenChanged(true);
  handler.HandleSeeking();
  handler.HandleSeekFinished();
  handler.HandleError(MediaError::DECODE_ERROR, "Decode error");
  handler.HandleVideoSizeChanged(1920.0, 1080.0);
}

TEST_F(NWebNativeMediaPlayerHandlerImplTest, ParameterValidation_ValuesPassedCorrectly) {
  listener_ptr_ = CefOwnPtr<CefMediaPlayerListener>(mock_listener_);
  NWebNativeMediaPlayerHandlerImpl handler(std::move(listener_ptr_));

  EXPECT_CALL(*mock_listener_, OnStatusChanged(static_cast<uint32_t>(PlaybackStatus::PAUSED))).Times(1);
  EXPECT_CALL(*mock_listener_, OnVolumeChanged(0.5)).Times(1);
  EXPECT_CALL(*mock_listener_, OnMutedChanged(false)).Times(1);
  EXPECT_CALL(*mock_listener_, OnPlaybackRateChanged(1.5)).Times(1);
  EXPECT_CALL(*mock_listener_, OnDurationChanged(300.0)).Times(1);
  EXPECT_CALL(*mock_listener_, OnTimeUpdate(150.0)).Times(1);
  EXPECT_CALL(*mock_listener_, OnBufferedEndTimeChanged(200.0)).Times(1);
  EXPECT_CALL(*mock_listener_, OnNetworkStateChanged(static_cast<uint32_t>(NetworkState::LOADING))).Times(1);
  EXPECT_CALL(*mock_listener_, OnReadyStateChanged(static_cast<uint32_t>(ReadyState::HAVE_CURRENT_DATA))).Times(1);
  EXPECT_CALL(*mock_listener_, OnFullscreenChanged(true)).Times(1);
  EXPECT_CALL(*mock_listener_, OnError(static_cast<uint32_t>(MediaError::NETWORK_ERROR), _)).Times(1);
  EXPECT_CALL(*mock_listener_, OnVideoSizeChanged(1280, 720)).Times(1);

  handler.HandleStatusChanged(PlaybackStatus::PAUSED);
  handler.HandleVolumeChanged(0.5);
  handler.HandleMutedChanged(false);
  handler.HandlePlaybackRateChanged(1.5);
  handler.HandleDurationChanged(300.0);
  handler.HandleTimeUpdate(150.0);
  handler.HandleBufferedEndTimeChanged(200.0);
  handler.HandleNetworkStateChanged(NetworkState::LOADING);
  handler.HandleReadyStateChanged(ReadyState::HAVE_CURRENT_DATA);
  handler.HandleFullScreenChanged(true);
  handler.HandleError(MediaError::NETWORK_ERROR, CefString("Network error"));
  handler.HandleVideoSizeChanged(1280.0, 720.0);
}
} // namespace OHOS