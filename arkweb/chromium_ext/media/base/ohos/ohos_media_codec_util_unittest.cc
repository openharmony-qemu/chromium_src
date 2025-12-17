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
#include <memory>
#include "base/logging.h"
#include "base/task/single_thread_task_executor.h"
#include "base/task/task_runner.h"
#include "base/trace_event/trace_event.h"
#include "media/base/ohos/decoder_format_adapter_impl.h"
#include "ohos_adapter_helper.h"
#include "ohos_media_codec_util.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/adapter_base.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::MockFunction;
using ::testing::NiceMock;
using ::testing::Ref;
using ::testing::Return;

namespace media {

class MockMediaCodecListAdapter : public OHOS::NWeb::MediaCodecListAdapter {
 public:
  MOCK_METHOD(std::shared_ptr<CapabilityDataAdapter>,
              GetCodecCapability,
              (const std::string&, bool),
              (override));
};

class MockOhosAdapterHelper : public OHOS::NWeb::OhosAdapterHelper {
 public:
  MOCK_METHOD(std::unique_ptr<AafwkAppMgrClientAdapter>,
              CreateAafwkAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<PowerMgrClientAdapter>,
              CreatePowerMgrClientAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<DisplayManagerAdapter>,
              CreateDisplayMgrAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<BatteryMgrClientAdapter>,
              CreateBatteryClientAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<NetConnectAdapter>,
              CreateNetConnectAdapter,
              (),
              (override));
  MOCK_METHOD(OhosWebDataBaseAdapter&,
              GetOhosWebDataBaseAdapterInstance,
              (),
              (override));
  MOCK_METHOD(PasteBoardClientAdapter&, GetPasteBoard, (), (override));
  MOCK_METHOD(std::unique_ptr<AudioRendererAdapter>,
              CreateAudioRendererAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<AudioCapturerAdapter>,
              CreateAudioCapturerAdapter,
              (),
              (override));
  MOCK_METHOD(AudioSystemManagerAdapter&,
              GetAudioSystemManager,
              (),
              (override));
  MOCK_METHOD(OhosWebPermissionDataBaseAdapter&,
              GetWebPermissionDataBaseInstance,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<MMIAdapter>, CreateMMIAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<SocPerfClientAdapter>,
              CreateSocPerfClientAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<OhosResourceAdapter>,
              GetResourceAdapter,
              (const std::string&),
              (override));
  MOCK_METHOD(SystemPropertiesAdapter&,
              GetSystemPropertiesInstance,
              (),
              (override));
  MOCK_METHOD(VSyncAdapter&, GetVSyncAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<OhosInitWebAdapter>,
              GetInitWebAdapter,
              (),
              (override));
  MOCK_METHOD(KeystoreAdapter&, GetKeystoreAdapterInstance, (), (override));
  MOCK_METHOD(EnterpriseDeviceManagementAdapter&,
              GetEnterpriseDeviceManagementInstance,
              (),
              (override));
  MOCK_METHOD(DatashareAdapter&, GetDatashareInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<IMFAdapter>, CreateIMFAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<CertManagerAdapter>,
              GetRootCertDataAdapter,
              (),
              (override));
  MOCK_METHOD(AccessTokenAdapter&,
              GetAccessTokenAdapterInstance,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<EventHandlerAdapter>,
              GetEventHandlerAdapter,
              (),
              (override));
  MOCK_METHOD(PrintManagerAdapter&, GetPrintManagerInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<IConsumerSurfaceAdapter>,
              CreateConsumerSurfaceAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<PlayerAdapter>,
              CreatePlayerAdapter,
              (),
              (override));
  MOCK_METHOD(WindowAdapter&, GetWindowAdapterInstance, (), (override));
  MOCK_METHOD(HiSysEventAdapter&, GetHiSysEventAdapterInstance, (), (override));
  MOCK_METHOD(HiTraceAdapter&, GetHiTraceAdapterInstance, (), (override));
  MOCK_METHOD(NetProxyAdapter&, GetNetProxyInstance, (), (override));
  MOCK_METHOD(CameraManagerAdapter&, GetCameraManagerAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<ScreenCaptureAdapter>,
              CreateScreenCaptureAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<DateTimeFormatAdapter>,
              CreateDateTimeFormatAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<MediaCodecDecoderAdapter>,
              CreateMediaCodecDecoderAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<NativeImageAdapter>,
              CreateNativeImageAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<MediaCodecAdapter>,
              CreateMediaCodecEncoderAdapter,
              (),
              (override));
  MOCK_METHOD(MediaCodecListAdapter&, GetMediaCodecListAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<FlowbufferAdapter>,
              CreateFlowbufferAdapter,
              (),
              (override));
  MOCK_METHOD(std::shared_ptr<MediaAVSessionAdapter>,
              CreateMediaAVSessionAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<OhosImageDecoderAdapter>,
              CreateOhosImageDecoderAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<SensorAdapter>,
              CreateSensorAdapter,
              (),
              (override));
  MOCK_METHOD(void,
              SetArkWebCoreHapPathOverride,
              (const std::string&),
              (override));
  MOCK_METHOD(OhosNativeBufferAdapter&,
              GetOhosNativeBufferAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<AudioCodecDecoderAdapter>,
              CreateAudioCodecDecoderAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<DrmAdapter>,
              CreateDrmAdapter,
              (),
              (override));
  MOCK_METHOD(OhosDrawingTextFontAdapter&,
               GetOhosDrawingTextFontAdapter,
               (),
               (override));
  MOCK_METHOD(OhosDrawingTextTypographyAdapter&,
              GetOhosDrawingTextTypographyAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<MigrationManagerAdapter>,
              CreateMigrationMgrAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<ScreenlockManagerAdapter>,
              CreateScreenlockManagerAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<CertManagerAdapter>,
              GetCertManagerAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<NetConfigAdapter>,
              GetNetConfigAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<NetEventAdapter>,
              CreateNetEventAdapter,
              (),
              (override));
  MOCK_METHOD(ColorPickerAdapter&,
              GetColorPickerAdapter,
              (),
              (override));
  MOCK_METHOD(HiAppeventAdapter&,
              GetHiAppeventAdapterInstance,
              (),
              (override));
  static MockOhosAdapterHelper& GetInstance() {
    static MockOhosAdapterHelper instance;
    return instance;
  }
};

class MockCapabilityDataAdapter : public CapabilityDataAdapter {
 public:
  MOCK_METHOD(int32_t, GetMaxWidth, (), (override));
  MOCK_METHOD(int32_t, GetMaxHeight, (), (override));
  MOCK_METHOD(int32_t, GetMaxframeRate, (), (override));
};

class OHOSMediaCodecUtilTest : public ::testing::Test {
 protected:
  void SetUp() override {
    mock_adapter_helper_ = new testing::NiceMock<media::MockOhosAdapterHelper>;
    ON_CALL(*mock_adapter_helper_, GetMediaCodecListAdapter())
        .WillByDefault(testing::ReturnRef(mock_media_codec_list_adapter_));
  }

  void TearDown() override { delete mock_adapter_helper_; }

  std::shared_ptr<media::MockCapabilityDataAdapter>
      mock_capability_data_adapter_;
  media::MockOhosAdapterHelper* mock_adapter_helper_;
  media::MockMediaCodecListAdapter mock_media_codec_list_adapter_;
};

TEST_F(OHOSMediaCodecUtilTest, GetCodecCapabilityReturnsCorrectValues001) {
  const std::string mime_type = "video/avc";
  bool is_encoder = false;
  auto mock_adapter = std::make_shared<MockCapabilityDataAdapter>();
  ON_CALL(mock_media_codec_list_adapter_,
          GetCodecCapability(mime_type, is_encoder))
      .WillByDefault(testing::Return(mock_adapter));
  CapabilityData capability =
      OHOSMediaCodecUtil::GetCodecCapability(mime_type, is_encoder);
  ASSERT_NE(capability.maxWidth, 0);
  ASSERT_NE(capability.maxHeight, 0);
  ASSERT_NE(capability.maxframeRate, 0);
}

TEST_F(OHOSMediaCodecUtilTest, IsPassthroughAudioFormat001) {
  AudioCodec codec = AudioCodec::kAC3;
  bool res = OHOSMediaCodecUtil::IsPassthroughAudioFormat(codec);
  EXPECT_EQ(res, true);
}

TEST_F(OHOSMediaCodecUtilTest, IsPassthroughAudioFormat002) {
  AudioCodec codec = AudioCodec::kEAC3;
  bool res = OHOSMediaCodecUtil::IsPassthroughAudioFormat(codec);
  EXPECT_EQ(res, true);
}

TEST_F(OHOSMediaCodecUtilTest, IsPassthroughAudioFormat003) {
  AudioCodec codec = AudioCodec::kDTS;
  bool res = OHOSMediaCodecUtil::IsPassthroughAudioFormat(codec);
  EXPECT_EQ(res, true);
}

TEST_F(OHOSMediaCodecUtilTest, IsPassthroughAudioFormat004) {
  AudioCodec codec = AudioCodec::kDTSXP2;
  bool res = OHOSMediaCodecUtil::IsPassthroughAudioFormat(codec);
  EXPECT_EQ(res, true);
}

TEST_F(OHOSMediaCodecUtilTest, IsPassthroughAudioFormat005) {
  AudioCodec codec = AudioCodec::kMpegHAudio;
  bool res = OHOSMediaCodecUtil::IsPassthroughAudioFormat(codec);
  EXPECT_EQ(res, true);
}

TEST_F(OHOSMediaCodecUtilTest, IsPassthroughAudioFormat006) {
  AudioCodec codec = AudioCodec::kIAMF;
  bool res = OHOSMediaCodecUtil::IsPassthroughAudioFormat(codec);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSMediaCodecUtilTest, VideoCodecToOHOSMimeType001) {
  VideoCodec codec = VideoCodec::kH264;
  std::string res = OHOSMediaCodecUtil::CodecToOHOSMimeType(codec);
  EXPECT_EQ(res, "video/avc");
}

TEST_F(OHOSMediaCodecUtilTest, VideoCodecToOHOSMimeType002) {
  VideoCodec codec = VideoCodec::kHEVC;
  std::string res = OHOSMediaCodecUtil::CodecToOHOSMimeType(codec);
  EXPECT_EQ(res, "video/hevc");
}

TEST_F(OHOSMediaCodecUtilTest, VideoCodecToOHOSMimeType003) {
  VideoCodec codec = VideoCodec::kVP8;
  std::string res = OHOSMediaCodecUtil::CodecToOHOSMimeType(codec);
  EXPECT_EQ(res, "video/x-vnd.on2.vp8");
}

TEST_F(OHOSMediaCodecUtilTest, VideoCodecToOHOSMimeType004) {
  VideoCodec codec = VideoCodec::kVP9;
  std::string res = OHOSMediaCodecUtil::CodecToOHOSMimeType(codec);
  EXPECT_EQ(res, "video/x-vnd.on2.vp9");
}

TEST_F(OHOSMediaCodecUtilTest, VideoCodecToOHOSMimeType005) {
  VideoCodec codec = VideoCodec::kDolbyVision;
  std::string res = OHOSMediaCodecUtil::CodecToOHOSMimeType(codec);
  EXPECT_EQ(res, "video/dolby-vision");
}

TEST_F(OHOSMediaCodecUtilTest, VideoCodecToOHOSMimeType006) {
  VideoCodec codec = VideoCodec::kAV1;
  std::string res = OHOSMediaCodecUtil::CodecToOHOSMimeType(codec);
  EXPECT_EQ(res, "video/av01");
}

TEST_F(OHOSMediaCodecUtilTest, VideoCodecToOHOSMimeType007) {
  VideoCodec codec = VideoCodec::kTheora;
  std::string res = OHOSMediaCodecUtil::CodecToOHOSMimeType(codec);
  EXPECT_EQ(res, "");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnBit_WhenFormatIsDts) {
  AudioCodec codec = AudioCodec::kMP3;
  SampleFormat sample_format = SampleFormat::kSampleFormatDts;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/raw");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnBit_WhenFormatIsDtsxP2) {
  AudioCodec codec = AudioCodec::kMP3;
  SampleFormat sample_format = SampleFormat::kSampleFormatDtsxP2;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/raw");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnBit_WhenFormatIsAc3) {
  AudioCodec codec = AudioCodec::kMP3;
  SampleFormat sample_format = SampleFormat::kSampleFormatAc3;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/raw");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnBit_WhenFormatIsEac3) {
  AudioCodec codec = AudioCodec::kMP3;
  SampleFormat sample_format = SampleFormat::kSampleFormatEac3;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/raw");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnBit_WhenFormatIsMpegHAudio) {
  AudioCodec codec = AudioCodec::kMP3;
  SampleFormat sample_format = SampleFormat::kSampleFormatMpegHAudio;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/raw");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnBit_WhenCodecIsAC3) {
  AudioCodec codec = AudioCodec::kAC3;
  SampleFormat sample_format = SampleFormat::kSampleFormatMpegHAudio;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/raw");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnBit_WhenCodecIsEAC3) {
  AudioCodec codec = AudioCodec::kEAC3;
  SampleFormat sample_format = SampleFormat::kSampleFormatMpegHAudio;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/raw");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnBit_WhenCodecIsDTS) {
  AudioCodec codec = AudioCodec::kDTS;
  SampleFormat sample_format = SampleFormat::kSampleFormatMpegHAudio;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/raw");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnBit_WhenCodecIsDTSXP2) {
  AudioCodec codec = AudioCodec::kDTSXP2;
  SampleFormat sample_format = SampleFormat::kSampleFormatMpegHAudio;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/raw");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnBit_WhenCodecIsMpegHAudio) {
  AudioCodec codec = AudioCodec::kMpegHAudio;
  SampleFormat sample_format = SampleFormat::kSampleFormatMpegHAudio;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/raw");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnMP3_WhenIsPassThrougnFalse) {
  AudioCodec codec = AudioCodec::kMP3;
  SampleFormat sample_format = SampleFormat::kSampleFormatPlanarU8;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/mpeg");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnVorbis_WhenIsPassThrougnFalse) {
  AudioCodec codec = AudioCodec::kVorbis;
  SampleFormat sample_format = SampleFormat::kSampleFormatPlanarU8;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/vorbis");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnFLAC_WhenIsPassThrougnFalse) {
  AudioCodec codec = AudioCodec::kFLAC;
  SampleFormat sample_format = SampleFormat::kSampleFormatPlanarU8;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/flac");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnOpus_WhenIsPassThrougnFalse) {
  AudioCodec codec = AudioCodec::kOpus;
  SampleFormat sample_format = SampleFormat::kSampleFormatPlanarU8;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/opus");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnAAC_WhenIsPassThrougnFalse) {
  AudioCodec codec = AudioCodec::kAAC;
  SampleFormat sample_format = SampleFormat::kSampleFormatPlanarU8;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/mp4a-latm");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnDTSE_WhenIsPassThrougnFalse) {
  AudioCodec codec = AudioCodec::kDTSE;
  SampleFormat sample_format = SampleFormat::kSampleFormatPlanarU8;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "audio/vnd.dts;profile=lbr");
}

TEST_F(OHOSMediaCodecUtilTest,
       TestAudioCodecToOHOSMimeType_ShouldReturnNull_WhenCodecIsIAMF) {
  AudioCodec codec = AudioCodec::kIAMF;
  SampleFormat sample_format = SampleFormat::kSampleFormatPlanarU8;
  std::string res =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(codec, sample_format);
  EXPECT_EQ(res, "");
}

TEST_F(OHOSMediaCodecUtilTest, VideoCanDecode001) {
  bool is_secure = false;
  VideoCodec codec = VideoCodec::kTheora;
  bool res = OHOSMediaCodecUtil::CanDecode(codec, is_secure);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSMediaCodecUtilTest, VideoCanDecode002) {
  bool is_secure = false;
  VideoCodec codec = VideoCodec::kH264;
  bool res = OHOSMediaCodecUtil::CanDecode(codec, is_secure);
  EXPECT_EQ(res, true);
}

TEST_F(OHOSMediaCodecUtilTest, AudioCanDecode001) {
  AudioCodec codec = AudioCodec::kIAMF;
  bool res = OHOSMediaCodecUtil::CanDecode(codec);
  EXPECT_EQ(res, false);
}

TEST_F(OHOSMediaCodecUtilTest, AudioCanDecode002) {
  AudioCodec codec = AudioCodec::kDTSE;
  bool res = OHOSMediaCodecUtil::CanDecode(codec);
  EXPECT_EQ(res, false);
}
}  // namespace media
