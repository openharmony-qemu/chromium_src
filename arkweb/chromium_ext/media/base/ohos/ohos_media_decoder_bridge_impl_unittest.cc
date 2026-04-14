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

#include <memory>

#include "base/functional/callback_helpers.h"
#define private public
#include "ohos_media_decoder_bridge_impl.h"
#undef private
#include "base/base_paths.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/task/single_thread_task_executor.h"
#include "base/task/task_runner.h"
#include "base/threading/thread.h"
#include "base/threading/thread_restrictions.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "media/base/ohos/decoder_format_adapter_impl.h"
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
  MOCK_METHOD(std::string,
              GetStringParameter,
              (const std::string& key, const std::string& defaultValue),
              (override));
  MOCK_METHOD(int32_t, GetInitialCongestionWindowSize, (), (override));
  MOCK_METHOD(int32_t,
              GetIntParameter,
              (const std::string& key, int32_t defaultValue),
              (override));
  MOCK_METHOD(int32_t, GetLoadUrlStrategy, (), (override));
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
  MOCK_METHOD(void, AddNativeWindowRef, (NWebNativeWindow window), (override));
  MOCK_METHOD(void, NativeWindowUnRef, (NWebNativeWindow window), (override));
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
  MOCK_METHOD(std::unique_ptr<DrmAdapter>, CreateDrmAdapter, (), (override));
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
  MOCK_METHOD(ColorPickerAdapter&, GetColorPickerAdapter, (), (override));
  MOCK_METHOD(HiAppeventAdapter&,
              GetHiAppeventAdapterInstance,
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
};

class MockSequencedTaskRunner : public base::SequencedTaskRunner {
 public:
  MOCK_METHOD(bool,
              PostNonNestableTask,
              (const base::Location& location, base::OnceClosure onceClosure),
              ());
  MOCK_METHOD(bool,
              PostNonNestableDelayedTask,
              (const base::Location& location,
               base::OnceClosure onceClosure,
               base::TimeDelta timeDelta),
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
  MOCK_METHOD(bool,
              PostDelayedTask,
              (const base::Location&, base::OnceClosure, base::TimeDelta),
              (override));
};

class MockDecoderFormatAdapter : public DecoderFormatAdapter {
 public:
  MOCK_METHOD(int32_t, GetWidth, (), (override));
  MOCK_METHOD(int32_t, GetHeight, (), (override));
  MOCK_METHOD(double, GetFrameRate, (), (override));
  MOCK_METHOD(void, SetWidth, (int32_t width), (override));
  MOCK_METHOD(void, SetHeight, (int32_t height), (override));
  MOCK_METHOD(void, SetFrameRate, (double frameRate), (override));
  MOCK_METHOD(DecoderAdapterCode,
              SetParameterDecoder,
              (const std::shared_ptr<DecoderFormatAdapter> format));
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

  void SetCodecEncodeBridgeCallback(
      std::shared_ptr<CodecBridgeCallback> callback) {
    bridge_->cb_ = callback;
  }

  void SetVideoDecoder(std::unique_ptr<MediaCodecDecoderAdapter> decoder) {
    bridge_->videoDecoder_ = std::move(decoder);
  }

  void SetIsRunning(bool is_running) { bridge_->isRunning_.store(is_running); }

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void SetVideoSurfaceId(int32_t id) { bridge_->video_surface_id_ = id; }

  void SetWindowFromSurface(void* surface) {
    bridge_->window_from_surface_ = surface;
  }
#endif

  std::shared_ptr<CodecBridgeCallback> GetCodecEncodeBridgeCallback() {
    return bridge_->cb_;
  }

  std::shared_ptr<DecoderBridgeSignal> GetSignal() { return bridge_->signal_; }

  DecoderAdapterCode PrepareForCallback() {
    return bridge_->PrepareForCallback();
  }

  void PopInqueueDec() { bridge_->PopInqueueDec(); }

  void PopOutqueueDec() { bridge_->PopOutqueueDec(); }

  DecoderAdapterCode PushInbufferDec(const uint32_t index,
                                     const uint32_t& bufferSize,
                                     const int64_t& time,
                                     bool is_key_frame) {
    return bridge_->PushInbufferDec(index, bufferSize, time, is_key_frame);
  }

  DecoderAdapterCode PushInbufferDecEos(const uint32_t index) {
    return bridge_->PushInbufferDecEos(index);
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
    VideoBridgeCodecConfig config;
    config.codec = media::VideoCodec::kH264;
    config.on_buffers_available_cb = base::DoNothing();
    bridge_ = MediaCodecDecoderBridgeImpl::CreateVideoDecoder(config);
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
};

TEST_F(MediaCodecDecoderBridgeImplTest,
       CreateVideoDecoder_WhenCodecTypeIsH264) {
  VideoBridgeCodecConfig config;
  config.codec = media::VideoCodec::kH264;
  config.on_buffers_available_cb = base::DoNothing();
  auto bridge_impl = MediaCodecDecoderBridgeImpl::CreateVideoDecoder(config);
  ASSERT_NE(bridge_impl, nullptr);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       CreateVideoDecoder_WhenCodecTypeIsHEVC) {
  VideoBridgeCodecConfig config;
  config.codec = media::VideoCodec::kHEVC;
  config.on_buffers_available_cb = base::DoNothing();
  auto bridge_impl = MediaCodecDecoderBridgeImpl::CreateVideoDecoder(config);
  ASSERT_NE(bridge_impl, nullptr);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       CreateVideoDecoder_ShouldReturnNull_WhenCodecTypeIsAV1) {
  VideoBridgeCodecConfig config;
  config.codec = media::VideoCodec::kAV1;
  config.on_buffers_available_cb = base::DoNothing();
  auto bridge_impl = MediaCodecDecoderBridgeImpl::CreateVideoDecoder(config);
  ASSERT_EQ(bridge_impl, nullptr);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       PrepareForCallback_WhenSignalAndCbAreNullptr) {
  SetSignal(nullptr);
  SetCodecEncodeBridgeCallback(nullptr);
  std::shared_ptr<DecoderCallbackAdapter> codec_cb =
      GetCodecEncodeBridgeCallback();
  auto mock_video_decode = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decode, SetCallbackDec(testing::_))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_video_decode, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decode));
  DecoderAdapterCode result = PrepareForCallback();
  ASSERT_TRUE(result == DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       PrepareForCallback_WhenSignalAndCbAreNotNullptr) {
  SetSignal(make_shared<NiceMock<MockDecoderBridgeSignal>>());
  SetCodecEncodeBridgeCallback(make_shared<CodecBridgeCallback>(signal_));
  std::shared_ptr<DecoderCallbackAdapter> codec_cb =
      GetCodecEncodeBridgeCallback();
  auto mock_video_decode = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decode, SetCallbackDec(codec_cb))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_video_decode, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decode));

  DecoderAdapterCode result = PrepareForCallback();
  ASSERT_TRUE(result == DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       PrepareForCallback_WhenSignalIsNullptr) {
  SetSignal(nullptr);
  SetCodecEncodeBridgeCallback(make_shared<CodecBridgeCallback>(signal_));
  std::shared_ptr<DecoderCallbackAdapter> codec_cb =
      GetCodecEncodeBridgeCallback();
  auto mock_video_decode = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decode, SetCallbackDec(codec_cb))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_video_decode, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decode));

  DecoderAdapterCode result = PrepareForCallback();
  ASSERT_TRUE(result == DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, PrepareForCallback_WhenCbIsNullptr) {
  SetSignal(make_shared<NiceMock<MockDecoderBridgeSignal>>());
  SetCodecEncodeBridgeCallback(nullptr);
  auto mock_video_decode = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decode, SetCallbackDec)
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_video_decode, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decode));

  DecoderAdapterCode result = PrepareForCallback();
  ASSERT_TRUE(result == DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       CreateVideoBridgeDecoderByMime_ShouldReturnError_WhenDecoderIsNull) {
  SetVideoDecoder(nullptr);
  mock_adapter_ = std::make_unique<NiceMock<MockMediaCodecDecoderAdapter>>();
  std::string codec_name = "video/h264";

  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByMime(codec_name);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(
    MediaCodecDecoderBridgeImplTest,
    CreateVideoBridgeDecoderByMime_ShouldReturnError_WhenCreateDecoderByMineFails) {
  std::string codec_name = "video/h264";
  auto mock_video_decode = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decode, CreateVideoDecoderByMime(codec_name))
      .WillOnce(Return(DecoderAdapterCode::DECODER_ERROR));
  SetVideoDecoder(std::move(mock_video_decode));

  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByMime(codec_name);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(
    MediaCodecDecoderBridgeImplTest,
    CreateVideoBridgeDecoderByMime_ShouldReturnOk_WhenCreateDecoderByMineSucceeds) {
  std::string codec_name = "video/h264";
  auto mock_video_decode = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decode, CreateVideoDecoderByMime(codec_name))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_video_decode, SetCallbackDec(testing::_))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_video_decode, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decode));

  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByMime(codec_name);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(
    MediaCodecDecoderBridgeImplTest,
    CreateVideoBridgeDecoderByName_ShouldReturnError_WhenVideoDecoderIsNull) {
  SetVideoDecoder(nullptr);
  std::string codec_name = "video/h264";
  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByName(codec_name);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(
    MediaCodecDecoderBridgeImplTest,
    CreateVideoBridgeDecoderByName_ShouldReturnOk_WhenCreateVideoDecoderByNameSucceeds) {
  std::string codec_name = "video/h264";
  auto mock_video_decode = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decode, CreateVideoDecoderByName(codec_name))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_video_decode, SetCallbackDec(testing::_))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_video_decode, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decode));

  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByName(codec_name);
  ASSERT_TRUE(result == DecoderAdapterCode::DECODER_OK);
}

TEST_F(
    MediaCodecDecoderBridgeImplTest,
    CreateVideoBridgeDecoderByName_ShouldReturnError_WhenCreateVideoDecoderByNameFails) {
  std::string codec_name = "video/h264";
  auto mock_video_decode = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decode, CreateVideoDecoderByName(codec_name))
      .WillOnce(Return(DecoderAdapterCode::DECODER_ERROR));
  EXPECT_CALL(*mock_video_decode, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decode));

  DecoderAdapterCode result =
      bridge_->CreateVideoBridgeDecoderByName(codec_name);
  EXPECT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       ConfigureBridgeDecoder_ShouldReturnError_WhenVideoDecoderIsNull) {
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

TEST_F(MediaCodecDecoderBridgeImplTest,
       ConfigureBridgeDecoder_ShouldReturnOk_WhenConfigureDecoderSucceeds) {
  DecoderFormat format;
  format.width = 640;
  format.height = 480;
  format.frameRate = 30.0;
  scoped_refptr<base::SequencedTaskRunner> decoder_task_runner = nullptr;
  auto signal = std::make_shared<MockDecoderBridgeSignal>();
  SetCodecEncodeBridgeCallback(std::make_shared<CodecBridgeCallback>(signal));
  auto mock_video_decode = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decode, ConfigureDecoder(testing::_))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_video_decode, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decode));

  DecoderAdapterCode result =
      bridge_->ConfigureBridgeDecoder(format, decoder_task_runner);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
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

TEST_F(MediaCodecDecoderBridgeImplTest,
       SetBridgeParameterDecoder_ShouldReturnError_WhenVideoDecoderIsNull) {
  DecoderFormat format;
  format.width = 640;
  format.height = 480;
  format.frameRate = 30.0;
  SetVideoDecoder(nullptr);
  DecoderAdapterCode result = bridge_->SetBridgeParameterDecoder(format);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       SetBridgeOutputSurface_ShouldReturnError_WhenWindowIsNull) {
  void* window = nullptr;
  DecoderAdapterCode result = bridge_->SetBridgeOutputSurface(window);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       SetBridgeOutputSurface_ShouldReturnError_WhenVideoDecoderIsNull) {
  SetVideoDecoder(nullptr);
  int32_t value = 1;
  intptr_t int_as_ptr = reinterpret_cast<intptr_t>(&value);
  void* window = reinterpret_cast<void*>(int_as_ptr);
  DecoderAdapterCode result = bridge_->SetBridgeOutputSurface(window);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       SetBridgeOutputSurface_ShouldReturnOk_WhenSetOutPutSurfaceSucceds) {
  int32_t value = 1;
  intptr_t int_as_ptr = reinterpret_cast<intptr_t>(&value);
  void* window = reinterpret_cast<void*>(int_as_ptr);
  auto mock_video_decode = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decode, SetOutputSurface(window))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_video_decode, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decode));

  DecoderAdapterCode result = bridge_->SetBridgeOutputSurface(window);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       GetOutputFormatBridgeDecoder_ShouldReturnError_WhenVideoDecoderIsNull) {
  DecoderFormat format;
  format.width = 0;
  format.height = 0;
  format.frameRate = 0.0;
  SetVideoDecoder(nullptr);

  DecoderAdapterCode result = bridge_->GetOutputFormatBridgeDecoder(format);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(
    MediaCodecDecoderBridgeImplTest,
    GetOutputFormatBridgeDecoder_ShouldReturnOk_WhenGetOutputFormatDecSucceeds) {
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

TEST_F(MediaCodecDecoderBridgeImplTest,
       PrepareBridgeDecoder_ShouldReturnError_WhenVideoDecoderIsNull) {
  SetVideoDecoder(nullptr);
  DecoderAdapterCode result = bridge_->PrepareBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       PrepareBridgeDecoder_ShouldReturnOk_WhenPrepareBridgeDecoderSucceeds) {
  auto video_decoder = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*video_decoder, PrepareDecoder())
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  SetVideoDecoder(std::move(video_decoder));

  DecoderAdapterCode result = bridge_->PrepareBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       StartBridgeDecoder_ShouldReturnError_WhenVideoDecoderIsNull) {
  SetVideoDecoder(nullptr);
  DecoderAdapterCode result = bridge_->StartBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       StartBridgeDecoder_ShouldReturnOk_WhenStartDecoderSucceeds) {
  auto mock_video_decode = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decode, StartDecoder())
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_video_decode, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decode));

  DecoderAdapterCode result = bridge_->StartBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       StopBridgeDecoder_ShouldReturnError_WhenVideoDecoderIsNull) {
  SetVideoDecoder(nullptr);
  DecoderAdapterCode result = bridge_->StopBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       StopBridgeDecoder_ShouldReturnOk_WhenStopDecoderSucceeds) {
  auto mock_video_decode = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decode, StopDecoder())
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_video_decode, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decode));
  DecoderAdapterCode result = bridge_->StopBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       FlushBridgeDecoder_ShouldReturnError_WhenDecoderIsNull) {
  SetVideoDecoder(nullptr);
  auto result = bridge_->FlushBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       FlushBridgeDecoder_ShouldReturnError_WhenSignalIsNull) {
  auto mock_video_decoder = make_unique<MockMediaCodecDecoderAdapter>();
  SetVideoDecoder(std::move(mock_video_decoder));
  SetSignal(nullptr);
  auto result = bridge_->FlushBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       FlushBridgeDecoder_ShouldReturnError_WhenFlushDecoderFails) {
  auto mock_video_decoder = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decoder, FlushDecoder())
      .WillOnce(Return(DecoderAdapterCode::DECODER_ERROR));
  EXPECT_CALL(*mock_video_decoder, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decoder));

  auto result = bridge_->FlushBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       ResetBridgeDecoder_ShouldReturnError_WhenDecoderIsNull) {
  SetVideoDecoder(nullptr);
  auto result = bridge_->ResetBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       ResetBridgeDecoder_ShouldReturnError_WhenSignalIsNull) {
  auto mock_video_decoder = make_unique<MockMediaCodecDecoderAdapter>();
  SetVideoDecoder(std::move(mock_video_decoder));
  SetSignal(nullptr);
  auto result = bridge_->ResetBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       ResetBridgeDecoder_ShouldReturnError_WhenResetDecoderFails) {
  auto mock_video_decoder = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decoder, ResetDecoder())
      .WillOnce(Return(DecoderAdapterCode::DECODER_ERROR));
  EXPECT_CALL(*mock_video_decoder, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decoder));

  auto result = bridge_->ResetBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       ReleaseBridgeDecoder_ShouldReturnError_WhenDecoderIsNull) {
  SetVideoDecoder(nullptr);
  auto result = bridge_->ReleaseBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       PopInqueueDec_ShouldReturn_WhenSignalIsNull) {
  SetSignal(nullptr);
  PopInqueueDec();
  ASSERT_EQ(GetSignal(), nullptr);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       PopInqueueDec_ShouldReturn_WhenIsOnErrorIsTrue) {
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = true;
  SetSignal(mock_signal);
  PopInqueueDec();
  ASSERT_NE(GetSignal(), nullptr);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       PopInqueueDec_ShouldReturn_WhenInputQueueIsEmpty) {
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  SetSignal(mock_signal);
  PopInqueueDec();
  bool expected_result = true;
  bool actual_result = GetSignal()->inputQueue_.empty();
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest, PushInbufferDec001) {
  auto mock_video_decoder = make_unique<MockMediaCodecDecoderAdapter>();
  uint32_t index = 0;
  uint32_t bufferSize = 1024;
  int64_t time = 100000000;
  EXPECT_CALL(*mock_video_decoder,
              QueueInputBufferDec(index, time, bufferSize, 0,
                                  BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_video_decoder, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decoder));
  auto result = PushInbufferDec(index, bufferSize, time, true);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, PushInbufferDec002) {
  auto mock_video_decoder = make_unique<MockMediaCodecDecoderAdapter>();
  uint32_t index = 0;
  uint32_t bufferSize = 1024;
  int64_t time = 100000000;
  EXPECT_CALL(*mock_video_decoder,
              QueueInputBufferDec(index, time, bufferSize, 0,
                                  BufferFlag::CODEC_BUFFER_FLAG_NONE))
      .WillOnce(Return(DecoderAdapterCode::DECODER_ERROR));
  EXPECT_CALL(*mock_video_decoder, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decoder));
  auto result = PushInbufferDec(index, bufferSize, time, false);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, PushInbufferDecEos001) {
  uint32_t index = 1;
  BufferFlag bufferFlag = BufferFlag::CODEC_BUFFER_FLAG_EOS;
  auto mock_video_decoder = make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decoder,
              QueueInputBufferDec(index, 0, 0, 0, bufferFlag))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_video_decoder, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decoder));
  auto result = PushInbufferDecEos(index);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBuffer_ShouldReturnError_WhenDataIsNull) {
  SetIsRunning(true);
  const uint8_t* data = nullptr;
  size_t data_size = 1;
  int64_t presentation_time = 1000000;
  auto expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto actual_result = bridge_->QueueInputBuffer(
      data, data_size, presentation_time, nullptr, false);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBuffer_ShouldReturnError_WhenDataSizeIsZero) {
  SetIsRunning(true);
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = 0;
  int64_t presentation_time = 1000000;
  auto expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto actual_result = bridge_->QueueInputBuffer(
      data, data_size, presentation_time, nullptr, false);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBuffer_ShouldReturnError_WhenSignalIsNull) {
  SetIsRunning(true);
  SetSignal(nullptr);
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  auto expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto actual_result = bridge_->QueueInputBuffer(
      data, data_size, presentation_time, nullptr, false);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBuffer_ShouldReturnError_WhenIsOnErrorIsTrue) {
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = true;
  SetSignal(mock_signal);
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  auto expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto actual_result = bridge_->QueueInputBuffer(
      data, data_size, presentation_time, nullptr, false);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBuffer_ShouldReturnRetry_WhenIsDecoderFlushingIsTrue) {
  SetIsRunning(true);
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  mock_signal->isDecoderFlushing_.store(true);
  SetSignal(mock_signal);
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;

  auto expected_result = DecoderAdapterCode::DECODER_RETRY;
  auto actual_result = bridge_->QueueInputBuffer(
      data, data_size, presentation_time, nullptr, false);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBuffer_ShouldReturnRetry_WhenInputQueueIsEmpty) {
  SetIsRunning(true);
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  mock_signal->isDecoderFlushing_.store(false);
  SetSignal(mock_signal);
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;

  auto expected_result = DecoderAdapterCode::DECODER_RETRY;
  auto actual_result = bridge_->QueueInputBuffer(
      data, data_size, presentation_time, nullptr, false);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBuffer_ShouldReturnRetry_WhenIsRunningIsFalse) {
  SetIsRunning(false);
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  mock_signal->isDecoderFlushing_.store(false);
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  uint8_t test = 1;
  uint8_t* addr = &test;
  VideoBridgeDecoderInputBuffer buffer = {10, {addr, 10}};
  mock_signal->inputQueue_.push(buffer);
  SetSignal(mock_signal);
  auto expected_result = DecoderAdapterCode::DECODER_RETRY;
  auto actual_result = bridge_->QueueInputBuffer(
      data, data_size, presentation_time, nullptr, false);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBuffer_ShouldReturnError_WhenDecoderIsNull) {
  SetIsRunning(true);
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  mock_signal->isDecoderFlushing_.store(false);
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  uint8_t test = 1;
  uint8_t* addr = &test;
  VideoBridgeDecoderInputBuffer buffer = {10, {addr, 10}};
  mock_signal->inputQueue_.push(buffer);
  SetSignal(mock_signal);
  SetVideoDecoder(nullptr);
  auto expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto actual_result = bridge_->QueueInputBuffer(
      data, data_size, presentation_time, nullptr, false);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBuffer_ShouldReturnOk_WhenPushInbufferDecSucceeds) {
  SetIsRunning(true);
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  mock_signal->isDecoderFlushing_.store(false);
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  auto test = std::make_unique<uint8_t>();
  uint8_t* addr = test.get();
  VideoBridgeDecoderInputBuffer buffer = {10, {addr, 8}};
  mock_signal->inputQueue_.push(buffer);
  SetSignal(mock_signal);
  auto mock_video_decoder = std::make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decoder,
              QueueInputBufferDec(testing::_, testing::_, testing::_, 0,
                                  BufferFlag::CODEC_BUFFER_FLAG_NONE))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*mock_video_decoder, ReleaseDecoder()).Times(1);
  SetVideoDecoder(std::move(mock_video_decoder));

  auto expected_result = DecoderAdapterCode::DECODER_OK;
  auto actual_result = bridge_->QueueInputBuffer(
      data, data_size, presentation_time, nullptr, false);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBufferEOS_ShouldReturnError_WhenSignalIsNull) {
  SetIsRunning(true);
  SetSignal(nullptr);
  auto expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto actual_result = bridge_->QueueInputBufferEOS();
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBufferEOS_ShouldReturnError_WhenIsOnErrorIsTrue) {
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = true;
  SetSignal(mock_signal);
  auto expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto actual_result = bridge_->QueueInputBufferEOS();
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBufferEOS_ShouldReturnRetry_WhenIsDecoderFlushingIsTrue) {
  SetIsRunning(true);
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  mock_signal->isDecoderFlushing_.store(true);
  SetSignal(mock_signal);
  auto expected_result = DecoderAdapterCode::DECODER_RETRY;
  auto actual_result = bridge_->QueueInputBufferEOS();
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBufferEOS_ShouldReturnRetry_WhenInputQueueIsEmpty) {
  SetIsRunning(true);
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  mock_signal->isDecoderFlushing_.store(false);
  SetSignal(mock_signal);

  auto expected_result = DecoderAdapterCode::DECODER_RETRY;
  auto actual_result = bridge_->QueueInputBufferEOS();
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBufferEOS_ShouldReturnRetry_WhenIsRunningIsFalse) {
  SetIsRunning(false);
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  mock_signal->isDecoderFlushing_.store(false);
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  uint8_t test = 1;
  uint8_t* addr = &test;
  VideoBridgeDecoderInputBuffer buffer = {10, {addr, 10}};
  mock_signal->inputQueue_.push(buffer);
  SetSignal(mock_signal);
  auto expected_result = DecoderAdapterCode::DECODER_RETRY;
  auto actual_result = bridge_->QueueInputBufferEOS();
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBufferEOS_ShouldReturnError_WhenDecoderIsNull) {
  SetIsRunning(true);
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  mock_signal->isDecoderFlushing_.store(false);
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  uint8_t test = 1;
  uint8_t* addr = &test;
  VideoBridgeDecoderInputBuffer buffer = {10, {addr, 10}};
  mock_signal->inputQueue_.push(buffer);
  SetSignal(mock_signal);
  SetVideoDecoder(nullptr);
  auto expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto actual_result = bridge_->QueueInputBufferEOS();
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       ReleaseOutputBuffer_ShoulReturnError_WhenDecoderIsNull) {
  SetVideoDecoder(nullptr);
  auto expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto actual_result = bridge_->ReleaseOutputBuffer(0, true);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       PopOutqueueDec_ShoulReturn_WhenSignalIsNull) {
  SetSignal(nullptr);
  auto expected_result = DecoderAdapterCode::DECODER_ERROR;
  PopOutqueueDec();
  auto actual_result = DecoderAdapterCode::DECODER_OK;
  ASSERT_NE(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       DequeueOutputBuffer_ShouldReturnError_WhenSignalIsNull) {
  SetSignal(nullptr);
  base::TimeDelta presentation_time;
  uint32_t index = 1;
  bool eos = true;
  auto expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto actual_result =
      bridge_->DequeueOutputBuffer(&presentation_time, index, eos);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       DequeueOutputBuffer_ShouldReturnError_WhenIsOnErrorIsTrue) {
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = true;
  SetSignal(mock_signal);
  base::TimeDelta presentation_time;
  uint32_t index = 1;
  bool eos = true;
  auto expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto actual_result =
      bridge_->DequeueOutputBuffer(&presentation_time, index, eos);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       DequeueOutputBuffer_ShouldReturnRetry_WhenIsDecoderFlushingIsTrue) {
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  mock_signal->isDecoderFlushing_.store(true);
  SetSignal(mock_signal);
  base::TimeDelta presentation_time;
  uint32_t index = 1;
  bool eos = true;
  auto expected_result = DecoderAdapterCode::DECODER_RETRY;
  auto actual_result =
      bridge_->DequeueOutputBuffer(&presentation_time, index, eos);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       DequeueOutputBuffer_ShouldReturnRetry_WhenOuputQueueIsEmpty) {
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  mock_signal->isDecoderFlushing_.store(false);
  SetSignal(mock_signal);
  base::TimeDelta presentation_time;
  uint32_t index = 1;
  bool eos = true;
  auto expected_result = DecoderAdapterCode::DECODER_RETRY;
  auto actual_result =
      bridge_->DequeueOutputBuffer(&presentation_time, index, eos);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       DequeueOutputBuffer_ShouldReturnError_WhenDecoderIsNull) {
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  mock_signal->isDecoderFlushing_.store(false);
  base::TimeDelta presentation_time;
  uint32_t index = 0;
  bool eos = true;
  BufferFlag outputBufferFlag = BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA;
  BufferInfo outputBufferInfo = {0, 0, 0};
  VideoBridgeDecoderOutputBuffer buffer = {10, outputBufferFlag,
                                           outputBufferInfo};
  mock_signal->outputQueue_.push(buffer);
  SetSignal(mock_signal);
  SetVideoDecoder(nullptr);
  auto expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto actual_result =
      bridge_->DequeueOutputBuffer(&presentation_time, index, eos);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       DequeueOutputBuffer_ShouldReturnOk_WhenPushInbufferDecSucceeds) {
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  mock_signal->isDecoderFlushing_.store(false);
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  base::TimeDelta presentation_time;
  uint32_t index = 0;
  bool eos = true;
  BufferFlag outputBufferFlag = BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA;
  BufferInfo outputBufferInfo = {0, 0, 0};
  VideoBridgeDecoderOutputBuffer buffer = {10, outputBufferFlag,
                                           outputBufferInfo};
  mock_signal->outputQueue_.push(buffer);
  SetSignal(mock_signal);
  auto mock_video_decoder = std::make_unique<MockMediaCodecDecoderAdapter>();
  SetVideoDecoder(std::move(mock_video_decoder));

  auto expected_result = DecoderAdapterCode::DECODER_OK;
  auto actual_result =
      bridge_->DequeueOutputBuffer(&presentation_time, index, eos);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       DestoryNativeWindow_ShouldReturn_WhenWindowIsNull) {
  void* window = nullptr;
  bridge_->DestoryNativeWindow(window);
  ASSERT_FALSE(window);
}

TEST_F(MediaCodecDecoderBridgeImplTest, OnNeedInputData) {
  base::Thread thread("ExampleThread");
  CHECK(thread.Start());
  scoped_refptr<base::SequencedTaskRunner> task_runner = thread.task_runner();
  callback_->decoder_callback_task_runner_ = task_runner;
  callback_->on_buffers_available_cb_ = base::DoNothing();
  uint32_t index = 1;
  auto mock_buffer = make_shared<InheritOhosBufferAdapter>();

  callback_->OnNeedInputData(index, mock_buffer);
  thread.Stop();
  ASSERT_FALSE(signal_->inputQueue_.empty());
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       OnNeedInputData_ShouldReturn_WhenSignalIsNull) {
  base::Thread thread("ExampleThread");
  CHECK(thread.Start());
  scoped_refptr<base::SequencedTaskRunner> task_runner = thread.task_runner();
  callback_->decoder_callback_task_runner_ = task_runner;
  callback_->on_buffers_available_cb_ = base::DoNothing();
  uint32_t index = 1;
  auto mock_buffer = make_shared<NiceMock<OHOS::NWeb::OhosBufferAdapter>>();
  callback_->signal_ = nullptr;

  callback_->OnNeedInputData(index, mock_buffer);
  thread.Stop();
  ASSERT_TRUE(signal_->inputQueue_.empty());
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       OnNeedInputData_ShouldReturn_WhenDecoderFlushingIsTrue) {
  base::Thread thread("ExampleThread");
  CHECK(thread.Start());
  scoped_refptr<base::SequencedTaskRunner> task_runner = thread.task_runner();
  callback_->decoder_callback_task_runner_ = task_runner;
  callback_->on_buffers_available_cb_ = base::DoNothing();
  uint32_t index = 1;
  auto mock_buffer = make_shared<NiceMock<OHOS::NWeb::OhosBufferAdapter>>();
  callback_->signal_->isDecoderFlushing_.store(true);

  callback_->OnNeedInputData(index, mock_buffer);
  thread.Stop();
  ASSERT_TRUE(signal_->inputQueue_.empty());
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       OnNeedInputData_ShouldReturn_WhenBufferIsNull) {
  base::Thread thread("ExampleThread");
  CHECK(thread.Start());
  scoped_refptr<base::SequencedTaskRunner> task_runner = thread.task_runner();
  callback_->decoder_callback_task_runner_ = task_runner;
  callback_->on_buffers_available_cb_ = base::DoNothing();
  uint32_t index = 1;
  auto mock_buffer = nullptr;

  callback_->OnNeedInputData(index, mock_buffer);
  thread.Stop();
  ASSERT_TRUE(signal_->inputQueue_.empty());
}

TEST_F(MediaCodecDecoderBridgeImplTest, OnNeedOutputData) {
  base::Thread thread("ExampleThread");
  CHECK(thread.Start());
  scoped_refptr<base::SequencedTaskRunner> task_runner = thread.task_runner();
  callback_->decoder_callback_task_runner_ = task_runner;
  callback_->on_buffers_available_cb_ = base::DoNothing();
  uint32_t index = 1;
  BufferFlag flag = BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA;
  auto mock_info = make_shared<InheritBufferInfoAdapter>();

  callback_->OnNeedOutputData(index, mock_info, flag);
  thread.Stop();
  ASSERT_FALSE(signal_->outputQueue_.empty());
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       OnNeedOutputData_ShouldReturn_WhenInfoIsNull) {
  base::Thread thread("ExampleThread");
  CHECK(thread.Start());
  scoped_refptr<base::SequencedTaskRunner> task_runner = thread.task_runner();
  callback_->decoder_callback_task_runner_ = task_runner;
  callback_->on_buffers_available_cb_ = base::DoNothing();
  uint32_t index = 1;
  BufferFlag flag = BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA;
  auto mock_info = nullptr;

  callback_->OnNeedOutputData(index, mock_info, flag);
  thread.Stop();
  ASSERT_TRUE(signal_->outputQueue_.empty());
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       OnNeedOutputData_ShouldReturn_WhenSignalIsNull) {
  base::Thread thread("ExampleThread");
  CHECK(thread.Start());
  scoped_refptr<base::SequencedTaskRunner> task_runner = thread.task_runner();
  callback_->decoder_callback_task_runner_ = task_runner;
  callback_->on_buffers_available_cb_ = base::DoNothing();
  uint32_t index = 1;
  BufferFlag flag = BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA;
  auto mock_info = make_shared<InheritBufferInfoAdapter>();
  callback_->signal_ = nullptr;

  callback_->OnNeedOutputData(index, mock_info, flag);
  thread.Stop();
  ASSERT_TRUE(signal_->outputQueue_.empty());
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       OnNeedOutputData_ShouldReturn_WhenDecoderFlushingIsTrue) {
  base::Thread thread("ExampleThread");
  CHECK(thread.Start());
  scoped_refptr<base::SequencedTaskRunner> task_runner = thread.task_runner();
  callback_->decoder_callback_task_runner_ = task_runner;
  callback_->on_buffers_available_cb_ = base::DoNothing();
  uint32_t index = 1;
  BufferFlag flag = BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA;
  auto mock_info = make_shared<InheritBufferInfoAdapter>();

  callback_->signal_->isDecoderFlushing_.store(true);

  callback_->OnNeedOutputData(index, mock_info, flag);
  thread.Stop();
  ASSERT_TRUE(signal_->outputQueue_.empty());
}

TEST_F(MediaCodecDecoderBridgeImplTest, PopInqueueDec_ShouldNotReturn) {
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  VideoBridgeDecoderInputBuffer inputBuffer;
  mock_signal->inputQueue_.push(inputBuffer);
  SetSignal(mock_signal);
  PopInqueueDec();
  bool expected_result = true;
  bool actual_result = GetSignal()->inputQueue_.empty();
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestSetAVCencInfo001) {
  SetVideoDecoder(nullptr);
  auto result = bridge_->SetAVCencInfo(0, nullptr);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestSetAVCencInfo002) {
  auto result = bridge_->SetAVCencInfo(0, nullptr);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestSetAVCencInfo003) {
  auto result = bridge_->SetAVCencInfo(0, nullptr);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestConstruct001) {
  testing::internal::CaptureStderr();
  std::make_unique<MediaCodecDecoderBridgeImpl>("test", base::DoNothing());
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("create decoder failed"), std::string::npos);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestConstruct002) {
  testing::internal::CaptureStderr();
  std::make_unique<MediaCodecDecoderBridgeImpl>("test");
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("create decoder failed"), std::string::npos);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestConstruct003) {
  testing::internal::CaptureStderr();
  std::make_unique<MediaCodecDecoderBridgeImpl>("video/h264");
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("create decoder failed"), std::string::npos);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       QueueInputBuffer_ShouldReturnError_WhenAddrIsNull) {
  SetIsRunning(true);
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  mock_signal->isDecoderFlushing_.store(false);
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  auto test = std::make_unique<uint8_t>();
  uint8_t* addr = nullptr;
  VideoBridgeDecoderInputBuffer buffer = {10, {addr, 8}};
  mock_signal->inputQueue_.push(buffer);
  SetSignal(mock_signal);
  auto mock_video_decoder = std::make_unique<MockMediaCodecDecoderAdapter>();
  SetVideoDecoder(std::move(mock_video_decoder));

  auto expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto actual_result = bridge_->QueueInputBuffer(
      data, data_size, presentation_time, nullptr, true);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       ReleaseOutputBuffer_ShoulReturnOk_WhenDecoderIsNotNull) {
  auto mock_video_decoder = std::make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decoder,
              ReleaseOutputBufferDec(testing::_, testing::_))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  SetVideoDecoder(std::move(mock_video_decoder));
  auto expected_result = DecoderAdapterCode::DECODER_OK;
  auto actual_result = bridge_->ReleaseOutputBuffer(0, true);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest,
       DequeueOutputBuffer_ShouldReturnOk_WhenPushInbufferDecSucceeds2) {
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  mock_signal->isOnError_ = false;
  mock_signal->isDecoderFlushing_.store(false);
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  base::TimeDelta presentation_time;
  uint32_t index = 0;
  bool eos = true;
  BufferFlag outputBufferFlag = BufferFlag::CODEC_BUFFER_FLAG_EOS;
  BufferInfo outputBufferInfo = {0, 0, 0};
  VideoBridgeDecoderOutputBuffer buffer = {10, outputBufferFlag,
                                           outputBufferInfo};
  mock_signal->outputQueue_.push(buffer);
  SetSignal(mock_signal);
  auto mock_video_decoder = std::make_unique<MockMediaCodecDecoderAdapter>();
  SetVideoDecoder(std::move(mock_video_decoder));

  auto expected_result = DecoderAdapterCode::DECODER_OK;
  auto actual_result =
      bridge_->DequeueOutputBuffer(&presentation_time, index, eos);
  ASSERT_EQ(expected_result, actual_result);
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
TEST_F(MediaCodecDecoderBridgeImplTest, TestSetVideoSurface001) {
  DecoderAdapterCode expected_result = DecoderAdapterCode::DECODER_OK;
  DecoderAdapterCode actual_result = bridge_->SetVideoSurface(-1);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestSetVideoSurface002) {
  DecoderAdapterCode expected_result = DecoderAdapterCode::DECODER_ERROR;
  SetVideoDecoder(nullptr);
  DecoderAdapterCode actual_result = bridge_->SetVideoSurface(0);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestSetVideoSurface003) {
  DecoderAdapterCode expected_result = DecoderAdapterCode::DECODER_OK;
  SetVideoSurfaceId(0);
  int32_t value = 1;
  intptr_t int_as_ptr = reinterpret_cast<intptr_t>(&value);
  void* window = reinterpret_cast<void*>(int_as_ptr);
  SetWindowFromSurface(window);
  auto mock_video_decoder = std::make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decoder, SetOutputSurface(testing::_))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  SetVideoDecoder(std::move(mock_video_decoder));
  DecoderAdapterCode actual_result = bridge_->SetVideoSurface(-1);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestSetVideoSurface004) {
  DecoderAdapterCode expected_result = DecoderAdapterCode::DECODER_ERROR;
  SetVideoSurfaceId(0);
  auto mock_video_decoder = std::make_unique<MockMediaCodecDecoderAdapter>();
  SetVideoDecoder(std::move(mock_video_decoder));
  DecoderAdapterCode actual_result = bridge_->SetVideoSurface(-1);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestSetVideoSurface005) {
  DecoderAdapterCode expected_result = DecoderAdapterCode::DECODER_OK;
  auto mock_video_decoder = std::make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decoder, SetOutputSurface(testing::_))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  SetVideoDecoder(std::move(mock_video_decoder));
  DecoderAdapterCode actual_result = bridge_->SetVideoSurface(0);
  ASSERT_EQ(expected_result, actual_result);
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
TEST_F(MediaCodecDecoderBridgeImplTest, TestRecycleDmaBuffer001) {
  DecoderAdapterCode expected_result = DecoderAdapterCode::DECODER_ERROR;
  SetVideoDecoder(nullptr);
  DecoderAdapterCode actual_result = bridge_->RecycleDmaBuffer();
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestRecycleDmaBuffer002) {
  DecoderAdapterCode expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto mock_video_decoder = std::make_unique<MockMediaCodecDecoderAdapter>();
  SetVideoDecoder(std::move(mock_video_decoder));
  SetSignal(nullptr);
  DecoderAdapterCode actual_result = bridge_->RecycleDmaBuffer();
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestRecycleDmaBuffer003) {
  DecoderAdapterCode expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto mock_video_decoder = std::make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decoder, StopDecoder())
      .WillOnce(Return(DecoderAdapterCode::DECODER_ERROR));
  SetVideoDecoder(std::move(mock_video_decoder));
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  SetSignal(mock_signal);
  DecoderAdapterCode actual_result = bridge_->RecycleDmaBuffer();
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestRecycleDmaBuffer004) {
  DecoderAdapterCode expected_result = DecoderAdapterCode::DECODER_OK;
  auto mock_video_decoder = std::make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decoder, StopDecoder())
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  SetVideoDecoder(std::move(mock_video_decoder));
  auto mock_signal = make_shared<NiceMock<MockDecoderBridgeSignal>>();
  SetSignal(mock_signal);
  DecoderAdapterCode actual_result = bridge_->RecycleDmaBuffer();
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestResumeDmaBuffer001) {
  DecoderAdapterCode expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto mock_video_decoder = std::make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decoder, StartDecoder())
      .WillOnce(Return(DecoderAdapterCode::DECODER_ERROR));
  SetVideoDecoder(std::move(mock_video_decoder));
  DecoderAdapterCode actual_result = bridge_->ResumeDmaBuffer();
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestResumeDmaBuffer002) {
  DecoderAdapterCode expected_result = DecoderAdapterCode::DECODER_OK;
  auto mock_video_decoder = std::make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decoder, StartDecoder())
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  SetVideoDecoder(std::move(mock_video_decoder));
  DecoderAdapterCode actual_result = bridge_->ResumeDmaBuffer();
  ASSERT_EQ(expected_result, actual_result);
}
#endif

TEST_F(MediaCodecDecoderBridgeImplTest, TestSetDecryptionConfig001) {
  DecoderAdapterCode expected_result = DecoderAdapterCode::DECODER_ERROR;
  auto mock_video_decoder = std::make_unique<MockMediaCodecDecoderAdapter>();
  SetVideoDecoder(nullptr);
  DecoderAdapterCode actual_result =
      bridge_->SetDecryptionConfig(nullptr, true);
  ASSERT_EQ(expected_result, actual_result);
}

TEST_F(MediaCodecDecoderBridgeImplTest, TestSetDecryptionConfig002) {
  DecoderAdapterCode expected_result = DecoderAdapterCode::DECODER_OK;
  auto mock_video_decoder = std::make_unique<MockMediaCodecDecoderAdapter>();
  EXPECT_CALL(*mock_video_decoder, SetDecryptionConfig(testing::_, testing::_))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  SetVideoDecoder(std::move(mock_video_decoder));
  DecoderAdapterCode actual_result =
      bridge_->SetDecryptionConfig(nullptr, true);
  ASSERT_EQ(expected_result, actual_result);
}
}  // namespace media