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

#ifndef ARK_OHOS_ADAPTER_HELPER_CAPI_H_
#define ARK_OHOS_ADAPTER_HELPER_CAPI_H_
#pragma once

#include "ohos_adapter/capi/ark_aafwk_app_mgr_client_adapter_capi.h"
#include "ohos_adapter/capi/ark_access_token_adapter_capi.h"
#include "ohos_adapter/capi/ark_audio_capturer_adapter_capi.h"
#include "ohos_adapter/capi/ark_audio_renderer_adapter_capi.h"
#include "ohos_adapter/capi/ark_audio_system_manager_adapter_capi.h"
#include "ohos_adapter/capi/ark_battery_mgr_client_adapter_capi.h"
#include "ohos_adapter/capi/ark_camera_manager_adapter_capi.h"
#include "ohos_adapter/capi/ark_cert_manager_adapter_capi.h"
#include "ohos_adapter/capi/ark_datashare_adapter_capi.h"
#include "ohos_adapter/capi/ark_date_time_format_adapter_capi.h"
#include "ohos_adapter/capi/ark_display_manager_adapter_capi.h"
#include "ohos_adapter/capi/ark_enterprise_device_management_adapter_capi.h"
#include "ohos_adapter/capi/ark_event_handler_adapter_capi.h"
#include "ohos_adapter/capi/ark_flowbuffer_adapter_capi.h"
#include "ohos_adapter/capi/ark_graphic_adapter_capi.h"
#include "ohos_adapter/capi/ark_hisysevent_adapter_capi.h"
#include "ohos_adapter/capi/ark_hitrace_adapter_capi.h"
#include "ohos_adapter/capi/ark_imf_adapter_capi.h"
#include "ohos_adapter/capi/ark_keystore_adapter_capi.h"
#include "ohos_adapter/capi/ark_media_adapter_capi.h"
#include "ohos_adapter/capi/ark_media_avsession_adapter_capi.h"
#include "ohos_adapter/capi/ark_media_codec_adapter_capi.h"
#include "ohos_adapter/capi/ark_media_codec_decoder_adapter_capi.h"
#include "ohos_adapter/capi/ark_mmi_adapter_capi.h"
#include "ohos_adapter/capi/ark_net_connect_adapter_capi.h"
#include "ohos_adapter/capi/ark_net_proxy_adapter_capi.h"
#include "ohos_adapter/capi/ark_ohos_init_web_adapter_capi.h"
#include "ohos_adapter/capi/ark_ohos_resource_adapter_capi.h"
#include "ohos_adapter/capi/ark_ohos_web_data_base_adapter_capi.h"
#include "ohos_adapter/capi/ark_pasteboard_client_adapter_capi.h"
#include "ohos_adapter/capi/ark_power_mgr_client_adapter_capi.h"
#include "ohos_adapter/capi/ark_print_manager_adapter_capi.h"
#include "ohos_adapter/capi/ark_screen_capture_adapter_capi.h"
#include "ohos_adapter/capi/ark_soc_perf_client_adapter_capi.h"
#include "ohos_adapter/capi/ark_system_properties_adapter_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_ohos_adapter_helper_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  ark_aafwk_app_mgr_client_adapter_t *(ARK_WEB_CALLBACK *create_aafwk_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_power_mgr_client_adapter_t *(
      ARK_WEB_CALLBACK *create_power_mgr_client_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_display_manager_adapter_t *(ARK_WEB_CALLBACK *create_display_mgr_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_battery_mgr_client_adapter_t *(
      ARK_WEB_CALLBACK *create_battery_client_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_net_connect_adapter_t *(ARK_WEB_CALLBACK *create_net_connect_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_ohos_web_data_base_adapter_t *(
      ARK_WEB_CALLBACK *get_ohos_web_data_base_adapter_instance)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_paste_board_client_adapter_t *(ARK_WEB_CALLBACK *get_paste_board)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_audio_renderer_adapter_t *(
      ARK_WEB_CALLBACK *create_audio_renderer_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_audio_capturer_adapter_t *(
      ARK_WEB_CALLBACK *create_audio_capturer_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_audio_system_manager_adapter_t *(
      ARK_WEB_CALLBACK *get_audio_system_manager)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_ohos_web_permission_data_base_adapter_t *(
      ARK_WEB_CALLBACK *get_web_permission_data_base_instance)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_mmiadapter_t *(ARK_WEB_CALLBACK *create_mmiadapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_soc_perf_client_adapter_t *(
      ARK_WEB_CALLBACK *create_soc_perf_client_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_ohos_resource_adapter_t *(ARK_WEB_CALLBACK *get_resource_adapter)(
      struct _ark_ohos_adapter_helper_t *self, const ArkWebString *hapPath);

  ark_system_properties_adapter_t *(
      ARK_WEB_CALLBACK *get_system_properties_instance)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_vsync_adapter_t *(ARK_WEB_CALLBACK *get_vsync_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_ohos_init_web_adapter_t *(ARK_WEB_CALLBACK *get_init_web_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_keystore_adapter_t *(ARK_WEB_CALLBACK *get_keystore_adapter_instance)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_enterprise_device_management_adapter_t *(
      ARK_WEB_CALLBACK *get_enterprise_device_management_instance)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_datashare_adapter_t *(ARK_WEB_CALLBACK *get_datashare_instance)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_imfadapter_t *(ARK_WEB_CALLBACK *create_imfadapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_cert_manager_adapter_t *(ARK_WEB_CALLBACK *get_root_cert_data_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_access_token_adapter_t *(
      ARK_WEB_CALLBACK *get_access_token_adapter_instance)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_event_handler_adapter_t *(ARK_WEB_CALLBACK *get_event_handler_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_print_manager_adapter_t *(ARK_WEB_CALLBACK *get_print_manager_instance)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_iconsumer_surface_adapter_t *(
      ARK_WEB_CALLBACK *create_consumer_surface_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_player_adapter_t *(ARK_WEB_CALLBACK *create_player_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_window_adapter_t *(ARK_WEB_CALLBACK *get_window_adapter_instance)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_hi_sys_event_adapter_t *(
      ARK_WEB_CALLBACK *get_hi_sys_event_adapter_instance)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_hi_trace_adapter_t *(ARK_WEB_CALLBACK *get_hi_trace_adapter_instance)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_net_proxy_adapter_t *(ARK_WEB_CALLBACK *get_net_proxy_instance)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_camera_manager_adapter_t *(ARK_WEB_CALLBACK *get_camera_manager_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_screen_capture_adapter_t *(
      ARK_WEB_CALLBACK *create_screen_capture_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_date_time_format_adapter_t *(
      ARK_WEB_CALLBACK *create_date_time_format_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_media_codec_decoder_adapter_t *(
      ARK_WEB_CALLBACK *create_media_codec_decoder_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_native_image_adapter_t *(ARK_WEB_CALLBACK *create_native_image_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_media_codec_adapter_t *(
      ARK_WEB_CALLBACK *create_media_codec_encoder_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_media_codec_list_adapter_t *(
      ARK_WEB_CALLBACK *get_media_codec_list_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_flowbuffer_adapter_t *(ARK_WEB_CALLBACK *create_flowbuffer_adapter)(
      struct _ark_ohos_adapter_helper_t *self);

  ark_media_avsession_adapter_t *(
      ARK_WEB_CALLBACK *create_media_avsession_adapter)(
      struct _ark_ohos_adapter_helper_t *self);
} ark_ohos_adapter_helper_t;

ARK_WEB_EXPORT ark_ohos_adapter_helper_t *
ark_ohos_adapter_helper_get_instance(void);

#ifdef __cplusplus
}
#endif

#endif // ARK_OHOS_ADAPTER_HELPER_CAPI_H_
