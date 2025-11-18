/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License") = 0;
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

#ifndef OHOS_ADAPTER_HELPER_MOCK_H
#define OHOS_ADAPTER_HELPER_MOCK_H

#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include <gmock/gmock.h>

namespace OHOS::NWeb {

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
    MOCK_METHOD(std::unique_ptr<OhosResourceAdapter>, GetResourceAdapter, (const std::string& hapPath), (override));
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
    MOCK_METHOD(std::shared_ptr<MediaAVSessionAdapter>, CreateMediaAVSessionAdapter, (), (override));
    MOCK_METHOD(std::unique_ptr<OhosImageDecoderAdapter>, CreateOhosImageDecoderAdapter, (), (override));
    MOCK_METHOD(std::unique_ptr<SensorAdapter>, CreateSensorAdapter, (), (override));
    MOCK_METHOD(void, SetArkWebCoreHapPathOverride, (const std::string& hapPath), (override));
    MOCK_METHOD(OhosNativeBufferAdapter&, GetOhosNativeBufferAdapter, (), (override));
    MOCK_METHOD(std::unique_ptr<AudioCodecDecoderAdapter>, CreateAudioCodecDecoderAdapter, (), (override));
    MOCK_METHOD(std::unique_ptr<DrmAdapter>, CreateDrmAdapter, (), (override));
    MOCK_METHOD(OhosDrawingTextFontAdapter&, GetOhosDrawingTextFontAdapter, (), (override));
    MOCK_METHOD(OhosDrawingTextTypographyAdapter&, GetOhosDrawingTextTypographyAdapter, (), (override));
    MOCK_METHOD(std::unique_ptr<MigrationManagerAdapter>, CreateMigrationMgrAdapter, (), (override));
    MOCK_METHOD(std::unique_ptr<ScreenlockManagerAdapter>, CreateScreenlockManagerAdapter, (), (override));
    MOCK_METHOD(std::unique_ptr<CertManagerAdapter>, GetCertManagerAdapter, (), (override));
    MOCK_METHOD(std::unique_ptr<NetConfigAdapter>, GetNetConfigAdapter, (), (override));
    MOCK_METHOD(std::unique_ptr<NetEventAdapter>, CreateNetEventAdapter, (), (override));
    MOCK_METHOD(ColorPickerAdapter&, GetColorPickerAdapter, (), (override));
    MOCK_METHOD(HiAppeventAdapter&, GetHiAppeventAdapterInstance, (), (override));
};

} // namespace OHOS::NWeb

#endif // OHOS_ADAPTER_HELPER_MOCK_H
