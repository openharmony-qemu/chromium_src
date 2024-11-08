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
#include "gmock/gmock.h"
#define private public
#include "media/capture/video/ohos/ohos_capture_delegate.h"

#include <stddef.h>

#include <utility>

#include <stddef.h>
#include <stdint.h>
#include "base/functional/bind.h"
#include "base/task/single_thread_task_runner.h"
#include "build/build_config.h"
#include "media/capture/video/blob_utils.h"
#include "media/capture/video/ohos/ohos_capture_delegate.h"
#include "ohos_adapter_helper.h"
#include "third_party/libyuv/include/libyuv.h"
#include "video_capture_common_ohos.h"

#include <memory>
#include <string>
#include <vector>

#include "base/containers/queue.h"
#include "base/files/file_util.h"
#include "base/files/scoped_file.h"
#include "base/test/bind.h"
#include "base/threading/thread.h"
#include "camera_manager_adapter.h"
#include "display_manager_adapter.h"
#include "media/capture/mojom/image_capture_types.h"
#include "media/capture/video/video_capture_device.h"
#include "media/capture/video_capture_types.h"
#include "video_capture_params_adapter_impl.h"
#include "video_capture_surface_buffer_listener_ohos.h"
#undef private

using namespace testing;

namespace media {

class OhosAdapterHelperMock;
class CameraManagerAdapterMock;
namespace {
std::shared_ptr<OHOSCaptureDelegate> c_delegate;
std::unique_ptr<OhosAdapterHelperMock> ohos_adapter_helper;
std::unique_ptr<CameraManagerAdapterMock> camera_manager_adapter;
}  // namespace

class SingleThreadTaskRunnerMock : public base::SingleThreadTaskRunner {};

class VideoCaptureParamsMock : public VideoCaptureParams {};

class OHOSCaptureDelegateTest : public testing::Test {
 public:
  static void SetUpTestCase(void);
  static void TearDownTestCase(void);
  void SetUp();
  void TearDown();
};

void OHOSCaptureDelegateTest::SetUpTestCase(void) {}

void OHOSCaptureDelegateTest::TearDownTestCase(void) {}

void OHOSCaptureDelegateTest::SetUp(void) {
  ohos_adapter_helper = std::make_unique<OhosAdapterHelperMock>();
  camera_manager_adapter = std::make_unique<CameraManagerAdapterMock>();

  VideoCaptureDeviceDescriptor device_descriptor;
  scoped_refptr<SingleThreadTaskRunnerMock> capture_stask_runner;
  VideoCaptureParamsMock capture_params;
  c_delegate = std::make_shared<OHOSCaptureDelegate>(
      device_descriptor, capture_stask_runner, capture_params);
  ASSERT_NE(c_delegate, nullptr);
}

void OHOSCaptureDelegateTest::TearDown(void) {
  c_delegate = nullptr;
  ohos_adapter_helper.reset();
  camera_manager_adapter.reset();
}

class ClientMock : public VideoCaptureDevice::Client {
 public:
  MOCK_METHOD(void, OnCaptureConfigurationChanged, (), (override));
  MOCK_METHOD(void,
              OnIncomingCapturedData,
              (const uint8_t* data,
               int length,
               const VideoCaptureFormat& frame_format,
               const gfx::ColorSpace& color_space,
               int clockwise_rotation,
               bool flip_y,
               base::TimeTicks reference_time,
               base::TimeDelta timestamp,
               int frame_feedback_id),
              (override));
  MOCK_METHOD(void,
              OnIncomingCapturedData,
              (const uint8_t* data,
               int length,
               const VideoCaptureFormat& frame_format,
               const gfx::ColorSpace& color_space,
               int clockwise_rotation,
               bool flip_y,
               base::TimeTicks reference_time,
               base::TimeDelta timestamp),
              ());
  MOCK_METHOD(void,
              OnIncomingCapturedGfxBuffer,
              (gfx::GpuMemoryBuffer * buffer,
               const VideoCaptureFormat& frame_format,
               int clockwise_rotation,
               base::TimeTicks reference_time,
               base::TimeDelta timestamp,
               int frame_feedback_id),
              (override));
  MOCK_METHOD(void,
              OnIncomingCapturedGfxBuffer,
              (gfx::GpuMemoryBuffer * buffer,
               const VideoCaptureFormat& frame_format,
               int clockwise_rotation,
               base::TimeTicks reference_time,
               base::TimeDelta timestamp),
              ());
  MOCK_METHOD(void,
              OnIncomingCapturedExternalBuffer,
              (CapturedExternalVideoBuffer buffer,
               std::vector<CapturedExternalVideoBuffer> scaled_buffers,
               base::TimeTicks reference_time,
               base::TimeDelta timestamp,
               gfx::Rect visible_rect),
              (override));
  MOCK_METHOD(Client::ReserveResult,
              ReserveOutputBuffer,
              (const gfx::Size& dimensions,
               VideoPixelFormat format,
               int frame_feedback_id,
               Client::Buffer* buffer),
              (override));
  MOCK_METHOD(void,
              OnIncomingCapturedBuffer,
              (Client::Buffer buffer,
               const VideoCaptureFormat& format,
               base::TimeTicks reference_time,
               base::TimeDelta timestamp),
              (override));
  MOCK_METHOD(void,
              OnIncomingCapturedBufferExt,
              (Client::Buffer buffer,
               const VideoCaptureFormat& format,
               const gfx::ColorSpace& color_space,
               base::TimeTicks reference_time,
               base::TimeDelta timestamp,
               gfx::Rect visible_rect,
               const VideoFrameMetadata& additional_metadata),
              (override));
  MOCK_METHOD(void,
              OnError,
              (VideoCaptureError error,
               const base::Location& from_here,
               const std::string& reason),
              (override));
  MOCK_METHOD(void,
              OnFrameDropped,
              (VideoCaptureFrameDropReason reason),
              (override));
  MOCK_METHOD(void, OnLog, (const std::string& message), (override));
  MOCK_METHOD(double, GetBufferPoolUtilization, (), (const, override));
  MOCK_METHOD(void, OnStarted, (), (override));
};

class CameraManagerAdapterMock : public CameraManagerAdapter {
 public:
  MOCK_METHOD(
      int32_t,
      Create,
      (std::shared_ptr<CameraStatusCallbackAdapter> cameraStatusCallback),
      (override));
  MOCK_METHOD(std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>>,
              GetDevicesInfo,
              (),
              (override));
  MOCK_METHOD(int32_t, ReleaseCameraManger, (), (override));
  MOCK_METHOD(int32_t,
              GetExposureModes,
              (std::vector<ExposureModeAdapter> & exposureModesAdapter),
              (override));
  MOCK_METHOD(int32_t,
              GetCurrentExposureMode,
              (ExposureModeAdapter & exposureModeAdapter),
              (override));
  MOCK_METHOD(std::shared_ptr<VideoCaptureRangeAdapter>,
              GetCaptionRangeById,
              (RangeIDAdapter rangeId),
              (override));
  MOCK_METHOD(bool,
              IsFocusModeSupported,
              (FocusModeAdapter focusMode),
              (override));
  MOCK_METHOD(FocusModeAdapter, GetCurrentFocusMode, (), (override));
  MOCK_METHOD(bool,
              IsFlashModeSupported,
              (FlashModeAdapter flashMode),
              (override));
  MOCK_METHOD(int32_t, RestartSession, (), (override));
  MOCK_METHOD(int32_t, StopSession, (CameraStopType stopType), (override));
  MOCK_METHOD(CameraStatusAdapter, GetCameraStatus, (), (override));
  MOCK_METHOD(bool, IsExistCaptureTask, (), (override));
  MOCK_METHOD(int32_t,
              StartStream,
              (const std::string& deviceId,
               const std::shared_ptr<VideoCaptureParamsAdapter> captureParams,
               std::shared_ptr<CameraBufferListenerAdapter> listener),
              (override));
  MOCK_METHOD(void, SetForegroundFlag, (bool isForeground), (override));
  MOCK_METHOD(void, SetCameraStatus, (CameraStatusAdapter status), (override));
  MOCK_METHOD(std::string, GetCurrentDeviceId, (), (override));
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
              (const std::string& hapPath),
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
              (const std::string& hapPath),
              (override));
};

class CameraSurfaceBufferAdapterMock : public CameraSurfaceBufferAdapter {
 public:
  MOCK_METHOD(int32_t, GetFileDescriptor, (), (override));
  MOCK_METHOD(int32_t, GetWidth, (), (override));
  MOCK_METHOD(int32_t, GetHeight, (), (override));
  MOCK_METHOD(int32_t, GetStride, (), (override));
  MOCK_METHOD(int32_t, GetFormat, (), (override));
  MOCK_METHOD(uint32_t, GetSize, (), (override));
  MOCK_METHOD(uint8_t*, GetBufferAddr, (), (override));
};

class CameraRotationInfoAdapterMock : public CameraRotationInfoAdapter {
 public:
  MOCK_METHOD(int32_t, GetRotation, (), (override));
  MOCK_METHOD(bool, GetIsFlipX, (), (override));
  MOCK_METHOD(bool, GetIsFlipY, (), (override));
};

class CameraSurfaceAdapterMock : public CameraSurfaceAdapter {
 public:
  MOCK_METHOD(int32_t,
              ReleaseBuffer,
              (std::shared_ptr<CameraSurfaceBufferAdapter> buffer,
               int32_t fence),
              (override));
};

void MockTakePhotoCallback(mojo::StructPtr<media::mojom::Blob> blob) {
  EXPECT_TRUE(blob.is_null());
}

void MockGetPhotoStateCallback(
    mojo::StructPtr<media::mojom::PhotoState> photo_state) {
  EXPECT_FALSE(photo_state.is_null());
  EXPECT_EQ(photo_state->height->max, 720);
  EXPECT_EQ(photo_state->width->max, 1280);
}

void MockSetPhotoOptionsCallback(bool result) {
  EXPECT_TRUE(result);
}

TEST_F(OHOSCaptureDelegateTest, AllocateAndStart_001) {
  auto cameraManagerAdapterMock = std::make_shared<CameraManagerAdapterMock>();
  auto client = std::make_unique<ClientMock>();
  c_delegate->client_ = std::move(client);
  EXPECT_CALL(*cameraManagerAdapterMock, GetCameraStatus())
      .WillRepeatedly(Return(CameraStatusAdapter::APPEAR));
  c_delegate->AllocateAndStart(std::move(client));
}

TEST_F(OHOSCaptureDelegateTest, StopAndDeAllocate_002) {
  c_delegate->client_ = std::unique_ptr<VideoCaptureDevice::Client>();
  c_delegate->StopAndDeAllocate();
  EXPECT_EQ(c_delegate->client_, nullptr);
}

TEST_F(OHOSCaptureDelegateTest, TakePhoto_003) {
  base::queue<VideoCaptureDevice::TakePhotoCallback> take_photo_callbacks_;
  base::queue<VideoCaptureDevice::TakePhotoCallback>().swap(
      c_delegate->take_photo_callbacks_);
  EXPECT_EQ(c_delegate->take_photo_callbacks_.empty(), true);
  media::VideoCaptureDevice::TakePhotoCallback callback =
      base::BindOnce(&MockTakePhotoCallback);
  c_delegate->TakePhoto(std::move(callback));
  EXPECT_NE(c_delegate->take_photo_callbacks_.empty(), true);
}

TEST_F(OHOSCaptureDelegateTest, GetPhotoState_004) {
  c_delegate->is_capturing_ = true;
  c_delegate->capture_format_.frame_size = gfx::Size(1280, 720);
  VideoCaptureDevice::GetPhotoStateCallback callback =
      base::BindOnce(&MockGetPhotoStateCallback);
  c_delegate->GetPhotoState(std::move(callback));
}

TEST_F(OHOSCaptureDelegateTest, SetPhotoOptions_005) {
  c_delegate->is_capturing_ = true;
  auto settings = media::mojom::PhotoSettings::New();
  VideoCaptureDevice::SetPhotoOptionsCallback callback =
      base::BindOnce(&MockSetPhotoOptionsCallback);
  c_delegate->SetPhotoOptions(std::move(settings), std::move(callback));
}

TEST_F(OHOSCaptureDelegateTest, MaybeSuspend_006) {
  c_delegate->MaybeSuspend();
}

TEST_F(OHOSCaptureDelegateTest, Resume_007) {
  c_delegate->is_capturing_ = true;
  c_delegate->Resume();
}

// No such function
TEST_F(OHOSCaptureDelegateTest, SetRotation_009) {}

TEST_F(OHOSCaptureDelegateTest, GetWeakPtr_010) {
  auto ret = c_delegate->GetWeakPtr();
  EXPECT_NE(ret, nullptr);
}

TEST_F(OHOSCaptureDelegateTest, StartStream_011) {
  auto cameraManagerAdapterMock = std::make_shared<CameraManagerAdapterMock>();
  EXPECT_CALL(*cameraManagerAdapterMock, GetCameraStatus())
      .WillRepeatedly(Return(CameraStatusAdapter::UNAVAILABLE));
  auto ret = c_delegate->StartStream();
  EXPECT_EQ(ret, false);
}

// No such function
TEST_F(OHOSCaptureDelegateTest, DoCapture_012) {}

TEST_F(OHOSCaptureDelegateTest, StopStream_013) {
  c_delegate->is_capturing_ = false;
  auto ret = c_delegate->StopStream();
  EXPECT_EQ(ret, false);
}

TEST_F(OHOSCaptureDelegateTest, SetErrorState_014) {
  VideoCaptureError error =
      VideoCaptureError::kAndroidApi1CameraErrorCallbackReceived;
  base::Location from_here = base::Location::Current();
  std::string reason = "Test error reason";
  ClientMock* client = new ClientMock();
  EXPECT_CALL(*client, OnError(testing::_, testing::_, testing::_))
      .WillOnce(testing::Return());
  c_delegate->client_ = std::unique_ptr<VideoCaptureDevice::Client>(client);
  c_delegate->SetErrorState(error, from_here, reason);
}

TEST_F(OHOSCaptureDelegateTest, TransToOHOSCaptrueParams_015) {
  const VideoCaptureParams in;
  std::shared_ptr<VideoCaptureParamsAdapterImpl> out = nullptr;
  auto ret = c_delegate->TransToOHOSCaptrueParams(in, out);
  EXPECT_EQ(ret, -1);
}

TEST_F(OHOSCaptureDelegateTest, GetUsableExposureMode_016) {
  ExposureModeAdapter exposure_mode_adapter =
      ExposureModeAdapter::EXPOSURE_MODE_LOCKED;
  MeteringMode exposure_mode = MeteringMode::NONE;
  auto ret =
      c_delegate->GetUsableExposureMode(exposure_mode_adapter, exposure_mode);
  EXPECT_EQ(ret, -1);
}

TEST_F(OHOSCaptureDelegateTest, GetCurrentExposureMode_017) {
  ExposureModeAdapter exposure_mode_adapter =
      ExposureModeAdapter::EXPOSURE_MODE_CONTINUOUS_AUTO;
  auto ret = c_delegate->GetCurrentExposureMode(exposure_mode_adapter);
  EXPECT_EQ(ret, MeteringMode::CONTINUOUS);
}

TEST_F(OHOSCaptureDelegateTest, GetExposureState_018) {
  mojom::PhotoStatePtr photo_capabilities = mojom::PhotoState::New();
  c_delegate->GetExposureState(photo_capabilities);
}

TEST_F(OHOSCaptureDelegateTest, RetrieveUserControlRange_019) {
  RangeIDAdapter rangeID = RangeIDAdapter::RANGE_ID_EXP_COMPENSATION;
  c_delegate->RetrieveUserControlRange(rangeID);
}

TEST_F(OHOSCaptureDelegateTest, GetFocusState_020) {
  mojom::PhotoStatePtr photo_capabilities = mojom::PhotoState::New();
  c_delegate->GetFocusState(photo_capabilities);
}

TEST_F(OHOSCaptureDelegateTest, GetFlashState_021) {
  mojom::PhotoStatePtr photo_capabilities = mojom::PhotoState::New();
  c_delegate->GetFlashState(photo_capabilities);
}
// No such function
TEST_F(OHOSCaptureDelegateTest, GetCameraRotation_022) {}

TEST_F(OHOSCaptureDelegateTest, GetFlashState04) {
  ON_CALL(*camera_manager_adapter,
          IsFlashModeSupported(FlashModeAdapter::FLASH_MODE_CLOSE))
      .WillByDefault(Return(false));
  mojom::PhotoStatePtr photo_capabilities = mojom::PhotoState::New();
  c_delegate->GetFlashState(photo_capabilities);
  EXPECT_TRUE(std::find(photo_capabilities->fill_light_mode.begin(),
                        photo_capabilities->fill_light_mode.end(),
                        mojom::FillLightMode::OFF) ==
              photo_capabilities->fill_light_mode.end());
}

TEST_F(OHOSCaptureDelegateTest, GetFlashState05) {
  ON_CALL(*camera_manager_adapter,
          IsFlashModeSupported(FlashModeAdapter::FLASH_MODE_OPEN))
      .WillByDefault(Return(false));
  mojom::PhotoStatePtr photo_capabilities = mojom::PhotoState::New();
  c_delegate->GetFlashState(photo_capabilities);
  EXPECT_TRUE(std::find(photo_capabilities->fill_light_mode.begin(),
                        photo_capabilities->fill_light_mode.end(),
                        mojom::FillLightMode::FLASH) ==
              photo_capabilities->fill_light_mode.end());
}

TEST_F(OHOSCaptureDelegateTest, GetFlashState06) {
  ON_CALL(*camera_manager_adapter,
          IsFlashModeSupported(FlashModeAdapter::FLASH_MODE_AUTO))
      .WillByDefault(Return(false));
  mojom::PhotoStatePtr photo_capabilities = mojom::PhotoState::New();
  c_delegate->GetFlashState(photo_capabilities);
  EXPECT_TRUE(std::find(photo_capabilities->fill_light_mode.begin(),
                        photo_capabilities->fill_light_mode.end(),
                        mojom::FillLightMode::AUTO) ==
              photo_capabilities->fill_light_mode.end());
}

TEST_F(OHOSCaptureDelegateTest, Resume) {
  c_delegate->is_capturing_ = false;
  c_delegate->Resume();
  EXPECT_FALSE(c_delegate->is_capturing_);
  c_delegate->is_capturing_ = true;
  c_delegate->Resume();
  EXPECT_TRUE(c_delegate->is_capturing_);
}

TEST_F(OHOSCaptureDelegateTest, StartStream1) {
  testing::internal::CaptureStderr();
  OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().SetCameraStatus(
      CameraStatusAdapter::AVAILABLE);
  bool result = c_delegate->StartStream();
  EXPECT_FALSE(result);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("can not find matched parameter"),
            std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, StartStream2) {
  testing::internal::CaptureStderr();
  OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().SetCameraStatus(
      CameraStatusAdapter::UNAVAILABLE);
  bool result = c_delegate->StartStream();
  EXPECT_FALSE(result);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("camera is not closed"), std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, TransToOHOSCaptrueParams) {
  const media::VideoCaptureParams in;
  std::shared_ptr<media::VideoCaptureParamsAdapterImpl> out = nullptr;
  auto ret = c_delegate->TransToOHOSCaptrueParams(in, out);
  EXPECT_EQ(ret, -1);
}

TEST_F(OHOSCaptureDelegateTest, GetUsableExposureMode1) {
  ExposureModeAdapter exposure_mode_adapter =
      ExposureModeAdapter::EXPOSURE_MODE_UNSUPPORTED;
  MeteringMode exposure_mode = MeteringMode::CONTINUOUS;
  auto ret =
      c_delegate->GetUsableExposureMode(exposure_mode_adapter, exposure_mode);
  EXPECT_EQ(ret, 0);
}

TEST_F(OHOSCaptureDelegateTest, GetUsableExposureMode2) {
  ExposureModeAdapter exposure_mode_adapter =
      ExposureModeAdapter::EXPOSURE_MODE_LOCKED;
  MeteringMode exposure_mode = MeteringMode::NONE;
  auto ret =
      c_delegate->GetUsableExposureMode(exposure_mode_adapter, exposure_mode);
  EXPECT_EQ(ret, -1);
}

TEST_F(OHOSCaptureDelegateTest, GetCurrentExposureMode1) {
  ExposureModeAdapter exposure_mode_adapter =
      ExposureModeAdapter::EXPOSURE_MODE_LOCKED;
  auto ret = c_delegate->GetCurrentExposureMode(exposure_mode_adapter);
  EXPECT_EQ(ret, MeteringMode::SINGLE_SHOT);
}

TEST_F(OHOSCaptureDelegateTest, GetCurrentExposureMode2) {
  testing::internal::CaptureStderr();
  int32_t a = 5;
  ExposureModeAdapter b = (ExposureModeAdapter)a;
  auto ret = c_delegate->GetCurrentExposureMode(b);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("not found."), std::string::npos);
  EXPECT_EQ(ret, MeteringMode::NONE);
}

TEST_F(OHOSCaptureDelegateTest, RetrieveUserControlRange) {
  testing::internal::CaptureStderr();
  RangeIDAdapter rangeID = RangeIDAdapter::RANGE_ID_EXP_COMPENSATION;
  mojom::RangePtr rangePtr = c_delegate->RetrieveUserControlRange(rangeID);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("get current caption range failed"),
            std::string::npos);
  mojom::RangePtr capability = mojom::Range::New();
  EXPECT_EQ(rangePtr, capability);
}

TEST_F(OHOSCaptureDelegateTest, GetExposureState) {
  testing::internal::CaptureStderr();
  mojom::PhotoStatePtr photo_capabilities = mojo::CreateEmptyPhotoState();
  c_delegate->GetExposureState(photo_capabilities);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("get exposure mode failed"), std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, OnBufferAvailable1) {
  std::shared_ptr<CameraSurfaceAdapter> surface = nullptr;
  std::shared_ptr<CameraSurfaceBufferAdapterMock> buffer = nullptr;
  std::shared_ptr<CameraRotationInfoAdapterMock> roration_info = nullptr;
  c_delegate->client_ = nullptr;
  c_delegate->OnBufferAvailable(surface, buffer, roration_info);
  base::TimeTicks null_time;
  EXPECT_NE(c_delegate->first_ref_time_, null_time);
}

TEST_F(OHOSCaptureDelegateTest, OnBufferAvailable2) {
  testing::internal::CaptureStderr();
  std::shared_ptr<CameraSurfaceAdapter> surface = nullptr;
  auto buffer = std::make_shared<CameraSurfaceBufferAdapterMock>();
  auto roration_info = std::make_shared<CameraRotationInfoAdapterMock>();
  c_delegate->OnBufferAvailable(surface, buffer, roration_info);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OnBufferAvailable params is nullptr"),
            std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, OnBufferAvailable3) {
  testing::internal::CaptureStderr();
  std::shared_ptr<CameraSurfaceBufferAdapter> buffer = nullptr;
  auto surface = std::make_shared<CameraSurfaceAdapterMock>();
  auto roration_info = std::make_shared<CameraRotationInfoAdapterMock>();
  c_delegate->OnBufferAvailable(surface, buffer, roration_info);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OnBufferAvailable params is nullptr"),
            std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, OnBufferAvailable4) {
  testing::internal::CaptureStderr();
  std::shared_ptr<CameraRotationInfoAdapter> roration_info = nullptr;
  auto surface = std::make_shared<CameraSurfaceAdapterMock>();
  auto buffer = std::make_shared<CameraSurfaceBufferAdapterMock>();
  c_delegate->OnBufferAvailable(surface, buffer, roration_info);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OnBufferAvailable params is nullptr"),
            std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, OnBufferAvailable5) {
  testing::internal::CaptureStderr();
  auto surface = std::make_shared<CameraSurfaceAdapterMock>();
  auto buffer = std::make_shared<CameraSurfaceBufferAdapterMock>();
  auto roration_info = std::make_shared<CameraRotationInfoAdapterMock>();
  c_delegate->client_ = nullptr;
  EXPECT_CALL(*roration_info, GetRotation()).WillRepeatedly(Return(90));
  EXPECT_CALL(*roration_info, GetIsFlipY()).WillRepeatedly(Return(false));
  EXPECT_CALL(*buffer, GetBufferAddr()).WillRepeatedly(Return(nullptr));
  EXPECT_CALL(*buffer, GetSize()).WillRepeatedly(Return(0));
  EXPECT_CALL(*surface, ReleaseBuffer(testing::_, testing::_))
      .WillRepeatedly(Return(0));
  c_delegate->first_ref_time_ = base::TimeTicks::Now();
  c_delegate->OnBufferAvailable(surface, buffer, roration_info);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OnBufferAvailable client is nullptr"),
            std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, OnBufferAvailable6) {
  testing::internal::CaptureStderr();
  auto surface = std::make_shared<CameraSurfaceAdapterMock>();
  auto buffer = std::make_shared<CameraSurfaceBufferAdapterMock>();
  auto roration_info = std::make_shared<CameraRotationInfoAdapterMock>();
  auto client = std::make_unique<ClientMock>();
  c_delegate->client_ = nullptr;
  EXPECT_CALL(*roration_info, GetRotation()).WillRepeatedly(Return(90));
  EXPECT_CALL(*roration_info, GetIsFlipY()).WillRepeatedly(Return(false));
  EXPECT_CALL(*buffer, GetBufferAddr()).WillRepeatedly(Return(nullptr));
  EXPECT_CALL(*buffer, GetSize()).WillRepeatedly(Return(0));
  EXPECT_CALL(*surface, ReleaseBuffer(testing::_, testing::_))
      .WillRepeatedly(Return(1));
  c_delegate->first_ref_time_ = base::TimeTicks::Now();
  c_delegate->OnBufferAvailable(surface, buffer, roration_info);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OnBufferAvailable ReleaseBuffer failed"),
            std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, GetPhotoState) {
  bool flag = false;
  c_delegate->capture_format_.frame_size = gfx::Size(1280, 720);
  c_delegate->is_capturing_ = false;
  c_delegate->GetPhotoState(base::BindLambdaForTesting(
      [&flag](mojom::PhotoStatePtr) { flag = true; }));
  EXPECT_FALSE(flag);
  c_delegate->is_capturing_ = true;
  c_delegate->GetPhotoState(base::BindLambdaForTesting(
      [&flag](mojom::PhotoStatePtr) { flag = true; }));
  EXPECT_TRUE(flag);
}

TEST_F(OHOSCaptureDelegateTest, SetPhotoOptions) {
  bool flag = false;
  auto settings = media::mojom::PhotoSettings::New();
  c_delegate->is_capturing_ = false;
  c_delegate->SetPhotoOptions(
      std::move(settings),
      base::BindLambdaForTesting([&flag](bool) { flag = true; }));
  EXPECT_FALSE(flag);
  c_delegate->is_capturing_ = true;
  c_delegate->SetPhotoOptions(
      std::move(settings),
      base::BindLambdaForTesting([&flag](bool) { flag = true; }));
  EXPECT_TRUE(flag);
}

TEST_F(OHOSCaptureDelegateTest, StopStream) {
  c_delegate->is_capturing_ = false;
  EXPECT_FALSE(c_delegate->StopStream());
  c_delegate->is_capturing_ = true;
  EXPECT_TRUE(c_delegate->StopStream());
}

TEST_F(OHOSCaptureDelegateTest, GetFocusState) {
  mojom::PhotoStatePtr photo_capabilities = mojo::CreateEmptyPhotoState();
  c_delegate->GetFocusState(photo_capabilities);
  int size = photo_capabilities->supported_focus_modes.size();
  EXPECT_EQ(size, 0);
  EXPECT_EQ(photo_capabilities->current_focus_mode, MeteringMode::MANUAL);
}

}  // namespace media
