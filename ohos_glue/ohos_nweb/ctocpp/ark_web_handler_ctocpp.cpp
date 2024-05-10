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

#include "ohos_nweb/ctocpp/ark_web_handler_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_nweb/cpptoc/ark_web_access_request_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_app_link_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_console_log_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_context_menu_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_context_menu_params_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_controller_handler_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_cursor_info_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_data_resubmission_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_date_time_chooser_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_date_time_chooser_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_drag_data_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_file_selector_params_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_first_meaningful_paint_details_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_full_screen_exit_handler_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_geo_location_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_image_options_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_js_all_ssl_error_result_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_js_dialog_result_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_js_http_auth_result_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_js_ssl_error_result_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_js_ssl_select_cert_result_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_key_event_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_largest_contentful_paint_details_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_load_committed_details_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_native_embed_data_info_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_native_embed_touch_event_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_nweb_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_quick_menu_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_quick_menu_params_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_screen_capture_access_request_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_select_popup_menu_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_select_popup_menu_param_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_string_vector_value_callback_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_touch_handle_hot_zone_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_touch_handle_state_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_url_resource_error_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_url_resource_request_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_url_resource_response_cpptoc.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::SetNWeb(ArkWebRefPtr<ArkWebNWeb> nweb) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_nweb, );

  // Execute
  _struct->set_nweb(_struct, ArkWebNWebCppToC::Invert(nweb));
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnFocus() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_focus, false);

  // Execute
  return _struct->on_focus(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnMessage(const ArkWebString &param) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_message, );

  // Execute
  _struct->on_message(_struct, &param);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnResource(const ArkWebString &url) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_resource, );

  // Execute
  _struct->on_resource(_struct, &url);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnPageIcon(const void *data, size_t width,
                                     size_t height, int color_type,
                                     int alpha_type) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_page_icon, );

  // Execute
  _struct->on_page_icon(_struct, data, width, height, color_type, alpha_type);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnPageTitle(const ArkWebString &title) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_page_title, );

  // Execute
  _struct->on_page_title(_struct, &title);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnProxyDied() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_proxy_died, );

  // Execute
  _struct->on_proxy_died(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnHttpError(
    ArkWebRefPtr<ArkWebUrlResourceRequest> request,
    ArkWebRefPtr<ArkWebUrlResourceResponse> error_response) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_http_error, );

  // Execute
  _struct->on_http_error(
      _struct, ArkWebUrlResourceRequestCppToC::Invert(request),
      ArkWebUrlResourceResponseCppToC::Invert(error_response));
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnConsoleLog(ArkWebRefPtr<ArkWebConsoleLog> message) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_console_log, false);

  // Execute
  return _struct->on_console_log(_struct,
                                 ArkWebConsoleLogCppToC::Invert(message));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnRouterPush(const ArkWebString &param) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_router_push, );

  // Execute
  _struct->on_router_push(_struct, &param);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnPageLoadEnd(int http_status_code,
                                        const ArkWebString &url) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_page_load_end, );

  // Execute
  _struct->on_page_load_end(_struct, http_status_code, &url);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnPageLoadBegin(const ArkWebString &url) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_page_load_begin, );

  // Execute
  _struct->on_page_load_begin(_struct, &url);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnPageLoadError(int error_code,
                                          const ArkWebString &description,
                                          const ArkWebString &failing_url) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_page_load_error, );

  // Execute
  _struct->on_page_load_error(_struct, error_code, &description, &failing_url);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnDesktopIconUrl(const ArkWebString &icon_url,
                                           bool precomposed) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_desktop_icon_url, );

  // Execute
  _struct->on_desktop_icon_url(_struct, &icon_url, precomposed);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnLoadingProgress(int new_progress) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_loading_progress, );

  // Execute
  _struct->on_loading_progress(_struct, new_progress);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnGeolocationShow(
    const ArkWebString &origin,
    ArkWebRefPtr<ArkWebGeoLocationCallback> callback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_geolocation_show, );

  // Execute
  _struct->on_geolocation_show(
      _struct, &origin, ArkWebGeoLocationCallbackCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnGeolocationHide() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_geolocation_hide, );

  // Execute
  _struct->on_geolocation_hide(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnFileSelectorShow(
    ArkWebRefPtr<ArkWebStringVectorValueCallback> callback,
    ArkWebRefPtr<ArkWebFileSelectorParams> params) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_file_selector_show, false);

  // Execute
  return _struct->on_file_selector_show(
      _struct, ArkWebStringVectorValueCallbackCppToC::Invert(callback),
      ArkWebFileSelectorParamsCppToC::Invert(params));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnResourceLoadError(
    ArkWebRefPtr<ArkWebUrlResourceRequest> request,
    ArkWebRefPtr<ArkWebUrlResourceError> error) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_resource_load_error, );

  // Execute
  _struct->on_resource_load_error(
      _struct, ArkWebUrlResourceRequestCppToC::Invert(request),
      ArkWebUrlResourceErrorCppToC::Invert(error));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnPermissionRequest(
    ArkWebRefPtr<ArkWebAccessRequest> request) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_permission_request, );

  // Execute
  _struct->on_permission_request(_struct,
                                 ArkWebAccessRequestCppToC::Invert(request));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnQuickMenuDismissed() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_quick_menu_dismissed, );

  // Execute
  _struct->on_quick_menu_dismissed(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnContextMenuDismissed() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_context_menu_dismissed, );

  // Execute
  _struct->on_context_menu_dismissed(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnTouchSelectionChanged(
    ArkWebRefPtr<ArkWebTouchHandleState> insert_handle,
    ArkWebRefPtr<ArkWebTouchHandleState> start_selection_handle,
    ArkWebRefPtr<ArkWebTouchHandleState> end_selection_handle) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_touch_selection_changed, );

  // Execute
  _struct->on_touch_selection_changed(
      _struct, ArkWebTouchHandleStateCppToC::Invert(insert_handle),
      ArkWebTouchHandleStateCppToC::Invert(start_selection_handle),
      ArkWebTouchHandleStateCppToC::Invert(end_selection_handle));
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnHandleInterceptRequest(
    ArkWebRefPtr<ArkWebUrlResourceRequest> request,
    ArkWebRefPtr<ArkWebUrlResourceResponse> response) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_handle_intercept_request, false);

  // Execute
  return _struct->on_handle_intercept_request(
      _struct, ArkWebUrlResourceRequestCppToC::Invert(request),
      ArkWebUrlResourceResponseCppToC::Invert(response));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnRefreshAccessedHistory(const ArkWebString &url,
                                                   bool is_reload) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_refresh_accessed_history, );

  // Execute
  _struct->on_refresh_accessed_history(_struct, &url, is_reload);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnPermissionRequestCanceled(
    ArkWebRefPtr<ArkWebAccessRequest> request) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_permission_request_canceled, );

  // Execute
  _struct->on_permission_request_canceled(
      _struct, ArkWebAccessRequestCppToC::Invert(request));
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnHandleInterceptUrlLoading(
    ArkWebRefPtr<ArkWebUrlResourceRequest> request) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_handle_intercept_url_loading,
                                   false);

  // Execute
  return _struct->on_handle_intercept_url_loading(
      _struct, ArkWebUrlResourceRequestCppToC::Invert(request));
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::RunQuickMenu(
    ArkWebRefPtr<ArkWebQuickMenuParams> params,
    ArkWebRefPtr<ArkWebQuickMenuCallback> callback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, run_quick_menu, false);

  // Execute
  return _struct->run_quick_menu(
      _struct, ArkWebQuickMenuParamsCppToC::Invert(params),
      ArkWebQuickMenuCallbackCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::RunContextMenu(
    ArkWebRefPtr<ArkWebContextMenuParams> params,
    ArkWebRefPtr<ArkWebContextMenuCallback> callback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, run_context_menu, false);

  // Execute
  return _struct->run_context_menu(
      _struct, ArkWebContextMenuParamsCppToC::Invert(params),
      ArkWebContextMenuCallbackCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::UpdateDragCursor(unsigned char op) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, update_drag_cursor, );

  // Execute
  _struct->update_drag_cursor(_struct, op);
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::FilterScrollEvent(const float x, const float y,
                                            const float x_velocity,
                                            const float y_velocity) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, filter_scroll_event, false);

  // Execute
  return _struct->filter_scroll_event(_struct, x, y, x_velocity, y_velocity);
}

ARK_WEB_NO_SANITIZE
ArkWebStringVector ArkWebHandlerCToCpp::VisitedUrlHistory() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_string_vector_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, visited_url_history,
                                   ark_web_string_vector_default);

  // Execute
  return _struct->visited_url_history(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnWindowNewByJS(
    const ArkWebString &target_url, bool is_alert, bool is_user_trigger,
    ArkWebRefPtr<ArkWebControllerHandler> handler) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_window_new_by_js, );

  // Execute
  _struct->on_window_new_by_js(_struct, &target_url, is_alert, is_user_trigger,
                               ArkWebControllerHandlerCppToC::Invert(handler));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnWindowExitByJS() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_window_exit_by_js, );

  // Execute
  _struct->on_window_exit_by_js(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnAlertDialogByJS(
    const ArkWebString &url, const ArkWebString &message,
    ArkWebRefPtr<ArkWebJsDialogResult> result) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_alert_dialog_by_js, false);

  // Execute
  return _struct->on_alert_dialog_by_js(
      _struct, &url, &message, ArkWebJsDialogResultCppToC::Invert(result));
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnBeforeUnloadByJS(
    const ArkWebString &url, const ArkWebString &message,
    ArkWebRefPtr<ArkWebJsDialogResult> result) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_before_unload_by_js, false);

  // Execute
  return _struct->on_before_unload_by_js(
      _struct, &url, &message, ArkWebJsDialogResultCppToC::Invert(result));
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnPromptDialogByJS(
    const ArkWebString &url, const ArkWebString &message,
    const ArkWebString &default_value,
    ArkWebRefPtr<ArkWebJsDialogResult> result) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_prompt_dialog_by_js, false);

  // Execute
  return _struct->on_prompt_dialog_by_js(
      _struct, &url, &message, &default_value,
      ArkWebJsDialogResultCppToC::Invert(result));
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnConfirmDialogByJS(
    const ArkWebString &url, const ArkWebString &message,
    ArkWebRefPtr<ArkWebJsDialogResult> result) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_confirm_dialog_by_js, false);

  // Execute
  return _struct->on_confirm_dialog_by_js(
      _struct, &url, &message, ArkWebJsDialogResultCppToC::Invert(result));
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnHttpAuthRequestByJS(
    ArkWebRefPtr<ArkWebJsHttpAuthResult> result, const ArkWebString &host,
    const ArkWebString &realm) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_http_auth_request_by_js, false);

  // Execute
  return _struct->on_http_auth_request_by_js(
      _struct, ArkWebJsHttpAuthResultCppToC::Invert(result), &host, &realm);
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnSslErrorRequestByJS(
    ArkWebRefPtr<ArkWebJsSslErrorResult> result, int error) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_ssl_error_request_by_js, false);

  // Execute
  return _struct->on_ssl_error_request_by_js(
      _struct, ArkWebJsSslErrorResultCppToC::Invert(result), error);
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnSslSelectCertRequestByJS(
    ArkWebRefPtr<ArkWebJsSslSelectCertResult> result, const ArkWebString &host,
    int port, const ArkWebStringVector &key_types,
    const ArkWebStringVector &issuers) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_ssl_select_cert_request_by_js,
                                   false);

  // Execute
  return _struct->on_ssl_select_cert_request_by_js(
      _struct, ArkWebJsSslSelectCertResultCppToC::Invert(result), &host, port,
      &key_types, &issuers);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnScroll(double x_offset, double y_offset) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_scroll, );

  // Execute
  _struct->on_scroll(_struct, x_offset, y_offset);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnOverScroll(float x_offset, float y_offset) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_over_scroll, );

  // Execute
  _struct->on_over_scroll(_struct, x_offset, y_offset);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnScrollState(bool scroll_state) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_scroll_state, );

  // Execute
  _struct->on_scroll_state(_struct, scroll_state);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnPageVisible(const ArkWebString &url) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_page_visible, );

  // Execute
  _struct->on_page_visible(_struct, &url);
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnPreKeyEvent(ArkWebRefPtr<ArkWebKeyEvent> event) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_pre_key_event, false);

  // Execute
  return _struct->on_pre_key_event(_struct,
                                   ArkWebKeyEventCppToC::Invert(event));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnScaleChanged(float old_scale_factor,
                                         float new_scale_factor) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_scale_changed, );

  // Execute
  _struct->on_scale_changed(_struct, old_scale_factor, new_scale_factor);
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnCursorChange(const int32_t &type,
                                         ArkWebRefPtr<ArkWebCursorInfo> info) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_cursor_change, false);

  // Execute
  return _struct->on_cursor_change(_struct, &type,
                                   ArkWebCursorInfoCppToC::Invert(info));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnRenderExited(int reason) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_render_exited, );

  // Execute
  _struct->on_render_exited(_struct, reason);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnResizeNotWork() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_resize_not_work, );

  // Execute
  _struct->on_resize_not_work(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnFullScreenExit() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_full_screen_exit, );

  // Execute
  _struct->on_full_screen_exit(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnFullScreenEnter(
    ArkWebRefPtr<ArkWebFullScreenExitHandler> handler) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_full_screen_enter, );

  // Execute
  _struct->on_full_screen_enter(
      _struct, ArkWebFullScreenExitHandlerCppToC::Invert(handler));
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnDragAndDropData(
    const void *data, size_t len, ArkWebRefPtr<ArkWebImageOptions> opt) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_drag_and_drop_data, false);

  // Execute
  return _struct->on_drag_and_drop_data(_struct, data, len,
                                        ArkWebImageOptionsCppToC::Invert(opt));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnSelectPopupMenu(
    ArkWebRefPtr<ArkWebSelectPopupMenuParam> params,
    ArkWebRefPtr<ArkWebSelectPopupMenuCallback> callback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_select_popup_menu, );

  // Execute
  _struct->on_select_popup_menu(
      _struct, ArkWebSelectPopupMenuParamCppToC::Invert(params),
      ArkWebSelectPopupMenuCallbackCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnDataResubmission(
    ArkWebRefPtr<ArkWebDataResubmissionCallback> handler) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_data_resubmission, );

  // Execute
  _struct->on_data_resubmission(
      _struct, ArkWebDataResubmissionCallbackCppToC::Invert(handler));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnRootLayerChanged(int width, int height) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_root_layer_changed, );

  // Execute
  _struct->on_root_layer_changed(_struct, width, height);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnAudioStateChanged(bool playing) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_audio_state_changed, );

  // Execute
  _struct->on_audio_state_changed(_struct, playing);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnOverScrollFlingEnd() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_over_scroll_fling_end, );

  // Execute
  _struct->on_over_scroll_fling_end(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnUnProcessedKeyEvent(
    ArkWebRefPtr<ArkWebKeyEvent> event) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_un_processed_key_event, false);

  // Execute
  return _struct->on_un_processed_key_event(
      _struct, ArkWebKeyEventCppToC::Invert(event));
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnDragAndDropDataUdmf(
    ArkWebRefPtr<ArkWebDragData> drag_data) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_drag_and_drop_data_udmf, false);

  // Execute
  return _struct->on_drag_and_drop_data_udmf(
      _struct, ArkWebDragDataCppToC::Invert(drag_data));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnFirstContentfulPaint(
    int64_t navigation_start_tick, int64_t first_contentful_paint_ms) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_first_contentful_paint, );

  // Execute
  _struct->on_first_contentful_paint(_struct, navigation_start_tick,
                                     first_contentful_paint_ms);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnDateTimeChooserPopup(
    ArkWebRefPtr<ArkWebDateTimeChooser> chooser,
    const ArkWebDateTimeSuggestionVector &suggestions,
    ArkWebRefPtr<ArkWebDateTimeChooserCallback> callback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_date_time_chooser_popup, );

  // Execute
  _struct->on_date_time_chooser_popup(
      _struct, ArkWebDateTimeChooserCppToC::Invert(chooser), &suggestions,
      ArkWebDateTimeChooserCallbackCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnDateTimeChooserClose() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_date_time_chooser_close, );

  // Execute
  _struct->on_date_time_chooser_close(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnScreenCaptureRequest(
    ArkWebRefPtr<ArkWebScreenCaptureAccessRequest> request) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_screen_capture_request, );

  // Execute
  _struct->on_screen_capture_request(
      _struct, ArkWebScreenCaptureAccessRequestCppToC::Invert(request));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnActivityStateChanged(int state, int type) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_activity_state_changed, );

  // Execute
  _struct->on_activity_state_changed(_struct, state, type);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnGetTouchHandleHotZone(
    ArkWebRefPtr<ArkWebTouchHandleHotZone> hot_zone) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_get_touch_handle_hot_zone, );

  // Execute
  _struct->on_get_touch_handle_hot_zone(
      _struct, ArkWebTouchHandleHotZoneCppToC::Invert(hot_zone));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnCompleteSwapWithNewSize() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_complete_swap_with_new_size, );

  // Execute
  _struct->on_complete_swap_with_new_size(_struct);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnOverScrollFlingVelocity(float x_velocity,
                                                    float y_velocity,
                                                    bool is_fling) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_over_scroll_fling_velocity, );

  // Execute
  _struct->on_over_scroll_fling_velocity(_struct, x_velocity, y_velocity,
                                         is_fling);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnNavigationEntryCommitted(
    ArkWebRefPtr<ArkWebLoadCommittedDetails> details) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_navigation_entry_committed, );

  // Execute
  _struct->on_navigation_entry_committed(
      _struct, ArkWebLoadCommittedDetailsCppToC::Invert(details));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnNativeEmbedLifecycleChange(
    ArkWebRefPtr<ArkWebNativeEmbedDataInfo> data_info) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_native_embed_lifecycle_change, );

  // Execute
  _struct->on_native_embed_lifecycle_change(
      _struct, ArkWebNativeEmbedDataInfoCppToC::Invert(data_info));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnNativeEmbedGestureEvent(
    ArkWebRefPtr<ArkWebNativeEmbedTouchEvent> touch_event) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_native_embed_gesture_event, );

  // Execute
  _struct->on_native_embed_gesture_event(
      _struct, ArkWebNativeEmbedTouchEventCppToC::Invert(touch_event));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnSafeBrowsingCheckResult(int threat_type) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_safe_browsing_check_result, );

  // Execute
  _struct->on_safe_browsing_check_result(_struct, threat_type);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnIntelligentTrackingPreventionResult(
    const ArkWebString &website_host, const ArkWebString &tracker_host) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct,
                                   on_intelligent_tracking_prevention_result, );

  // Execute
  _struct->on_intelligent_tracking_prevention_result(_struct, &website_host,
                                                     &tracker_host);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnFullScreenEnterWithVideoSize(
    ArkWebRefPtr<ArkWebFullScreenExitHandler> handler, int video_natural_width,
    int video_natural_height) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct,
                                   on_full_screen_enter_with_video_size, );

  // Execute
  _struct->on_full_screen_enter_with_video_size(
      _struct, ArkWebFullScreenExitHandlerCppToC::Invert(handler),
      video_natural_width, video_natural_height);
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnHandleOverrideUrlLoading(
    ArkWebRefPtr<ArkWebUrlResourceRequest> request) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_handle_override_url_loading,
                                   false);

  // Execute
  return _struct->on_handle_override_url_loading(
      _struct, ArkWebUrlResourceRequestCppToC::Invert(request));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnFirstMeaningfulPaint(
    ArkWebRefPtr<ArkWebFirstMeaningfulPaintDetails> details) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_first_meaningful_paint, );

  // Execute
  _struct->on_first_meaningful_paint(
      _struct, ArkWebFirstMeaningfulPaintDetailsCppToC::Invert(details));
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnLargestContentfulPaint(
    ArkWebRefPtr<ArkWebLargestContentfulPaintDetails> details) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_largest_contentful_paint, );

  // Execute
  _struct->on_largest_contentful_paint(
      _struct, ArkWebLargestContentfulPaintDetailsCppToC::Invert(details));
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnAllSslErrorRequestByJS(
    ArkWebRefPtr<ArkWebJsAllSslErrorResult> result, int error,
    const ArkWebString &url, const ArkWebString &originalUrl,
    const ArkWebString &referrer, bool isFatalError, bool isMainFrame) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_all_ssl_error_request_by_js,
                                   false);

  // Execute
  return _struct->on_all_ssl_error_request_by_js(
      _struct, ArkWebJsAllSslErrorResultCppToC::Invert(result), error, &url,
      &originalUrl, &referrer, isFatalError, isMainFrame);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnTooltip(const ArkWebString &tooltip) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_tooltip, );

  // Execute
  _struct->on_tooltip(_struct, &tooltip);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::ReleaseResizeHold() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, release_resize_hold, );

  // Execute
  _struct->release_resize_hold(_struct);
}

void ArkWebHandlerCToCpp::OnShowAutofillPopup(
    const float offsetX, const float offsetY,
    const ArkWebStringVector &menu_items) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_show_autofill_popup, );

  // Execute
  _struct->on_show_autofill_popup(_struct, offsetX, offsetY, &menu_items);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnHideAutofillPopup() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_hide_autofill_popup, );

  // Execute
  _struct->on_hide_autofill_popup(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebCharVector ArkWebHandlerCToCpp::GetWordSelection(const ArkWebString &text,
                                                       int8_t offset) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ark_web_char_vector_default);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_word_selection,
                                   ark_web_char_vector_default);

  // Execute
  return _struct->get_word_selection(_struct, &text, offset);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::UpdateClippedSelectionBounds(int x, int y, int w,
                                                       int h) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, update_clipped_selection_bounds, );

  // Execute
  _struct->update_clipped_selection_bounds(_struct, x, y, w, h);
}

void ArkWebHandlerCToCpp::OnViewportFitChange(int viewportFit) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t* _struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_viewport_fit_change, );

  // Execute
  _struct->on_viewport_fit_change(_struct, viewportFit);
}

ARK_WEB_NO_SANITIZE
bool ArkWebHandlerCToCpp::OnOpenAppLink(
    const ArkWebString &url, ArkWebRefPtr<ArkWebAppLinkCallback> callback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_open_app_link, false);

  // Execute
  return _struct->on_open_app_link(
      _struct, &url, ArkWebAppLinkCallbackCppToC::Invert(callback));
}


ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnRenderProcessNotResponding(const ArkWebString& js_stack, int pid, int reason) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t* _struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_render_process_not_responding, );

  // Execute
  _struct->on_render_process_not_responding(_struct,
      &js_stack,
      pid,
      reason);
}

ARK_WEB_NO_SANITIZE
void ArkWebHandlerCToCpp::OnRenderProcessResponding() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_web_handler_t* _struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_render_process_responding, );

  // Execute
  _struct->on_render_process_responding(_struct);
}

ArkWebHandlerCToCpp::ArkWebHandlerCToCpp() {
}

ArkWebHandlerCToCpp::~ArkWebHandlerCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<ArkWebHandlerCToCpp, ArkWebHandler,
                                        ark_web_handler_t>::kBridgeType =
    ARK_WEB_HANDLER;

} // namespace OHOS::ArkWeb
