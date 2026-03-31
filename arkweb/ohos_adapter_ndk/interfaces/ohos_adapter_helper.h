/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ADAPTER_HELPER_H
#define OHOS_ADAPTER_HELPER_H

#include <memory>
#include "arkweb/build/features/features.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/aafwk_app_mgr_client_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/access_token_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/audio_capturer_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/audio_renderer_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/audio_system_manager_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/battery_mgr_client_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/camera_manager_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/cert_mgr_adapter.h"
#include "color_picker_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/datashare_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/date_time_format_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/display_manager_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/drm_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/enterprise_device_management_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/event_handler_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/flowbuffer_adapter.h"
#include "arkweb/ohos_adapter_ndk/interfaces/graphic_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/hisysevent_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/hitrace_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/imf_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/keystore_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/media_adapter.h"
#include "arkweb/ohos_adapter_ndk/interfaces/media_avsession_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/media_codec_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/media_codec_decoder_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/migration_manager_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/audio_codec_decoder_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/mmi_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/net_connect_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/net_proxy_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_drawing_text_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_image_decoder_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_init_web_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_native_buffer_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_resource_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_web_data_base_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/pasteboard_client_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/power_mgr_client_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/print_manager_adapter.h"
#include "arkweb/ohos_adapter_ndk/interfaces/screen_capture_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/screenlock_manager_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/soc_perf_client_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/system_properties_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/sensor_adapter.h"
#include "arkweb/ohos_adapter_ndk/interfaces/net_config_adapter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/background_task_adapter.h"
#include "arkweb/ohos_adapter_ndk/interfaces/net_event_adapter.h"
#include "arkweb/ohos_adapter_ndk/interfaces/hiappevent_adapter.h"

namespace OHOS::NWeb {

class OhosAdapterHelper {
public:
    static OhosAdapterHelper& GetInstance();
#if BUILDFLAG(ARKWEB_TEST)
    static void SetInstance(OhosAdapterHelper* instance);
#endif
    virtual ~OhosAdapterHelper() = default;

    virtual std::unique_ptr<AafwkAppMgrClientAdapter> CreateAafwkAdapter() = 0;

    virtual std::unique_ptr<PowerMgrClientAdapter> CreatePowerMgrClientAdapter() = 0;

    virtual std::unique_ptr<DisplayManagerAdapter> CreateDisplayMgrAdapter() = 0;

    virtual std::unique_ptr<BatteryMgrClientAdapter> CreateBatteryClientAdapter() = 0;

    virtual std::unique_ptr<NetConnectAdapter> CreateNetConnectAdapter() = 0;

    virtual OhosWebDataBaseAdapter& GetOhosWebDataBaseAdapterInstance() = 0;

    virtual PasteBoardClientAdapter& GetPasteBoard() = 0;

    virtual std::unique_ptr<AudioRendererAdapter> CreateAudioRendererAdapter() = 0;

    virtual std::unique_ptr<AudioCapturerAdapter> CreateAudioCapturerAdapter() = 0;

    virtual AudioSystemManagerAdapter& GetAudioSystemManager() = 0;

    virtual OhosWebPermissionDataBaseAdapter& GetWebPermissionDataBaseInstance() = 0;

    virtual std::unique_ptr<MMIAdapter> CreateMMIAdapter() = 0;

    virtual std::unique_ptr<SocPerfClientAdapter> CreateSocPerfClientAdapter() = 0;

    virtual std::unique_ptr<OhosResourceAdapter> GetResourceAdapter(const std::string& hapPath = "") = 0;

    virtual SystemPropertiesAdapter& GetSystemPropertiesInstance() = 0;

    virtual VSyncAdapter& GetVSyncAdapter() = 0;

    virtual std::unique_ptr<OhosInitWebAdapter> GetInitWebAdapter() = 0;

    virtual KeystoreAdapter& GetKeystoreAdapterInstance() = 0;

    virtual EnterpriseDeviceManagementAdapter& GetEnterpriseDeviceManagementInstance() = 0;

    virtual DatashareAdapter& GetDatashareInstance() = 0;

    virtual std::unique_ptr<IMFAdapter> CreateIMFAdapter() = 0;

    virtual std::unique_ptr<CertManagerAdapter> GetRootCertDataAdapter() = 0;

    virtual AccessTokenAdapter& GetAccessTokenAdapterInstance() = 0;

    virtual std::unique_ptr<EventHandlerAdapter> GetEventHandlerAdapter() = 0;

    virtual PrintManagerAdapter& GetPrintManagerInstance() = 0;

    virtual std::unique_ptr<IConsumerSurfaceAdapter> CreateConsumerSurfaceAdapter() = 0;

    virtual std::unique_ptr<PlayerAdapter> CreatePlayerAdapter() = 0;

    virtual WindowAdapter& GetWindowAdapterInstance() = 0;

    virtual HiSysEventAdapter& GetHiSysEventAdapterInstance() = 0;

    virtual HiTraceAdapter& GetHiTraceAdapterInstance() = 0;

    virtual NetProxyAdapter& GetNetProxyInstance() = 0;

    virtual CameraManagerAdapter& GetCameraManagerAdapter() = 0;

    virtual std::unique_ptr<ScreenCaptureAdapter> CreateScreenCaptureAdapter() = 0;

    virtual std::unique_ptr<DateTimeFormatAdapter> CreateDateTimeFormatAdapter() = 0;

    virtual std::unique_ptr<MediaCodecDecoderAdapter> CreateMediaCodecDecoderAdapter() = 0;

    virtual std::unique_ptr<NativeImageAdapter> CreateNativeImageAdapter() = 0;

    virtual std::unique_ptr<MediaCodecAdapter> CreateMediaCodecEncoderAdapter() = 0;

    virtual MediaCodecListAdapter& GetMediaCodecListAdapter() = 0;

    virtual std::unique_ptr<FlowbufferAdapter> CreateFlowbufferAdapter() = 0;

    virtual std::shared_ptr<MediaAVSessionAdapter> CreateMediaAVSessionAdapter() = 0;

    virtual std::unique_ptr<OhosImageDecoderAdapter> CreateOhosImageDecoderAdapter() = 0;

    virtual std::unique_ptr<SensorAdapter> CreateSensorAdapter() = 0;

    virtual void SetArkWebCoreHapPathOverride(const std::string& hapPath) = 0;

    virtual OhosNativeBufferAdapter& GetOhosNativeBufferAdapter() = 0;

    virtual std::unique_ptr<AudioCodecDecoderAdapter> CreateAudioCodecDecoderAdapter() = 0;

    virtual std::unique_ptr<DrmAdapter> CreateDrmAdapter() = 0;

    virtual OhosDrawingTextFontAdapter& GetOhosDrawingTextFontAdapter() = 0;

    virtual OhosDrawingTextTypographyAdapter& GetOhosDrawingTextTypographyAdapter() = 0;
    virtual std::unique_ptr<MigrationManagerAdapter> CreateMigrationMgrAdapter() = 0;

    virtual std::unique_ptr<ScreenlockManagerAdapter> CreateScreenlockManagerAdapter() = 0;

    virtual std::unique_ptr<CertManagerAdapter> GetCertManagerAdapter() = 0;

    virtual std::unique_ptr<BackgroundTaskAdapter> CreateBackgroundTaskAdapter() {return nullptr;}

    virtual std::unique_ptr<NetConfigAdapter> GetNetConfigAdapter() = 0;

    virtual std::unique_ptr<NetEventAdapter> CreateNetEventAdapter() = 0;

    virtual ColorPickerAdapter& GetColorPickerAdapter() = 0;

    virtual HiAppeventAdapter& GetHiAppeventAdapterInstance() = 0;
};

} // namespace OHOS::NWeb

#endif // OHOS_ADAPTER_HELPER_H
