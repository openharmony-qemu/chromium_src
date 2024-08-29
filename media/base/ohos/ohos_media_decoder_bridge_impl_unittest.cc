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

#include "gtest/gtest.h"
#define private public
#include "ohos_media_decoder_bridge_impl.h"
#undef private
#include "ohos_adapter_helper.h" 
#include <gmock/gmock.h>
#include "testing/gtest/include/gtest/gtest.h"
#include <memory>
#include "base/logging.h"
#include "base/task/task_runner.h"
#include "base/trace_event/trace_event.h"
#include "base/task/single_thread_task_executor.h"
#include "media/base/ohos/decoder_format_adapter_impl.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/adapter_base.h"

using ::testing::_;
using ::testing::Eq;
using ::testing::Ref;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::AtLeast;
using ::testing::NiceMock;
using ::testing::MockFunction;
using namespace OHOS::NWeb;
using namespace std;
const std::string PRODUCT_MODEL = "noemulator"; 

namespace testing {

template <>
class NiceMock<OHOS::NWeb::OhosBufferAdapter> : public OHOS::NWeb::OhosBufferAdapter {
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
  MOCK_METHOD(bool, GetResourceUseHapPathEnable, (), ());
  MOCK_METHOD(std::string, GetDeviceInfoProductModel, (), ());
  MOCK_METHOD(std::string, GetDeviceInfoBrand, (), ());
  MOCK_METHOD(int32_t, GetDeviceInfoMajorVersion, (), ());
  MOCK_METHOD(ProductDeviceType, GetProductDeviceType, (), ());
  MOCK_METHOD(bool, GetWebOptimizationValue, (), ());
  MOCK_METHOD(bool, IsAdvancedSecurityMode, (), ());
  MOCK_METHOD(std::string, GetUserAgentOSName, (), ());
  MOCK_METHOD(int32_t, GetSoftwareMajorVersion, (), ());
  MOCK_METHOD(int32_t, GetSoftwareSeniorVersion, (), ());
  MOCK_METHOD(std::string, GetNetlogMode, (), ());
  MOCK_METHOD(bool, GetTraceDebugEnable, (), ());
  MOCK_METHOD(std::string, GetSiteIsolationMode, (), ());
  MOCK_METHOD(int32_t, GetFlowBufMaxFd, (), ());
  MOCK_METHOD(bool, GetOOPGPUEnable, (), ());
  MOCK_METHOD(void, SetOOPGPUDisable, (), (override));
  MOCK_METHOD(void, AttachSysPropObserver, (PropertiesKey key, SystemPropertiesObserver* observer), (override));
  MOCK_METHOD(void, DetachSysPropObserver, (PropertiesKey key, SystemPropertiesObserver* observer), (override));
  MOCK_METHOD(bool, GetBoolParameter, (const std::string& key, bool defaultValue), (override));
  MOCK_METHOD(std::vector<FrameRateSetting>, GetLTPOConfig, (const std::string& settingName), (override));
  MOCK_METHOD(std::string, GetOOPGPUStatus, (), ());
  MOCK_METHOD(bool, IsLTPODynamicApp, (const std::string& bundleName), (override));
};

class MockWindowAdapter : public WindowAdapter {
public:
  MOCK_METHOD(NWebNativeWindow, CreateNativeWindowFromSurface, (void* pSurface), (override));
  MOCK_METHOD(void, DestroyNativeWindow, (NWebNativeWindow window), ());
  MOCK_METHOD(int32_t, NativeWindowSetBufferGeometry, (NWebNativeWindow window, int32_t width, int32_t height), (override));
  MOCK_METHOD(void, NativeWindowSurfaceCleanCache, (NWebNativeWindow window), (override));
  MOCK_METHOD(void, NativeWindowSurfaceCleanCacheWithPara, (NWebNativeWindow window, bool cleanAll), (override));
};

class MockMediaCodecDecoderAdapter : public MediaCodecDecoderAdapter {
public:
  MOCK_METHOD(DecoderAdapterCode, CreateVideoDecoderByMime, (const std::string&), (override));
  MOCK_METHOD(DecoderAdapterCode, CreateVideoDecoderByName, (const std::string&), (override));
  MOCK_METHOD(DecoderAdapterCode, ConfigureDecoder, (std::shared_ptr<DecoderFormatAdapter>), (override));
  MOCK_METHOD(DecoderAdapterCode, SetParameterDecoder, (std::shared_ptr<DecoderFormatAdapter>), (override));
  MOCK_METHOD(DecoderAdapterCode, SetOutputSurface, (void* window), (override));
  MOCK_METHOD(DecoderAdapterCode, PrepareDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, StartDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, StopDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, FlushDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, ResetDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, ReleaseDecoder, (), (override));
  MOCK_METHOD(DecoderAdapterCode, QueueInputBufferDec, (uint32_t, int64_t, int32_t, int32_t, BufferFlag), (override));
  MOCK_METHOD(DecoderAdapterCode, GetOutputFormatDec, (std::shared_ptr<DecoderFormatAdapter> format), (override));
  MOCK_METHOD(DecoderAdapterCode, ReleaseOutputBufferDec, (uint32_t, bool), (override));
  MOCK_METHOD(DecoderAdapterCode, SetCallbackDec, (std::shared_ptr<DecoderCallbackAdapter>), (override));
};

class MockOhosAdapterHelper : public OhosAdapterHelper {
public:
  MOCK_METHOD(std::unique_ptr<AafwkAppMgrClientAdapter>, CreateAafwkAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<PowerMgrClientAdapter>, CreatePowerMgrClientAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<DisplayManagerAdapter>, CreateDisplayMgrAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<BatteryMgrClientAdapter>, CreateBatteryClientAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<NetConnectAdapter>, CreateNetConnectAdapter, (), (override));
  MOCK_METHOD(OhosWebDataBaseAdapter&, GetOhosWebDataBaseAdapterInstance, (), (override));
  MOCK_METHOD(PasteBoardClientAdapter&, GetPasteBoard, (), (override));
  MOCK_METHOD(std::unique_ptr<AudioRendererAdapter>, CreateAudioRendererAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<AudioCapturerAdapter>, CreateAudioCapturerAdapter, (), (override));
  MOCK_METHOD(AudioSystemManagerAdapter&, GetAudioSystemManager, (), (override));
  MOCK_METHOD(OhosWebPermissionDataBaseAdapter&, GetWebPermissionDataBaseInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<MMIAdapter>, CreateMMIAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<SocPerfClientAdapter>, CreateSocPerfClientAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<OhosResourceAdapter>, GetResourceAdapter, (const std::string&), (override));
  MOCK_METHOD(SystemPropertiesAdapter&, GetSystemPropertiesInstance, (), (override));
  MOCK_METHOD(VSyncAdapter&, GetVSyncAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<OhosInitWebAdapter>, GetInitWebAdapter, (), (override));
  MOCK_METHOD(KeystoreAdapter&, GetKeystoreAdapterInstance, (), (override));
  MOCK_METHOD(EnterpriseDeviceManagementAdapter&, GetEnterpriseDeviceManagementInstance, (), (override));
  MOCK_METHOD(DatashareAdapter&, GetDatashareInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<IMFAdapter>, CreateIMFAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<CertManagerAdapter>, GetRootCertDataAdapter, (), (override));
  MOCK_METHOD(AccessTokenAdapter&, GetAccessTokenAdapterInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<EventHandlerAdapter>, GetEventHandlerAdapter, (), (override));
  MOCK_METHOD(PrintManagerAdapter&, GetPrintManagerInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<IConsumerSurfaceAdapter>, CreateConsumerSurfaceAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<PlayerAdapter>, CreatePlayerAdapter, (), (override));
  MOCK_METHOD(WindowAdapter&, GetWindowAdapterInstance, (), (override));
  MOCK_METHOD(HiSysEventAdapter&, GetHiSysEventAdapterInstance, (), (override));
  MOCK_METHOD(HiTraceAdapter&, GetHiTraceAdapterInstance, (), (override));
  MOCK_METHOD(NetProxyAdapter&, GetNetProxyInstance, (), (override));
  MOCK_METHOD(CameraManagerAdapter&, GetCameraManagerAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<ScreenCaptureAdapter>, CreateScreenCaptureAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<DateTimeFormatAdapter>, CreateDateTimeFormatAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<MediaCodecDecoderAdapter>, CreateMediaCodecDecoderAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<NativeImageAdapter>, CreateNativeImageAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<MediaCodecAdapter>, CreateMediaCodecEncoderAdapter, (), (override));
  MOCK_METHOD(MediaCodecListAdapter&, GetMediaCodecListAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<FlowbufferAdapter>, CreateFlowbufferAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<MediaAVSessionAdapter>, CreateMediaAVSessionAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<OhosImageDecoderAdapter>, CreateOhosImageDecoderAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<SensorAdapter>, CreateSensorAdapter, (), (override));
  MOCK_METHOD(void, SetArkWebCoreHapPathOverride, (const std::string&), (override));

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
  MOCK_METHOD(void, swap_inputQueue_, (std::queue<VideoBridgeDecoderInputBuffer>&), ());
  MOCK_METHOD(void, swap_outputQueue_, (std::queue<VideoBridgeDecoderOutputBuffer>&), ());
  MOCK_METHOD(bool, isDecoderFlushing_, (), ());
};

class MockSequencedTaskRunner : public base::SequencedTaskRunner {
public:
  MOCK_METHOD(bool, PostNonNestableTask, (const base::Location&, base::OnceClosure), ());
  MOCK_METHOD(bool, PostNonNestableDelayedTask, (const base::Location&, base::OnceClosure, base::TimeDelta), (override));
  MOCK_METHOD(base::DelayedTaskHandle, PostCancelableDelayedTask, (base::subtle::PostDelayedTaskPassKey, const base::Location&, base::OnceClosure, base::TimeDelta), (override));
  MOCK_METHOD(base::DelayedTaskHandle, PostCancelableDelayedTaskAt, (base::subtle::PostDelayedTaskPassKey, const base::Location&, base::OnceClosure, base::TimeTicks, base::subtle::DelayPolicy), (override));
  MOCK_METHOD(bool, PostDelayedTaskAt, (base::subtle::PostDelayedTaskPassKey, const base::Location&, base::OnceClosure, base::TimeTicks, base::subtle::DelayPolicy), (override));
  MOCK_METHOD(bool, RunsTasksInCurrentSequence, (), (const, override));
  MOCK_METHOD(bool, DeleteOrReleaseSoonInternal, (const base::Location&, void (*)(const void*), const void*), (override));
};

class MockDecoderFormatAdapter : public DecoderFormatAdapter {
public:
  MOCK_METHOD(int32_t, GetWidth, (), (override));
  MOCK_METHOD(int32_t, GetHeight, (), (override));
  MOCK_METHOD(double, GetFrameRate, (), (override));
  MOCK_METHOD(void, SetWidth, (int32_t width), (override));
  MOCK_METHOD(void, SetHeight, (int32_t height), (override));
  MOCK_METHOD(void, SetFrameRate, (double frameRate), (override));
};

class MediaCodecDecoderBridgeImplTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock_adapter_ = std::make_unique<NiceMock<MockMediaCodecDecoderAdapter>>();
    signal_ = std::make_shared<NiceMock<MockDecoderBridgeSignal>>();
    ohos_adapter_helper_mock_ = std::make_unique<NiceMock<MockOhosAdapterHelper>>();
    window_adapter_mock_ = std::make_unique<NiceMock<MockWindowAdapter>>();
    ON_CALL(*ohos_adapter_helper_mock_, GetWindowAdapterInstance())
        .WillByDefault(ReturnRef(*window_adapter_mock_));
    
    bridge_ = std::make_unique<MediaCodecDecoderBridgeImpl>("video/avc");
    bridge_->videoDecoder_ = std::move(mock_adapter_);
    bridge_->signal_ = signal_;
    callback_ = std::make_shared<CodecBridgeCallback>(signal_);

    ohos_adapter_helper_mocks_ = std::make_shared<NiceMock<MockOhosAdapterHelper>>();
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

TEST_F(MediaCodecDecoderBridgeImplTest, CreateVideoDecoder) {
  VideoBridgeCodecConfig config;
  config.codec = media::VideoCodec::kH264;
  config.on_buffers_available_cb = base::DoNothing();
  auto bridge_impl = MediaCodecDecoderBridgeImpl::CreateVideoDecoder(config);
  ASSERT_NE(bridge_impl, nullptr);
}

TEST_F(MediaCodecDecoderBridgeImplTest, CreateVideoBridgeDecoderByMime) {
  mock_adapter_ = std::make_unique<NiceMock<MockMediaCodecDecoderAdapter>>();
  bridge_ = std::make_unique<MediaCodecDecoderBridgeImpl>("video/avc");
  std::string codec_name = "video/h264";
  DecoderAdapterCode result = bridge_->CreateVideoBridgeDecoderByMime(codec_name);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, CreateVideoBridgeDecoderByName) {
  mock_adapter_ = std::make_unique<NiceMock<MockMediaCodecDecoderAdapter>>();
  bridge_ = std::make_unique<MediaCodecDecoderBridgeImpl>("video/avc");
  std::string codec_name = "video/h264";
  DecoderAdapterCode result = bridge_->CreateVideoBridgeDecoderByName(codec_name);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, Constructor) {
  auto ohos_adapter_helper_mock = std::make_shared<NiceMock<MockOhosAdapterHelper>>();
  mock_adapter_ = std::make_unique<NiceMock<MockMediaCodecDecoderAdapter>>();
  base::RepeatingClosure on_buffers_available_cb = base::DoNothing();
  ASSERT_FALSE(bridge_->CheckHasCreated());
}

TEST_F(MediaCodecDecoderBridgeImplTest, SetBridgeParameterDecoder) {
  auto format_adapter_mock = std::make_shared<NiceMock<MockDecoderFormatAdapter>>();
  DecoderFormat format;
  format.width = 640;
  format.height = 480;
  format.frameRate = 30.0;
  DecoderAdapterCode result = bridge_->SetBridgeParameterDecoder(format);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
  ASSERT_EQ(format.width, 640);
  ASSERT_EQ(format.height, 480);
  ASSERT_DOUBLE_EQ(format.frameRate, 30.0);
}

TEST_F(MediaCodecDecoderBridgeImplTest, SetBridgeOutputSurface) {
  void* window = nullptr;
  DecoderAdapterCode result = bridge_->SetBridgeOutputSurface(window);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, GetOutputFormatBridgeDecoder) {
  DecoderFormat format;
  format.width = 0;
  format.height = 0;
  format.frameRate = 0.0;

  DecoderAdapterCode result = bridge_->GetOutputFormatBridgeDecoder(format);

  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
  ASSERT_EQ(format.width, 0);
  ASSERT_EQ(format.height, 0);
  ASSERT_DOUBLE_EQ(format.frameRate, 0.0);
}

TEST_F(MediaCodecDecoderBridgeImplTest, PrepareBridgeDecoder) {
  bridge_->videoDecoder_ = nullptr;
  DecoderAdapterCode result = bridge_->PrepareBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, StartBridgeDecoder) {
  bridge_->videoDecoder_ = nullptr;
  DecoderAdapterCode result = bridge_->StartBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, StopBridgeDecoder) {
  bridge_->videoDecoder_ = nullptr;
  DecoderAdapterCode result = bridge_->StopBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, FlushBridgeDecoderReturns) {
  bridge_->videoDecoder_ = nullptr;
  auto result = bridge_->FlushBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, ResetBridgeDecoderReturns) {
  bridge_->videoDecoder_ = nullptr;
  auto result = bridge_->ResetBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, ReleaseBridgeDecoderReturns) {
  bridge_->videoDecoder_ = nullptr;
  auto result = bridge_->ReleaseBridgeDecoder();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_ERROR);
}

TEST_F(MediaCodecDecoderBridgeImplTest, PopInqueueDec) {
  bridge_->signal_ = nullptr;
  bridge_->PopInqueueDec();
}

TEST_F(MediaCodecDecoderBridgeImplTest, PushInbufferDec) {
  auto video_decoder_s = new testing::NiceMock<MockMediaCodecDecoderAdapter>;   
  uint32_t index = 0;
  uint32_t bufferSize = 1024;
  int64_t time = 100000000;
  EXPECT_CALL(*video_decoder_s, QueueInputBufferDec(index, time, bufferSize, 0, BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));

  auto result = bridge_->PushInbufferDec(index, bufferSize, time);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, PushInbufferDecEos) {
  uint32_t index = 1;
  BufferFlag bufferFlag = BufferFlag::CODEC_BUFFER_FLAG_EOS;
  auto mock_adapter_s = new testing::NiceMock<MockMediaCodecDecoderAdapter>;
  EXPECT_CALL(*mock_adapter_s, QueueInputBufferDec(index, 0, 0, 0, bufferFlag))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));

  auto result = bridge_->PushInbufferDecEos(index);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBuffer) {
  const uint8_t* data = reinterpret_cast<const uint8_t*>("testdata");
  size_t data_size = strlen(reinterpret_cast<const char*>(data));
  int64_t presentation_time = 1000000;
  uint32_t index = 0;
  uint32_t bufferSize = 100;
  auto signal_s = new testing::NiceMock<MockDecoderBridgeSignal>;
  auto mock_adapter_s = new testing::NiceMock<MockMediaCodecDecoderAdapter>;
  EXPECT_CALL(*signal_s, isOnError).WillOnce(Return(false));
  EXPECT_CALL(*signal_s, isDecoderFlushing).WillOnce(Return(false));
  EXPECT_CALL(*signal_s, size).WillOnce(Return(1));
  EXPECT_CALL(*signal_s, front_inputBufferIndex).WillOnce(Return(index));
  EXPECT_CALL(*signal_s, front_inputBuffer).WillOnce(Return(OhosBuffer{nullptr, bufferSize}));
  EXPECT_CALL(*mock_adapter_s, QueueInputBufferDec(index, presentation_time, data_size, 0, BufferFlag::CODEC_BUFFER_FLAG_NONE))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  EXPECT_CALL(*signal_s, pop).Times(1);

  auto result = bridge_->QueueInputBuffer(data, data_size, presentation_time);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_RETRY);
}

TEST_F(MediaCodecDecoderBridgeImplTest, QueueInputBufferEOS) {
  uint32_t index = 0;
  auto signal_s = new testing::NiceMock<MockDecoderBridgeSignal>;
  auto mock_adapter_s = new testing::NiceMock<MockMediaCodecDecoderAdapter>;
  EXPECT_CALL(*signal_s, isOnError).WillOnce(Return(false));
  EXPECT_CALL(*signal_s, isDecoderFlushing).WillOnce(Return(false));
  EXPECT_CALL(*signal_s, empty).WillOnce(Return(false));
  EXPECT_CALL(*signal_s, front_inputBufferIndex).WillOnce(Return(index));
  EXPECT_CALL(*mock_adapter_s, QueueInputBufferDec(index, 0, 0, 0, BufferFlag::CODEC_BUFFER_FLAG_EOS))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));

  auto result = bridge_->QueueInputBufferEOS();
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_RETRY);
}

TEST_F(MediaCodecDecoderBridgeImplTest, ReleaseOutputBuffer) {
  uint32_t index = 0; 
  bool render = true; 
  auto mock_adapter_s = new testing::NiceMock<MockMediaCodecDecoderAdapter>;
  EXPECT_CALL(*mock_adapter_s, ReleaseOutputBufferDec(index, render))
      .WillOnce(Return(DecoderAdapterCode::DECODER_OK));
  auto result = bridge_->ReleaseOutputBuffer(index, render);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_OK);
}

TEST_F(MediaCodecDecoderBridgeImplTest, PopOutqueueDec) {
  bridge_->signal_ = nullptr;
  bridge_->PopOutqueueDec();
}

TEST_F(MediaCodecDecoderBridgeImplTest, DequeueOutputBuffer) {
  base::TimeDelta presentation_time;
  uint32_t index = 1;
  bool eos;
  auto signal_s = new testing::NiceMock<MockDecoderBridgeSignal>;
  EXPECT_CALL(*signal_s, isOnError).WillOnce(Return(false));
  EXPECT_CALL(*signal_s, isDecoderFlushing).WillOnce(Return(false));
  EXPECT_CALL(*signal_s, empty).WillOnce(Return(false));
  EXPECT_CALL(*signal_s, front_outputBufferIndex).WillOnce(Return(1));
  EXPECT_CALL(*signal_s, front_outputBufferFlag).WillOnce(Return(BufferFlag::CODEC_BUFFER_FLAG_NONE));
  EXPECT_CALL(*signal_s, front_outputBufferInfo_presentationTimeUs).WillOnce(Return(1000000)); 
  EXPECT_CALL(*signal_s, pop).Times(1);
  auto result = bridge_->DequeueOutputBuffer(&presentation_time, index, eos);
  ASSERT_EQ(result, DecoderAdapterCode::DECODER_RETRY);
}

TEST_F(MediaCodecDecoderBridgeImplTest, DestoryNativeWindow) {
  void* window = nullptr;
  bridge_->DestoryNativeWindow(window);
}

TEST_F(MediaCodecDecoderBridgeImplTest, OnError) {
  ErrorType errorType = ErrorType::CODEC_ERROR_INTERNAL;
  int32_t errorCode = 123;
  bridge_->signal_ = nullptr;
  callback_->OnError(errorType, errorCode);
}

TEST_F(MediaCodecDecoderBridgeImplTest, OnStreamChanged) {
  int32_t width = 1280;
  int32_t height = 720;
  double frameRate = 30.0;
  callback_ = std::make_shared<CodecBridgeCallback>(signal_);
  callback_->OnStreamChanged(width, height, frameRate);
}
}