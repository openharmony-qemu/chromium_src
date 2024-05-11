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

#include "ohos_nweb/cpptoc/ark_web_nweb_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "ohos_nweb/cpptoc/ark_web_accessibility_node_info_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_drag_data_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_history_list_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_hit_test_result_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_preference_cpptoc.h"
#include "ohos_nweb/ctocpp/ark_web_accessibility_event_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_bool_value_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_cache_options_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_create_native_media_player_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_download_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_drag_event_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_find_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_handler_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_js_result_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_message_value_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_release_surface_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_screen_lock_callback_ctocpp.h"
#include "ohos_nweb/ctocpp/ark_web_string_value_callback_ctocpp.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_web_nweb_resize(struct _ark_web_nweb_t *self,
                                          uint32_t width, uint32_t height,
                                          bool is_keyboard) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->Resize(width, height, is_keyboard);
}

void ARK_WEB_CALLBACK ark_web_nweb_on_pause(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnPause();
}

void ARK_WEB_CALLBACK ark_web_nweb_on_continue(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnContinue();
}

void ARK_WEB_CALLBACK ark_web_nweb_on_destroy(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnDestroy();
}

void ARK_WEB_CALLBACK ark_web_nweb_on_focus(struct _ark_web_nweb_t *self,
                                            const int32_t *focus_reason) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(focus_reason, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnFocus(*focus_reason);
}

void ARK_WEB_CALLBACK ark_web_nweb_on_blur(struct _ark_web_nweb_t *self,
                                           const int32_t *blur_reason) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(blur_reason, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnBlur(*blur_reason);
}

void ARK_WEB_CALLBACK ark_web_nweb_on_touch_press(struct _ark_web_nweb_t *self,
                                                  int32_t id, double x,
                                                  double y, bool from_overlay) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnTouchPress(id, x, y, from_overlay);
}

void ARK_WEB_CALLBACK
ark_web_nweb_on_touch_release(struct _ark_web_nweb_t *self, int32_t id,
                              double x, double y, bool from_overlay) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnTouchRelease(id, x, y, from_overlay);
}

void ARK_WEB_CALLBACK ark_web_nweb_on_touch_move1(struct _ark_web_nweb_t *self,
                                                  int32_t id, double x,
                                                  double y, bool from_overlay) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnTouchMove(id, x, y, from_overlay);
}

void ARK_WEB_CALLBACK ark_web_nweb_on_touch_move2(
    struct _ark_web_nweb_t *self,
    const ArkWebTouchPointInfoVector *touch_point_infos, bool from_overlay) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(touch_point_infos, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnTouchMove(*touch_point_infos, from_overlay);
}

void ARK_WEB_CALLBACK
ark_web_nweb_on_touch_cancel(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnTouchCancel();
}

void ARK_WEB_CALLBACK
ark_web_nweb_on_navigate_back(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnNavigateBack();
}

bool ARK_WEB_CALLBACK ark_web_nweb_send_key_event(struct _ark_web_nweb_t *self,
                                                  int32_t key_code,
                                                  int32_t key_action) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->SendKeyEvent(key_code, key_action);
}

void ARK_WEB_CALLBACK
ark_web_nweb_send_mouse_wheel_event(struct _ark_web_nweb_t *self, double x,
                                    double y, double delta_x, double delta_y) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SendMouseWheelEvent(x, y, delta_x, delta_y);
}

void ARK_WEB_CALLBACK
ark_web_nweb_send_mouse_event(struct _ark_web_nweb_t *self, int x, int y,
                              int button, int action, int count) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SendMouseEvent(x, y, button, action, count);
}

int ARK_WEB_CALLBACK ark_web_nweb_load1(struct _ark_web_nweb_t *self,
                                        const ArkWebString *url) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(url, 0);

  // Execute
  return ArkWebNWebCppToC::Get(self)->Load(*url);
}

bool ARK_WEB_CALLBACK
ark_web_nweb_is_navigatebackward_allowed(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->IsNavigatebackwardAllowed();
}

bool ARK_WEB_CALLBACK
ark_web_nweb_is_navigate_forward_allowed(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->IsNavigateForwardAllowed();
}

bool ARK_WEB_CALLBACK ark_web_nweb_can_navigate_back_or_forward(
    struct _ark_web_nweb_t *self, int num_steps) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->CanNavigateBackOrForward(num_steps);
}

void ARK_WEB_CALLBACK ark_web_nweb_navigate_back(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->NavigateBack();
}

void ARK_WEB_CALLBACK
ark_web_nweb_navigate_forward(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->NavigateForward();
}

void ARK_WEB_CALLBACK
ark_web_nweb_navigate_back_or_forward(struct _ark_web_nweb_t *self, int step) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->NavigateBackOrForward(step);
}

void ARK_WEB_CALLBACK
ark_web_nweb_delete_navigate_history(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->DeleteNavigateHistory();
}

void ARK_WEB_CALLBACK ark_web_nweb_reload(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->Reload();
}

int ARK_WEB_CALLBACK ark_web_nweb_zoom(struct _ark_web_nweb_t *self,
                                       float zoom_factor) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNWebCppToC::Get(self)->Zoom(zoom_factor);
}

int ARK_WEB_CALLBACK ark_web_nweb_zoom_in(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNWebCppToC::Get(self)->ZoomIn();
}

int ARK_WEB_CALLBACK ark_web_nweb_zoom_out(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNWebCppToC::Get(self)->ZoomOut();
}

void ARK_WEB_CALLBACK ark_web_nweb_stop(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->Stop();
}

void ARK_WEB_CALLBACK ark_web_nweb_execute_java_script1(
    struct _ark_web_nweb_t *self, const ArkWebString *code) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(code, );

  // Execute
  ArkWebNWebCppToC::Get(self)->ExecuteJavaScript(*code);
}

void ARK_WEB_CALLBACK ark_web_nweb_execute_java_script2(
    struct _ark_web_nweb_t *self, const ArkWebString *code,
    ark_web_message_value_callback_t *callback, bool extention) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(code, );

  // Execute
  ArkWebNWebCppToC::Get(self)->ExecuteJavaScript(
      *code, ArkWebMessageValueCallbackCToCpp::Invert(callback), extention);
}

ark_web_preference_t *ARK_WEB_CALLBACK
ark_web_nweb_get_preference(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebPreference> _retval =
      ArkWebNWebCppToC::Get(self)->GetPreference();

  // Return type: refptr_same
  return ArkWebPreferenceCppToC::Invert(_retval);
}

unsigned int ARK_WEB_CALLBACK
ark_web_nweb_get_web_id(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNWebCppToC::Get(self)->GetWebId();
}

ark_web_hit_test_result_t *ARK_WEB_CALLBACK
ark_web_nweb_get_hit_test_result(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebHitTestResult> _retval =
      ArkWebNWebCppToC::Get(self)->GetHitTestResult();

  // Return type: refptr_same
  return ArkWebHitTestResultCppToC::Invert(_retval);
}

void ARK_WEB_CALLBACK
ark_web_nweb_put_background_color(struct _ark_web_nweb_t *self, int color) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->PutBackgroundColor(color);
}

void ARK_WEB_CALLBACK ark_web_nweb_initial_scale(struct _ark_web_nweb_t *self,
                                                 float scale) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->InitialScale(scale);
}

void ARK_WEB_CALLBACK ark_web_nweb_put_download_callback(
    struct _ark_web_nweb_t *self,
    ark_web_download_callback_t *download_listener) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->PutDownloadCallback(
      ArkWebDownloadCallbackCToCpp::Invert(download_listener));
}

void ARK_WEB_CALLBACK ark_web_nweb_put_accessibility_event_callback(
    struct _ark_web_nweb_t *self,
    ark_web_accessibility_event_callback_t *accessibility_event_listener) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->PutAccessibilityEventCallback(
      ArkWebAccessibilityEventCallbackCToCpp::Invert(
          accessibility_event_listener));
}

void ARK_WEB_CALLBACK ark_web_nweb_put_accessibility_id_generator(
    struct _ark_web_nweb_t *self,
    const AccessibilityIdGenerateFunc accessibility_id_generator) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->PutAccessibilityIdGenerator(
      accessibility_id_generator);
}

void ARK_WEB_CALLBACK ark_web_nweb_set_nweb_handler(
    struct _ark_web_nweb_t *self, ark_web_handler_t *handler) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetNWebHandler(
      ArkWebHandlerCToCpp::Invert(handler));
}

ArkWebString ARK_WEB_CALLBACK ark_web_nweb_title(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebNWebCppToC::Get(self)->Title();
}

int ARK_WEB_CALLBACK
ark_web_nweb_page_load_progress(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNWebCppToC::Get(self)->PageLoadProgress();
}

int ARK_WEB_CALLBACK ark_web_nweb_content_height(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNWebCppToC::Get(self)->ContentHeight();
}

float ARK_WEB_CALLBACK ark_web_nweb_scale(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNWebCppToC::Get(self)->Scale();
}

int ARK_WEB_CALLBACK
ark_web_nweb_load2(struct _ark_web_nweb_t *self, const ArkWebString *url,
                   const ArkWebStringMap *additional_http_headers) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(url, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(additional_http_headers, 0);

  // Execute
  return ArkWebNWebCppToC::Get(self)->Load(*url, *additional_http_headers);
}

int ARK_WEB_CALLBACK ark_web_nweb_load_with_data_and_base_url(
    struct _ark_web_nweb_t *self, const ArkWebString *base_url,
    const ArkWebString *data, const ArkWebString *mime_type,
    const ArkWebString *encoding, const ArkWebString *history_url) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(base_url, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(data, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(mime_type, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(encoding, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(history_url, 0);

  // Execute
  return ArkWebNWebCppToC::Get(self)->LoadWithDataAndBaseUrl(
      *base_url, *data, *mime_type, *encoding, *history_url);
}

int ARK_WEB_CALLBACK ark_web_nweb_load_with_data(struct _ark_web_nweb_t *self,
                                                 const ArkWebString *data,
                                                 const ArkWebString *mime_type,
                                                 const ArkWebString *encoding) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(data, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(mime_type, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(encoding, 0);

  // Execute
  return ArkWebNWebCppToC::Get(self)->LoadWithData(*data, *mime_type,
                                                   *encoding);
}

void ARK_WEB_CALLBACK ark_web_nweb_register_ark_jsfunction1(
    struct _ark_web_nweb_t *self, const ArkWebString *object_name,
    const ArkWebStringVector *method_list, const int32_t object_id) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(object_name, );

  ARK_WEB_CPPTOC_CHECK_PARAM(method_list, );

  // Execute
  ArkWebNWebCppToC::Get(self)->RegisterArkJSfunction(*object_name, *method_list,
                                                     object_id);
}

void ARK_WEB_CALLBACK ark_web_nweb_unregister_ark_jsfunction(
    struct _ark_web_nweb_t *self, const ArkWebString *object_name,
    const ArkWebStringVector *method_list) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(object_name, );

  ARK_WEB_CPPTOC_CHECK_PARAM(method_list, );

  // Execute
  ArkWebNWebCppToC::Get(self)->UnregisterArkJSfunction(*object_name,
                                                       *method_list);
}

void ARK_WEB_CALLBACK ark_web_nweb_set_nweb_java_script_result_call_back(
    struct _ark_web_nweb_t *self, ark_web_js_result_callback_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetNWebJavaScriptResultCallBack(
      ArkWebJsResultCallbackCToCpp::Invert(callback));
}

void ARK_WEB_CALLBACK ark_web_nweb_put_find_callback(
    struct _ark_web_nweb_t *self, ark_web_find_callback_t *find_listener) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->PutFindCallback(
      ArkWebFindCallbackCToCpp::Invert(find_listener));
}

void ARK_WEB_CALLBACK ark_web_nweb_find_all_async(
    struct _ark_web_nweb_t *self, const ArkWebString *search_str) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(search_str, );

  // Execute
  ArkWebNWebCppToC::Get(self)->FindAllAsync(*search_str);
}

void ARK_WEB_CALLBACK ark_web_nweb_clear_matches(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->ClearMatches();
}

void ARK_WEB_CALLBACK ark_web_nweb_find_next(struct _ark_web_nweb_t *self,
                                             const bool forward) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->FindNext(forward);
}

void ARK_WEB_CALLBACK ark_web_nweb_store_web_archive(
    struct _ark_web_nweb_t *self, const ArkWebString *base_name, bool auto_name,
    ark_web_string_value_callback_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(base_name, );

  // Execute
  ArkWebNWebCppToC::Get(self)->StoreWebArchive(
      *base_name, auto_name, ArkWebStringValueCallbackCToCpp::Invert(callback));
}

ArkWebStringVector ARK_WEB_CALLBACK
ark_web_nweb_create_web_message_ports(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_vector_default);

  // Execute
  return ArkWebNWebCppToC::Get(self)->CreateWebMessagePorts();
}

void ARK_WEB_CALLBACK ark_web_nweb_post_web_message(
    struct _ark_web_nweb_t *self, const ArkWebString *message,
    const ArkWebStringVector *ports, const ArkWebString *target_uri) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(message, );

  ARK_WEB_CPPTOC_CHECK_PARAM(ports, );

  ARK_WEB_CPPTOC_CHECK_PARAM(target_uri, );

  // Execute
  ArkWebNWebCppToC::Get(self)->PostWebMessage(*message, *ports, *target_uri);
}

void ARK_WEB_CALLBACK ark_web_nweb_close_port(struct _ark_web_nweb_t *self,
                                              const ArkWebString *port_handle) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(port_handle, );

  // Execute
  ArkWebNWebCppToC::Get(self)->ClosePort(*port_handle);
}

void ARK_WEB_CALLBACK ark_web_nweb_post_port_message(
    struct _ark_web_nweb_t *self, const ArkWebString *port_handle,
    const ArkWebMessage *data) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(port_handle, );

  ARK_WEB_CPPTOC_CHECK_PARAM(data, );

  // Execute
  ArkWebNWebCppToC::Get(self)->PostPortMessage(*port_handle, *data);
}

void ARK_WEB_CALLBACK ark_web_nweb_set_port_message_callback(
    struct _ark_web_nweb_t *self, const ArkWebString *port_handle,
    ark_web_message_value_callback_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(port_handle, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetPortMessageCallback(
      *port_handle, ArkWebMessageValueCallbackCToCpp::Invert(callback));
}

void ARK_WEB_CALLBACK ark_web_nweb_send_drag_event(
    struct _ark_web_nweb_t *self, ark_web_drag_event_t *drag_event) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SendDragEvent(
      ArkWebDragEventCToCpp::Invert(drag_event));
}

void ARK_WEB_CALLBACK
ark_web_nweb_clear_ssl_cache(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->ClearSslCache();
}

ArkWebString ARK_WEB_CALLBACK
ark_web_nweb_get_url(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebNWebCppToC::Get(self)->GetUrl();
}

void ARK_WEB_CALLBACK
ark_web_nweb_clear_client_authentication_cache(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->ClearClientAuthenticationCache();
}

void ARK_WEB_CALLBACK ark_web_nweb_update_locale(struct _ark_web_nweb_t *self,
                                                 const ArkWebString *language,
                                                 const ArkWebString *region) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(language, );

  ARK_WEB_CPPTOC_CHECK_PARAM(region, );

  // Execute
  ArkWebNWebCppToC::Get(self)->UpdateLocale(*language, *region);
}

const ArkWebString ARK_WEB_CALLBACK
ark_web_nweb_get_original_url(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebNWebCppToC::Get(self)->GetOriginalUrl();
}

bool ARK_WEB_CALLBACK ark_web_nweb_get_favicon(struct _ark_web_nweb_t *self,
                                               const void **data, size_t *width,
                                               size_t *height, int *color_type,
                                               int *alpha_type) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(data, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(width, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(height, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(color_type, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(alpha_type, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->GetFavicon(data, *width, *height,
                                                 *color_type, *alpha_type);
}

void ARK_WEB_CALLBACK ark_web_nweb_put_network_available(
    struct _ark_web_nweb_t *self, bool available) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->PutNetworkAvailable(available);
}

void ARK_WEB_CALLBACK ark_web_nweb_has_images(
    struct _ark_web_nweb_t *self, ark_web_bool_value_callback_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->HasImages(
      ArkWebBoolValueCallbackCToCpp::Invert(callback));
}

void ARK_WEB_CALLBACK ark_web_nweb_remove_cache(struct _ark_web_nweb_t *self,
                                                bool include_disk_files) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->RemoveCache(include_disk_files);
}

ark_web_history_list_t *ARK_WEB_CALLBACK
ark_web_nweb_get_history_list(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebHistoryList> _retval =
      ArkWebNWebCppToC::Get(self)->GetHistoryList();

  // Return type: refptr_same
  return ArkWebHistoryListCppToC::Invert(_retval);
}

void ARK_WEB_CALLBACK ark_web_nweb_put_release_surface_callback(
    struct _ark_web_nweb_t *self,
    ark_web_release_surface_callback_t *release_surface_listener) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->PutReleaseSurfaceCallback(
      ArkWebReleaseSurfaceCallbackCToCpp::Invert(release_surface_listener));
}

ArkWebUint8Vector ARK_WEB_CALLBACK
ark_web_nweb_serialize_web_state(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_uint8_vector_default);

  // Execute
  return ArkWebNWebCppToC::Get(self)->SerializeWebState();
}

bool ARK_WEB_CALLBACK ark_web_nweb_restore_web_state(
    struct _ark_web_nweb_t *self, const ArkWebUint8Vector *state) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(state, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->RestoreWebState(*state);
}

void ARK_WEB_CALLBACK ark_web_nweb_page_up(struct _ark_web_nweb_t *self,
                                           bool top) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->PageUp(top);
}

void ARK_WEB_CALLBACK ark_web_nweb_page_down(struct _ark_web_nweb_t *self,
                                             bool bottom) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->PageDown(bottom);
}

void ARK_WEB_CALLBACK ark_web_nweb_scroll_to(struct _ark_web_nweb_t *self,
                                             float x, float y) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->ScrollTo(x, y);
}

void ARK_WEB_CALLBACK ark_web_nweb_scroll_by(struct _ark_web_nweb_t *self,
                                             float delta_x, float delta_y) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->ScrollBy(delta_x, delta_y);
}

void ARK_WEB_CALLBACK ark_web_nweb_slide_scroll(struct _ark_web_nweb_t *self,
                                                float vx, float vy) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SlideScroll(vx, vy);
}

bool ARK_WEB_CALLBACK ark_web_nweb_get_cert_chain_der_data(
    struct _ark_web_nweb_t *self, ArkWebStringVector *cert_chain_data,
    bool is_single_cert) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(cert_chain_data, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->GetCertChainDerData(*cert_chain_data,
                                                          is_single_cert);
}

void ARK_WEB_CALLBACK ark_web_nweb_set_screen_off_set(
    struct _ark_web_nweb_t *self, double x, double y) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetScreenOffSet(x, y);
}

void ARK_WEB_CALLBACK ark_web_nweb_set_audio_muted(struct _ark_web_nweb_t *self,
                                                   bool muted) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetAudioMuted(muted);
}

void ARK_WEB_CALLBACK ark_web_nweb_set_should_frame_submission_before_draw(
    struct _ark_web_nweb_t *self, bool should) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetShouldFrameSubmissionBeforeDraw(should);
}

void ARK_WEB_CALLBACK ark_web_nweb_notify_popup_window_result(
    struct _ark_web_nweb_t *self, bool result) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->NotifyPopupWindowResult(result);
}

void ARK_WEB_CALLBACK ark_web_nweb_set_audio_resume_interval(
    struct _ark_web_nweb_t *self, int32_t resume_interval) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetAudioResumeInterval(resume_interval);
}

void ARK_WEB_CALLBACK ark_web_nweb_set_audio_exclusive(
    struct _ark_web_nweb_t *self, bool audio_exclusive) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetAudioExclusive(audio_exclusive);
}

void ARK_WEB_CALLBACK ark_web_nweb_register_screen_lock_function(
    struct _ark_web_nweb_t *self, int32_t window_id,
    ark_web_screen_lock_callback_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->RegisterScreenLockFunction(
      window_id, ArkWebScreenLockCallbackCToCpp::Invert(callback));
}

void ARK_WEB_CALLBACK ark_web_nweb_un_register_screen_lock_function(
    struct _ark_web_nweb_t *self, int32_t window_id) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->UnRegisterScreenLockFunction(window_id);
}

void ARK_WEB_CALLBACK
ark_web_nweb_notify_memory_level(struct _ark_web_nweb_t *self, int32_t level) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->NotifyMemoryLevel(level);
}

void ARK_WEB_CALLBACK
ark_web_nweb_on_webview_hide(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnWebviewHide();
}

void ARK_WEB_CALLBACK
ark_web_nweb_on_webview_show(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnWebviewShow();
}

ark_web_drag_data_t *ARK_WEB_CALLBACK
ark_web_nweb_get_or_create_drag_data(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebDragData> _retval =
      ArkWebNWebCppToC::Get(self)->GetOrCreateDragData();

  // Return type: refptr_same
  return ArkWebDragDataCppToC::Invert(_retval);
}

void ARK_WEB_CALLBACK ark_web_nweb_prefetch_page(
    struct _ark_web_nweb_t *self, const ArkWebString *url,
    const ArkWebStringMap *additional_http_headers) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(url, );

  ARK_WEB_CPPTOC_CHECK_PARAM(additional_http_headers, );

  // Execute
  ArkWebNWebCppToC::Get(self)->PrefetchPage(*url, *additional_http_headers);
}

void ARK_WEB_CALLBACK ark_web_nweb_set_window_id(struct _ark_web_nweb_t *self,
                                                 uint32_t window_id) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetWindowId(window_id);
}

void ARK_WEB_CALLBACK ark_web_nweb_on_occluded(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnOccluded();
}

void ARK_WEB_CALLBACK ark_web_nweb_on_unoccluded(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnUnoccluded();
}

void ARK_WEB_CALLBACK ark_web_nweb_set_token(struct _ark_web_nweb_t *self,
                                             void *token) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(token, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetToken(token);
}

void ARK_WEB_CALLBACK ark_web_nweb_set_nested_scroll_mode(
    struct _ark_web_nweb_t *self, const int32_t *nested_scroll_mode) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(nested_scroll_mode, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetNestedScrollMode(*nested_scroll_mode);
}

void ARK_WEB_CALLBACK ark_web_nweb_set_enable_lower_frame_rate(
    struct _ark_web_nweb_t *self, bool enabled) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetEnableLowerFrameRate(enabled);
}

void ARK_WEB_CALLBACK ark_web_nweb_set_virtual_key_board_arg(
    struct _ark_web_nweb_t *self, int32_t width, int32_t height,
    double keyboard) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetVirtualKeyBoardArg(width, height, keyboard);
}

bool ARK_WEB_CALLBACK
ark_web_nweb_should_virtual_keyboard_overlay(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->ShouldVirtualKeyboardOverlay();
}

void ARK_WEB_CALLBACK ark_web_nweb_set_draw_rect(struct _ark_web_nweb_t *self,
                                                 int32_t x, int32_t y,
                                                 int32_t width,
                                                 int32_t height) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetDrawRect(x, y, width, height);
}

void ARK_WEB_CALLBACK ark_web_nweb_set_draw_mode(struct _ark_web_nweb_t *self,
                                                 int32_t mode) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetDrawMode(mode);
}

void *ARK_WEB_CALLBACK ark_web_nweb_create_web_print_document_adapter(
    struct _ark_web_nweb_t *self, const ArkWebString *job_name) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  ARK_WEB_CPPTOC_CHECK_PARAM(job_name, NULL);

  // Execute
  return ArkWebNWebCppToC::Get(self)->CreateWebPrintDocumentAdapter(*job_name);
}

int ARK_WEB_CALLBACK ark_web_nweb_post_url(struct _ark_web_nweb_t *self,
                                           const ArkWebString *url,
                                           const ArkWebCharVector *post_data) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(url, 0);

  ARK_WEB_CPPTOC_CHECK_PARAM(post_data, 0);

  // Execute
  return ArkWebNWebCppToC::Get(self)->PostUrl(*url, *post_data);
}

void ARK_WEB_CALLBACK ark_web_nweb_java_script_on_document_start(
    struct _ark_web_nweb_t *self, const ArkWebStringVectorMap *script_items) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(script_items, );

  // Execute
  ArkWebNWebCppToC::Get(self)->JavaScriptOnDocumentStart(*script_items);
}

void ARK_WEB_CALLBACK ark_web_nweb_execute_action(struct _ark_web_nweb_t *self,
                                                  int64_t accessibility_id,
                                                  uint32_t action) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->ExecuteAction(accessibility_id, action);
}

ark_web_accessibility_node_info_t *ARK_WEB_CALLBACK
ark_web_nweb_get_focused_accessibility_node_info(struct _ark_web_nweb_t *self,
                                                 int64_t accessibility_id,
                                                 bool is_accessibility_focus) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebAccessibilityNodeInfo> _retval =
      ArkWebNWebCppToC::Get(self)->GetFocusedAccessibilityNodeInfo(
          accessibility_id, is_accessibility_focus);

  // Return type: refptr_same
  return ArkWebAccessibilityNodeInfoCppToC::Invert(_retval);
}

ark_web_accessibility_node_info_t *ARK_WEB_CALLBACK
ark_web_nweb_get_accessibility_node_info_by_id(struct _ark_web_nweb_t *self,
                                               int64_t accessibility_id) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebAccessibilityNodeInfo> _retval =
      ArkWebNWebCppToC::Get(self)->GetAccessibilityNodeInfoById(
          accessibility_id);

  // Return type: refptr_same
  return ArkWebAccessibilityNodeInfoCppToC::Invert(_retval);
}

ark_web_accessibility_node_info_t *ARK_WEB_CALLBACK
ark_web_nweb_get_accessibility_node_info_by_focus_move(
    struct _ark_web_nweb_t *self, int64_t accessibility_id, int32_t direction) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

  // Execute
  ArkWebRefPtr<ArkWebAccessibilityNodeInfo> _retval =
      ArkWebNWebCppToC::Get(self)->GetAccessibilityNodeInfoByFocusMove(
          accessibility_id, direction);

  // Return type: refptr_same
  return ArkWebAccessibilityNodeInfoCppToC::Invert(_retval);
}

void ARK_WEB_CALLBACK
ark_web_nweb_set_accessibility_state(struct _ark_web_nweb_t *self, bool state) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetAccessibilityState(state);
}

bool ARK_WEB_CALLBACK
ark_web_nweb_need_soft_keyboard(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->NeedSoftKeyboard();
}

bool ARK_WEB_CALLBACK ark_web_nweb_discard(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->Discard();
}

bool ARK_WEB_CALLBACK ark_web_nweb_restore(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->Restore();
}

int ARK_WEB_CALLBACK
ark_web_nweb_get_security_level(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNWebCppToC::Get(self)->GetSecurityLevel();
}

void ARK_WEB_CALLBACK ark_web_nweb_call_h5function(
    struct _ark_web_nweb_t *self, int32_t routingId, int32_t h5ObjectId,
    const ArkWebString *h5MethodName, const ArkWebValueVector *args) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(h5MethodName, );

  ARK_WEB_CPPTOC_CHECK_PARAM(args, );

  // Execute
  ArkWebNWebCppToC::Get(self)->CallH5Function(routingId, h5ObjectId,
                                              *h5MethodName, *args);
}

bool ARK_WEB_CALLBACK
ark_web_nweb_is_incognito_mode(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->IsIncognitoMode();
}

void ARK_WEB_CALLBACK ark_web_nweb_register_native_ark_jsfunction(
    struct _ark_web_nweb_t *self, const char *objName,
    const ArkWebJsProxyCallbackVector *callbacks) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(objName, );

  ARK_WEB_CPPTOC_CHECK_PARAM(callbacks, );

  // Execute
  ArkWebNWebCppToC::Get(self)->RegisterNativeArkJSFunction(objName, *callbacks);
}

void ARK_WEB_CALLBACK ark_web_nweb_un_register_native_ark_jsfunction(
    struct _ark_web_nweb_t *self, const char *objName) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(objName, );

  // Execute
  ArkWebNWebCppToC::Get(self)->UnRegisterNativeArkJSFunction(objName);
}

void ARK_WEB_CALLBACK ark_web_nweb_register_native_valide_callback(
    struct _ark_web_nweb_t *self, const char *webName,
    const NativeArkWebOnValidCallback callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(webName, );

  // Execute
  ArkWebNWebCppToC::Get(self)->RegisterNativeValideCallback(webName, callback);
}

void ARK_WEB_CALLBACK ark_web_nweb_register_native_destroy_callback(
    struct _ark_web_nweb_t *self, const char *webName,
    const NativeArkWebOnValidCallback callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(webName, );

  // Execute
  ArkWebNWebCppToC::Get(self)->RegisterNativeDestroyCallback(webName, callback);
}

void ARK_WEB_CALLBACK ark_web_nweb_java_script_on_document_end(
    struct _ark_web_nweb_t *self, const ArkWebStringVectorMap *script_items) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(script_items, );

  // Execute
  ArkWebNWebCppToC::Get(self)->JavaScriptOnDocumentEnd(*script_items);
}

void ARK_WEB_CALLBACK
ark_web_nweb_enable_safe_browsing(struct _ark_web_nweb_t *self, bool enable) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->EnableSafeBrowsing(enable);
}

bool ARK_WEB_CALLBACK
ark_web_nweb_is_safe_browsing_enabled(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->IsSafeBrowsingEnabled();
}

void ARK_WEB_CALLBACK
ark_web_nweb_set_print_background(struct _ark_web_nweb_t *self, bool enable) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetPrintBackground(enable);
}

bool ARK_WEB_CALLBACK
ark_web_nweb_get_print_background(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->GetPrintBackground();
}

void ARK_WEB_CALLBACK
ark_web_nweb_close_all_media_presentations(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->CloseAllMediaPresentations();
}

void ARK_WEB_CALLBACK
ark_web_nweb_stop_all_media(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->StopAllMedia();
}

void ARK_WEB_CALLBACK
ark_web_nweb_resume_all_media(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->ResumeAllMedia();
}

void ARK_WEB_CALLBACK
ark_web_nweb_pause_all_media(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->PauseAllMedia();
}

int ARK_WEB_CALLBACK
ark_web_nweb_get_media_playback_state(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNWebCppToC::Get(self)->GetMediaPlaybackState();
}

void ARK_WEB_CALLBACK ark_web_nweb_enable_intelligent_tracking_prevention(
    struct _ark_web_nweb_t *self, bool enable) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->EnableIntelligentTrackingPrevention(enable);
}

bool ARK_WEB_CALLBACK ark_web_nweb_is_intelligent_tracking_prevention_enabled(
    struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->IsIntelligentTrackingPreventionEnabled();
}

void ARK_WEB_CALLBACK ark_web_nweb_start_camera(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->StartCamera();
}

void ARK_WEB_CALLBACK ark_web_nweb_stop_camera(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->StopCamera();
}

void ARK_WEB_CALLBACK ark_web_nweb_close_camera(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->CloseCamera();
}

ArkWebString ARK_WEB_CALLBACK
ark_web_nweb_get_last_javascript_proxy_calling_frame_url(
    struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebNWebCppToC::Get(self)->GetLastJavascriptProxyCallingFrameUrl();
}

bool ARK_WEB_CALLBACK
ark_web_nweb_get_pending_size_status(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->GetPendingSizeStatus();
}

void ARK_WEB_CALLBACK
ark_web_nweb_scroll_by_ref_screen(struct _ark_web_nweb_t *self, float delta_x,
                                  float delta_y, float vx, float vy) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->ScrollByRefScreen(delta_x, delta_y, vx, vy);
}

void ARK_WEB_CALLBACK ark_web_nweb_execute_java_script_ext(
    struct _ark_web_nweb_t *self, const int fd, const size_t scriptLength,
    ark_web_message_value_callback_t *callback, bool extention) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->ExecuteJavaScriptExt(
      fd, scriptLength, ArkWebMessageValueCallbackCToCpp::Invert(callback),
      extention);
}

void ARK_WEB_CALLBACK
ark_web_nweb_on_render_to_background(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnRenderToBackground();
}

void ARK_WEB_CALLBACK
ark_web_nweb_on_render_to_foreground(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnRenderToForeground();
}

void ARK_WEB_CALLBACK ark_web_nweb_precompile_java_script(
    struct _ark_web_nweb_t *self, const ArkWebString *url,
    const ArkWebString *script, ark_web_cache_options_t **cacheOptions,
    ark_web_message_value_callback_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(url, );

  ARK_WEB_CPPTOC_CHECK_PARAM(script, );

  ARK_WEB_CPPTOC_CHECK_PARAM(cacheOptions, );

  // Translate param: cacheOptions; type: refptr_diff_byref
  ArkWebRefPtr<ArkWebCacheOptions> cacheOptionsPtr;
  if (cacheOptions && *cacheOptions) {
    cacheOptionsPtr = ArkWebCacheOptionsCToCpp::Invert(*cacheOptions);
  }
  ArkWebCacheOptions *cacheOptionsOrig = cacheOptionsPtr.get();

  // Execute
  ArkWebNWebCppToC::Get(self)->PrecompileJavaScript(
      *url, *script, cacheOptionsPtr,
      ArkWebMessageValueCallbackCToCpp::Invert(callback));

  // Restore param: cacheOptions; type: refptr_diff_byref
  if (cacheOptions) {
    if (cacheOptionsPtr.get()) {
      if (cacheOptionsPtr.get() != cacheOptionsOrig) {
        *cacheOptions = ArkWebCacheOptionsCToCpp::Revert(cacheOptionsPtr);
      }
    } else {
      *cacheOptions = nullptr;
    }
  }
}

void ARK_WEB_CALLBACK ark_web_nweb_on_create_native_media_player(
    struct _ark_web_nweb_t *self,
    ark_web_create_native_media_player_callback_t *callback) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnCreateNativeMediaPlayer(
      ArkWebCreateNativeMediaPlayerCallbackCToCpp::Invert(callback));
}

void ARK_WEB_CALLBACK ark_web_nweb_drag_resize(struct _ark_web_nweb_t *self,
                                               uint32_t width, uint32_t height,
                                               uint32_t pre_height,
                                               uint32_t pre_width) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );
  // Execute
  ArkWebNWebCppToC::Get(self)->DragResize(width, height, pre_height, pre_width);
}

void ARK_WEB_CALLBACK
ark_web_nweb_on_touch_cancel_by_id(struct _ark_web_nweb_t *self, int32_t id,
                                   double x, double y, bool from_overlay) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnTouchCancelById(id, x, y, from_overlay);
}

int ARK_WEB_CALLBACK
ark_web_nweb_scale_gesture_change(struct _ark_web_nweb_t *self, double scale,
                                  double centerX, double centerY) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

  // Execute
  return ArkWebNWebCppToC::Get(self)->ScaleGestureChange(scale, centerX,
                                                         centerY);
}


void ARK_WEB_CALLBACK ark_web_nweb_inject_offline_resource(
    struct _ark_web_nweb_t *self, const ArkWebString *url,
    const ArkWebString *origin, const ArkWebUint8Vector *resource,
    const ArkWebStringMap *responseHeaders, const int type) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(url, );

  ARK_WEB_CPPTOC_CHECK_PARAM(origin, );

  ARK_WEB_CPPTOC_CHECK_PARAM(resource, );

  ARK_WEB_CPPTOC_CHECK_PARAM(responseHeaders, );

  // Execute
  ArkWebNWebCppToC::Get(self)->InjectOfflineResource(*url, *origin, *resource,
                                                     *responseHeaders, type);
}

bool ARK_WEB_CALLBACK
ark_web_nweb_terminate_render_process(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebNWebCppToC::Get(self)->TerminateRenderProcess();
}

void ARK_WEB_CALLBACK
ark_web_nweb_suggestion_selected(struct _ark_web_nweb_t *self, int32_t index) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SuggestionSelected(index);
}

void ARK_WEB_CALLBACK ark_web_nweb_send_touchpad_fling_event(struct _ark_web_nweb_t *self,
                                                             double x, double y, double vx, double vy) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SendTouchpadFlingEvent(
      x,
      y,
      vx,
      vy);
}

void ARK_WEB_CALLBACK ark_web_nweb_register_ark_jsfunction2(
    struct _ark_web_nweb_t *self, const ArkWebString *object_name,
    const ArkWebStringVector *method_list,
    const ArkWebStringVector *async_method_list, const int32_t object_id) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(object_name, );

  ARK_WEB_CPPTOC_CHECK_PARAM(method_list, );

  ARK_WEB_CPPTOC_CHECK_PARAM(async_method_list, );

  // Execute
  ArkWebNWebCppToC::Get(self)->RegisterArkJSfunction(
      *object_name, *method_list, *async_method_list, object_id);
}

void ARK_WEB_CALLBACK ark_web_nweb_set_fit_content_mode(struct _ark_web_nweb_t *self,
                                                 int32_t mode) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->SetFitContentMode(mode);
}

ArkWebString ARK_WEB_CALLBACK ark_web_nweb_get_select_info(
    struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebNWebCppToC::Get(self)->GetSelectInfo();
}

void ARK_WEB_CALLBACK
ark_web_nweb_on_safe_insets_change(struct _ark_web_nweb_t* self,
                                   int left,
                                   int top,
                                   int right,
                                   int bottom) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnSafeInsetsChange(left, top, right, bottom);
}

void ARK_WEB_CALLBACK ark_web_nweb_on_online_render_to_foreground(struct _ark_web_nweb_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebNWebCppToC::Get(self)->OnOnlineRenderToForeground();
}
} // namespace

ArkWebNWebCppToC::ArkWebNWebCppToC() {
  GetStruct()->resize = ark_web_nweb_resize;
  GetStruct()->on_pause = ark_web_nweb_on_pause;
  GetStruct()->on_continue = ark_web_nweb_on_continue;
  GetStruct()->on_destroy = ark_web_nweb_on_destroy;
  GetStruct()->on_focus = ark_web_nweb_on_focus;
  GetStruct()->on_blur = ark_web_nweb_on_blur;
  GetStruct()->on_touch_press = ark_web_nweb_on_touch_press;
  GetStruct()->on_touch_release = ark_web_nweb_on_touch_release;
  GetStruct()->on_touch_move1 = ark_web_nweb_on_touch_move1;
  GetStruct()->on_touch_move2 = ark_web_nweb_on_touch_move2;
  GetStruct()->on_touch_cancel = ark_web_nweb_on_touch_cancel;
  GetStruct()->on_navigate_back = ark_web_nweb_on_navigate_back;
  GetStruct()->send_key_event = ark_web_nweb_send_key_event;
  GetStruct()->send_mouse_wheel_event = ark_web_nweb_send_mouse_wheel_event;
  GetStruct()->send_mouse_event = ark_web_nweb_send_mouse_event;
  GetStruct()->load1 = ark_web_nweb_load1;
  GetStruct()->is_navigatebackward_allowed =
      ark_web_nweb_is_navigatebackward_allowed;
  GetStruct()->is_navigate_forward_allowed =
      ark_web_nweb_is_navigate_forward_allowed;
  GetStruct()->can_navigate_back_or_forward =
      ark_web_nweb_can_navigate_back_or_forward;
  GetStruct()->navigate_back = ark_web_nweb_navigate_back;
  GetStruct()->navigate_forward = ark_web_nweb_navigate_forward;
  GetStruct()->navigate_back_or_forward = ark_web_nweb_navigate_back_or_forward;
  GetStruct()->delete_navigate_history = ark_web_nweb_delete_navigate_history;
  GetStruct()->reload = ark_web_nweb_reload;
  GetStruct()->zoom = ark_web_nweb_zoom;
  GetStruct()->zoom_in = ark_web_nweb_zoom_in;
  GetStruct()->zoom_out = ark_web_nweb_zoom_out;
  GetStruct()->stop = ark_web_nweb_stop;
  GetStruct()->execute_java_script1 = ark_web_nweb_execute_java_script1;
  GetStruct()->execute_java_script2 = ark_web_nweb_execute_java_script2;
  GetStruct()->get_preference = ark_web_nweb_get_preference;
  GetStruct()->get_web_id = ark_web_nweb_get_web_id;
  GetStruct()->get_hit_test_result = ark_web_nweb_get_hit_test_result;
  GetStruct()->put_background_color = ark_web_nweb_put_background_color;
  GetStruct()->initial_scale = ark_web_nweb_initial_scale;
  GetStruct()->put_download_callback = ark_web_nweb_put_download_callback;
  GetStruct()->put_accessibility_event_callback =
      ark_web_nweb_put_accessibility_event_callback;
  GetStruct()->put_accessibility_id_generator =
      ark_web_nweb_put_accessibility_id_generator;
  GetStruct()->set_nweb_handler = ark_web_nweb_set_nweb_handler;
  GetStruct()->title = ark_web_nweb_title;
  GetStruct()->page_load_progress = ark_web_nweb_page_load_progress;
  GetStruct()->content_height = ark_web_nweb_content_height;
  GetStruct()->scale = ark_web_nweb_scale;
  GetStruct()->load2 = ark_web_nweb_load2;
  GetStruct()->load_with_data_and_base_url =
      ark_web_nweb_load_with_data_and_base_url;
  GetStruct()->load_with_data = ark_web_nweb_load_with_data;
  GetStruct()->register_ark_jsfunction1 = ark_web_nweb_register_ark_jsfunction1;
  GetStruct()->unregister_ark_jsfunction =
      ark_web_nweb_unregister_ark_jsfunction;
  GetStruct()->set_nweb_java_script_result_call_back =
      ark_web_nweb_set_nweb_java_script_result_call_back;
  GetStruct()->put_find_callback = ark_web_nweb_put_find_callback;
  GetStruct()->find_all_async = ark_web_nweb_find_all_async;
  GetStruct()->clear_matches = ark_web_nweb_clear_matches;
  GetStruct()->find_next = ark_web_nweb_find_next;
  GetStruct()->store_web_archive = ark_web_nweb_store_web_archive;
  GetStruct()->create_web_message_ports = ark_web_nweb_create_web_message_ports;
  GetStruct()->post_web_message = ark_web_nweb_post_web_message;
  GetStruct()->close_port = ark_web_nweb_close_port;
  GetStruct()->post_port_message = ark_web_nweb_post_port_message;
  GetStruct()->set_port_message_callback =
      ark_web_nweb_set_port_message_callback;
  GetStruct()->send_drag_event = ark_web_nweb_send_drag_event;
  GetStruct()->clear_ssl_cache = ark_web_nweb_clear_ssl_cache;
  GetStruct()->get_url = ark_web_nweb_get_url;
  GetStruct()->clear_client_authentication_cache =
      ark_web_nweb_clear_client_authentication_cache;
  GetStruct()->update_locale = ark_web_nweb_update_locale;
  GetStruct()->get_original_url = ark_web_nweb_get_original_url;
  GetStruct()->get_favicon = ark_web_nweb_get_favicon;
  GetStruct()->put_network_available = ark_web_nweb_put_network_available;
  GetStruct()->has_images = ark_web_nweb_has_images;
  GetStruct()->remove_cache = ark_web_nweb_remove_cache;
  GetStruct()->get_history_list = ark_web_nweb_get_history_list;
  GetStruct()->put_release_surface_callback =
      ark_web_nweb_put_release_surface_callback;
  GetStruct()->serialize_web_state = ark_web_nweb_serialize_web_state;
  GetStruct()->restore_web_state = ark_web_nweb_restore_web_state;
  GetStruct()->page_up = ark_web_nweb_page_up;
  GetStruct()->page_down = ark_web_nweb_page_down;
  GetStruct()->scroll_to = ark_web_nweb_scroll_to;
  GetStruct()->scroll_by = ark_web_nweb_scroll_by;
  GetStruct()->slide_scroll = ark_web_nweb_slide_scroll;
  GetStruct()->get_cert_chain_der_data = ark_web_nweb_get_cert_chain_der_data;
  GetStruct()->set_screen_off_set = ark_web_nweb_set_screen_off_set;
  GetStruct()->set_audio_muted = ark_web_nweb_set_audio_muted;
  GetStruct()->set_should_frame_submission_before_draw =
      ark_web_nweb_set_should_frame_submission_before_draw;
  GetStruct()->notify_popup_window_result =
      ark_web_nweb_notify_popup_window_result;
  GetStruct()->set_audio_resume_interval =
      ark_web_nweb_set_audio_resume_interval;
  GetStruct()->set_audio_exclusive = ark_web_nweb_set_audio_exclusive;
  GetStruct()->register_screen_lock_function =
      ark_web_nweb_register_screen_lock_function;
  GetStruct()->un_register_screen_lock_function =
      ark_web_nweb_un_register_screen_lock_function;
  GetStruct()->notify_memory_level = ark_web_nweb_notify_memory_level;
  GetStruct()->on_webview_hide = ark_web_nweb_on_webview_hide;
  GetStruct()->on_webview_show = ark_web_nweb_on_webview_show;
  GetStruct()->get_or_create_drag_data = ark_web_nweb_get_or_create_drag_data;
  GetStruct()->prefetch_page = ark_web_nweb_prefetch_page;
  GetStruct()->set_window_id = ark_web_nweb_set_window_id;
  GetStruct()->on_occluded = ark_web_nweb_on_occluded;
  GetStruct()->on_unoccluded = ark_web_nweb_on_unoccluded;
  GetStruct()->set_token = ark_web_nweb_set_token;
  GetStruct()->set_nested_scroll_mode = ark_web_nweb_set_nested_scroll_mode;
  GetStruct()->set_enable_lower_frame_rate =
      ark_web_nweb_set_enable_lower_frame_rate;
  GetStruct()->set_virtual_key_board_arg =
      ark_web_nweb_set_virtual_key_board_arg;
  GetStruct()->should_virtual_keyboard_overlay =
      ark_web_nweb_should_virtual_keyboard_overlay;
  GetStruct()->set_draw_rect = ark_web_nweb_set_draw_rect;
  GetStruct()->set_draw_mode = ark_web_nweb_set_draw_mode;
  GetStruct()->create_web_print_document_adapter =
      ark_web_nweb_create_web_print_document_adapter;
  GetStruct()->post_url = ark_web_nweb_post_url;
  GetStruct()->java_script_on_document_start =
      ark_web_nweb_java_script_on_document_start;
  GetStruct()->execute_action = ark_web_nweb_execute_action;
  GetStruct()->get_focused_accessibility_node_info =
      ark_web_nweb_get_focused_accessibility_node_info;
  GetStruct()->get_accessibility_node_info_by_id =
      ark_web_nweb_get_accessibility_node_info_by_id;
  GetStruct()->get_accessibility_node_info_by_focus_move =
      ark_web_nweb_get_accessibility_node_info_by_focus_move;
  GetStruct()->set_accessibility_state = ark_web_nweb_set_accessibility_state;
  GetStruct()->need_soft_keyboard = ark_web_nweb_need_soft_keyboard;
  GetStruct()->discard = ark_web_nweb_discard;
  GetStruct()->restore = ark_web_nweb_restore;
  GetStruct()->get_security_level = ark_web_nweb_get_security_level;
  GetStruct()->call_h5function = ark_web_nweb_call_h5function;
  GetStruct()->is_incognito_mode = ark_web_nweb_is_incognito_mode;
  GetStruct()->register_native_ark_jsfunction =
      ark_web_nweb_register_native_ark_jsfunction;
  GetStruct()->un_register_native_ark_jsfunction =
      ark_web_nweb_un_register_native_ark_jsfunction;
  GetStruct()->register_native_valide_callback =
      ark_web_nweb_register_native_valide_callback;
  GetStruct()->register_native_destroy_callback =
      ark_web_nweb_register_native_destroy_callback;
  GetStruct()->java_script_on_document_end =
      ark_web_nweb_java_script_on_document_end;
  GetStruct()->enable_safe_browsing = ark_web_nweb_enable_safe_browsing;
  GetStruct()->is_safe_browsing_enabled = ark_web_nweb_is_safe_browsing_enabled;
  GetStruct()->set_print_background = ark_web_nweb_set_print_background;
  GetStruct()->get_print_background = ark_web_nweb_get_print_background;
  GetStruct()->close_all_media_presentations =
      ark_web_nweb_close_all_media_presentations;
  GetStruct()->stop_all_media = ark_web_nweb_stop_all_media;
  GetStruct()->resume_all_media = ark_web_nweb_resume_all_media;
  GetStruct()->pause_all_media = ark_web_nweb_pause_all_media;
  GetStruct()->get_media_playback_state = ark_web_nweb_get_media_playback_state;
  GetStruct()->enable_intelligent_tracking_prevention =
      ark_web_nweb_enable_intelligent_tracking_prevention;
  GetStruct()->is_intelligent_tracking_prevention_enabled =
      ark_web_nweb_is_intelligent_tracking_prevention_enabled;
  GetStruct()->start_camera = ark_web_nweb_start_camera;
  GetStruct()->stop_camera = ark_web_nweb_stop_camera;
  GetStruct()->close_camera = ark_web_nweb_close_camera;
  GetStruct()->get_last_javascript_proxy_calling_frame_url =
      ark_web_nweb_get_last_javascript_proxy_calling_frame_url;
  GetStruct()->get_pending_size_status = ark_web_nweb_get_pending_size_status;
  GetStruct()->scroll_by_ref_screen = ark_web_nweb_scroll_by_ref_screen;
  GetStruct()->execute_java_script_ext = ark_web_nweb_execute_java_script_ext;
  GetStruct()->on_render_to_background = ark_web_nweb_on_render_to_background;
  GetStruct()->on_render_to_foreground = ark_web_nweb_on_render_to_foreground;
  GetStruct()->precompile_java_script = ark_web_nweb_precompile_java_script;
  GetStruct()->on_create_native_media_player =
      ark_web_nweb_on_create_native_media_player;
  GetStruct()->ark_web_nweb_drag_resize = ark_web_nweb_drag_resize;
  GetStruct()->on_touch_cancel_by_id = ark_web_nweb_on_touch_cancel_by_id;
  GetStruct()->scale_gesture_change = ark_web_nweb_scale_gesture_change;
  GetStruct()->inject_offline_resource = ark_web_nweb_inject_offline_resource;
  GetStruct()->terminate_render_process = ark_web_nweb_terminate_render_process;
  GetStruct()->suggestion_selected = ark_web_nweb_suggestion_selected;
  GetStruct()->send_touchpad_fling_event = ark_web_nweb_send_touchpad_fling_event;
  GetStruct()->register_ark_jsfunction2 = ark_web_nweb_register_ark_jsfunction2;
  GetStruct()->set_fit_content_mode = ark_web_nweb_set_fit_content_mode;
  GetStruct()->get_select_info = ark_web_nweb_get_select_info;
  GetStruct()->on_safe_insets_change = ark_web_nweb_on_safe_insets_change;
  GetStruct()->on_online_render_to_foreground = ark_web_nweb_on_online_render_to_foreground;
}

ArkWebNWebCppToC::~ArkWebNWebCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<ArkWebNWebCppToC, ArkWebNWeb,
                                        ark_web_nweb_t>::kBridgeType =
    ARK_WEB_NWEB;

} // namespace OHOS::ArkWeb
