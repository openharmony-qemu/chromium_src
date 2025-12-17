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

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>

#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"
#define private public
#include "arkweb/ohos_adapter_ndk/screen_capture_adapter/screen_capture_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/screen_capture_adapter/screen_capture_adapter_impl.cpp"
#undef private

#include "third_party/bounds_checking_function/include/securec.h"

using namespace OHOS::NWeb;

namespace {
constexpr int32_t kUnittestAudioSampleRate = 48000;
constexpr int32_t kUnittestAudioChannels = 2;
constexpr int32_t kUnittestInnerAudioSampleRate = 48000;
constexpr int32_t kUnittestInnerAudioBitrate = 48000;
constexpr int32_t kUnittestVideoBitrate = 2000000;
constexpr int32_t kUnittestVideoFrameRate = 30;
} // namespace

class ScreenCaptureAdapterImplMock : public ScreenCaptureAdapter {
 public:
  MOCK_METHOD(int32_t, Init, (const std::shared_ptr<ScreenCaptureConfigAdapter> config), (override));
  MOCK_METHOD(int32_t, SetMicrophoneEnable, (bool enable), (override));
  MOCK_METHOD(int32_t, StartCapture, (), (override));
  MOCK_METHOD(int32_t, StopCapture, (), (override));
  MOCK_METHOD(int32_t, SetCaptureCallback,
      (const std::shared_ptr<ScreenCaptureCallbackAdapter> callback), (override));
  MOCK_METHOD(std::shared_ptr<SurfaceBufferAdapter>, AcquireVideoBuffer, (), (override));
  MOCK_METHOD(int32_t, ReleaseVideoBuffer, (), (override));
  MOCK_METHOD(int32_t, AcquireAudioBuffer,
      (std::shared_ptr<AudioBufferAdapter> audiobuffer, AudioCaptureSourceTypeAdapter type), (override));
  MOCK_METHOD(int32_t, ReleaseAudioBuffer, (AudioCaptureSourceTypeAdapter type), (override));
  MOCK_METHOD(int32_t, InitV2,
      (const std::shared_ptr<ScreenCaptureConfigAdapter> config, int nweb_id), (override));
};

class AudioCaptureInfoAdapterMock : public AudioCaptureInfoAdapter {
 public:
  AudioCaptureInfoAdapterMock() = default;

  ~AudioCaptureInfoAdapterMock() override = default;

  int32_t GetAudioSampleRate() override
  {
    return sample_rate_;
  }

  int32_t GetAudioChannels() override
  {
    return channels_;
  }

  AudioCaptureSourceTypeAdapter GetAudioSource() override
  {
    return source_;
  }

  void SetAudioSampleRate(int32_t rate)
  {
    sample_rate_ = rate;
  }

  void SetAudioChannels(int32_t channel)
  {
    channels_ = channel;
  }

  void SetAudioSource(AudioCaptureSourceTypeAdapter type)
  {
    source_ = type;
  }

  int32_t sample_rate_ = 0;
  int32_t channels_ = 0;
  AudioCaptureSourceTypeAdapter source_ = AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT;
};

class AudioEncInfoAdapterMock : public AudioEncInfoAdapter {
 public:
  AudioEncInfoAdapterMock() = default;

  ~AudioEncInfoAdapterMock() override = default;

  int32_t GetAudioBitrate() override
  {
    return bitrate_;
  }

  AudioCodecFormatAdapter GetAudioCodecformat() override
  {
    return codec_format_;
  }

  void SetAudioBitrate(int32_t bitrate)
  {
    bitrate_ = bitrate;
  }

  void SetAudioCodecformat(AudioCodecFormatAdapter format)
  {
    codec_format_ = format;
  }

  int32_t bitrate_ = 0;
  AudioCodecFormatAdapter codec_format_ =
      AudioCodecFormatAdapter::AUDIO_DEFAULT;
};

class AudioInfoAdapterMock : public AudioInfoAdapter {
 public:
  AudioInfoAdapterMock() = default;

  ~AudioInfoAdapterMock() override = default;

  std::shared_ptr<AudioCaptureInfoAdapter> GetMicCapInfo() override
  {
    return mic_cap_info_;
  }

  std::shared_ptr<AudioCaptureInfoAdapter> GetInnerCapInfo() override
  {
    return inner_cap_info_;
  }

  std::shared_ptr<AudioEncInfoAdapter> GetAudioEncInfo() override
  {
    return enc_info_;
  }

  void SetMicCapInfo(std::shared_ptr<AudioCaptureInfoAdapter> info)
  {
    mic_cap_info_ = info;
  }

  void SetInnerCapInfo(std::shared_ptr<AudioCaptureInfoAdapter> info)
  {
    inner_cap_info_ = info;
  }

  void SetAudioEncInfo(std::shared_ptr<AudioEncInfoAdapter> info)
  {
    enc_info_ = info;
  }

  std::shared_ptr<AudioCaptureInfoAdapter> mic_cap_info_ = nullptr;
  std::shared_ptr<AudioCaptureInfoAdapter> inner_cap_info_ = nullptr;
  std::shared_ptr<AudioEncInfoAdapter> enc_info_ = nullptr;
};

class VideoCaptureInfoAdapterMock : public VideoCaptureInfoAdapter {
 public:
  VideoCaptureInfoAdapterMock() = default;

  ~VideoCaptureInfoAdapterMock() override = default;

  uint64_t GetDisplayId() override
  {
    return display_id_;
  }

  std::list<int32_t> GetTaskIDs() override
  {
    return task_ids_;
  }

  int32_t GetVideoFrameWidth() override
  {
    return video_frame_width_;
  }

  int32_t GetVideoFrameHeight() override
  {
    return video_frame_height_;
  }

  VideoSourceTypeAdapter GetVideoSourceType() override
  {
    return video_source_;
  }

  void SetDisplayId(uint64_t id)
  {
      display_id_ = id;
  }

  void SetTaskIDs(std::list<int32_t> ids)
  {
      task_ids_ = ids;
  }

  void SetVideoFrameWidth(int32_t width)
  {
      video_frame_width_ = width;
  }

  void SetVideoFrameHeight(int32_t height)
  {
      video_frame_height_ = height;
  }

  void SetVideoSourceType(VideoSourceTypeAdapter type)
  {
      video_source_ = type;
  }

  uint64_t display_id_ = 0;
  std::list<int32_t> task_ids_;
  int32_t video_frame_width_ = 0;
  int32_t video_frame_height_ = 0;
  VideoSourceTypeAdapter video_source_ =
      VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_RGBA;
};

class VideoEncInfoAdapterMock : public VideoEncInfoAdapter {
public:
  VideoEncInfoAdapterMock() = default;

  ~VideoEncInfoAdapterMock() override = default;

  VideoCodecFormatAdapter GetVideoCodecFormat() override
  {
    return codec_;
  }

  int32_t GetVideoBitrate() override
  {
    return bitrate_;
  }

  int32_t GetVideoFrameRate() override
  {
    return frame_rate_;
  }

  void SetVideoCodecFormat(VideoCodecFormatAdapter format)
  {
    codec_ = format;
  }

  void SetVideoBitrate(int32_t bitrate)
  {
    bitrate_ = bitrate;
  }

  void SetVideoFrameRate(int32_t frameRate)
  {
    frame_rate_ = frameRate;
  }

  VideoCodecFormatAdapter codec_ = VideoCodecFormatAdapter::VIDEO_DEFAULT;
  int32_t bitrate_ = 0;
  int32_t frame_rate_ = 0;
};

class VideoInfoAdapterMock : public VideoInfoAdapter {
 public:
  VideoInfoAdapterMock() = default;

  ~VideoInfoAdapterMock() override = default;

  std::shared_ptr<VideoCaptureInfoAdapter> GetVideoCapInfo() override
  {
    return cap_info_;
  }

  std::shared_ptr<VideoEncInfoAdapter> GetVideoEncInfo() override
  {
    return enc_info_;
  }

  void SetVideoCapInfo(std::shared_ptr<VideoCaptureInfoAdapter> info)
  {
    cap_info_ = info;
  }

  void SetVideoEncInfo(std::shared_ptr<VideoEncInfoAdapter> info)
  {
    enc_info_ = info;
  }

  std::shared_ptr<VideoCaptureInfoAdapter> cap_info_ = nullptr;
  std::shared_ptr<VideoEncInfoAdapter> enc_info_ = nullptr;
};

class RecorderInfoAdapterMock : public RecorderInfoAdapter {
 public:
  RecorderInfoAdapterMock() = default;

  ~RecorderInfoAdapterMock() override = default;

  std::string GetUrl() override { return url_; }

  ContainerFormatTypeAdapter GetFileFormat() override { return type_; }

  std::string url_;
  ContainerFormatTypeAdapter type_ = ContainerFormatTypeAdapter::CFT_MPEG_4A_TYPE;
};

class ScreenCaptureConfigAdapterMock : public ScreenCaptureConfigAdapter {
 public:
  ScreenCaptureConfigAdapterMock() = default;

  ~ScreenCaptureConfigAdapterMock() override = default;

  CaptureModeAdapter GetCaptureMode() override
  {
    return capture_mode_;
  }

  DataTypeAdapter GetDataType() override
  {
    return data_type_;
  }

  std::shared_ptr<AudioInfoAdapter> GetAudioInfo() override
  {
    return audio_info_;
  }

  std::shared_ptr<VideoInfoAdapter> GetVideoInfo() override
  {
    return video_info_;
  }

  std::shared_ptr<RecorderInfoAdapter> GetRecorderInfo() override
  {
    return recorder_info_;
  }

  void SetCaptureMode(CaptureModeAdapter mode)
  {
    capture_mode_ = mode;
  }

  void SetDataType(DataTypeAdapter type)
  {
    data_type_ = type;
  }

  void SetAudioInfo(std::shared_ptr<AudioInfoAdapter> info)
  {
    audio_info_ = info;
  }

  void SetVideoInfo(std::shared_ptr<VideoInfoAdapter> info)
  {
    video_info_ = info;
  }

  void SetRecorderInfo(std::shared_ptr<RecorderInfoAdapter> info)
  {
    recorder_info_ = info;
  }

  CaptureModeAdapter capture_mode_ = CaptureModeAdapter::CAPTURE_INVAILD;
  DataTypeAdapter data_type_ = DataTypeAdapter::INVAILD_DATA_TYPE;
  std::shared_ptr<AudioInfoAdapter> audio_info_ = nullptr;
  std::shared_ptr<VideoInfoAdapter> video_info_ = nullptr;
  std::shared_ptr<RecorderInfoAdapter> recorder_info_ = nullptr;
};

class OHOSScreenCaptureCallbackMock : public ScreenCaptureCallbackAdapter {
 public:
  OHOSScreenCaptureCallbackMock() = default;

  ~OHOSScreenCaptureCallbackMock() override = default;

  void OnError(int32_t errorCode) override {}

  void OnAudioBufferAvailable(bool isReady, AudioCaptureSourceTypeAdapter type) override {}

  void OnVideoBufferAvailable(bool isReady) override {}

  void OnStateChange(ScreenCaptureStateCodeAdapter stateCode) override {}

  void OnAudioBufferAvailableV2(bool isReady, AudioCaptureSourceTypeAdapter type, int32_t nweb_id) override {}

  void OnVideoBufferAvailableV2(bool isReady, int32_t nweb_id) override {}

  void OnStateChangeV2(ScreenCaptureStateCodeAdapter stateCode, int32_t nweb_id) override {}
};

class ScreenCaptureAdapterImplTest : public testing::Test {
 public:
  static void SetUpTestCase(void) {}
  static void TearDownTestCase(void) {}
  void SetUp() {}
  void TearDown() {}
};

std::shared_ptr<AudioInfoAdapterMock> GetAudioInfo(bool invalid = false) {
  std::shared_ptr<AudioInfoAdapterMock> audioInfo =
      std::make_shared<AudioInfoAdapterMock>();
  if (audioInfo == nullptr) {
    return nullptr;
  }
  if (invalid) {
    audioInfo->SetMicCapInfo(nullptr);
    audioInfo->SetInnerCapInfo(nullptr);
    audioInfo->SetAudioEncInfo(nullptr);
    return audioInfo;
  }
  // setting the microphone information
  std::shared_ptr<AudioCaptureInfoAdapterMock> micCapInfo =
      std::make_shared<AudioCaptureInfoAdapterMock>();
  if (micCapInfo == nullptr) {
    return nullptr;
  }
  micCapInfo->SetAudioSampleRate(kUnittestAudioSampleRate);
  micCapInfo->SetAudioChannels(kUnittestAudioChannels);
  micCapInfo->SetAudioSource(
      OHOS::NWeb::AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT);

  // setting the system audio information
  std::shared_ptr<AudioCaptureInfoAdapterMock> innerCapInfo =
      std::make_shared<AudioCaptureInfoAdapterMock>();
  if (innerCapInfo == nullptr) {
    return nullptr;
  }
  innerCapInfo->SetAudioSampleRate(kUnittestInnerAudioSampleRate);
  innerCapInfo->SetAudioChannels(kUnittestAudioChannels);
  innerCapInfo->SetAudioSource(
      OHOS::NWeb::AudioCaptureSourceTypeAdapter::ALL_PLAYBACK);

  // setting Audio Encoding Information
  std::shared_ptr<AudioEncInfoAdapterMock> audioEncInfo =
      std::make_shared<AudioEncInfoAdapterMock>();
  if (audioEncInfo == nullptr) {
    return nullptr;
  }
  audioEncInfo->SetAudioBitrate(kUnittestInnerAudioBitrate);
  audioEncInfo->SetAudioCodecformat(
      OHOS::NWeb::AudioCodecFormatAdapter::AUDIO_DEFAULT);

  audioInfo->SetMicCapInfo(micCapInfo);
  audioInfo->SetInnerCapInfo(innerCapInfo);
  audioInfo->SetAudioEncInfo(audioEncInfo);
  return audioInfo;
}

std::shared_ptr<ScreenCaptureConfigAdapterMock> GetScreenCaptureConfig(
    bool invalid = false) {
  std::shared_ptr<ScreenCaptureConfigAdapterMock> config =
      std::make_shared<ScreenCaptureConfigAdapterMock>();
  auto displayMgr = OHOS::NWeb::OhosAdapterHelperExt::CreateDisplayMgrAdapter();
  if (!displayMgr) {
    return nullptr;
  }
  auto display = displayMgr->GetDefaultDisplay();
  if (!display) {
    return nullptr;
  }
  int32_t videoFrameWidth = display->GetWidth();
  int32_t videoFrameHeight = display->GetHeight();

  // setting video information
  std::shared_ptr<VideoCaptureInfoAdapterMock> videoCapInfo =
      std::make_shared<VideoCaptureInfoAdapterMock>();
  videoCapInfo->SetVideoFrameWidth(videoFrameWidth);
  videoCapInfo->SetVideoFrameHeight(videoFrameHeight);
  videoCapInfo->SetVideoSourceType(
      OHOS::NWeb::VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_RGBA);

  // setting Video Encoding Information
  std::shared_ptr<VideoEncInfoAdapterMock> videoEncInfo =
      std::make_shared<VideoEncInfoAdapterMock>();
  videoEncInfo->SetVideoBitrate(kUnittestVideoBitrate);
  videoEncInfo->SetVideoFrameRate(kUnittestVideoFrameRate);
  videoEncInfo->SetVideoCodecFormat(OHOS::NWeb::VideoCodecFormatAdapter::H264);

  std::shared_ptr<VideoInfoAdapterMock> videoInfo =
      std::make_shared<VideoInfoAdapterMock>();
  videoInfo->SetVideoCapInfo(videoCapInfo);
  videoInfo->SetVideoEncInfo(videoEncInfo);
  config->SetCaptureMode(OHOS::NWeb::CaptureModeAdapter::CAPTURE_HOME_SCREEN);
  config->SetDataType(OHOS::NWeb::DataTypeAdapter::ORIGINAL_STREAM_DATA_TYPE);
  std::shared_ptr<AudioInfoAdapterMock> audioInfo = GetAudioInfo(invalid);
  config->SetAudioInfo(audioInfo);
  config->SetVideoInfo(videoInfo);
  return config;
}

TEST_F(ScreenCaptureAdapterImplTest, Init) {
  // test Init
  auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
  EXPECT_NE(adapterImpl, nullptr);
  EXPECT_EQ(adapterImpl->Init(nullptr), 0);

  // test InitV2
  adapterImpl->screenCapture_ = OH_AVScreenCapture_Create();
  int32_t result = adapterImpl->InitV2(nullptr, 0);
  EXPECT_EQ(result, 0);
  OH_AVScreenCapture_Release(adapterImpl->screenCapture_);
  adapterImpl->screenCapture_ = nullptr;

  auto config = GetScreenCaptureConfig();
  result = adapterImpl->InitV2(config, 0);
  EXPECT_EQ(result, 0);
  OH_AVScreenCapture_Release(adapterImpl->screenCapture_);
  adapterImpl->screenCapture_ = nullptr;

  config->SetDataType(OHOS::NWeb::DataTypeAdapter::INVAILD_DATA_TYPE);
  result = adapterImpl->InitV2(config, 0);
  EXPECT_EQ(result, -1);
}

TEST_F(ScreenCaptureAdapterImplTest, SetMicrophoneEnable) {
  auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
  EXPECT_NE(adapterImpl, nullptr);
  int32_t result = adapterImpl->SetMicrophoneEnable(false);
  EXPECT_EQ(result, -1);
  auto config = GetScreenCaptureConfig();
  result = adapterImpl->InitV2(config, 0);
  EXPECT_EQ(result, 0);

  result = adapterImpl->SetMicrophoneEnable(false);
  EXPECT_EQ(result, 0);
  result = adapterImpl->SetMicrophoneEnable(true);
  EXPECT_EQ(result, 0);
}

TEST_F(ScreenCaptureAdapterImplTest, SetCaptureCallback) {
  auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
  EXPECT_NE(adapterImpl, nullptr);
  int32_t result = adapterImpl->SetCaptureCallback(nullptr);
  EXPECT_EQ(result, -1);

  auto config = GetScreenCaptureConfig();
  result = adapterImpl->InitV2(config, 0);
  EXPECT_EQ(result, 0);
  result = adapterImpl->SetCaptureCallback(nullptr);
  EXPECT_EQ(result, -1);
  auto callback = std::make_shared<OHOSScreenCaptureCallbackMock>();
  result = adapterImpl->SetCaptureCallback(callback);
  EXPECT_EQ(result, 0);
}

TEST_F(ScreenCaptureAdapterImplTest, StartCapture) {
  auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
  EXPECT_NE(adapterImpl, nullptr);
  int32_t result = adapterImpl->StartCapture();
  EXPECT_EQ(result, -1);

  auto config = GetScreenCaptureConfig();
  result = adapterImpl->InitV2(config, 0);
  EXPECT_EQ(result, 0);
  result = adapterImpl->StartCapture();
  EXPECT_EQ(result, 0);

  auto callback = std::make_shared<OHOSScreenCaptureCallbackMock>();
  result = adapterImpl->StartCapture();
  EXPECT_EQ(result, -1);
}

TEST_F(ScreenCaptureAdapterImplTest, AcquireVideoBuffer) {
  auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
  EXPECT_NE(adapterImpl, nullptr);
  std::shared_ptr<SurfaceBufferAdapter> buffer =
      adapterImpl->AcquireVideoBuffer();
  EXPECT_EQ(buffer, nullptr);

  auto config = GetScreenCaptureConfig();
  int32_t result = adapterImpl->InitV2(config, 0);
  EXPECT_EQ(result, 0);
  buffer = adapterImpl->AcquireVideoBuffer();
  EXPECT_EQ(buffer, nullptr);

  auto callback = std::make_shared<OHOSScreenCaptureCallbackMock>();
  result = adapterImpl->SetCaptureCallback(callback);
  EXPECT_EQ(result, 0);
  result = adapterImpl->StartCapture();
  EXPECT_EQ(result, 0);

  std::queue<std::shared_ptr<SurfaceBufferAdapter>> bufferAvailableQueue1;
  adapterImpl->nweb_id_ = 100;
  bufferAvailableQueueMap_.emplace(100, bufferAvailableQueue1);
  buffer = adapterImpl->AcquireVideoBuffer();
  EXPECT_EQ(buffer, nullptr);
  adapterImpl->ReleaseAudioBuffer(
      OHOS::NWeb::AudioCaptureSourceTypeAdapter::ALL_PLAYBACK);
}

TEST_F(ScreenCaptureAdapterImplTest, AcquireAudioBuffer) {
  OH_AVBuffer* avBuffer = OH_AVBuffer_Create(1);
  EXPECT_NE(avBuffer, nullptr);
  std::shared_ptr<OH_AudioBufferAdapterImpl> buffer =
      std::make_shared<OH_AudioBufferAdapterImpl>(
          avBuffer, 1, OH_AudioCaptureSourceType::OH_ALL_PLAYBACK);

  auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
  EXPECT_NE(adapterImpl, nullptr);
  int32_t result = adapterImpl->AcquireAudioBuffer(
      buffer, OHOS::NWeb::AudioCaptureSourceTypeAdapter::ALL_PLAYBACK);
  EXPECT_EQ(result, -1);

  auto config = GetScreenCaptureConfig();
  result = adapterImpl->InitV2(config, 0);
  EXPECT_EQ(result, 0);
  adapterImpl->nweb_id_ = 101;
  result = adapterImpl->AcquireAudioBuffer(
      buffer, OHOS::NWeb::AudioCaptureSourceTypeAdapter::ALL_PLAYBACK);
  EXPECT_EQ(result, -1);

  adapterImpl->nweb_id_ = 102;
  std::queue<std::shared_ptr<OH_AudioBufferAdapterImpl>> bufferQueue;
  audioBufferAvailableQueueMap_.emplace(102, bufferQueue);
  result = adapterImpl->AcquireAudioBuffer(
      buffer, OHOS::NWeb::AudioCaptureSourceTypeAdapter::ALL_PLAYBACK);
  EXPECT_EQ(result, -1);

  audioBufferAvailableQueueMap_.erase(102);
  adapterImpl->nweb_id_ = 0;
  auto callback = std::make_shared<OHOSScreenCaptureCallbackMock>();
  result = adapterImpl->SetCaptureCallback(callback);
  EXPECT_EQ(result, 0);
  result = adapterImpl->StartCapture();
  EXPECT_EQ(result, 0);

  CallbackInfo cb;
  cb.callback = std::make_shared<OHOSScreenCaptureCallbackMock>();
  cb.nweb_id = 0;
  void* userData = static_cast<void*>(&cb);
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnBufferAvailable(
      nullptr, avBuffer,
      OH_AVScreenCaptureBufferType::OH_SCREEN_CAPTURE_BUFFERTYPE_AUDIO_INNER, 0,
      userData);

  result = adapterImpl->AcquireAudioBuffer(
      buffer, OHOS::NWeb::AudioCaptureSourceTypeAdapter::ALL_PLAYBACK);
  EXPECT_EQ(result, 0);
  adapterImpl->ReleaseAudioBuffer(
      OHOS::NWeb::AudioCaptureSourceTypeAdapter::ALL_PLAYBACK);
  OH_AVBuffer_Destroy(avBuffer);
}

TEST_F(ScreenCaptureAdapterImplTest, StopCapture) {
  auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
  EXPECT_NE(adapterImpl, nullptr);
  int32_t result = adapterImpl->StopCapture();
  EXPECT_EQ(result, -1);

  auto config = GetScreenCaptureConfig();
  result = adapterImpl->InitV2(config, 0);
  EXPECT_EQ(result, 0);

  result = adapterImpl->StartCapture();
  EXPECT_EQ(result, 0);
  result = adapterImpl->StopCapture();
  EXPECT_EQ(result, 0);
}

TEST_F(ScreenCaptureAdapterImplTest, Release) {
  auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
  EXPECT_NE(adapterImpl, nullptr);
  adapterImpl->Release();

  auto config = GetScreenCaptureConfig();
  auto result = adapterImpl->InitV2(config, 0);
  EXPECT_EQ(result, 0);
  adapterImpl->Release();
}

TEST_F(ScreenCaptureAdapterImplTest, GetOHCaptureMode) {
  std::unordered_map<CaptureModeAdapter, OH_CaptureMode> modeMap = {
      {CaptureModeAdapter::CAPTURE_HOME_SCREEN,
       OH_CaptureMode::OH_CAPTURE_HOME_SCREEN},
      {CaptureModeAdapter::CAPTURE_SPECIFIED_SCREEN,
       OH_CaptureMode::OH_CAPTURE_SPECIFIED_SCREEN},
      {CaptureModeAdapter::CAPTURE_SPECIFIED_WINDOW,
       OH_CaptureMode::OH_CAPTURE_SPECIFIED_WINDOW},
      {CaptureModeAdapter::CAPTURE_INVAILD, OH_CaptureMode::OH_CAPTURE_INVAILD},
  };

  for (auto it = modeMap.begin(); it != modeMap.end(); it++) {
    EXPECT_EQ(GetOHCaptureMode(it->first), it->second);
  }
}

TEST_F(ScreenCaptureAdapterImplTest, GetOHDataType) {
  std::unordered_map<DataTypeAdapter, OH_DataType> typeMap = {
      {DataTypeAdapter::ORIGINAL_STREAM_DATA_TYPE,
       OH_DataType::OH_ORIGINAL_STREAM},
      {DataTypeAdapter::ENCODED_STREAM_DATA_TYPE,
       OH_DataType::OH_ENCODED_STREAM},
      {DataTypeAdapter::CAPTURE_FILE_DATA_TYPE, OH_DataType::OH_CAPTURE_FILE},
      {DataTypeAdapter::INVAILD_DATA_TYPE, OH_DataType::OH_INVAILD},
  };

  for (auto it = typeMap.begin(); it != typeMap.end(); it++) {
    EXPECT_EQ(GetOHDataType(it->first), it->second);
  }
}

TEST_F(ScreenCaptureAdapterImplTest, GetOHAudioCaptureSourceType) {
  std::unordered_map<AudioCaptureSourceTypeAdapter, OH_AudioCaptureSourceType>
      typeMap = {
          {AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT,
           OH_AudioCaptureSourceType::OH_SOURCE_DEFAULT},
          {AudioCaptureSourceTypeAdapter::MIC,
           OH_AudioCaptureSourceType::OH_MIC},
          {AudioCaptureSourceTypeAdapter::ALL_PLAYBACK,
           OH_AudioCaptureSourceType::OH_ALL_PLAYBACK},
          {AudioCaptureSourceTypeAdapter::APP_PLAYBACK,
           OH_AudioCaptureSourceType::OH_APP_PLAYBACK},
          {AudioCaptureSourceTypeAdapter::SOURCE_INVALID,
           OH_AudioCaptureSourceType::OH_SOURCE_INVALID},
      };

  for (auto it = typeMap.begin(); it != typeMap.end(); it++) {
    EXPECT_EQ(GetOHAudioCaptureSourceType(it->first), it->second);
  }
}

TEST_F(ScreenCaptureAdapterImplTest, GetOHAudioCodecFormat) {
  std::unordered_map<AudioCodecFormatAdapter, OH_AudioCodecFormat> formatMap = {
      {AudioCodecFormatAdapter::AUDIO_DEFAULT,
       OH_AudioCodecFormat::OH_AUDIO_DEFAULT},
      {AudioCodecFormatAdapter::AAC_LC, OH_AudioCodecFormat::OH_AAC_LC},
      {AudioCodecFormatAdapter::AUDIO_CODEC_FORMAT_BUTT,
       OH_AudioCodecFormat::OH_AUDIO_CODEC_FORMAT_BUTT},
  };

  for (auto it = formatMap.begin(); it != formatMap.end(); it++) {
    EXPECT_EQ(GetOHAudioCodecFormat(it->first), it->second);
  }
}

TEST_F(ScreenCaptureAdapterImplTest, GetOHVideoSourceType) {
  std::unordered_map<VideoSourceTypeAdapter, OH_VideoSourceType> typeMap = {
      {VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_YUV,
       OH_VideoSourceType::OH_VIDEO_SOURCE_SURFACE_YUV},
      {VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_ES,
       OH_VideoSourceType::OH_VIDEO_SOURCE_SURFACE_ES},
      {VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_RGBA,
       OH_VideoSourceType::OH_VIDEO_SOURCE_SURFACE_RGBA},
      {VideoSourceTypeAdapter::VIDEO_SOURCE_BUTT,
       OH_VideoSourceType::OH_VIDEO_SOURCE_BUTT},
  };

  for (auto it = typeMap.begin(); it != typeMap.end(); it++) {
    EXPECT_EQ(GetOHVideoSourceType(it->first), it->second);
  }
}

TEST_F(ScreenCaptureAdapterImplTest, GetOHVideoCodecFormat) {
  std::unordered_map<VideoCodecFormatAdapter, OH_VideoCodecFormat> formatMap = {
      {VideoCodecFormatAdapter::VIDEO_DEFAULT,
       OH_VideoCodecFormat::OH_VIDEO_DEFAULT},
      {VideoCodecFormatAdapter::H264, OH_VideoCodecFormat::OH_H264},
      {VideoCodecFormatAdapter::MPEG4, OH_VideoCodecFormat::OH_MPEG4},
      {VideoCodecFormatAdapter::VIDEO_CODEC_FORMAT_BUTT,
       OH_VideoCodecFormat::OH_VIDEO_CODEC_FORMAT_BUTT},
  };

  for (auto it = formatMap.begin(); it != formatMap.end(); it++) {
    EXPECT_EQ(GetOHVideoCodecFormat(it->first), it->second);
  }
}

TEST_F(ScreenCaptureAdapterImplTest, GetOHContainerFormatType) {
  std::unordered_map<ContainerFormatTypeAdapter, OH_ContainerFormatType>
      typeMap = {
          {ContainerFormatTypeAdapter::CFT_MPEG_4A_TYPE,
           OH_ContainerFormatType::CFT_MPEG_4A},
          {ContainerFormatTypeAdapter::CFT_MPEG_4_TYPE,
           OH_ContainerFormatType::CFT_MPEG_4},
      };

  for (auto it = typeMap.begin(); it != typeMap.end(); it++) {
    EXPECT_EQ(GetOHContainerFormatType(it->first), it->second);
  }
}

TEST_F(ScreenCaptureAdapterImplTest,
       TestConvertScreenCaptureConfig_ShouldReturnNull_WhenConfigIsNull) {
  OH_DataType expected_result = OH_DataType::OH_ORIGINAL_STREAM;
  OH_DataType actual_result = ConvertScreenCaptureConfig(nullptr).dataType;
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(ScreenCaptureAdapterImplTest,
       TestConvertScreenCaptureConfig_ShouldReturnNull_WhenDataTypeIsInvalid) {
  char* expected_result = nullptr;
  std::shared_ptr<ScreenCaptureConfigAdapterMock> configMock =
      GetScreenCaptureConfig();
  char* actual_result =
      ConvertScreenCaptureConfig(configMock).recorderInfo.url;
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(ScreenCaptureAdapterImplTest,
       TestConvertScreenCaptureConfig_ShouldReturnNull_WhenRecordInfoIsNull) {
  char* expected_result = nullptr;
  std::shared_ptr<ScreenCaptureConfigAdapterMock> configMock =
      GetScreenCaptureConfig();
  configMock->SetDataType(DataTypeAdapter::CAPTURE_FILE_DATA_TYPE);
  char* actual_result =
      ConvertScreenCaptureConfig(configMock).recorderInfo.url;
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(
    ScreenCaptureAdapterImplTest,
    TestConvertScreenCaptureConfig_ShouldNotReturnNull_WhenConditionIsValid) {
  OH_DataType expected_result = OH_DataType::OH_INVAILD;
  std::shared_ptr<ScreenCaptureConfigAdapterMock> configMock =
      GetScreenCaptureConfig();
  configMock->SetDataType(DataTypeAdapter::CAPTURE_FILE_DATA_TYPE);
  std::shared_ptr<RecorderInfoAdapterMock> record_info_mock =
      std::make_shared<RecorderInfoAdapterMock>();
  record_info_mock->url_ = "test.com";
  configMock->SetRecorderInfo(record_info_mock);
  OH_DataType actual_result =
      ConvertScreenCaptureConfig(configMock).dataType;
  EXPECT_NE(expected_result, actual_result);
}

TEST_F(ScreenCaptureAdapterImplTest,
       TestSetAudioInfo_ShouldReturn_WhenAudioInfoIsNull) {
  std::shared_ptr<AudioInfoAdapter> audio_info = nullptr;
  SetAudioInfo(audio_info);
  EXPECT_FALSE(audio_info);
}

TEST_F(ScreenCaptureAdapterImplTest,
       TestSetAudioInfo_ShouldReturn_WhenCapInfoAndEncInfoAreNull) {
  std::shared_ptr<AudioInfoAdapterMock> audio_info = GetAudioInfo(true);
  SetAudioInfo(audio_info);
  EXPECT_FALSE(audio_info->GetMicCapInfo());
  EXPECT_FALSE(audio_info->GetInnerCapInfo());
  EXPECT_FALSE(audio_info->GetAudioEncInfo());
}

TEST_F(ScreenCaptureAdapterImplTest,
       TestSetAudioInfo_ShouldReturn_WhenCapInfoAndEncInfoAreNotNull) {
  std::shared_ptr<AudioInfoAdapterMock> audio_info = GetAudioInfo(false);
  SetAudioInfo(audio_info);
  EXPECT_TRUE(audio_info->GetMicCapInfo());
  EXPECT_TRUE(audio_info->GetInnerCapInfo());
  EXPECT_TRUE(audio_info->GetAudioEncInfo());
}

TEST_F(ScreenCaptureAdapterImplTest,
       TestSetVideoInfo_ShouldReturn_WhenVideoInfoIsNull) {
  std::shared_ptr<VideoInfoAdapter> video_info = nullptr;
  SetVideoInfo(video_info);
  EXPECT_FALSE(video_info);
}

TEST_F(ScreenCaptureAdapterImplTest,
       TestSetVideoInfo_ShouldReturn_WhenCapInfoAndEncInfoAreNull) {
  std::shared_ptr<VideoInfoAdapterMock> video_info =
      std::make_shared<VideoInfoAdapterMock>();
  video_info->SetVideoCapInfo(nullptr);
  video_info->SetVideoEncInfo(nullptr);
  SetVideoInfo(video_info);
  EXPECT_FALSE(video_info->GetVideoCapInfo());
  EXPECT_FALSE(video_info->GetVideoEncInfo());
}

TEST_F(ScreenCaptureAdapterImplTest,
       TestSetVideoInfo_ShouldReturn_WhenCapInfoAndEncInfoAreNotNull) {
  auto displayMgr = OHOS::NWeb::OhosAdapterHelperExt::CreateDisplayMgrAdapter();
  EXPECT_TRUE(displayMgr);
  auto display = displayMgr->GetDefaultDisplay();
  EXPECT_TRUE(display);
  int32_t videoFrameWidth = display->GetWidth();
  int32_t videoFrameHeight = display->GetHeight();

  // setting video information
  std::shared_ptr<VideoCaptureInfoAdapterMock> videoCapInfo =
      std::make_shared<VideoCaptureInfoAdapterMock>();
  videoCapInfo->SetVideoFrameWidth(videoFrameWidth);
  videoCapInfo->SetVideoFrameHeight(videoFrameHeight);
  videoCapInfo->SetVideoSourceType(
      OHOS::NWeb::VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_RGBA);

  // setting Video Encoding Information
  std::shared_ptr<VideoEncInfoAdapterMock> videoEncInfo =
      std::make_shared<VideoEncInfoAdapterMock>();
  videoEncInfo->SetVideoBitrate(kUnittestVideoBitrate);
  videoEncInfo->SetVideoFrameRate(kUnittestVideoFrameRate);
  videoEncInfo->SetVideoCodecFormat(OHOS::NWeb::VideoCodecFormatAdapter::H264);

  std::shared_ptr<VideoInfoAdapterMock> video_info =
      std::make_shared<VideoInfoAdapterMock>();
  video_info->SetVideoCapInfo(videoCapInfo);
  video_info->SetVideoEncInfo(videoEncInfo);
  SetVideoInfo(video_info);
  EXPECT_TRUE(video_info->GetVideoCapInfo());
  EXPECT_TRUE(video_info->GetVideoEncInfo());
}

TEST_F(ScreenCaptureAdapterImplTest, GetScreenCaptureStateCodeAdapter) {
  std::unordered_map<OH_AVScreenCaptureStateCode, ScreenCaptureStateCodeAdapter>
      typeMap = {
          {OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_STARTED,
           ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STARTED},
          {OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_CANCELED,
           ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_CANCELED},
          {OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_STOPPED_BY_USER,
           ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STOPPED_BY_USER},
          {OH_AVScreenCaptureStateCode::
               OH_SCREEN_CAPTURE_STATE_INTERRUPTED_BY_OTHER,
           ScreenCaptureStateCodeAdapter::
               SCREEN_CAPTURE_STATE_INTERRUPTED_BY_OTHER},
          {OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_STOPPED_BY_CALL,
           ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STOPPED_BY_CALL},
          {OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_MIC_UNAVAILABLE,
           ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_MIC_UNAVAILABLE},
          {OH_AVScreenCaptureStateCode::
               OH_SCREEN_CAPTURE_STATE_MIC_MUTED_BY_USER,
           ScreenCaptureStateCodeAdapter::
               SCREEN_CAPTURE_STATE_MIC_MUTED_BY_USER},
          {OH_AVScreenCaptureStateCode::
               OH_SCREEN_CAPTURE_STATE_MIC_UNMUTED_BY_USER,
           ScreenCaptureStateCodeAdapter::
               SCREEN_CAPTURE_STATE_MIC_UNMUTED_BY_USER},
          {OH_AVScreenCaptureStateCode::
               OH_SCREEN_CAPTURE_STATE_ENTER_PRIVATE_SCENE,
           ScreenCaptureStateCodeAdapter::
               SCREEN_CAPTURE_STATE_ENTER_PRIVATE_SCENE},
          {OH_AVScreenCaptureStateCode::
               OH_SCREEN_CAPTURE_STATE_EXIT_PRIVATE_SCENE,
           ScreenCaptureStateCodeAdapter::
               SCREEN_CAPTURE_STATE_EXIT_PRIVATE_SCENE},
          {OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_CANCELED,
           ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_INVLID},
      };

  for (auto it = typeMap.begin(); it != typeMap.end(); it++) {
    EXPECT_EQ(OHOS::NWeb::GetScreenCaptureStateCodeAdapter(it->first),
              it->second);
  }
}

TEST_F(ScreenCaptureAdapterImplTest, ConvertAudioCaptureSourceType) {
  std::unordered_map<OH_AudioCaptureSourceType, AudioCaptureSourceTypeAdapter>
      typeMap = {
          {OH_AudioCaptureSourceType::OH_SOURCE_DEFAULT,
           AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT},
          {OH_AudioCaptureSourceType::OH_MIC,
           AudioCaptureSourceTypeAdapter::MIC},
          {OH_AudioCaptureSourceType::OH_ALL_PLAYBACK,
           AudioCaptureSourceTypeAdapter::ALL_PLAYBACK},
          {OH_AudioCaptureSourceType::OH_APP_PLAYBACK,
           AudioCaptureSourceTypeAdapter::APP_PLAYBACK},
          {OH_AudioCaptureSourceType::OH_SOURCE_INVALID,
           AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT},
      };

  for (auto it = typeMap.begin(); it != typeMap.end(); it++) {
    EXPECT_EQ(ConvertAudioCaptureSourceType(it->first), it->second);
  }
}

TEST_F(ScreenCaptureAdapterImplTest,
       TestScreenCaptureCallbackOnError_ShouldReturn_WhenUserDataIsNull) {
  OH_AVScreenCapture* capture = nullptr;
  int32_t errorCode = 0;
  void* userData = nullptr;
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnError(capture, errorCode, userData);
  EXPECT_FALSE(userData);
}

TEST_F(ScreenCaptureAdapterImplTest,
       TestScreenCaptureCallbackOnError_ShouldReturn_WhenCallbackIsNull) {
  OH_AVScreenCapture* capture = nullptr;
  int32_t errorCode = 0;
  CallbackInfo cb;
  cb.callback = nullptr;
  void* userData = static_cast<void*>(&cb);
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnError(capture, errorCode, userData);
  EXPECT_TRUE(userData);
}

TEST_F(ScreenCaptureAdapterImplTest,
       TestScreenCaptureCallbackOnError_ShouldReturn_WhenCallbackIsNotNull) {
  OH_AVScreenCapture* capture = nullptr;
  int32_t errorCode = 0;
  CallbackInfo cb;
  cb.callback = std::make_shared<OHOSScreenCaptureCallbackMock>();
  void* userData = static_cast<void*>(&cb);
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnError(capture, errorCode, userData);
  EXPECT_TRUE(userData);
}

TEST_F(
    ScreenCaptureAdapterImplTest,
    TestScreenCaptureCallbackOnBufferAvailable_ShouldReturn_WhenUserDataIsNull) {
  OH_AVScreenCapture* capture = nullptr;
  OH_AVBuffer* buffer = nullptr;
  void* userData = nullptr;
  OH_AVScreenCaptureBufferType bufferType =
      OH_AVScreenCaptureBufferType::OH_SCREEN_CAPTURE_BUFFERTYPE_VIDEO;
  int64_t timestamp = 0;
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnBufferAvailable(
    capture, buffer, bufferType, timestamp, userData);
  EXPECT_FALSE(userData);
}

TEST_F(
    ScreenCaptureAdapterImplTest,
    TestScreenCaptureCallbackOnBufferAvailable_ShouldReturn_WhenBufferIsNull) {
  OH_AVScreenCapture* capture = nullptr;
  OH_AVBuffer* buffer = nullptr;
  CallbackInfo cb;
  cb.callback = nullptr;
  void* userData = static_cast<void*>(&cb);
  OH_AVScreenCaptureBufferType bufferType =
      OH_AVScreenCaptureBufferType::OH_SCREEN_CAPTURE_BUFFERTYPE_VIDEO;
  int64_t timestamp = 0;
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnBufferAvailable(
    capture, buffer, bufferType, timestamp, userData);
  EXPECT_FALSE(buffer);
}

TEST_F(
    ScreenCaptureAdapterImplTest,
    TestScreenCaptureCallbackOnBufferAvailable_ShouldReturn_WhenCallbackIsNull) {
  OH_AVScreenCapture* capture = nullptr;
  OH_AVBuffer* buffer = OH_AVBuffer_Create(1);
  CallbackInfo cb;
  cb.callback = nullptr;
  void* userData = static_cast<void*>(&cb);
  OH_AVScreenCaptureBufferType bufferType =
      OH_AVScreenCaptureBufferType::OH_SCREEN_CAPTURE_BUFFERTYPE_VIDEO;
  int64_t timestamp = 0;
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnBufferAvailable(
    capture, buffer, bufferType, timestamp, userData);
  EXPECT_FALSE(cb.callback);
  OH_AVBuffer_Destroy(buffer);
}

TEST_F(
    ScreenCaptureAdapterImplTest,
    TestScreenCaptureCallbackOnBufferAvailable_ShouldReturn_WhenBufferTypeIsVideo) {
  OH_AVScreenCapture* capture = nullptr;
  OH_AVBuffer* buffer = OH_AVBuffer_Create(1);
  CallbackInfo cb;
  cb.callback = std::make_shared<OHOSScreenCaptureCallbackMock>();
  void* userData = static_cast<void*>(&cb);
  OH_AVScreenCaptureBufferType bufferType =
      OH_AVScreenCaptureBufferType::OH_SCREEN_CAPTURE_BUFFERTYPE_VIDEO;
  int64_t timestamp = 0;
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnBufferAvailable(
    capture, buffer, bufferType, timestamp, userData);
  EXPECT_NE(
      bufferType,
      OH_AVScreenCaptureBufferType::OH_SCREEN_CAPTURE_BUFFERTYPE_AUDIO_MIC);
  OH_AVBuffer_Destroy(buffer);
}

TEST_F(
    ScreenCaptureAdapterImplTest,
    TestScreenCaptureCallbackOnBufferAvailable_ShouldReturn_WhenBufferTypeIsAudioInner) {
  OH_AVScreenCapture* capture = nullptr;
  OH_AVBuffer* buffer = OH_AVBuffer_Create(1);
  CallbackInfo cb;
  cb.callback = std::make_shared<OHOSScreenCaptureCallbackMock>();
  void* userData = static_cast<void*>(&cb);
  OH_AVScreenCaptureBufferType bufferType =
      OH_AVScreenCaptureBufferType::OH_SCREEN_CAPTURE_BUFFERTYPE_AUDIO_INNER;
  int64_t timestamp = 0;
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnBufferAvailable(
    capture, buffer, bufferType, timestamp, userData);
  EXPECT_NE(
      bufferType,
      OH_AVScreenCaptureBufferType::OH_SCREEN_CAPTURE_BUFFERTYPE_AUDIO_MIC);
  OH_AVBuffer_Destroy(buffer);
}

TEST_F(
    ScreenCaptureAdapterImplTest,
    TestScreenCaptureCallbackOnBufferAvailable_ShouldReturn_WhenBufferTypeIsAudioMic) {
  OH_AVScreenCapture* capture = nullptr;
  OH_AVBuffer* buffer = OH_AVBuffer_Create(1);
  CallbackInfo cb;
  cb.callback = std::make_shared<OHOSScreenCaptureCallbackMock>();
  void* userData = static_cast<void*>(&cb);
  OH_AVScreenCaptureBufferType bufferType =
      OH_AVScreenCaptureBufferType::OH_SCREEN_CAPTURE_BUFFERTYPE_AUDIO_MIC;
  int64_t timestamp = 0;
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnBufferAvailable(
    capture, buffer, bufferType, timestamp, userData);
  EXPECT_NE(
      bufferType,
      OH_AVScreenCaptureBufferType::OH_SCREEN_CAPTURE_BUFFERTYPE_AUDIO_INNER);
  OH_AVBuffer_Destroy(buffer);
}

TEST_F(ScreenCaptureAdapterImplTest,
       TestScreenCaptureCallbackOnStateChange_ShouldReturn_WhenUserDataIsNull) {
  OH_AVScreenCapture* capture = nullptr;
  OH_AVScreenCaptureStateCode stateCode =
      OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_STARTED;
  void* userData = nullptr;
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnStateChange(capture, stateCode, userData);
  EXPECT_FALSE(userData);
}

TEST_F(ScreenCaptureAdapterImplTest,
       TestScreenCaptureCallbackOnStateChange_ShouldReturn_WhenCallbackIsNull) {
  OH_AVScreenCapture* capture = nullptr;
  OH_AVScreenCaptureStateCode stateCode =
      OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_STARTED;
  CallbackInfo cb;
  cb.callback = nullptr;
  void* userData = static_cast<void*>(&cb);
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnStateChange(capture, stateCode, userData);
  EXPECT_FALSE(cb.callback);
}

TEST_F(
    ScreenCaptureAdapterImplTest,
    TestScreenCaptureCallbackOnStateChange_ShouldReturn_WhenCallbackIsNotNull) {
  OH_AVScreenCapture* capture = nullptr;
  OH_AVScreenCaptureStateCode stateCode =
      OH_AVScreenCaptureStateCode::OH_SCREEN_CAPTURE_STATE_STARTED;
  CallbackInfo cb;
  cb.callback = std::make_shared<OHOSScreenCaptureCallbackMock>();
  void* userData = static_cast<void*>(&cb);
  ScreenCaptureAdapterImpl::ScreenCaptureCallbackOnStateChange(capture, stateCode, userData);
  EXPECT_TRUE(cb.callback);
}

TEST_F(ScreenCaptureAdapterImplTest, OH_AudioBufferAdapterImpl_Functions) {
  OH_AVBuffer* avBuffer = OH_AVBuffer_Create(1);
  OH_AudioBufferAdapterImpl buffer(
      avBuffer, 1, OH_AudioCaptureSourceType::OH_SOURCE_DEFAULT);
  EXPECT_NE(buffer.GetBuffer(), nullptr);
  EXPECT_EQ(buffer.GetLength(), 1);
  EXPECT_EQ(buffer.GetTimestamp(), 1);
  EXPECT_EQ(buffer.GetSourcetype(),
            AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT);

  OH_AudioBufferAdapterImpl buffer1(
      nullptr, 1, OH_AudioCaptureSourceType::OH_SOURCE_DEFAULT);
  EXPECT_EQ(buffer1.GetBuffer(), nullptr);

  buffer1.SetBuffer(buffer1.avBuffer_);
  buffer1.SetLength(1);
  buffer1.SetTimestamp(1);
  buffer1.SetSourcetype(AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT);

  OH_AVBuffer_Destroy(avBuffer);
}
