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

#ifndef ARK_WEB_HANDLER_CAPI_H_
#define ARK_WEB_HANDLER_CAPI_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "ohos_nweb/capi/ark_web_access_request_capi.h"
#include "ohos_nweb/capi/ark_web_console_log_capi.h"
#include "ohos_nweb/capi/ark_web_context_menu_callback_capi.h"
#include "ohos_nweb/capi/ark_web_context_menu_params_capi.h"
#include "ohos_nweb/capi/ark_web_controller_handler_capi.h"
#include "ohos_nweb/capi/ark_web_data_resubmission_callback_capi.h"
#include "ohos_nweb/capi/ark_web_date_time_chooser_callback_capi.h"
#include "ohos_nweb/capi/ark_web_file_selector_params_capi.h"
#include "ohos_nweb/capi/ark_web_full_screen_exit_handler_capi.h"
#include "ohos_nweb/capi/ark_web_geo_location_callback_capi.h"
#include "ohos_nweb/capi/ark_web_js_dialog_result_capi.h"
#include "ohos_nweb/capi/ark_web_js_http_auth_result_capi.h"
#include "ohos_nweb/capi/ark_web_js_ssl_error_result_capi.h"
#include "ohos_nweb/capi/ark_web_js_ssl_select_cert_result_capi.h"
#include "ohos_nweb/capi/ark_web_key_event_capi.h"
#include "ohos_nweb/capi/ark_web_load_committed_details_capi.h"
#include "ohos_nweb/capi/ark_web_native_embed_data_info_capi.h"
#include "ohos_nweb/capi/ark_web_native_embed_touch_event_capi.h"
#include "ohos_nweb/capi/ark_web_nweb_capi.h"
#include "ohos_nweb/capi/ark_web_quick_menu_callback_capi.h"
#include "ohos_nweb/capi/ark_web_quick_menu_params_capi.h"
#include "ohos_nweb/capi/ark_web_screen_capture_access_request_capi.h"
#include "ohos_nweb/capi/ark_web_select_popup_menu_callback_capi.h"
#include "ohos_nweb/capi/ark_web_select_popup_menu_param_capi.h"
#include "ohos_nweb/capi/ark_web_string_vector_value_callback_capi.h"
#include "ohos_nweb/capi/ark_web_touch_handle_state_capi.h"
#include "ohos_nweb/capi/ark_web_url_resource_error_capi.h"
#include "ohos_nweb/capi/ark_web_url_resource_request_capi.h"
#include "ohos_nweb/capi/ark_web_url_resource_response_capi.h"
#include "ohos_nweb/include/ark_web_date_time_suggestion_vector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_handler_t {
  /**
   * @brief Base structure.
   */
  ark_web_base_ref_counted_t base;

  void(ARK_WEB_CALLBACK *set_nweb)(struct _ark_web_handler_t *self,
                                   ark_web_nweb_t *nweb);

  bool(ARK_WEB_CALLBACK *on_focus)(struct _ark_web_handler_t *self);

  void(ARK_WEB_CALLBACK *on_message)(struct _ark_web_handler_t *self,
                                     const ArkWebString *param);

  void(ARK_WEB_CALLBACK *on_resource)(struct _ark_web_handler_t *self,
                                      const ArkWebString *url);

  void(ARK_WEB_CALLBACK *on_page_icon)(struct _ark_web_handler_t *self,
                                       const void *data, size_t width,
                                       size_t height, int color_type,
                                       int alpha_type);

  void(ARK_WEB_CALLBACK *on_page_title)(struct _ark_web_handler_t *self,
                                        const ArkWebString *title);

  void(ARK_WEB_CALLBACK *on_proxy_died)(struct _ark_web_handler_t *self);

  void(ARK_WEB_CALLBACK *on_http_error)(
      struct _ark_web_handler_t *self, ark_web_url_resource_request_t *request,
      ark_web_url_resource_response_t *error_response);

  bool(ARK_WEB_CALLBACK *on_console_log)(struct _ark_web_handler_t *self,
                                         ark_web_console_log_t *message);

  void(ARK_WEB_CALLBACK *on_router_push)(struct _ark_web_handler_t *self,
                                         const ArkWebString *param);

  void(ARK_WEB_CALLBACK *on_page_load_end)(struct _ark_web_handler_t *self,
                                           int http_status_code,
                                           const ArkWebString *url);

  void(ARK_WEB_CALLBACK *on_page_load_begin)(struct _ark_web_handler_t *self,
                                             const ArkWebString *url);

  void(ARK_WEB_CALLBACK *on_page_load_error)(struct _ark_web_handler_t *self,
                                             int error_code,
                                             const ArkWebString *description,
                                             const ArkWebString *failing_url);

  void(ARK_WEB_CALLBACK *on_desktop_icon_url)(struct _ark_web_handler_t *self,
                                              const ArkWebString *icon_url,
                                              bool precomposed);

  void(ARK_WEB_CALLBACK *on_loading_progress)(struct _ark_web_handler_t *self,
                                              int new_progress);

  void(ARK_WEB_CALLBACK *on_geolocation_show)(
      struct _ark_web_handler_t *self, const ArkWebString *origin,
      ark_web_geo_location_callback_t *callback);

  void(ARK_WEB_CALLBACK *on_geolocation_hide)(struct _ark_web_handler_t *self);

  bool(ARK_WEB_CALLBACK *on_file_selector_show)(
      struct _ark_web_handler_t *self,
      ark_web_string_vector_value_callback_t *callback,
      ark_web_file_selector_params_t *params);

  void(ARK_WEB_CALLBACK *on_resource_load_error)(
      struct _ark_web_handler_t *self, ark_web_url_resource_request_t *request,
      ark_web_url_resource_error_t *error);

  void(ARK_WEB_CALLBACK *on_permission_request)(
      struct _ark_web_handler_t *self, ark_web_access_request_t *request);

  void(ARK_WEB_CALLBACK *on_quick_menu_dismissed)(
      struct _ark_web_handler_t *self);

  void(ARK_WEB_CALLBACK *on_context_menu_dismissed)(
      struct _ark_web_handler_t *self);

  void(ARK_WEB_CALLBACK *on_touch_selection_changed)(
      struct _ark_web_handler_t *self,
      ark_web_touch_handle_state_t *insert_handle,
      ark_web_touch_handle_state_t *start_selection_handle,
      ark_web_touch_handle_state_t *end_selection_handle);

  bool(ARK_WEB_CALLBACK *on_handle_intercept_request)(
      struct _ark_web_handler_t *self, ark_web_url_resource_request_t *request,
      ark_web_url_resource_response_t *response);

  void(ARK_WEB_CALLBACK *on_refresh_accessed_history)(
      struct _ark_web_handler_t *self, const ArkWebString *url, bool is_reload);

  void(ARK_WEB_CALLBACK *on_permission_request_canceled)(
      struct _ark_web_handler_t *self, ark_web_access_request_t *request);

  bool(ARK_WEB_CALLBACK *on_handle_intercept_url_loading)(
      struct _ark_web_handler_t *self, ark_web_url_resource_request_t *request);

  bool(ARK_WEB_CALLBACK *run_quick_menu)(
      struct _ark_web_handler_t *self, ark_web_quick_menu_params_t *params,
      ark_web_quick_menu_callback_t *callback);

  bool(ARK_WEB_CALLBACK *run_context_menu)(
      struct _ark_web_handler_t *self, ark_web_context_menu_params_t *params,
      ark_web_context_menu_callback_t *callback);

  void(ARK_WEB_CALLBACK *update_drag_cursor)(struct _ark_web_handler_t *self,
                                             unsigned char op);

  bool(ARK_WEB_CALLBACK *filter_scroll_event)(struct _ark_web_handler_t *self,
                                              const float x, const float y,
                                              const float x_velocity,
                                              const float y_velocity);

  ArkWebStringVector(ARK_WEB_CALLBACK *visited_url_history)(
      struct _ark_web_handler_t *self);

  void(ARK_WEB_CALLBACK *on_window_new_by_js)(
      struct _ark_web_handler_t *self, const ArkWebString *target_url,
      bool is_alert, bool is_user_trigger,
      ark_web_controller_handler_t *handler);

  void(ARK_WEB_CALLBACK *on_window_exit_by_js)(struct _ark_web_handler_t *self);

  bool(ARK_WEB_CALLBACK *on_alert_dialog_by_js)(
      struct _ark_web_handler_t *self, const ArkWebString *url,
      const ArkWebString *message, ark_web_js_dialog_result_t *result);

  bool(ARK_WEB_CALLBACK *on_before_unload_by_js)(
      struct _ark_web_handler_t *self, const ArkWebString *url,
      const ArkWebString *message, ark_web_js_dialog_result_t *result);

  bool(ARK_WEB_CALLBACK *on_prompt_dialog_by_js)(
      struct _ark_web_handler_t *self, const ArkWebString *url,
      const ArkWebString *message, const ArkWebString *default_value,
      ark_web_js_dialog_result_t *result);

  bool(ARK_WEB_CALLBACK *on_confirm_dialog_by_js)(
      struct _ark_web_handler_t *self, const ArkWebString *url,
      const ArkWebString *message, ark_web_js_dialog_result_t *result);

  bool(ARK_WEB_CALLBACK *on_http_auth_request_by_js)(
      struct _ark_web_handler_t *self, ark_web_js_http_auth_result_t *result,
      const ArkWebString *host, const ArkWebString *realm);

  bool(ARK_WEB_CALLBACK *on_ssl_error_request_by_js)(
      struct _ark_web_handler_t *self, ark_web_js_ssl_error_result_t *result,
      int error);

  bool(ARK_WEB_CALLBACK *on_ssl_select_cert_request_by_js)(
      struct _ark_web_handler_t *self,
      ark_web_js_ssl_select_cert_result_t *result, const ArkWebString *host,
      int port, const ArkWebStringVector *key_types,
      const ArkWebStringVector *issuers);

  void(ARK_WEB_CALLBACK *on_scroll)(struct _ark_web_handler_t *self,
                                    double x_offset, double y_offset);

  void(ARK_WEB_CALLBACK *on_over_scroll)(struct _ark_web_handler_t *self,
                                         float x_offset, float y_offset);

  void(ARK_WEB_CALLBACK *on_scroll_state)(struct _ark_web_handler_t *self,
                                          bool scroll_state);

  void(ARK_WEB_CALLBACK *on_page_visible)(struct _ark_web_handler_t *self,
                                          const ArkWebString *url);

  bool(ARK_WEB_CALLBACK *on_pre_key_event)(struct _ark_web_handler_t *self,
                                           ark_web_key_event_t *event);

  void(ARK_WEB_CALLBACK *on_scale_changed)(struct _ark_web_handler_t *self,
                                           float old_scale_factor,
                                           float new_scale_factor);

  bool(ARK_WEB_CALLBACK *on_cursor_change)(struct _ark_web_handler_t *self,
                                           const int32_t *type,
                                           const ArkWebCursorInfo *info);

  void(ARK_WEB_CALLBACK *on_render_exited)(struct _ark_web_handler_t *self,
                                           int reason);

  void(ARK_WEB_CALLBACK *on_resize_not_work)(struct _ark_web_handler_t *self);

  void(ARK_WEB_CALLBACK *on_full_screen_exit)(struct _ark_web_handler_t *self);

  void(ARK_WEB_CALLBACK *on_full_screen_enter)(
      struct _ark_web_handler_t *self,
      ark_web_full_screen_exit_handler_t *handler);

  bool(ARK_WEB_CALLBACK *on_drag_and_drop_data)(struct _ark_web_handler_t *self,
                                                const void *data, size_t len,
                                                const ArkWebImageOptions *opt);

  void(ARK_WEB_CALLBACK *on_select_popup_menu)(
      struct _ark_web_handler_t *self,
      ark_web_select_popup_menu_param_t *params,
      ark_web_select_popup_menu_callback_t *callback);

  void(ARK_WEB_CALLBACK *on_data_resubmission)(
      struct _ark_web_handler_t *self,
      ark_web_data_resubmission_callback_t *handler);

  void(ARK_WEB_CALLBACK *on_root_layer_changed)(struct _ark_web_handler_t *self,
                                                int width, int height);

  void(ARK_WEB_CALLBACK *on_audio_state_changed)(
      struct _ark_web_handler_t *self, bool playing);

  void(ARK_WEB_CALLBACK *on_over_scroll_fling_end)(
      struct _ark_web_handler_t *self);

  bool(ARK_WEB_CALLBACK *on_un_processed_key_event)(
      struct _ark_web_handler_t *self, ark_web_key_event_t *event);

  bool(ARK_WEB_CALLBACK *on_drag_and_drop_data_udmf)(
      struct _ark_web_handler_t *self, ark_web_drag_data_t *drag_data);

  void(ARK_WEB_CALLBACK *on_first_contentful_paint)(
      struct _ark_web_handler_t *self, int64_t navigation_start_tick,
      int64_t first_contentful_paint_ms);

  void(ARK_WEB_CALLBACK *on_date_time_chooser_popup)(
      struct _ark_web_handler_t *self, const ArkWebDateTimeChooser *chooser,
      const ArkWebDateTimeSuggestionVector *suggestions,
      ark_web_date_time_chooser_callback_t *callback);

  void(ARK_WEB_CALLBACK *on_date_time_chooser_close)(
      struct _ark_web_handler_t *self);

  void(ARK_WEB_CALLBACK *on_screen_capture_request)(
      struct _ark_web_handler_t *self,
      ark_web_screen_capture_access_request_t *request);

  void(ARK_WEB_CALLBACK *on_activity_state_changed)(
      struct _ark_web_handler_t *self, int state, int type);

  void(ARK_WEB_CALLBACK *on_get_touch_handle_hot_zone)(
      struct _ark_web_handler_t *self, ArkWebTouchHandleHotZone *hot_zone);

  void(ARK_WEB_CALLBACK *on_complete_swap_with_new_size)(
      struct _ark_web_handler_t *self);

  void(ARK_WEB_CALLBACK *on_over_scroll_fling_velocity)(
      struct _ark_web_handler_t *self, float x_velocity, float y_velocity,
      bool is_fling);

  void(ARK_WEB_CALLBACK *on_navigation_entry_committed)(
      struct _ark_web_handler_t *self,
      ark_web_load_committed_details_t *details);

  void(ARK_WEB_CALLBACK *on_native_embed_lifecycle_change)(
      struct _ark_web_handler_t *self,
      ark_web_native_embed_data_info_t *data_info);

  void(ARK_WEB_CALLBACK *on_native_embed_gesture_event)(
      struct _ark_web_handler_t *self,
      ark_web_native_embed_touch_event_t *touch_event);

  void(ARK_WEB_CALLBACK *on_safe_browsing_check_result)(
      struct _ark_web_handler_t *self, int threat_type);

  void(ARK_WEB_CALLBACK *on_intelligent_tracking_prevention_result)(
      struct _ark_web_handler_t *self, const ArkWebString *website_host,
      const ArkWebString *tracker_host);

  void(ARK_WEB_CALLBACK *on_full_screen_enter_with_video_size)(
      struct _ark_web_handler_t *self,
      ark_web_full_screen_exit_handler_t *handler, int video_natural_width,
      int video_natural_height);

  bool(ARK_WEB_CALLBACK *on_handle_override_url_loading)(
      struct _ark_web_handler_t *self, ark_web_url_resource_request_t *request);
} ark_web_handler_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_HANDLER_CAPI_H_
