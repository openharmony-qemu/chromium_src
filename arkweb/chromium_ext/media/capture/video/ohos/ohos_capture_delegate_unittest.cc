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

#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include "media/capture/video/ohos/ohos_capture_delegate.h"

#include <utility>
#include <cstddef>
#include <cstdint>
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
        (const uint8_t *data, int length, const VideoCaptureFormat &frame_format, const gfx::ColorSpace &color_space,
            int clockwise_rotation, bool flip_y, base::TimeTicks reference_time, base::TimeDelta timestamp,
            std::optional<base::TimeTicks> capture_begin_timestamp, int frame_feedback_id),
        (override));
    MOCK_METHOD(void, OnIncomingCapturedData,
        (const uint8_t *data, int length, const VideoCaptureFormat &frame_format, const gfx::ColorSpace &color_space,
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
        (const gfx::Size &dimensions, VideoPixelFormat format, int frame_feedback_id, Client::Buffer *buffer,
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

void MockTakePhotoCallback(mojo::StructPtr<media::mojom::Blob> blob)
{
    EXPECT_TRUE(blob.is_null());
}

void MockGetPhotoStateCallback(mojo::StructPtr<media::mojom::PhotoState> photo_state)
{
    EXPECT_FALSE(photo_state.is_null());
    EXPECT_EQ(photo_state->height->max, 720);
    EXPECT_EQ(photo_state->width->max, 1280);
}

class OHOSCaptureDelegateTest : public testing::Test {
public:
    void SetUp()
    {
        ohos_adapter_helper_mock = std::make_unique<OhosAdapterHelperMock>();
        camera_manager_adapter_mock = std::make_unique<CameraManagerAdapterMock>();
        ON_CALL(*ohos_adapter_helper_mock, GetCameraManagerAdapter())
            .WillByDefault(ReturnRef(*camera_manager_adapter_mock));
        VideoCaptureDeviceDescriptor device_descriptor;
        scoped_refptr<SingleThreadTaskRunnerMock> capture_stask_runner;
        VideoCaptureParamsMock capture_params;
        c_delegate = std::make_shared<OHOSCaptureDelegate>(device_descriptor, capture_stask_runner, capture_params);
    }
    void TearDown()
    {
        c_delegate = nullptr;
        ohos_adapter_helper_mock.reset();
        camera_manager_adapter_mock.reset();
    }
    bool StartStream()
    {
        return c_delegate->StartStream();
    }
    void DoCapture()
    {
        c_delegate->DoCapture();
    }
    bool StopStream()
    {
        return c_delegate->StopStream();
    }
    void SetErrorState(VideoCaptureError error, const base::Location &from_here, const std::string &reason)
    {
        c_delegate->SetErrorState(error, from_here, reason);
    }
    int TransToOHOSCaptrueParams(const VideoCaptureParams &in, std::shared_ptr<VideoCaptureParamsAdapterImpl> &out)
    {
        return c_delegate->TransToOHOSCaptrueParams(in, out);
    }
    int GetUsableExposureMode(ExposureModeAdapter &exposure_mode_adapter, MeteringMode &exposure_mode)
    {
        return c_delegate->GetUsableExposureMode(exposure_mode_adapter, exposure_mode);
    }
    void GetExposureState(mojom::PhotoStatePtr &photo_capabilities)
    {
        c_delegate->GetExposureState(photo_capabilities);
    }
    mojom::RangePtr RetrieveUserControlRange(RangeIDAdapter rangeID)
    {
        return c_delegate->RetrieveUserControlRange(rangeID);
    }
    void GetFocusState(mojom::PhotoStatePtr &photo_capabilities)
    {
        c_delegate->GetFocusState(photo_capabilities);
    }
    void GetFlashState(mojom::PhotoStatePtr &photo_capabilities)
    {
        c_delegate->GetFlashState(photo_capabilities);
    }
    int GetCameraRotation(int surface_rotation)
    {
        return c_delegate->GetCameraRotation(surface_rotation);
    }
    MeteringMode GetCurrentExposureMode(ExposureModeAdapter &exposure_mode_adapter)
    {
        return c_delegate->GetCurrentExposureMode(exposure_mode_adapter);
    }

    std::shared_ptr<OHOSCaptureDelegate> c_delegate;
    std::unique_ptr<OhosAdapterHelperMock> ohos_adapter_helper_mock;
    std::unique_ptr<CameraManagerAdapterMock> camera_manager_adapter_mock;

protected:
    void SetCaptureFormat(VideoCaptureFormat capture_format)
    {
        c_delegate->capture_format_ = capture_format;
    }
    void SetClient(std::unique_ptr<VideoCaptureDevice::Client> client)
    {
        c_delegate->client_ = std::move(client);
    }
    void SetIsCapturing(bool is_capturing)
    {
        c_delegate->is_capturing_ = is_capturing;
    }
    void SetTimeoutCount(int timeout_count)
    {
        c_delegate->timeout_count_ = timeout_count;
    }
    void SetFirstRefTime(base::TimeTicks first_ref_time)
    {
        c_delegate->first_ref_time_ = first_ref_time;
    }
    void SetRotation(int rotation)
    {
        c_delegate->rotation_ = rotation;
    }
    void SetCaptureFormatFrameSize(gfx::Size frame_size)
    {
        c_delegate->capture_format_.frame_size = frame_size;
    }

    VideoCaptureFormat GetVideoCaptureFormat()
    {
        return c_delegate->capture_format_;
    }
    std::unique_ptr<VideoCaptureDevice::Client> GetClient()
    {
        return std::move(c_delegate->client_);
    }
    base::queue<VideoCaptureDevice::TakePhotoCallback> GetTakePhoteCallbacks()
    {
        return std::move(c_delegate->take_photo_callbacks_);
    }
    bool GetIsCapturing()
    {
        return c_delegate->is_capturing_;
    }
    base::TimeTicks GetFirstRefTime()
    {
        return c_delegate->first_ref_time_;
    }
};

TEST_F(OHOSCaptureDelegateTest, AllocateAndStart_001)
{
    auto cameraManagerAdapterMock = std::make_shared<CameraManagerAdapterMock>();
    auto client = std::make_unique<ClientMock>();
    SetClient(std::move(client));
    EXPECT_CALL(*cameraManagerAdapterMock, GetCameraStatus())
      .WillRepeatedly(Return(CameraStatusAdapter::APPEAR));
    c_delegate->AllocateAndStart(std::move(client));
}

TEST_F(OHOSCaptureDelegateTest, StopAndDeAllocate_002)
{
    SetClient(std::unique_ptr<VideoCaptureDevice::Client>());
    c_delegate->StopAndDeAllocate();
    EXPECT_EQ(GetClient(), nullptr);
}

TEST_F(OHOSCaptureDelegateTest, MaybeSuspend_ShouldReturn_WhenIsCapturingIsFalse)
{
    testing::internal::CaptureStderr();
    SetIsCapturing(false);
    c_delegate->MaybeSuspend();
    std::string log_output1 = testing::internal::GetCapturedStderr();
    EXPECT_EQ(log_output1.find("MaybeSuspend"), std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, Resume_ShouldReturn_WhenIsCapturingIsFalse)
{
    testing::internal::CaptureStderr();
    SetIsCapturing(false);
    c_delegate->Resume();
    std::string log_output1 = testing::internal::GetCapturedStderr();
    EXPECT_EQ(log_output1.find("*"), std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, StartStream)
{
    auto cameraManagerAdapterMock = std::make_shared<CameraManagerAdapterMock>();
    EXPECT_CALL(*cameraManagerAdapterMock, GetCameraStatus())
      .WillRepeatedly(Return(CameraStatusAdapter::UNAVAILABLE));
    auto ret = StartStream();
    EXPECT_EQ(ret, false);
}

TEST_F(OHOSCaptureDelegateTest, StopStream_ShouldReturnFalse_WhenIsCapturingIsFalse)
{
    SetIsCapturing(false);
    auto ret = StopStream();
    EXPECT_EQ(ret, false);
}

TEST_F(OHOSCaptureDelegateTest, StopStream_ShouldReturnTrue_WhenIsCapturingIsTrue)
{
    SetIsCapturing(true);
    auto ret = StopStream();
    EXPECT_EQ(ret, true);
}

TEST_F(OHOSCaptureDelegateTest, SetErrorState)
{
    VideoCaptureError error = VideoCaptureError::kAndroidApi1CameraErrorCallbackReceived;
    base::Location from_here = base::Location::Current();
    std::string reason = "Test error reason";
    ClientMock *client = new ClientMock();
    EXPECT_CALL(*client, OnError(testing::_, testing::_, testing::_)).WillOnce(testing::Return());
    SetClient(std::unique_ptr<VideoCaptureDevice::Client>(client));
    SetErrorState(error, from_here, reason);
}

TEST_F(OHOSCaptureDelegateTest, TransToOHOSCaptrueParams)
{
    const VideoCaptureParams in;
    std::shared_ptr<VideoCaptureParamsAdapterImpl> out = nullptr;
    auto ret = TransToOHOSCaptrueParams(in, out);
    EXPECT_EQ(ret, -1);
}

TEST_F(OHOSCaptureDelegateTest, GetUsableExposureMode)
{
    ExposureModeAdapter exposure_mode_adapter = ExposureModeAdapter::EXPOSURE_MODE_LOCKED;
    MeteringMode exposure_mode = MeteringMode::NONE;
    auto ret = GetUsableExposureMode(exposure_mode_adapter, exposure_mode);
    EXPECT_EQ(ret, -1);
}

TEST_F(OHOSCaptureDelegateTest, GetCurrentExposureMode)
{
    ExposureModeAdapter exposure_mode_adapter = ExposureModeAdapter::EXPOSURE_MODE_CONTINUOUS_AUTO;
    auto ret = GetCurrentExposureMode(exposure_mode_adapter);
    EXPECT_EQ(ret, MeteringMode::CONTINUOUS);
}

TEST_F(OHOSCaptureDelegateTest, GetFocusState001)
{
    ON_CALL(*camera_manager_adapter_mock, IsFocusModeSupported(FocusModeAdapter::FOCUS_MODE_MANUAL))
        .WillByDefault(Return(true));
    ON_CALL(*camera_manager_adapter_mock, IsFocusModeSupported(FocusModeAdapter::FOCUS_MODE_CONTINUOUS_AUTO))
        .WillByDefault(Return(true));
    ON_CALL(*camera_manager_adapter_mock, GetCurrentFocusMode())
        .WillByDefault(Return(FocusModeAdapter::FOCUS_MODE_MANUAL));
    mojom::PhotoStatePtr photo_capabilities = mojo::CreateEmptyPhotoState();
    GetFocusState(photo_capabilities);
    EXPECT_EQ(photo_capabilities->current_focus_mode, MeteringMode::MANUAL);
}

TEST_F(OHOSCaptureDelegateTest, GetFlashState001)
{
    ON_CALL(*camera_manager_adapter_mock, IsFlashModeSupported(FlashModeAdapter::FLASH_MODE_CLOSE))
        .WillByDefault(Return(false));
    mojom::PhotoStatePtr photo_capabilities = mojom::PhotoState::New();
    GetFlashState(photo_capabilities);
    EXPECT_TRUE(std::find(photo_capabilities->fill_light_mode.begin(),
                    photo_capabilities->fill_light_mode.end(),
                    mojom::FillLightMode::OFF) == photo_capabilities->fill_light_mode.end());
}

TEST_F(OHOSCaptureDelegateTest, GetFlashState002)
{
    ON_CALL(*camera_manager_adapter_mock, IsFlashModeSupported(FlashModeAdapter::FLASH_MODE_OPEN))
        .WillByDefault(Return(false));
    mojom::PhotoStatePtr photo_capabilities = mojom::PhotoState::New();
    GetFlashState(photo_capabilities);
    EXPECT_TRUE(std::find(photo_capabilities->fill_light_mode.begin(),
                    photo_capabilities->fill_light_mode.end(),
                    mojom::FillLightMode::FLASH) == photo_capabilities->fill_light_mode.end());
}

TEST_F(OHOSCaptureDelegateTest, GetFlashState003)
{
    ON_CALL(*camera_manager_adapter_mock, IsFlashModeSupported(FlashModeAdapter::FLASH_MODE_AUTO))
        .WillByDefault(Return(false));
    mojom::PhotoStatePtr photo_capabilities = mojom::PhotoState::New();
    GetFlashState(photo_capabilities);
    EXPECT_TRUE(std::find(photo_capabilities->fill_light_mode.begin(),
                    photo_capabilities->fill_light_mode.end(),
                    mojom::FillLightMode::AUTO) == photo_capabilities->fill_light_mode.end());
}

TEST_F(OHOSCaptureDelegateTest, Resume)
{
    SetIsCapturing(false);
    c_delegate->Resume();
    EXPECT_FALSE(GetIsCapturing());
    SetIsCapturing(true);
    c_delegate->Resume();
    EXPECT_TRUE(GetIsCapturing());
}

TEST_F(OHOSCaptureDelegateTest, StartStream_ShouldReturnFalse_WhenStatusIsUnAvailable)
{
    testing::internal::CaptureStderr();
    OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().SetCameraStatus(CameraStatusAdapter::UNAVAILABLE);
    bool result = StartStream();
    EXPECT_FALSE(result);
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("camera is not closed"), std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, StartStream_ShouldReturnFalse_WhenNotMatchedParam)
{
    testing::internal::CaptureStderr();
    OhosAdapterHelper::GetInstance().GetCameraManagerAdapter().SetCameraStatus(CameraStatusAdapter::AVAILABLE);
    bool result = StartStream();
    EXPECT_FALSE(result);
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_EQ(log_output.find("can not find matched parameter"), std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, GetUsableExposureMode1)
{
    ExposureModeAdapter exposure_mode_adapter = ExposureModeAdapter::EXPOSURE_MODE_UNSUPPORTED;
    MeteringMode exposure_mode = MeteringMode::CONTINUOUS;
    auto ret = GetUsableExposureMode(exposure_mode_adapter, exposure_mode);
    EXPECT_EQ(ret, 0);
}

TEST_F(OHOSCaptureDelegateTest, GetUsableExposureMode2)
{
    ExposureModeAdapter exposure_mode_adapter = ExposureModeAdapter::EXPOSURE_MODE_LOCKED;
    MeteringMode exposure_mode = MeteringMode::NONE;
    auto ret = GetUsableExposureMode(exposure_mode_adapter, exposure_mode);
    EXPECT_EQ(ret, -1);
}

TEST_F(OHOSCaptureDelegateTest, GetCurrentExposureMode1)
{
    ExposureModeAdapter exposure_mode_adapter = ExposureModeAdapter::EXPOSURE_MODE_LOCKED;
    auto ret = GetCurrentExposureMode(exposure_mode_adapter);
    EXPECT_EQ(ret, MeteringMode::SINGLE_SHOT);
}

TEST_F(OHOSCaptureDelegateTest, GetCurrentExposureMode2)
{
    testing::internal::CaptureStderr();
    int32_t a = 5;
    ExposureModeAdapter b = (ExposureModeAdapter)a;
    auto ret = GetCurrentExposureMode(b);
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("not found."), std::string::npos);
    EXPECT_EQ(ret, MeteringMode::NONE);
}

TEST_F(OHOSCaptureDelegateTest, RetrieveUserControlRange)
{
    testing::internal::CaptureStderr();
    RangeIDAdapter rangeID = RangeIDAdapter::RANGE_ID_EXP_COMPENSATION;
    mojom::RangePtr rangePtr = RetrieveUserControlRange(rangeID);
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("get current caption range failed"), std::string::npos);
    mojom::RangePtr capability = mojom::Range::New();
    EXPECT_EQ(rangePtr, capability);
}

TEST_F(OHOSCaptureDelegateTest, OnBufferAvailable1)
{
    std::shared_ptr<CameraSurfaceAdapter> surface = nullptr;
    std::shared_ptr<CameraSurfaceBufferAdapterMock> buffer = nullptr;
    std::shared_ptr<CameraRotationInfoAdapterMock> roration_info = nullptr;
    SetClient(nullptr);
    c_delegate->OnBufferAvailable(surface, buffer, roration_info);
    base::TimeTicks null_time;
    EXPECT_NE(GetFirstRefTime(), null_time);
}

TEST_F(OHOSCaptureDelegateTest, OnBufferAvailable2)
{
    testing::internal::CaptureStderr();
    std::shared_ptr<CameraSurfaceAdapter> surface = nullptr;
    auto buffer = std::make_shared<CameraSurfaceBufferAdapterMock>();
    auto roration_info = std::make_shared<CameraRotationInfoAdapterMock>();
    c_delegate->OnBufferAvailable(surface, buffer, roration_info);
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("OnBufferAvailable params is nullptr"), std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, OnBufferAvailable3)
{
    testing::internal::CaptureStderr();
    std::shared_ptr<CameraSurfaceBufferAdapter> buffer = nullptr;
    auto surface = std::make_shared<CameraSurfaceAdapterMock>();
    auto roration_info = std::make_shared<CameraRotationInfoAdapterMock>();
    c_delegate->OnBufferAvailable(surface, buffer, roration_info);
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("OnBufferAvailable params is nullptr"), std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, OnBufferAvailable4)
{
    testing::internal::CaptureStderr();
    std::shared_ptr<CameraRotationInfoAdapter> roration_info = nullptr;
    auto surface = std::make_shared<CameraSurfaceAdapterMock>();
    auto buffer = std::make_shared<CameraSurfaceBufferAdapterMock>();
    c_delegate->OnBufferAvailable(surface, buffer, roration_info);
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("OnBufferAvailable params is nullptr"), std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, OnBufferAvailable5)
{
    testing::internal::CaptureStderr();
    auto surface = std::make_shared<CameraSurfaceAdapterMock>();
    auto buffer = std::make_shared<CameraSurfaceBufferAdapterMock>();
    auto roration_info = std::make_shared<CameraRotationInfoAdapterMock>();
    SetClient(nullptr);
    EXPECT_CALL(*roration_info, GetRotation()).WillRepeatedly(Return(90));
    EXPECT_CALL(*roration_info, GetIsFlipY()).WillRepeatedly(Return(false));
    EXPECT_CALL(*buffer, GetBufferAddr()).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(*buffer, GetSize()).WillRepeatedly(Return(0));
    EXPECT_CALL(*surface, ReleaseBuffer(testing::_, testing::_)).WillRepeatedly(Return(0));
    SetFirstRefTime(base::TimeTicks::Now());
    c_delegate->OnBufferAvailable(surface, buffer, roration_info);
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("OnBufferAvailable client is nullptr"), std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, OnBufferAvailable6)
{
    testing::internal::CaptureStderr();
    auto surface = std::make_shared<CameraSurfaceAdapterMock>();
    auto buffer = std::make_shared<CameraSurfaceBufferAdapterMock>();
    auto roration_info = std::make_shared<CameraRotationInfoAdapterMock>();
    auto client = std::make_unique<ClientMock>();
    SetClient(nullptr);
    EXPECT_CALL(*roration_info, GetRotation()).WillRepeatedly(Return(90));
    EXPECT_CALL(*roration_info, GetIsFlipY()).WillRepeatedly(Return(false));
    EXPECT_CALL(*buffer, GetBufferAddr()).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(*buffer, GetSize()).WillRepeatedly(Return(0));
    EXPECT_CALL(*surface, ReleaseBuffer(testing::_, testing::_)).WillRepeatedly(Return(1));
    SetFirstRefTime(base::TimeTicks::Now());
    c_delegate->OnBufferAvailable(surface, buffer, roration_info);
    std::string log_output = testing::internal::GetCapturedStderr();
    EXPECT_NE(log_output.find("OnBufferAvailable ReleaseBuffer failed"), std::string::npos);
}

TEST_F(OHOSCaptureDelegateTest, SetPhotoOptions)
{
    bool flag = false;
    auto settings = media::mojom::PhotoSettings::New();
    SetIsCapturing(false);
    c_delegate->SetPhotoOptions(std::move(settings), base::BindLambdaForTesting([&flag](bool) { flag = true; }));
    EXPECT_FALSE(flag);
    SetIsCapturing(true);
    c_delegate->SetPhotoOptions(std::move(settings), base::BindLambdaForTesting([&flag](bool) { flag = true; }));
    EXPECT_TRUE(flag);
}
}  // namespace media
