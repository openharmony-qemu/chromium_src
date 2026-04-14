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

#include "media/base/video_frame.h"
#include "gtest/gtest.h"
#define private public
#include "media/capture/video/ohos/video_capture_device_ohos.h"
#undef private

#include <utility>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "base/run_loop.h"
#include "base/functional/bind.h"
#include "base/task/single_thread_task_runner.h"
#include "build/build_config.h"
#include "media/capture/video/blob_utils.h"
#include "media/capture/video/ohos/ohos_capture_delegate.h"
#include "ohos_adapter_helper.h"
#include "third_party/libyuv/include/libyuv.h"
#include "video_capture_common_ohos.h"
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

using namespace testing;

namespace media {
class OhosAdapterHelperMock;
class CameraManagerAdapterMock;

class SingleThreadTaskRunnerMock : public base::SingleThreadTaskRunner {};

class VideoCaptureParamsMock : public VideoCaptureParams {};

class ClientMock : public VideoCaptureDevice::Client {
 public:
  MOCK_METHOD(void, OnCaptureConfigurationChanged, (), (override));
  MOCK_METHOD(void, OnIncomingCapturedData,
              (const uint8_t *data, int length, const VideoCaptureFormat &frame_format,
              const gfx::ColorSpace &color_space,
              int clockwise_rotation, bool flip_y, base::TimeTicks reference_time, base::TimeDelta timestamp,
              std::optional<base::TimeTicks> capture_begin_timestamp, int frame_feedback_id),
              (override));
  MOCK_METHOD(void, OnIncomingCapturedData,
              (const uint8_t *data, int length, const VideoCaptureFormat &frame_format,
              const gfx::ColorSpace &color_space,
              int clockwise_rotation, bool flip_y, base::TimeTicks reference_time, base::TimeDelta timestamp,
              std::optional<base::TimeTicks> capture_begin_timestamp),
              ());
  MOCK_METHOD(void, OnIncomingCapturedGfxBuffer,
              (gfx::GpuMemoryBuffer * buffer, const VideoCaptureFormat &frame_format, int clockwise_rotation,
              base::TimeTicks reference_time, base::TimeDelta timestamp,
              std::optional<base::TimeTicks> capture_begin_timestamp, int frame_feedback_id),
              (override));
  MOCK_METHOD(void, OnIncomingCapturedGfxBuffer,
              (gfx::GpuMemoryBuffer * buffer, const VideoCaptureFormat &frame_format, int clockwise_rotation,
              base::TimeTicks reference_time, base::TimeDelta timestamp,
              std::optional<base::TimeTicks> capture_begin_timestamp),
              ());
  MOCK_METHOD(void, OnIncomingCapturedExternalBuffer,
              (CapturedExternalVideoBuffer buffer, base::TimeTicks reference_time, base::TimeDelta timestamp,
              std::optional<base::TimeTicks> capture_begin_timestamp, const gfx::Rect &visible_rect),
              (override));
  MOCK_METHOD(Client::ReserveResult, ReserveOutputBuffer,
              (const gfx::Size &dimensions, VideoPixelFormat format,
              int frame_feedback_id, Client::Buffer *buffer,
              int *require_new_buffer_id, int *retire_old_buffer_id),
              (override));
  MOCK_METHOD(void, OnIncomingCapturedBuffer,
              (Client::Buffer buffer, const VideoCaptureFormat &format, base::TimeTicks reference_time,
              base::TimeDelta timestamp, std::optional<base::TimeTicks> capture_begin_timestamp),
              (override));
  MOCK_METHOD(void, OnIncomingCapturedBufferExt,
              (Client::Buffer buffer, const VideoCaptureFormat &format, const gfx::ColorSpace &color_space,
              base::TimeTicks reference_time, base::TimeDelta timestamp,
              std::optional<base::TimeTicks> capture_begin_timestamp, gfx::Rect visible_rect,
              const VideoFrameMetadata &additional_metadata),
              (override));
  MOCK_METHOD(void, OnError, (VideoCaptureError error, const base::Location &from_here, const std::string &reason),
              (override));
  MOCK_METHOD(void, OnFrameDropped, (VideoCaptureFrameDropReason reason), (override));
  MOCK_METHOD(void, OnLog, (const std::string &message), (override));
  MOCK_METHOD(double, GetBufferPoolUtilization, (), (const, override));
  MOCK_METHOD(void, OnStarted, (), (override));
};

class OhosAdapterHelperMock : public OhosAdapterHelper {
 public:
  MOCK_METHOD(std::unique_ptr<AafwkAppMgrClientAdapter>, CreateAafwkAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<PowerMgrClientAdapter>, CreatePowerMgrClientAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<DisplayManagerAdapter>, CreateDisplayMgrAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<BatteryMgrClientAdapter>, CreateBatteryClientAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<NetConnectAdapter>, CreateNetConnectAdapter, (), (override));
  MOCK_METHOD(OhosWebDataBaseAdapter &, GetOhosWebDataBaseAdapterInstance, (), (override));
  MOCK_METHOD(PasteBoardClientAdapter &, GetPasteBoard, (), (override));
  MOCK_METHOD(std::unique_ptr<AudioRendererAdapter>, CreateAudioRendererAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<AudioCapturerAdapter>, CreateAudioCapturerAdapter, (), (override));
  MOCK_METHOD(AudioSystemManagerAdapter &, GetAudioSystemManager, (), (override));
  MOCK_METHOD(OhosWebPermissionDataBaseAdapter &, GetWebPermissionDataBaseInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<MMIAdapter>, CreateMMIAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<SocPerfClientAdapter>, CreateSocPerfClientAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<OhosResourceAdapter>, GetResourceAdapter, (const std::string &hapPath), (override));
  MOCK_METHOD(SystemPropertiesAdapter &, GetSystemPropertiesInstance, (), (override));
  MOCK_METHOD(VSyncAdapter &, GetVSyncAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<OhosInitWebAdapter>, GetInitWebAdapter, (), (override));
  MOCK_METHOD(KeystoreAdapter &, GetKeystoreAdapterInstance, (), (override));
  MOCK_METHOD(EnterpriseDeviceManagementAdapter &, GetEnterpriseDeviceManagementInstance, (), (override));
  MOCK_METHOD(DatashareAdapter &, GetDatashareInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<IMFAdapter>, CreateIMFAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<CertManagerAdapter>, GetRootCertDataAdapter, (), (override));
  MOCK_METHOD(AccessTokenAdapter &, GetAccessTokenAdapterInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<EventHandlerAdapter>, GetEventHandlerAdapter, (), (override));
  MOCK_METHOD(PrintManagerAdapter &, GetPrintManagerInstance, (), (override));
  MOCK_METHOD(std::unique_ptr<IConsumerSurfaceAdapter>, CreateConsumerSurfaceAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<PlayerAdapter>, CreatePlayerAdapter, (), (override));
  MOCK_METHOD(WindowAdapter &, GetWindowAdapterInstance, (), (override));
  MOCK_METHOD(HiSysEventAdapter &, GetHiSysEventAdapterInstance, (), (override));
  MOCK_METHOD(HiTraceAdapter &, GetHiTraceAdapterInstance, (), (override));
  MOCK_METHOD(NetProxyAdapter &, GetNetProxyInstance, (), (override));
  MOCK_METHOD(CameraManagerAdapter &, GetCameraManagerAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<ScreenCaptureAdapter>, CreateScreenCaptureAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<DateTimeFormatAdapter>, CreateDateTimeFormatAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<MediaCodecDecoderAdapter>, CreateMediaCodecDecoderAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<NativeImageAdapter>, CreateNativeImageAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<MediaCodecAdapter>, CreateMediaCodecEncoderAdapter, (), (override));
  MOCK_METHOD(MediaCodecListAdapter &, GetMediaCodecListAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<FlowbufferAdapter>, CreateFlowbufferAdapter, (), (override));
  MOCK_METHOD(std::shared_ptr<MediaAVSessionAdapter>, CreateMediaAVSessionAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<OhosImageDecoderAdapter>, CreateOhosImageDecoderAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<SensorAdapter>, CreateSensorAdapter, (), (override));
  MOCK_METHOD(void, SetArkWebCoreHapPathOverride, (const std::string &hapPath), (override));
  MOCK_METHOD(OhosNativeBufferAdapter &, GetOhosNativeBufferAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<AudioCodecDecoderAdapter>, CreateAudioCodecDecoderAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<DrmAdapter>, CreateDrmAdapter, (), (override));
  MOCK_METHOD(OhosDrawingTextFontAdapter &, GetOhosDrawingTextFontAdapter, (), (override));
  MOCK_METHOD(OhosDrawingTextTypographyAdapter &, GetOhosDrawingTextTypographyAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<MigrationManagerAdapter>, CreateMigrationMgrAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<ScreenlockManagerAdapter>, CreateScreenlockManagerAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<CertManagerAdapter>, GetCertManagerAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<NetConfigAdapter>, GetNetConfigAdapter, (), (override));
  MOCK_METHOD(std::unique_ptr<NetEventAdapter>, CreateNetEventAdapter, (), (override));
  MOCK_METHOD(ColorPickerAdapter &, GetColorPickerAdapter, (), (override));
  MOCK_METHOD(HiAppeventAdapter &, GetHiAppeventAdapterInstance, (), (override));
};

class CameraSurfaceBufferAdapterMock : public CameraSurfaceBufferAdapter {
 public:
  MOCK_METHOD(int32_t, GetFileDescriptor, (), (override));
  MOCK_METHOD(int32_t, GetWidth, (), (override));
  MOCK_METHOD(int32_t, GetHeight, (), (override));
  MOCK_METHOD(int32_t, GetStride, (), (override));
  MOCK_METHOD(int32_t, GetFormat, (), (override));
  MOCK_METHOD(uint32_t, GetSize, (), (override));
  MOCK_METHOD(uint8_t *, GetBufferAddr, (), (override));
};

class CameraRotationInfoAdapterMock : public CameraRotationInfoAdapter {
 public:
  MOCK_METHOD(int32_t, GetRotation, (), (override));
  MOCK_METHOD(bool, GetIsFlipX, (), (override));
  MOCK_METHOD(bool, GetIsFlipY, (), (override));
};

class CameraSurfaceAdapterMock : public CameraSurfaceAdapter {
 public:
  MOCK_METHOD(
    int32_t, ReleaseBuffer, (std::shared_ptr<CameraSurfaceBufferAdapter> buffer, int32_t fence), (override));
};

class CameraManagerAdapterMock : public CameraManagerAdapter {
 public:
  MOCK_METHOD(int32_t, Create, (std::shared_ptr<CameraStatusCallbackAdapter> cameraStatusCallback), (override));
  MOCK_METHOD(std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>>, GetDevicesInfo, (), (override));
  MOCK_METHOD(int32_t, ReleaseCameraManger, (), (override));
  MOCK_METHOD(int32_t, GetExposureModes, (std::vector<ExposureModeAdapter> & exposureModesAdapter), (override));
  MOCK_METHOD(int32_t, GetCurrentExposureMode, (ExposureModeAdapter & exposureModeAdapter), (override));
  MOCK_METHOD(std::shared_ptr<VideoCaptureRangeAdapter>, GetCaptionRangeById, (RangeIDAdapter rangeId), (override));
  MOCK_METHOD(bool, IsFocusModeSupported, (FocusModeAdapter focusMode), (override));
  MOCK_METHOD(FocusModeAdapter, GetCurrentFocusMode, (), (override));
  MOCK_METHOD(bool, IsFlashModeSupported, (FlashModeAdapter flashMode), (override));
  MOCK_METHOD(int32_t, RestartSession, (), (override));
  MOCK_METHOD(int32_t, StopSession, (CameraStopType stopType), (override));
  MOCK_METHOD(CameraStatusAdapter, GetCameraStatus, (), (override));
  MOCK_METHOD(bool, IsExistCaptureTask, (), (override));
  MOCK_METHOD(int32_t, StartStream,
              (const std::string &deviceId, const std::shared_ptr<VideoCaptureParamsAdapter> captureParams,
              std::shared_ptr<CameraBufferListenerAdapter> listener),
              (override));
  MOCK_METHOD(void, SetForegroundFlag, (bool isForeground), (override));
  MOCK_METHOD(void, SetCameraStatus, (CameraStatusAdapter status), (override));
  MOCK_METHOD(std::string, GetCurrentDeviceId, (), (override));
  MOCK_METHOD(int32_t, GetSupportedFlashModes, (std::vector<FlashModeAdapter>& flashModesAdapter), (override));
  MOCK_METHOD(bool, HasFlash, (), (override));
  MOCK_METHOD(FlashModeAdapter, GetFlashMode, (), (override));
  MOCK_METHOD(int32_t, SetFlashMode, (FlashModeAdapter flashMode), (override));
};

class VideoCaptureDeviceOhosTest : public testing::Test {
 public:
  void SetUp() {
    ohos_adapter_helper_mock = std::make_unique<OhosAdapterHelperMock>();
    camera_manager_adapter_mock = std::make_unique<CameraManagerAdapterMock>();
    ON_CALL(*ohos_adapter_helper_mock, GetCameraManagerAdapter())
        .WillByDefault(ReturnRef(*camera_manager_adapter_mock));
    scoped_refptr<SingleThreadTaskRunnerMock> capture_stask_runner;
    VideoCaptureDeviceDescriptor device_descriptor;
    c_device = std::make_shared<VideoCaptureDeviceOHOS>(device_descriptor);
  }
  void TearDown() {
    c_device = nullptr;
    ohos_adapter_helper_mock.reset();
    camera_manager_adapter_mock.reset();
  }
  void AllocateAndStart(const VideoCaptureParams& params, std::unique_ptr<VideoCaptureDevice::Client> client) {
    c_device->AllocateAndStart(params, std::move(client));
  }

  void StopAndDeAllocate() {
    c_device->StopAndDeAllocate();
  }

  void TakePhoto(VideoCaptureDevice::TakePhotoCallback callback) {
    c_device->TakePhoto(std::move(callback));
  }

  void GetPhotoState(VideoCaptureDevice::GetPhotoStateCallback callback) {
    c_device->GetPhotoState(std::move(callback));
  }

  void SetPhotoOptions(mojom::PhotoSettingsPtr settings, VideoCaptureDevice::SetPhotoOptionsCallback callback) {
    c_device->SetPhotoOptions(std::move(settings), std::move(callback));
  }

  void MaybeSuspend() {
    c_device->MaybeSuspend();
  }

  void Resume() {
    c_device->Resume();
  }

  void TransToOHOSCaptrueParams(const VideoCaptureParams& in, VideoCaptureParamsAdapter& out) {
    c_device->TransToOHOSCaptrueParams(in, out);
  }

  std::shared_ptr<VideoCaptureDeviceOHOS> c_device;
  std::unique_ptr<OhosAdapterHelperMock> ohos_adapter_helper_mock;
  std::unique_ptr<CameraManagerAdapterMock> camera_manager_adapter_mock;
  VideoCaptureParamsMock params_;

 protected:
  void SetDelegate(std::unique_ptr<OHOSCaptureDelegate> delegate) {
    c_device->capture_impl_ = std::move(delegate);
  }

  void SetQueue(std::vector<base::OnceClosure> queue) {
    c_device->photo_requests_queue_ = std::move(queue);
  }

  void SetCaptureFormat(VideoCaptureFormat capture_format) {
    c_device->capture_format_ = capture_format;
  }

  void SetCameraManagerAdapter(std::shared_ptr<CameraManagerAdapter> camera_manager_adapter) {
    c_device->camera_manager_adapter_ = std::move(camera_manager_adapter);
  }

  bool IsCaptureThreadRunningForTesting() const {
    return c_device->capture_thread_.IsRunning();
  }
  
  bool HasCaptureImplForTesting() const {
    return c_device->capture_impl_ != nullptr;
  }

  void ThreadStop() const {
    return c_device->capture_thread_.Stop();
  }

  bool QueueSize(uint size) const {
    return c_device->photo_requests_queue_.size() == size;
  }
};

TEST_F(VideoCaptureDeviceOhosTest, AllocateAndStart_01) {
  auto client1 = std::make_unique<ClientMock>();
  c_device->AllocateAndStart(params_, std::move(client1));
  EXPECT_TRUE(IsCaptureThreadRunningForTesting());
  auto client2 = std::make_unique<ClientMock>();
  c_device->AllocateAndStart(params_, std::move(client2));
}

TEST_F(VideoCaptureDeviceOhosTest, AllocateAndStart_02) {
  auto client = std::make_unique<ClientMock>();
  c_device->AllocateAndStart(params_, std::move(client));
  EXPECT_TRUE(HasCaptureImplForTesting());
}

TEST_F(VideoCaptureDeviceOhosTest, StopAndDeAllocate_01) {
  auto client = std::make_unique<ClientMock>();
  c_device->AllocateAndStart(params_, std::move(client));
  EXPECT_TRUE(IsCaptureThreadRunningForTesting());
  c_device->StopAndDeAllocate();
  EXPECT_FALSE(HasCaptureImplForTesting());
  EXPECT_FALSE(IsCaptureThreadRunningForTesting());
  c_device->StopAndDeAllocate();
}

TEST_F(VideoCaptureDeviceOhosTest, TakePhoto) {
  auto client = std::make_unique<ClientMock>();
  c_device->AllocateAndStart(params_, std::move(client));
  EXPECT_TRUE(IsCaptureThreadRunningForTesting());
  auto callback = base::DoNothing();
  c_device->TakePhoto(callback);
  ThreadStop();
  EXPECT_FALSE(IsCaptureThreadRunningForTesting());
  c_device->TakePhoto(callback);
  EXPECT_TRUE(QueueSize(1));
}

TEST_F(VideoCaptureDeviceOhosTest, GetPhotoState) {
  auto client = std::make_unique<ClientMock>();
  c_device->AllocateAndStart(params_, std::move(client));
  EXPECT_TRUE(IsCaptureThreadRunningForTesting());
  auto callback = base::DoNothing();
  c_device->GetPhotoState(callback);
  ThreadStop();
  EXPECT_FALSE(IsCaptureThreadRunningForTesting());
  c_device->GetPhotoState(callback);
  EXPECT_TRUE(QueueSize(1));
}

TEST_F(VideoCaptureDeviceOhosTest, SetPhotoOptions) {
  auto client = std::make_unique<ClientMock>();
  c_device->AllocateAndStart(params_, std::move(client));
  EXPECT_TRUE(IsCaptureThreadRunningForTesting());
  auto settings = media::mojom::PhotoSettings::New();
  auto callback = base::DoNothing();
  c_device->SetPhotoOptions(std::move(settings), callback);
  ThreadStop();
  EXPECT_FALSE(IsCaptureThreadRunningForTesting());
  c_device->SetPhotoOptions(std::move(settings), callback);
  EXPECT_TRUE(QueueSize(1));
}

TEST_F(VideoCaptureDeviceOhosTest, MaybeSuspend) {
  auto client = std::make_unique<ClientMock>();
  c_device->AllocateAndStart(params_, std::move(client));
  EXPECT_TRUE(IsCaptureThreadRunningForTesting());
  c_device->MaybeSuspend();
  ThreadStop();
  EXPECT_FALSE(IsCaptureThreadRunningForTesting());
  c_device->MaybeSuspend();
  EXPECT_TRUE(QueueSize(1));
}

TEST_F(VideoCaptureDeviceOhosTest, Resume) {
  auto client = std::make_unique<ClientMock>();
  c_device->AllocateAndStart(params_, std::move(client));
  EXPECT_TRUE(IsCaptureThreadRunningForTesting());
  c_device->Resume();
  ThreadStop();
  EXPECT_FALSE(IsCaptureThreadRunningForTesting());
  c_device->Resume();
  EXPECT_TRUE(QueueSize(1));
}
}  // namespace media