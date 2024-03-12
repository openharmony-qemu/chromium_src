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

#include "ohos_nweb/cpptoc/ark_web_handler_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_nweb/ctocpp/ark_web_access_request_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_console_log_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_context_menu_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_context_menu_params_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_controller_handler_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_data_resubmission_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_date_time_chooser_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_drag_data_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_file_selector_params_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_first_meaningful_paint_details_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_full_screen_exit_handler_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_geo_location_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_js_dialog_result_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_js_http_auth_result_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_js_ssl_error_result_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_js_ssl_select_cert_result_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_key_event_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_largest_contentful_paint_details_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_load_committed_details_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_native_embed_data_info_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_native_embed_touch_event_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_nweb_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_quick_menu_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_quick_menu_params_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_screen_capture_access_request_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_select_popup_menu_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_select_popup_menu_param_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_string_vector_value_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_touch_handle_state_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_url_resource_error_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_url_resource_request_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_url_resource_response_ctocpp.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_web_handler_set_nweb(struct _ark_web_handler_t *self,
                                               ark_web_nweb_t *nweb) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->SetNWeb(ArkWebNWebCToCpp::Invert(nweb));
}

bool ARK_WEB_CALLBACK
ark_web_handler_on_focus(struct _ark_web_handler_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnFocus();
}

void ARK_WEB_CALLBACK ark_web_handler_on_message(
    struct _ark_web_handler_t *self, const ArkWebString *param) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(param, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnMessage(*param);
}

void ARK_WEB_CALLBACK ark_web_handler_on_resource(
    struct _ark_web_handler_t *self, const ArkWebString *url) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(url, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnResource(*url);
}

void ARK_WEB_CALLBACK ark_web_handler_on_page_icon(
    struct _ark_web_handler_t *self, const void *data, size_t width,
    size_t height, int color_type, int alpha_type) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(data, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnPageIcon(data, width, height, color_type,
                                             alpha_type);
}

void ARK_WEB_CALLBACK ark_web_handler_on_page_title(
    struct _ark_web_handler_t *self, const ArkWebString *title) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(title, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnPageTitle(*title);
}

void ARK_WEB_CALLBACK
ark_web_handler_on_proxy_died(struct _ark_web_handler_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnProxyDied();
}

void ARK_WEB_CALLBACK ark_web_handler_on_http_error(
    struct _ark_web_handler_t *self, ark_web_url_resource_request_t *request,
    ark_web_url_resource_response_t *error_response) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnHttpError(
      ArkWebUrlResourceRequestCToCpp::Invert(request),
      ArkWebUrlResourceResponseCToCpp::Invert(error_response));
}

bool ARK_WEB_CALLBACK ark_web_handler_on_console_log(
    struct _ark_web_handler_t *self, ark_web_console_log_t *message) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnConsoleLog(
      ArkWebConsoleLogCToCpp::Invert(message));
}

void ARK_WEB_CALLBACK ark_web_handler_on_router_push(
    struct _ark_web_handler_t *self, const ArkWebString *param) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(param, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnRouterPush(*param);
}

void ARK_WEB_CALLBACK ark_web_handler_on_page_load_end(
    struct _ark_web_handler_t *self, int http_status_code,
    const ArkWebString *url) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(url, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnPageLoadEnd(http_status_code, *url);
}

void ARK_WEB_CALLBACK ark_web_handler_on_page_load_begin(
    struct _ark_web_handler_t *self, const ArkWebString *url) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(url, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnPageLoadBegin(*url);
}

void ARK_WEB_CALLBACK ark_web_handler_on_page_load_error(
    struct _ark_web_handler_t *self, int error_code,
    const ArkWebString *description, const ArkWebString *failing_url) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(description, );

  ARK_WEB_CPPTOC_CHECK_PARAM(failing_url, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnPageLoadError(error_code, *description,
                                                  *failing_url);
}

void ARK_WEB_CALLBACK ark_web_handler_on_desktop_icon_url(
    struct _ark_web_handler_t *self, const ArkWebString *icon_url,
    bool precomposed) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(icon_url, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnDesktopIconUrl(*icon_url, precomposed);
}

void ARK_WEB_CALLBACK ark_web_handler_on_loading_progress(
    struct _ark_web_handler_t *self, int new_progress) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnLoadingProgress(new_progress);
}

void ARK_WEB_CALLBACK ark_web_handler_on_geolocation_show(
    struct _ark_web_handler_t *self, const ArkWebString *origin,
    ark_web_geo_location_callback_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(origin, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnGeolocationShow(
      *origin, ArkWebGeoLocationCallbackCToCpp::Invert(callback));
}

void ARK_WEB_CALLBACK
ark_web_handler_on_geolocation_hide(struct _ark_web_handler_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnGeolocationHide();
}

bool ARK_WEB_CALLBACK ark_web_handler_on_file_selector_show(
    struct _ark_web_handler_t *self,
    ark_web_string_vector_value_callback_t *callback,
    ark_web_file_selector_params_t *params) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnFileSelectorShow(
      ArkWebStringVectorValueCallbackCToCpp::Invert(callback),
      ArkWebFileSelectorParamsCToCpp::Invert(params));
}

void ARK_WEB_CALLBACK ark_web_handler_on_resource_load_error(
    struct _ark_web_handler_t *self, ark_web_url_resource_request_t *request,
    ark_web_url_resource_error_t *error) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnResourceLoadError(
      ArkWebUrlResourceRequestCToCpp::Invert(request),
      ArkWebUrlResourceErrorCToCpp::Invert(error));
}

void ARK_WEB_CALLBACK ark_web_handler_on_permission_request(
    struct _ark_web_handler_t *self, ark_web_access_request_t *request) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnPermissionRequest(
      ArkWebAccessRequestCToCpp::Invert(request));
}

void ARK_WEB_CALLBACK
ark_web_handler_on_quick_menu_dismissed(struct _ark_web_handler_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnQuickMenuDismissed();
}

void ARK_WEB_CALLBACK
ark_web_handler_on_context_menu_dismissed(struct _ark_web_handler_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnContextMenuDismissed();
}

void ARK_WEB_CALLBACK ark_web_handler_on_touch_selection_changed(
    struct _ark_web_handler_t *self,
    ark_web_touch_handle_state_t *insert_handle,
    ark_web_touch_handle_state_t *start_selection_handle,
    ark_web_touch_handle_state_t *end_selection_handle) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnTouchSelectionChanged(
      ArkWebTouchHandleStateCToCpp::Invert(insert_handle),
      ArkWebTouchHandleStateCToCpp::Invert(start_selection_handle),
      ArkWebTouchHandleStateCToCpp::Invert(end_selection_handle));
}

bool ARK_WEB_CALLBACK ark_web_handler_on_handle_intercept_request(
    struct _ark_web_handler_t *self, ark_web_url_resource_request_t *request,
    ark_web_url_resource_response_t *response) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnHandleInterceptRequest(
      ArkWebUrlResourceRequestCToCpp::Invert(request),
      ArkWebUrlResourceResponseCToCpp::Invert(response));
}

void ARK_WEB_CALLBACK ark_web_handler_on_refresh_accessed_history(
    struct _ark_web_handler_t *self, const ArkWebString *url, bool is_reload) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(url, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnRefreshAccessedHistory(*url, is_reload);
}

void ARK_WEB_CALLBACK ark_web_handler_on_permission_request_canceled(
    struct _ark_web_handler_t *self, ark_web_access_request_t *request) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnPermissionRequestCanceled(
      ArkWebAccessRequestCToCpp::Invert(request));
}

bool ARK_WEB_CALLBACK ark_web_handler_on_handle_intercept_url_loading(
    struct _ark_web_handler_t *self, ark_web_url_resource_request_t *request) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnHandleInterceptUrlLoading(
      ArkWebUrlResourceRequestCToCpp::Invert(request));
}

bool ARK_WEB_CALLBACK ark_web_handler_run_quick_menu(
    struct _ark_web_handler_t *self, ark_web_quick_menu_params_t *params,
    ark_web_quick_menu_callback_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->RunQuickMenu(
      ArkWebQuickMenuParamsCToCpp::Invert(params),
      ArkWebQuickMenuCallbackCToCpp::Invert(callback));
}

bool ARK_WEB_CALLBACK ark_web_handler_run_context_menu(
    struct _ark_web_handler_t *self, ark_web_context_menu_params_t *params,
    ark_web_context_menu_callback_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->RunContextMenu(
      ArkWebContextMenuParamsCToCpp::Invert(params),
      ArkWebContextMenuCallbackCToCpp::Invert(callback));
}

void ARK_WEB_CALLBACK ark_web_handler_update_drag_cursor(
    struct _ark_web_handler_t *self, unsigned char op) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->UpdateDragCursor(op);
}

bool ARK_WEB_CALLBACK ark_web_handler_filter_scroll_event(
    struct _ark_web_handler_t *self, const float x, const float y,
    const float x_velocity, const float y_velocity) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->FilterScrollEvent(x, y, x_velocity,
                                                           y_velocity);
}

ArkWebStringVector ARK_WEB_CALLBACK
ark_web_handler_visited_url_history(struct _ark_web_handler_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_vector_default);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->VisitedUrlHistory();
}

void ARK_WEB_CALLBACK ark_web_handler_on_window_new_by_js(
    struct _ark_web_handler_t *self, const ArkWebString *target_url,
    bool is_alert, bool is_user_trigger,
    ark_web_controller_handler_t *handler) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(target_url, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnWindowNewByJS(
      *target_url, is_alert, is_user_trigger,
      ArkWebControllerHandlerCToCpp::Invert(handler));
}

void ARK_WEB_CALLBACK
ark_web_handler_on_window_exit_by_js(struct _ark_web_handler_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnWindowExitByJS();
}

bool ARK_WEB_CALLBACK ark_web_handler_on_alert_dialog_by_js(
    struct _ark_web_handler_t *self, const ArkWebString *url,
    const ArkWebString *message, ark_web_js_dialog_result_t *result) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(url, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(message, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnAlertDialogByJS(
      *url, *message, ArkWebJsDialogResultCToCpp::Invert(result));
}

bool ARK_WEB_CALLBACK ark_web_handler_on_before_unload_by_js(
    struct _ark_web_handler_t *self, const ArkWebString *url,
    const ArkWebString *message, ark_web_js_dialog_result_t *result) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(url, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(message, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnBeforeUnloadByJS(
      *url, *message, ArkWebJsDialogResultCToCpp::Invert(result));
}

bool ARK_WEB_CALLBACK ark_web_handler_on_prompt_dialog_by_js(
    struct _ark_web_handler_t *self, const ArkWebString *url,
    const ArkWebString *message, const ArkWebString *default_value,
    ark_web_js_dialog_result_t *result) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(url, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(message, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(default_value, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnPromptDialogByJS(
      *url, *message, *default_value,
      ArkWebJsDialogResultCToCpp::Invert(result));
}

bool ARK_WEB_CALLBACK ark_web_handler_on_confirm_dialog_by_js(
    struct _ark_web_handler_t *self, const ArkWebString *url,
    const ArkWebString *message, ark_web_js_dialog_result_t *result) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(url, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(message, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnConfirmDialogByJS(
      *url, *message, ArkWebJsDialogResultCToCpp::Invert(result));
}

bool ARK_WEB_CALLBACK ark_web_handler_on_http_auth_request_by_js(
    struct _ark_web_handler_t *self, ark_web_js_http_auth_result_t *result,
    const ArkWebString *host, const ArkWebString *realm) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(host, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(realm, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnHttpAuthRequestByJS(
      ArkWebJsHttpAuthResultCToCpp::Invert(result), *host, *realm);
}

bool ARK_WEB_CALLBACK ark_web_handler_on_ssl_error_request_by_js(
    struct _ark_web_handler_t *self, ark_web_js_ssl_error_result_t *result,
    int error) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnSslErrorRequestByJS(
      ArkWebJsSslErrorResultCToCpp::Invert(result), error);
}

bool ARK_WEB_CALLBACK ark_web_handler_on_ssl_select_cert_request_by_js(
    struct _ark_web_handler_t *self,
    ark_web_js_ssl_select_cert_result_t *result, const ArkWebString *host,
    int port, const ArkWebStringVector *key_types,
    const ArkWebStringVector *issuers) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(host, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(key_types, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(issuers, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnSslSelectCertRequestByJS(
      ArkWebJsSslSelectCertResultCToCpp::Invert(result), *host, port,
      *key_types, *issuers);
}

void ARK_WEB_CALLBACK ark_web_handler_on_scroll(struct _ark_web_handler_t *self,
                                                double x_offset,
                                                double y_offset) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnScroll(x_offset, y_offset);
}

void ARK_WEB_CALLBACK ark_web_handler_on_over_scroll(
    struct _ark_web_handler_t *self, float x_offset, float y_offset) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnOverScroll(x_offset, y_offset);
}

void ARK_WEB_CALLBACK ark_web_handler_on_scroll_state(
    struct _ark_web_handler_t *self, bool scroll_state) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnScrollState(scroll_state);
}

void ARK_WEB_CALLBACK ark_web_handler_on_page_visible(
    struct _ark_web_handler_t *self, const ArkWebString *url) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(url, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnPageVisible(*url);
}

bool ARK_WEB_CALLBACK ark_web_handler_on_pre_key_event(
    struct _ark_web_handler_t *self, ark_web_key_event_t *event) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnPreKeyEvent(
      ArkWebKeyEventCToCpp::Invert(event));
}

void ARK_WEB_CALLBACK ark_web_handler_on_scale_changed(
    struct _ark_web_handler_t *self, float old_scale_factor,
    float new_scale_factor) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnScaleChanged(old_scale_factor,
                                                 new_scale_factor);
}

bool ARK_WEB_CALLBACK ark_web_handler_on_cursor_change(
    struct _ark_web_handler_t *self, const int32_t *type,
    const ArkWebCursorInfo *info) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(type, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(info, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnCursorChange(*type, *info);
}

void ARK_WEB_CALLBACK
ark_web_handler_on_render_exited(struct _ark_web_handler_t *self, int reason) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnRenderExited(reason);
}

void ARK_WEB_CALLBACK
ark_web_handler_on_resize_not_work(struct _ark_web_handler_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnResizeNotWork();
}

void ARK_WEB_CALLBACK
ark_web_handler_on_full_screen_exit(struct _ark_web_handler_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnFullScreenExit();
}

void ARK_WEB_CALLBACK ark_web_handler_on_full_screen_enter(
    struct _ark_web_handler_t *self,
    ark_web_full_screen_exit_handler_t *handler) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnFullScreenEnter(
      ArkWebFullScreenExitHandlerCToCpp::Invert(handler));
}

bool ARK_WEB_CALLBACK ark_web_handler_on_drag_and_drop_data(
    struct _ark_web_handler_t *self, const void *data, size_t len,
    const ArkWebImageOptions *opt) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(data, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(opt, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnDragAndDropData(data, len, *opt);
}

void ARK_WEB_CALLBACK ark_web_handler_on_select_popup_menu(
    struct _ark_web_handler_t *self, ark_web_select_popup_menu_param_t *params,
    ark_web_select_popup_menu_callback_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnSelectPopupMenu(
      ArkWebSelectPopupMenuParamCToCpp::Invert(params),
      ArkWebSelectPopupMenuCallbackCToCpp::Invert(callback));
}

void ARK_WEB_CALLBACK ark_web_handler_on_data_resubmission(
    struct _ark_web_handler_t *self,
    ark_web_data_resubmission_callback_t *handler) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnDataResubmission(
      ArkWebDataResubmissionCallbackCToCpp::Invert(handler));
}

void ARK_WEB_CALLBACK ark_web_handler_on_root_layer_changed(
    struct _ark_web_handler_t *self, int width, int height) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnRootLayerChanged(width, height);
}

void ARK_WEB_CALLBACK ark_web_handler_on_audio_state_changed(
    struct _ark_web_handler_t *self, bool playing) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnAudioStateChanged(playing);
}

void ARK_WEB_CALLBACK
ark_web_handler_on_over_scroll_fling_end(struct _ark_web_handler_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnOverScrollFlingEnd();
}

bool ARK_WEB_CALLBACK ark_web_handler_on_un_processed_key_event(
    struct _ark_web_handler_t *self, ark_web_key_event_t *event) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnUnProcessedKeyEvent(
      ArkWebKeyEventCToCpp::Invert(event));
}

bool ARK_WEB_CALLBACK ark_web_handler_on_drag_and_drop_data_udmf(
    struct _ark_web_handler_t *self, ark_web_drag_data_t *drag_data) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebHandlerCppToC::Get(self)->OnDragAndDropDataUdmf(
      ArkWebDragDataCToCpp::Invert(drag_data));
}

void ARK_WEB_CALLBACK ark_web_handler_on_first_contentful_paint(
    struct _ark_web_handler_t *self, int64_t navigation_start_tick,
    int64_t first_contentful_paint_ms) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnFirstContentfulPaint(
      navigation_start_tick, first_contentful_paint_ms);
}

void ARK_WEB_CALLBACK ark_web_handler_on_date_time_chooser_popup(
    struct _ark_web_handler_t *self, const ArkWebDateTimeChooser *chooser,
    const ArkWebDateTimeSuggestionVector *suggestions,
    ark_web_date_time_chooser_callback_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(chooser, );

  ARK_WEB_CPPTOC_CHECK_PARAM(suggestions, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnDateTimeChooserPopup(
      *chooser, *suggestions,
      ArkWebDateTimeChooserCallbackCToCpp::Invert(callback));
}

void ARK_WEB_CALLBACK
ark_web_handler_on_date_time_chooser_close(struct _ark_web_handler_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnDateTimeChooserClose();
}

void ARK_WEB_CALLBACK ark_web_handler_on_screen_capture_request(
    struct _ark_web_handler_t *self,
    ark_web_screen_capture_access_request_t *request) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnScreenCaptureRequest(
      ArkWebScreenCaptureAccessRequestCToCpp::Invert(request));
}

void ARK_WEB_CALLBACK ark_web_handler_on_activity_state_changed(
    struct _ark_web_handler_t *self, int state, int type) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnActivityStateChanged(state, type);
}

void ARK_WEB_CALLBACK ark_web_handler_on_get_touch_handle_hot_zone(
    struct _ark_web_handler_t *self, ArkWebTouchHandleHotZone *hot_zone) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(hot_zone, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnGetTouchHandleHotZone(*hot_zone);
}

void ARK_WEB_CALLBACK ark_web_handler_on_complete_swap_with_new_size(
    struct _ark_web_handler_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnCompleteSwapWithNewSize();
}

void ARK_WEB_CALLBACK ark_web_handler_on_over_scroll_fling_velocity(
    struct _ark_web_handler_t *self, float x_velocity, float y_velocity,
    bool is_fling) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnOverScrollFlingVelocity(
      x_velocity, y_velocity, is_fling);
}

void ARK_WEB_CALLBACK ark_web_handler_on_navigation_entry_committed(
    struct _ark_web_handler_t *self,
    ark_web_load_committed_details_t *details) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnNavigationEntryCommitted(
      ArkWebLoadCommittedDetailsCToCpp::Invert(details));
}

void ARK_WEB_CALLBACK ark_web_handler_on_native_embed_lifecycle_change(
    struct _ark_web_handler_t *self,
    ark_web_native_embed_data_info_t *data_info) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnNativeEmbedLifecycleChange(
      ArkWebNativeEmbedDataInfoCToCpp::Invert(data_info));
}

void ARK_WEB_CALLBACK ark_web_handler_on_native_embed_gesture_event(
    struct _ark_web_handler_t *self,
    ark_web_native_embed_touch_event_t *touch_event) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnNativeEmbedGestureEvent(
      ArkWebNativeEmbedTouchEventCToCpp::Invert(touch_event));
}

void ARK_WEB_CALLBACK ark_web_handler_on_safe_browsing_check_result(
    struct _ark_web_handler_t *self, int threat_type) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnSafeBrowsingCheckResult(threat_type);
}

void ARK_WEB_CALLBACK ark_web_handler_on_first_meaningful_paint(
    struct _ark_web_handler_t *self,
    ark_web_first_meaningful_paint_details_t *details) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnFirstMeaningfulPaint(
      ArkWebFirstMeaningfulPaintDetailsCToCpp::Invert(details));
}

void ARK_WEB_CALLBACK ark_web_handler_on_largest_contentful_paint(
    struct _ark_web_handler_t *self,
    ark_web_largest_contentful_paint_details_t *details) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebHandlerCppToC::Get(self)->OnLargestContentfulPaint(
      ArkWebLargestContentfulPaintDetailsCToCpp::Invert(details));
}

} // namespace

ArkWebHandlerCppToC::ArkWebHandlerCppToC() {
  GetStruct()->set_nweb = ark_web_handler_set_nweb;
  GetStruct()->on_focus = ark_web_handler_on_focus;
  GetStruct()->on_message = ark_web_handler_on_message;
  GetStruct()->on_resource = ark_web_handler_on_resource;
  GetStruct()->on_page_icon = ark_web_handler_on_page_icon;
  GetStruct()->on_page_title = ark_web_handler_on_page_title;
  GetStruct()->on_proxy_died = ark_web_handler_on_proxy_died;
  GetStruct()->on_http_error = ark_web_handler_on_http_error;
  GetStruct()->on_console_log = ark_web_handler_on_console_log;
  GetStruct()->on_router_push = ark_web_handler_on_router_push;
  GetStruct()->on_page_load_end = ark_web_handler_on_page_load_end;
  GetStruct()->on_page_load_begin = ark_web_handler_on_page_load_begin;
  GetStruct()->on_page_load_error = ark_web_handler_on_page_load_error;
  GetStruct()->on_desktop_icon_url = ark_web_handler_on_desktop_icon_url;
  GetStruct()->on_loading_progress = ark_web_handler_on_loading_progress;
  GetStruct()->on_geolocation_show = ark_web_handler_on_geolocation_show;
  GetStruct()->on_geolocation_hide = ark_web_handler_on_geolocation_hide;
  GetStruct()->on_file_selector_show = ark_web_handler_on_file_selector_show;
  GetStruct()->on_resource_load_error = ark_web_handler_on_resource_load_error;
  GetStruct()->on_permission_request = ark_web_handler_on_permission_request;
  GetStruct()->on_quick_menu_dismissed =
      ark_web_handler_on_quick_menu_dismissed;
  GetStruct()->on_context_menu_dismissed =
      ark_web_handler_on_context_menu_dismissed;
  GetStruct()->on_touch_selection_changed =
      ark_web_handler_on_touch_selection_changed;
  GetStruct()->on_handle_intercept_request =
      ark_web_handler_on_handle_intercept_request;
  GetStruct()->on_refresh_accessed_history =
      ark_web_handler_on_refresh_accessed_history;
  GetStruct()->on_permission_request_canceled =
      ark_web_handler_on_permission_request_canceled;
  GetStruct()->on_handle_intercept_url_loading =
      ark_web_handler_on_handle_intercept_url_loading;
  GetStruct()->run_quick_menu = ark_web_handler_run_quick_menu;
  GetStruct()->run_context_menu = ark_web_handler_run_context_menu;
  GetStruct()->update_drag_cursor = ark_web_handler_update_drag_cursor;
  GetStruct()->filter_scroll_event = ark_web_handler_filter_scroll_event;
  GetStruct()->visited_url_history = ark_web_handler_visited_url_history;
  GetStruct()->on_window_new_by_js = ark_web_handler_on_window_new_by_js;
  GetStruct()->on_window_exit_by_js = ark_web_handler_on_window_exit_by_js;
  GetStruct()->on_alert_dialog_by_js = ark_web_handler_on_alert_dialog_by_js;
  GetStruct()->on_before_unload_by_js = ark_web_handler_on_before_unload_by_js;
  GetStruct()->on_prompt_dialog_by_js = ark_web_handler_on_prompt_dialog_by_js;
  GetStruct()->on_confirm_dialog_by_js =
      ark_web_handler_on_confirm_dialog_by_js;
  GetStruct()->on_http_auth_request_by_js =
      ark_web_handler_on_http_auth_request_by_js;
  GetStruct()->on_ssl_error_request_by_js =
      ark_web_handler_on_ssl_error_request_by_js;
  GetStruct()->on_ssl_select_cert_request_by_js =
      ark_web_handler_on_ssl_select_cert_request_by_js;
  GetStruct()->on_scroll = ark_web_handler_on_scroll;
  GetStruct()->on_over_scroll = ark_web_handler_on_over_scroll;
  GetStruct()->on_scroll_state = ark_web_handler_on_scroll_state;
  GetStruct()->on_page_visible = ark_web_handler_on_page_visible;
  GetStruct()->on_pre_key_event = ark_web_handler_on_pre_key_event;
  GetStruct()->on_scale_changed = ark_web_handler_on_scale_changed;
  GetStruct()->on_cursor_change = ark_web_handler_on_cursor_change;
  GetStruct()->on_render_exited = ark_web_handler_on_render_exited;
  GetStruct()->on_resize_not_work = ark_web_handler_on_resize_not_work;
  GetStruct()->on_full_screen_exit = ark_web_handler_on_full_screen_exit;
  GetStruct()->on_full_screen_enter = ark_web_handler_on_full_screen_enter;
  GetStruct()->on_drag_and_drop_data = ark_web_handler_on_drag_and_drop_data;
  GetStruct()->on_select_popup_menu = ark_web_handler_on_select_popup_menu;
  GetStruct()->on_data_resubmission = ark_web_handler_on_data_resubmission;
  GetStruct()->on_root_layer_changed = ark_web_handler_on_root_layer_changed;
  GetStruct()->on_audio_state_changed = ark_web_handler_on_audio_state_changed;
  GetStruct()->on_over_scroll_fling_end =
      ark_web_handler_on_over_scroll_fling_end;
  GetStruct()->on_un_processed_key_event =
      ark_web_handler_on_un_processed_key_event;
  GetStruct()->on_drag_and_drop_data_udmf =
      ark_web_handler_on_drag_and_drop_data_udmf;
  GetStruct()->on_first_contentful_paint =
      ark_web_handler_on_first_contentful_paint;
  GetStruct()->on_date_time_chooser_popup =
      ark_web_handler_on_date_time_chooser_popup;
  GetStruct()->on_date_time_chooser_close =
      ark_web_handler_on_date_time_chooser_close;
  GetStruct()->on_screen_capture_request =
      ark_web_handler_on_screen_capture_request;
  GetStruct()->on_activity_state_changed =
      ark_web_handler_on_activity_state_changed;
  GetStruct()->on_get_touch_handle_hot_zone =
      ark_web_handler_on_get_touch_handle_hot_zone;
  GetStruct()->on_complete_swap_with_new_size =
      ark_web_handler_on_complete_swap_with_new_size;
  GetStruct()->on_over_scroll_fling_velocity =
      ark_web_handler_on_over_scroll_fling_velocity;
  GetStruct()->on_navigation_entry_committed =
      ark_web_handler_on_navigation_entry_committed;
  GetStruct()->on_native_embed_lifecycle_change =
      ark_web_handler_on_native_embed_lifecycle_change;
  GetStruct()->on_native_embed_gesture_event =
      ark_web_handler_on_native_embed_gesture_event;
  GetStruct()->on_safe_browsing_check_result =
      ark_web_handler_on_safe_browsing_check_result;
  GetStruct()->on_first_meaningful_paint =
      ark_web_handler_on_first_meaningful_paint;
  GetStruct()->on_largest_contentful_paint =
      ark_web_handler_on_largest_contentful_paint;
}

ArkWebHandlerCppToC::~ArkWebHandlerCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<ArkWebHandlerCppToC, ArkWebHandler,
                                        ark_web_handler_t>::kBridgeType =
    ARK_WEB_HANDLER;

} // namespace OHOS::ArkWeb
