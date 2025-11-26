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
#include "base/memory/weak_ptr.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "base_window_capturer.h"
#include "base/task/single_thread_task_runner.h"
#undef private
#include "base/logging.h"
#include "base/task/single_thread_task_executor.h"
#include "base/task/task_runner.h"
#include "ohos_adapter_helper.h"
#include "audio_capturer_adapter.h"
#include "base/memory/scoped_refptr.h"
#include "base/run_loop.h"
#include "base/task/sequenced_task_runner.h"
#include "base/test/gtest_util.h"
#include "base/test/task_environment.h"
#include "base/test/test_simple_task_runner.h"
#include "media/audio/ohos/ohos_audio_capturer_source.h"

using namespace media;
using namespace base;
using namespace testing;

namespace webrtc {
class MockReadDataCallback {
 public:
  MOCK_METHOD(void, Run, (), ());
};

class WindowCapturerReadCallbackTest : public ::testing::Test {
 public:
  WindowCapturerReadCallbackTest()
      : mock_callback(),
        read_callback(base::BindRepeating(&MockReadDataCallback::Run,
                                          base::Unretained(&mock_callback))) {}

 protected:
  MockReadDataCallback mock_callback;
  WindowCapturerReadCallback read_callback;
};

TEST_F(WindowCapturerReadCallbackTest, OnReadDataTest) {
  bool result = read_callback.readDataCallback_.is_null();
  EXPECT_CALL(mock_callback, Run()).Times(1);
  read_callback.OnReadData();
  EXPECT_FALSE(result);
}

TEST_F(WindowCapturerReadCallbackTest, OnReadDataNoCallbackTest) {
  read_callback.readDataCallback_ = base::RepeatingCallback<void()>();
  bool result = read_callback.readDataCallback_.is_null();
  EXPECT_CALL(mock_callback, Run()).Times(0);
  read_callback = WindowCapturerReadCallback(base::RepeatingCallback<void()>());
  read_callback.OnReadData();
  EXPECT_TRUE(result);
}

class ScreenCaptureAdapterMock : public ScreenCaptureAdapter {
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

class MockSurfaceBufferAdapter : public SurfaceBufferAdapter {
public:
  MOCK_METHOD(int32_t, GetFileDescriptor, (), (override));
  MOCK_METHOD(int32_t, GetWidth, (), (override));
  MOCK_METHOD(int32_t, GetHeight, (), (override));
  MOCK_METHOD(int32_t, GetStride, (), (override));
  MOCK_METHOD(int32_t, GetFormat, (), (override));
  MOCK_METHOD(uint32_t, GetSize, (), (override));
  MOCK_METHOD(void*, GetVirAddr, (), (override));
};

class MockCallback : public BaseWindowCapturer::Callback {
public:
  MOCK_METHOD(void, OnFrameCaptureStart, (), (override));
  MOCK_METHOD(void, OnCaptureResult, 
              (DesktopCapturer::Result result, std::unique_ptr<DesktopFrame> frame), 
              (override));
};

class BaseWindowCapturerTest : public ::testing::Test {
 public:
  void SetUp() override {
    scoped_refptr<SingleThreadTaskRunner> task_runner_1(
      MakeRefCounted<TestSimpleTaskRunner>().get());
    SingleThreadTaskRunner::CurrentDefaultHandle sttcd1(task_runner_1.get(),
      SingleThreadTaskRunner::CurrentDefaultHandle::MayAlreadyExist{});

    BaseWindowCapturer::CaptureSourceType source_type = BaseWindowCapturer::CaptureSourceType::kScreen;
    bool is_picker_show = true;
    int nweb_id = 1;
    base_window_capturer = std::make_unique<BaseWindowCapturer>(source_type, is_picker_show, nweb_id);
  }
  
  void TearDown() override {
    base_window_capturer.reset();
    int nweb_id = 1;
    auto& capture_source = BaseScreenCaptureSource::GetInstance();
    auto it = capture_source.screen_capture_adapter_map_.find(nweb_id);
    if (it != capture_source.screen_capture_adapter_map_.end()) {
      capture_source.screen_capture_adapter_map_.erase(it);
    }
  }

 protected:
  std::unique_ptr<BaseWindowCapturer> base_window_capturer;
};

TEST_F(BaseWindowCapturerTest, HandleBuffer01) {
  std::unique_ptr<OHOS::NWeb::ScreenCaptureAdapter> screen_capture_adapter = 
    std::make_unique<ScreenCaptureAdapterMock>();
  int nweb_id = 1;
  BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id] = 
    std::move(screen_capture_adapter);
  base_window_capturer->portal_init_failed_ = true;

  base_window_capturer->HandleBuffer();

  EXPECT_NE(BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id], nullptr);
}

TEST_F(BaseWindowCapturerTest, HandleBuffer02) {
  int nweb_id = 1;
  base_window_capturer->portal_init_failed_ = false;
  BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_.erase(nweb_id);

  base_window_capturer->HandleBuffer();

  EXPECT_FALSE(BaseScreenCaptureSource::GetInstance().ScreenCaptureAdapterIsExist(nweb_id));
}

TEST_F(BaseWindowCapturerTest, HandleBuffer03) {
  auto mock_adapter = std::make_unique<ScreenCaptureAdapterMock>();
  ScreenCaptureAdapterMock* raw_mock = mock_adapter.get();
  int nweb_id = 1;

  EXPECT_CALL(*raw_mock, AcquireVideoBuffer())
      .WillOnce(testing::Return(nullptr));
  BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id] = 
    std::move(mock_adapter);

  base_window_capturer->portal_init_failed_ = false;

  base_window_capturer->HandleBuffer();
}

TEST_F(BaseWindowCapturerTest, HandleBuffer04) {
  auto mock_adapter = std::make_unique<ScreenCaptureAdapterMock>();
  ScreenCaptureAdapterMock* raw_mock = mock_adapter.get();
  int nweb_id = 1;
  auto mockBuffer = std::make_shared<MockSurfaceBufferAdapter>();

  EXPECT_CALL(*raw_mock, AcquireVideoBuffer())
      .WillOnce(testing::Return(mockBuffer));
  EXPECT_CALL(*mockBuffer, GetFormat())
      .WillOnce(testing::Return(OHOS::NWeb::PixelFormatAdapter::PIXEL_FMT_RGB_555));
  EXPECT_CALL(*mockBuffer, GetSize())
      .Times(0);
  BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id] = 
    std::move(mock_adapter);

  base_window_capturer->portal_init_failed_ = false;

  base_window_capturer->HandleBuffer();
}

TEST_F(BaseWindowCapturerTest, HandleBuffer05) {
  auto mock_adapter = std::make_unique<ScreenCaptureAdapterMock>();
  ScreenCaptureAdapterMock* raw_mock = mock_adapter.get();
  int nweb_id = 1;
  auto mockBuffer = std::make_shared<MockSurfaceBufferAdapter>();

  EXPECT_CALL(*raw_mock, AcquireVideoBuffer())
      .WillOnce(testing::Return(mockBuffer));
  EXPECT_CALL(*mockBuffer, GetFormat())
      .WillOnce(testing::Return(OHOS::NWeb::PixelFormatAdapter::PIXEL_FMT_RGBA_8888));
  EXPECT_CALL(*mockBuffer, GetSize())
      .WillOnce(testing::Return(1));
  EXPECT_CALL(*mockBuffer, GetStride())
      .WillOnce(testing::Return(1));
  EXPECT_CALL(*mockBuffer, GetHeight())
      .WillOnce(testing::Return(1));
  EXPECT_CALL(*raw_mock, ReleaseVideoBuffer())
      .Times(1);
  BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id] = 
    std::move(mock_adapter);

  base_window_capturer->portal_init_failed_ = false;

  base_window_capturer->HandleBuffer();
}

TEST_F(BaseWindowCapturerTest, HandleBuffer06) {
  auto mock_adapter = std::make_unique<ScreenCaptureAdapterMock>();
  ScreenCaptureAdapterMock* raw_mock = mock_adapter.get();
  int nweb_id = 1;
  auto mockBuffer = std::make_shared<MockSurfaceBufferAdapter>();

  EXPECT_CALL(*raw_mock, AcquireVideoBuffer())
      .WillOnce(testing::Return(mockBuffer));
  EXPECT_CALL(*mockBuffer, GetFormat())
      .WillOnce(testing::Return(OHOS::NWeb::PixelFormatAdapter::PIXEL_FMT_RGBA_8888));
  EXPECT_CALL(*mockBuffer, GetSize())
      .WillOnce(testing::Return(2));
  EXPECT_CALL(*mockBuffer, GetStride())
      .WillOnce(testing::Return(1));
  EXPECT_CALL(*mockBuffer, GetHeight())
      .WillOnce(testing::Return(1));
  EXPECT_CALL(*mockBuffer, GetVirAddr())
      .Times(1);
  BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id] = 
    std::move(mock_adapter);

  base_window_capturer->portal_init_failed_ = false;

  base_window_capturer->HandleBuffer();
}

TEST_F(BaseWindowCapturerTest, HandleBuffer07) {
  auto mock_adapter = std::make_unique<ScreenCaptureAdapterMock>();
  ScreenCaptureAdapterMock* raw_mock = mock_adapter.get();
  int nweb_id = 1;
  auto mockBuffer = std::make_shared<MockSurfaceBufferAdapter>();

  EXPECT_CALL(*raw_mock, AcquireVideoBuffer())
      .WillOnce(testing::Return(mockBuffer));
  EXPECT_CALL(*mockBuffer, GetFormat())
      .WillOnce(testing::Return(OHOS::NWeb::PixelFormatAdapter::PIXEL_FMT_RGBA_8888));
  EXPECT_CALL(*mockBuffer, GetSize())
      .WillOnce(testing::Return(2));
  EXPECT_CALL(*mockBuffer, GetStride())
      .WillOnce(testing::Return(1));
  EXPECT_CALL(*mockBuffer, GetHeight())
      .WillOnce(testing::Return(1));
  EXPECT_CALL(*mockBuffer, GetVirAddr())
      .WillOnce(testing::Return(nullptr));
  EXPECT_CALL(*raw_mock, ReleaseVideoBuffer())
      .Times(1);
  BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id] = 
    std::move(mock_adapter);

  base_window_capturer->portal_init_failed_ = false;

  base_window_capturer->HandleBuffer();
}

TEST_F(BaseWindowCapturerTest, HandleBuffer08) {
  auto mock_adapter = std::make_unique<ScreenCaptureAdapterMock>();
  ScreenCaptureAdapterMock* raw_mock = mock_adapter.get();
  int nweb_id = 1;
  auto mockBuffer = std::make_shared<MockSurfaceBufferAdapter>();
  uint8_t test_buffer[1024] = {0};

  EXPECT_CALL(*raw_mock, AcquireVideoBuffer())
      .WillOnce(testing::Return(mockBuffer));
  EXPECT_CALL(*mockBuffer, GetFormat())
      .WillOnce(testing::Return(OHOS::NWeb::PixelFormatAdapter::PIXEL_FMT_RGBA_8888));
  EXPECT_CALL(*mockBuffer, GetSize())
      .WillOnce(testing::Return(2));
  EXPECT_CALL(*mockBuffer, GetStride())
      .WillOnce(testing::Return(1));
  EXPECT_CALL(*mockBuffer, GetHeight())
      .WillOnce(testing::Return(1));
  EXPECT_CALL(*mockBuffer, GetVirAddr())
      .WillOnce(testing::Return(static_cast<void*>(test_buffer)));
  BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id] = 
    std::move(mock_adapter);

  base_window_capturer->portal_init_failed_ = false;

  base_window_capturer->HandleBuffer();
}

TEST_F(BaseWindowCapturerTest, Start01) {
  BaseWindowCapturer::Callback* callback = nullptr;
  auto mock_adapter = std::make_unique<ScreenCaptureAdapterMock>();
  ScreenCaptureAdapterMock* raw_mock = mock_adapter.get();
  int nweb_id = 1;
  auto mockBuffer = std::make_shared<MockSurfaceBufferAdapter>();
  BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id] = 
    std::move(mock_adapter);

  base_window_capturer->portal_init_failed_ = false;
  base_window_capturer->isStart_ = false;

  base_window_capturer->Start(callback);

  EXPECT_TRUE(base_window_capturer->isStart_);
}

TEST_F(BaseWindowCapturerTest, Start02) {
  BaseWindowCapturer::Callback* callback = nullptr;
  auto mock_adapter = std::make_unique<ScreenCaptureAdapterMock>();
  ScreenCaptureAdapterMock* raw_mock = mock_adapter.get();
  int nweb_id = 1;
  auto mockBuffer = std::make_shared<MockSurfaceBufferAdapter>();
  BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id] = 
    std::move(mock_adapter);
  
  EXPECT_CALL(*raw_mock, StartCapture())
      .Times(0);

  base_window_capturer->portal_init_failed_ = false;
  base_window_capturer->isStart_ = true;

  base_window_capturer->Start(callback);
}

TEST_F(BaseWindowCapturerTest, Start03) {
  BaseWindowCapturer::Callback* callback = nullptr;
  auto mock_adapter = std::make_unique<ScreenCaptureAdapterMock>();
  ScreenCaptureAdapterMock* raw_mock = mock_adapter.get();
  int nweb_id = 1;
  auto mockBuffer = std::make_shared<MockSurfaceBufferAdapter>();
  BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id] = 
    std::move(mock_adapter);
  
  EXPECT_CALL(*raw_mock, StartCapture())
      .Times(0);

  base_window_capturer->portal_init_failed_ = true;
  base_window_capturer->isStart_ = false;

  base_window_capturer->Start(callback);
}

TEST_F(BaseWindowCapturerTest, HandleCaptureStateCode01) {
  const OHOS::NWeb::ScreenCaptureStateCodeAdapter& code = 
      OHOS::NWeb::ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_CANCELED;

  auto res = base_window_capturer->HandleCaptureStateCode(code);
  EXPECT_EQ(res, DesktopCapturer::Result::ERROR_PERMANENT);
}

TEST_F(BaseWindowCapturerTest, HandleCaptureStateCode02) {
  const OHOS::NWeb::ScreenCaptureStateCodeAdapter& code = 
      OHOS::NWeb::ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STOPPED_BY_USER;

  auto res = base_window_capturer->HandleCaptureStateCode(code);
  EXPECT_EQ(res, DesktopCapturer::Result::ERROR_PERMANENT);
}

TEST_F(BaseWindowCapturerTest, HandleCaptureStateCode03) {
  const OHOS::NWeb::ScreenCaptureStateCodeAdapter& code = 
      OHOS::NWeb::ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_INTERRUPTED_BY_OTHER;

  auto res = base_window_capturer->HandleCaptureStateCode(code);
  EXPECT_EQ(res, DesktopCapturer::Result::ERROR_PERMANENT);
}

TEST_F(BaseWindowCapturerTest, HandleCaptureStateCode04) {
  const OHOS::NWeb::ScreenCaptureStateCodeAdapter& code = 
      OHOS::NWeb::ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STOPPED_BY_CALL;

  auto res = base_window_capturer->HandleCaptureStateCode(code);
  EXPECT_EQ(res, DesktopCapturer::Result::ERROR_PERMANENT);
}

TEST_F(BaseWindowCapturerTest, HandleCaptureStateCode05) {
  const OHOS::NWeb::ScreenCaptureStateCodeAdapter& code = 
      OHOS::NWeb::ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_ENTER_PRIVATE_SCENE;

  auto res = base_window_capturer->HandleCaptureStateCode(code);
  EXPECT_EQ(res, DesktopCapturer::Result::ERROR_TEMPORARY);
}

TEST_F(BaseWindowCapturerTest, HandleCaptureStateCode06) {
  const OHOS::NWeb::ScreenCaptureStateCodeAdapter& code = 
      OHOS::NWeb::ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_EXIT_PRIVATE_SCENE;

  auto res = base_window_capturer->HandleCaptureStateCode(code);
  EXPECT_EQ(res, DesktopCapturer::Result::SUCCESS);
}

TEST_F(BaseWindowCapturerTest, CaptureFrame01) {
  int nweb_id = 1;
  BaseScreenCaptureSource::GetInstance().capture_state_code_map_[nweb_id] = 
    OHOS::NWeb::ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_CANCELED;

  base_window_capturer->portal_init_failed_ = true;
  auto mock_callback = std::make_shared<MockCallback>();
  base_window_capturer->callback_ = mock_callback.get();

  EXPECT_CALL(*mock_callback, OnCaptureResult(testing::_, testing::_))
      .Times(1);

  base_window_capturer->CaptureFrame();
}

TEST_F(BaseWindowCapturerTest, CaptureFrame02) {
  int nweb_id = 1;
  BaseScreenCaptureSource::GetInstance().capture_state_code_map_[nweb_id] = 
    OHOS::NWeb::ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_CANCELED;

  base_window_capturer->portal_init_failed_ = false;
  auto mock_callback = std::make_shared<MockCallback>();
  base_window_capturer->callback_ = mock_callback.get();

  EXPECT_CALL(*mock_callback, OnCaptureResult(testing::_, testing::_))
      .Times(1);

  base_window_capturer->CaptureFrame();
}

TEST_F(BaseWindowCapturerTest, CaptureFrame03) {
  int nweb_id = 1;
  BaseScreenCaptureSource::GetInstance().capture_state_code_map_[nweb_id] = 
    OHOS::NWeb::ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_EXIT_PRIVATE_SCENE;

  base_window_capturer->portal_init_failed_ = false;
  auto mock_callback = std::make_shared<MockCallback>();
  base_window_capturer->callback_ = mock_callback.get();
  base_window_capturer->current_frame_ = nullptr;

  EXPECT_CALL(*mock_callback, OnCaptureResult(testing::_, testing::_))
      .Times(1);

  base_window_capturer->CaptureFrame();
}
}
