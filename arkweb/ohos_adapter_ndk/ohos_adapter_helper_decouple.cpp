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

#include "arkweb/ohos_adapter_ndk/access_token_adapter/access_token_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/audio_capturer_adapter/audio_capturer_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/battery_mgr_adapter/battery_mgr_client_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/cert_mgr_adapter/cert_mgr_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/date_time_format_adapter/date_time_format_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/display_manager_adapter/native_display_manager_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/drawing_text_adapter/ohos_drawing_text_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_image_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_window_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/inputmethodframework_adapter/imf_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/interfaces/ark_ohos_adapter_helper_wrapper.h"
#include "arkweb/ohos_adapter_ndk/keystore_adapter/keystore_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/audio_codec_decoder_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/drm_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/include/media_codec_list_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/include/video_encoder_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/media_codec_decoder_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_avsession_adapter/media_avsession_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/multimodalinputnew_adapter/mmi_new_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/net_config_adapter/net_config_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/net_connect_adapter/net_connect_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/net_event_adapter/net_event_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/net_proxy_adapter_impl/net_proxy_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/ohos_image_adapter/ohos_image_decoder_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/ohos_native_buffer_adapter/ohos_native_buffer_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/screen_capture_adapter/screen_capture_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/sensor_adapter/sensor_adapter_impl.h"
#include "base/bridge/ark_web_bridge_macros.h"
#include "datashare_adapter/datashare_adapter_impl.h"
#include "distributeddatamgr_adapter/ohos_web_data_base_adapter_impl.h"
#include "hiviewdfx_adapter/hitrace_adapter_impl.h"
#include "ohos_adapter/bridge/ark_aafwk_app_mgr_client_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_access_token_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_capturer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_codec_decoder_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_renderer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_system_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_background_task_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_battery_info_wrapper.h"
#include "ohos_adapter/bridge/ark_battery_mgr_client_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_camera_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_camera_surface_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_camera_surface_buffer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_cert_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_datashare_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_date_time_format_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_display_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_display_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_drm_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_enterprise_device_management_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_event_handler_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_flowbuffer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_hisysevent_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_hitrace_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_iconsumer_surface_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_imfadapter_wrapper.h"
#include "ohos_adapter/bridge/ark_keystore_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_media_avsession_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_media_codec_decoder_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_media_codec_encoder_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_media_codec_list_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_migration_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_mmi_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_net_connect_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_net_proxy_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_drawing_text_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_file_mapper_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_image_decoder_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_init_web_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_native_buffer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_resource_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_web_data_base_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_player_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_print_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_running_lock_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_screen_capture_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_screenlock_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_sensor_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_soc_perf_client_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_surface_buffer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_system_properties_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_vsync_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_web_timezone_info_wrapper.h"
#include "ohos_adapter/bridge/ark_window_adapter_wrapper.h"
#include "color_picker_adapter/color_picker_adapter_impl.h"
#include "pasteboard_adapter/include/pasteboard_client_adapter_impl.h"
#include "hiviewdfx_adapter/hiappevent_adapter_impl.h"

namespace OHOS::NWeb {
#if BUILDFLAG(ARKWEB_TEST)
static OhosAdapterHelper* instance_ = nullptr;
#endif
OhosAdapterHelper& OhosAdapterHelper::GetInstance() {
#if BUILDFLAG(ARKWEB_TEST)
    if (instance_) {
        return *instance_;
    }
#endif
  static ArkWeb::ArkOhosAdapterHelperWrapper instance(
      ArkWeb::ArkOhosAdapterHelper::GetInstance());
  return instance;
}

#if BUILDFLAG(ARKWEB_TEST)
void OhosAdapterHelper::SetInstance(OhosAdapterHelper* instance)
{
    instance_ = instance;
}
#endif
}  // namespace OHOS::NWeb

namespace OHOS::ArkWeb {

ArkOhosAdapterHelperWrapper::ArkOhosAdapterHelperWrapper(
    ArkWebRefPtr<ArkOhosAdapterHelper> ref)
    : ctocpp_(ref) {}

std::unique_ptr<NWeb::AafwkAppMgrClientAdapter>
ArkOhosAdapterHelperWrapper::CreateAafwkAdapter() {
  if (CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    return nullptr;
  }

  ArkWebRefPtr<ArkAafwkAppMgrClientAdapter> adapter =
      ctocpp_->CreateAafwkAdapter();

  if (CHECK_REF_PTR_IS_NULL(adapter)) {
    return nullptr;
  }

  return std::make_unique<ArkAafwkAppMgrClientAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::PowerMgrClientAdapter>
ArkOhosAdapterHelperWrapper::CreatePowerMgrClientAdapter() {
  return nullptr;
}

std::unique_ptr<NWeb::DisplayManagerAdapter>
ArkOhosAdapterHelperWrapper::CreateDisplayMgrAdapter() {
  if (CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    return nullptr;
  }

  ArkWebRefPtr<ArkDisplayManagerAdapter> adapter =
      ctocpp_->CreateDisplayMgrAdapter();
  if (CHECK_REF_PTR_IS_NULL(adapter)) {
    return nullptr;
  }

  return std::make_unique<ArkDisplayManagerAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::BatteryMgrClientAdapter>
ArkOhosAdapterHelperWrapper::CreateBatteryClientAdapter() {
  return std::make_unique<NWeb::BatteryMgrClientAdapterImpl>();
}

std::unique_ptr<NWeb::NetConnectAdapter>
ArkOhosAdapterHelperWrapper::CreateNetConnectAdapter() {
  ArkWebRefPtr<ArkNetConnectAdapter> adapter =
      ctocpp_->CreateNetConnectAdapter();

  if (CHECK_REF_PTR_IS_NULL(adapter)) {
    return nullptr;
  }

  return std::make_unique<ArkNetConnectAdapterWrapper>(adapter);
}

NWeb::OhosWebDataBaseAdapter&
ArkOhosAdapterHelperWrapper::GetOhosWebDataBaseAdapterInstance() {
  ArkWebRefPtr<ArkOhosWebDataBaseAdapter> ref = nullptr;
  if (!CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    ref = ctocpp_->GetOhosWebDataBaseAdapterInstance();
  }

  static ArkOhosWebDataBaseAdapterWrapper instance(ref);
  return instance;
}

NWeb::PasteBoardClientAdapter& ArkOhosAdapterHelperWrapper::GetPasteBoard() {
  static PasteBoardClientAdapterImpl instance;
  return instance;
}

NWeb::ColorPickerAdapter& ArkOhosAdapterHelperWrapper::GetColorPickerAdapter() {
  static ColorPickerAdapterImpl instance;
  return instance;
}

std::unique_ptr<NWeb::AudioRendererAdapter>
ArkOhosAdapterHelperWrapper::CreateAudioRendererAdapter() {
  return nullptr;
}

std::unique_ptr<NWeb::AudioCapturerAdapter>
ArkOhosAdapterHelperWrapper::CreateAudioCapturerAdapter() {
  return std::make_unique<NWeb::AudioCapturerAdapterImpl>();
}

NWeb::AudioSystemManagerAdapter&
ArkOhosAdapterHelperWrapper::GetAudioSystemManager() {
  ArkWebRefPtr<ArkAudioSystemManagerAdapter> ref = nullptr;
  if (!CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    ref = ctocpp_->GetAudioSystemManager();
  }

  static ArkAudioSystemManagerAdapterWrapper instance(ref);
  return instance;
}

NWeb::OhosWebPermissionDataBaseAdapter&
ArkOhosAdapterHelperWrapper::GetWebPermissionDataBaseInstance() {
  ArkWebRefPtr<ArkOhosWebPermissionDataBaseAdapter> ref = nullptr;
  if (!CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    ref = ctocpp_->GetWebPermissionDataBaseInstance();
  }

  static ArkOhosWebPermissionDataBaseAdapterWrapper instance(ref);
  return instance;
}

std::unique_ptr<NWeb::MMIAdapter>
ArkOhosAdapterHelperWrapper::CreateMMIAdapter() {
  if (CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    return nullptr;
  }

  ArkWebRefPtr<ArkMMIAdapter> adapter = ctocpp_->CreateMMIAdapter();

  if (CHECK_REF_PTR_IS_NULL(adapter)) {
    return nullptr;
  }

  return std::make_unique<ArkMMIAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::SocPerfClientAdapter>
ArkOhosAdapterHelperWrapper::CreateSocPerfClientAdapter() {
  if (CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    return nullptr;
  }

  ArkWebRefPtr<ArkSocPerfClientAdapter> adapter =
      ctocpp_->CreateSocPerfClientAdapter();

  if (CHECK_REF_PTR_IS_NULL(adapter)) {
    return nullptr;
  }

  return std::make_unique<ArkSocPerfClientAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::OhosResourceAdapter>
ArkOhosAdapterHelperWrapper::GetResourceAdapter(const std::string& hapPath) {
  if (CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    return nullptr;
  }

  ArkWebString str = ArkWebStringClassToStruct(hapPath);
  ArkWebRefPtr<ArkOhosResourceAdapter> adapter =
      ctocpp_->GetResourceAdapter(str);
  ArkWebStringStructRelease(str);

  if (CHECK_REF_PTR_IS_NULL(adapter)) {
    return nullptr;
  }

  return std::make_unique<ArkOhosResourceAdapterWrapper>(adapter);
}

NWeb::SystemPropertiesAdapter&
ArkOhosAdapterHelperWrapper::GetSystemPropertiesInstance() {
  ArkWebRefPtr<ArkSystemPropertiesAdapter> ref = nullptr;
  if (!CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    ref = ctocpp_->GetSystemPropertiesInstance();
  }

  static ArkSystemPropertiesAdapterWrapper instance(ref);
  return instance;
}

NWeb::VSyncAdapter& ArkOhosAdapterHelperWrapper::GetVSyncAdapter() {
  ArkWebRefPtr<ArkVSyncAdapter> ref = nullptr;
  if (!CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    ref = ctocpp_->GetVSyncAdapter();
  }

  static ArkVSyncAdapterWrapper instance(ref);
  return instance;
}

std::unique_ptr<NWeb::OhosInitWebAdapter>
ArkOhosAdapterHelperWrapper::GetInitWebAdapter() {
  if (CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    return nullptr;
  }

  ArkWebRefPtr<ArkOhosInitWebAdapter> adapter = ctocpp_->GetInitWebAdapter();

  if (CHECK_REF_PTR_IS_NULL(adapter)) {
    return nullptr;
  }

  return std::make_unique<ArkOhosInitWebAdapterWrapper>(adapter);
}

NWeb::KeystoreAdapter&
ArkOhosAdapterHelperWrapper::GetKeystoreAdapterInstance() {
  return KeystoreAdapterImpl::GetInstance();
}

NWeb::EnterpriseDeviceManagementAdapter&
ArkOhosAdapterHelperWrapper::GetEnterpriseDeviceManagementInstance() {
  ArkWebRefPtr<ArkEnterpriseDeviceManagementAdapter> ref = nullptr;
  if (!CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    ref = ctocpp_->GetEnterpriseDeviceManagementInstance();
  }

  static ArkEnterpriseDeviceManagementAdapterWrapper instance(ref);
  return instance;
}

NWeb::DatashareAdapter& ArkOhosAdapterHelperWrapper::GetDatashareInstance() {
  return DatashareAdapterImpl::GetInstance();
}

std::unique_ptr<NWeb::IMFAdapter>
ArkOhosAdapterHelperWrapper::CreateIMFAdapter() {
  if (CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    return nullptr;
  }

  ArkWebRefPtr<ArkIMFAdapter> adapter = ctocpp_->CreateIMFAdapter();

  if (CHECK_REF_PTR_IS_NULL(adapter)) {
    return nullptr;
  }

  return std::make_unique<ArkIMFAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::CertManagerAdapter>
ArkOhosAdapterHelperWrapper::GetRootCertDataAdapter() {
  return std::make_unique<CertManagerAdapterImpl>();
}

std::unique_ptr<NWeb::CertManagerAdapter>
ArkOhosAdapterHelperWrapper::GetCertManagerAdapter() {
  if (CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    return nullptr;
  }

  ArkWebRefPtr<ArkCertManagerAdapter> adapter =
      ctocpp_->GetRootCertDataAdapter();
 
  if (CHECK_REF_PTR_IS_NULL(adapter)) {
    return nullptr;
  }
 
  return std::make_unique<ArkCertManagerAdapterWrapper>(adapter);
}

NWeb::AccessTokenAdapter&
ArkOhosAdapterHelperWrapper::GetAccessTokenAdapterInstance() {
  return AccessTokenAdapterImpl::GetInstance();
}

std::unique_ptr<NWeb::EventHandlerAdapter>
ArkOhosAdapterHelperWrapper::GetEventHandlerAdapter() {
  if (CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    return nullptr;
  }

  ArkWebRefPtr<ArkEventHandlerAdapter> adapter =
      ctocpp_->GetEventHandlerAdapter();

  if (CHECK_REF_PTR_IS_NULL(adapter)) {
    return nullptr;
  }

  return std::make_unique<ArkEventHandlerAdapterWrapper>(adapter);
}

NWeb::PrintManagerAdapter&
ArkOhosAdapterHelperWrapper::GetPrintManagerInstance() {
  ArkWebRefPtr<ArkPrintManagerAdapter> ref = nullptr;
  if (!CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    ref = ctocpp_->GetPrintManagerInstance();
  }

  static ArkPrintManagerAdapterWrapper instance(ref);
  return instance;
}

std::unique_ptr<NWeb::IConsumerSurfaceAdapter>
ArkOhosAdapterHelperWrapper::CreateConsumerSurfaceAdapter() {
  return std::make_unique<NWeb::ConsumerNativeAdapterImpl>();
}

std::unique_ptr<NWeb::PlayerAdapter>
ArkOhosAdapterHelperWrapper::CreatePlayerAdapter() {
  ArkWebRefPtr<ArkPlayerAdapter> adapter = ctocpp_->CreatePlayerAdapter();
  if (CHECK_REF_PTR_IS_NULL(adapter)) {
    return nullptr;
  }
  return std::make_unique<ArkPlayerAdapterWrapper>(adapter);
}

NWeb::WindowAdapter& ArkOhosAdapterHelperWrapper::GetWindowAdapterInstance() {
  ArkWebRefPtr<ArkWindowAdapter> ref = nullptr;
  if (!CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    ref = ctocpp_->GetWindowAdapterInstance();
  }

  static ArkWindowAdapterWrapper instance(ref);
  return instance;
}

NWeb::HiSysEventAdapter&
ArkOhosAdapterHelperWrapper::GetHiSysEventAdapterInstance() {
  ArkWebRefPtr<ArkHiSysEventAdapter> ref = nullptr;
  if (!CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    ref = ctocpp_->GetHiSysEventAdapterInstance();
  }

  static ArkHiSysEventAdapterWrapper instance(ref);
  return instance;
}

NWeb::HiTraceAdapter& ArkOhosAdapterHelperWrapper::GetHiTraceAdapterInstance() {
  return HiTraceAdapterImpl::GetInstance();
}

NWeb::NetProxyAdapter& ArkOhosAdapterHelperWrapper::GetNetProxyInstance() {
  ArkWebRefPtr<ArkNetProxyAdapter> ref = nullptr;
  if (!CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    ref = ctocpp_->GetNetProxyInstance();
  }

  static ArkNetProxyAdapterWrapper instance(ref);
  return instance;
}

NWeb::CameraManagerAdapter&
ArkOhosAdapterHelperWrapper::GetCameraManagerAdapter() {
  ArkWebRefPtr<ArkCameraManagerAdapter> ref = nullptr;
  if (!CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    ref = ctocpp_->GetCameraManagerAdapter();
  }

  static ArkCameraManagerAdapterWrapper instance(ref);
  return instance;
}

std::unique_ptr<NWeb::ScreenCaptureAdapter>
ArkOhosAdapterHelperWrapper::CreateScreenCaptureAdapter() {
  return std::make_unique<NWeb::ScreenCaptureAdapterImpl>();
}

std::unique_ptr<NWeb::DateTimeFormatAdapter>
ArkOhosAdapterHelperWrapper::CreateDateTimeFormatAdapter() {
  return std::make_unique<DateTimeFormatAdapterImpl>();
}

std::unique_ptr<NWeb::MediaCodecDecoderAdapter>
ArkOhosAdapterHelperWrapper::CreateMediaCodecDecoderAdapter() {
  return std::make_unique<NWeb::MediaCodecDecoderAdapterImpl>();
}

std::unique_ptr<NWeb::NativeImageAdapter>
ArkOhosAdapterHelperWrapper::CreateNativeImageAdapter() {
  return std::make_unique<NativeImageAdapterImpl>();
}

std::unique_ptr<NWeb::MediaCodecAdapter>
ArkOhosAdapterHelperWrapper::CreateMediaCodecEncoderAdapter() {
  return std::make_unique<NWeb::VideoEncoderAdapterImpl>();
}

NWeb::MediaCodecListAdapter&
ArkOhosAdapterHelperWrapper::GetMediaCodecListAdapter() {
  return MediaCodecListAdapterImpl::GetInstance();
}

std::unique_ptr<NWeb::FlowbufferAdapter>
ArkOhosAdapterHelperWrapper::CreateFlowbufferAdapter() {
  if (CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    return nullptr;
  }

  ArkWebRefPtr<ArkFlowbufferAdapter> adapter =
      ctocpp_->CreateFlowbufferAdapter();

  if (CHECK_REF_PTR_IS_NULL(adapter)) {
    return nullptr;
  }

  return std::make_unique<ArkFlowbufferAdapterWrapper>(adapter);
}

std::shared_ptr<NWeb::MediaAVSessionAdapter>
ArkOhosAdapterHelperWrapper::CreateMediaAVSessionAdapter() {
  return std::make_shared<NWeb::MediaAVSessionAdapterImpl>();
}

std::unique_ptr<NWeb::OhosImageDecoderAdapter>
ArkOhosAdapterHelperWrapper::CreateOhosImageDecoderAdapter() {
  return std::make_unique<NWeb::OhosImageDecoderAdapterImpl>();
}

std::unique_ptr<NWeb::SensorAdapter>
ArkOhosAdapterHelperWrapper::CreateSensorAdapter() {
  return std::make_unique<NWeb::SensorAdapterImpl>();
}

void ArkOhosAdapterHelperWrapper::SetArkWebCoreHapPathOverride(
    const std::string& hapPath) {
  if (CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    WVLOG_E("set arkweb core hap path override failed, ref ptr is null");
    return;
  }

  ArkWebString str = ArkWebStringClassToStruct(hapPath);
  ctocpp_->SetArkWebCoreHapPathOverride(str);
  ArkWebStringStructRelease(str);
}

NWeb::OhosNativeBufferAdapter&
ArkOhosAdapterHelperWrapper::GetOhosNativeBufferAdapter() {
  return OhosNativeBufferAdapterImpl::GetInstance();
}

std::unique_ptr<NWeb::AudioCodecDecoderAdapter>
ArkOhosAdapterHelperWrapper::CreateAudioCodecDecoderAdapter() {
  return std::make_unique<AudioCodecDecoderAdapterImpl>();
}

std::unique_ptr<NWeb::DrmAdapter>
ArkOhosAdapterHelperWrapper::CreateDrmAdapter() {
  return std::make_unique<DrmAdapterImpl>();
}

NWeb::OhosDrawingTextFontAdapter&
ArkOhosAdapterHelperWrapper::GetOhosDrawingTextFontAdapter() {
  return OhosDrawingTextFontAdapterImpl::GetInstance();
}

NWeb::OhosDrawingTextTypographyAdapter&
ArkOhosAdapterHelperWrapper::GetOhosDrawingTextTypographyAdapter() {
  return OhosDrawingTextTypographyAdapterImpl::GetInstance();
}

std::unique_ptr<NWeb::MigrationManagerAdapter>
ArkOhosAdapterHelperWrapper::CreateMigrationMgrAdapter() {
  if (CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    return nullptr;
  }

  ArkWebRefPtr<ArkMigrationManagerAdapter> adapter =
      ctocpp_->CreateMigrationMgrAdapter();

  if (CHECK_REF_PTR_IS_NULL(adapter)) {
    return nullptr;
  }

  return std::make_unique<ArkMigrationManagerAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::ScreenlockManagerAdapter>
ArkOhosAdapterHelperWrapper::CreateScreenlockManagerAdapter() {
  if (CHECK_REF_PTR_IS_NULL(ctocpp_)) {
    return nullptr;
  }

  ArkWebRefPtr<ArkScreenlockManagerAdapter> adapter =
      ctocpp_->CreateScreenlockManagerAdapter();

  if (CHECK_REF_PTR_IS_NULL(adapter)) {
    return nullptr;
  }

  return std::make_unique<ArkScreenlockManagerAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::NetConfigAdapter>
ArkOhosAdapterHelperWrapper::GetNetConfigAdapter() {
  return std::make_unique<NetConfigAdapterImpl>();
}

std::unique_ptr<NWeb::BackgroundTaskAdapter>
ArkOhosAdapterHelperWrapper::CreateBackgroundTaskAdapter() {
  ArkWebRefPtr<ArkBackgroundTaskAdapter> adapter =
      ctocpp_->CreateBackgroundTaskAdapter();

  if (CHECK_REF_PTR_IS_NULL(adapter)) {
    return nullptr;
  }

  return std::make_unique<ArkBackgroundTaskAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::NetEventAdapter>
ArkOhosAdapterHelperWrapper::CreateNetEventAdapter() {
  return std::make_unique<NetEventAdapterImpl>();
}

NWeb::HiAppeventAdapter& ArkOhosAdapterHelperWrapper::GetHiAppeventAdapterInstance() {
  return HiAppeventAdapterImpl::GetInstance();
}
}  // namespace OHOS::ArkWeb
