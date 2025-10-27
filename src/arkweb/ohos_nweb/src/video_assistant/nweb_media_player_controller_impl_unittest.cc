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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <utility>
#include <cstdint>
#include "cef/ohos_cef_ext/include/cef_media_player_controller.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"
#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"
#define private public
#define protected public
#include "ohos_nweb/src/video_assistant/nweb_media_player_controller_impl.h"

using namespace testing;
namespace OHOS::NWeb {
class MockCefMediaPlayerController : public CefMediaPlayerController {
 public:
  ~MockCefMediaPlayerController() = default;
  MOCK_METHOD(void, Play, (), (override));
  MOCK_METHOD(void, Pause, (), (override));
  MOCK_METHOD(void, Seek, (double time), (override));
  MOCK_METHOD(void, SetMuted, (bool muted), (override));
  MOCK_METHOD(void, SetPlaybackRate, (double playback_rate), (override));
  MOCK_METHOD(void, ExitFullscreen, (), (override));
  MOCK_METHOD(bool, SetVideoSurface, (void* native_window), (override));
  MOCK_METHOD(void, Download, (), (override));
  MOCK_METHOD(void, SetVolume, (double volume), (override));
  MOCK_METHOD(double, GetVolume, (), (override));
};

class NWebMediaPlayerControllerImplTest : public ::testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp(void) override;
  void TearDown(void) override;
  std::unique_ptr<MockCefMediaPlayerController> mock_cef_controller_;
  std::shared_ptr<NWebMediaPlayerControllerImpl> mediaplayer_controller_impl_;
  MockCefMediaPlayerController* raw_mock_controller_;
};

void NWebMediaPlayerControllerImplTest::SetUpTestCase(void) {}

void NWebMediaPlayerControllerImplTest::TearDownTestCase(void) {}

void NWebMediaPlayerControllerImplTest::SetUp() {
  mock_cef_controller_ = std::make_unique<MockCefMediaPlayerController>();
  raw_mock_controller_ = mock_cef_controller_.get();
  ASSERT_NE(mock_cef_controller_, nullptr);
  mediaplayer_controller_impl_ = std::make_shared<NWebMediaPlayerControllerImpl>(std::move(mock_cef_controller_));
  ASSERT_NE(mediaplayer_controller_impl_, nullptr);
  ASSERT_NE(mediaplayer_controller_impl_->cef_controller_, nullptr);
}

void NWebMediaPlayerControllerImplTest::TearDown() {
  mediaplayer_controller_impl_ = nullptr;
}

TEST_F(NWebMediaPlayerControllerImplTest, Play001) {
  std::shared_ptr<NWebMediaPlayerControllerImpl> mediaplayer_controller_impl = 
      std::make_shared<NWebMediaPlayerControllerImpl>(nullptr);
  EXPECT_CALL(*raw_mock_controller_, Play()).Times(0);
  mediaplayer_controller_impl->Play();
  EXPECT_NE(mediaplayer_controller_impl, nullptr);
  EXPECT_EQ(mediaplayer_controller_impl->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, Play002) {
  EXPECT_NE(mediaplayer_controller_impl_, nullptr);
  EXPECT_CALL(*raw_mock_controller_, Play()).Times(1);
  mediaplayer_controller_impl_->Play();
  EXPECT_NE(mediaplayer_controller_impl_->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, Pause001) {
  std::shared_ptr<NWebMediaPlayerControllerImpl> mediaplayer_controller_impl = 
      std::make_shared<NWebMediaPlayerControllerImpl>(nullptr);
  EXPECT_CALL(*raw_mock_controller_, Pause()).Times(0);
  mediaplayer_controller_impl->Pause();
  EXPECT_NE(mediaplayer_controller_impl, nullptr);
  EXPECT_EQ(mediaplayer_controller_impl->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, Pause002) {
  EXPECT_NE(mediaplayer_controller_impl_, nullptr);
  EXPECT_CALL(*raw_mock_controller_, Pause()).Times(1);
  mediaplayer_controller_impl_->Pause();
  EXPECT_NE(mediaplayer_controller_impl_->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, Seek001) {
  double time = 1.0;
  std::shared_ptr<NWebMediaPlayerControllerImpl> mediaplayer_controller_impl = 
      std::make_shared<NWebMediaPlayerControllerImpl>(nullptr);
  EXPECT_CALL(*raw_mock_controller_, Seek(time)).Times(0);
  mediaplayer_controller_impl->Seek(time);
  EXPECT_NE(mediaplayer_controller_impl, nullptr);
  EXPECT_EQ(mediaplayer_controller_impl->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, Seek002) {
  double time = 1.0;
  EXPECT_NE(mediaplayer_controller_impl_, nullptr);
  EXPECT_CALL(*raw_mock_controller_, Seek(time)).Times(1);
  mediaplayer_controller_impl_->Seek(time);
  EXPECT_NE(mediaplayer_controller_impl_->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, SetMuted001) {
  bool muted = false;
  std::shared_ptr<NWebMediaPlayerControllerImpl> mediaplayer_controller_impl = 
      std::make_shared<NWebMediaPlayerControllerImpl>(nullptr);
  EXPECT_CALL(*raw_mock_controller_, SetMuted(muted)).Times(0);
  mediaplayer_controller_impl->SetMuted(muted);
  EXPECT_NE(mediaplayer_controller_impl, nullptr);
  EXPECT_EQ(mediaplayer_controller_impl->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, SetMuted002) {
  bool muted = false;
  EXPECT_NE(mediaplayer_controller_impl_, nullptr);
  EXPECT_CALL(*raw_mock_controller_, SetMuted(muted)).Times(1);
  mediaplayer_controller_impl_->SetMuted(muted);
  EXPECT_NE(mediaplayer_controller_impl_->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, SetPlaybackRate001) {
  double playback_rate = 1.0;
  std::shared_ptr<NWebMediaPlayerControllerImpl> mediaplayer_controller_impl = 
      std::make_shared<NWebMediaPlayerControllerImpl>(nullptr);
  EXPECT_CALL(*raw_mock_controller_, SetPlaybackRate(playback_rate)).Times(0);
  mediaplayer_controller_impl->SetPlaybackRate(playback_rate);
  EXPECT_NE(mediaplayer_controller_impl, nullptr);
  EXPECT_EQ(mediaplayer_controller_impl->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, SetPlaybackRate002) {
  double playback_rate = 1.0;
  EXPECT_NE(mediaplayer_controller_impl_, nullptr);
  EXPECT_CALL(*raw_mock_controller_, SetPlaybackRate(playback_rate)).Times(1);
  mediaplayer_controller_impl_->SetPlaybackRate(playback_rate);
  EXPECT_NE(mediaplayer_controller_impl_->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, ExitFullscreen001) {
  std::shared_ptr<NWebMediaPlayerControllerImpl> mediaplayer_controller_impl = 
      std::make_shared<NWebMediaPlayerControllerImpl>(nullptr);
  EXPECT_CALL(*raw_mock_controller_, ExitFullscreen()).Times(0);
  mediaplayer_controller_impl->ExitFullscreen();
  EXPECT_NE(mediaplayer_controller_impl, nullptr);
  EXPECT_EQ(mediaplayer_controller_impl->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, ExitFullscreen002) {
  EXPECT_NE(mediaplayer_controller_impl_, nullptr);
  EXPECT_CALL(*raw_mock_controller_, ExitFullscreen()).Times(1);
  mediaplayer_controller_impl_->ExitFullscreen();
  EXPECT_NE(mediaplayer_controller_impl_->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, SetVideoSurface001) {
  void* native_window = nullptr;
  std::shared_ptr<NWebMediaPlayerControllerImpl> mediaplayer_controller_impl = 
      std::make_shared<NWebMediaPlayerControllerImpl>(nullptr);
  EXPECT_CALL(*raw_mock_controller_, SetVideoSurface(native_window)).Times(0);
  mediaplayer_controller_impl->SetVideoSurface(native_window);
  EXPECT_NE(mediaplayer_controller_impl, nullptr);
  EXPECT_EQ(mediaplayer_controller_impl->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, SetVideoSurface002) {
  void* native_window = nullptr;
  EXPECT_NE(mediaplayer_controller_impl_, nullptr);
  EXPECT_CALL(*raw_mock_controller_, SetVideoSurface(native_window)).Times(1);
  mediaplayer_controller_impl_->SetVideoSurface(native_window);
  EXPECT_NE(mediaplayer_controller_impl_->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, Download001) {
  std::shared_ptr<NWebMediaPlayerControllerImpl> mediaplayer_controller_impl = 
      std::make_shared<NWebMediaPlayerControllerImpl>(nullptr);
  EXPECT_CALL(*raw_mock_controller_, Download()).Times(0);
  mediaplayer_controller_impl->Download();
  EXPECT_NE(mediaplayer_controller_impl, nullptr);
  EXPECT_EQ(mediaplayer_controller_impl->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, Download002) {
  EXPECT_NE(mediaplayer_controller_impl_, nullptr);
  EXPECT_CALL(*raw_mock_controller_, Download()).Times(1);
  mediaplayer_controller_impl_->Download();
  EXPECT_NE(mediaplayer_controller_impl_->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, SetVolume001) {
  double volume = 1.0;
  std::shared_ptr<NWebMediaPlayerControllerImpl> mediaplayer_controller_impl = 
      std::make_shared<NWebMediaPlayerControllerImpl>(nullptr);
  EXPECT_CALL(*raw_mock_controller_, SetVolume(volume)).Times(0);
  mediaplayer_controller_impl->SetVolume(volume);
  EXPECT_NE(mediaplayer_controller_impl, nullptr);
  EXPECT_EQ(mediaplayer_controller_impl->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, SetVolume002) {
  double volume = 1.0;
  EXPECT_NE(mediaplayer_controller_impl_, nullptr);
  EXPECT_CALL(*raw_mock_controller_, SetVolume(volume)).Times(1);
  mediaplayer_controller_impl_->SetVolume(volume);
  EXPECT_NE(mediaplayer_controller_impl_->cef_controller_, nullptr);
}

TEST_F(NWebMediaPlayerControllerImplTest, GetVolume001) {
  double temp = -1.0;
  std::shared_ptr<NWebMediaPlayerControllerImpl> mediaplayer_controller_impl = 
      std::make_shared<NWebMediaPlayerControllerImpl>(nullptr);
  EXPECT_CALL(*raw_mock_controller_, GetVolume()).Times(0);
  auto result = mediaplayer_controller_impl->GetVolume();
  EXPECT_NE(mediaplayer_controller_impl, nullptr);
  EXPECT_EQ(mediaplayer_controller_impl->cef_controller_, nullptr);
  EXPECT_EQ(result, temp);
}

TEST_F(NWebMediaPlayerControllerImplTest, GetVolume002) {
  double temp = 1.0;
  EXPECT_NE(mediaplayer_controller_impl_, nullptr);
  EXPECT_CALL(*raw_mock_controller_, GetVolume()).WillOnce(::testing::Return(temp));
  auto result = mediaplayer_controller_impl_->GetVolume();
  EXPECT_NE(mediaplayer_controller_impl_->cef_controller_, nullptr);
  EXPECT_EQ(result, temp);
}
}  // namespace OHOS::NWeb
