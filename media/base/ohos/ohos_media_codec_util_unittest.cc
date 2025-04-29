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

#include "ohos_media_codec_util.h"
#include <memory>
#include "base/logging.h"
#include "base/task/single_thread_task_executor.h"
#include "base/task/task_runner.h"
#include "base/trace_event/trace_event.h"
#include "media/base/audio_codecs.h"
#include "media/base/ohos/decoder_format_adapter_impl.h"
#include "media/base/video_codecs.h"
#include "ohos_adapter_helper.h"
#include "testing/gmock/include/gmock/gmock.h"
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
  MOCK_METHOD(std::unique_ptr<MediaAVSessionAdapter>,
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
  MOCK_METHOD(std::unique_ptr<MigrationManagerAdapter>,
              CreateMigrationMgrAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<AudioCodecDecoderAdapter>,
              CreateAudioCodecDecoderAdapter,
              (),
              (override));
  MOCK_METHOD(std::unique_ptr<DrmAdapter>, CreateDrmAdapter, (), (override));
  MOCK_METHOD(OhosDrawingTextFontAdapter&,
              GetOhosDrawingTextFontAdapter,
              (),
              (override));
  MOCK_METHOD(OhosDrawingTextTypographyAdapter&,
              GetOhosDrawingTextTypographyAdapter,
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

TEST_F(OHOSMediaCodecUtilTest, GetCodecCapabilityReturnsCorrectValues) {
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

TEST_F(OHOSMediaCodecUtilTest, CodecToOHOSMimeType) {
  auto result = OHOSMediaCodecUtil::CodecToOHOSMimeType(AudioCodec::kMP3);
  EXPECT_EQ(result, "audio/mpeg");
}

TEST_F(OHOSMediaCodecUtilTest, CodecToOHOSMimeType_Audio) {
  auto result1 = OHOSMediaCodecUtil::CodecToOHOSMimeType(AudioCodec::kMP3,
                                                         kSampleFormatAc3);
  EXPECT_EQ(result1, "audio/raw");

  auto result2 = OHOSMediaCodecUtil::CodecToOHOSMimeType(AudioCodec::kVorbis,
                                                         kUnknownSampleFormat);
  EXPECT_EQ(result2, "audio/vorbis");

  auto result3 = OHOSMediaCodecUtil::CodecToOHOSMimeType(AudioCodec::kMP3,
                                                         kUnknownSampleFormat);
  EXPECT_EQ(result3, "audio/mpeg");

  auto result4 = OHOSMediaCodecUtil::CodecToOHOSMimeType(AudioCodec::kFLAC,
                                                         kUnknownSampleFormat);
  EXPECT_EQ(result4, "audio/flac");

  auto result5 = OHOSMediaCodecUtil::CodecToOHOSMimeType(AudioCodec::kOpus,
                                                         kUnknownSampleFormat);
  EXPECT_EQ(result5, "audio/opus");

  auto result6 = OHOSMediaCodecUtil::CodecToOHOSMimeType(AudioCodec::kOpus,
                                                         kUnknownSampleFormat);
  EXPECT_EQ(result6, "audio/opus");

  auto result7 = OHOSMediaCodecUtil::CodecToOHOSMimeType(AudioCodec::kAAC,
                                                         kUnknownSampleFormat);
  EXPECT_EQ(result7, "audio/mp4a-latm");

  auto result8 = OHOSMediaCodecUtil::CodecToOHOSMimeType(
      AudioCodec::kAC3, kSampleFormatPlanarS32);
  EXPECT_EQ(result8, "audio/raw");

  auto result9 = OHOSMediaCodecUtil::CodecToOHOSMimeType(
      AudioCodec::kDTS, kSampleFormatPlanarS32);
  EXPECT_EQ(result9, "audio/raw");

  auto result10 = OHOSMediaCodecUtil::CodecToOHOSMimeType(AudioCodec::kDTSE,
                                                          kUnknownSampleFormat);
  EXPECT_EQ(result10, "audio/vnd.dts;profile=lbr");

  auto result11 = OHOSMediaCodecUtil::CodecToOHOSMimeType(AudioCodec::kDTSXP2,
                                                          kUnknownSampleFormat);
  EXPECT_EQ(result11, "audio/raw");
}

TEST_F(OHOSMediaCodecUtilTest, CodecToOHOSMimeType_Video) {
  auto result1 = OHOSMediaCodecUtil::CodecToOHOSMimeType(VideoCodec::kH264);
  EXPECT_EQ(result1, "video/avc");

  auto result2 = OHOSMediaCodecUtil::CodecToOHOSMimeType(VideoCodec::kHEVC);
  EXPECT_EQ(result1, "video/avc");

  auto result3 = OHOSMediaCodecUtil::CodecToOHOSMimeType(VideoCodec::kVP8);
  EXPECT_EQ(result3, "video/x-vnd.on2.vp8");

  auto result4 = OHOSMediaCodecUtil::CodecToOHOSMimeType(VideoCodec::kVP9);
  EXPECT_EQ(result4, "video/x-vnd.on2.vp9");

  auto result5 =
      OHOSMediaCodecUtil::CodecToOHOSMimeType(VideoCodec::kDolbyVision);
  EXPECT_EQ(result5, "video/dolby-vision");

  auto result6 = OHOSMediaCodecUtil::CodecToOHOSMimeType(VideoCodec::kAV1);
  EXPECT_EQ(result6, "video/av01");
}

TEST_F(OHOSMediaCodecUtilTest, IsPassthroughAudioFormat) {
  auto result_true =
      OHOSMediaCodecUtil::IsPassthroughAudioFormat(AudioCodec::kAC3);
  EXPECT_EQ(result_true, true);

  auto result_false =
      OHOSMediaCodecUtil::IsPassthroughAudioFormat(AudioCodec::kMP3);
  EXPECT_EQ(result_false, false);
}

TEST_F(OHOSMediaCodecUtilTest, CanDecode_Video) {
  VideoCodec codec = VideoCodec::kDolbyVision;
  bool result_true = OHOSMediaCodecUtil::CanDecode(codec, false);
  EXPECT_TRUE(result_true);
}

TEST_F(OHOSMediaCodecUtilTest, CanDecode_Audio) {
  AudioCodec codec = AudioCodec::kAC3;
  bool result_true = OHOSMediaCodecUtil::CanDecode(codec);
  EXPECT_TRUE(result_true);
}
}  // namespace media
