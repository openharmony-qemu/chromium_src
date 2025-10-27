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
#ifndef OHOS_ADAPTER_NDK_MOCK_H
#define OHOS_ADAPTER_NDK_MOCK_H
#include <gmock/gmock.h>
#include "arkweb/ohos_adapter_ndk/interfaces/ark_ohos_adapter_helper_wrapper.h"

namespace OHOS::ArkWeb {
class MockArkAafwkAppMgrClientAdapter : public ArkAafwkAppMgrClientAdapter {
 public:
  static ArkWebRefPtr<MockArkAafwkAppMgrClientAdapter> Create() {
    return ArkWebRefPtr<MockArkAafwkAppMgrClientAdapter>(
        new MockArkAafwkAppMgrClientAdapter());
  }

  MOCK_METHOD(void, IncreRef, (), (const, override));
  MOCK_METHOD(void, DecreRef, (), (const, override));
  MOCK_METHOD(ArkWebRefPtr<ArkWebBaseRefCounted>, AddRef, (), ());
  MOCK_METHOD(ArkWebRefPtr<ArkWebBaseRefCounted>, Release, (), ());
  MOCK_METHOD(bool, HasOneRef, (), (const));
  MOCK_METHOD(bool, HasAtLeastOneRef, (), (const));
  MOCK_METHOD(int, StartRenderProcess, (const ArkWebString&, int32_t, int32_t, int32_t, pid_t&), (override));
  MOCK_METHOD(void, AttachRenderProcess, (ArkWebRefPtr<ArkAafwkRenderSchedulerHostAdapter>),(override));
  MOCK_METHOD(int, GetRenderProcessTerminationStatus, (pid_t, int&), (override));
  MOCK_METHOD(int, StartChildProcess,
      (const ArkWebString&, int32_t, int32_t, int32_t, pid_t&, const ArkWebString&), (override));
  MOCK_METHOD(void, SaveBrowserConnect,(ArkWebRefPtr<ArkAafwkBrowserHostAdapter>),(override));
};

class MockArkDisplayManagerAdapter : public ArkDisplayManagerAdapter {
 public:
  MOCK_METHOD(void, IncreRef, (), (const,override));
  MOCK_METHOD(void, DecreRef, (), (const,override));
  MOCK_METHOD(ArkWebRefPtr<ArkWebBaseRefCounted>, AddRef, () );
  MOCK_METHOD(ArkWebRefPtr<ArkWebBaseRefCounted>, Release, () );
  MOCK_METHOD(bool, HasOneRef, (), (const));
  MOCK_METHOD(bool, HasAtLeastOneRef, (), (const));
  MOCK_METHOD(uint64_t, GetDefaultDisplayId, (), (override));
  MOCK_METHOD(ArkWebRefPtr<ArkDisplayAdapter>, GetDefaultDisplay, (), (override));
  MOCK_METHOD(uint32_t, RegisterDisplayListener, (ArkWebRefPtr<ArkDisplayListenerAdapter>), (override));
  MOCK_METHOD(bool, UnregisterDisplayListener, (uint32_t), (override));
  MOCK_METHOD(bool, IsDefaultPortrait, (), (override));
  MOCK_METHOD(uint32_t, RegisterFoldStatusListener, (ArkWebRefPtr<ArkFoldStatusListenerAdapter>), (override));
  MOCK_METHOD(bool, UnregisterFoldStatusListener, (uint32_t), (override));
  MOCK_METHOD(ArkWebRefPtr<ArkDisplayAdapter>, GetPrimaryDisplay, (), (override));
  MOCK_METHOD(ArkDisplayAdapterVector, GetAllDisplays, (), (override));
};

class MockArkDisplayAdapter : public ArkDisplayAdapter {
 public:
  MOCK_METHOD(void, IncreRef, (), (const, override));
  MOCK_METHOD(void, DecreRef, (), (const, override));
  MOCK_METHOD(ArkWebRefPtr<ArkWebBaseRefCounted>, AddRef, ());
  MOCK_METHOD(ArkWebRefPtr<ArkWebBaseRefCounted>, Release, ());
  MOCK_METHOD(bool, HasOneRef, (), (const));
  MOCK_METHOD(bool, HasAtLeastOneRef, (), (const));
  MOCK_METHOD(uint64_t, GetId, (), (override));
  MOCK_METHOD(int32_t, GetWidth, (), (override));
  MOCK_METHOD(int32_t, GetHeight, (), (override));
  MOCK_METHOD(float, GetVirtualPixelRatio, (), (override));
  MOCK_METHOD(uint32_t, GetRotation, (), (override));
  MOCK_METHOD(uint32_t, GetOrientation, (), (override));
  MOCK_METHOD(int32_t, GetDpi, (), (override));
  MOCK_METHOD(uint32_t, GetDisplayOrientation, (), (override));
  MOCK_METHOD(uint32_t, GetFoldStatus, (), (override));
  MOCK_METHOD(bool, IsFoldable, (), (override));
  MOCK_METHOD(ArkWebString, GetName, (), (override));
  MOCK_METHOD(int32_t, GetAvailableWidth, (), (override));
  MOCK_METHOD(int32_t, GetAvailableHeight, (), (override));
  MOCK_METHOD(bool, GetAliveStatus, (), (override));
  MOCK_METHOD(uint32_t, GetDisplayState, (), (override));
  MOCK_METHOD(int32_t, GetDensityDpi, (), (override));
  MOCK_METHOD(int32_t, GetX, (), (override));
  MOCK_METHOD(int32_t, GetY, (), (override));
  MOCK_METHOD(uint32_t, GetDisplaySourceMode, (), (override));
  MOCK_METHOD(int32_t, GetPhysicalWidth, (), (override));
  MOCK_METHOD(int32_t, GetPhysicalHeight, (), (override));
  MOCK_METHOD(float, GetDefaultVirtualPixelRatio, (), (override));
};

class MockArkDisplayListenerAdapter : public ArkDisplayListenerAdapter {
 public:
  MOCK_METHOD(void, IncreRef, (), (const, override));
  MOCK_METHOD(void, DecreRef, (), (const, override));
  MOCK_METHOD(ArkWebRefPtr<ArkWebBaseRefCounted>, AddRef, ());
  MOCK_METHOD(ArkWebRefPtr<ArkWebBaseRefCounted>, Release, ());
  MOCK_METHOD(bool, HasOneRef, (), (const));
  MOCK_METHOD(bool, HasAtLeastOneRef, (), (const));
  MOCK_METHOD(void, OnCreate, (uint64_t), (override));
  MOCK_METHOD(void, OnDestroy, (uint64_t), (override));
  MOCK_METHOD(void, OnChange, (uint64_t), (override));
};

class MockArkFoldStatusListenerAdapter : public ArkFoldStatusListenerAdapter {
 public:
  MOCK_METHOD(void, IncreRef, (), (const, override));
  MOCK_METHOD(void, DecreRef, (), (const, override));
  MOCK_METHOD(ArkWebRefPtr<ArkWebBaseRefCounted>, AddRef, ());
  MOCK_METHOD(ArkWebRefPtr<ArkWebBaseRefCounted>, Release, ());
  MOCK_METHOD(bool, HasOneRef, (), (const));
  MOCK_METHOD(bool, HasAtLeastOneRef, (), (const));
  MOCK_METHOD(void, OnFoldStatusChanged, (uint32_t foldstatus), (override));
};

class MockArkNetConnectAdapter : public ArkNetConnectAdapter {
 public:
  MOCK_METHOD(void, IncreRef, (), (const,override));
  MOCK_METHOD(void, DecreRef, (), (const,override));
  MOCK_METHOD(int32_t, RegisterNetConnCallback, (ArkWebRefPtr<ArkNetConnCallback>), (override));
  MOCK_METHOD(int32_t, UnregisterNetConnCallback, (int32_t), (override));
  MOCK_METHOD(int32_t, GetDefaultNetConnect, (uint32_t&, uint32_t&), (override));
  MOCK_METHOD(ArkWebStringVector, GetDnsServers, (), (override));
  MOCK_METHOD(ArkWebStringVector, GetDnsServersByNetId, (int32_t), (override));
  MOCK_METHOD(ArkWebStringVector, GetDnsServersForVpn, (), (override));
  MOCK_METHOD(void, RegisterVpnListener, (ArkWebRefPtr<ArkVpnListener>), (override));
  MOCK_METHOD(void, UnRegisterVpnListener, (), (override));
};

class MockArkAudioSystemManagerAdapter : public ArkAudioSystemManagerAdapter {
 public:
  MOCK_METHOD(void, IncreRef, (), (const,override));
  MOCK_METHOD(void, DecreRef, (), (const,override));
  MOCK_METHOD(bool, HasAudioOutputDevices, (), (override));
  MOCK_METHOD(bool, HasAudioInputDevices, (), (override));
  MOCK_METHOD(int32_t, RequestAudioFocus, (const ArkWebRefPtr<ArkAudioInterruptAdapter>), (override));
  MOCK_METHOD(int32_t, AbandonAudioFocus, (const ArkWebRefPtr<ArkAudioInterruptAdapter>), (override));
  MOCK_METHOD(int32_t, SetAudioManagerInterruptCallback, (ArkWebRefPtr<ArkAudioManagerCallbackAdapter>), (override));
  MOCK_METHOD(int32_t, UnsetAudioManagerInterruptCallback, (), (override));
  MOCK_METHOD(ArkAudioDeviceDescAdapterVector, GetDevices, (int32_t), (override));
  MOCK_METHOD(int32_t, SelectAudioDeviceById, (int32_t, bool), (override));
  MOCK_METHOD(int32_t, SetDeviceChangeCallback, (ArkWebRefPtr<ArkAudioManagerDeviceChangeCallbackAdapter>), (override));
  MOCK_METHOD(int32_t, UnsetDeviceChangeCallback, (), (override));
  MOCK_METHOD(ArkWebRefPtr<ArkAudioDeviceDescAdapter>, GetDefaultOutputDevice, (), (override));
  MOCK_METHOD(ArkWebRefPtr<ArkAudioDeviceDescAdapter>, GetDefaultInputDevice, (), (override));
  MOCK_METHOD(bool, SetLanguage, (const ArkWebString&), (override));
};

class MockArkOhosAdapterHelper : public ArkOhosAdapterHelper {
 public:
  static ArkWebRefPtr<MockArkOhosAdapterHelper> Create() {
    return ArkWebRefPtr<MockArkOhosAdapterHelper>(
        new MockArkOhosAdapterHelper());
  }
  MOCK_METHOD(void, IncreRef, (), (const,override));
    MOCK_METHOD(void, DecreRef, (), (const,override));
    MOCK_METHOD(ArkWebRefPtr<ArkWebBaseRefCounted>, AddRef, (), ());
    MOCK_METHOD(ArkWebRefPtr<ArkWebBaseRefCounted>, Release, (), ());
    MOCK_METHOD(bool, HasOneRef, (), (const));
    MOCK_METHOD(bool, HasAtLeastOneRef, (), (const));

    MOCK_METHOD(ArkWebRefPtr<ArkAafwkAppMgrClientAdapter>, CreateAafwkAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkPowerMgrClientAdapter>, CreatePowerMgrClientAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkDisplayManagerAdapter>, CreateDisplayMgrAdapter, (), (override));
    
    MOCK_METHOD(ArkWebRefPtr<ArkBatteryMgrClientAdapter>, CreateBatteryClientAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkNetConnectAdapter>, CreateNetConnectAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkOhosWebDataBaseAdapter>, GetOhosWebDataBaseAdapterInstance, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkPasteBoardClientAdapter>, GetPasteBoard, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkAudioRendererAdapter>, CreateAudioRendererAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkAudioCapturerAdapter>, CreateAudioCapturerAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkAudioSystemManagerAdapter>, GetAudioSystemManager, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkOhosWebPermissionDataBaseAdapter>, GetWebPermissionDataBaseInstance, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkMMIAdapter>, CreateMMIAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkSocPerfClientAdapter>, CreateSocPerfClientAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkOhosResourceAdapter>, GetResourceAdapter, (const ArkWebString&), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkSystemPropertiesAdapter>, GetSystemPropertiesInstance, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkVSyncAdapter>, GetVSyncAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkOhosInitWebAdapter>, GetInitWebAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkKeystoreAdapter>, GetKeystoreAdapterInstance, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkEnterpriseDeviceManagementAdapter>, GetEnterpriseDeviceManagementInstance, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkDatashareAdapter>, GetDatashareInstance, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkIMFAdapter>, CreateIMFAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkCertManagerAdapter>, GetRootCertDataAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkAccessTokenAdapter>, GetAccessTokenAdapterInstance, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkEventHandlerAdapter>, GetEventHandlerAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkPrintManagerAdapter>, GetPrintManagerInstance, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkIConsumerSurfaceAdapter>, CreateConsumerSurfaceAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkPlayerAdapter>, CreatePlayerAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkWindowAdapter>, GetWindowAdapterInstance, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkHiSysEventAdapter>, GetHiSysEventAdapterInstance, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkHiTraceAdapter>, GetHiTraceAdapterInstance, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkNetProxyAdapter>, GetNetProxyInstance, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkCameraManagerAdapter>, GetCameraManagerAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkScreenCaptureAdapter>, CreateScreenCaptureAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkDateTimeFormatAdapter>, CreateDateTimeFormatAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkMediaCodecDecoderAdapter>, CreateMediaCodecDecoderAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkNativeImageAdapter>, CreateNativeImageAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkMediaCodecAdapter>, CreateMediaCodecEncoderAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkMediaCodecListAdapter>, GetMediaCodecListAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkFlowbufferAdapter>, CreateFlowbufferAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkMediaAVSessionAdapter>, CreateMediaAVSessionAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkOhosImageDecoderAdapter>, CreateOhosImageDecoderAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkSensorAdapter>, CreateSensorAdapter, (), (override));
    MOCK_METHOD(void, SetArkWebCoreHapPathOverride, (const ArkWebString&), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkOhosNativeBufferAdapter>, GetOhosNativeBufferAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkMigrationManagerAdapter>, CreateMigrationMgrAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkAudioCodecDecoderAdapter>, CreateAudioCodecDecoderAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkDrmAdapter>, CreateDrmAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkOhosDrawingTextFontAdapter>, GetOhosDrawingTextFontAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkOhosDrawingTextTypographyAdapter>, GetOhosDrawingTextTypographyAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkScreenlockManagerAdapter>, CreateScreenlockManagerAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkCertManagerAdapter>, GetCertManagerAdapter, (), (override));
    MOCK_METHOD(ArkWebRefPtr<ArkBackgroundTaskAdapter>, CreateBackgroundTaskAdapter, (), (override));
};

class MockArkMMIAdapter : public ArkMMIAdapter {
 public:
  MOCK_METHOD(void, IncreRef, (), (const, override));
  MOCK_METHOD(void, DecreRef, (), (const, override));
  MOCK_METHOD(char*, KeyCodeToString, (int32_t), (override));
  MOCK_METHOD(int32_t, RegisterMMIInputListener, (ArkWebRefPtr<ArkMMIInputListenerAdapter>), (override));
  MOCK_METHOD(void, UnregisterMMIInputListener, (int32_t), (override));
  MOCK_METHOD(int32_t, RegisterDevListener, (ArkWebString, ArkWebRefPtr<ArkMMIListenerAdapter>), (override));
  MOCK_METHOD(int32_t, UnregisterDevListener, (ArkWebString), (override));
  MOCK_METHOD(int32_t, GetKeyboardType, (int32_t, int32_t&), (override));
  MOCK_METHOD(int32_t, GetDeviceIds, (ArkWebInt32Vector&), (override));
  MOCK_METHOD(int32_t, GetDeviceInfo, (int32_t, ArkWebRefPtr<ArkMMIDeviceInfoAdapter>), (override));
  MOCK_METHOD(int32_t, GetMaxTouchPoints, (), (override));
};

class MockArkSocPerfClientAdapter : public ArkSocPerfClientAdapter {
 public:
  MOCK_METHOD(void, IncreRef, (), (const, override));
  MOCK_METHOD(void, DecreRef, (), (const, override));
  MOCK_METHOD(void, ApplySocPerfConfigById, (int32_t id), (override));
  MOCK_METHOD(void, ApplySocPerfConfigByIdEx, (int32_t id, bool onOffTag), (override));
};

class MockArkIMFAdapter : public ArkIMFAdapter {
 public:
  MOCK_METHOD(void, IncreRef, (), (const, override));
  MOCK_METHOD(void, DecreRef, (), (const, override));
  MOCK_METHOD(bool, Attach, (ArkWebRefPtr<ArkIMFTextListenerAdapter>, bool), (override));
  MOCK_METHOD(bool, Attach, 
      (ArkWebRefPtr<ArkIMFTextListenerAdapter>, bool, ArkWebRefPtr<ArkIMFTextConfigAdapter>, bool), (override));
  MOCK_METHOD(bool, AttachWithRequestKeyboardReason, 
      (ArkWebRefPtr<ArkIMFTextListenerAdapter>, bool, ArkWebRefPtr<ArkIMFTextConfigAdapter>, 
          bool, int32_t), (override));
  MOCK_METHOD(void, ShowCurrentInput, (const int32_t&), (override));
  MOCK_METHOD(void, HideTextInput, (), (override));
  MOCK_METHOD(void, Close, (), (override));
  MOCK_METHOD(void, OnCursorUpdate, (ArkWebRefPtr<ArkIMFCursorInfoAdapter>), (override));
  MOCK_METHOD(void, OnSelectionChange, (ArkWebU16String&, int, int), (override));
  MOCK_METHOD(bool, SendPrivateCommand, (const ArkWebString&, const ArkWebString&), (override));
};

class MockArkPlayerAdapter : public ArkPlayerAdapter {
 public:
  MOCK_METHOD(void, IncreRef, (), (const, override));
  MOCK_METHOD(void, DecreRef, (), (const, override));
  MOCK_METHOD(int32_t, SetPlayerCallback, (ArkWebRefPtr<ArkPlayerCallbackAdapter>), (override));
  MOCK_METHOD(int32_t, SetSource, (const ArkWebString&), (override));
  MOCK_METHOD(int32_t, SetSource, (int32_t, int64_t, int64_t), (override));
  MOCK_METHOD(int32_t, SetVideoSurface, (ArkWebRefPtr<ArkIConsumerSurfaceAdapter>), (override));
  MOCK_METHOD(int32_t, SetVolume, (float, float), (override));
  MOCK_METHOD(int32_t, Seek, (int32_t, int32_t), (override));
  MOCK_METHOD(int32_t, Play, (), (override));
  MOCK_METHOD(int32_t, Pause, (), (override));
  MOCK_METHOD(int32_t, PrepareAsync, (), (override));
  MOCK_METHOD(int32_t, GetCurrentTime, (int32_t&), (override));
  MOCK_METHOD(int32_t, GetDuration, (int32_t&), (override));
  MOCK_METHOD(int32_t, SetPlaybackSpeed, (int32_t), (override));
  MOCK_METHOD(int32_t, SetVideoSurfaceNew, (void*), (override));
  MOCK_METHOD(int32_t, SetMediaSourceHeader, (const ArkWebString&, const ArkWebStringMap&), (override));
};

class MockArkFlowbufferAdapter : public ArkFlowbufferAdapter {
 public:
  MOCK_METHOD(void, IncreRef, (), (const, override));
  MOCK_METHOD(void, DecreRef, (), (const, override));
  MOCK_METHOD(void, StartPerformanceBoost, (), (override));
  MOCK_METHOD(void*, CreateAshmem, (size_t, int, int&), (override));
  MOCK_METHOD(void*, CreateAshmemWithFd, (const int, size_t, int), (override));
};

class MockArkMigrationManagerAdapter : public ArkMigrationManagerAdapter {
 public:
  MOCK_METHOD(void, IncreRef, (), (const, override));
  MOCK_METHOD(void, DecreRef, (), (const, override));
  MOCK_METHOD(void, SetMigrationParam, (const ArkWebString&, const ArkWebString&, const ArkWebString&), (override));
  MOCK_METHOD(bool, SendMigrationRequest, (const char*), (override));
  MOCK_METHOD(uint32_t, RegisterMigrationListener, (ArkWebRefPtr<ArkMigrationListenerAdapter>), (override));
};
}  // namespace OHOS::ArkWeb

#endif //OHOS_ADAPTER_NDK_MOCK_H