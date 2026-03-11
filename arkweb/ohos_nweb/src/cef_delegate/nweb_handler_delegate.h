/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_HANDLER_DELEGATE_H
#define NWEB_HANDLER_DELEGATE_H

#include <condition_variable>
#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_set>

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#include "capi/nweb_app_client_extension_callback.h"
#include "capi/nweb_extension_api_callback.h"
#include "capi/nweb_extension_callback.h"
#include "cef/include/base/cef_lock.h"
#include "cef/include/cef_dialog_handler.h"
#include "cef/include/cef_frame_handler.h"
#include "cef/include/cef_jsdialog_handler.h"
#include "cef/include/cef_resource_request_handler.h"
#include "cef/ohos_cef_ext/include/arkweb_dialog_handler_ext.h"
#include "cef/ohos_cef_ext/include/arkweb_display_handler_ext.h"
#include "include/arkweb_client_ext.h"
#include "include/arkweb_dialog_handler_ext.h"
#include "include/arkweb_load_handler_ext.h"
#include "include/arkweb_render_handler_ext.h"
#include "include/arkweb_request_ext.h"
#include "include/arkweb_resource_request_handler_ext.h"
#include "include/cef_form_handler.h"
#include "include/cef_media_handler.h"
#include "include/cef_permission_request.h"
#include "nweb_download_callback.h"
#include "nweb_event_handler.h"
#include "nweb_handler.h"
#include "nweb_javascript_result_callback.h"
#include "nweb_preference_delegate.h"
#include "nweb_render_handler.h"
#include "nweb_value.h"

#include "ohos_nweb/src/video_assistant/nweb_media_player_controller_impl.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
#include "capi/nweb_logger_callback.h"
#include "cef/include/cef_logger_callback_api_handler.h"
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
#include "custom_media_player_impl.h"
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
#include "base/cancelable_callback.h"
#endif

#if BUILDFLAG(ARKWEB_NAVIGATION)
#include "capi/nweb_icon_size.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_RECEIVE_RESPONSE)
#include "ohos_nweb/src/capi/nweb_resource_request.h"
#include "ohos_nweb/src/capi/nweb_resource_response.h"
#include "ohos_nweb/src/capi/nweb_resource_request_response.h"
#endif

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/extension/nweb_app_client_extension_dispatcher.h"
#include "ohos_nweb_ex/public/capi/nweb_static_types.h"
#endif

struct NativeWindow;

namespace OHOS::NWeb {
class NWebHandler;
class NWebGeolocationCallback;
class NWebFindDelegate;

class NWebHandlerDelegate : public ArkWebClientExt,
                            public CefLifeSpanHandler,
                            public ArkWebLoadHandlerExt,
                            public CefRequestHandlerExt,
                            public ArkWebResourceRequestHandlerExt,
                            public ArkWebDisplayHandlerExt,
                            public CefDownloadHandler,
                            public CefFocusHandler,
                            public CefPermissionRequest,
                            public CefJSDialogHandler,
                            public CefContextMenuHandlerExt,
                            public CefFindHandler,
                            public CefKeyboardHandler,
                            public CefMediaHandler,
                            public CefFormHandler,
                            public CefFrameHandler,
                            public CefDialogHandlerExt,
#if BUILDFLAG(ARKWEB_SAME_LAYER)
                            public CefWebClientExtensionHandler,
#endif
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
                            public CefLoggerCallbackApiHandler,
#endif  // defined(ARKWEB_LOGGER_REPORT)
#if BUILDFLAG(ARKWEB_PRINT)
                            public CefCookieAccessFilter,
                            public CefPrintHandler {
#else
                            public CefCookieAccessFilter {
#endif  // BUILDFLAG(ARKWEB_PRINT)
 public:
  static CefRefPtr<NWebHandlerDelegate> Create(
      std::shared_ptr<NWebPreferenceDelegate> preference_delegate,
      CefRefPtr<NWebRenderHandler> render_handler,
      std::shared_ptr<NWebEventHandler> event_handler,
      std::shared_ptr<NWebFindDelegate> find_delegate,
      bool is_enhance_surface,
      void* window);

  NWebHandlerDelegate(
      std::shared_ptr<NWebPreferenceDelegate> preference_delegate,
      CefRefPtr<NWebRenderHandler> render_handler,
      std::shared_ptr<NWebEventHandler> event_handler,
      std::shared_ptr<NWebFindDelegate> find_delegate,
      bool is_enhance_surface,
      void* window);
  ~NWebHandlerDelegate() = default;

  void OnDestroy();

  void RegisterDownLoadListener(
      std::shared_ptr<NWebDownloadCallback> download_listener);
  void RegisterReleaseSurfaceListener(
      std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener);
  void RegisterNWebHandler(std::shared_ptr<NWebHandler> handler);
#if BUILDFLAG(ARKWEB_NWEB_EX)
  void RegisterArkWebAppClientExtensionListener(
      std::shared_ptr<ArkWebAppClientExtensionCallback> callback);
#endif
  void RegisterWebAppClientExtensionListener(
      std::shared_ptr<NWebAppClientExtensionCallback>
          web_app_client_extension_listener);
  void RegisterNWebJavaScriptCallBack(
      std::shared_ptr<NWebJavaScriptResultCallBack> callback);

  void RegisterNativeJavaScriptCallBack(
      const char* objName,
      const std::vector<std::shared_ptr<NWebJsProxyCallback>>& callbacks);
#if BUILDFLAG(ARKWEB_AI)
  void RegisterNWebAgentHandler(std::shared_ptr<NWebAgentHandler> handler);
  void RegisterOnLoadStartedCbForHighlightContent(std::function<void(void)>&& callback);
#endif

  using NativeJSProxyCallbackFunc =
      std::function<char*(std::vector<std::vector<uint8_t>>&,
                          std::vector<size_t>&)>;
  using NativeJSProxyCallbackFuncWithResult =
      std::function<std::shared_ptr<OHOS::NWeb::NWebValue>(
          std::vector<std::vector<uint8_t>>&,
          std::vector<size_t>&)>;
  void RegisterNativeJavaScriptCallBack(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<NativeJSProxyCallbackFunc>&& callback,
      bool isAsync,
      const std::string& permission);
  void RegisterNativeJavaScriptCallBackWithResult(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<NativeJSProxyCallbackFuncWithResult>&& callback,
      bool isAsync,
      const std::string& permission);
  void RegisterNativeAsyncThreadJavaScriptCallBackWithResult(
      const std::string& objName,
      const std::vector<std::string>& methodName,
      std::vector<NativeJSProxyCallbackFuncWithResult>&& callback,
      const std::string& permission);
  void RegisterNativeLoadStartCallback(std::function<void(void)>&& callback);
  void RegisterNativeLoadEndCallback(std::function<void(void)>&& callback);
#if BUILDFLAG(ARKWEB_AI)
  void RegisterOnLoadStartedCbForContentChange(std::function<void(void)>&& callback);
#endif
  int GetFlowbufCount(void* mem);
  char* FlowbufStrAtIndex(void* mem,
                          int flowbufIndex,
                          int* argIndex,
                          int* strLen);
  int ProcessNativeProxyResultNew(CefRefPtr<CefListValue> args,
                                  const CefString& method,
                                  const CefString& object_name,
                                  CefRefPtr<CefListValue> result);
  int ProcessNativeProxyResultNewForReturnValue(CefRefPtr<CefListValue> args,
                                                const CefString& method,
                                                const CefString& object_name,
                                                CefRefPtr<CefListValue> result);
  int ProcessNativeProxyResultNewFlowbuf(CefRefPtr<CefListValue> args,
                                         const CefString& method,
                                         const CefString& object_name,
                                         int fd,
                                         CefRefPtr<CefListValue> result);

  int ProcessNativeProxyResultThread(CefRefPtr<CefListValue> args,
                                     const CefString& method,
                                     const CefString& object_name,
                                     CefRefPtr<CefListValue> result);

  int ProcessNativeProxyResult(CefRefPtr<CefListValue> args,
                               const CefString& method,
                               const CefString& object_name,
                               CefRefPtr<CefListValue> result);

  int ProcessNativeProxyResultFlowbuf(CefRefPtr<CefListValue> args,
                                      const CefString& method,
                                      const CefString& object_name,
                                      int fd,
                                      CefRefPtr<CefListValue> result);

#if BUILDFLAG(ARKWEB_NWEB_EX)
  void UnRegisterArkWebAppClientExtensionListener();
  void UnRegisterWebAppClientExtensionListener();
  void RegisterWebExtensionListener(
      std::shared_ptr<NWebExtensionCallback> web_extension_listener);
  void UnRegisterWebExtensionListener();
#endif  // BUILDFLAG(ARKWEB_NWEB_EX)

  bool OnOpenURLFromTab(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        const CefString& target_url,
                        WindowOpenDisposition target_disposition,
                        bool user_gesture) override;

  // #if BUILDFLAG(ARKWEB_EXT_PASSWORD)
  void ShowPasswordDialog(bool is_update, const CefString& url) override;
  void OnShowAutofillPopup(CefRefPtr<CefBrowser> browser,
                           const CefRect& bounds,
                           bool right_aligned,
                           const std::vector<CefAutofillPopupItem>& menu_items,
                           bool is_password_popup_type) override;
  void OnHideAutofillPopup() override;
  // #endif

  // Request that all existing browser windows close.
  void CloseAllBrowsers(bool force_close);
  bool IsClosing() const;
  const CefRefPtr<ArkWebBrowserExt> GetBrowser();
  bool OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text) override;

  /* CefClient methods begin */
  CefRefPtr<CefDownloadHandler> GetDownloadHandler() override;
  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
  CefRefPtr<ArkWebLoadHandlerExt> GetLoadHandler() override;
  CefRefPtr<ArkWebRenderHandlerExt> GetRenderHandler() override;
  CefRefPtr<CefRequestHandler> GetRequestHandler() override;
  CefRefPtr<ArkWebDisplayHandlerExt> GetDisplayHandler() override;
  CefRefPtr<CefFocusHandler> GetFocusHandler() override;
  CefRefPtr<CefPermissionRequest> GetPermissionRequest() override;
  CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override;
  CefRefPtr<CefDialogHandlerExt> GetDialogHandler() override;
  CefRefPtr<CefContextMenuHandlerExt> GetContextMenuHandler() override;
  CefRefPtr<CefCookieAccessFilter> GetCookieAccessFilter(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request) override;
  virtual bool OnProcessMessageReceived(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefProcessId source_process,
      CefRefPtr<CefProcessMessage> message) override;
  int NotifyJavaScriptResult(CefRefPtr<CefListValue> args,
                             const CefString& method,
                             const CefString& object_name,
                             CefRefPtr<CefListValue> result,
                             int32_t routing_id,
                             int32_t object_id) override;
  int NotifyJavaScriptResultFlowbuf(CefRefPtr<CefListValue> args,
                                    const CefString& method,
                                    const CefString& object_name,
                                    int fd,
                                    CefRefPtr<CefListValue> result,
                                    int32_t routing_id,
                                    int32_t object_id) override;
  bool HasJavaScriptObjectMethods(int32_t object_id,
                                  const CefString& method_name) override;
  bool HasNativeAsyncThreadJavaScriptMethods(std::string& object_name,
                                  const std::string& method_name) override;
  void GetJavaScriptObjectMethods(
      int32_t object_id,
      CefRefPtr<CefValue> returned_method_names) override;
  void RemoveJavaScriptObjectHolder(int32_t holder, int32_t object_id) override;
  void RemoveTransientJavaScriptObject() override;

  CefRefPtr<CefFindHandler> GetFindHandler() override;
  CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override;
  CefRefPtr<CefFormHandler> GetFormHandler() override;
#if BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
  CefRefPtr<CefMediaHandler> GetMediaHandler() override;
#endif  // BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)

#if BUILDFLAG(ARKWEB_PRINT)
  CefRefPtr<CefPrintHandler> GetPrintHandler() override;
#endif  // BUILDFLAG(ARKWEB_PRINT)
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  CefRefPtr<CefWebClientExtensionHandler> GetWebClientExtensionHandler()
      override;
#endif
  CefRefPtr<CefFrameHandler> GetFrameHandler() override;

  /* CefClient methods end */
  CefRefPtr<ArkWebClientExt> AsArkWebClient() override { return this; }
  /* CefLifeSpanHandler methods begin */
  void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  bool DoClose(CefRefPtr<CefBrowser> browser) override;
  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;
  bool OnBeforePopup(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      int popup_id,
      const CefString& target_url,
      const CefString& target_frame_name,
      CefLifeSpanHandler::WindowOpenDisposition target_disposition,
      bool user_gesture,
      const CefPopupFeatures& popup_features,
      CefWindowInfo& window_info,
      CefRefPtr<CefClient>& client,
      CefBrowserSettings& settings,
      CefRefPtr<CefDictionaryValue>& extra_info,
      bool* no_javascript_access) override;
  bool OnPreBeforePopup(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      const CefString& target_url,
      CefLifeSpanHandler::WindowOpenDisposition target_disposition,
      bool user_gesture,
      const CefRect& window_features,
      CefRefPtr<CefCallback> callback) override;
  /* CefLifeSpanHandler methods end */

  /* CefLoadHandler methods begin */
  void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                            bool is_loading,
                            bool can_go_back,
                            bool can_go_forward) override;

  void OnLoadStart(CefRefPtr<CefBrowser> browser,
                   CefRefPtr<CefFrame> frame,
                   const CefString& url,
                   TransitionType transition_type) override;

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int http_status_code) override;

  void OnLoadError(CefRefPtr<CefBrowser> browser,
                   CefRefPtr<CefFrame> frame,
                   ErrorCode error_code,
                   const CefString& error_text,
                   const CefString& failed_url) override;

  void OnLoadErrorWithRequest(CefRefPtr<CefRequest> request,
                              CefRefPtr<CefFrame> frame,
                              bool is_main_frame,
                              bool has_user_gesture,
                              int error_code,
                              const CefString& error_text) override;

  void OnHttpError(CefRefPtr<CefRequest> request,
                   bool is_main_frame,
                   bool has_user_gesture,
                   CefRefPtr<CefResponse> response) override;

  void OnRefreshAccessedHistory(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                const CefString& url,
                                bool isReload,
                                bool isMainFrame) override;

  void OnPageVisible(CefRefPtr<CefBrowser> browser,
                     const CefString& url,
                     bool success) override;

  void OnFirstContentfulPaint(int64_t navigationStartTick,
                              int64_t firstContentfulPaintMs) override;

  void OnFirstMeaningfulPaint(
      CefRefPtr<CefFirstMeaningfulPaintDetails> details) override;

  void OnLargestContentfulPaint(
      CefRefPtr<CefLargestContentfulPaintDetails> details) override;

  void OnDataResubmission(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefCallback> callback) override;

  void OnSafeBrowsingCheckResult(int threat_type) override;

  void OnNavigationEntryCommitted(
      CefRefPtr<CefLoadCommittedDetails> details) override;

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  void OnLoadStarted(CefRefPtr<CefFrame> frame,
                     const CefString& url) override;

  void OnLoadFinished(CefRefPtr<CefFrame> frame,
                      const CefString& url) override;
#endif
  /* CefLoadHandler methods end */

  /* CefRequestHandler methods begin */
  bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                      CefRefPtr<CefFrame> frame,
                      CefRefPtr<CefRequest> request,
                      bool user_gesture,
                      bool is_redirect) override;
  bool OnCertificateErrorExt(CefRefPtr<CefBrowser> browser,
                             cef_errorcode_t cert_error,
                             const CefString& request_url,
                             CefRefPtr<CefSSLInfo> ssl_info,
                             CefRefPtr<ArkWebCefSslCallback> callback) override;
  void OnReportNewNavigationInfo(
      CefRefPtr<CefWebNavigationInfo> navigation_info) override;

  bool OnSelectClientCertificate(
      CefRefPtr<CefBrowser> browser,
      bool isProxy,
      const CefString& host,
      int port,
      const std::vector<CefString>& key_types,
      const std::vector<CefString>& principals,
      const X509CertificateList& certificates,
      CefRefPtr<CefSelectClientCertificateCallback> callback) override;

  bool OnVerifyPin(
      const std::string& identity,
      CefRefPtr<CefVerifyPinCallback> callback) override;

  CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request,
      bool is_navigation,
      bool is_download,
      const CefString& request_initiator,
      bool& disable_default_handling) override;

  void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
                                 TerminationStatus status,
                                 int error_code,
                                 const CefString& error_string) override;

  bool GetAuthCredentials(CefRefPtr<CefBrowser> browser,
                          const CefString& origin_url,
                          bool isProxy,
                          const CefString& host,
                          int port,
                          const CefString& realm,
                          const CefString& scheme,
                          CefRefPtr<CefAuthCallback> callback) override;

  bool ShouldOverrideUrlLoading(
      CefRefPtr<CefBrowser> browser,
      const CefString& url,
      const CefString& method,
      bool user_gesture,
      bool is_redirect,
      bool is_outermost_main_frame,
      const CefString& extra_request_headers_str) override;
  bool OnOpenAppLink(const CefString& url,
                     CefRefPtr<CefOpenAppLinkCallback> callback) override;
  /* CefRequestHandler methods end */

  /* CefDownloadHandler methods begin */
  bool OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefDownloadItem> download_item,
                        const CefString& suggested_name,
                        CefRefPtr<CefBeforeDownloadCallback> callback) override;
  /* CefDownloadHandler methods end */

  /* CefKeyboardHandler methods begin */
  bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                     const CefKeyEvent& event,
                     CefEventHandle os_event,
                     bool* is_keyboard_shortcut) override;

  bool OnKeyEvent(CefRefPtr<CefBrowser> browser,
                  const CefKeyEvent& event,
                  CefEventHandle os_event) override;
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void KeyboardReDispatch(const CefKeyEvent& event, bool isUsed) override;
  void OnTakeFocus(CefRefPtr<CefBrowser> browser, bool next) override;
  bool IsCurrentFocus();
  void SetImeShow(bool visible) override;
#endif
  /* CefKeyboardHandler methods begin */

  /* CefResourceRequestHandler method begin */
  ReturnValue OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefRequest> request,
                                   CefRefPtr<CefCallback> callback) override;

  CefRefPtr<CefResourceHandler> GetResourceHandler(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request) override;

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  CefRefPtr<ArkWebResourceRequestHandlerExt> AsArkWebResourceRequestHandlerExt()
      override {
    return this;
  }
#endif

  void GetResourceHandlerByIO(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefRequest> request,
                              CefRefPtr<CefInterceptCallback> callback,
                              CefRefPtr<CefSchemeHandlerFactory> scheme_factory,
                              const CefString& scheme) override;
  /* CefResourceRequestHandler method end */

  /* CefMediaHandler methods begin */
#if BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)
  void OnAudioStateChanged(CefRefPtr<CefBrowser> browser,
                           bool audible) override;
#endif  // BUILDFLAG(ARKWEB_MEDIA_MUTE_AUDIO)

  void OnMediaStateChanged(CefRefPtr<CefBrowser> browser,
                           MediaType type,
                           MediaPlayingState state) override;
  /* CefFindandler methods end */

  /* CefDisplayHandler method begin */
  void OnTitleChange(CefRefPtr<CefBrowser> browser,
                     const CefString& title,
                     bool isRealTitle) override;
  void OnLoadingProgressChange(CefRefPtr<CefBrowser> browser,
                               double progress) override;
#if BUILDFLAG(ARKWEB_NWEB_EX)
  bool OnAutoResize(CefRefPtr<CefBrowser> browser,
                    const CefSize& new_size) override;
#endif
  void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser,
                              bool full_screen,
                              const CefSize& video_natural_size) override;
  void OnReceivedIcon(const void* data,
                      size_t width,
                      size_t height,
                      cef_color_type_t color_type,
                      cef_alpha_type_t alpha_type) override;
  void OnReceivedIconUrl(const CefString& image_url,
                         const void* data,
                         size_t width,
                         size_t height,
                         cef_color_type_t color_type,
                         cef_alpha_type_t alpha_type) override;
#if BUILDFLAG(ARKWEB_NAVIGATION)
  void OnTouchIconUrlWithSizesReceived(const CefString &image_url,
                                       bool precomposed,
                                       const std::vector<IconSize> &sizes) override;
#endif
  void OnReceivedTouchIconUrl(CefRefPtr<CefBrowser> browser,
                              const CefString& icon_url,
                              bool precomposed) override;
  bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                        cef_log_severity_t level,
                        int sourceType,
                        const CefString& message,
                        const CefString& source,
                        int line) override;
  void OnScaleChanged(CefRefPtr<CefBrowser> browser,
                      float old_page_scale_factor,
                      float new_page_scale_factor) override;
  void OnScaleInited(CefRefPtr<CefBrowser> browser,
                     float page_scale_factor) override;
  void OnContentsBrowserZoomChange(double zoom_factor,
                                   bool can_show_bubble) override;
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool OnCursorChange(CefRefPtr<CefBrowser> browser,
                      CefCursorHandle cursor,
                      cef_cursor_type_t type,
                      const CefCursorInfo& custom_cursor_info) override;
  bool GetContinueNeedFocus();
  void SetContinueNeedFocus(bool continueNeedFocus);
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
  /* CefDisplayHandler method end */

  /* CefFocusHandler method begin */
  bool OnSetFocus(CefRefPtr<CefBrowser> browser, FocusSource source) override;
  /* CefFocusHandler method end */

  /* CefFormHandler method begin */
  void OnFormEditingStateChanged(CefRefPtr<CefBrowser> browser,
                                 bool is_editing,
                                 uint64_t form_id) override;
  /* CefFormHandler method end */

  /* CefPermissionRequest method begin */
  void OnGeolocationShow(const CefString& origin) override;

  void OnGeolocationHide() override;

  void OnPermissionRequest(CefRefPtr<CefAccessRequest> request) override;
  void OnPermissionRequestCanceled(
      CefRefPtr<CefAccessRequest> request) override;

#if BUILDFLAG(ARKWEB_EXT_PERMISSION)
  void PermissionRequestGrant(int32_t resourse_id, int nweb_request_key);
  void PermissionRequestDeny(int nweb_request_key);
  std::string PermissionRequestGetOrigin(int nweb_request_key);
  int32_t PermissionRequestGetResourceId(int nweb_request_key);
  void PermissionRequestDelete(int nweb_request_key);
#endif  // ARKWEB_EXT_PERMISSION

  void OnScreenCaptureRequest(
      CefRefPtr<CefScreenCaptureAccessRequest> request) override;
  /* CefPermissionRequest method end */

  /* CefJSDialogHandler method begin */
  bool OnJSDialog(CefRefPtr<CefBrowser> browser,
                  const CefString& origin_url,
                  JSDialogType dialog_type,
                  const CefString& message_text,
                  const CefString& default_prompt_text,
                  CefRefPtr<CefJSDialogCallback> callback,
                  bool& suppress_message) override;

  bool OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,
                            const CefString& message_text,
                            bool is_reload,
                            CefRefPtr<CefJSDialogCallback> callback) override;
  /* CefJSDialogHandler method end */

  /* CefDialogHandler method begin */
  bool OnFileDialog(CefRefPtr<CefBrowser> browser,
                    FileDialogMode mode,
                    const CefString& title,
                    const CefString& default_file_path,
                    const std::vector<CefString>& accept_filters,
                    const std::vector<CefString>& accept_extensions,
                    const std::vector<CefString>& accept_descriptions,
                    const CefString& accepts,
                    const CefString& start_in,
                    bool is_exclude_accept_all_options,
                    bool capture,
                    const std::vector<CefString>& mime_filters,
                    CefRefPtr<CefFileDialogCallback> callback) override;

#if BUILDFLAG(ARKWEB_HTML_SELECT)
  void OnSelectPopupMenu(CefRefPtr<CefBrowser> browser,
                         const CefRect& bounds,
                         int item_height,
                         double item_font_size,
                         int selected_item,
                         const std::vector<CefSelectPopupItem>& menu_items,
                         bool right_aligned,
                         bool allow_multiple_selection,
                         CefRefPtr<CefSelectPopupCallback> callback) override;
#endif  // #if BUILDFLAG(ARKWEB_HTML_SELECT)
#if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
  void OnDateTimeChooserPopup(
      CefRefPtr<CefBrowser> browser,
      const CefDateTimeChooser& date_time_chooser,
      const std::vector<CefDateTimeSuggestion>& suggestion,
      CefRefPtr<CefDateTimeChooserCallback> callback) override;
  void OnDateTimeChooserClose() override;
#endif  // #if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
  /* CefDialogHandler method end */

  /* CefContextMenuHandler method begin */
  void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           CefRefPtr<CefContextMenuParams> params,
                           CefRefPtr<CefMenuModel> model) override;
  bool RunContextMenu(CefRefPtr<CefBrowser> browser,
                      CefRefPtr<CefFrame> frame,
                      CefRefPtr<CefContextMenuParams> params,
                      CefRefPtr<CefMenuModel> model,
                      CefRefPtr<CefRunContextMenuCallback> callback) override;
  void OnGetImageForContextNode(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefImage> image,
                                int command_id) override;
  void OnGetImageFromCache(CefRefPtr<CefImage> image, int command_id) override;
  bool OnContextMenuCommand(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefContextMenuParams> params,
      int command_id,
      CefContextMenuHandler::EventFlags event_flags) override;
  void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame) override;
  bool RunQuickMenu(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      const CefPoint& location,
      const CefSize& size,
      const CefRect& select_bounds,
      CefContextMenuHandler::QuickMenuEditStateFlags edit_state_flags,
      CefRefPtr<CefRunQuickMenuCallback> callback,
      bool is_mouse_trigger,
      bool is_long_press_actived) override;

  bool UpdateClippedSelectionBounds(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    const CefRect& select_bounds) override;

  bool OnQuickMenuCommand(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      int command_id,
      CefContextMenuHandler::EventFlags event_flags) override;
  void OnQuickMenuDismissed(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            bool is_mouse_trigger) override;
  void HideHandleAndQuickMenuIfNecessary(bool hide) override;
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  void ChangeVisibilityOfQuickMenu() override;
  bool IsQuickMenuShow() override;
#endif
#if BUILDFLAG(ARKWEB_AI)
  bool CloseImageOverlaySelection() override;
  void OnAgentEventReport(const std::string& json) override;
#endif
  /* CefContextMenuHandler method end */

  /* CefFindandler methods begin */
  void OnFindResult(CefRefPtr<CefBrowser> browser,
                    int identifier,
                    int count,
                    const CefRect& selectionRect,
                    int activeMatchOrdinal,
                    bool finalUpdate) override;
  /* CefFindandler methods end */

  /* CefResourceRequestHandler methods begin */
  bool CanSendCookie(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     CefRefPtr<CefRequest> request,
                     const CefCookie& cookie) override;
  bool CanSaveCookie(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     CefRefPtr<CefRequest> request,
                     CefRefPtr<CefResponse> response,
                     const CefCookie& cookie) override;
  /* CefResourceRequestHandler methods end */

  /* CefPrintHandler method begin */
#if BUILDFLAG(ARKWEB_PRINT)
  void OnPrintStart(CefRefPtr<CefBrowser> browser) override;
  void OnPrintSettings(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefPrintSettings> settings,
                       bool get_defaults) override;
  bool OnPrintDialog(CefRefPtr<CefBrowser> browser,
                     bool has_selection,
                     CefRefPtr<CefPrintDialogCallback> callback) override;
  bool OnPrintJob(CefRefPtr<CefBrowser> browser,
                  const CefString& document_name,
                  const CefString& pdf_file_path,
                  CefRefPtr<CefPrintJobCallback> callback) override;
  void OnPrintReset(CefRefPtr<CefBrowser> browser) override;
  CefSize GetPdfPaperSize(CefRefPtr<CefBrowser> browser,
                          int device_units_per_inch) override;
#endif  // BUILDFLAG(ARKWEB_PRINT)
  /* CefPrintHandler method end */

  /* CefFrameHandler methods begin */
  void OnMainFrameChanged(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> old_frame,
                          CefRefPtr<CefFrame> new_frame) override;
#if BUILDFLAG(ARKWEB_NWEB_EX)
  void OnFrameCreated(CefRefPtr<CefBrowser> browser,
                      CefRefPtr<CefFrame> frame) override;
  void OnFrameDetached(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame) override;
#endif
  /* CefFrameHandler methods end */

  const std::vector<std::string> GetVisitedHistory();

  void SetNWebId(uint32_t nwebId);
  uint32_t GetNWebId();

  void SetWindowId(uint32_t window_id) { window_id_ = window_id; }
  uint32_t GetWindowId() override { return window_id_; }

#if BUILDFLAG(ARKWEB_BFCACHE)
  void UpdateFavicon(CefRefPtr<CefBrowser> browser) override;
  void SetMediaResumeFromBFCachePage(bool resume);
#endif

  void SetFavicon(const void* icon_data,
                  size_t width,
                  size_t height,
                  ImageColorType color_type,
                  ImageAlphaType alpha_type);
  bool GetFavicon(const void** data,
                  size_t& width,
                  size_t& height,
                  ImageColorType& colorType,
                  ImageAlphaType& alphaType);
#if BUILDFLAG(ARKWEB_FOCUS)
  bool GetFocusState();
  void SetFocusState(bool focusState);
#endif  // #if BUILDFLAG(ARKWEB_FOCUS)
#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
  float GetScale() const { return scale_; }
#endif  // #if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  void NotifyPopupWindowResult(bool result);
#endif  // BUILDFLAG(ARKWEB_MULTI_WINDOW)

#if BUILDFLAG(ARKWEB_ADBLOCK)
  void OnAdsBlocked(CefRefPtr<CefBrowser> browser,
                    const CefString& url,
                    const std::map<CefString, CefString>& adsBlocked,
                    bool is_site_first_report) override;

  void SaveGlobalAdsBlock(bool enable);

  bool TrigAdBlockEnabledForSiteFromUi(CefRefPtr<CefBrowser> browser,
                                       const CefString& url,
                                       int main_frame_tree_node_id) override;

#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  void OnShareFile(const std::string& file_path,
                   const std::string& utd_type_id) override;
#endif

#if BUILDFLAG(ARKWEB_EXT_PERMISSION)
  static std::shared_ptr<NWebPermissionRequest> GetPermissionRequestByKey(
      int key);
  static int InsertPermissionRequest(
      std::shared_ptr<NWebPermissionRequest> nweb_request);
#endif  // ARKWEB_EXT_PERMISSION

  // #if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  void OnTopControlsChanged(float top_controls_offset,
                            float top_content_offset) override;
  int OnGetTopControlsHeight() override;
  bool DoBrowserControlsShrinkRendererSize() override;
  // #endif

  // save ark js function for window.open
  void SavaArkJSFunctionForPopup(
      const std::string& object_name,
      const std::vector<std::string>& method_list,
      const std::vector<std::string>& async_method_list,
      const int32_t object_id,
      const std::string& permission);

#if BUILDFLAG(IS_ARKWEB)
  void SaveEnableAppLinking(bool enable);
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  bool IsDragEnter() const { return is_drag_enter_; }
  void SetDragEnter(bool enter) { is_drag_enter_ = enter; }
#endif  // #if BUILDFLAG(ARKWEB_DRAG_DROP)

#if BUILDFLAG(ARKWEB_ITP)
  void OnIntelligentTrackingPreventionResult(
      const CefString& website_host,
      const CefString& tracker_host) override;
#endif

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
  void SetWakeLockCallback(
      int32_t windowId,
      const std::shared_ptr<NWebScreenLockCallback>& callback);
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  bool OnAllCertificateError(CefRefPtr<CefBrowser> browser,
                             cef_errorcode_t cert_error,
                             const CefString& request_url,
                             const CefString& origin_url,
                             const CefString& referrer,
                             bool is_main_frame_request,
                             bool is_fatal_error,
                             CefRefPtr<CefSSLInfo> ssl_info,
                             CefRefPtr<ArkWebCefSslCallback> callback) override;
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  void RegisterOnCreateNativeMediaPlayerListener(
      std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback);

  CefOwnPtr<CefCustomMediaPlayerDelegate> OnCreateCustomMediaPlayer(
      CefOwnPtr<CefMediaPlayerListener> listener,
      const CefCustomMediaInfo& media_info) override;
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

  void OnShowToast(double duration, const CefString& toast) override;
  void OnShowVideoAssistant(const CefString& videoAssistantItems) override;
  void OnReportStatisticLog(const CefString& content) override;

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  CefOwnPtr<CefMediaPlayerListenerForVAST> OnFullScreenOverlayEnter(
      CefOwnPtr<CefMediaPlayerController> media_player_controller,
      const std::string& extra_info) override;
  CefOwnPtr<CefMediaPlayerListenerForVAST> OnAVCastStarted(
      CefOwnPtr<CefMediaPlayerController> media_player_controller,
      const std::string& extra_info) override;
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MENU)
  void SetIsRichText(bool is_rich_text) { is_rich_text_ = is_rich_text; }
#endif

#if BUILDFLAG(ARKWEB_RENDERER_ANR_DUMP)
  void OnRenderProcessNotResponding(CefRefPtr<CefBrowser> browser,
                                    const CefString& js_stack,
                                    int pid,
                                    int reason) override;
  void OnRenderProcessResponding(CefRefPtr<CefBrowser> browser) override;
#endif

#if BUILDFLAG(ARKWEB_NWEB_EX)
  void OnUpdateTargetURL(CefRefPtr<CefBrowser> browser,
                         const CefString& url) override;
#endif

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  void OnViewportFitChange(CefRefPtr<CefBrowser> browser,
                           int viewportFit) override;
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  bool IsWebPaintedForSnapshot() { return isWebPaintedForSnapshot_; }
  void SetWebPaintedForSnapshot() { isWebPaintedForSnapshot_ = true; }
#endif

#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
  void SetTransformHint(uint32_t rotation);
#endif

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  void OnActivateContent() override;
#endif

#if BUILDFLAG(ARKWEB_EXT_PULL_TO_REFRESH)
  bool OnPullToRefreshAction(int action) override;
  void OnPullToRefreshPull(float offset_x, float offset_y) override;
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  void SetPopupSurface(void* popup_window);
#endif
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  static void RegisterLoggerCallback(
      std::shared_ptr<NWebLoggerCallback> logger_callback);
  static void UnRegisterLoggerCallback();

  // CefLoggerCallbackApiHandler implements
  void logFeedback(const CefString& tag,
                   int level,
                   const CefString& message) override;
  void logUrl(const CefString& url) override;
#endif

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
  void OnBeforeUnloadFired(CefRefPtr<CefBrowser> browser,
                           bool proceed) override;
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  // CefWebClientExtensionHandler implements
  void OnNativeEmbedFirstFramePaint(
      CefRefPtr<CefNativeEmbedFirstFramePaintEvent> request) override;
#endif
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void EnableVideoAssistant(bool enable);
  void EnableVideoAssistantAVCast(bool enable);
  void CustomWebMediaPlayer(bool enable);
  void WebMediaPlayerControllerPlay();
  void WebMediaPlayerControllerPause();
  void WebMediaPlayerControllerSeek(double time);
  void WebMediaPlayerControllerSetMuted(bool muted);
  void WebMediaPlayerControllerSetPlaybackRate(double playback_rate);
  void WebMediaPlayerControllerExitFullscreen();
  void WebMediaPlayerControllerSetVideoSurface(void* native_window);
  void WebMediaPlayerControllerDownload();
  void WebMediaPlayerControllerSetVolume(double volume);
  double WebMediaPlayerControllerGetVolume();
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
  void RegisterScreenCaptureDelegateListener(CefRefPtr<CefScreenCaptureCallback> screen_capture_cb);
#endif // ARKWEB_EX_SCREEN_CAPTURE

#if BUILDFLAG(ARKWEB_DEVTOOLS)
  void OnRequestOpenDevTools(const RequestOpenDevToolsParams& params) override;
#endif // ARKWEB_DEVTOOLS

  void Discard();

  void HandleSafeBrowsingDetection(int detectMode,
                                   int detectSwitch,
                                   const CefString& url) override;

#if BUILDFLAG(ARKWEB_PIP)
  bool OnPip(CefRefPtr<CefBrowser> browser,
             int status,
             int delegate_id,
             int child_id,
             int frame_routing_id,
             int width,
             int height) override;
  void OnPipEvent(CefRefPtr<CefBrowser> browser,
                  int event) override;
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
  bool OnStartBackgroundTask(int32_t type, const std::string& message) override;
#endif  // ARKWEB_PERFORMANCE_PERSISTENT_TASK

#if BUILDFLAG(ARKWEB_READER_MODE)
  void OnIsPageDistillable(int page_type,
                          const std::string& distillable_page_url, const std::string& title) override;
  void OnDidMeaningfulLayout(const std::string& url) override;
#endif

#if BUILDFLAG(ARKWEB_PDF)
  void OnPdfScrollAtBottom(const std::string& url) override;
  void OnPdfLoadEvent(int32_t result, const std::string& url) override;
#endif

#if BUILDFLAG(ARKWEB_MENU)
  void OnVisibleChanged(bool isVisible);
  void SetHandleVisibleCallback(
      const base::RepeatingCallback<void(bool)>& on_handle_visible) override {
    this->on_handle_visible_ = on_handle_visible;
  }
  void ShowMagnifier() override;
  void HideMagnifier() override;
  bool IsShowHandle() override;
#endif

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
  std::string OverrideErrorPage(
    CefRefPtr<CefBrowser> browser,
    const CefString& url,
    const CefString& method,
    bool user_gesture,
    bool is_redirect,
    bool is_outermost_main_frame,
    const CefString& extra_request_headers_str,
    int error_code,
    const CefString& error_text) override;
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void SetBlanklessLoadingKey(uint64_t blankless_key);
  void ClearSnapshot();
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  std::string OnRewriteUrlForNavigation(const std::string& original_url,
                                        const std::string& referrer,
                                        int transition_type,
                                        bool is_key_request) override;
  void OnRewriteUrlForNavigationAsync(
      const CefString& original_url,
      const CefString& referrer,
      int transition_type,
      bool is_key_request,
      CefRefPtr<CefRewriteUrlCallback> callback) override;
#endif

#if BUILDFLAG(ARKWEB_WEBRTC)
  void OnCameraCaptureStateChanged(int original_state, int new_state) override;
#endif

#if BUILDFLAG(ARKWEB_WEBRTC)
  void OnMicrophoneCaptureStateChanged(int original_state, int new_state) override;
#endif

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
  void OnDocumentEndReady(const CefString& id, const CefString& parent_id) override;
#endif

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
void OnMediaCastEnter() override;
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  void OnSafeBrowsingCheckDetail(int code, int policy, int threat) override;
#endif

#if BUILDFLAG(ARKWEB_EXT_RECEIVE_RESPONSE)
  void OnReceiveResponse(CefRefPtr<CefRequest> request,
                         bool is_request_gesture,
                         int transition_type,
                         bool is_main_frame,
                         bool is_redirect,
                         int resource_type,
                         CefRefPtr<CefResponse> response_info,
                         bool is_from_network) override;
 
  std::map<std::string, std::string> ResourceRequestGetRequestHeader(int nweb_request_key);
  std::string ResourceRequestGetRequestUrl(int nweb_request_key);
  bool ResourceRequestIsRequestGesture(int nweb_request_key);
  bool ResourceRequestIsMainFrame(int nweb_request_key);
  bool ResourceRequestIsRedirect(int nweb_request_key);
  std::string ResourceRequestGetRequestMethod(int nweb_request_key);
  int32_t ResourceRequestGetPageTransition(int nweb_request_key);
  int32_t ResourceRequestGetRequestType(int nweb_request_key);
 
  std::string ResourceResponseGetMimeType(int nweb_response_key);
  std::string ResourceResponseGetEncoding(int nweb_response_key);
  int32_t ResourceResponseGetStatusCode(int nweb_response_key);
  std::string ResourceResponseGetReasonPhrase(int nweb_response_key);
  std::map<std::string, std::string> ResourceResponseGetResponseHeader(int nweb_response_key);
  bool ResourceResponseGetIsFromNetwork(int nweb_response_key);
 
  int32_t GetLastCommittedEntryPageTransition();
 
  void ResourceRequestDelete(int nweb_request_key);
  void ResourceResponseDelete(int nweb_response_key);
  
  static int InsertResourceRequest(std::shared_ptr<NWebResourceRequest> nweb_request);
  static std::shared_ptr<NWebResourceRequest> GetResourceRequestByKey(int key);
  
  static int InsertResourceResponse(std::shared_ptr<NWebResourceResponse> nweb_response);
  static std::shared_ptr<NWebResourceResponse> GetResourceResponseByKey(int key);
#endif

 private:
#if BUILDFLAG(ARKWEB_JSPROXY)
  enum class JsRunTime{Start = 0, End = 1, HEAD_READY};
  void InjectJsToWeb(JsRunTime time);
  void InjectJsToWebInner(JsRunTime time,
                          ScriptItems& scriptItems,
                          ScriptRegexItems& scriptRegexItems,
                          ScriptItemsByOrder& scriptItemsByOrder);
#endif
  void CopyImageToClipboard(CefRefPtr<CefImage> image);

  // List of existing browser windows. Only accessed on the CEF UI thread.
  typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
  BrowserList browser_list_;

  CefRefPtr<CefBrowser> main_browser_ = nullptr;
  bool is_closing_ = false;

#if BUILDFLAG(IS_ARKWEB)
  bool is_arkweb_applinking_enabled_ = true;
#endif

  std::shared_ptr<NWebPreferenceDelegate> preference_delegate_ = nullptr;
  CefRefPtr<NWebRenderHandler> render_handler_ = nullptr;

  std::shared_ptr<NWebEventHandler> event_handler_ = nullptr;

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(NWebHandlerDelegate);

  std::shared_ptr<NWebDownloadCallback> download_listener_ = nullptr;
  std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener_ = nullptr;
  std::shared_ptr<NWebHandler> nweb_handler_ = nullptr;
#if BUILDFLAG(ARKWEB_AI)
  std::shared_ptr<NWebAgentHandler> nweb_agent_handler_ = nullptr;
  std::function<void(void)> onLoadStartedCbForHighlightContent_ = nullptr;
#endif
  std::shared_ptr<NWebJavaScriptResultCallBack> nweb_javascript_callback_ =
      nullptr;
  std::shared_ptr<NWebFindDelegate> find_delegate_ = nullptr;
  std::shared_ptr<NWebAppClientExtensionCallback>
      web_app_client_extension_listener_ = nullptr;
#if BUILDFLAG(ARKWEB_NWEB_EX)
  void OnGetImageData(CefRefPtr<CefImage> image);
  void OnGetImageFromCacheEx(int command_id,
                             uint8_t* data,
                             int buffer_size) override;
  void OnGetImageForShare(uint8_t* data, int buffer_size);
  std::shared_ptr<NWebExtensionCallback> web_extension_listener_ = nullptr;
#endif  // #if BUILDFLAG(ARKWEB_NWEB_EX)
  std::shared_ptr<NWebGeolocationCallback> callback_ = nullptr;
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  std::shared_ptr<NWebCreateNativeMediaPlayerCallback>
      create_native_media_player_cb_ = nullptr;
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

  bool is_enhance_surface_ = false;
  raw_ptr<void> window_ = nullptr;

#if BUILDFLAG(ARKWEB_PERFORMANCE_JITTER)
  raw_ptr<void> popup_window_ = nullptr;
#endif

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
  std::shared_ptr<NWebScreenLockCallback> screen_lock_callback_ = nullptr;
  int32_t screen_lock_window_id_ = -1;
#endif

  CefString image_cache_src_url_;

  // the received icon
  base::Lock state_lock_;
  raw_ptr<const void> data_ = nullptr;
  size_t width_ = 0;
  size_t height_ = 0;
  ImageColorType color_type_ = ImageColorType::COLOR_TYPE_UNKNOWN;
  ImageAlphaType alpha_type_ = ImageAlphaType::ALPHA_TYPE_UNKNOWN;

  uint32_t nweb_id_ = 0;
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  // To avoid include blankless_controller.h in nweb_handler_delegate.h,
  // we use UINT64_MAX instead of INVALID_BLANKLESS_KEY.
  uint64_t blankless_key_ = UINT64_MAX;
#endif

#if BUILDFLAG(ARKWEB_PIP)
  int pip_status_ = -1;
  int pip_delegate_id_ = 0;
  int pip_child_id_ = 0;
  int pip_frame_routing_id_ = 0;
#endif

#if defined(REPORT_SYS_EVENT)
  // For page load statistics
  uint32_t access_sum_count_ = 0;
  uint32_t access_success_count_ = 0;
  uint32_t access_fail_count_ = 0;
#endif
  uint32_t window_id_ = 0;
  bool focusState_ = false;
  static int32_t popIndex_;
  CefRefPtr<CefCallback> popupWindowCallback_ = nullptr;

#if BUILDFLAG(ARKWEB_NWEB_EX)
  bool on_load_start_notified_ = false;
  float top_content_offset_ = 0;
#endif  // ARKWEB_NWEB_EX
#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
  float scale_ = 100.0;
#endif  // #if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool continueNeedFocus_ = false;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
  std::vector<uint64_t> edited_forms_id_;
#endif
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  bool is_drag_enter_ = false;
#endif  // #if BUILDFLAG(ARKWEB_DRAG_DROP)
#if BUILDFLAG(ARKWEB_MENU)
  bool is_rich_text_ = false;
#endif
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  bool isWebPaintedForSnapshot_ = false;
  base::CancelableOnceClosure setWebPaintedTask_;
#endif
  // js property name and object id
  std::unordered_map<
      std::string,
      std::unordered_map<std::string,
                         std::function<char*(const char** argv, int32_t argc)>>>
      objMap_;
  std::unordered_map<std::string,
                     std::unordered_map<std::string, NativeJSProxyCallbackFunc>>
      syncProxyObjMap_;
  std::unordered_map<
      std::string,
      std::unordered_map<std::string, NativeJSProxyCallbackFuncWithResult>>
      syncProxyObjWithResultMap_;
  std::unordered_map<std::string,
                     std::unordered_map<std::string, NativeJSProxyCallbackFunc>>
      asyncProxyObjMap_;
  std::unordered_map<
      std::string,
      std::unordered_map<std::string, NativeJSProxyCallbackFuncWithResult>>
      asyncProxyObjWithResultMap_;
  std::unordered_map<
      std::string,
      std::unordered_set<std::string>>
      asyncThreadProxyObjWithResultMap_;
  std::unordered_map<std::string, std::string> asyncProxyPermissionMap_;
  std::unordered_map<std::string, std::string> syncProxyPermissionMap_;
  using MethodPair = std::pair<std::string, std::unordered_set<std::string>>;
  using PermissionMap = std::map<int32_t, std::string>;
  using ObjectMethodMap = std::map<int32_t, MethodPair>;
  ObjectMethodMap javascript_sync_method_map_;
  ObjectMethodMap javascript_async_method_map_;
  PermissionMap javascript_sync_permission_map_;
  PermissionMap javascript_async_permission_map_;
  std::function<void(void)> onLoadStartCallback_ = nullptr;
  std::function<void(void)> onLoadEndCallback_ = nullptr;
#if BUILDFLAG(ARKWEB_AI)
  std::function<void(void)> onLoadStartedCbForContentChange_ = nullptr;
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
  bool is_global_adblock_enabled_ = false;
#endif

#if BUILDFLAG(ARKWEB_BFCACHE)
  bool media_resume_from_bfcache_page_ = true;
#endif // BUILDFLAG(ARKWEB_BFCACHE)

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  std::optional<bool> video_assistant_enabled_;
  std::optional<bool> video_assistant_avcast_enabled_;
  std::optional<bool> custom_web_media_player_enabled_;
  std::unique_ptr<NWebMediaPlayerControllerImpl> nweb_media_player_controller_;
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_NWEB_EX)
  NWebAppClientExtensionDispatcher dispatcher_;
#endif

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
  CefRefPtr<CefSafeBrowsingDetectionCallback>
      safe_browsing_detection_callback_ = nullptr;
#endif

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
  CefRefPtr<CefScreenCaptureCallback> screen_capture_cb_ = nullptr;
#endif // ARKWEB_EX_SCREEN_CAPTURE

  base::WeakPtr<NWebHandlerDelegate> weak_this_;
  base::WeakPtrFactory<NWebHandlerDelegate> weak_factory_{this};

#if BUILDFLAG(ARKWEB_MENU)
  base::RepeatingCallback<void(bool)> on_handle_visible_;
#endif
};
}  // namespace OHOS::NWeb

#endif
