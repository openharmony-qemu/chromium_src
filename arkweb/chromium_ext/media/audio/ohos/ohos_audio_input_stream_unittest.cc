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

#define protected public
#include "ohos_audio_input_stream.h"
#undef protected
#include "arkweb/chromium_ext/media/base/ohos/ohos_media_player_bridge.h"
#include "base/logging.h"
#include "content/public/browser/web_contents.h"
#include "content/public/test/test_renderer_host.h"
#include "content/test/test_web_contents.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using namespace media;
using namespace testing;
using namespace OHOS::NWeb;
using namespace base;
using namespace content;

namespace media {
namespace {
constexpr int kDefaultSampleRate = 48000;
constexpr int kDefaultChannelCount = 2;
constexpr int kMinimumInputBufferSize = 2048;
constexpr double kVolumeEpsilon = 1e-6;
constexpr int kInvalidNWebId = -1;
}  // namespace

class MockAudioThread : public AudioThread {
 public:
  void Stop() { std::cout << "Stop function called" << std::endl; }

  bool IsHung() const { return false; }

  base::SingleThreadTaskRunner* GetTaskRunner() { return nullptr; }

  base::SingleThreadTaskRunner* GetWorkerTaskRunner() { return nullptr; }
};

class OHOSAudioInputStreamTest : public content::RenderViewHostTestHarness {
 public:
  OHOSAudioInputStreamTest() : content::RenderViewHostTestHarness() {}

  OHOSAudioInputStreamTest(const OHOSAudioInputStreamTest&) = delete;
  OHOSAudioInputStreamTest& operator=(const OHOSAudioInputStreamTest&) = delete;

  void SetUp() override {
    std::unique_ptr<MockAudioThread> mock_audio_thread =
        std::make_unique<MockAudioThread>();
    ohos_audio_manager_ = std::make_shared<media::OHOSAudioManager>(
        std::move(mock_audio_thread), nullptr);
    createAudioInputStream();

    content::RenderViewHostTestHarness::SetUp();
  }
  void TearDown() override {
    content::RenderViewHostTestHarness::TearDown();
  }

  void createAudioInputStream() {
    params_ = AudioParameters(AudioParameters::AUDIO_FAKE,
                              ChannelLayoutConfig::Guess(kDefaultChannelCount),
                              kDefaultSampleRate, kMinimumInputBufferSize);
    std::string device_id = "some_device_id";
    audioInputStream_ = static_cast<OHOSAudioInputStream*>(
        ohos_audio_manager_->MakeLowLatencyInputStream(params_, device_id,
                                                       log_callback_));
  }

 public:
  std::shared_ptr<media::OHOSAudioManager> ohos_audio_manager_;
  AudioParameters params_;
  AudioManagerBase::LogCallback log_callback_;
  OHOSAudioInputStream* audioInputStream_;
};
}  // namespace media

TEST_F(OHOSAudioInputStreamTest, Open001) {
  EXPECT_EQ(audioInputStream_->Open(), AudioInputStream::OpenOutcome::kSuccess);
}

TEST_F(OHOSAudioInputStreamTest, Start001) {
#if BUILDFLAG(ARKWEB_WEBRTC)
  ohos_audio_manager_->SelectAudioDevice("screen:systemAudio:-2:1", true);
#endif
  ASSERT_NO_FATAL_FAILURE(audioInputStream_->Start(nullptr));
  audioInputStream_->Stop();
}

TEST_F(OHOSAudioInputStreamTest, Start002) {
#if BUILDFLAG(ARKWEB_WEBRTC)
  ohos_audio_manager_->SelectAudioDevice("screen:systemAudio:-2:0", true);
#endif
  ASSERT_NO_FATAL_FAILURE(audioInputStream_->Start(nullptr));
  audioInputStream_->Stop();
}

TEST_F(OHOSAudioInputStreamTest, Stop001) {
  ASSERT_NO_FATAL_FAILURE(audioInputStream_->Stop());
}

TEST_F(OHOSAudioInputStreamTest, GetMaxVolume001) {
  EXPECT_EQ(audioInputStream_->GetMaxVolume(), 1.0);
}

TEST_F(OHOSAudioInputStreamTest, SetVolume001) {
  ohos_audio_manager_->SelectAudioDevice("screen:systemAudio:-2:1", true);
  audioInputStream_->Start(nullptr);
  audioInputStream_->SetVolume(0.5);
  EXPECT_EQ(audioInputStream_->GetVolume(), 0.5);
  audioInputStream_->Stop();
}

TEST_F(OHOSAudioInputStreamTest, SetVolume002) {
  ohos_audio_manager_->SelectAudioDevice("screen:systemAudio:-2:0", true);
  audioInputStream_->Start(nullptr);
  audioInputStream_->SetVolume(0.5);
  EXPECT_EQ(audioInputStream_->GetVolume(), 0.5);
  EXPECT_EQ(audioInputStream_->IsMuted(), false);
  audioInputStream_->SetVolume(0);
  EXPECT_EQ(audioInputStream_->GetVolume(), 0);
  EXPECT_EQ(audioInputStream_->IsMuted(), true);
  audioInputStream_->Stop();
}

TEST_F(OHOSAudioInputStreamTest, SetVolume003) {
  audioInputStream_->SetVolume(0);
  EXPECT_EQ(audioInputStream_->GetVolume(), 0);
  EXPECT_EQ(audioInputStream_->IsMuted(), true);
}

TEST_F(OHOSAudioInputStreamTest, SetAutomaticGainControl001) {
  ohos_audio_manager_->SelectAudioDevice("screen:systemAudio:-2:1", true);
  audioInputStream_->Start(nullptr);
  audioInputStream_->SetAutomaticGainControl(true);
  EXPECT_EQ(audioInputStream_->GetAutomaticGainControl(), true);
  audioInputStream_->Stop();
}

TEST_F(OHOSAudioInputStreamTest, SetAutomaticGainControl002) {
  ohos_audio_manager_->SelectAudioDevice("screen:systemAudio:-2:0", true);
  audioInputStream_->Start(nullptr);
  audioInputStream_->SetAutomaticGainControl(false);
  EXPECT_EQ(audioInputStream_->GetAutomaticGainControl(), false);
  audioInputStream_->Stop();
}

TEST_F(OHOSAudioInputStreamTest, SetAutomaticGainControl003) {
  audioInputStream_->SetAutomaticGainControl(false);
  EXPECT_EQ(audioInputStream_->GetAutomaticGainControl(), false);
}

TEST_F(OHOSAudioInputStreamTest, SetOutputDeviceForAec001) {
  ohos_audio_manager_->SelectAudioDevice("screen:systemAudio:-2:1", true);
  audioInputStream_->Start(nullptr);
  ASSERT_NO_FATAL_FAILURE(audioInputStream_->SetOutputDeviceForAec("deviceId"));
  audioInputStream_->Stop();
}

TEST_F(OHOSAudioInputStreamTest, SetOutputDeviceForAec002) {
  ohos_audio_manager_->SelectAudioDevice("screen:systemAudio:-2:0", true);
  audioInputStream_->Start(nullptr);
  ASSERT_NO_FATAL_FAILURE(
      audioInputStream_->SetOutputDeviceForAec("someDeviceId"));
  audioInputStream_->Stop();
}

TEST_F(OHOSAudioInputStreamTest, SetOutputDeviceForAec003) {
  ASSERT_NO_FATAL_FAILURE(
      audioInputStream_->SetOutputDeviceForAec("someDeviceId"));
}

TEST_F(OHOSAudioInputStreamTest, GetNWebId001) {
  EXPECT_EQ(audioInputStream_->GetNWebId(params_), -1);
}

TEST_F(OHOSAudioInputStreamTest, GetNWebIdOnUIThread001) {
  EXPECT_EQ(audioInputStream_->GetNWebIdOnUIThread(params_), -1);
}

TEST_F(OHOSAudioInputStreamTest, GetNWebIdOnUIThread002) {
  auto frameHost = main_rfh();
  ASSERT_NE(frameHost, nullptr);
  params_.set_render_process_id(frameHost->GetProcess()->GetID());
  params_.set_render_frame_id(frameHost->GetRoutingID());

  EXPECT_NE(audioInputStream_->GetNWebIdOnUIThread(params_), -1);
}
