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

#include "arkweb/ohos_adapter_ndk/interfaces/ndk_ohos_adapter_helper.h"

#include "arkweb/ohos_adapter_ndk/camera_adapter/include/camera_manager_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/inputmethodframework_adapter/imf_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/include/media_codec_list_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"
#include "ohos_adapter/bridge/ark_enterprise_device_management_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_hisysevent_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_drawing_text_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_print_manager_adapter_wrapper.h"

#include "arkweb/ohos_adapter_ndk/event_handler_adapter/event_handler_adapter_impl.h"
#include "datashare_adapter/datashare_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/distributeddatamgr_adapter/ohos_web_data_base_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/distributeddatamgr_adapter/ohos_web_permission_data_base_adapter_impl.h"

#include "arkweb/ohos_adapter_ndk/ohos_native_buffer_adapter/ohos_native_buffer_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/cert_mgr_adapter/cert_mgr_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_avsession_adapter/media_avsession_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/audio_capturer_adapter/audio_capturer_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/screen_capture_adapter/screen_capture_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/audio_capturer_adapter/audio_system_manager_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/access_token_adapter/access_token_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/player_framework_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/multimodalinputnew_adapter/mmi_new_adapter_impl.h"
#include "pasteboard_adapter/include/pasteboard_client_adapter_impl.h"

#include "arkweb/ohos_adapter_ndk/display_manager_adapter/native_display_manager_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/sensor_adapter/sensor_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/battery_mgr_adapter/battery_mgr_client_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/drm_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/audio_codec_decoder_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/media_codec_decoder_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/ohos_image_adapter/ohos_image_decoder_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/keystore_adapter/keystore_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/date_time_format_adapter/date_time_format_adapter_impl.h"

#include "arkweb/ohos_adapter_ndk/net_connect_adapter/net_connect_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/net_proxy_adapter_impl/net_proxy_adapter_impl.h"

#include "arkweb/ohos_adapter_ndk/media_adapter/include/video_encoder_adapter_impl.h"
#include "hiviewdfx_adapter/hitrace_adapter_impl.h"

#include "arkweb/ohos_adapter_ndk/drawing_text_adapter/ohos_drawing_text_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/ohos_resource_adapter/ohos_resource_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/system_properties_adapter/system_properties_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/aafwk_adapter/aafwk_app_mgr_client_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_image_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_window_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/graphic_adapter/vsync_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/graphic_adapter/window_adapter_impl.h"

namespace OHOS::NWeb {

OhosAdapterHelper& OhosAdapterHelper::GetInstance() {
  static ArkWeb::NDKOhosAdapterHelper instance;
  return instance;
}

}  // namespace OHOS::NWeb

namespace OHOS::ArkWeb {

NDKOhosAdapterHelper::NDKOhosAdapterHelper()
    {}

std::unique_ptr<NWeb::AafwkAppMgrClientAdapter>
NDKOhosAdapterHelper::CreateAafwkAdapter() {
  return std::make_unique<AafwkAppMgrClientAdapterImpl>();
}

std::unique_ptr<NWeb::PowerMgrClientAdapter>
NDKOhosAdapterHelper::CreatePowerMgrClientAdapter() {
  return nullptr;
}

std::unique_ptr<NWeb::DisplayManagerAdapter>
NDKOhosAdapterHelper::CreateDisplayMgrAdapter() {
  return std::make_unique<NativeDisplayManagerAdapterImpl>();
}

std::unique_ptr<NWeb::BatteryMgrClientAdapter>
NDKOhosAdapterHelper::CreateBatteryClientAdapter() {
  return std::make_unique<NWeb::BatteryMgrClientAdapterImpl>();
}

std::unique_ptr<NWeb::NetConnectAdapter>
NDKOhosAdapterHelper::CreateNetConnectAdapter() {
  return std::make_unique<NetConnectAdapterImpl>();
}

NWeb::OhosWebDataBaseAdapter&
NDKOhosAdapterHelper::GetOhosWebDataBaseAdapterInstance() {
  return OhosWebDataBaseAdapterImpl::GetInstance();
}

NWeb::PasteBoardClientAdapter& NDKOhosAdapterHelper::GetPasteBoard() {
  static PasteBoardClientAdapterImpl instance;
  return instance;
}

std::unique_ptr<NWeb::AudioRendererAdapter>
NDKOhosAdapterHelper::CreateAudioRendererAdapter() {
  return nullptr;
}

std::unique_ptr<NWeb::AudioCapturerAdapter>
NDKOhosAdapterHelper::CreateAudioCapturerAdapter() {
  return std::make_unique<NWeb::AudioCapturerAdapterImpl>();
}

NWeb::AudioSystemManagerAdapter&
NDKOhosAdapterHelper::GetAudioSystemManager() {
  return AudioSystemManagerAdapterImpl::GetInstance();
}

NWeb::OhosWebPermissionDataBaseAdapter&
NDKOhosAdapterHelper::GetWebPermissionDataBaseInstance() {
  return OhosWebPermissionDataBaseAdapterImpl::GetInstance();
}

std::unique_ptr<NWeb::MMIAdapter>
NDKOhosAdapterHelper::CreateMMIAdapter() {
  return nullptr;
}

std::unique_ptr<NWeb::SocPerfClientAdapter>
NDKOhosAdapterHelper::CreateSocPerfClientAdapter() {
  return nullptr;
}

std::unique_ptr<NWeb::OhosResourceAdapter>
NDKOhosAdapterHelper::GetResourceAdapter(const std::string& hapPath) {
  return std::make_unique<NWeb::OhosResourceAdapterImpl>(hapPath);
}

NWeb::SystemPropertiesAdapter&
NDKOhosAdapterHelper::GetSystemPropertiesInstance() {
  return SystemPropertiesAdapterImpl::GetInstance();
}

NWeb::VSyncAdapter& NDKOhosAdapterHelper::GetVSyncAdapter() {
  return VSyncAdapterNdkImpl::GetInstance();
}

std::unique_ptr<NWeb::OhosInitWebAdapter>
NDKOhosAdapterHelper::GetInitWebAdapter() {
  return nullptr;
}

NWeb::KeystoreAdapter&
NDKOhosAdapterHelper::GetKeystoreAdapterInstance() {
  return KeystoreAdapterImpl::GetInstance();
}

NWeb::EnterpriseDeviceManagementAdapter&
NDKOhosAdapterHelper::GetEnterpriseDeviceManagementInstance() {
  static ArkEnterpriseDeviceManagementAdapterWrapper instance(nullptr);
  return instance;
}

NWeb::DatashareAdapter& NDKOhosAdapterHelper::GetDatashareInstance() {
  return DatashareAdapterImpl::GetInstance();
}

std::unique_ptr<NWeb::IMFAdapter>
NDKOhosAdapterHelper::CreateIMFAdapter() {
  return nullptr;
}

std::unique_ptr<NWeb::CertManagerAdapter>
NDKOhosAdapterHelper::GetRootCertDataAdapter() {
  return std::make_unique<CertManagerAdapterImpl>();
}

NWeb::AccessTokenAdapter&
NDKOhosAdapterHelper::GetAccessTokenAdapterInstance() {
  return AccessTokenAdapterImpl::GetInstance();
}

std::unique_ptr<NWeb::EventHandlerAdapter>
NDKOhosAdapterHelper::GetEventHandlerAdapter() {
  return std::make_unique<NWeb::EventHandlerAdapterImpl>();
}

NWeb::PrintManagerAdapter&
NDKOhosAdapterHelper::GetPrintManagerInstance() {
  static ArkPrintManagerAdapterWrapper instance(nullptr);
  return instance;
}

std::unique_ptr<NWeb::IConsumerSurfaceAdapter>
NDKOhosAdapterHelper::CreateConsumerSurfaceAdapter() {
  return std::make_unique<NWeb::ConsumerNativeAdapterImpl>();
}

std::unique_ptr<NWeb::PlayerAdapter>
NDKOhosAdapterHelper::CreatePlayerAdapter() {
  return nullptr;
}

NWeb::WindowAdapter& NDKOhosAdapterHelper::GetWindowAdapterInstance() {
  return WindowAdapterNdkImpl::GetInstance();
}

NWeb::HiSysEventAdapter&
NDKOhosAdapterHelper::GetHiSysEventAdapterInstance() {
  static ArkHiSysEventAdapterWrapper instance(nullptr);
  return instance;
}

NWeb::HiTraceAdapter& NDKOhosAdapterHelper::GetHiTraceAdapterInstance() {
  return HiTraceAdapterImpl::GetInstance();
}

NWeb::NetProxyAdapter& NDKOhosAdapterHelper::GetNetProxyInstance() {
  return NetProxyAdapterImpl::GetInstance();
}

NWeb::CameraManagerAdapter&
NDKOhosAdapterHelper::GetCameraManagerAdapter() {
  return CameraManagerAdapterImpl::GetInstance();
}

std::unique_ptr<NWeb::ScreenCaptureAdapter>
NDKOhosAdapterHelper::CreateScreenCaptureAdapter() {
  return std::make_unique<NWeb::ScreenCaptureAdapterImpl>();
}

std::unique_ptr<NWeb::DateTimeFormatAdapter>
NDKOhosAdapterHelper::CreateDateTimeFormatAdapter() {
  return std::make_unique<DateTimeFormatAdapterImpl>();
}

std::unique_ptr<NWeb::MediaCodecDecoderAdapter>
NDKOhosAdapterHelper::CreateMediaCodecDecoderAdapter() {
  return std::make_unique<NWeb::MediaCodecDecoderAdapterImpl>();
}

std::unique_ptr<NWeb::NativeImageAdapter>
NDKOhosAdapterHelper::CreateNativeImageAdapter() {
  return std::make_unique<NativeImageAdapterImpl>();
}

std::unique_ptr<NWeb::MediaCodecAdapter>
NDKOhosAdapterHelper::CreateMediaCodecEncoderAdapter() {
  return std::make_unique<NWeb::VideoEncoderAdapterImpl>();
}

NWeb::MediaCodecListAdapter&
NDKOhosAdapterHelper::GetMediaCodecListAdapter() {
  return MediaCodecListAdapterImpl::GetInstance();
}

std::unique_ptr<NWeb::FlowbufferAdapter>
NDKOhosAdapterHelper::CreateFlowbufferAdapter() {
  return nullptr;
}

std::unique_ptr<NWeb::MediaAVSessionAdapter>
NDKOhosAdapterHelper::CreateMediaAVSessionAdapter() {
  return std::make_unique<NWeb::MediaAVSessionAdapterImpl>();
}

std::unique_ptr<NWeb::OhosImageDecoderAdapter>
NDKOhosAdapterHelper::CreateOhosImageDecoderAdapter() {
  return std::make_unique<NWeb::OhosImageDecoderAdapterImpl>();
}

std::unique_ptr<NWeb::SensorAdapter>
NDKOhosAdapterHelper::CreateSensorAdapter() {
  return std::make_unique<NWeb::SensorAdapterImpl>();
}

void NDKOhosAdapterHelper::SetArkWebCoreHapPathOverride(
    const std::string& hapPath) {
}

NWeb::OhosNativeBufferAdapter&
NDKOhosAdapterHelper::GetOhosNativeBufferAdapter() {
  return OhosNativeBufferAdapterImpl::GetInstance();
}

std::unique_ptr<NWeb::AudioCodecDecoderAdapter>
NDKOhosAdapterHelper::CreateAudioCodecDecoderAdapter() {
  return std::make_unique<AudioCodecDecoderAdapterImpl>();
}

std::unique_ptr<NWeb::DrmAdapter>
NDKOhosAdapterHelper::CreateDrmAdapter() {
  return std::make_unique<DrmAdapterImpl>();
}

NWeb::OhosDrawingTextFontAdapter&
NDKOhosAdapterHelper::GetOhosDrawingTextFontAdapter() {
  return OhosDrawingTextFontAdapterImpl::GetInstance();
}

NWeb::OhosDrawingTextTypographyAdapter&
NDKOhosAdapterHelper::GetOhosDrawingTextTypographyAdapter() {
  return OhosDrawingTextTypographyAdapterImpl::GetInstance();
}

std::unique_ptr<NWeb::MigrationManagerAdapter>
NDKOhosAdapterHelper::CreateMigrationMgrAdapter() {
  return nullptr;
}

}  // namespace OHOS::ArkWeb
