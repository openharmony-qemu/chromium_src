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

#include "base/functional/callback_helpers.h"
#include "ohos_media_decoder_bridge_impl.h"
#include "ohos_adapter_helper.h" 
#include "testing/gmock/include/gmock/gmock.h"
#include <memory>
#include "base/logging.h"
#include "base/task/single_thread_task_executor.h"
#include "base/task/task_runner.h"
#include "base/trace_event/trace_event.h"
#include "media/base/ohos/decoder_format_adapter_impl.h"
#include "ohos_adapter_helper.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/adapter_base.h"

#include "base/base_paths.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/location.h"
#include "base/threading/thread.h"
#include "base/threading/thread_restrictions.h"
#include "base/time/time.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Eq;
using ::testing::Invoke;
using ::testing::MockFunction;
using ::testing::NiceMock;
using ::testing::Ref;
using ::testing::Return;
using namespace OHOS::NWeb;
using namespace std;
const std::string PRODUCT_MODEL = "noemulator";

namespace testing {

template <>
class NiceMock<OHOS::NWeb::OhosBufferAdapter>
    : public OHOS::NWeb::OhosBufferAdapter {
 public:
  MOCK_METHOD(uint8_t*, GetAddr, (), (override));
  MOCK_METHOD(uint32_t, GetBufferSize, (), (override));
};

template <>
class NiceMock<BufferInfoAdapter> : public BufferInfoAdapter {
 public:
  MOCK_METHOD(int64_t, GetPresentationTimeUs, (), (override));
  MOCK_METHOD(int32_t, GetSize, (), (override));
  MOCK_METHOD(int32_t, GetOffset, (), (override));
};

}  // namespace testing

namespace media {

class MockSystemPropertiesAdapter : public OHOS::NWeb::SystemPropertiesAdapter {
 public:
  MOCK_METHOD(bool, GetResourceUseHapPathEnable, (), (override));
  MOCK_METHOD(std::string, GetDeviceInfoProductModel, (), (override));
  MOCK_METHOD(std::string, GetDeviceInfoBrand, (), (override));
  MOCK_METHOD(int32_t, GetDeviceInfoMajorVersion, (), (override));
  MOCK_METHOD(ProductDeviceType, GetProductDeviceType, (), (override));
  MOCK_METHOD(bool, GetWebOptimizationValue, (), (override));
  MOCK_METHOD(bool, IsAdvancedSecurityMode, (), (override));
  MOCK_METHOD(std::string, GetUserAgentOSName, (), (override));
  MOCK_METHOD(int32_t, GetSoftwareMajorVersion, (), (override));
  MOCK_METHOD(int32_t, GetSoftwareSeniorVersion, (), (override));
  MOCK_METHOD(std::string, GetNetlogMode, (), (override));
  MOCK_METHOD(bool, GetTraceDebugEnable, (), (override));
  MOCK_METHOD(std::string, GetSiteIsolationMode, (), (override));
  MOCK_METHOD(int32_t, GetFlowBufMaxFd, (), (override));
  MOCK_METHOD(bool, GetOOPGPUEnable, (), (override));
  MOCK_METHOD(void, SetOOPGPUDisable, (), (override));
  MOCK_METHOD(void,
              AttachSysPropObserver,
              (PropertiesKey key, SystemPropertiesObserver* observer),
              (override));
  MOCK_METHOD(void,
              DetachSysPropObserver,
              (PropertiesKey key, SystemPropertiesObserver* observer),
              (override));
  MOCK_METHOD(bool,
              GetBoolParameter,
              (const std::string& key, bool defaultValue),
              (override));
  MOCK_METHOD(std::vector<FrameRateSetting>,
              GetLTPOConfig,
              (const std::string& settingName),
              (override));
  MOCK_METHOD(std::string, GetOOPGPUStatus, (), (override));
  MOCK_METHOD(bool,
              IsLTPODynamicApp,
              (const std::string& bundleName),
              (override));
  MOCK_METHOD(int32_t, GetLTPOStrategy, (), (override));
  MOCK_METHOD(std::string, GetUserAgentBaseOSName, (), (override));
  MOCK_METHOD(std::string, GetUserAgentOSVersion, (), (override));
  MOCK_METHOD(std::string, GetVulkanStatus, (), (override));
  MOCK_METHOD(std::string, GetCompatibleDeviceType, (), (override));
  MOCK_METHOD(std::string, GetDeviceInfoApiVersion, (), (override));
  MOCK_METHOD(std::string, GetPRPPreloadMode, (), (override));
  MOCK_METHOD(std::string, GetScrollVelocityScale, (), (override));
  MOCK_METHOD(std::string, GetScrollFriction, (), (override));
  MOCK_METHOD(std::string, GetBundleName, (), (override));
  MOCK_METHOD(std::string, GetStringParameter, (const std::string& key, const td::string& defaultValue), (override));
  MOCK_METHOD(int32_t, GetInitialCongestionWindowSize, (), (override));
};


class MockWindowAdapter : public WindowAdapter {
 public:
  MOCK_METHOD(NWebNativeWindow,
              CreateNativeWindowFromSurface,
              (void* pSurface),
              (override));
  MOCK_METHOD(void, DestroyNativeWindow, (NWebNativeWindow window), ());
  MOCK_METHOD(int32_t,
              NativeWindowSetBufferGeometry,
              (NWebNativeWindow window, int32_t width, int32_t height),
              (override));
  MOCK_METHOD(void,
              NativeWindowSurfaceCleanCache,
              (NWebNativeWindow window),
              (override));
  MOCK_METHOD(void,
              NativeWindowSurfaceCleanCacheWithPara,
              (NWebNativeWindow window, bool cleanAll),
              (override));
  MOCK_METHOD(void,
              SetTransformHint,
              (uint32_t rotation, NWebNativeWindow window),
              (override));
  MOCK_METHOD(void,
              AddNativeWindowRef,
              (NWebNativeWindow window),
              (override));
  MOCK_METHOD(void,
              NativeWindowUnRef,
              (NWebNativeWindow window),
              (override));
};

class MockMediaCodecDecoderAdapter : public MediaCodecDecoderAdapter {
 public:
  MOCK_METHOD(DecoderAdapterCode,
              CreateVideoDecoderByMime,
              (const std::string& mime),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              CreateVideoDecoderByName,
              (const std::string& name),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              ConfigureDecoder,
              (std::shared_ptr<DecoderFormatAdapter> adapter),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              SetParameterDecoder,
              (std::shared_ptr<DecoderFormatAdapter> adapter),
              (override));
  MOCK_METHOD(DecoderAdapterCode, SetOutputSurface, (void* window), (override));
  MOCK_METHOD(DecoderAdapterCode, PrepareDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, StartDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, StopDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, FlushDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, ReleaseDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, ResetDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode,
              QueueInputBufferDec,
              (uint32_t, int64_t, int32_t, int32_t, BufferFlag),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              GetOutputFormatDec,
              (std::shared_ptr<DecoderFormatAdapter> format),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              ReleaseOutputBufferDec,
              (uint32_t, bool),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              SetCallbackDec,
              (std::shared_ptr<DecoderCallbackAdapter> adapter),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              SetDecryptionConfig,
              (void*, bool),
              (override));
  MOCK_METHOD(DecoderAdapterCode,
              SetAVCencInfo,
              (uint32_t, const std::shared_ptr<AudioCencInfoAdapter>),
              (override));
};

class MockOhosAdapterHelper : public OhosAdapterHelper {
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
  static MockOhosAdapterHelper& GetInstance() {
    static MockOhosAdapterHelper instance;
    return instance;
  }
};

class MockDecoderBridgeSignal : public DecoderBridgeSignal {
 public:
  MOCK_METHOD(void, pop, (), ());
  MOCK_METHOD(bool, isOnError, (), ());
  MOCK_METHOD(bool, isDecoderFlushing, (), ());
  MOCK_METHOD(size_t, size, (), ());
  MOCK_METHOD(uint32_t, front_inputBufferIndex, (), ());
  MOCK_METHOD(OhosBuffer, front_inputBuffer, (), ());
  MOCK_METHOD(bool, empty, (), ());
  MOCK_METHOD(int64_t, front_outputBufferInfo_presentationTimeUs, (), ());
  MOCK_METHOD(uint32_t, front_outputBufferIndex, (), ());
  MOCK_METHOD(BufferFlag, front_outputBufferFlag, (), ());
  MOCK_METHOD(void,
              swap_inputQueue_,
              (std::queue<VideoBridgeDecoderInputBuffer>&),
              ());
  MOCK_METHOD(void,
              swap_outputQueue_,
              (std::queue<VideoBridgeDecoderOutputBuffer>&),
              ());
  MOCK_METHOD(bool, isDecoderFlushing_, (), ());
};

class MockSequencedTaskRunner : public base::SequencedTaskRunner {
 public:
  MOCK_METHOD(bool,
              PostNonNestableTask,
              (const base::Location& location, base::OnceClosure onceClosure),
              ());
  MOCK_METHOD(bool,
              PostNonNestableDelayedTask,
              (const base::Location& location, base::OnceClosure onceClosure, ase::TimeDelta timeDelta),
              (override));
  MOCK_METHOD(base::DelayedTaskHandle,
              PostCancelableDelayedTask,
              (base::subtle::PostDelayedTaskPassKey postDelayedTaskPassKey,
               const base::Location& location,
               base::OnceClosure onceClosure,
               base::TimeDelta timeDelta),
              (override));
  MOCK_METHOD(base::DelayedTaskHandle,
              PostCancelableDelayedTaskAt,
              (base::subtle::PostDelayedTaskPassKey postDelayedTaskPassKey,
               const base::Location& location,
               base::OnceClosure onceClosure,
               base::TimeTicks timeTicks,
               base::subtle::DelayPolicy delayPolicy),
              (override));
  MOCK_METHOD(bool,
              PostDelayedTaskAt,
              (base::subtle::PostDelayedTaskPassKey postDelayedTaskPassKey,
               const base::Location& location,
               base::OnceClosure onceClosure,
               base::TimeTicks timeTicks,
               base::subtle::DelayPolicy delayPolicy),
              (override));
  MOCK_METHOD(bool, RunsTasksInCurrentSequence, (), (const, override));
  MOCK_METHOD(bool,
              DeleteOrReleaseSoonInternal,
              (const base::Location&, void (*)(const void*), const void*),
              (override));
  MOCK_METHOD(bool, PostDelayedTask, (const base::Location&, base::OnceClosure, ase::TimeDelta), (override));
};

class MockDecoderFormatAdapter : public DecoderFormatAdapter {
 public:
  MOCK_METHOD(int32_t, GetWidth, (), (override));
  MOCK_METHOD(int32_t, GetHeight, (), (override));
  MOCK_METHOD(double, GetFrameRate, (), (override));
  MOCK_METHOD(void, SetWidth, (int32_t width), (override));
  MOCK_METHOD(void, SetHeight, (int32_t height), (override));
  MOCK_METHOD(void, SetFrameRate, (double frameRate), (override));
  MOCK_METHOD(DecoderAdapterCode, SetParameterDecoder, (const td::shared_ptr<DecoderFormatAdapter> format));
  MOCK_METHOD(DecoderAdapterCode, PrepareDecoder, ());
};

class InheritBufferInfoAdapter : public BufferInfoAdapter {
 public:
  int64_t GetPresentationTimeUs() override { return 100; }
  int32_t GetSize() override { return 1; }
  int32_t GetOffset() override { return 0; }
};

class InheritOhosBufferAdapter : public OhosBufferAdapter {
 public:
  uint8_t* GetAddr() override { return nullptr; }
  uint32_t GetBufferSize() override { return 8; }
};

class MediaCodecDecoderBridgeImplTest : public ::testing::Test {
 public:
  void SetSignal(std::shared_ptr<NiceMock<MockDecoderBridgeSignal>> signal) {
    bridge_->signal_ = signal;
  }

  void SetCodecEncodeBridgeCallback(std::shared_ptr<CodecBridgeCallback> allback) {
    bridge_->cb_ = callback;
  }

  void SetVideoDecoder(std::unique_ptr<MediaCodecDecoderAdapter> decoder) {
    bridge_->videoDecoder_ = std::move(decoder);
  }

  std::shared_ptr<CodecBridgeCallback> GetCodecEncodeBridgeCallback() {
    return bridge_->cb_;
  }

  DecoderAdapterCode PrepareForCallback() {
    return bridge_->PrepareForCallback();
  }
 protected:
  void SetUp() override {
    mock_adapter_ = std::make_unique<NiceMock<MockMediaCodecDecoderAdapter>>();
    signal_ = std::make_shared<NiceMock<MockDecoderBridgeSignal>>();
    ohos_adapter_helper_mock_ =
        std::make_unique<NiceMock<MockOhosAdapterHelper>>();
    window_adapter_mock_ = std::make_unique<NiceMock<MockWindowAdapter>>();
    ON_CALL(*ohos_adapter_helper_mock_, GetWindowAdapterInstance())
        .WillByDefault(ReturnRef(*window_adapter_mock_));

    bridge_ = std::unique_ptr<MediaCodecDecoderBridgeImpl>(new MediaCodecDecoderBridgeImpl("video/avc"));
    bridge_->videoDecoder_ = std::move(mock_adapter_);
    bridge_->signal_ = signal_;
    callback_ = std::make_shared<CodecBridgeCallback>(signal_);

    ON_CALL(*ohos_adapter_helper_mock_, GetSystemPropertiesInstance())
        .WillByDefault(ReturnRef(system_properties_adapter_mock_));
    ON_CALL(system_properties_adapter_mock_, GetDeviceInfoProductModel())
        .WillByDefault(Return("NotAnEmulator"));
    mock_signal_ = std::make_shared<NiceMock<DecoderBridgeSignal>>();
    callback_c = std::make_shared<CodecBridgeCallback>(mock_signal_);

    video_decoder_ = new testing::NiceMock<MockMediaCodecDecoderAdapter>;
  }

  void TearDown() override {
    window_adapter_mock_.reset();
    bridge_.reset();
    mock_adapter_.reset();
    signal_.reset();
    ohos_adapter_helper_mock_.reset();
    callback_.reset();
    ohos_adapter_helper_mock_.reset();
    delete video_decoder_;
  }

  std::shared_ptr<CodecBridgeCallback> callback_;
  std::unique_ptr<NiceMock<MockWindowAdapter>> window_adapter_mock_;
  std::unique_ptr<NiceMock<MockOhosAdapterHelper>> ohos_adapter_helper_mock_;
  std::shared_ptr<NiceMock<MockDecoderBridgeSignal>> signal_;
  std::unique_ptr<NiceMock<MockMediaCodecDecoderAdapter>> mock_adapter_;
  std::unique_ptr<MediaCodecDecoderBridgeImpl> bridge_;
  std::shared_ptr<NiceMock<MockOhosAdapterHelper>> ohos_adapter_helper_mocks_;
  NiceMock<MockSystemPropertiesAdapter> system_properties_adapter_mock_;
  std::shared_ptr<NiceMock<DecoderBridgeSignal>> mock_signal_;
  std::shared_ptr<CodecBridgeCallback> callback_c;
  MockMediaCodecDecoderAdapter* video_decoder_;
  bool isFirstDecFrame_ = true;
};

TEST_F(MediaCodecDecoderBridgeImplTest, CreateVideoDecoder_WhenCodecTypeIsH264) {
  VideoBridgeCodecConfig config;
  config.codec = media::VideoCodec::kH264;
  config.on_buffers_available_cb = base::DoNothing();
  auto bridge_impl = MediaCodecDecoderBridgeImpl::CreateVideoDecoder(config);
  ASSERT_NE(bridge_impl, nullptr);
}

TEST_F(MediaCodecDecoderBridgeImplTest, CreateVideoDecoder_WhenCodecTypeIsHEVC) {
  VideoBridgeCodecConfig config;
  config.codec = media::VideoCodec::kHEVC;
  config.on_buffers_available_cb = base::DoNothing();
  auto bridge_impl = MediaCodecDecoderBridgeImpl::CreateVideoDecoder(config);
  ASSERT_NE(bridge_impl, nullptr);
}

TEST_F(MediaCodecDecoderBridgeImplTest, reateVideoDecoder_ShouldReturnNull_WhenCodecTypeIsAV1) {
  VideoBridgeCodecConfig config;
  config.codec = media::VideoCodec::kAV1;
  config.on_buffers_available_cb = base::DoNothing();
  auto bridge_impl = MediaCodecDecoderBridgeImpl::CreateVideoDecoder(config);
  ASSERT_EQ(bridge_impl, nullptr);
}

TEST_F(MediaCodecDecoderBridgeImplTest, repareForCallback_WhenSignalAndCbAreNullptr) {
  SetSignal(nullptr);
  SetCodecEncodeBridgeCallback(nullptr);
  std::shared_ptr<DecoderCallbackAdapter> codec_cb = GetCodecEncodeBridgeCallback);
  auto mock_media_player_ = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_media_player_, SetCallbackDec(testing::_))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_media_player_, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_media_player_));
  DecoderAdapterCode result = PrepareForCallback();
  ASSERT_TRUE(result == DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, repareForCallback_WhenSignalAndCbAreNotNullptr) {
  SetSignal(make_shared<NiceMock<MockDecoderBridgeSignal>>());
  SetCodecEncodeBridgeCallback(make_shared<CodecBridgeCallback>(signal_));
  std::shared_ptr<DecoderCallbackAdapter> codec_cb = GetCodecEncodeBridgeCallback);
  auto mock_media_player_ = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_media_player_, SetCallbackDec(codec_cb))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_media_player_, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_media_player_));

  DecoderAdapterCode result = PrepareForCallback();
  ASSERT_TRUE(result == DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, PrepareForCallback_WhenSignalIsNullptr) {
  SetSignal(nullptr);
  SetCodecEncodeBridgeCallback(make_shared<CodecBridgeCallback>(signal_));
  std::shared_ptr<DecoderCallbackAdapter> codec_cb = GetCodecEncodeBridgeCallback();
  auto mock_media_player_ = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_media_player_, SetCallbackDec(codec_cb))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_media_player_, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_media_player_));

  DecoderAdapterCode result = PrepareForCallback();
  ASSERT_TRUE(result == DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, PrepareForCallback_WhenCbIsNullptr) {
  SetSignal(make_shared<NiceMock<MockDecoderBridgeSignal>>());
  SetCodecEncodeBridgeCallback(nullptr);
  auto mock_media_player_ = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_media_player_, SetCallbackDec)
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_media_player_, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_media_player_));

  DecoderAdapterCode result = PrepareForCallback();
  ASSERT_TRUE(result == DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, reateVideoBridgeDecoderByMime_ShouldReturnError_WhenDecoderIsNull) {
  SetVideoDecoder(nullptr);
  mock_adapter_ = std::make_unique<NiceMock<MockMediaCodecDecoderAdapter>>();
  std::string codec_name = "video/h264";

  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByMime(codec_name);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}


TEST_F(MediaCodecDecoderBridgeImplTest, reateVideoBridgeDecoderByMime_ShouldReturnError_WhenCreateDecoderByMineFails) {
  std::string codec_name = "video/h264";
  auto mock_media_player_ = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_media_player_, CreateVideoDecoderByMime(codec_name))
      .WillOnce(Return(DecoderAdapterCode::DECODER_ERROR));
  SetVideoDecoder(std::move(mock_media_player_));

  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByMime(codec_name);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, reateVideoBridgeDecoderByMime_ShouldReturnOk_WhenCreateDecoderByMineSucceeds) {
  std::string codec_name = "video/h264";
  auto mock_media_player_ = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_media_player_, CreateVideoDecoderByMime(codec_name))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_media_player_, SetCallbackDec(testing::_))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_media_player_, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_media_player_));

  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByMime(codec_name);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, reateVideoBridgeDecoderByName_ShouldReturnError_WhenVideoDecoderIsNull) {
  SetVideoDecoder(nullptr);
  std::string codec_name = "video/h264";
  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByName(codec_name);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, reateVideoBridgeDecoderByName_ShouldReturnOk_WhenCreateVideoDecoderByNameSucceeds {
  std::string codec_name = "video/h264";
  auto mock_media_player_ = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_media_player_, CreateVideoDecoderByName(codec_name))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_media_player_, SetCallbackDec(testing::_))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_media_player_, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_media_player_));

  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByName(codec_name);
  ASSERT_TRUE(result == DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, reateVideoBridgeDecoderByName_ShouldReturnError_WhenCreateVideoDecoderByNameFails {
  std::string codec_name = "video/h264";
  auto mock_media_player_ = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_media_player_, CreateVideoDecoderByName(codec_name))
      .WillOnce(Return(DecoderAdapterCode::DECODER_ERROR));
  EXPECT_CALL(*mock_media_player_, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_media_player_));

  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByName(codec_name);
  EXPECT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, onfigureBridgeDecoder_ShouldReturnError_WhenVideoDecoderIsNull) {
  DecoderFormat format;
  format.width = 640;
  format.height = 480;
  format.frameRate = 30.0;
  scoped_refptr<base::SequencedTaskRunner> decoder_task_runner = nullptr;
  auto signal = std::make_shared<MockDecoderBridgeSignal>();
  SetCodecEncodeBridgeCallback(std::make_shared<CodecBridgeCallback>(signal));
  SetVideoDecoder(nullptr);

  DecoderAdapterCode result =
      bridge_->ConfigureBridgeDecoder(format, decoder_task_runner);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, onfigureBridgeDecoder_ShouldReturnOk_WhenConfigureDecoderSucceeds) {
  DecoderFormat format;
  format.width = 640;
  format.height = 480;
  format.frameRate = 30.0;
  scoped_refptr<base::SequencedTaskRunner> decoder_task_runner = nullptr;
  auto signal = std::make_shared<MockDecoderBridgeSignal>();
  SetCodecEncodeBridgeCallback(std::make_shared<CodecBridgeCallback>(signal));
  auto mock_media_player_ = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_media_player_, ConfigureDecoder(testing::_))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_media_player_, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_media_player_));

  DecoderAdapterCode result =
      bridge_->ConfigureBridgeDecoder(format, decoder_task_runner);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, Constructor) {
  auto ohos_adapter_helper_mock =
      std::make_shared<NiceMock<MockOhosAdapterHelper>>();
  mock_adapter_ = std::make_unique<NiceMock<MockMediaCodecDecoderAdapter>>();
  base::RepeatingClosure on_buffers_available_cb = base::DoNothing();
  ASSERT_FALSE(bridge_->CheckHasCreated());
}

TEST_F(MediaCodecDecoderBridgeImplTest, SetBridgeParameterDecoder) {
  auto video_deocder = make_unique<MockMediaCodecDecoderAdapter>();
  SetVideoDecoder(std::move(video_deocder));
  DecoderFormat format;
  format.width = 640;
  format.height = 480;
  format.frameRate = 30.0;
  DecoderAdapterCode result = bridge_->SetBridgeParameterDecoder(format);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, etBridgeParameterDecoder_ShouldReturnError_WhenVideoDecoderIsNull) {
  DecoderFormat format;
  format.width = 640;
  format.height = 480;
  format.frameRate = 30.0;
  SetVideoDecoder(nullptr);
  DecoderAdapterCode result = bridge_->SetBridgeParameterDecoder(format);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, etBridgeOutputSurface_ShouldReturnError_WhenWindowIsNull) {
  void* window = nullptr;
  DecoderAdapterCode result = bridge_->SetBridgeOutputSurface(window);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, etBridgeOutputSurface_ShouldReturnError_WhenVideoDecoderIsNull) {
  SetVideoDecoder(nullptr);
  int32_t value = 1;
  intptr_t int_as_ptr = reinterpret_cast<intptr_t>(&value);
  void* window = reinterpret_cast<void*>(int_as_ptr);
  DecoderAdapterCode result = bridge_->SetBridgeOutputSurface(window);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, etBridgeOutputSurface_ShouldReturnOk_WhenSetOutPutSurfaceSucceds) {
  int32_t value = 1;
  intptr_t int_as_ptr = reinterpret_cast<intptr_t>(&value);
  void* window = reinterpret_cast<void*>(int_as_ptr);
  auto mock_media_player_ = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_media_player_, SetOutputSurface(window))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_media_player_, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_media_player_));

  DecoderAdapterCode result = bridge_->SetBridgeOutputSurface(window);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, etOutputFormatBridgeDecoder_ShouldReturnError_WhenVideoDecoderIsNull) {
  DecoderFormat format;
  format.width = 0;
  format.height = 0;
  format.frameRate = 0.0;
  SetVideoDecoder(nullptr);

  DecoderAdapterCode result = bridge_->GetOutputFormatBridgeDecoder(format);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, etOutputFormatBridgeDecoder_ShouldReturnOk_WhenGetOutputFormatDecSucceeds) {
  DecoderFormat format;
  format.width = 640;
  format.height = 480;
  format.frameRate = 30.0;
  auto video_decoder = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*video_decoder, GetOutputFormatDec)
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  SetVideoDecoder(std::move(video_decoder));

  DecoderAdapterCode result = bridge_->GetOutputFormatBridgeDecoder(format);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, repareBridgeDecoder_ShouldReturnError_WhenVideoDecoderIsNull) {
  SetVideoDecoder(nullptr);
  DecoderAdapterCode result = bridge_->PrepareBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, repareBridgeDecoder_ShouldReturnOk_WhenPrepareBridgeDecoderSucceeds) {
  auto video_decoder = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*video_decoder, PrepareDecoder())
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  SetVideoDecoder(std::move(video_decoder));

  DecoderAdapterCode result = bridge_->PrepareBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, tartBridgeDecoder_ShouldReturnError_WhenVideoDecoderIsNull) {
  SetVideoDecoder(nullptr);
  DecoderAdapterCode result = bridge_->StartBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, tartBridgeDecoder_ShouldReturnOk_WhenStartDecoderSucceeds) {
  auto mock_media_player_ = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_media_player_, StartDecoder())
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_media_player_, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_media_player_));

  DecoderAdapterCode result = bridge_->StartBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, topBridgeDecoder_ShouldReturnError_WhenVideoDecoderIsNull) {
  SetVideoDecoder(nullptr);
  DecoderAdapterCode result = bridge_->StopBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, topBridgeDecoder_ShouldReturnOk_WhenStopDecoderSucceeds) {
  auto mock_media_player_ = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_media_player_, StopDecoder())
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_media_player_, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_media_player_));
  DecoderAdapterCode result = bridge_->StopBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

}  // namespace media