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
#include <gtest/gtest.h>
#define private public
#include "media/capture/video/ohos/video_capture_device_factory_ohos.h"
#undef private

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include "gmock/gmock.h"

#include <algorithm>
#include <utility>

#include "base/containers/contains.h"
#include "base/files/file_enumerator.h"
#include "base/files/file_util.h"
#include "base/posix/eintr_wrapper.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/system/system_monitor.h"
#include "base/task/single_thread_task_runner.h"
#include "build/build_config.h"
#include "camera_manager_adapter.h"
#include "media/capture/video/ohos/video_capture_device_ohos.h"
#include "ohos_adapter_helper.h"
#include "video_capture_common_ohos.h"
#include "video_capture_status_callback_listener_ohos.h"

using namespace testing;

namespace media {

namespace {
std::shared_ptr<VideoCaptureDeviceFactoryOHOS> v_factory;

}

class SingleThreadTaskRunnerMock : public base::SingleThreadTaskRunner {
 public:
  SingleThreadTaskRunnerMock() = default;
  ~SingleThreadTaskRunnerMock() = default;
};

class VideoCaptureDeviceDescriptorMock : public VideoCaptureDeviceDescriptor {
 public:
  VideoCaptureDeviceDescriptorMock()
      : VideoCaptureDeviceDescriptor("", "0", VideoCaptureApi::UNKNOWN) {}
};

class CameraManagerAdapterMock : public CameraManagerAdapter {
 public:
  MOCK_METHOD(int32_t,
              Create,
              (std::shared_ptr<CameraStatusCallbackAdapter>),
              (override));
  MOCK_METHOD(std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>>,
              GetDevicesInfo,
              (),
              (override));
  MOCK_METHOD(int32_t, ReleaseCameraManger, (), (override));
  MOCK_METHOD(int32_t,
              GetExposureModes,
              (std::vector<ExposureModeAdapter>&),
              (override));
  MOCK_METHOD(int32_t,
              GetCurrentExposureMode,
              (ExposureModeAdapter&),
              (override));
  MOCK_METHOD(std::shared_ptr<VideoCaptureRangeAdapter>,
              GetCaptionRangeById,
              (RangeIDAdapter),
              (override));
  MOCK_METHOD(bool, IsFocusModeSupported, (FocusModeAdapter), (override));
  MOCK_METHOD(FocusModeAdapter, GetCurrentFocusMode, (), (override));
  MOCK_METHOD(bool, IsFlashModeSupported, (FlashModeAdapter), (override));
  MOCK_METHOD(int32_t, RestartSession, (), (override));
  MOCK_METHOD(int32_t, StopSession, (CameraStopType), (override));
  MOCK_METHOD(CameraStatusAdapter, GetCameraStatus, (), (override));
  MOCK_METHOD(bool, IsExistCaptureTask, (), (override));
  MOCK_METHOD(int32_t,
              StartStream,
              (const std::string&,
               const std::shared_ptr<VideoCaptureParamsAdapter>,
               std::shared_ptr<CameraBufferListenerAdapter>),
              ());
  MOCK_METHOD(void, SetForegroundFlag, (bool), (override));
  MOCK_METHOD(void, SetCameraStatus, (CameraStatusAdapter), (override));
  MOCK_METHOD(std::string, GetCurrentDeviceId, (), (override));
};

class VideoDeviceDescriptorAdapterMock : public VideoDeviceDescriptorAdapter {
 public:
  MOCK_METHOD(std::string, GetDisplayName, (), (override));
  MOCK_METHOD(std::string, GetDeviceId, (), (override));
  MOCK_METHOD(std::string, GetModelId, (), (override));
  MOCK_METHOD(std::shared_ptr<VideoControlSupportAdapter>,
              GetControlSupport,
              (),
              (override));
  MOCK_METHOD(VideoTransportType, GetTransportType, (), (override));
  MOCK_METHOD(VideoFacingModeAdapter, GetFacingMode, (), (override));
  MOCK_METHOD(std::vector<std::shared_ptr<FormatAdapter>>,
              GetSupportCaptureFormats,
              (),
              (override));
};

class OhosAdapterHelperMock : public OhosAdapterHelper {
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
};

class VideoControlSupportAdapterMock : public VideoControlSupportAdapter {
 public:
  MOCK_METHOD(bool, GetPan, (), (override));
  MOCK_METHOD(bool, GetTilt, (), (override));
  MOCK_METHOD(bool, GetZoom, (), (override));
};

class VideoCaptureDeviceFactoryOHOSTest : public testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();
};

void VideoCaptureDeviceFactoryOHOSTest::SetUpTestCase(void) {}

void VideoCaptureDeviceFactoryOHOSTest::TearDownTestCase(void) {}

void VideoCaptureDeviceFactoryOHOSTest::SetUp(void) {
  scoped_refptr<SingleThreadTaskRunnerMock> ui_task_runner;
  v_factory = std::make_shared<VideoCaptureDeviceFactoryOHOS>(ui_task_runner);
  ASSERT_NE(v_factory, nullptr);
}

void VideoCaptureDeviceFactoryOHOSTest::TearDown(void) {
  v_factory = nullptr;
}

// CreateDevice
TEST_F(VideoCaptureDeviceFactoryOHOSTest,
       MediaVideoFactoryTest_VideoCaptureDeviceFactoryOHOS_001) {
  auto expect = VideoCaptureErrorOrDevice(
      VideoCaptureError::kVideoCaptureDeviceFactoryChromeOSCreateDeviceFailed);
  VideoCaptureDeviceDescriptorMock device_descriptor_mock;
  auto ret = v_factory->CreateDevice(device_descriptor_mock);
  EXPECT_EQ(expect.error_code_, ret.error_code_);
}

// GetDevicesInfo
TEST_F(VideoCaptureDeviceFactoryOHOSTest,
       MediaVideoFactoryTest_VideoCaptureDeviceFactoryOHOS_002) {
  auto camera_manager_adapter_mock =
      std::make_shared<CameraManagerAdapterMock>();
  auto video_device_descriptor_adapter_mock =
      std::make_shared<VideoDeviceDescriptorAdapterMock>();
  auto video_control_support_adapter_mock =
      std::make_shared<VideoControlSupportAdapterMock>();

  EXPECT_CALL(*video_device_descriptor_adapter_mock, GetDisplayName())
      .WillRepeatedly(Return("lcam001"));
  EXPECT_CALL(*video_device_descriptor_adapter_mock, GetDeviceId())
      .WillRepeatedly(Return("lcam001"));
  EXPECT_CALL(*video_device_descriptor_adapter_mock, GetFacingMode())
      .WillRepeatedly(Return(VideoFacingModeAdapter::FACING_NONE));
  EXPECT_CALL(*video_device_descriptor_adapter_mock, GetControlSupport())
      .WillRepeatedly(Return(video_control_support_adapter_mock));
  EXPECT_CALL(*video_control_support_adapter_mock, GetPan())
      .WillRepeatedly(Return(false));
  EXPECT_CALL(*video_control_support_adapter_mock, GetTilt())
      .WillRepeatedly(Return(false));
  EXPECT_CALL(*video_control_support_adapter_mock, GetZoom())
      .WillRepeatedly(Return(false));
  EXPECT_CALL(*video_device_descriptor_adapter_mock, GetSupportCaptureFormats())
      .WillRepeatedly(Return(std::vector<std::shared_ptr<FormatAdapter>>()));

  std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>> devices_desc = {
      video_device_descriptor_adapter_mock};
  EXPECT_CALL(*camera_manager_adapter_mock, GetDevicesInfo())
      .WillRepeatedly(Return(devices_desc));
  auto o_mock = std::make_shared<OhosAdapterHelperMock>();
  EXPECT_CALL(*o_mock, GetCameraManagerAdapter())
      .WillRepeatedly(ReturnRef(*camera_manager_adapter_mock));
  bool callback_invoked = false;
  VideoCaptureDeviceFactory::GetDevicesInfoCallback callback = base::BindOnce(
      [](bool* invoked, std::vector<VideoCaptureDeviceInfo> devices_info) {
        *invoked = true;
        EXPECT_EQ(devices_info.size(), 1u);
        EXPECT_EQ(devices_info[0].descriptor.device_id, "lcam001");
        EXPECT_EQ(devices_info[0].descriptor.display_name(), "lcam001");
        EXPECT_EQ(devices_info[0].descriptor.control_support().pan, false);
        EXPECT_EQ(devices_info[0].descriptor.control_support().tilt, false);
        EXPECT_EQ(devices_info[0].descriptor.control_support().zoom, false);
      },
      &callback_invoked);

  v_factory->GetDevicesInfo(std::move(callback));
  EXPECT_TRUE(callback_invoked);
}

// OnCameraStatusChanged
TEST_F(VideoCaptureDeviceFactoryOHOSTest,
       MediaVideoFactoryTest_VideoCaptureDeviceFactoryOHOS_003) {
  CameraStatusAdapter camera_status = CameraStatusAdapter::AVAILABLE;
  std::string callback_device_id = "callback_device_id";
  auto camera_manager_adapter_mock =
      std::make_shared<CameraManagerAdapterMock>();
  EXPECT_CALL(*camera_manager_adapter_mock, IsExistCaptureTask())
      .WillRepeatedly(::testing::Return(false));
  v_factory->OnCameraStatusChanged(camera_status, callback_device_id);
}
// CheckDeviceId
TEST_F(VideoCaptureDeviceFactoryOHOSTest,
       MediaVideoFactoryTest_VideoCaptureDeviceFactoryOHOS_004) {
  std::string device_id = "device_id";
  auto devices_desc =
      std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>>();
  auto camera_manager_adapter_mock =
      std::make_shared<CameraManagerAdapterMock>();
  EXPECT_CALL(*camera_manager_adapter_mock, GetDevicesInfo())
      .WillRepeatedly(::testing::Return(devices_desc));
  auto number = v_factory->CheckDeviceId(device_id);
  EXPECT_EQ(number, kErrorReturnValue);
}

}  // namespace media