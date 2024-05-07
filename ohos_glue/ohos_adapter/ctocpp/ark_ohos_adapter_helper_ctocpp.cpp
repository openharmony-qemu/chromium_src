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

#include "ohos_adapter/ctocpp/ark_ohos_adapter_helper_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/bridge/ark_web_adapter_bridge_helper.h"
#include "ohos_adapter/ctocpp/ark_aafwk_app_mgr_client_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_access_token_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_audio_capturer_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_audio_renderer_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_audio_system_manager_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_battery_mgr_client_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_camera_manager_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_cert_manager_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_datashare_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_date_time_format_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_display_manager_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_enterprise_device_management_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_event_handler_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_flowbuffer_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_hi_sys_event_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_hi_trace_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_iconsumer_surface_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_imfadapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_keystore_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_media_avsession_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_media_codec_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_media_codec_decoder_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_media_codec_list_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_mmiadapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_native_image_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_net_connect_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_net_proxy_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_ohos_init_web_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_ohos_resource_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_ohos_web_data_base_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_ohos_web_permission_data_base_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_paste_board_client_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_player_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_power_mgr_client_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_print_manager_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_screen_capture_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_soc_perf_client_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_system_properties_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_vsync_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_window_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

using ArkOhosAdapterHelperGetInstanceFunc =
    ark_ohos_adapter_helper_t *(*)(void);
static ArkOhosAdapterHelperGetInstanceFunc
    ark_ohos_adapter_helper_get_instance = nullptr;

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkOhosAdapterHelper> ArkOhosAdapterHelper::GetInstance() {
  ARK_WEB_CTOCPP_DV_LOG();

  if (!ark_ohos_adapter_helper_get_instance) {
    ark_ohos_adapter_helper_get_instance =
        reinterpret_cast<ArkOhosAdapterHelperGetInstanceFunc>(
            ArkWebAdapterBridgeHelper::GetInstance().LoadFuncSymbol(
                "ark_ohos_adapter_helper_get_instance_static"));
    if (!ark_ohos_adapter_helper_get_instance) {
      ARK_WEB_CTOCPP_WRAN_LOG("failed to get static function symbol");
      return nullptr;
    }
  }

  // Execute
  ark_ohos_adapter_helper_t *_retval = ark_ohos_adapter_helper_get_instance();

  // Return type: refptr_same
  return ArkOhosAdapterHelperCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkAafwkAppMgrClientAdapter>
ArkOhosAdapterHelperCToCpp::CreateAafwkAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_aafwk_adapter, nullptr);

  // Execute
  ark_aafwk_app_mgr_client_adapter_t *_retval =
      _struct->create_aafwk_adapter(_struct);

  // Return type: refptr_same
  return ArkAafwkAppMgrClientAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkPowerMgrClientAdapter>
ArkOhosAdapterHelperCToCpp::CreatePowerMgrClientAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_power_mgr_client_adapter,
                                   nullptr);

  // Execute
  ark_power_mgr_client_adapter_t *_retval =
      _struct->create_power_mgr_client_adapter(_struct);

  // Return type: refptr_same
  return ArkPowerMgrClientAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkDisplayManagerAdapter>
ArkOhosAdapterHelperCToCpp::CreateDisplayMgrAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_display_mgr_adapter,
                                   nullptr);

  // Execute
  ark_display_manager_adapter_t *_retval =
      _struct->create_display_mgr_adapter(_struct);

  // Return type: refptr_same
  return ArkDisplayManagerAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkBatteryMgrClientAdapter>
ArkOhosAdapterHelperCToCpp::CreateBatteryClientAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_battery_client_adapter,
                                   nullptr);

  // Execute
  ark_battery_mgr_client_adapter_t *_retval =
      _struct->create_battery_client_adapter(_struct);

  // Return type: refptr_same
  return ArkBatteryMgrClientAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkNetConnectAdapter>
ArkOhosAdapterHelperCToCpp::CreateNetConnectAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_net_connect_adapter,
                                   nullptr);

  // Execute
  ark_net_connect_adapter_t *_retval =
      _struct->create_net_connect_adapter(_struct);

  // Return type: refptr_same
  return ArkNetConnectAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkOhosWebDataBaseAdapter>
ArkOhosAdapterHelperCToCpp::GetOhosWebDataBaseAdapterInstance() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(
      _struct, get_ohos_web_data_base_adapter_instance, nullptr);

  // Execute
  ark_ohos_web_data_base_adapter_t *_retval =
      _struct->get_ohos_web_data_base_adapter_instance(_struct);

  // Return type: refptr_same
  return ArkOhosWebDataBaseAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkPasteBoardClientAdapter>
ArkOhosAdapterHelperCToCpp::GetPasteBoard() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_paste_board, nullptr);

  // Execute
  ark_paste_board_client_adapter_t *_retval = _struct->get_paste_board(_struct);

  // Return type: refptr_same
  return ArkPasteBoardClientAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkAudioRendererAdapter>
ArkOhosAdapterHelperCToCpp::CreateAudioRendererAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_audio_renderer_adapter,
                                   nullptr);

  // Execute
  ark_audio_renderer_adapter_t *_retval =
      _struct->create_audio_renderer_adapter(_struct);

  // Return type: refptr_same
  return ArkAudioRendererAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkAudioCapturerAdapter>
ArkOhosAdapterHelperCToCpp::CreateAudioCapturerAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_audio_capturer_adapter,
                                   nullptr);

  // Execute
  ark_audio_capturer_adapter_t *_retval =
      _struct->create_audio_capturer_adapter(_struct);

  // Return type: refptr_same
  return ArkAudioCapturerAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkAudioSystemManagerAdapter>
ArkOhosAdapterHelperCToCpp::GetAudioSystemManager() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_audio_system_manager, nullptr);

  // Execute
  ark_audio_system_manager_adapter_t *_retval =
      _struct->get_audio_system_manager(_struct);

  // Return type: refptr_same
  return ArkAudioSystemManagerAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkOhosWebPermissionDataBaseAdapter>
ArkOhosAdapterHelperCToCpp::GetWebPermissionDataBaseInstance() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(
      _struct, get_web_permission_data_base_instance, nullptr);

  // Execute
  ark_ohos_web_permission_data_base_adapter_t *_retval =
      _struct->get_web_permission_data_base_instance(_struct);

  // Return type: refptr_same
  return ArkOhosWebPermissionDataBaseAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkMMIAdapter> ArkOhosAdapterHelperCToCpp::CreateMMIAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_mmiadapter, nullptr);

  // Execute
  ark_mmiadapter_t *_retval = _struct->create_mmiadapter(_struct);

  // Return type: refptr_same
  return ArkMMIAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkSocPerfClientAdapter>
ArkOhosAdapterHelperCToCpp::CreateSocPerfClientAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_soc_perf_client_adapter,
                                   nullptr);

  // Execute
  ark_soc_perf_client_adapter_t *_retval =
      _struct->create_soc_perf_client_adapter(_struct);

  // Return type: refptr_same
  return ArkSocPerfClientAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkOhosResourceAdapter>
ArkOhosAdapterHelperCToCpp::GetResourceAdapter(const ArkWebString &hapPath) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_resource_adapter, nullptr);

  // Execute
  ark_ohos_resource_adapter_t *_retval =
      _struct->get_resource_adapter(_struct, &hapPath);

  // Return type: refptr_same
  return ArkOhosResourceAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkSystemPropertiesAdapter>
ArkOhosAdapterHelperCToCpp::GetSystemPropertiesInstance() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_system_properties_instance,
                                   nullptr);

  // Execute
  ark_system_properties_adapter_t *_retval =
      _struct->get_system_properties_instance(_struct);

  // Return type: refptr_same
  return ArkSystemPropertiesAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkVSyncAdapter> ArkOhosAdapterHelperCToCpp::GetVSyncAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_vsync_adapter, nullptr);

  // Execute
  ark_vsync_adapter_t *_retval = _struct->get_vsync_adapter(_struct);

  // Return type: refptr_same
  return ArkVSyncAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkOhosInitWebAdapter>
ArkOhosAdapterHelperCToCpp::GetInitWebAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_init_web_adapter, nullptr);

  // Execute
  ark_ohos_init_web_adapter_t *_retval = _struct->get_init_web_adapter(_struct);

  // Return type: refptr_same
  return ArkOhosInitWebAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkKeystoreAdapter>
ArkOhosAdapterHelperCToCpp::GetKeystoreAdapterInstance() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_keystore_adapter_instance,
                                   nullptr);

  // Execute
  ark_keystore_adapter_t *_retval =
      _struct->get_keystore_adapter_instance(_struct);

  // Return type: refptr_same
  return ArkKeystoreAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkEnterpriseDeviceManagementAdapter>
ArkOhosAdapterHelperCToCpp::GetEnterpriseDeviceManagementInstance() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(
      _struct, get_enterprise_device_management_instance, nullptr);

  // Execute
  ark_enterprise_device_management_adapter_t *_retval =
      _struct->get_enterprise_device_management_instance(_struct);

  // Return type: refptr_same
  return ArkEnterpriseDeviceManagementAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkDatashareAdapter>
ArkOhosAdapterHelperCToCpp::GetDatashareInstance() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_datashare_instance, nullptr);

  // Execute
  ark_datashare_adapter_t *_retval = _struct->get_datashare_instance(_struct);

  // Return type: refptr_same
  return ArkDatashareAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkIMFAdapter> ArkOhosAdapterHelperCToCpp::CreateIMFAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_imfadapter, nullptr);

  // Execute
  ark_imfadapter_t *_retval = _struct->create_imfadapter(_struct);

  // Return type: refptr_same
  return ArkIMFAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkCertManagerAdapter>
ArkOhosAdapterHelperCToCpp::GetRootCertDataAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_root_cert_data_adapter,
                                   nullptr);

  // Execute
  ark_cert_manager_adapter_t *_retval =
      _struct->get_root_cert_data_adapter(_struct);

  // Return type: refptr_same
  return ArkCertManagerAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkAccessTokenAdapter>
ArkOhosAdapterHelperCToCpp::GetAccessTokenAdapterInstance() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_access_token_adapter_instance,
                                   nullptr);

  // Execute
  ark_access_token_adapter_t *_retval =
      _struct->get_access_token_adapter_instance(_struct);

  // Return type: refptr_same
  return ArkAccessTokenAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkEventHandlerAdapter>
ArkOhosAdapterHelperCToCpp::GetEventHandlerAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_event_handler_adapter, nullptr);

  // Execute
  ark_event_handler_adapter_t *_retval =
      _struct->get_event_handler_adapter(_struct);

  // Return type: refptr_same
  return ArkEventHandlerAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkPrintManagerAdapter>
ArkOhosAdapterHelperCToCpp::GetPrintManagerInstance() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_print_manager_instance,
                                   nullptr);

  // Execute
  ark_print_manager_adapter_t *_retval =
      _struct->get_print_manager_instance(_struct);

  // Return type: refptr_same
  return ArkPrintManagerAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkIConsumerSurfaceAdapter>
ArkOhosAdapterHelperCToCpp::CreateConsumerSurfaceAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_consumer_surface_adapter,
                                   nullptr);

  // Execute
  ark_iconsumer_surface_adapter_t *_retval =
      _struct->create_consumer_surface_adapter(_struct);

  // Return type: refptr_same
  return ArkIConsumerSurfaceAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkPlayerAdapter>
ArkOhosAdapterHelperCToCpp::CreatePlayerAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_player_adapter, nullptr);

  // Execute
  ark_player_adapter_t *_retval = _struct->create_player_adapter(_struct);

  // Return type: refptr_same
  return ArkPlayerAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkWindowAdapter>
ArkOhosAdapterHelperCToCpp::GetWindowAdapterInstance() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_window_adapter_instance,
                                   nullptr);

  // Execute
  ark_window_adapter_t *_retval = _struct->get_window_adapter_instance(_struct);

  // Return type: refptr_same
  return ArkWindowAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkHiSysEventAdapter>
ArkOhosAdapterHelperCToCpp::GetHiSysEventAdapterInstance() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_hi_sys_event_adapter_instance,
                                   nullptr);

  // Execute
  ark_hi_sys_event_adapter_t *_retval =
      _struct->get_hi_sys_event_adapter_instance(_struct);

  // Return type: refptr_same
  return ArkHiSysEventAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkHiTraceAdapter>
ArkOhosAdapterHelperCToCpp::GetHiTraceAdapterInstance() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_hi_trace_adapter_instance,
                                   nullptr);

  // Execute
  ark_hi_trace_adapter_t *_retval =
      _struct->get_hi_trace_adapter_instance(_struct);

  // Return type: refptr_same
  return ArkHiTraceAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkNetProxyAdapter>
ArkOhosAdapterHelperCToCpp::GetNetProxyInstance() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_net_proxy_instance, nullptr);

  // Execute
  ark_net_proxy_adapter_t *_retval = _struct->get_net_proxy_instance(_struct);

  // Return type: refptr_same
  return ArkNetProxyAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkCameraManagerAdapter>
ArkOhosAdapterHelperCToCpp::GetCameraManagerAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_camera_manager_adapter,
                                   nullptr);

  // Execute
  ark_camera_manager_adapter_t *_retval =
      _struct->get_camera_manager_adapter(_struct);

  // Return type: refptr_same
  return ArkCameraManagerAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkScreenCaptureAdapter>
ArkOhosAdapterHelperCToCpp::CreateScreenCaptureAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_screen_capture_adapter,
                                   nullptr);

  // Execute
  ark_screen_capture_adapter_t *_retval =
      _struct->create_screen_capture_adapter(_struct);

  // Return type: refptr_same
  return ArkScreenCaptureAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkDateTimeFormatAdapter>
ArkOhosAdapterHelperCToCpp::CreateDateTimeFormatAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_date_time_format_adapter,
                                   nullptr);

  // Execute
  ark_date_time_format_adapter_t *_retval =
      _struct->create_date_time_format_adapter(_struct);

  // Return type: refptr_same
  return ArkDateTimeFormatAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkMediaCodecDecoderAdapter>
ArkOhosAdapterHelperCToCpp::CreateMediaCodecDecoderAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_media_codec_decoder_adapter,
                                   nullptr);

  // Execute
  ark_media_codec_decoder_adapter_t *_retval =
      _struct->create_media_codec_decoder_adapter(_struct);

  // Return type: refptr_same
  return ArkMediaCodecDecoderAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkNativeImageAdapter>
ArkOhosAdapterHelperCToCpp::CreateNativeImageAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_native_image_adapter,
                                   nullptr);

  // Execute
  ark_native_image_adapter_t *_retval =
      _struct->create_native_image_adapter(_struct);

  // Return type: refptr_same
  return ArkNativeImageAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkMediaCodecAdapter>
ArkOhosAdapterHelperCToCpp::CreateMediaCodecEncoderAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_media_codec_encoder_adapter,
                                   nullptr);

  // Execute
  ark_media_codec_adapter_t *_retval =
      _struct->create_media_codec_encoder_adapter(_struct);

  // Return type: refptr_same
  return ArkMediaCodecAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkMediaCodecListAdapter>
ArkOhosAdapterHelperCToCpp::GetMediaCodecListAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_media_codec_list_adapter,
                                   nullptr);

  // Execute
  ark_media_codec_list_adapter_t *_retval =
      _struct->get_media_codec_list_adapter(_struct);

  // Return type: refptr_same
  return ArkMediaCodecListAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkFlowbufferAdapter>
ArkOhosAdapterHelperCToCpp::CreateFlowbufferAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_flowbuffer_adapter, nullptr);

  // Execute
  ark_flowbuffer_adapter_t *_retval =
      _struct->create_flowbuffer_adapter(_struct);

  // Return type: refptr_same
  return ArkFlowbufferAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkMediaAVSessionAdapter>
ArkOhosAdapterHelperCToCpp::CreateMediaAVSessionAdapter() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_ohos_adapter_helper_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, create_media_avsession_adapter,
                                   nullptr);

  // Execute
  ark_media_avsession_adapter_t *_retval =
      _struct->create_media_avsession_adapter(_struct);

  // Return type: refptr_same
  return ArkMediaAVSessionAdapterCToCpp::Invert(_retval);
}

ArkOhosAdapterHelperCToCpp::ArkOhosAdapterHelperCToCpp() {
}

ArkOhosAdapterHelperCToCpp::~ArkOhosAdapterHelperCToCpp() {
}

template <>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkOhosAdapterHelperCToCpp, ArkOhosAdapterHelper,
                           ark_ohos_adapter_helper_t>::kBridgeType =
        ARK_OHOS_ADAPTER_HELPER;

} // namespace OHOS::ArkWeb
