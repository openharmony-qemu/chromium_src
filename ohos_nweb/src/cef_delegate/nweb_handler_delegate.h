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

#include "cef/include/base/cef_lock.h"
#include "cef/include/cef_client.h"
#include "cef/include/cef_dialog_handler.h"
#include "cef/include/cef_form_handler.h"
#include "cef/include/cef_frame_handler.h"
#include "cef/include/cef_jsdialog_handler.h"
#include "cef/include/cef_media_handler.h"
#include "cef/include/cef_permission_request.h"
#include "cef/include/cef_resource_request_handler.h"

#include "nweb_event_handler.h"
#include "nweb_handler.h"
#include "nweb_preference_delegate.h"
#include "nweb_render_handler.h"

#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <string>
#include <unordered_set>
#include <map>
#include "capi/nweb_app_client_extension_callback.h"
#include "nweb_download_callback.h"
#include "nweb_javascript_result_callback.h"
#include "nweb_value.h"

#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
#include "custom_media_player_impl.h"
#endif // OHOS_CUSTOM_VIDEO_PLAYER

#if defined(OHOS_SOFTWARE_COMPOSITOR)
#include "base/cancelable_callback.h"
#endif

struct NativeWindow;

namespace OHOS::NWeb {
class NWebHandler;
class NWebGeolocationCallback;
class NWebFindDelegate;

class NWebHandlerDelegate : public CefClient,
                            public CefLifeSpanHandler,
                            public CefLoadHandler,
                            public CefRequestHandler,
                            public CefResourceRequestHandler,
                            public CefDisplayHandler,
                            public CefDownloadHandler,
                            public CefFocusHandler,
                            public CefPermissionRequest,
                            public CefJSDialogHandler,
                            public CefDialogHandler,
                            public CefContextMenuHandler,
                            public CefFindHandler,
                            public CefKeyboardHandler,
                            public CefMediaHandler,
                            public CefFormHandler,
                            public CefFrameHandler,
#if defined(OHOS_PRINT)
                            public CefCookieAccessFilter,
                            public CefPrintHandler {
#else
                            public CefCookieAccessFilter {
#endif  // defined(OHOS_PRINT)
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
  void RegisterWebAppClientExtensionListener(
      std::shared_ptr<NWebAppClientExtensionCallback>
          web_app_client_extension_listener);
  void RegisterNWebJavaScriptCallBack(
      std::shared_ptr<NWebJavaScriptResultCallBack> callback);

  void RegisterNativeJavaScriptCallBack(
      const char* objName,
      const std::vector<std::shared_ptr<NWebJsProxyCallback>>& callbacks);

  void SetInputMethodClient(CefRefPtr<NWebInputMethodClient> client);

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
  void RegisterNativeLoadStartCallback(std::function<void(void)>&& callback);
  void RegisterNativeLoadEndCallback(std::function<void(void)>&& callback);
  int GetFlowbufCount(void* mem);
  char* FlowbufStrAtIndex(void* mem, int flowbufIndex, int* argIndex, int* strLen);
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

#if defined(OHOS_NWEB_EX)
  void UnRegisterWebAppClientExtensionListener();
#endif  // defined(OHOS_NWEB_EX)

  // #if defined(OHOS_EX_PASSWORD)
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
  const CefRefPtr<CefBrowser> GetBrowser();
  bool OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text) override;

  /* CefClient methods begin */
  CefRefPtr<CefDownloadHandler> GetDownloadHandler() override;
  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
  CefRefPtr<CefLoadHandler> GetLoadHandler() override;
  CefRefPtr<CefRenderHandler> GetRenderHandler() override;
  CefRefPtr<CefRequestHandler> GetRequestHandler() override;
  CefRefPtr<CefDisplayHandler> GetDisplayHandler() override;
  CefRefPtr<CefFocusHandler> GetFocusHandler() override;
  CefRefPtr<CefPermissionRequest> GetPermissionRequest() override;
  CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override;
  CefRefPtr<CefDialogHandler> GetDialogHandler() override;
  CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override;
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
  void GetJavaScriptObjectMethods(
      int32_t object_id,
      CefRefPtr<CefValue> returned_method_names) override;
  void RemoveJavaScriptObjectHolder(int32_t holder, int32_t object_id) override;
  void RemoveTransientJavaScriptObject() override;

  CefRefPtr<CefFindHandler> GetFindHandler() override;
  CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override;
  CefRefPtr<CefFormHandler> GetFormHandler() override;
#if defined(OHOS_MEDIA_MUTE_AUDIO)
  CefRefPtr<CefMediaHandler> GetMediaHandler() override;
#endif  // defined(OHOS_MEDIA_MUTE_AUDIO)

#if defined(OHOS_PRINT)
  CefRefPtr<CefPrintHandler> GetPrintHandler() override;
#endif  // defined(OHOS_PRINT)

  CefRefPtr<CefFrameHandler> GetFrameHandler() override;
  /* CefClient methods end */

  /* CefLifeSpanHandler methods begin */
  void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  bool DoClose(CefRefPtr<CefBrowser> browser) override;
  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;
  bool OnBeforePopup(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
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
                                bool isReload) override;

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

  void OnNavigationEntryCommitted(
      CefRefPtr<CefLoadCommittedDetails> details) override;

  void OnSafeBrowsingCheckResult(int threat_type) override;
  /* CefLoadHandler methods end */

  /* CefRequestHandler methods begin */
  bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                      CefRefPtr<CefFrame> frame,
                      CefRefPtr<CefRequest> request,
                      bool user_gesture,
                      bool is_redirect) override;
  bool OnCertificateError(CefRefPtr<CefBrowser> browser,
                          cef_errorcode_t cert_error,
                          const CefString& request_url,
                          CefRefPtr<CefSSLInfo> ssl_info,
                          CefRefPtr<CefCallback> callback) override;

  bool OnSelectClientCertificate(
      CefRefPtr<CefBrowser> browser,
      bool isProxy,
      const CefString& host,
      int port,
      const std::vector<CefString>& key_types,
      const std::vector<CefString>& principals,
      const X509CertificateList& certificates,
      CefRefPtr<CefSelectClientCertificateCallback> callback) override;

  CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request,
      bool is_navigation,
      bool is_download,
      const CefString& request_initiator,
      bool& disable_default_handling) override;

  void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
                                 TerminationStatus status) override;

  bool GetAuthCredentials(CefRefPtr<CefBrowser> browser,
                          const CefString& origin_url,
                          bool isProxy,
                          const CefString& host,
                          int port,
                          const CefString& realm,
                          const CefString& scheme,
                          CefRefPtr<CefAuthCallback> callback) override;

  bool ShouldOverrideUrlLoading(CefRefPtr<CefBrowser> browser,
                                const CefString& url,
                                const CefString& method,
                                bool user_gesture,
                                bool is_redirect,
                                bool is_outermost_main_frame) override;
  bool OnOpenAppLink(const CefString& url,
                     CefRefPtr<CefOpenAppLinkCallback> callback) override;
  /* CefRequestHandler methods end */

  /* CefDownloadHandler methods begin */
  void OnBeforeDownload(CefRefPtr<CefBrowser> browser,
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
#if defined(OHOS_INPUT_EVENTS)
  void KeyboardReDispatch(const CefKeyEvent& event,  bool isUsed) override;
  void OnTakeFocus(CefRefPtr<CefBrowser> browser,  bool next) override;
  bool IsCurrentFocus();
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

  void GetResourceHandlerByIO(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request,
      CefRefPtr<CefInterceptCallback> callback,
      CefRefPtr<CefSchemeHandlerFactory> scheme_factory,
      const CefString& scheme) override;
  /* CefResourceRequestHandler method end */

  /* CefMediaHandler methods begin */
#if defined(OHOS_MEDIA_MUTE_AUDIO)
  void OnAudioStateChanged(CefRefPtr<CefBrowser> browser,
                           bool audible) override;
#endif  // defined(OHOS_MEDIA_MUTE_AUDIO)

  void OnMediaStateChanged(CefRefPtr<CefBrowser> browser,
                           MediaType type,
                           MediaPlayingState state) override;
  /* CefFindandler methods end */

  /* CefDisplayHandler method begin */
  void OnTitleChange(CefRefPtr<CefBrowser> browser,
                     const CefString& title) override;
  void OnLoadingProgressChange(CefRefPtr<CefBrowser> browser,
                               double progress) override;
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
  void OnReceivedTouchIconUrl(CefRefPtr<CefBrowser> browser,
                              const CefString& icon_url,
                              bool precomposed) override;
  bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                        cef_log_severity_t level,
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
#if defined(OHOS_INPUT_EVENTS)
  bool OnCursorChange(CefRefPtr<CefBrowser> browser,
                      CefCursorHandle cursor,
                      cef_cursor_type_t type,
                      const CefCursorInfo& custom_cursor_info) override;
  bool GetContinueNeedFocus();
  void SetContinueNeedFocus(bool continueNeedFocus);
#endif  // defined(OHOS_INPUT_EVENTS)
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
                            const CefString& url,
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
                    bool capture,
                    CefRefPtr<CefFileDialogCallback> callback) override;

#ifdef OHOS_HTML_SELECT
  void OnSelectPopupMenu(CefRefPtr<CefBrowser> browser,
                         const CefRect& bounds,
                         int item_height,
                         double item_font_size,
                         int selected_item,
                         const std::vector<CefSelectPopupItem>& menu_items,
                         bool right_aligned,
                         bool allow_multiple_selection,
                         CefRefPtr<CefSelectPopupCallback> callback) override;
#endif  // #ifdef OHOS_HTML_SELECT
#ifdef OHOS_CSS_INPUT_TIME
  void OnDateTimeChooserPopup(
      CefRefPtr<CefBrowser> browser,
      const CefDateTimeChooser& date_time_chooser,
      const std::vector<CefDateTimeSuggestion>& suggestion,
      CefRefPtr<CefDateTimeChooserCallback> callback) override;
  void OnDateTimeChooserClose() override;
#endif  // #ifdef OHOS_CSS_INPUT_TIME
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
                                CefRefPtr<CefImage> image) override;
  void OnGetImageFromCache(CefRefPtr<CefImage> image) override;
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

  bool UpdateClippedSelectionBounds(
      CefRefPtr<CefBrowser> browser,
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
  void ChangeVisibilityOfQuickMenu() override;
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
#if defined(OHOS_PRINT)
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
#endif  // defined(OHOS_PRINT)
  /* CefPrintHandler method end */

  /* CefFrameHandler method begin */
  void OnMainFrameChanged(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> old_frame,
                          CefRefPtr<CefFrame> new_frame) override;
  /* CefFrameHandler method end */

  const std::vector<std::string> GetVisitedHistory();

  void SetNWebId(uint32_t nwebId);
  uint32_t GetNWebId();

  void SetWindowId(uint32_t window_id) { window_id_ = window_id; }

#ifdef OHOS_BFCACHE
  void UpdateFavicon(CefRefPtr<CefBrowser> browser) override;
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
#ifdef OHOS_FOCUS
  bool GetFocusState();
  void SetFocusState(bool focusState);
#endif  // #ifdef OHOS_FOCUS
#ifdef OHOS_PAGE_UP_DOWN
  float GetScale() const { return scale_; }
#endif  // #ifdef OHOS_PAGE_UP_DOWN

#if defined(OHOS_MULTI_WINDOW)
  void NotifyPopupWindowResult(bool result);
#endif  // defined(OHOS_MULTI_WINDOW)

#ifdef OHOS_ARKWEB_ADBLOCK
  void OnAdsBlocked(CefRefPtr<CefBrowser> browser,
                    const CefString& url,
                    const std::map<CefString, CefString>& adsBlocked,
                    bool is_site_first_report) override;
#endif

  // #if defined(OHOS_EX_TOPCONTROLS)
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

#ifdef OHOS_ARKWEB_ADBLOCK
  void SaveGlobalAdsBlock(bool enable);

  bool TrigAdBlockEnabledForSiteFromUi(CefRefPtr<CefBrowser> browser,
                                       const CefString& url,
                                       int main_frame_tree_node_id) override;
#endif

#ifdef OHOS_DRAG_DROP
  bool IsDragEnter() const { return is_drag_enter_; }
  void SetDragEnter(bool enter) { is_drag_enter_ = enter; }
#endif  // #ifdef OHOS_DRAG_DROP

#ifdef OHOS_ITP
  void OnIntelligentTrackingPreventionResult(
      const CefString& website_host, const CefString& tracker_host) override;
#endif

#ifdef OHOS_NETWORK_LOAD
  bool OnAllCertificateError(CefRefPtr<CefBrowser> browser,
                             cef_errorcode_t cert_error,
                             const CefString& request_url,
                             const CefString& origin_url,
                             const CefString& referrer,
                             bool is_main_frame_request,
                             bool is_fatal_error,
                             CefRefPtr<CefSSLInfo> ssl_info,
                             CefRefPtr<CefCallback> callback) override;
#endif

#if defined(OHOS_SCREEN_LOCK)
  void SetWakeLockCallback(int32_t windowId, const std::shared_ptr<NWebScreenLockCallback>& callback);
#endif

#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
  void RegisterOnCreateNativeMediaPlayerListener(
      std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback);

  CefOwnPtr<CefCustomMediaPlayerDelegate> OnCreateCustomMediaPlayer(
      CefOwnPtr<CefMediaPlayerListener> listener,
      const CefCustomMediaInfo& media_info) override;
#endif // OHOS_CUSTOM_VIDEO_PLAYER

#if defined(OHOS_CLIPBOARD)
  void SetIsRichText(bool is_rich_text) { is_rich_text_ = is_rich_text; }
#endif

#if defined(OHOS_RENDERER_ANR_DUMP)
  void OnRenderProcessNotResponding(CefRefPtr<CefBrowser> browser,
                                    const CefString& js_stack,
                                    int pid,
                                    int reason) override;
  void OnRenderProcessResponding(CefRefPtr<CefBrowser> browser) override;
#endif

#ifdef OHOS_DISPLAY_CUTOUT
  void OnViewportFitChange(CefRefPtr<CefBrowser> browser,
                           int viewport_fit) override;
#endif

#if defined(OHOS_SOFTWARE_COMPOSITOR)
  bool IsWebPaintedForSnapshot () { return isWebPaintedForSnapshot_; }
  void SetWebPaintedForSnapshot() { isWebPaintedForSnapshot_ = true; }
#endif

 void SetPopupSurface(void* popup_window);
 void SetTransformHint(uint32_t rotation);

  void OnRequestOpenDevTools();

 private:
  void CopyImageToClipboard(CefRefPtr<CefImage> image);
  // List of existing browser windows. Only accessed on the CEF UI thread.
  typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
  BrowserList browser_list_;

  CefRefPtr<CefBrowser> main_browser_ = nullptr;
  bool is_closing_ = false;

  std::shared_ptr<NWebPreferenceDelegate> preference_delegate_ = nullptr;
  CefRefPtr<NWebRenderHandler> render_handler_ = nullptr;

  std::shared_ptr<NWebEventHandler> event_handler_ = nullptr;

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(NWebHandlerDelegate);

  std::shared_ptr<NWebDownloadCallback> download_listener_ = nullptr;
  std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener_ = nullptr;
  std::shared_ptr<NWebHandler> nweb_handler_ = nullptr;
  std::shared_ptr<NWebJavaScriptResultCallBack> nweb_javascript_callback_ =
      nullptr;
  std::shared_ptr<NWebFindDelegate> find_delegate_ = nullptr;
  std::shared_ptr<NWebAppClientExtensionCallback>
      web_app_client_extension_listener_ = nullptr;
  CefRefPtr<NWebInputMethodClient> input_method_client_ = nullptr;
  std::shared_ptr<NWebGeolocationCallback> callback_ = nullptr;
#if defined(OHOS_CUSTOM_VIDEO_PLAYER)
  std::shared_ptr<NWebCreateNativeMediaPlayerCallback>
      create_native_media_player_cb_ = nullptr;
#endif // OHOS_CUSTOM_VIDEO_PLAYER

  bool is_enhance_surface_ = false;
  void* window_ = nullptr;
  void* popup_window_ = nullptr;

#if defined(OHOS_SCREEN_LOCK)
  std::shared_ptr<NWebScreenLockCallback> screen_lock_callback_ = nullptr;
  int32_t screen_lock_window_id_ = -1;
#endif

  CefString image_cache_src_url_;

  // the received icon
  base::Lock state_lock_;
  const void* data_ = nullptr;
  size_t width_ = 0;
  size_t height_ = 0;
  ImageColorType color_type_ = ImageColorType::COLOR_TYPE_UNKNOWN;
  ImageAlphaType alpha_type_ = ImageAlphaType::ALPHA_TYPE_UNKNOWN;

  uint32_t nweb_id_ = 0;
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

#if defined(OHOS_NWEB_EX)
  bool on_load_start_notified_ = false;
  float top_content_offset_ = 0;
#endif  // OHOS_NWEB_EX
#ifdef OHOS_PAGE_UP_DOWN
  float scale_ = 100.0;
#endif  // #ifdef OHOS_PAGE_UP_DOWN
#if defined(OHOS_INPUT_EVENTS)
  bool continueNeedFocus_ = false;
#endif  // defined(OHOS_INPUT_EVENTS)
#if BUILDFLAG(IS_OHOS)
  std::vector<uint64_t> edited_forms_id_;
#endif
#ifdef OHOS_DRAG_DROP
  bool is_drag_enter_ = false;
#endif  // #ifdef OHOS_DRAG_DROP
#if defined(OHOS_CLIPBOARD)
  bool is_rich_text_ = false;
#endif
#if defined(OHOS_SOFTWARE_COMPOSITOR)
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

#ifdef OHOS_ARKWEB_ADBLOCK
      bool is_global_adblock_enabled_ = false;
#endif

  base::WeakPtrFactory<NWebHandlerDelegate> weak_factory_{this};
};
}  // namespace OHOS::NWeb

#endif
