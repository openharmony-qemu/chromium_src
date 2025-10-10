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

#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_window_adapter_impl.h"
#define private public
#include "player_framework_adapter_impl.h"
#undef private
#include <gtest/gtest.h>

#include "gmock/gmock.h"
#include "player_framework_adapter_impl.cpp"

using namespace testing;

namespace OHOS::NWeb {
class PlayerAdapterImplTest : public testing::Test {
 protected:
  void SetUp() { playerAdapter_ = std::make_unique<PlayerAdapterImpl>(); }

  std::unique_ptr<PlayerAdapterImpl> playerAdapter_ = nullptr;
  const std::string sourceUrl_ = "sourcetest.mp3";
  int32_t current_time_ = 0;
  int32_t duration_ = 0;
};

TEST_F(PlayerAdapterImplTest, TestConverterState) {
  EXPECT_EQ(ConverterState(AVPlayerState::AV_IDLE),
            PlayerAdapter::PlayerStates::PLAYER_IDLE);
  EXPECT_EQ(ConverterState(AVPlayerState::AV_INITIALIZED),
            PlayerAdapter::PlayerStates::PLAYER_INITIALIZED);
  EXPECT_EQ(ConverterState(AVPlayerState::AV_PREPARED),
            PlayerAdapter::PlayerStates::PLAYER_PREPARED);
  EXPECT_EQ(ConverterState(AVPlayerState::AV_PLAYING),
            PlayerAdapter::PlayerStates::PLAYER_STARTED);
  EXPECT_EQ(ConverterState(AVPlayerState::AV_PAUSED),
            PlayerAdapter::PlayerStates::PLAYER_PAUSED);
  EXPECT_EQ(ConverterState(AVPlayerState::AV_STOPPED),
            PlayerAdapter::PlayerStates::PLAYER_STOPPED);
  EXPECT_EQ(ConverterState(AVPlayerState::AV_COMPLETED),
            PlayerAdapter::PlayerStates::PLAYER_PLAYBACK_COMPLETE);
  EXPECT_EQ(ConverterState(AVPlayerState::AV_RELEASED),
            PlayerAdapter::PlayerStates::PLAYER_RELEASED);
  EXPECT_EQ(ConverterState(AVPlayerState::AV_ERROR),
            PlayerAdapter::PlayerStates::PLAYER_STATE_ERROR);
  int32_t unknown_state = 111;
  EXPECT_EQ(ConverterState(unknown_state), unknown_state);
}

TEST_F(PlayerAdapterImplTest, TestIsUnsupportType) {
  EXPECT_TRUE(IsUnsupportType(OH_AVErrCode::AV_ERR_DRM_BASE));
  EXPECT_TRUE(IsUnsupportType(OH_AVErrCode::AV_ERR_DRM_DECRYPT_FAILED));
  EXPECT_TRUE(IsUnsupportType(OH_AVErrCode::AV_ERR_VIDEO_BASE));
  EXPECT_TRUE(IsUnsupportType(
      OH_AVErrCode::AV_ERR_VIDEO_UNSUPPORTED_COLOR_SPACE_CONVERSION));
  EXPECT_FALSE(IsUnsupportType(OH_AVErrCode::AV_ERR_IO_UNSUPPORTED_REQUEST));
}

TEST_F(PlayerAdapterImplTest, TestIsFatalError) {
  EXPECT_TRUE(IsFatalError(OH_AVErrCode::AV_ERR_NO_MEMORY));
  EXPECT_TRUE(IsFatalError(OH_AVErrCode::AV_ERR_IO));
  EXPECT_TRUE(IsFatalError(OH_AVErrCode::AV_ERR_SERVICE_DIED));
  EXPECT_TRUE(IsFatalError(OH_AVErrCode::AV_ERR_EXTEND_START));
  EXPECT_TRUE(IsFatalError(OH_AVErrCode::AV_ERR_IO_CANNOT_FIND_HOST));
  EXPECT_TRUE(IsFatalError(OH_AVErrCode::AV_ERR_IO_NETWORK_UNAVAILABLE));
  EXPECT_TRUE(IsFatalError(OH_AVErrCode::AV_ERR_IO_NO_PERMISSION));
  EXPECT_TRUE(IsFatalError(OH_AVErrCode::AV_ERR_IO_NETWORK_ACCESS_DENIED));
  EXPECT_TRUE(IsFatalError(OH_AVErrCode::AV_ERR_IO_RESOURCE_NOT_FOUND));
  EXPECT_TRUE(IsFatalError(OH_AVErrCode::AV_ERR_IO_UNSUPPORTED_REQUEST));
  EXPECT_FALSE(IsFatalError(OH_AVErrCode::AV_ERR_VIDEO_BASE));
}

TEST_F(PlayerAdapterImplTest, TestConvertInfoType) {
  EXPECT_EQ(ConvertInfoType(AVPlayerOnInfoType::AV_INFO_TYPE_SEEKDONE),
            NWeb::PlayerOnInfoType::INFO_TYPE_SEEKDONE);
  EXPECT_EQ(ConvertInfoType(AVPlayerOnInfoType::AV_INFO_TYPE_EOS),
            NWeb::PlayerOnInfoType::INFO_TYPE_EOS);
  EXPECT_EQ(ConvertInfoType(AVPlayerOnInfoType::AV_INFO_TYPE_STATE_CHANGE),
            NWeb::PlayerOnInfoType::INFO_TYPE_STATE_CHANGE);
  EXPECT_EQ(ConvertInfoType(AVPlayerOnInfoType::AV_INFO_TYPE_POSITION_UPDATE),
            NWeb::PlayerOnInfoType::INFO_TYPE_POSITION_UPDATE);
  EXPECT_EQ(ConvertInfoType(AVPlayerOnInfoType::AV_INFO_TYPE_MESSAGE),
            NWeb::PlayerOnInfoType::INFO_TYPE_MESSAGE);
  EXPECT_EQ(ConvertInfoType(AVPlayerOnInfoType::AV_INFO_TYPE_INTERRUPT_EVENT),
            NWeb::PlayerOnInfoType::INFO_TYPE_INTERRUPT_EVENT);
  EXPECT_EQ(ConvertInfoType(AVPlayerOnInfoType::AV_INFO_TYPE_RESOLUTION_CHANGE),
            NWeb::PlayerOnInfoType::INFO_TYPE_RESOLUTION_CHANGE);
  EXPECT_EQ(ConvertInfoType(AVPlayerOnInfoType::AV_INFO_TYPE_IS_LIVE_STREAM),
            NWeb::PlayerOnInfoType::INFO_TYPE_UNSET);
}

TEST_F(PlayerAdapterImplTest, TestConvertSeekMode) {
  EXPECT_EQ(ConvertSeekMode(NWeb::PlayerSeekMode::SEEK_NEXT_SYNC),
            AVPlayerSeekMode::AV_SEEK_NEXT_SYNC);
  EXPECT_EQ(ConvertSeekMode(NWeb::PlayerSeekMode::SEEK_PREVIOUS_SYNC),
            AVPlayerSeekMode::AV_SEEK_PREVIOUS_SYNC);
  EXPECT_EQ(ConvertSeekMode(NWeb::PlayerSeekMode::SEEK_CLOSEST_SYNC),
            AVPlayerSeekMode::AV_SEEK_CLOSEST);
  EXPECT_EQ(ConvertSeekMode(NWeb::PlayerSeekMode::SEEK_CLOSEST),
            AVPlayerSeekMode::AV_SEEK_CLOSEST);
}

TEST_F(PlayerAdapterImplTest, TestConvertRateMode) {
  EXPECT_EQ(ConvertRateMode(NWeb::PlaybackRateMode::SPEED_FORWARD_0_75_X),
            AVPlaybackSpeed::AV_SPEED_FORWARD_0_75_X);
  EXPECT_EQ(ConvertRateMode(NWeb::PlaybackRateMode::SPEED_FORWARD_1_00_X),
            AVPlaybackSpeed::AV_SPEED_FORWARD_1_00_X);
  EXPECT_EQ(ConvertRateMode(NWeb::PlaybackRateMode::SPEED_FORWARD_1_25_X),
            AVPlaybackSpeed::AV_SPEED_FORWARD_1_25_X);
  EXPECT_EQ(ConvertRateMode(NWeb::PlaybackRateMode::SPEED_FORWARD_1_75_X),
            AVPlaybackSpeed::AV_SPEED_FORWARD_1_75_X);
  EXPECT_EQ(ConvertRateMode(NWeb::PlaybackRateMode::SPEED_FORWARD_2_00_X),
            AVPlaybackSpeed::AV_SPEED_FORWARD_2_00_X);
}

TEST_F(PlayerAdapterImplTest, TestGetErrorCode001) {
  int expect_result = 0;
  int actual_result = GetErrorCode(OH_AVErrCode::AV_ERR_OK);
  EXPECT_EQ(actual_result, expect_result);
}

TEST_F(PlayerAdapterImplTest, TestGetErrorCode002) {
  int expect_result = 1;
  int actual_result = GetErrorCode(AV_ERR_NO_MEMORY);
  EXPECT_EQ(actual_result, expect_result);
}

TEST_F(PlayerAdapterImplTest, TestPlayerInfoCallback001) {
  testing::internal::CaptureStderr();
  PlayerInfoCallback(playerAdapter_->player_,
                     AVPlayerOnInfoType::AV_INFO_TYPE_STATE_CHANGE, nullptr,
                     nullptr);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("userData is null"), std::string::npos);
}

TEST_F(PlayerAdapterImplTest, TestPlayerErrorCallback001) {
  testing::internal::CaptureStderr();
  PlayerErrorCallback(playerAdapter_->player_, 0, nullptr, nullptr);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("userData is null"), std::string::npos);
}

TEST_F(PlayerAdapterImplTest, TestSetPlayerCallback001) {
  int32_t expect_result = -1;
  playerAdapter_->player_ = nullptr;
  int32_t actual_result = playerAdapter_->SetPlayerCallback(nullptr);
  EXPECT_EQ(actual_result, expect_result);
}

TEST_F(PlayerAdapterImplTest, TestSetPlayerCallback002) {
  int32_t expect_result = -1;
  int32_t actual_result = playerAdapter_->SetPlayerCallback(nullptr);
  EXPECT_EQ(actual_result, expect_result);
}

TEST_F(PlayerAdapterImplTest, TestSetSource001) {
  int32_t expected_result = -1;
  playerAdapter_->player_ = nullptr;
  int32_t actual_result = playerAdapter_->SetSource(sourceUrl_);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestSetSource002) {
  int32_t expected_result = 3;
  int32_t actual_result = playerAdapter_->SetSource(sourceUrl_);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestSetSource003) {
  int32_t expected_result = -1;
  playerAdapter_->player_ = nullptr;
  int32_t actual_result = playerAdapter_->SetSource(0, 1, 2);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestSetSource004) {
  int32_t expected_result = 0;
  int32_t actual_result = playerAdapter_->SetSource(0, 1, 2);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestSetVideoSurface001) {
  int32_t expected_result = -1;
  playerAdapter_->player_ = nullptr;
  int32_t actual_result = playerAdapter_->SetVideoSurface(nullptr);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestSetVideoSurface002) {
  int32_t expected_result = -1;
  int32_t actual_result = playerAdapter_->SetVideoSurface(nullptr);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestSetVolume001) {
  int32_t expected_result = -1;
  playerAdapter_->player_ = nullptr;
  int32_t actual_result = playerAdapter_->SetVolume(1.0, 1.0);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestSetVolume002) {
  int32_t expected_result = 0;
  int32_t actual_result = playerAdapter_->SetVolume(1.0, 1.0);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestSeek001) {
  int32_t expected_result = -1;
  playerAdapter_->player_ = nullptr;
  int32_t actual_result =
      playerAdapter_->Seek(0, NWeb::PlayerSeekMode::SEEK_CLOSEST_SYNC);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestSeek002) {
  int32_t expected_result = 3;
  int32_t actual_result =
      playerAdapter_->Seek(0, NWeb::PlayerSeekMode::SEEK_CLOSEST_SYNC);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestPlay001) {
  int32_t expected_result = -1;
  playerAdapter_->player_ = nullptr;
  int32_t actual_result = playerAdapter_->Play();
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestPlay002) {
  int32_t expected_result = 3;
  int32_t actual_result = playerAdapter_->Play();
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestPause001) {
  int32_t expected_result = -1;
  playerAdapter_->player_ = nullptr;
  int32_t actual_result = playerAdapter_->Pause();
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestPause002) {
  int32_t expected_result = 3;
  int32_t actual_result = playerAdapter_->Pause();
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestPrepareAsync001) {
  int32_t expected_result = -1;
  playerAdapter_->player_ = nullptr;
  int32_t actual_result = playerAdapter_->Pause();
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestPrepareAsync002) {
  int32_t expected_result = 3;
  int32_t actual_result = playerAdapter_->Pause();
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestGetCurrentTime001) {
  int32_t expected_result = -1;
  playerAdapter_->player_ = nullptr;
  int32_t actual_result = playerAdapter_->GetCurrentTime(current_time_);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestGetCurrentTime002) {
  int32_t expected_result = 3;
  int32_t actual_result = playerAdapter_->GetCurrentTime(current_time_);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestGetDuration001) {
  int32_t expected_result = -1;
  playerAdapter_->player_ = nullptr;
  int32_t actual_result = playerAdapter_->GetDuration(duration_);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestGetDuration002) {
  int32_t expected_result = 3;
  int32_t actual_result = playerAdapter_->GetDuration(duration_);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestSetPlaybackSpeed001) {
  int32_t expected_result = -1;
  playerAdapter_->player_ = nullptr;
  int32_t actual_result = playerAdapter_->SetPlaybackSpeed(
      NWeb::PlaybackRateMode::SPEED_FORWARD_1_00_X);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestSetPlaybackSpeed002) {
  int32_t expected_result = 3;
  int32_t actual_result = playerAdapter_->SetPlaybackSpeed(
      NWeb::PlaybackRateMode::SPEED_FORWARD_1_00_X);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestSetVideoSurfaceNew001) {
  int32_t expected_result = -1;
  playerAdapter_->player_ = nullptr;
  void* native_window = nullptr;
  int32_t actual_result = playerAdapter_->SetVideoSurfaceNew(native_window);
  EXPECT_EQ(actual_result, expected_result);
}

TEST_F(PlayerAdapterImplTest, TestSetVideoSurfaceNew002) {
  int32_t expected_result = 3;
  void* native_window = nullptr;
  int32_t actual_result = playerAdapter_->SetVideoSurfaceNew(native_window);
  EXPECT_EQ(actual_result, expected_result);
}
}  // namespace OHOS::NWeb